#include "ARC_Modbus.h"
#include "ARC_Modbus_Decoder.h"
#include "ARC_Modbus_Encoder.h"

#if ARC_Modbus_RTU_ENABLED > 0
void ARC_MODBUS_RTU_RxWork(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);

void ARC_MODBUS_RTU_Initial(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, uint8_t iSlaveID)
{
	iModbusHandel->slave_id = iSlaveID;
	iModbusHandel->interface.Event_TxWork = NULL;
	iModbusHandel->rx_handler.buffer_enable = false;
}


void ARC_MODBUS_RTU_InitialInterface(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, void* iEvent_TxWork, void* iCallBack_Coils, void* iCallBack_InputCoils, void* iCallBack_Register, void* iCallBack_InputRegs)
{
	iModbusHandel->interface.Event_TxWork = iEvent_TxWork;
	iModbusHandel->interface.CallBack_Coils = iCallBack_Coils;
	iModbusHandel->interface.CallBack_InputCoils = iCallBack_InputCoils;
	iModbusHandel->interface.CallBack_InputRegs = iCallBack_InputRegs;
	iModbusHandel->interface.CallBack_Register = iCallBack_Register;
}

void ARC_MODBUS_RTU_RxWorkEx(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char* iBytes, int iLength)
{
	for (int i = 0; i < iLength; i++)
	{
		ARC_MODBUS_RTU_RxWork(iModbusHandel, iBytes[iLength]);
	}

	if (iModbusHandel->interface.Event_TxWork != NULL)
	{
		iModbusHandel->interface.Event_TxWork(iModbusHandel, iBytes, iLength);
	}
}

void ARC_MODBUS_RTU_RxWork(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{

}
#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif





