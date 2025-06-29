#include "queue_manager.h"

QueueHandle_t usbCommandQueue = NULL;

// for now I will push incoming commands for the bridge here in a shared queue
// this will be accessed by the bluetooth manager to push commands and by the 
// usb manager to read commands and send to the connected device
void initialize_queues()
{
    // do more research into what to do here
}