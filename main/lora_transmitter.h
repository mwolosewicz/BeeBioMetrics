//
// Created by mw on 25.06.23.
//

#ifndef ESP32_LORA_BATT_MONITOR_TRANSMITTER_H
#define ESP32_LORA_BATT_MONITOR_TRANSMITTER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * Start the LoRa transmitter task.
 * @param send_queue the queue to send data to the transmitter task.
 */
void lora_transmitter_start_sending(QueueHandle_t send_queue);

/**
 * Check if the data is currently transmitting. It may be used ex. when we want to send data and then go sleep.
 * To avoid situation that we go sleep before the data is sent we can ceck if the data is currently transmitting
 * using this function.
 *
 * Function is thread safe.
 *
 * @return true if there is data pending to send, false otherwise.
 */
bool lora_transmitter_is_transmiting();

/**
 * Get counter for lora messages
 *
 * @return counter
 */
uint16_t lora_get_msg_counter(void);

#endif //ESP32_LORA_BATT_MONITOR_TRANSMITTER_H
