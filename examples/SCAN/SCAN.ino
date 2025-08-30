// SCAN - I2C Device Scanner Example

#include <Arduino.h>
#include "MicroWire.h"
#include "DebugSerial.h"

#define __DEBUG_SERIAL_HANDLE__ DEBUG_SERIAL_HANDLE(PB0, 9600)
#define I2C MICRO_WIRE_HANDLE(PB2, PB3)

uint8_t data[16];

void setup() {
  DEBUG_SERIAL_BEGIN();
  DEBUG_SERIAL_WAIT_FOR();
  DEBUG_SERIAL_PRINTLN("");
  DEBUG_SERIAL_PRINTLN("--");

  MicroWire_begin(I2C);
  delay(100);

  DEBUG_SERIAL_PRINTLN("SCAN");
}

void loop() {
  DEBUG_SERIAL_PRINTLN("--");

  for (uint8_t address = 1; address < 127; address++) {
    if (MicroWire_beginTransmission(I2C, address)) {
      DEBUG_SERIAL_PRINT("Found I2C device at 0x");
      DEBUG_SERIAL_PRINTLNHEX(address);
      MicroWire_endTransmission(I2C);
    }
    delay(10);
  }

  DEBUG_SERIAL_PRINTLN("");
  delay(3 * 1000);
}
