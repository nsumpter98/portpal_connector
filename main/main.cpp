#include <esp_log.h>
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "vcp_usb_manager.h"

namespace
{
    static const char *TAG = "MAIN";
    extern "C" void app_main(void)
    {
        ESP_LOGI(TAG, "Entering main");

        ESP_LOGI(TAG, "Starting usb manager task...");
        xTaskCreate(
            vcp_usb_manager::vcp_usb_manager_run,
            "vcp_usb_manager",
            4096,
            NULL,
            10,
            NULL
        );
    }
}