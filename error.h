#ifndef __error__
#define __error__

#include "chickenHouse.h"
#include <Arduino.h>

#define ERROR_INSIDE_TEMP_IMPLAUSIBLE         0x001
#define ERROR_OUTSIDE_TEMP_IMPLAUSIBLE        0x002
#define ERROR_MAX_DOOR_TIME_EXCEEDED          0x004
#define ERROR_IMPLAUSIBLE_END_SWITCH          0x008
#define ERROR_WATCHDOG_CYCLE_TIME             0x010
#define ERROR_DATE_TIME_COULDNT_BE_SET        0x020
#define ERROR_DATE_TIME_COULDNT_BE_READ       0x040
#define ERROR_SD_INIT                         0x080
#define ERROR_SD_FILE_WRITE                   0x100

#define ERROR_RS485_INPUT_BUFFER_OUT_OF_BOUNDS  0x400
#define ERROR_INPUT_BUFFER_INVALID_CRC          0x800
#define ERROR_INPUT_BUFFER_INVALID_LENGTH       0x1000
#define ERROR_INPUT_BUFFER_UNKNOWN_FUNCTION     0x2000
#define ERROR_INPUT_BUFFER_INVALID_COMMAND      0x4000
#define ERROR_INPUT_BUFFER_OVERFLOW             0x8000
#define ERROR_INPUT_BUFFER_UNEXPECTED_INPUT     0x10000

class Error {
private:
  static bool errorPin;
public:
  static unsigned long error;
  static unsigned int comErrorCount;
  static void init();
  static void setError(unsigned long err, unsigned long additionalValue);
  static void setError(unsigned long err);
  static void setErrorWithoutLogging(unsigned long err);
  static unsigned long getError();
  static void output();
  static void resetError(unsigned long err);
  static void incComErrorCount();
  static unsigned int getComErrorCount();
};

#endif
