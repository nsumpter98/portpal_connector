#include "esp_mac.h"
#include "usb/vcp_usb_manager.hpp"
#include "bluetooth/bluetooth_manager.hpp"

static const char *TAG_MAIN = "MAIN";

// this will get cleaned up soon :p
extern "C" void app_main(void)
{

    ESP_LOGI(TAG_MAIN, "Adding test to queue");

    initialize_queues();
    ESP_LOGI(TAG_MAIN, "Entering main");

    ESP_LOGI(TAG_MAIN, "Starting usb manager task...");
    bluetooth_manager();
    vcp_usb_manager();
}
