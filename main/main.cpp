#include <esp_log.h>
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "vcp_usb_manager.h"
#include "queue_manager.h"

namespace
{
    static const char *TAG = "MAIN";
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
        ESP_LOGI(TAG, "Entering main");

        ESP_LOGI(TAG, "Starting usb manager task...");
        xTaskCreate(
            vcp_usb_manager_run,
            "vcp_usb_manager",
            5 * 1028,
            NULL,
            10,
            NULL);
    }
}