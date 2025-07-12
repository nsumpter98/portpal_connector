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

// GAP (Generic Access Profile) layer defines the connection behaviors between Bluetooth LE devices and the roles they play in the connection.
// Ref: https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32s3/api-guides/ble/get-started/ble-introduction.html#:~:text=The-,GAP,-(Generic%20Access%20Profile
// since we only plan to use portpal as a peripheral device I wont focus any on scanning.
// currently referencing the following docs to build this out: https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32s3/api-guides/ble/get-started/ble-introduction.html
class Gap
{
public:
    Gap();
    void advertizing_init(void);
    int gap_init(void);

private:
    uint8_t own_addr_type;
    uint8_t addr_val[6] = {0};
    uint8_t esp_uri[16] = {BLE_GAP_URI_PREFIX_HTTPS, '/', '/', 'e', 's', 'p', 'r', 'e', 's', 's', 'i', 'f', '.', 'c', 'o', 'm'};

    void print_conn_desc(struct ble_gap_conn_desc *desc);
    void format_addr(char *addr_str, uint8_t addr[]);
    static int gap_event_handler(struct ble_gap_event *event, void *arg);
    int handle_gap_event(struct ble_gap_event *event);
    void start_advertising(void);
};

#endif // GAP_SVC_H
