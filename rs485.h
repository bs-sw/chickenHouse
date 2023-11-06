#ifndef __Rs485__
#define __Rs485__

#define SLAVE_NUMBER 2

#define BAUDRATE 115200

#define INPUT_BUFFER_SIZE 64
#define OUTPUT_BUFFER_SIZE 64

#define FUNCTION_DOOR               'D'   //byte
#define FUNCTION_ILLUMINATION       'L'   //byte
#define FUNCTION_TEMPERATURE_OUT    'O'   //float
#define FUNCTION_TEMPERATURE_IN     'I'   //float
#define FUNCTION_SUNRISE            'R'   //int
#define FUNCTION_SUNSET             'S'   //int
#define FUNCTION_DATE_TIME          'T'   //5 x byte
#define FUNCTION_ERROR              'E'   //unsigned long

#define BUFFER_POSITION_SLAVE_NUMBER    1 
#define BUFFER_POSITION_PACKAGE_LENGTH  2 
#define BUFFER_POSITION_CRC_BYTE_0      3 
#define BUFFER_POSITION_CRC_BYTE_1      4 
#define BUFFER_POSITION_CRC_BYTE_2      5 
#define BUFFER_POSITION_CRC_BYTE_3      6 
#define BUFFER_POSITION_PAYLOAD         7 

#include "chickenHouse.h"
#include "error.h"
#include <Arduino.h>
#include "realTimeClock.h"
#include "temperatureSensors.h"
#include "doorController.h"
#include "illumination.h"
#include <avr/pgmspace.h>

const static PROGMEM uint32_t crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

class Rs485 {
private:
  static byte inputBuffer[];
  static byte inputBufferIndex;
  static byte outputBuffer[];
  static byte outputBufferIndex;
  static bool inputBufferReady;
  static bool packageReceived;
  static void initializeInputBuffer();
  static void initializeOutputBuffer();
  static bool addToInputBuffer(byte data);
  static bool addToOutputBuffer(byte data);
  static void finalizeOutputBuffer();
  static void printOutputBuffer();
  static bool inputBufferCrcValid();
  static unsigned long crc_update(unsigned long crc, byte data);
  static unsigned long crc_string(byte *s, int len);
  static void executeDoorCommand(byte command);
  static void executeIlluminationCommand(byte command);
  static void executeDateTimeCommand(byte year, byte month, byte day, byte hour, byte minute);
  static void send();
  static bool receive();
public:
  static void init();
  static void communicate();
};

#endif
