/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "usb/vcp_usb_manager.hpp"

using namespace esp_usb;

// Change these values to match your needs
#define EXAMPLE_BAUDRATE (115200)
#define EXAMPLE_STOP_BITS (0) // 0: 1 stopbit, 1: 1.5 stopbits, 2: 2 stopbits
#define EXAMPLE_PARITY (0)    // 0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
#define EXAMPLE_DATA_BITS (8)

namespace
{
    static const char *TAG = "VCP MANAGER";
    static SemaphoreHandle_t device_disconnected_sem;
    static SemaphoreHandle_t ok_received_sem;

    /**
     * @brief Data received callback
     *
     * Just pass received data to stdout
     *
     * @param[in] data     Pointer to received data
     * @param[in] data_len Length of received data in bytes
     * @param[in] arg      Argument we passed to the device open function
     * @return
     *   true:  We have processed the received data
     *   false: We expect more data
     */
    static bool handle_rx(const uint8_t *data, size_t data_len, void *arg)
    {
        printf("%.*s", (int)data_len, data);

        // Check for "ok" in the received data
        if (data_len >= 2)
        {
            for (size_t i = 0; i < data_len - 1; ++i)
            {
                if (data[i] == 'o' && data[i + 1] == 'k')
                {
                    xSemaphoreGive(ok_received_sem);
                    break;
                }
            }
        }

        return true;
    }

    /**
     * @brief Device event callback
     *
     * Apart from handling device disconnection it doesn't do anything useful
     *
     * @param[in] event    Device event type and data
     * @param[in] user_ctx Argument we passed to the device open function
     */
    static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
    {
        switch (event->type)
        {
        case CDC_ACM_HOST_ERROR:
            ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %d", event->data.error);
            break;
        case CDC_ACM_HOST_DEVICE_DISCONNECTED:
            ESP_LOGI(TAG, "Device suddenly disconnected");
            xSemaphoreGive(device_disconnected_sem);
            break;
        case CDC_ACM_HOST_SERIAL_STATE:
            ESP_LOGI(TAG, "Serial state notif 0x%04X", event->data.serial_state.val);
            break;
        case CDC_ACM_HOST_NETWORK_CONNECTION:
        default:
            break;
        }
    }

    /**
     * @brief USB Host library handling task
     *
     * @param arg Unused
     */
    static void usb_lib_task(void *arg)
    {
        while (1)
        {
            // Start handling system events
            uint32_t event_flags;
            usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS)
            {
                ESP_ERROR_CHECK(usb_host_device_free_all());
            }
            if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE)
            {
                ESP_LOGI(TAG, "USB: All devices freed");
                // Continue handling USB events to allow device reconnection
            }
        }
    }
}

/**
 * @brief Main application
 *
 * This function shows how you can use Virtual COM Port drivers
 */
void vcp_usb_manager_run(void *arg)
{
    device_disconnected_sem = xSemaphoreCreateBinary();
    assert(device_disconnected_sem);
    ok_received_sem = xSemaphoreCreateBinary();
    assert(ok_received_sem);
    char *rxmesage;

    // Install USB Host driver. Should only be called once in entire application
    ESP_LOGI(TAG, "Installing USB Host");
    usb_host_config_t host_config = {};
    host_config.skip_phy_setup = false;
    host_config.intr_flags = ESP_INTR_FLAG_LEVEL1;
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    // Create a task that will handle USB library events
    BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, 10, NULL);
    assert(task_created == pdTRUE);

    ESP_LOGI(TAG, "Installing CDC-ACM driver");
    ESP_ERROR_CHECK(cdc_acm_host_install(NULL));

    // Register VCP drivers to VCP service
    VCP::register_driver<CH34x>();

    while (1)
    {
        if (usbCommandQueue != 0)
        {
            const cdc_acm_host_device_config_t dev_config = {
                .connection_timeout_ms = 5000, // 5 seconds, enough time to plug the device in or experiment with timeout
                .out_buffer_size = 512,
                .in_buffer_size = 512,
                .event_cb = handle_event,
                .data_cb = handle_rx,
                .user_arg = NULL,
            };

            // You don't need to know the device's VID and PID. Just plug in any device and the VCP service will load correct (already registered) driver for the device
            ESP_LOGI(TAG, "Opening any VCP device...");
            auto vcp = std::unique_ptr<CdcAcmDevice>(VCP::open(&dev_config));

            if (vcp == nullptr)
            {
                ESP_LOGI(TAG, "Failed to open VCP device");
                continue;
            }
            vTaskDelay(10);
            ESP_LOGI(TAG, "Setting up line coding");
            cdc_acm_line_coding_t line_coding = {
                .dwDTERate = EXAMPLE_BAUDRATE,
                .bCharFormat = EXAMPLE_STOP_BITS,
                .bParityType = EXAMPLE_PARITY,
                .bDataBits = EXAMPLE_DATA_BITS,
            };

            while (1)
            {
                if ((xQueueReceive(usbCommandQueue, &(rxmesage), 10)) == pdTRUE)
                {
                    ESP_ERROR_CHECK(vcp->line_coding_set(&line_coding));

                    // Send some dummy data
                    ESP_LOGI(TAG, "Sending data through CdcAcmDevice %s", rxmesage);
                    ESP_ERROR_CHECK(vcp->tx_blocking((uint8_t *)rxmesage, strlen(rxmesage)));
                    // free up the pointer here and reset it to nullptr;
                    // I was trying to do this in gatt_svc file and was wondering why I was receiving garble here... 
                    // hmm strange right?!? NOT!
                    free(rxmesage);
                    rxmesage = nullptr;
                    ESP_ERROR_CHECK(vcp->set_control_line_state(true, true));
                    ESP_LOGI(TAG, "Waiting for device 'ok'...");
                    xSemaphoreTake(ok_received_sem, portMAX_DELAY);
                }
            }

            // We are done. Wait for device disconnection and start over
            ESP_LOGI(TAG, "Done. You can reconnect the VCP device to run again.");
            xSemaphoreTake(device_disconnected_sem, portMAX_DELAY);
            ESP_LOGI(TAG, "end");
        }
    }
}

void vcp_usb_manager(){
        xTaskCreate(
        vcp_usb_manager_run,
        "vcp_usb_manager",
        4 * 1028,
        NULL,
        10,
        NULL);
}
