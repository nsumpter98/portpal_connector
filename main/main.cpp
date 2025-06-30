#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "vcp_usb_manager.hpp"
#include "queue_manager.h"
#include "common.h"
#include "gap.h"
#include "gatt_svc.h"
#include "bluetooth_manager.h"

static const char *TAG_MAIN = "MAIN";

extern "C" void app_main(void)
{

    ESP_LOGI(TAG, "Adding test to queue");

    initialize_queues();
    ESP_LOGI(TAG_MAIN, "Entering main");

    ESP_LOGI(TAG_MAIN, "Starting usb manager task...");
    bluetooth_manager();
    xTaskCreate(
        vcp_usb_manager_run,
        "vcp_usb_manager",
        5 * 1028,
        NULL,
        10,
        NULL);
    
}
