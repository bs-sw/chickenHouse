#include "lcd.h"

LiquidCrystal_I2C Lcd::llcd(LCD_ADDRESS, 16, 4);

char Lcd::timeString[9];
char Lcd::dateString[11];
char Lcd::line[4][17];

void Lcd::init() {
  llcd.init();
}

void Lcd::backlight(bool state) {
  if (state)
    llcd.backlight();
  else
    llcd.noBacklight();
}

// ==================
// |21.01.2021 22:16|
// |R 07:30  S 22:20|
// |O:-10.4  I: 50.1|
// |D:00     E:  100|
// ==================

void Lcd::refresh() {
  RealTimeClock::getDateTimeString(line[0], true);
  line[0][10] = ' ';
  line[0][16] = '\0';

  line[1][ 0] = 'R';
  line[1][ 1] = ' ';
  sprintf_P(line[1] + 2, PSTR("%02i:%02i"), ((int) (RealTimeClock::getSunrise()/60)), RealTimeClock::getSunrise()%60); 
  line[1][ 7] = ' ';
  line[1][ 8] = ' ';
  line[1][ 9] = 'S';
  line[1][10] = ' ';
  sprintf_P(line[1] + 11, PSTR("%02i:%02i"), RealTimeClock::getSunset()/60, RealTimeClock::getSunset()%60); 

  line[2][ 0] = 'O';
  line[2][ 1] = ':';
  dtostrf(TemperatureSensors::getValueOut(), 5, 1, line[2] + 2);
  line[2][ 7] = ' ';
  line[2][ 8] = ' ';
  line[2][ 9] = 'I';
  line[2][10] = ':';
  dtostrf(TemperatureSensors::getValueIn(), 5, 1, line[2] + 11);
  
  line[3][ 0] = 'D';
  line[3][ 1] = ':';
  sprintf_P(line[3] + 2, PSTR("%02i"), DoorController::getState()); 
  line[3][ 4] = ' ';
  line[3][ 5] = ' ';
  line[3][ 6] = ' ';
  line[3][ 7] = ' ';
  line[3][ 8] = ' ';
  line[3][ 9] = 'E';
  line[3][10] = ':';  
  sprintf_P(line[3] + 11, PSTR("%5lX"), Error::getError()); 

  for (int i = 0; i < 4; i++) {
    llcd.setCursor(0, i);
    llcd.print(line[i]);
  }

#ifdef NO_PERIPHERY
  Serial.println("==================");
  for (int i = 0; i < 4; i++) {
    Serial.println(line[i]);
  }
  Serial.println("==================");
#endif
}
