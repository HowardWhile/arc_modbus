#include "ARC_Modbus_Encoder.h"

typedef union
{
	short wValue;
	char cValue[2];
}tu_convert16; //轉換	

#if ARC_Modbus_RTU_ENABLED > 0

#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif