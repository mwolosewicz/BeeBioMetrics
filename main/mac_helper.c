//
// Created by mw on 25.06.23.
//

#include <string.h>

#include "mac_helper.h"
#include "esp_log.h"
#include "esp_mac.h"

static const char *TAG = "MAC_HELPER";

mac_helper_status_t mac_helper_get_mac (uint8_t *mac_addr) {
    if (esp_efuse_mac_get_default(mac_addr) == ESP_OK) {
        return MAC_HELPER_STATUS_OK;
    } else {
        memset(mac_addr, 0, MAC_ADDR_LEN);
        return MAC_HELPER_STATUS_ERROR;
    }
}

void mac_helper_debug_show_mac (uint8_t *mac_addr) {

    ESP_LOGI(TAG, "MAC: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
             mac_addr[4], mac_addr[5], mac_addr[6], mac_addr[7]);
}