#ifndef __doorController__
#define __doorController__

#define MAX_DOOR_TIME 12000
#define DOOR_UPWARDS 0
#define DOOR_DOWNWARDS 180
#define EARLIEST_OPENING_TIME 450     //minutes since midnight
#define STATE_NONE 0
#define STATE_OPEN 1
#define STATE_CLOSE 2
#define DELAY_CLOSED_STATE 2000

#include "chickenHouse.h"
#include <Arduino.h>
#include "temperatureSensors.h"
#include "realTimeClock.h"
#include "error.h"
#include <Servo.h>

class DoorController {
private:
  static Servo doorServo;
  static bool pushButtonStateSaved;
  static bool isDaySaved;
  static long startTime;
  static long endSwitchClosedStateReachedTime;
  static bool monitoring;
  static void openDoor();
  static void closeDoor();
  static void stopDoor();  
  static void resetDoorErrors();
  static bool endSwitchOpenedState;
  static bool endSwitchClosedState;
  static byte action;
  static byte nextAction;
  static byte endSwitch;
  static byte endSwitchSaved;
  static bool postponeOpening;
  static bool endSwitchClosedStateReached;
public:
  static void init();
  static void process();
  static byte getState();
  static void extOpenDoor();
  static void extCloseDoor();
  static void extStopDoor();  
};

#endif
