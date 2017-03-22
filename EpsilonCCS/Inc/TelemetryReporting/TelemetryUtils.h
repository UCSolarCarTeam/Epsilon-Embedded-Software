#pragma once

#define CHECKSUM_LENGTH (2)
#define FRAMING_LENGTH_INCREASE (2)
#define TERMINATING_BYTE (0x00)

unsigned short calculateCrc16(unsigned char* input, int length);

unsigned int frameData(const unsigned char* dataToEncode, unsigned long length, unsigned char* frameData);
unsigned int stuffData(const unsigned char* dataToEncode, unsigned long length, unsigned char* encodedData);
void addChecksum(unsigned char* data, unsigned int length);

void writeFloatIntoArray(unsigned char* data, int index, const float& value);
void writeShortIntoArray(unsigned char* data, int index, const short& value);
void writeUShortIntoArray(unsigned char* data, int index, const unsigned short& value);
void writeUIntIntoArray(unsigned char* data, int index, const unsigned int& value);
void writeBoolsIntoArray(unsigned char* data, int index, const bool values[], int numValues);
