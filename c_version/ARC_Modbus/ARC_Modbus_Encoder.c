#include "ARC_Modbus_Encoder.h"

#include <string.h>

typedef union
{
	short wValue;
	char cValue[2];
}tu_convert16; //轉換	


#if ARC_Modbus_RTU_ENABLED > 0
void ARC_MODBUS_RTU_pkg_make_exception(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, int iFunctionCode, ARC_MODBUS_Exception iError);
void ARC_MODBUS_RTU_pkg_make_function_read(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, int iFunctionCode, int iRegsNumber, char* iRegBuffer);
void ARC_MODBUS_RTU_pkg_make_function_write(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, int iFunctionCode, int iAddress, int iParameter);

void ARC_MODBUS_RTU_PacketMake(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, ARC_MODBUS_Exception iError)
{
	ARC_MODBUS_CommandParameter* iPara = &iModbusHandel->rx_handler.result;
	char* iRegBuffer = iModbusHandel->reg_buffer;

	if (iError != Ex_none)
	{//有錯誤訊息 製作相對應的例外訊息
		ARC_MODBUS_RTU_pkg_make_exception(iModbusHandel, iPara->FunctionCode, iError);
	}
	else
	{
		int function_code = iPara->FunctionCode;
		if (function_code == 1 || function_code == 2 || function_code == 3 || function_code == 4)
		{
			// 讀取
			ARC_MODBUS_RTU_pkg_make_function_read(iModbusHandel, function_code, iPara->Parameter, iRegBuffer);
		}
		else
		{
			// 寫入
			ARC_MODBUS_RTU_pkg_make_function_write(iModbusHandel, function_code, iPara->DataAddres, iPara->Parameter);
		}
	}
}

void ARC_MODBUS_RTU_pkg_make_exception(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, int iFunctionCode, ARC_MODBUS_Exception iError)
{
	/* ---------------------------------- */
	tu_convert16 cTool;
	char* oBuffer = iModbusHandel->tx_handler.tx_msg_buffer;
	/* ---------------------------------- */
	oBuffer[INDEX_SLAVE_ID] = (char)iModbusHandel->slave_id;
	oBuffer[INDEX_FUNCTION_CODE] = iFunctionCode | 0x80;
	oBuffer[INDEX_EXCEPTION_CODE] = (char)iError;
	// crc 16
	cTool.wValue = ARC_MODBUS_CRC16(oBuffer, 3);
	oBuffer[3] = cTool.cValue[0];
	oBuffer[4] = cTool.cValue[1];
	/* ---------------------------------- */
	iModbusHandel->tx_handler.tx_msg_length = 5;		
	/* ---------------------------------- */
}


void ARC_MODBUS_RTU_pkg_make_function_read(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, int iFunctionCode, int iRegsNumber, char* iRegBuffer)
{
	/* ---------------------------------- */
	tu_convert16 cTool;
	char* oBuffer = iModbusHandel->tx_handler.tx_msg_buffer;
	/* ---------------------------------- */
	oBuffer[INDEX_SLAVE_ID] = (char)iModbusHandel->slave_id;
	oBuffer[INDEX_FUNCTION_CODE] = iFunctionCode;
	/* ---------------------------------- */
	/* The number of data bytes to follow */
	int bytes_number;
	if (iFunctionCode == 1 || iFunctionCode == 2)
	{
		// 讀取線圈命令
		bytes_number = iRegsNumber / 8 + (iRegsNumber % 8 != 0);
	}
	else if (iFunctionCode == 3 || iFunctionCode == 4)
	{
		// 讀取數值命令
		bytes_number = iRegsNumber * 2;
	}
	/* ---------------------------------- */
	oBuffer[2] = bytes_number;
	memcpy(&oBuffer[3], iRegBuffer, bytes_number);
	/* ---------------------------------- */
	// crc 16
	cTool.wValue = ARC_MODBUS_CRC16(oBuffer, 3 + bytes_number);
	oBuffer[bytes_number + 3] = cTool.cValue[0];
	oBuffer[bytes_number + 4] = cTool.cValue[1];
	/* ---------------------------------- */
	iModbusHandel->tx_handler.tx_msg_length = bytes_number + 5; // 3(id, code, num) + bytes_number + 2(crc16)
	/* ---------------------------------- */
}


void ARC_MODBUS_RTU_pkg_make_function_write(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, int iFunctionCode, int iAddress, int iParameter)
{
	/* ---------------------------------- */
	tu_convert16 cTool;
	char* oBuffer = iModbusHandel->tx_handler.tx_msg_buffer;
	/* ---------------------------------- */
	oBuffer[INDEX_SLAVE_ID] = (char)iModbusHandel->slave_id;
	oBuffer[INDEX_FUNCTION_CODE] = iFunctionCode;
	/* --------------------------------- */
	// The Data Address
	cTool.wValue = iAddress;
	oBuffer[INDEX_DATA_ADDRESS] = cTool.cValue[1];
	oBuffer[INDEX_DATA_ADDRESS + 1] = cTool.cValue[0];
	/* --------------------------------- */
	// The Parameter
	cTool.wValue = iParameter;
	oBuffer[INDEX_PARAMETER] = cTool.cValue[1];
	oBuffer[INDEX_PARAMETER + 1] = cTool.cValue[0];
	/* --------------------------------- */
	// crc 16
	cTool.wValue = ARC_MODBUS_CRC16(oBuffer, 6);
	oBuffer[6] = cTool.cValue[0];
	oBuffer[7] = cTool.cValue[1];
	/* --------------------------------- */
	iModbusHandel->tx_handler.tx_msg_length = 8;
	/* --------------------------------- */
}




#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif