#ifndef ARC_Modbus_Decoder_h__
#define ARC_Modbus_Decoder_h__
#ifdef __cplusplus
extern "C" {
#endif

#include "ARC_Modbus_Struct.h"
#include <stdbool.h>

#if ARC_Modbus_RTU_ENABLED > 0
	bool ARC_MODBUS_RTU_Decoder_RxByte(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);
	bool ARC_MODBUS_RTU_function0106_decoder(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);
	bool ARC_MODBUS_RTU_function1516_decoder(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);
	bool ARC_MODBUS_RTU_check_1516_head(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);
#endif
#if ARC_Modbus_ASCII_ENABLED > 0

#endif
#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif

#ifdef __cplusplus
}
#endif

#endif // ARC_Modbus_Decoder_h__
