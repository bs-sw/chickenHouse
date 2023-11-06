#include "illumination.h"

bool Illumination::illuminationOn;
unsigned long Illumination::switchOnTime;
bool Illumination::pushButtonStateSaved;
bool Illumination::dayExtensionActiveStateSaved;
bool Illumination::backlightIsOn;

void Illumination::init() {
  switchOff();
  pinMode(ILLUMINATION_RELAY_PIN, OUTPUT);
  pinMode(ILLUMINATION_SWITCH_PIN, INPUT);
}

bool Illumination::dayExtensionActive() {
  if (RealTimeClock::getDayLength() < MINIMUM_DAY_TIME) {
    if (RealTimeClock::getMinutesSinceMidnight() < RealTimeClock::getSunrise() 
      && RealTimeClock::getMinutesSinceMidnight() >= (RealTimeClock::getSunset() - MINIMUM_DAY_TIME)) 
      return true;
  }
  return false;
}

void Illumination::process(){
  bool pushButtonState = digitalRead(ILLUMINATION_SWITCH_PIN);
  bool dayExtensionActiveState = dayExtensionActive();

  if (pushButtonStateSaved != pushButtonState) {
    pushButtonStateSaved = pushButtonState;
    if (pushButtonState) {
      if (!illuminationOn) {
        switchOn();
        Lcd::backlight(true);
        backlightIsOn = true;
        return;
      } else {
        switchOff();
        Lcd::backlight(false);
        backlightIsOn = false;
        return;
      }
    }
  }

  if (dayExtensionActiveStateSaved != dayExtensionActiveState) {   
    dayExtensionActiveStateSaved = dayExtensionActiveState;
    if (dayExtensionActiveStateSaved) {
      switchOn();
      return;
    } else {
      switchOff();
      return;
    }
  }

  if ((millis() - switchOnTime > AUTOMATIC_SWITCH_OFF_TIME)) {
    if (backlightIsOn) {
      Lcd::backlight(false);
      backlightIsOn = false;
    }
    if (!dayExtensionActiveState) {
      switchOff();
      return;
    }
  }
}

void Illumination::switchOn() {
  digitalWrite(ILLUMINATION_RELAY_PIN, LOW);
  switchOnTime = millis();
  illuminationOn = true;
}

void Illumination::switchOff() {
  digitalWrite(ILLUMINATION_RELAY_PIN, HIGH);
  illuminationOn = false;
}

byte Illumination::getState() {
  if (illuminationOn) return 1;
	else return 0;
}
