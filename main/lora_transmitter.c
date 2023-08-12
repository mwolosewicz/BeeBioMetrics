//
// Created by mw on 25.06.23.
//

#include "lora_transmitter.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "lora.h"
#include "lora_common.h"
#include "sleep.h"


static QueueHandle_t transmitter_data_queue;
static bool is_transmitting = false;
static portMUX_TYPE xTransmitMutex = portMUX_INITIALIZER_UNLOCKED;

static void task_tx(void *pvParameters) {
    ESP_LOGI(pcTaskGetName(NULL), "Start TX");
    lora_common_data_t data_to_send;
    while(1)
    {
        if (transmitter_data_queue == NULL) {
            ESP_LOGE(pcTaskGetName(NULL), "Transmitter queue not initialized.");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if(xQueueReceive(transmitter_data_queue, &(data_to_send), (TickType_t)10)) {
            taskENTER_CRITICAL(&xTransmitMutex);
            is_transmitting = true;
            taskEXIT_CRITICAL(&xTransmitMutex);

            lora_send_packet(data_to_send.data, data_to_send.len);

            taskENTER_CRITICAL(&xTransmitMutex);
            is_transmitting = false;
            taskEXIT_CRITICAL(&xTransmitMutex);

            ESP_LOGI(pcTaskGetName(NULL), "%d byte packet sent...", data_to_send.len);

            if (CONFIG_SLEEP_ENABLED) {
                ESP_LOGI(pcTaskGetName(NULL), "Sleep enabled, going to sleep for %d", CONFIG_SLEEP_TIME_SECONDS);
                sleep_for_time(CONFIG_SLEEP_TIME_SECONDS);
            }
        }
    }
}

void lora_transmitter_start_sending(QueueHandle_t send_queue) {
    transmitter_data_queue = send_queue;
    xTaskCreate(task_tx, "task_tx", 1024*4, NULL, 5, NULL);
}

bool lora_transmitter_is_transmiting() {
    bool ret;
    taskENTER_CRITICAL(&xTransmitMutex);
    ret = is_transmitting;
    taskEXIT_CRITICAL(&xTransmitMutex);
    return ret;
}