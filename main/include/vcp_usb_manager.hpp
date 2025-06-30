#include "queue_manager.h"
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

void vcp_usb_manager_run(void *arg);
// still trying to figure out a good structure for everything.
// this will work for now but I intend to find a cleaner way of doing this
void vcp_usb_manager();
