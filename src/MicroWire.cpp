// MicroWire - Lightweight bit-bang I2C Master library

/*
 * MIT License
 *
 * Copyright (c) 2025 Hiroaki SHIBUKI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if defined(ARDUINO)
#include <Arduino.h>
#if defined(__AVR__)
#include <avr/io.h>
#include <util/delay.h>
#endif
#else
#include <stdio.h>
#endif

// Get I2C SDA pin number
#define MICRO_WIRE_SDA(bus) (0x0f & (bus >> 4))

// Get I2C SCL pin number
#define MICRO_WIRE_SCL(bus) (0x0f & bus)

#include "MicroWire.h"

// Timing definitions
#if defined(ARDUINO)
#if defined(__AVR__)
#define MICRO_WIRE_DELAY() _delay_us(5)
#else
#define MICRO_WIRE_DELAY() delayMicroseconds(5)
#endif
#else
#define MICRO_WIRE_DELAY()
#endif

// Initialize I2C pins
void MicroWire_begin(uint8_t handle) {
  uint8_t sda = MICRO_WIRE_SDA(handle);
  uint8_t scl = MICRO_WIRE_SCL(handle);
  MicroWire_sda_high(sda);
  MicroWire_scl_high_wait(scl);
}

// Begin I2C transmission to address, returns true if ACK received
bool MicroWire_beginTransmission(uint8_t handle, uint8_t address) {
  MicroWire_start(handle);
  return MicroWire_write(handle, address << 1);
}

// End I2C transmission
void MicroWire_endTransmission(uint8_t handle) {
  MicroWire_stop(handle);
}

// Request specified quantity of bytes from I2C device, returns true if ACK received
bool MicroWire_requestFrom(uint8_t handle, uint8_t address, uint8_t quantity) {
  MicroWire_start(handle);
  return MicroWire_write(handle, (address << 1) | 1);
}

// Read data byte from I2C device, ack=true sends ACK, ack=false sends NACK
void MicroWire_read(uint8_t handle, bool ack, uint8_t* data) {
  uint8_t sda = MICRO_WIRE_SDA(handle);
  uint8_t scl = MICRO_WIRE_SCL(handle);
  *data = 0;
  MicroWire_sda_high(sda);
  for (uint8_t i = 0; i < 8; i++) {
    MicroWire_scl_low(scl);
    MICRO_WIRE_DELAY();
    MicroWire_scl_high_wait(scl);
    MICRO_WIRE_DELAY();
    *data <<= 1;
    if (MicroWire_sda_read(sda)) { *data |= 1; }
  }
  MicroWire_scl_low(scl);
  if (ack) {
    MicroWire_sda_low(sda);
  } else {
    MicroWire_sda_high(sda);
  }
  MICRO_WIRE_DELAY();
  MicroWire_scl_high_wait(scl);
  MICRO_WIRE_DELAY();
  MicroWire_scl_low(scl);
  MICRO_WIRE_DELAY();
  MicroWire_sda_high(sda);
}

// Write data byte, returns true if ACK received
bool MicroWire_write(uint8_t handle, uint8_t data) {
  uint8_t sda = MICRO_WIRE_SDA(handle);
  uint8_t scl = MICRO_WIRE_SCL(handle);
  for (uint8_t i = 0; i < 8; i++) {
    MicroWire_scl_low(scl);
    if (data & 0x80) {
      MicroWire_sda_high(sda);
    } else {
      MicroWire_sda_low(sda);
    }
    MICRO_WIRE_DELAY();
    MicroWire_scl_high_wait(scl);
    MICRO_WIRE_DELAY();
    data <<= 1;
  }
  MicroWire_scl_low(scl);
  MicroWire_sda_high(sda);
  MICRO_WIRE_DELAY();
  MicroWire_scl_high_wait(scl);
  MICRO_WIRE_DELAY();
  bool ack = !MicroWire_sda_read(sda);
  MicroWire_scl_low(scl);
  MICRO_WIRE_DELAY();
  return ack;
}

// Generate I2C start condition
void MicroWire_start(uint8_t handle) {
  uint8_t sda = MICRO_WIRE_SDA(handle);
  uint8_t scl = MICRO_WIRE_SCL(handle);
  MicroWire_sda_high(sda);
  MICRO_WIRE_DELAY();
  MicroWire_scl_high_wait(scl);
  MICRO_WIRE_DELAY();
  MicroWire_sda_low(sda);
  MICRO_WIRE_DELAY();
  MicroWire_scl_low(scl);
  MICRO_WIRE_DELAY();
}

// Generate I2C stop condition
void MicroWire_stop(uint8_t handle) {
  uint8_t sda = MICRO_WIRE_SDA(handle);
  uint8_t scl = MICRO_WIRE_SCL(handle);
  MicroWire_scl_low(scl);
  MICRO_WIRE_DELAY();
  MicroWire_sda_low(sda);
  MICRO_WIRE_DELAY();
  MicroWire_scl_high_wait(scl);
  MICRO_WIRE_DELAY();
  MicroWire_sda_high(sda);
  MICRO_WIRE_DELAY();
}

// Set I2C SDA pin high
void MicroWire_sda_high(uint8_t sda) {
  #if defined(MICRO_WIRE_AVR)
  DDRB &= ~_BV(sda);
  PORTB |= _BV(sda);
  #else
  pinMode(sda, INPUT_PULLUP);
  #endif
}

// Set I2C SDA pin low
void MicroWire_sda_low(uint8_t sda) {
  #if defined(MICRO_WIRE_AVR)
  DDRB |= _BV(sda);
  PORTB &= ~_BV(sda);
  #else
  pinMode(sda, OUTPUT);
  digitalWrite(sda, LOW);
  #endif
}

// Read I2C SDA pin state
bool MicroWire_sda_read(uint8_t sda) {
  #if defined(MICRO_WIRE_AVR)
  return (PINB & _BV(sda)) != 0;
  #else
  return digitalRead(sda) == HIGH;
  #endif
}

// Set I2C SCL pin high
void MicroWire_scl_high(uint8_t scl) {
  #if defined(MICRO_WIRE_AVR)
  DDRB &= ~_BV(scl);
  PORTB |= _BV(scl);
  #else
  pinMode(scl, INPUT_PULLUP);
  #endif
}

// Set I2C SCL pin low
void MicroWire_scl_low(uint8_t scl) {
  #if defined(MICRO_WIRE_AVR)
  DDRB |= _BV(scl);
  PORTB &= ~_BV(scl);
  #else
  pinMode(scl, OUTPUT);
  digitalWrite(scl, LOW);
  #endif
}

// Read I2C SCL pin state
bool MicroWire_scl_read(uint8_t scl) {
  #if defined(MICRO_WIRE_AVR)
  return (PINB & _BV(scl)) != 0;
  #else
  return digitalRead(scl) == HIGH;
  #endif
}

// Set I2C SCL pin high and wait for clock stretching
void MicroWire_scl_high_wait(uint8_t scl) {
  #if defined(MICRO_WIRE_AVR)
  DDRB &= ~_BV(scl);
  PORTB |= _BV(scl);
  while (!MicroWire_scl_read(scl));
  #else
  pinMode(scl, INPUT_PULLUP);
  while (!MicroWire_scl_read(scl));
  #endif
}
