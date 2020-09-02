#ifndef ARC_Modbus_h__
#define ARC_Modbus_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "ARC_Modbus_Struct.h"

#if ARC_Modbus_RTU_ENABLED > 0
	void ARC_MODBUS_RTU_Initial(ARC_MODUBS_RTU_HandleTypeDef* iModbusHandel, uint8_t iSlaveID);	

	void ARC_MODBUS_RTU_RxWork_s(ARC_MODUBS_RTU_HandleTypeDef* iModbusHandel, char* iBytes, int iLength);
	void ARC_MODBUS_RTU_RxWork(ARC_MODUBS_RTU_HandleTypeDef* iModbusHandel, char iByte);
#endif
#if ARC_Modbus_ASCII_ENABLED > 0

#endif
#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif


#ifdef __cplusplus
}
#endif

#endif // ARC_Modbus_h__
