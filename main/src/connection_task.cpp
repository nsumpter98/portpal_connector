#include "Connection.hpp"
#include "common.hpp"
#include <cstdint>
#include <cstddef>
extern QueueHandle_t usbCommandQueue;
void connection_task(void *pvParameters)
{
    Connection *conn = static_cast<Connection *>(pvParameters);
    conn->open();
    uint8_t buffer[512];
    char *rxmsg = nullptr;
    while (1)
    {
        size_t receivedLen = 0;
        if (conn->receive(buffer, sizeof(buffer), receivedLen))
        {
            // Process received data
        }
        if (xQueueReceive(usbCommandQueue, &rxmsg, portMAX_DELAY) == pdTRUE)
        {
            size_t msglen = strlen(rxmsg);
            conn->send(reinterpret_cast<uint8_t *>(rxmsg), msglen);
        }
        // Optionally, send outgoing data if needed
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    conn->close();
    vTaskDelete(NULL);
}