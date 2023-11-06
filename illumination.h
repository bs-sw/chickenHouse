#ifndef __illumination__
#define __illumination__

#define MINIMUM_DAY_TIME 720
#define AUTOMATIC_SWITCH_OFF_TIME 120000

#include "chickenHouse.h"
#include <Arduino.h>
#include "realTimeClock.h"
#include "error.h"
#include "lcd.h"

class Illumination {
private:
  static bool illuminationOn;
  static bool pushButtonStateSaved;
  static bool dayExtensionActiveStateSaved;
  static bool dayExtensionActive();
  static unsigned long switchOnTime;
  static bool backlightIsOn;
public:
  static void init();
  static void process();
  static byte getState();
  static void switchOn();
  static void switchOff();
};

#endif
