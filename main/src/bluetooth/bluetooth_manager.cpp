#include "common.hpp"
#include "bluetooth/gap.hpp"
#include "bluetooth/gatt_svc.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
    void ble_store_config_init(void);
#ifdef __cplusplus
}
#endif

static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);
static uint16_t led_chr_val_handle;
static const ble_uuid128_t led_chr_uuid =
    BLE_UUID128_INIT(0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde, 0xef,
                     0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

Gap g = Gap();
GattSvc gatt = GattSvc();



// THE BELOW IS MESSY BUT WILL BE CLEANED UP BEFORE MERGE TO MASTER
/* Private function declarations */
static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg);

/* Private functions */
static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    /* Local variables */
    int rc;

    /* Handle access events */
    /* Note: LED characteristic is write only */
    switch (ctxt->op)
    {

    /* Write characteristic event */
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        /* Verify connection handle */
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE)
        {
            ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
                     conn_handle, attr_handle);
        }
        else
        {
            ESP_LOGI(TAG,
                     "characteristic write by nimble stack; attr_handle=%d",
                     attr_handle);
        }

        /* Verify attribute handle */
        if (attr_handle == led_chr_val_handle)
        {
            /* Verify access buffer length */
            if (ctxt->om->om_len >= 1)
            {
                // Allocate buffer for the string (+1 for null terminator)
                char *cmdStr = (char *)malloc(ctxt->om->om_len + 2);
                if (cmdStr == NULL)
                {
                    ESP_LOGE(TAG, "malloc failed");
                    goto error;
                }

                // Copy data and null-terminate
                memcpy(cmdStr, ctxt->om->om_data, ctxt->om->om_len);
                cmdStr[ctxt->om->om_len] = '\n';     // Add newline
                cmdStr[ctxt->om->om_len + 1] = '\0'; // Null-terminate

                ESP_LOGI(TAG, "Received GATT write: %s", cmdStr);

                // Send the pointer to the queue
                xQueueSend(usbCommandQueue, &cmdStr, 0);

                /* Turn the LED on or off according to the operation bit */
                if (ctxt->om->om_data[0])
                {
                    // led_on();
                    ESP_LOGI(TAG, "led turned on!");
                }
                else
                {
                    // led_off();
                    ESP_LOGI(TAG, "led turned off!");
                }
            }
            else
            {
                goto error;
            }
            return rc;
        }
        goto error;

    /* Unknown event */
    default:
        goto error;
    }

error:
    ESP_LOGE(TAG,
             "unexpected access operation to led characteristic, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
}

static void on_stack_reset(int reason)
{
    /* On reset, print reset reason to console */
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}

static void on_stack_sync(void)
{
    /* On stack sync, do advertising initialization */
    g.advertizing_init();
}

static void nimble_host_config_init(void)
{
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.gatts_register_cb = gatt.gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Store host configuration */
    ble_store_config_init();
}

static void nimble_host_task(void *param)
{
    /* Task entry log */
    ESP_LOGI(TAG, "nimble host task has been started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    /* Clean up at exit */
    vTaskDelete(NULL);
}

void bluetooth_manager()
{
    /* Local variables */
    int rc;
    esp_err_t ret;

    /* LED initialization */
    // led_init();

    /*
     * NVS flash initialization
     * Dependency of BLE stack to store configurations
     */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", ret);
        return;
    }

    /* NimBLE stack initialization */
    ret = nimble_port_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "failed to initialize nimble stack, error code: %d ",
                 ret);
        return;
    }

    /* GAP service initialization */
    rc = g.gap_init();
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to initialize GAP service, error code: %d", rc);
        return;
    }

    ESP_LOGI(TAG, "adding services");
    /* GATT server initialization */
    gatt.add_ble_gatt_svc({
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &auto_io_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){/* LED characteristic */
                                        {.uuid = &led_chr_uuid.u,
                                         .access_cb = led_chr_access,
                                         .flags = BLE_GATT_CHR_F_WRITE,
                                         .val_handle = &led_chr_val_handle},
                                        {0}},
    });
    ESP_LOGI(TAG, "services added now init");
    rc = gatt.gatt_svc_init();
    if (rc != 0)
    {
        ESP_LOGE(TAG, "failed to initialize GATT server, error code: %d", rc);
        return;
    }
    /* NimBLE host configuration initialization */
    nimble_host_config_init();

    xTaskCreate(nimble_host_task, "NimBLE Host", 4 * 1024, NULL, 5, NULL);
}
