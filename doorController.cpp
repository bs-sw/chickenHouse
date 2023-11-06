#include "doorController.h"

Servo DoorController::doorServo;

bool DoorController::pushButtonStateSaved;
bool DoorController::isDaySaved;
long DoorController::startTime;
bool DoorController::endSwitchClosedStateReached;
long DoorController::endSwitchClosedStateReachedTime;
bool DoorController::endSwitchOpenedState;
bool DoorController::endSwitchClosedState;
byte DoorController::action;
byte DoorController::nextAction;
byte DoorController::endSwitch;
byte DoorController::endSwitchSaved;
bool DoorController::postponeOpening;

void DoorController::init() {
  stopDoor();
  pinMode(DOOR_PUSH_BUTTON_PIN, INPUT);
  pinMode(DOOR_END_SWITCH_OPENED_PIN, INPUT);
  pinMode(DOOR_END_SWITCH_CLOSED_PIN, INPUT);  
  isDaySaved = RealTimeClock::isDayLight();
  endSwitchSaved = STATE_NONE;
  nextAction = STATE_CLOSE;
  if (!digitalRead(DOOR_END_SWITCH_OPENED_PIN)) {
    endSwitchSaved = STATE_OPEN;
    nextAction = STATE_CLOSE;
  }
  if (!digitalRead(DOOR_END_SWITCH_CLOSED_PIN)) {
    endSwitchSaved = STATE_CLOSE;
    nextAction = STATE_OPEN;
    endSwitchClosedStateReached = true;
    endSwitchClosedStateReachedTime = millis() - DELAY_CLOSED_STATE;    
  }
}

void DoorController::process(){
  bool pushButtonState = digitalRead(DOOR_PUSH_BUTTON_PIN);
  bool endSwitchOpenedState = !digitalRead(DOOR_END_SWITCH_OPENED_PIN);
  bool endSwitchClosedState= !digitalRead(DOOR_END_SWITCH_CLOSED_PIN);

  if (endSwitchOpenedState && endSwitchClosedState) {
    Error::setError(ERROR_IMPLAUSIBLE_END_SWITCH);
    stopDoor();
    return;
  }
  
  endSwitch = STATE_NONE;

  if (endSwitchOpenedState) endSwitch = STATE_OPEN;
  if (endSwitchClosedState) {
    if (!endSwitchClosedStateReached) {
      endSwitchClosedStateReachedTime = millis();
      endSwitchClosedStateReached = true;
    }
    if (endSwitchClosedStateReached && (millis() - endSwitchClosedStateReachedTime > DELAY_CLOSED_STATE)) {
      endSwitch = STATE_CLOSE;
    }
  } else {
    endSwitchClosedStateReached = false;
  }

  if (endSwitchSaved != endSwitch) {
    endSwitchSaved = endSwitch;
    if (!action) {
      Error::setError(ERROR_IMPLAUSIBLE_END_SWITCH);
    }
    if (action != endSwitch && endSwitch) {
       Error::setError(ERROR_IMPLAUSIBLE_END_SWITCH);
       stopDoor();
    }
  }
  
  if (action != STATE_NONE) {
    if (millis() - startTime > MAX_DOOR_TIME) {
      Error::setError(ERROR_MAX_DOOR_TIME_EXCEEDED);
      stopDoor();
      return;
    }
    if (action == endSwitch) {
      stopDoor();
      return;
    }
  }

  if (pushButtonStateSaved != pushButtonState) {
    pushButtonStateSaved = pushButtonState;
    if (pushButtonState) {
      resetDoorErrors();
      if (action) {
        stopDoor();
        return;
      }
      if (nextAction == STATE_OPEN) {
        openDoor(); 
        return;
      } 
      if (nextAction == STATE_CLOSE) {
        closeDoor();
        return;
      }
    }
  }

  if (isDaySaved != RealTimeClock::isDayLight()) {
    isDaySaved = RealTimeClock::isDayLight();
    if (!isDaySaved && endSwitch != STATE_CLOSE) {
      closeDoor();
      return;
    }
    if (isDaySaved && endSwitch != STATE_OPEN) {
      if (!TemperatureSensors::isColdOutside()) {
        openDoor();
        return;
      } else {
        postponeOpening = true;
      }
    }
  }
  
  if (isDaySaved && endSwitch != STATE_OPEN && postponeOpening && RealTimeClock::laterOpeningTimeReached()) {
    postponeOpening = false;
    openDoor();
    return;
  }
}

void DoorController::stopDoor() {
  doorServo.detach();
  if (action == STATE_OPEN) nextAction = STATE_CLOSE;
  else nextAction = STATE_OPEN;
  action = STATE_NONE;
}

void DoorController::openDoor() {
  doorServo.attach(DOOR_SERVO_PIN);
  doorServo.write(DOOR_DOWNWARDS);
  action = STATE_OPEN;
  startTime = millis();
}

void DoorController::closeDoor() {
  doorServo.attach(DOOR_SERVO_PIN);
  doorServo.write(DOOR_UPWARDS);
  action = STATE_CLOSE;
  startTime = millis();
}

void DoorController::extOpenDoor() {
  resetDoorErrors();
  openDoor();
}

void DoorController::extCloseDoor() {
  resetDoorErrors();
  closeDoor();
}

void DoorController::extStopDoor() {
  resetDoorErrors();
  stopDoor();
}

void DoorController::resetDoorErrors() {
  Error::resetError(ERROR_IMPLAUSIBLE_END_SWITCH | ERROR_MAX_DOOR_TIME_EXCEEDED);
}

byte DoorController::getState() {
  return action * 10 + endSwitch;
}
