//
// Created by mw on 25.06.23.
//

#ifndef ESP32_LORA_BATT_MONITOR_RECEIVER_H
#define ESP32_LORA_BATT_MONITOR_RECEIVER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * Start receiveing LoRa packets. The received packets are put into the given queue.
 * @param received_queue is the queue to put the received packets into.
 */
void lora_receiver_start_receiving(QueueHandle_t received_queue);


#endif //ESP32_LORA_BATT_MONITOR_RECEIVER_H
