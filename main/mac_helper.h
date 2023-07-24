//
// Created by mw on 25.06.23.
//

#ifndef ESP32_LORA_BATT_MONITOR_MAC_HELPER_H
#define ESP32_LORA_BATT_MONITOR_MAC_HELPER_H

#include <stdint.h>

#define MAC_ADDR_LEN 8
_Static_assert(MAC_ADDR_LEN >= 8, "MAC_ADDR_LEN must not be smaller than 8");

typedef enum {
    MAC_HELPER_STATUS_OK,
    MAC_HELPER_STATUS_ERROR
} mac_helper_status_t;

/**
 * Get the MAC address for this device.
 * @param mac_addr pointer to a 8 byte array to store the MAC address,
 * @return MAC_HELPER_STATUS_OK if the MAC address was successfully retrieved, MAC_HELPER_STATUS_ERROR otherwise.
 */
mac_helper_status_t mac_helper_get_mac (uint8_t *mac_addr);

/**
 * Print the MAC address to the debug log.
 * @param mac_addr pointer to a 8 byte array containing the MAC address.
 */
void mac_helper_debug_show_mac (uint8_t *mac_addr);

#endif //ESP32_LORA_BATT_MONITOR_MAC_HELPER_H
