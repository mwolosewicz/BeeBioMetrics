//
// Created 22.08.2023 by pk
//

// System includes
#include "driver/i2c.h"
#include "esp_log.h"

// Project includes
#include "bme280.h"
#include "bme280_helper.h"

// Original driver setup gpio
#define SDA_PIN GPIO_NUM_0
#define SCL_PIN GPIO_NUM_4

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

#define FAIL    ((s8)-1)

static double bme280_pressure;
static double bme280_temperature;
static double bme280_humidity;

const static char *TAG = "BME280";


static s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BME280_INIT_VALUE;

	esp_err_t espRc;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(cmd, reg_addr, true);
	i2c_master_write(cmd, reg_data, cnt, true);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_NUM_1, cmd, 10/portTICK_PERIOD_MS);
	if (espRc == ESP_OK) {
		iError = SUCCESS;
	} else {
		iError = FAIL;
	}
	i2c_cmd_link_delete(cmd);

	return (s8)iError;
}

static s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BME280_INIT_VALUE;
	esp_err_t espRc;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);

	if (cnt > 1) {
		i2c_master_read(cmd, reg_data, cnt-1, I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd, reg_data+cnt-1, I2C_MASTER_NACK);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_NUM_1, cmd, 10/portTICK_PERIOD_MS);
	if (espRc == ESP_OK) {
		iError = SUCCESS;
	} else {
		iError = FAIL;
	}

	i2c_cmd_link_delete(cmd);

	return (s8)iError;
}

static void BME280_delay_msek(u32 msek)
{
	vTaskDelay(msek/portTICK_PERIOD_MS);
}

void bme280_i2c_master_init(void)
{
	i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = SDA_PIN,
		.scl_io_num = SCL_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 1000000
	};
	i2c_param_config(I2C_NUM_1, &i2c_config);
	i2c_driver_install(I2C_NUM_1, I2C_MODE_MASTER, 0, 0, 0);
}

void bme280_normal_mode_task(void *ignore)
{
	struct bme280_t bme280 = {
		.bus_write = BME280_I2C_bus_write,
		.bus_read = BME280_I2C_bus_read,
		.dev_addr = BME280_I2C_ADDRESS2,
		.delay_msec = BME280_delay_msek
	};

	s32 com_rslt;
	s32 v_uncomp_pressure_s32;
	s32 v_uncomp_temperature_s32;
	s32 v_uncomp_humidity_s32;

	com_rslt = bme280_init(&bme280);

	com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
	com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_2X);
	com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);

	com_rslt += bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
	com_rslt += bme280_set_filter(BME280_FILTER_COEFF_16);

	com_rslt += bme280_set_power_mode(BME280_NORMAL_MODE);
	if (com_rslt == SUCCESS) {
		while(true) {
		  	vTaskDelay(pdMS_TO_TICKS(1000));
			com_rslt = bme280_read_uncomp_pressure_temperature_humidity(
				&v_uncomp_pressure_s32, &v_uncomp_temperature_s32, &v_uncomp_humidity_s32);

			if (com_rslt == SUCCESS) {
			  bme280_temperature = bme280_compensate_temperature_double(v_uncomp_temperature_s32);
			  bme280_pressure = bme280_compensate_pressure_double(v_uncomp_pressure_s32)/100;
			  bme280_humidity = bme280_compensate_humidity_double(v_uncomp_humidity_s32);

			  ESP_LOGI(TAG, "%.1f degC / %.1f hPa / %.1f %%",
				   bme280_temperature,
				   bme280_pressure,
				   bme280_humidity);
			} else {
				ESP_LOGE(TAG, "measure error. code: %d", com_rslt);
			}
		}
	} else {
		ESP_LOGE(TAG, "init or setting error. code: %d", com_rslt);
	}

	vTaskDelete(NULL);
}

void bme280_forced_mode_task(void *ignore)
{
	struct bme280_t bme280 = {
		.bus_write = BME280_I2C_bus_write,
		.bus_read = BME280_I2C_bus_read,
		.dev_addr = BME280_I2C_ADDRESS2,
		.delay_msec = BME280_delay_msek
	};

	s32 com_rslt;
	s32 v_uncomp_pressure_s32;
	s32 v_uncomp_temperature_s32;
	s32 v_uncomp_humidity_s32;
	
	com_rslt = bme280_init(&bme280);

	com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_1X);
	com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_1X);
	com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);

	com_rslt += bme280_set_filter(BME280_FILTER_COEFF_OFF);
	if (com_rslt == SUCCESS) {
		while(true) {
			com_rslt = bme280_get_forced_uncomp_pressure_temperature_humidity(
				&v_uncomp_pressure_s32, &v_uncomp_temperature_s32, &v_uncomp_humidity_s32);

			if (com_rslt == SUCCESS) {
			  ESP_LOGI(TAG, "%.2f degC / %.3f hPa / %.3f %%",
				   bme280_compensate_temperature_double(v_uncomp_temperature_s32),
				   bme280_compensate_pressure_double(v_uncomp_pressure_s32)/100, // Pa -> hPa
				   bme280_compensate_humidity_double(v_uncomp_humidity_s32));
			} else {
				ESP_LOGE(TAG, "measure error. code: %d", com_rslt);
			}
		}
	} else {
		ESP_LOGE(TAG, "init or setting error. code: %d", com_rslt);
	}

	vTaskDelete(NULL);
}

bme280_helper_status_t bme280_get_data(bme280_data_t *p_data)
{
  bme280_helper_status_t ret = BME280_HELPER_STATUS_ERROR;
  
  if (p_data != NULL) {
    p_data->temperature.value = bme280_temperature;
    p_data->humidity.value = bme280_humidity;
    p_data->pressure.value = bme280_pressure;
    ret = BME280_HELPER_STATUS_OK;
  }

  return ret;
}

