#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "vcp_usb_manager.hpp"
#include "queue_manager.h"
#include "common.h"
#include "gap.h"
#include "gatt_svc.h"

static const char *TAG_MAIN = "MAIN";

/* Library function declarations */
void ble_store_config_init(void);


static void on_stack_reset(int reason) {
    /* On reset, print reset reason to console */
    ESP_LOGI(TAG_MAIN, "nimble stack reset, reset reason: %d", reason);
}

static void on_stack_sync(void) {
    /* On stack sync, do advertising initialization */
    adv_init();
}


static void nimble_host_config_init(void) {
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Store host configuration */
    ble_store_config_init();
}

static void nimble_host_task(void *param) {
    /* Task entry log */
    ESP_LOGI(TAG_MAIN, "nimble host task has been started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    /* Clean up at exit */
    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Adding test to queue");

    initialize_queues();
    const char *initialStr = "G28\n";
    xQueueSend(usbCommandQueue, &initialStr, 0);
    const char *initialStr2 = "G28 X\n";
    xQueueSend(usbCommandQueue, &initialStr2, 0);
    const char *initialStr3 = "G0 X200 Y200 F30000\n";
    xQueueSend(usbCommandQueue, &initialStr3, 0);
    ESP_LOGI(TAG_MAIN, "Entering main");

    ESP_LOGI(TAG_MAIN, "Starting usb manager task...");
    xTaskCreate(
        vcp_usb_manager_run,
        "vcp_usb_manager",
        5 * 1028,
        NULL,
        10,
        NULL);
}
