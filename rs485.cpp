#include "rs485.h"

byte Rs485::inputBuffer[INPUT_BUFFER_SIZE];
byte Rs485::outputBuffer[OUTPUT_BUFFER_SIZE];
byte Rs485::outputBufferIndex;
byte Rs485::inputBufferIndex;
bool Rs485::inputBufferReady;
bool Rs485::packageReceived;

union Variant {
  float f;
  unsigned long ul;
  byte b[4];
  int i[2];
};

void Rs485::init() {
  pinMode(OUTPUT_SEND_ENABLE_PIN, OUTPUT);
  digitalWrite(OUTPUT_SEND_ENABLE_PIN, LOW); 
  Serial.begin(BAUDRATE);
}

unsigned long Rs485::crc_update(unsigned long crc, byte data) {
    byte tbl_idx;
    tbl_idx = crc ^ (data >> (0 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    tbl_idx = crc ^ (data >> (1 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    return crc;
}

unsigned long Rs485::crc_string(byte *s, int len) {
  unsigned long crc = ~0L;
  int i;
  for (i=0; i<len; i++) {
    crc = crc_update(crc, *s++);
  }
  crc = ~crc;
  return crc;
}

void Rs485::initializeOutputBuffer() {
  outputBuffer[0] = '{';
  outputBuffer[BUFFER_POSITION_SLAVE_NUMBER] = SLAVE_NUMBER;
  outputBuffer[BUFFER_POSITION_PACKAGE_LENGTH] = 0;
  outputBuffer[BUFFER_POSITION_CRC_BYTE_0] = 0;
  outputBuffer[BUFFER_POSITION_CRC_BYTE_1] = 0;
  outputBuffer[BUFFER_POSITION_CRC_BYTE_2] = 0;
  outputBuffer[BUFFER_POSITION_CRC_BYTE_3] = 0;
  outputBufferIndex = BUFFER_POSITION_PAYLOAD;
}

bool Rs485::addToOutputBuffer(byte data) {
  outputBuffer[outputBufferIndex] = data;
  outputBufferIndex++;
  if (outputBufferIndex > OUTPUT_BUFFER_SIZE) return false;
  return true;
}

bool Rs485::addToInputBuffer(byte data) {
  inputBuffer[inputBufferIndex] = data;
  inputBufferIndex++;
  if (inputBufferIndex > INPUT_BUFFER_SIZE) return false;
  return true;
}

bool Rs485::inputBufferCrcValid() {
  #ifdef NO_CRC_CHECK
  return true;
  #endif
  unsigned long transmittedCrc = 
    ((unsigned long) inputBuffer[BUFFER_POSITION_CRC_BYTE_0])       | 
    ((unsigned long) inputBuffer[BUFFER_POSITION_CRC_BYTE_1])<<8    | 
    ((unsigned long) inputBuffer[BUFFER_POSITION_CRC_BYTE_2])<<16   | 
    ((unsigned long) inputBuffer[BUFFER_POSITION_CRC_BYTE_3])<<24;
  #ifdef DEBUG 
    Serial.println("F(transmitted crc"));
    Serial.write((byte) transmittedCrc);
    Serial.write((byte) (transmittedCrc >> 8));
    Serial.write((byte) (transmittedCrc >> 16));
    Serial.write((byte) (transmittedCrc >> 24));
    Serial.println();
  #endif
  inputBuffer[BUFFER_POSITION_CRC_BYTE_0] = 0;
  inputBuffer[BUFFER_POSITION_CRC_BYTE_1] = 0;  
  inputBuffer[BUFFER_POSITION_CRC_BYTE_2] = 0;
  inputBuffer[BUFFER_POSITION_CRC_BYTE_3] = 0;
  unsigned long calculatedCrc = crc_string(inputBuffer, inputBufferIndex);
  #ifdef DEBUG 
    Serial.println(F("calculated crc"));
    Serial.write((byte) calculatedCrc);
    Serial.write((byte) (calculatedCrc >> 8));
    Serial.write((byte) (calculatedCrc >> 16));
    Serial.write((byte) (calculatedCrc >> 24));
    Serial.println();
  #endif
  return (calculatedCrc == transmittedCrc);
}

void Rs485::printOutputBuffer() {
  digitalWrite(OUTPUT_SEND_ENABLE_PIN, HIGH); //enable MAX485 driver
  Serial.write(outputBuffer, outputBufferIndex);
  Serial.flush();
  digitalWrite(OUTPUT_SEND_ENABLE_PIN, LOW); //enable MAX485 driver
}

void Rs485::initializeInputBuffer() {
  inputBufferIndex = 0;
}

void Rs485::finalizeOutputBuffer() {
  addToOutputBuffer('}');
  outputBuffer[BUFFER_POSITION_PACKAGE_LENGTH] = outputBufferIndex;
  unsigned long crc = crc_string(outputBuffer, outputBufferIndex);
  outputBuffer[BUFFER_POSITION_CRC_BYTE_0] = ((byte) crc);
  outputBuffer[BUFFER_POSITION_CRC_BYTE_1] = ((byte) (crc >> 8));   
  outputBuffer[BUFFER_POSITION_CRC_BYTE_2] = ((byte) (crc >> 16));
  outputBuffer[BUFFER_POSITION_CRC_BYTE_3] = ((byte) (crc >> 24));
}

void Rs485::send() {
  union Variant temp;
  initializeOutputBuffer();
  addToOutputBuffer(FUNCTION_DOOR);
  addToOutputBuffer(DoorController::getState());
  addToOutputBuffer(FUNCTION_ILLUMINATION);
  addToOutputBuffer(Illumination::getState());
  addToOutputBuffer(FUNCTION_TEMPERATURE_OUT);
  temp.f = TemperatureSensors::getValueOut();
  addToOutputBuffer(temp.b[0]);
  addToOutputBuffer(temp.b[1]);
  addToOutputBuffer(temp.b[2]);
  addToOutputBuffer(temp.b[3]);
  addToOutputBuffer(FUNCTION_TEMPERATURE_IN);
  temp.f = TemperatureSensors::getValueIn();
  addToOutputBuffer(temp.b[0]);
  addToOutputBuffer(temp.b[1]);
  addToOutputBuffer(temp.b[2]);
  addToOutputBuffer(temp.b[3]);
  addToOutputBuffer(FUNCTION_SUNRISE);
  temp.i[0] = RealTimeClock::getSunrise();
  addToOutputBuffer(temp.b[0]);
  addToOutputBuffer(temp.b[1]); 
  addToOutputBuffer(FUNCTION_SUNSET);
  temp.i[0] = RealTimeClock::getSunset();
  addToOutputBuffer(temp.b[0]);
  addToOutputBuffer(temp.b[1]); 
  addToOutputBuffer(FUNCTION_DATE_TIME);
  addToOutputBuffer((byte) RealTimeClock::getYear());
  addToOutputBuffer((byte) RealTimeClock::getMonth());
  addToOutputBuffer((byte) RealTimeClock::getDay());
  addToOutputBuffer((byte) RealTimeClock::getHour());
  addToOutputBuffer((byte) RealTimeClock::getMinute());
  addToOutputBuffer(FUNCTION_ERROR);
  temp.ul = Error::getError();
  addToOutputBuffer(temp.b[0]);
  addToOutputBuffer(temp.b[1]);
  addToOutputBuffer(temp.b[2]);
  addToOutputBuffer(temp.b[3]);
  finalizeOutputBuffer();
  printOutputBuffer();
}
//{   sn  le  crc........   func                }
//7B  02  08  6A CC 60 C1                      7D   //call without command
//7B  02  0A  33 6A 3A 1C   4C 01              7D   //call with command illumination to on
//7B  02  0A  72 5B 21 05   4C 00              7D   //call with command illumination to off
//7B  02  0A  CA 0A 32 0B   44 00              7D   //call with command stop door
//7B  02  0A  40 E2 DD F1   44 0A              7D   //call with command open door
//7B  02  0A  9F DD 9C 25   44 14              7D   //call with command close door
//7B  02  0C  FF CC 7A 50   44 14  4C 01       7D   //call with command close door and illumination to on
//7B  02  0E  00 00 00 00   54 15 03 02 06 07  7D   //call with command setting of date & time 2.3.2021 6:30
//7B  02  0E  00 00 00 00   54 15 03 02 12 07  7D   //call with command setting of date & time 2.3.2021 18:07

bool Rs485::receive() {
	  //Paket in Puffer einlesen
  while(Serial.available()) {
    byte input = Serial.read();
    #ifdef DEBUG 
      Serial.println(input);
    #endif
    if (!inputBufferReady) {
      if (input == '{') {
        initializeInputBuffer();
        inputBufferReady = true;
        #ifdef DEBUG 
          Serial.println(F("input buffer initialized"));
        #endif
      } else {
        Error::setError(ERROR_INPUT_BUFFER_UNEXPECTED_INPUT);
      }
    }
    if (inputBufferReady) {
      if (!addToInputBuffer(input)) {
        Error::setError(ERROR_INPUT_BUFFER_OVERFLOW);
        inputBufferReady = false;
        #ifdef DEBUG 
          Serial.println(F("input buffer overflow"));
        #endif
      }
      if (inputBufferReady && input == '}') {
        packageReceived = true;
        inputBufferReady = false;
        #ifdef DEBUG 
          Serial.println(F("package received"));
        #endif
        break;
      } 
    }
  }

  //Process received package
  if (packageReceived) {
    packageReceived = false;

    byte packageLen = inputBuffer[BUFFER_POSITION_PACKAGE_LENGTH];
    if (packageLen != inputBufferIndex || packageLen > INPUT_BUFFER_SIZE) {
      Error::setError(ERROR_INPUT_BUFFER_INVALID_LENGTH);
      #ifdef DEBUG 
        Serial.println(F("invalid length"));
      #endif
      return false;
    }
    
    if(!inputBufferCrcValid()) {
      Error::setError(ERROR_INPUT_BUFFER_INVALID_CRC);
      #ifdef DEBUG 
        Serial.println(F("invalid crc"));
      #endif
      return false;
    }

    #ifdef DEBUG 
      Serial.print(F("Slave number "));
      Serial.println(inputBuffer[BUFFER_POSITION_SLAVE_NUMBER]);
    #endif
      
    if(inputBuffer[BUFFER_POSITION_SLAVE_NUMBER] == SLAVE_NUMBER) {
      #ifdef DEBUG 
        Serial.println(F("Slave number matching"));
      #endif
      inputBufferIndex = BUFFER_POSITION_PAYLOAD;
      while(inputBufferIndex < packageLen - 1) {
        byte function = inputBuffer[inputBufferIndex];
        inputBufferIndex += 1;
        switch(function) {
          case FUNCTION_DOOR:  
            executeDoorCommand(inputBuffer[inputBufferIndex]);
            inputBufferIndex += 1;
            break;
          case FUNCTION_ILLUMINATION: 
            executeIlluminationCommand(inputBuffer[inputBufferIndex]);
            inputBufferIndex += 1;
            break;
          case FUNCTION_DATE_TIME: 
            executeDateTimeCommand(inputBuffer[inputBufferIndex], inputBuffer[inputBufferIndex+1], inputBuffer[inputBufferIndex+2],
              inputBuffer[inputBufferIndex+3], inputBuffer[inputBufferIndex+4]);
            inputBufferIndex += 5;
            break;
          default: 
            Error::setError(ERROR_INPUT_BUFFER_UNKNOWN_FUNCTION);
            #ifdef DEBUG 
              Serial.println(F("Unknown function"));
            #endif
            return false;
        }
        if (inputBufferIndex > packageLen - 1) {
          Error::setError(ERROR_INPUT_BUFFER_INVALID_LENGTH);
          #ifdef DEBUG 
              Serial.println(F("invalid length"));
          #endif
          return false;
        }
      }
	  return true;
    }
  }
  return false;
}

void Rs485::communicate() {
  if (receive()) send();
}

void Rs485::executeDateTimeCommand(byte year, byte month, byte day, byte hour, byte minute) {
  RealTimeClock::setTime(year, month, day, hour, minute);
}

void Rs485::executeDoorCommand(byte command) {
  switch(command) {
    case 0: DoorController::extStopDoor(); break;
    case 10: DoorController::extOpenDoor(); break;
    case 20: DoorController::extCloseDoor(); break;
    default: Error::setError(ERROR_INPUT_BUFFER_INVALID_COMMAND);
  }
}

void Rs485::executeIlluminationCommand(byte command) {
  switch(command) {
    case 0: Illumination::switchOff(); break;
    case 1: Illumination::switchOn(); break;
    default: Error::setError(ERROR_INPUT_BUFFER_INVALID_COMMAND);
  }
}
