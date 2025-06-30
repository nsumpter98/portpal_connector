#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

// adding this compiler check since I plan to write most of this project in pure c
// reason is to keep my code consistant with the majority of the esp-idf example code.
// only need cpp for the usb manager so far
#ifdef __cplusplus
extern "C"
{
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

    extern QueueHandle_t usbCommandQueue;
    void initialize_queues();

#ifdef __cplusplus
}
#endif

#endif
