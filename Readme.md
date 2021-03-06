# ESP32 Game Controller GRBL jogger

## About The Project

Use a Bluetooth PS3 game controller to jog/control a GRBL based CNC like [Grbl_Esp32](https://github.com/bdring/Grbl_Esp32) or [FluidCNC](https://github.com/bdring/FluidNC).

With Arduino the ESP32 will crash if Bluetooth and WIFI are connected at the same time. This ESP32 connects to a Bluetooth game controller and then sends out jog commands etc. over the serial port.

The USB UART pins of the target GRBL can be shared on ESP32 boards like Lolin32. E.g. connect to GPIO_1 and GPIO_3 of ESP32 running GRBL.

## Controls

| Button | GRBL |
| ------ | ---- |
| Right Joy X  |  Jog X axis |
| Right Joy Y  |  Jog Y axis |
| Left Joy Y  |  Jog Z axis |
| L1 | Slow feed for joysticks |
| L2 | Fast feed for joysticks |
| start  |  START|
| select | HOLD |
| contr  | ALARM_RESET |
| R1 + circle | HOME |
| R1 + square | HOME_X |
| R1 + triangle | HOME_Y |
| R1 + cross | HOME_Z |
| circle | ZERO_ALL |
| square | ZERO_X |
| triangle | ZERO_Y |
| cross | ZERO_Z |

## Hardware Setup

* Connect Jogger RXD GPIO_16 to GRBL ESP32 GPIO_1 (USB TXD)
* Connect Jogger TXD GPIO_17 to GRBL ESP32 GPIO_3 (USB RXD)
* Connect common ground
* Connect common +5V or 3.3V

## Libraries

Needs [jvpernis/esp32-ps3](https://github.com/jvpernis/esp32-ps3) library.
