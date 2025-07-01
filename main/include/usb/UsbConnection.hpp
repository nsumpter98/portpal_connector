#include "Connection.hpp"
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include <cstring>
#include <cassert>
#include "queue_manager.hpp"
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "usb/cdc_acm_host.h"
#include "usb/vcp_ch34x.hpp"
#include "usb/vcp_cp210x.hpp"
#include "usb/vcp_ftdi.hpp"
#include "usb/vcp.hpp"
#include "usb/usb_host.h"
#include <string>

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
    static void usb_lib_task(void *arg);
    SemaphoreHandle_t ok_received_sem;
    SemaphoreHandle_t device_disconnected_sem;
    static bool handle_rx(const uint8_t *data, size_t data_len, void *arg);
    static void handle_event(const cdc_acm_host_dev_event_data_t *event, void *user_ctx);
    void setup_line_coding();
    bool is_open = false;
};