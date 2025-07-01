#include "esp_mac.h"
#include "bluetooth/bluetooth_manager.hpp"
#include "usb/UsbConnection.hpp"
#include "src/connection_task.cpp"

static const char *TAG_MAIN = "MAIN";

// this will get cleaned up soon :p
extern "C" void app_main(void)
{

    ESP_LOGI(TAG_MAIN, "Adding test to queue");

    initialize_queues();
    ESP_LOGI(TAG_MAIN, "Entering main");

    ESP_LOGI(TAG_MAIN, "Starting usb manager task...");
    bluetooth_manager();
    UsbConnection* usbConn = new UsbConnection();
    xTaskCreate(connection_task, "usb_conn_task", 4096, usbConn, 10, NULL);
}
