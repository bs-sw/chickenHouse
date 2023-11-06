#include "chickenHouse.h"

const float baseClock = 200;                            //200 Millisekunden
const float level1Divider = 1000/baseClock;             //1 Sekunde
const float level2Divider = 2000/baseClock;             //2 Sekunden
const float level3Divider = 60000/baseClock;            //1 Minute

unsigned long cycleCounter;
unsigned long lastTime;

void setup()
{
  analogReference(INTERNAL);  //reference voltage for A/D converter set to 1.1V
  Error::init();
  #if !defined (NO_PERIPHERY)
  RealTimeClock::init();
  #endif
  DoorController::init();
  Lcd::init();
  #if !defined (NO_PERIPHERY)
  Logger::init();
  TemperatureSensors::init();
  Rs485::init();
  #endif
  Illumination::init();
  #if defined (NO_PERIPHERY)
  Serial.begin(115200);
  #endif

  //RealTimeClock::setTime(21, 1, 4, 15, 33);
}

void loop() { 
  if (millis() - lastTime > baseClock) {
    //Watchdog
    if (millis() - lastTime > 2 * baseClock) {
      Error::setError(ERROR_WATCHDOG_CYCLE_TIME, cycleCounter);
    }

    lastTime += baseClock;
    cycleCounter++;
    #if defined(NO_PERIPHERY) 
    Serial.println(cycleCounter); 
    #endif

    //Level0 - alle 200ms
    Illumination::process();
    DoorController::process();

    //Level1 - jede Sekunde                                             //Offset: 0
    if ((cycleCounter + 0) % ((unsigned long) level1Divider) == 0) {
      RealTimeClock::addSecond();
      Lcd::refresh();
      #if defined(NO_PERIPHERY) 
      Serial.println("Level1"); 
      #endif
    }

    //Level2 - alle 2 Sekunden                                          //Offset: 2
    if ((cycleCounter + 2)  % ((unsigned long) level2Divider) == 0) {
      Rs485::communicate();                                                                   //---------------------- temporary moved
      Error::output();
      #if defined(NO_PERIPHERY) 
      Serial.println("Level2-2"); 
      #endif
    }

    //Level3 - jede Minute                                              //Offset: 3
    if ((cycleCounter + 3)  % ((unsigned long) level3Divider) == 0) {
      RealTimeClock::refresh();
      TemperatureSensors::refreshTemperatureValues();
      TemperatureSensors::startTemperatureConversion();
      #if defined(NO_PERIPHERY) 
      Serial.println("Level3-1"); 
      #endif
    }

    //Level3 - jede Minute                                              //Offset: 4  (Achtung: 4 ist der maximale Offset. Bei 5 gibt es schon Überschneidungen mit dem Level 1.)
    if ((cycleCounter + 4) % ((unsigned long) level3Divider) == 0) {
      Logger::writeToSD();
      #if defined(NO_PERIPHERY) 
      Serial.println("Level3-2"); 
      #endif
    }
  }
}
