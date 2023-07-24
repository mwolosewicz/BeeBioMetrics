//
// Created by mw on 24.06.23.
//

#ifndef ESP32_LORA_BATT_MONITOR_LORA_HELPER_H
#define ESP32_LORA_BATT_MONITOR_LORA_HELPER_H

typedef enum {
    LORA_HELPER_STATUS_OK,
    LORA_HELPER_STATUS_ERROR,
} lora_helper_status_t;

/**
 * Initialzie the LoRa device with default parameters.
 * @return LORA_HELPER_STATUS_OK if everything is ok, otherwise LORA_HELPER_STATUS_ERROR.
 */
lora_helper_status_t lora_helper_init();


#endif //ESP32_LORA_BATT_MONITOR_LORA_HELPER_H
