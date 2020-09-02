#include "ARC_Modbus_Decoder.h"

#if ARC_Modbus_RTU_ENABLED > 0

bool ARC_MODBUS_RTU_Decoder_RxByte(ARC_MODUBS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{
	if (ARC_MODBUS_RTU_function0106_decoder(iModbusHandel, iByte) == true)
		return true;

	if (ARC_MODBUS_RTU_function1516_decoder(iModbusHandel, iByte) == true)
		return true;

	return false;
}

bool ARC_MODBUS_RTU_function0106_decoder(ARC_MODUBS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{

	return false;
}

bool ARC_MODBUS_RTU_function1516_decoder(ARC_MODUBS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{

	return false;
}

bool ARC_MODBUS_RTU_check_1516_head(ARC_MODUBS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{

	return false;
}

#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif


