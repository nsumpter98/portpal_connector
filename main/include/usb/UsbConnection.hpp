#include "Connection.hpp"
#include "usb/vcp_usb_manager.hpp"
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <cstring>
#include <cassert>


class UsbConnection : public Connection
{
public:
    UsbConnection();
    ~UsbConnection();
    bool open() override;
    void close() override;
    bool send(uint8_t *data, size_t len) override;
    bool receive(uint8_t *data, size_t maxLen, size_t &receivedLen) override;

private:
    std::unique_ptr<CdcAcmDevice> vcp;
    SemaphoreHandle_t ok_received_sem;
    SemaphoreHandle_t device_disconnected_sem;
    static bool handle_rx(const uint8_t *data, size_t data_len, void *arg);
    static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx);
    void setup_line_coding();
    bool is_open = false;
};