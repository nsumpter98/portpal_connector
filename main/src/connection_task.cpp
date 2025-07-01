#include "Connection.hpp"
#include "common.hpp"
#include <cstdint>
#include <cstddef>


void connection_task(void* pvParameters) {
    Connection* conn = static_cast<Connection*>(pvParameters);
    conn->open();
    uint8_t buffer[512];

    while (1) {
        size_t receivedLen = 0;
        if (conn->receive(buffer, sizeof(buffer), receivedLen)) {
            // Process received data
        }
        // Optionally, send outgoing data if needed
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    conn->close();
    vTaskDelete(NULL);
}