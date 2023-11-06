#include "temperatureSensors.h"

byte TemperatureSensors::adr_out[8] = {0x28,0x93,0xD7,0x5B,0x06,0x00,0x00,0x64};
byte TemperatureSensors::adr_in[8] = {0x28,0x93,0xD7,0x5B,0x06,0x00,0x00,0x64};
OneWire TemperatureSensors::ds(ONEWIRE_PIN);
byte TemperatureSensors::data[12];
float TemperatureSensors::temperature_out;
float TemperatureSensors::temperature_in;

void TemperatureSensors::init() {
  setSensorResolution(adr_out);
  setSensorResolution(adr_in);
  startTemperatureConversion();
  delay(1000);
  refreshTemperatureValues();
}

bool TemperatureSensors::isColdOutside() {
  return temperature_out < IS_COLD_OUTSIDE_TEMPERATURE;
}

float TemperatureSensors::getValueOut() {
#ifdef TEMPERATURES_SIMULATED
  return -10;
#else
  return temperature_out;
#endif
}

float TemperatureSensors::getValueIn() {
#ifdef TEMPERATURES_SIMULATED
  return 25;
#else
  return temperature_in;
#endif
}

void TemperatureSensors::startTemperatureConversion() {
  ds.reset();
  ds.write(0xCC);  //Skip ROM - all slaves should be requested
  ds.write(0x44);  //start conversion
}

void TemperatureSensors::refreshTemperatureValues() {
  ds.reset();
  ds.select(adr_out);    
  ds.write(0xBE);
  
  for (int i = 0; i < 9; i++) {           // 9 bytes
    data[i] = ds.read();
  }
  
  if (OneWire::crc8(data, 8) == data[8]) temperature_out = ((data[1] << 8) + data[0])/16.0;
  
  if (temperature_out > 50 || temperature_out < -50) {
    temperature_out = -99;
    Error::setError(ERROR_OUTSIDE_TEMP_IMPLAUSIBLE);
  }

  ds.reset();
  ds.select(adr_in);    
  ds.write(0xBE);

  for (int i = 0; i < 9; i++) {           // 9 bytes
    data[i] = ds.read();
  }

  if (OneWire::crc8(data, 8) == data[8]) temperature_in = ((data[1] << 8) + data[0])/16.0;

  if (temperature_in > 50 || temperature_in < -50) {
    temperature_in = -99;
    Error::setError(ERROR_INSIDE_TEMP_IMPLAUSIBLE);
  }
}

void TemperatureSensors::setSensorResolution(byte adress[]) {
  ds.reset();
  ds.select(adress); 
  ds.write(0x4E);
  ds.write(0);              //temperature alarm configuration TH
  ds.write(0);              //temperature alarm configuration TL
  ds.write(0x7F);           //resolution 12 Bit 0x7F

  ds.reset();
  ds.select(adress);
  ds.write(0x48, false);    //save resolution in EEPROM
  delay(20);                //Writing of EEPROM takes 10ms
  ds.reset();
}
