//
// Created by mw on 25.06.23.
//

// System
#include <string.h>

// ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Project
#include "lora_common.h"
#include "lora.h"

const static char *TAG = "LORA_COMMON";

lora_status_t lora_common_init() {
    if (lora_init() == 0) {
        ESP_LOGE(TAG, "Can't initialize LoRa. Does not recognize the module");
        return LORA_STATUS_ERROR;
    }

    lora_set_tx_power(CONFIG_TX_POWER);

    lora_set_tx_power(CONFIG_TX_POWER);

#if CONFIG_169MHZ
    ESP_LOGI(pcTaskGetName(NULL), "Frequency is 169MHz");
	lora_set_frequency(169e6); // 169MHz
#elif CONFIG_433MHZ
    ESP_LOGI(pcTaskGetName(NULL), "Frequency is 433MHz");
	lora_set_frequency(433e6); // 433MHz
#elif CONFIG_470MHZ
    ESP_LOGI(pcTaskGetName(NULL), "Frequency is 470MHz");
	lora_set_frequency(470e6); // 470MHz
#elif CONFIG_866MHZ
    ESP_LOGI(pcTaskGetName(NULL), "Frequency is 866MHz");
	lora_set_frequency(866e6); // 866MHz
#elif CONFIG_915MHZ
    ESP_LOGI(pcTaskGetName(NULL), "Frequency is 915MHz");
	lora_set_frequency(915e6); // 915MHz
#elif CONFIG_OTHER
    ESP_LOGI(pcTaskGetName(NULL), "Frequency is %dMHz", CONFIG_OTHER_FREQUENCY);
	long frequency = CONFIG_OTHER_FREQUENCY * 1000000;
	lora_set_frequency(frequency);
#endif

    lora_enable_crc();

    int cr = 1;
    int bw = 7;
    int sf = 7;
#if CONFIF_ADVANCED
    cr = CONFIG_CODING_RATE
	bw = CONFIG_BANDWIDTH;
	sf = CONFIG_SF_RATE;
#endif

    lora_set_coding_rate(cr);
    ESP_LOGI(pcTaskGetName(NULL), "coding_rate=%d", cr);

    lora_set_bandwidth(bw);
    ESP_LOGI(pcTaskGetName(NULL), "bandwidth=%d", bw);

    lora_set_spreading_factor(sf);
    ESP_LOGI(pcTaskGetName(NULL), "spreading_factor=%d", sf);

    return LORA_STATUS_OK;
}
