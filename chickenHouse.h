#ifndef __chickenHouse__
#define __chickenHouse__

//#define TEMPERATURES_SIMULATED
//#define DEBUG
//#define NO_PERIPHERY
//#define NO_CRC_CHECK

#define ERROR_OUTPUT_PIN 2
#define OUTPUT_SEND_ENABLE_PIN 3
#define ILLUMINATION_SWITCH_PIN 4
#define DOOR_SERVO_PIN 5
#define ONEWIRE_PIN 6
#define DOOR_PUSH_BUTTON_PIN 7
#define SPI_CHIP_SELECT_SD_PIN 8
#define ILLUMINATION_RELAY_PIN 10

#define DOOR_END_SWITCH_OPENED_PIN A3
#define DOOR_END_SWITCH_CLOSED_PIN A2

#include "rs485.h"
#include "temperatureSensors.h"
#include "doorController.h"
#include "lcd.h"
#include "realTimeClock.h"
#include "illumination.h"
#include "logger.h"

#endif
