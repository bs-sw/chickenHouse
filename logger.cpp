#include "logger.h"

void Logger::init() {
  pinMode(SPI_CHIP_SELECT_SD_PIN, OUTPUT);
  if (!SD.begin(SPI_CHIP_SELECT_SD_PIN)) {
    Error::setErrorWithoutLogging(ERROR_SD_INIT);
    digitalWrite(SPI_CHIP_SELECT_SD_PIN, HIGH);
    return;
  }
}

void Logger::writeErrorToSD(unsigned long err, unsigned long additionalValue) {
  char dateTimeString[20];
  char buf[30];

  File dataFile = SD.open(F("err.csv"), FILE_WRITE);   //maximal: 8.3-Format
  if (dataFile) {
    RealTimeClock::getDateTimeString(dateTimeString, false);
    dataFile.print(dateTimeString);
    sprintf_P(buf, PSTR(";%ld.%ld\r\n"), err, additionalValue);
    dataFile.print(buf);
    dataFile.close();
  }
  else {
    Error::setErrorWithoutLogging(ERROR_SD_FILE_WRITE);
  }
  dataFile.close();
}

void Logger::writeToSD() {
  char dateTimeString[20];
  char fileName[]="logs/yyyy-mm.csv";   //maximal: 8.3-Format
  char buf[50];

  RealTimeClock::getYearMonthString(fileName + 5);
  fileName[12]='.'; //replace 0 termination

  File dataFile = SD.open(fileName, FILE_WRITE);

  if (dataFile) {
    RealTimeClock::getDateTimeString(dateTimeString, false);
    dataFile.print(dateTimeString);
    dataFile.print(";");
    dtostrf(TemperatureSensors::getValueOut(), 5, 1, buf);
    buf[5] = ';';
    dtostrf(TemperatureSensors::getValueIn(), 4, 1, buf +  6);
    buf[10] = ';';
    dataFile.print(buf);
    dataFile.close();
  }
  else {
    Error::setErrorWithoutLogging(ERROR_SD_FILE_WRITE);
  }

  dataFile.close();
}
