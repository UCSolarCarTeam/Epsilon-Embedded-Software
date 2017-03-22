#pragma once

#define CHECKSUM_LENGTH (2)
#define FRAMING_LENGTH_INCREASE (2)
#define TERMINATING_BYTE (0x00)

union DataUnion
{
    float floatData;
    short shortData[2];
    unsigned short uShortData[2];
    char charData[4];
    unsigned int uIntData;
};

unsigned short calculateCrc16(unsigned char* input, int length);

unsigned int frameData(unsigned char* dataToEncode, unsigned long length, unsigned char* frameData);
unsigned int stuffData(unsigned char* dataToEncode, unsigned long length, unsigned char* encodedData);
void addChecksum(unsigned char* data, unsigned int length);

void writeFloatIntoArray(unsigned char* data, int index, float value);
void writeShortIntoArray(unsigned char* data, int index, short value);
void writeUShortIntoArray(unsigned char* data, int index, unsigned short value);
void writeUIntIntoArray(unsigned char* data, int index, unsigned int value);
void writeBoolsIntoArray(unsigned char* data, int index, unsigned char* values, int numValues);
