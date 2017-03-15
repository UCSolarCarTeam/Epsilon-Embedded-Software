#include "AuxBmsCanParser.h"

#define STARTUP_INFO_ID 0x301
#define PACK_INFO_ID 0x302
#define ERRORS_ID 0x303
#define TEMPINFO_ID 0x304
#define CELL_VOLTAGES_ID 0x305	

void parseBmuCanMessage(uint32_t stdId, uint8_t data[8])
{
	switch(stdId) {
		case STARTUP_INFO_ID:
    		parseBmuHeartbeat(data);
			break;
		case PACK_INFO_ID:
    		parseStartupInfo(data);
			break;
		case ERRORS_ID:
    		parsePackInfo(data);
			break;
		case TEMPINFO_ID:
    		parseErrorsTempInfo(data);
			break;
		case CELL_VOLTAGES_ID:
    		parseCellVoltages(data);
			break;
	}
}

void parseBmuHeartbeat(uint8_t data[8])
{
    
}

void parseStartupInfo(uint8_t data[8])
{

}

void parsePackInfo(uint8_t data[8])
{

}

void parseErrorsTempInfo(uint8_t data[8])
{

}

void parseCellVoltages(uint8_t data[8])
{

}
