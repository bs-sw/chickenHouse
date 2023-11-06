#ifndef __temperatureSensors__
#define __temperatureSensors__

#include "chickenhouse.h"
#include "error.h"
#include <Arduino.h>
#include <OneWire.h>

#define IS_COLD_OUTSIDE_TEMPERATURE 0.0

class TemperatureSensors {
private:
  static OneWire ds;
  static byte adr_out[]; //Adresse Aussensensor
  static byte adr_in[]; //Adresse Innensensor
  static byte data[12];
  static void setSensorResolution(byte adress[]);
public:
  static float temperature_out;
  static float temperature_in;
  static void refreshTemperatureValues();
  static void startTemperatureConversion();
  static void init();
  static float getValueOut();
  static float getValueIn();
  static bool isColdOutside();
};

#endif
