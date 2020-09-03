#ifndef ARC_Modbus_Decoder_h__
#define ARC_Modbus_Decoder_h__
#ifdef __cplusplus
extern "C" {
#endif

#include "ARC_Modbus_Struct.h"
#include <stdbool.h>

#if ARC_Modbus_RTU_ENABLED > 0
	bool ARC_MODBUS_RTU_Decoder_RxByte(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);
	char* ARC_MODBUS_RTU_GetMultipleWrite(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel); // 多重寫入(FC15,FC16)的資料由此取得
#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif
#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif

#ifdef __cplusplus
}
#endif

#endif // ARC_Modbus_Decoder_h__
