#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/********************************************\
|*  Pin Definitions
\********************************************/

const int LED_PIN = 5;

const int I2C_SCL_PIN = 22;
const int I2C_SDA_PIN = 21;

const int UART1_RXD_PIN = 16;
const int UART1_TXD_PIN = 17;

const int GRBL_SERIAL_SPEED = 115200;


/* ============================================== *\
 * Debug
\* ============================================== */

#define SERIAL_DEBUG 1
#define SERIAL_SPEED 115200

#if SERIAL_DEBUG < 1
#define DEBUG_println(...) 
#define DEBUG_print(...) 
#define DEBUG_printf(...) 
#else
#define DEBUG_println(...) if (debugStream) {debugStream->println(__VA_ARGS__);}
#define DEBUG_print(...) if (debugStream) {debugStream->print(__VA_ARGS__);}
#define DEBUG_printf(...) if (debugStream) {debugStream->printf(__VA_ARGS__);}
#endif

#define FST (const char *)F
#define PM (const __FlashStringHelper *)


/* ============================================== *\
 * Constants
\* ============================================== */

const char HOSTNAME[] PROGMEM = "grbl-pad";
#define HTTP_PORT 80

const char version_number[] PROGMEM = "0.2";
const char version_date[] PROGMEM = __DATE__;
const char version_time[] PROGMEM = __TIME__;

extern Stream* debugStream;

#define ENABLE_GAME_PAD 1

#define USE_GRBL_SERIAL 1

#define MAX_GRBL_CMD 32


#endif
