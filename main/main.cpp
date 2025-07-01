#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "vcp_usb_manager.hpp"
#include "queue_manager.hpp"
#include "common.hpp"
#include "gap.hpp"
#include "gatt_svc.hpp"
#include "bluetooth_manager.hpp"

static const char *TAG_MAIN = "MAIN";

// this will get cleaned up soon :p
extern "C" void app_main(void)
{

    ESP_LOGI(TAG, "Adding test to queue");

    initialize_queues();
    ESP_LOGI(TAG_MAIN, "Entering main");

    ESP_LOGI(TAG_MAIN, "Starting usb manager task...");
    bluetooth_manager();
    vcp_usb_manager();
}
