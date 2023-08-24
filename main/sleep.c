//
// Created by mw on 07.08.23.
//

#include "esp_log.h"
#include "esp_sleep.h"

#include "sleep.h"

const static char *TAG = "MAIN";

void sleep_for_time(int seconds)
{
    ESP_LOGI(TAG, "Going to sleep, for: %d seconds\n", seconds);
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(seconds * 1000000));
    esp_light_sleep_start();
}
