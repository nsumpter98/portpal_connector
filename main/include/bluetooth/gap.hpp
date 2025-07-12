#ifndef GAP_SVC_H
#define GAP_SVC_H

/* Includes */
/* NimBLE GAP APIs */
#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"
#include "common.hpp"

/* Defines */
#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0200
#define BLE_GAP_URI_PREFIX_HTTPS 0x17
#define BLE_GAP_LE_ROLE_PERIPHERAL 0x00

static uint8_t own_addr_type;
static uint8_t addr_val[6] = {0};
static uint8_t esp_uri[16] = {BLE_GAP_URI_PREFIX_HTTPS, '/', '/', 'e', 's', 'p', 'r', 'e', 's', 's', 'i', 'f', '.', 'c', 'o', 'm'};

// since we only plan to use portpal as a peripheral device I wont focus any on scanning.
// currently referencing the following docs to build this out: https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32s3/api-guides/ble/get-started/ble-introduction.html
class Gap
{
public:
    Gap();
    void advertizing_init(void);
    int gap_init(void);

private:
    // had to make all these static because of the ble_gap_adv_start function in the ble_gap_adv_start method.
    // I think this is because that function is expecting a standard c pointer and I was trying to pass a member
    // of this class. would like to find a better way to do this in the future.
    void print_conn_desc(struct ble_gap_conn_desc *desc);
    void format_addr(char *addr_str, uint8_t addr[]);
    static int gap_event_handler(struct ble_gap_event *event, void *arg);
    int handle_gap_event(struct ble_gap_event *event);
    void start_advertising(void);
};

#endif // GAP_SVC_H
