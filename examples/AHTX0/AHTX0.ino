// AHTX0 - Temperature and Humidity Sensor Example

#include <Arduino.h>
#include "MicroWire.h"
#include "DebugSerial.h"

#define __DEBUG_SERIAL_HANDLE__ DEBUG_SERIAL_HANDLE(PB0, 9600)
#define I2C MICRO_WIRE_HANDLE(PB2, PB3)

#define AHTX0_I2C_ADDRESS 0x38
#define AHTX0_CMD_SOFTRESET 0xBA
#define AHTX0_CMD_CALIBRATE 0xBE
#define AHTX0_CMD_TRIGGER 0xAC

uint8_t data[8];

void setup() {
  DEBUG_SERIAL_BEGIN();
  DEBUG_SERIAL_WAIT_FOR();
  DEBUG_SERIAL_PRINTLN("");
  DEBUG_SERIAL_PRINTLN("--");

  MicroWire_begin(I2C);
  delay(100);

  ahtX0_write_cmd(AHTX0_CMD_SOFTRESET, 0x00, 0x00);
  delay(20);
  ahtX0_write_cmd(AHTX0_CMD_CALIBRATE, 0x08, 0x00);
  delay(10);

  DEBUG_SERIAL_PRINTLN("AHTX0");
}

void loop() {
  ahtX0_write_cmd(AHTX0_CMD_TRIGGER, 0x33, 0x00);
  delay(80);

  if (ahtX0_read_data(data)) {
    float temperature, humidity;
    ahtX0_calc_temp_hum(data, &temperature, &humidity);
    DEBUG_SERIAL_PRINT(temperature);
    DEBUG_SERIAL_PRINT("C ");
    DEBUG_SERIAL_PRINT(humidity);
    DEBUG_SERIAL_PRINTLN("%");
  } else {
    DEBUG_SERIAL_PRINTLN("Fail to read data");
  }

  delay(3 * 1000);
}

void ahtX0_write_cmd(uint8_t cmd, uint8_t data1, uint8_t data2) {
  MicroWire_beginTransmission(I2C, AHTX0_I2C_ADDRESS);
  MicroWire_write(I2C, cmd);
  MicroWire_write(I2C, data1);
  MicroWire_write(I2C, data2);
  MicroWire_endTransmission(I2C);
}

bool ahtX0_read_data(uint8_t data[6]) {
  MicroWire_requestFrom(I2C, AHTX0_I2C_ADDRESS, 6);
  for (uint8_t i = 0; i < 6; i++) {
    MicroWire_read(I2C, (i < 5) ? true : false, &data[i]);
  }
  return ((data[0] & 0x80) == 0) && !(data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF);
}

void ahtX0_calc_temp_hum(uint8_t* data, float* temperature, float* humidity) {
  uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
  *temperature = ((float)raw_temp / 1048576.0) * 200.0 - 50.0;

  uint32_t raw_hum = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
  *humidity = ((float)raw_hum * 100) / 1048576.0;
}
