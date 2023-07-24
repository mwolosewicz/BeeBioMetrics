//
// Created by mw on 25.06.23.
//

#include "lora_receiver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "lora_common.h"
#include "lora.h"

static QueueHandle_t receiver_data_queue;

static void task_rx(void *pvParameters)
{
    ESP_LOGI(pcTaskGetName(NULL), "Start RX");
    lora_common_data_t rcvd_data;

    while(1) {
        lora_receive(); // put into receive mode
        if (receiver_data_queue == NULL) {
            ESP_LOGE(pcTaskGetName(NULL), "Receiver queue not initialized.");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        if (lora_received()) {
            rcvd_data.len = lora_receive_packet(rcvd_data.data, sizeof(rcvd_data.data));
            ESP_LOGD(pcTaskGetName(NULL), "%d byte packet received:[%.*s]", rcvd_data.len, rcvd_data.len, rcvd_data.data);

            if (xQueueSend(receiver_data_queue, &rcvd_data, 0) != pdTRUE) {
                ESP_LOGE(pcTaskGetName(NULL), "Cannot send received data to queue.");
            }
        }
        vTaskDelay(1); // Avoid WatchDog alerts
    }
}

void lora_receiver_start_receiving(QueueHandle_t queue) {
    receiver_data_queue = queue;
    xTaskCreate(task_rx, "task_rx", 1024*4, NULL, 5, NULL);
}
