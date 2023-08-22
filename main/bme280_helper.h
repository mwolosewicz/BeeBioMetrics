//
// Created 22.08.2023 by pk
//
#ifndef ESP32_BME280_HELPER_H
#define ESP32_BME280_HELPER_H

#define SERIALIZED_DOUBLE_SZ 8

typedef enum {
    BME280_HELPER_STATUS_OK,
    BME280_HELPER_STATUS_ERROR,
} bme280_helper_status_t;

typedef union {
  double value;
  uint8_t serialized[SERIALIZED_DOUBLE_SZ];
} serialized_data_t;

typedef struct {
  serialized_data_t temperature;
  serialized_data_t humidity;
  serialized_data_t pressure;
} bme280_data_t;

void bme280_i2c_master_init(void);
void bme280_normal_mode_task(void *ignore);
void bme280_forced_mode_task(void *ignore);
bme280_helper_status_t bme280_get_data(bme280_data_t *p_data);



#endif //ESP32_BME280_HELPER_H

