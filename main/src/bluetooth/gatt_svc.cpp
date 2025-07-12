#include "bluetooth/gatt_svc.hpp"

void GattSvc::add_ble_gatt_svc(struct ble_gatt_svc_def gatt_svc)
{
    gatt_svcs.push_back(gatt_svc);
}

void GattSvc::gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    /* Local variables */
    char buf[BLE_UUID_STR_LEN];

    /* Handle GATT attributes register events */
    switch (ctxt->op)
    {

    /* Service register event */
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;

    /* Characteristic register event */
    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG,
                 "registering characteristic %s with "
                 "def_handle=%d val_handle=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;

    /* Descriptor register event */
    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    /* Unknown event */
    default:
        assert(0);
        break;
    }
}

int GattSvc::gatt_svc_init(void)
{
    /* Local variables */
    int rc;
    svc_count = gatt_svcs.size();

    ESP_LOGI("gatt", "about to convert vector to string");
    // copy the contents of the vector into the array
    gatt_svr_svcs = std::make_unique<ble_gatt_svc_def[]>(svc_count + 1); // +1 for terminating zeroed entry
    std::copy(gatt_svcs.begin(), gatt_svcs.end(), gatt_svr_svcs.get());
    memset(&gatt_svr_svcs[svc_count], 0, sizeof(ble_gatt_svc_def));

    ESP_LOGI("gatt", "vector converted");

    /* 1. GATT service initialization */
    ble_svc_gatt_init();

    /* 2. Update GATT services counter */
    rc = ble_gatts_count_cfg(gatt_svr_svcs.get());
    if (rc != 0)
    {
        return rc;
    }

    /* 3. Add GATT services */
    rc = ble_gatts_add_svcs(gatt_svr_svcs.get());
    if (rc != 0)
    {
        return rc;
    }

    return 0;
}