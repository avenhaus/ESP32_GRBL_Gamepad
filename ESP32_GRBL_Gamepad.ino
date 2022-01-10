
#include <Arduino.h>
#include "HardwareSerial.h"
#include "math.h"

#include "Config.h"
#include "GRBL.h"
#include "GamePad.h"

Stream* debugStream = &Serial;

void setup() {
  Serial.begin(SERIAL_SPEED);
  
  DEBUG_printf(FST("\n\nGRBL PAD %s | %s | %s\n"), version_number, version_date, version_time);
  DEBUG_printf(FST("Compiled with ESP32 SDK:%s\n\n"), ESP.getSdkVersion());

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); 

  grbl_init();
  game_pad_init();
}


void loop() {
  grbl_run();
}
