#include "usb/UsbConnection.hpp"

using namespace esp_usb;

#define EXAMPLE_BAUDRATE (115200)
#define EXAMPLE_STOP_BITS (0)
#define EXAMPLE_PARITY (0)
#define EXAMPLE_DATA_BITS (8)

static const char *TAG = "UsbConnection";

UsbConnection::UsbConnection()
{
    ok_received_sem = xSemaphoreCreateBinary();
    device_disconnected_sem = xSemaphoreCreateBinary();
}

UsbConnection::~UsbConnection()
{
    close();
    vSemaphoreDelete(ok_received_sem);
    vSemaphoreDelete(device_disconnected_sem);
}

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

bool UsbConnection::open()
{
    usb_host_config_t host_config = {};
    host_config.skip_phy_setup = false;
    host_config.intr_flags = ESP_INTR_FLAG_LEVEL1;
    ESP_ERROR_CHECK(usb_host_install(&host_config));
    BaseType_t task_created = xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, 10, NULL);
    assert(task_created == pdTRUE);
    ESP_ERROR_CHECK(cdc_acm_host_install(NULL));
    VCP::register_driver<CH34x>();
    // Device config
    const cdc_acm_host_device_config_t dev_config = {
        .connection_timeout_ms = 5000,
        .out_buffer_size = 512,
        .in_buffer_size = 512,
        .event_cb = handle_event,
        .data_cb = handle_rx,
        .user_arg = this,
    };
    vcp.reset(VCP::open(&dev_config));
    if (!vcp)
    {
        ESP_LOGE(TAG, "Failed to open VCP device");
        return false;
    }
    setup_line_coding();
    is_open = true;
    return true;
}

void UsbConnection::setup_line_coding()
{
    cdc_acm_line_coding_t line_coding = {
        .dwDTERate = EXAMPLE_BAUDRATE,
        .bCharFormat = EXAMPLE_STOP_BITS,
        .bParityType = EXAMPLE_PARITY,
        .bDataBits = EXAMPLE_DATA_BITS,
    };
    ESP_ERROR_CHECK(vcp->line_coding_set(&line_coding));
    ESP_ERROR_CHECK(vcp->set_control_line_state(true, true));
}

bool UsbConnection::send(uint8_t *data, size_t len)
{
    if (!is_open || !vcp)
        return false;
    ESP_LOGI(TAG, "Sending data through CdcAcmDevice");
    esp_err_t err = vcp->tx_blocking(data, len);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to send data: %d", err);
        return false;
    }
    ESP_LOGI(TAG, "Waiting for device 'ok'...");
    xSemaphoreTake(ok_received_sem, portMAX_DELAY);
    return true;
}

bool UsbConnection::receive(uint8_t *data, size_t maxLen, size_t &receivedLen)
{
    // This would be handled via the callback, but you can queue/pipe data to your app here.
    return 0;
}

void UsbConnection::close()
{
    if (is_open && vcp)
    {
        vcp.reset();
        is_open = false;
    }
}

bool UsbConnection::handle_rx(const uint8_t *data, size_t data_len, void *arg)
{
    UsbConnection *self = static_cast<UsbConnection *>(arg);
    printf("%.*s", (int)data_len, data);
    if (data_len >= 2)
    {
        for (size_t i = 0; i < data_len - 1; ++i)
        {
            if (data[i] == 'o' && data[i + 1] == 'k')
            {
                xSemaphoreGive(self->ok_received_sem);
                break;
            }
        }
    }
    return true;
}

void UsbConnection::handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx)
{
    UsbConnection *self = static_cast<UsbConnection *>(user_ctx);
    switch (event->type)
    {
    case CDC_ACM_HOST_ERROR:
        ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %d", event->data.error);
        break;
    case CDC_ACM_HOST_DEVICE_DISCONNECTED:
        ESP_LOGI(TAG, "Device suddenly disconnected");
        xSemaphoreGive(self->device_disconnected_sem);
        break;
    case CDC_ACM_HOST_SERIAL_STATE:
        ESP_LOGI(TAG, "Serial state notif 0x%04X", event->data.serial_state.val);
        break;
    case CDC_ACM_HOST_NETWORK_CONNECTION:
    default:
        break;
    }
}