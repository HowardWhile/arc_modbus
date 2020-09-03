#ifndef ARC_Modbus_Encoder_h__
#define ARC_Modbus_Encoder_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "ARC_Modbus_Struct.h"

#if ARC_Modbus_RTU_ENABLED > 0
	void ARC_MODBUS_RTU_GetResponse(
		ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel,
		char* iRegBuffer,
		const ARC_MODBUS_CommandParameter iPara,
		ARC_MODBUS_Exception iError /* exceptions 錯誤訊息 */
	);
#endif
#if ARC_Modbus_ASCII_ENABLED > 0

#endif
#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif

#ifdef __cplusplus
}
#endif

#endif // ARC_Modbus_Encoder_h__
