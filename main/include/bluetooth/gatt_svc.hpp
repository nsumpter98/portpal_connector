#ifndef GATT_SVR_H
#define GATT_SVR_H

#include "common.hpp"
#include "queue_manager.hpp"
#include "vector"
#include <memory>

/* NimBLE GATT APIs */
#include "host/ble_gatt.h"
#include "services/gatt/ble_svc_gatt.h"

/* NimBLE GAP APIs */
#include "host/ble_gap.h"

class GattSvc
{
public:
    /*
    Example:
    will decide later if I want to break down the services/characteristics further. for now this works
        {
            .type = BLE_GATT_SVC_TYPE_PRIMARY,
            .uuid = &auto_io_svc_uuid.u,
            .characteristics =
                (struct ble_gatt_chr_def[]){
                                            {.uuid = &led_chr_uuid.u,
                                             .access_cb = led_chr_access,
                                             .flags = BLE_GATT_CHR_F_WRITE,
                                             .val_handle = &led_chr_val_handle},
                                            {0}},
        },

        {
            0,
        },
    */
    void add_ble_gatt_svc(struct ble_gatt_svc_def gatt_svc);

    /*
     *  Handle GATT attribute register events
     *      - Service register event
     *      - Characteristic register event
     *      - Descriptor register event
     */
    static void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

    /*
     *  GATT server initialization
     *      1. Initialize GATT service
     *      2. Update NimBLE host GATT services counter
     *      3. Add GATT services to server
     */
    int gatt_svc_init(void);

private:
    // decided to use vector here to simplify the resizing of memory
    std::vector<ble_gatt_svc_def> gatt_svcs;
    std::unique_ptr<ble_gatt_svc_def[]> gatt_svr_svcs;
    size_t svc_count = 0;
};

#endif // GATT_SVR_H
