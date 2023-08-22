// System includes
#include <string.h>

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_rom_gpio.h"

// Project includes
#include "lora_common.h"
#include "lora_receiver.h"
#include "lora_transmitter.h"
#include "mac_helper.h"
#include "bme280_helper.h"

const static char *TAG = "MAIN";

#if CONFIG_RECEIVER
static void convert_uint8_array_to_int(uint8_t *data, int *value) {
    *value = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

static QueueHandle_t lora_rcvd_data_queue;


static void get_mac_addr_from_lora_data(lora_common_data_t *lora_data, uint8_t *mac_addr) {
    memcpy(mac_addr, lora_data->data, MAC_ADDR_LEN);
}

// Task handling the received data:
static void handle_rx(void *pvParameters) {
    lora_common_data_t rxData;
    serialized_data_t temperature;
    serialized_data_t humidity;
    serialized_data_t pressure;

    uint8_t mac_addr[MAC_ADDR_LEN];

    for(;;)
    {
        if(xQueueReceive(lora_rcvd_data_queue, &(rxData), (TickType_t)10))
        {
            // The data from the queue was successfully received and removed from the queue
            // You can now use the rxData
            get_mac_addr_from_lora_data(&rxData, mac_addr);

            ESP_LOGI(pcTaskGetName(NULL), "Data from : [ %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x ]",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
                     mac_addr[4], mac_addr[5], mac_addr[6], mac_addr[7]);

	    if (rxData.data[LORA_MSG_TYPE_OFFSET] == vLORA_MSG_TYPE_BME280) {
	      ESP_LOGI(pcTaskGetName(NULL), "Received BME280 msg...");
	      memcpy(temperature.serialized, &rxData.data[LORA_MSG_TEMPERATURE_OFFSET], SERIALIZED_DOUBLE_SZ);
	      memcpy(humidity.serialized, &rxData.data[LORA_MSG_HUMIDITY_OFFSET], SERIALIZED_DOUBLE_SZ);
	      memcpy(pressure.serialized, &rxData.data[LORA_MSG_PRESSURE_OFFSET], SERIALIZED_DOUBLE_SZ);
	      ESP_LOGI(pcTaskGetName(NULL), "Temperature:%.1f", temperature.value);
	      ESP_LOGI(pcTaskGetName(NULL), "Humidity:%.1f", humidity.value);
	      ESP_LOGI(pcTaskGetName(NULL), "Pressure:%.1f", pressure.value);
	    } else {
	      ESP_LOGI(pcTaskGetName(NULL), "Unknown message");
	    }
        }
        // To keep watchdog happy:
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void act_as_receiver() {
    // Test receiver with xQueues.
    lora_rcvd_data_queue = xQueueCreate(4, sizeof(lora_common_data_t));
    if(lora_rcvd_data_queue == NULL)
    {
        ESP_LOGE(TAG, "Could not create receive queue.");
    }

    xTaskCreate(handle_rx, "handle_rx", 1024*4, NULL, 5, NULL);
    lora_receiver_start_receiving(lora_rcvd_data_queue);
}
#endif

#ifdef CONFIG_SENDER
static void get_mac(uint8_t *mac_addr) {
    mac_helper_status_t mac_helper_status = mac_helper_get_mac(mac_addr);
    if (mac_helper_status == MAC_HELPER_STATUS_OK) {
        mac_helper_debug_show_mac(mac_addr);
    } else {
        ESP_LOGE(TAG, "Could not get MAC address.");
    }
}

static void print_uint8_array(uint8_t *data, int len) {

    char buf[256] = {0};

    int pos = 0;

    for (int i = 0; i < len; i++) {
        pos += snprintf(buf + pos, sizeof(buf) - pos, "%02x ", data[i]);
    }
}

static void convert_int_to_uint8_array(int value, uint8_t *data) {
    data[0] = (value >> 24) & 0xFF;
    data[1] = (value >> 16) & 0xFF;
    data[2] = (value >> 8) & 0xFF;
    data[3] = value & 0xFF;
}

static void prepare_tx_packet(lora_common_data_t *tx_data, uint8_t *mac_addr, bme280_data_t *p_data) {
    // Copy MAC address to tx_data
    memcpy(tx_data->data, mac_addr, MAC_ADDR_LEN);

    tx_data->data[LORA_MSG_TYPE_OFFSET] = vLORA_MSG_TYPE_BME280;

    // BME280 temperature, humidity, pressure
    memcpy(&tx_data->data[LORA_MSG_TEMPERATURE_OFFSET], p_data->temperature.serialized, SERIALIZED_DOUBLE_SZ);
    memcpy(&tx_data->data[LORA_MSG_HUMIDITY_OFFSET], p_data->humidity.serialized, SERIALIZED_DOUBLE_SZ);
    memcpy(&tx_data->data[LORA_MSG_PRESSURE_OFFSET], p_data->pressure.serialized, SERIALIZED_DOUBLE_SZ);

    tx_data->len = MAC_ADDR_LEN + LORA_MSG_TYPE_SZ + (SERIALIZED_DOUBLE_SZ * 3);

    // Debug, show tx_data->data array:
    print_uint8_array(tx_data->data, (int)tx_data->len);
}

static void act_as_sender() {
    lora_common_data_t txData;
    QueueHandle_t lora_send_data_queue;
    bme280_data_t bme280_data;

    lora_send_data_queue = xQueueCreate(4, sizeof(lora_common_data_t));

    // Get current MAC address
    uint8_t mac_addr[8];
    get_mac(mac_addr);

    lora_transmitter_start_sending(lora_send_data_queue);

    while (1) {

        // Print MAC accress
        ESP_LOGI(TAG, "Data to send: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
                 mac_addr[4], mac_addr[5], mac_addr[6], mac_addr[7]);

	if (BME280_HELPER_STATUS_OK == bme280_get_data(&bme280_data)) {
	  prepare_tx_packet(&txData, mac_addr, &bme280_data);
	  if(xQueueSend(lora_send_data_queue, (void*)&txData, (TickType_t)10) != pdPASS)
	    {
	      ESP_LOGE(TAG, "Could not send data to queue. Queue full.");
	    }
	}

	vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
#endif

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

    // Init BME280
    bme280_i2c_master_init();
    xTaskCreate(&bme280_normal_mode_task, "bme280_normal_mode",  2048, NULL, 6, NULL);
    
    // Init lora
    lora_status_t lora_init_status = lora_common_init();
    if (lora_init_status == LORA_STATUS_OK) {
        ESP_LOGI(TAG, "LoRa initialization successful");
#if CONFIG_RECEIVER
    act_as_receiver();
#elif CONFIG_SENDER
    act_as_sender();
#endif

    } else {
        ESP_LOGE(TAG, "LoRa initialization failed: %d", lora_init_status);
    }



    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Free heap: %"PRIu32"", esp_get_free_heap_size());
    }
}
