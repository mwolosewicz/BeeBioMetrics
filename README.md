# BeeBioMetrics

BeeEnviroMonitor is an embedded device project aimed at monitoring the environmental parameters within a bee hive. Utilizing LoRa communication and the ESP32 platform, this device measures temperature, humidity, and pressure, providing essential insights to beekeepers about the conditions within their hives.

## Features

- Temperature monitoring
- Humidity monitoring
- Pressure monitoring
- Long-range, low-power LoRa communication

### Deep sleep after TX
The device offers a feature that allows it to enter a deep sleep mode after each transmission (TX) to conserve battery life.
This sleep feature can be activated and configured via the menuconfig. Navigate to the following path in the menu: `Application Configuration -> Enable Sleep After Send`.

### BME280 readout
This device (as a sender) can be connected with BME280 and read measured temperature, humidity, pressure. Data is read from I2C and transmitted via LoRa.

## Hardware Requirements

- ESP32 Microcontroller
- LoRa Module
- BMP280 Pressure and Temperature Sensor

### Low power
As of now the power consumption is as following:
- ESP32+LoRa modem 2mA total (about 2 months on INR18650)
- ES32+LoRa(no VCC) 132uA total (2-4 years INR18650)

LoRa modem can be set to low power taking 0.1uA.

## Software Requirements

## Message description
8[B] - MAC address.
2[B] - Message counter. Each transmitted message has an unique ID.
1[B] - Message type. At the moment only one message type is supported. Readout of BME280.
8[B] - Temperature in degC as a double.
8[B] - Humidity in % as a double.
8[B] - Pressure in hPa as a double.

## Installation and Setup

1. **Setting up the hardware:** Connect your BMP280 sensor and LoRa module to the ESP32 following the schematic provided in the 'schematics' directory of this repository.

2. **Software setup:** Install the ESP-IDF and setup the ESP32 development environment. Install the required libraries from the 'Software Requirements' section above.

3. **Compile and upload the code:** Open the 'BeeBioMetrics' project in your ESP-IDF environment, compile and upload it to your ESP32.

## Contributing

We welcome contributions from the community. Please read the contributing guidelines before getting started.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

