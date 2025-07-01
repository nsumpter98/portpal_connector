#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

extern QueueHandle_t usbCommandQueue;
void initialize_queues();

#endif
