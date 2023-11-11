#ifndef __lcd__
#define __lcd__

#include "chickenhouse.h"
#include <Arduino.h>
#include <Wire.h> 
#include "LiquidCrystal_I2C.h"
#include "temperatureSensors.h"
#include "realTimeClock.h"
#include "error.h"

#define LCD_ADDRESS 0x3F

class Lcd {
private:
  static LiquidCrystal_I2C llcd;
  static char timeString[];
  static char dateString[];
  static char line[][17];
public:
  static void init();
  static void backlight(bool state);
  static void refresh();
};

#endif
