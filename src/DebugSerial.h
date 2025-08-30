// DebugSerial - Lightweight debug Serial library

#ifndef __DEBUG_SERIAL_H__
#define __DEBUG_SERIAL_H__

#include <Arduino.h>
#include <MicroSerial.h>

#if defined(DEBUG)
#define DEBUG_SERIAL_HANDLE(tx, baudrate) MICRO_SERIAL_HANDLE(tx, baudrate)
#define DEBUG_SERIAL_BEGIN() MicroSerial_begin(__DEBUG_SERIAL_HANDLE__)
#define DEBUG_SERIAL_WAIT_FOR() delay(100)
#define DEBUG_SERIAL_PRINT(x) MicroSerial_print(__DEBUG_SERIAL_HANDLE__, x)
#define DEBUG_SERIAL_PRINTLN(x) \
  {                              \
    MicroSerial_print(__DEBUG_SERIAL_HANDLE__, x); \
    MicroSerial_println(__DEBUG_SERIAL_HANDLE__); \
  }
#define DEBUG_SERIAL_PRINTHEX(x) MicroSerial_printhex(__DEBUG_SERIAL_HANDLE__, x)
#define DEBUG_SERIAL_PRINTLNHEX(x) \
  {                          \
    MicroSerial_printhex(__DEBUG_SERIAL_HANDLE__, x); \
    MicroSerial_println(__DEBUG_SERIAL_HANDLE__); \
  }
#else
#define DEBUG_SERIAL_HANDLE(tx, baudrate)
#define DEBUG_SERIAL_BEGIN(x)
#define DEBUG_SERIAL_WAIT_FOR()
#define DEBUG_SERIAL_PRINT(x)
#define DEBUG_SERIAL_PRINTLN(x)
#define DEBUG_SERIAL_PRINTHEX(x)
#define DEBUG_SERIAL_PRINTLNHEX(x)
#endif

#endif // __DEBUG_SERIAL_H__
