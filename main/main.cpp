#include <esp_log.h>
#include "esp_mac.h"

namespace
{
    static const char *TAG = "MAIN";
    extern "C" void app_main(void)
    {
        ESP_LOGI(TAG, "Entering main");
        
        while (1)
        {
            
        }
    }
}