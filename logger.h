#ifndef __logger__
#define __logger__

#include "chickenhouse.h"
#include <SPI.h>
#include <SD.h>
#include "error.h"
#include "temperatureSensors.h"
#include "realTimeClock.h"

class Logger {
private:
public:
  static void init();
  static void writeToSD();
  static void writeErrorToSD(unsigned long err, unsigned long additionalValue);
};

#endif
