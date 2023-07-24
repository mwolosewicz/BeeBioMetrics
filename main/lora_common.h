//
// Created by mw on 25.06.23.
//

#ifndef ESP32_LORA_BATT_MONITOR_LORA_COMMON_H
#define ESP32_LORA_BATT_MONITOR_LORA_COMMON_H

#include <stdint.h>

typedef enum {
    LORA_STATUS_OK,
    LORA_STATUS_ERROR
} lora_status_t;

typedef enum {
    LORA_DATA_TYPE_VOLTAGE,
} lora_data_type_t;

typedef struct {
    uint8_t mac_addr[8];
    uint8_t data_type;
    uint8_t data[256 - 8 - 1];
    uint8_t data_len;
} lora_packet_data_t;

/**
 * Data structure to send data to the LoRa transmitter task.
 * - data is the data to send.
 * - len is the length of the data.
 */
typedef struct {
    uint8_t data[256];
    uint8_t len;
} lora_common_data_t;

/**
 * Initialzie the LoRa device with default parameters.
 * @return LORA_OK if everything is ok, otherwise LORA_ERROR.
 */
lora_status_t lora_common_init();


#endif //ESP32_LORA_BATT_MONITOR_LORA_COMMON_H
