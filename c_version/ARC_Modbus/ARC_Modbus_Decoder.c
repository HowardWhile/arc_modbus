#include "ARC_Modbus_Decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef union
{
	short wValue;
	char cValue[2];
}tu_convert16; 

#if ARC_Modbus_RTU_ENABLED > 0

bool ARC_MODBUS_RTU_Decoder_RxByte(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{
	if (ARC_MODBUS_RTU_function0106_decoder(iModbusHandel, iByte) == true)
		return true;

	if (ARC_MODBUS_RTU_function1516_decoder(iModbusHandel, iByte) == true)
		return true;

	return false;
}

bool ARC_MODBUS_RTU_function0106_decoder(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{
	ARC_MODBUS_RTU_Rx_HandleTypeDef* h_rx = &iModbusHandel->rx_handler;
	ARC_MODBUS_CommandParameter* result = &h_rx->result;	
	tu_convert16 cTool;

	/* Function Code 01~06 長度固定用 memmove decode */
	memmove(h_rx->rx_buffer_0106, h_rx->rx_buffer_0106 + 1, sizeof(h_rx->rx_buffer_0106));
	h_rx->rx_buffer_0106[sizeof(h_rx->rx_buffer_0106) - 1] = iByte;

	if (h_rx->rx_buffer_0106[0] == iModbusHandel->slave_id)
	{
		cTool.wValue = ARC_MODBUS_CRC16(h_rx->rx_buffer_0106, 6);

		if (cTool.cValue[0] == h_rx->rx_buffer_0106[6] &&
			cTool.cValue[1] == h_rx->rx_buffer_0106[7]) // crc16
		{
			result->SlaveAddress = iModbusHandel->slave_id;
			result->FunctionCode = h_rx->rx_buffer_0106[INDEX_FUNCTION_CODE];

			static const int idx_data_addr_l = INDEX_DATA_ADDRESS + 1;
			static const int idx_data_addr_h = INDEX_DATA_ADDRESS;
			cTool.cValue[0] = h_rx->rx_buffer_0106[idx_data_addr_l];
			cTool.cValue[1] = h_rx->rx_buffer_0106[idx_data_addr_h];
			result->DataAddres = cTool.wValue;

			static const int idx_para_l = INDEX_PARAMETER + 1;
			static const int idx_para_h = INDEX_PARAMETER;
			cTool.cValue[0] = h_rx->rx_buffer_0106[idx_para_l];
			cTool.cValue[1] = h_rx->rx_buffer_0106[idx_para_h];
			result->Parameter = cTool.wValue;

			return true;
		}
	}

	return false;
}

bool ARC_MODBUS_RTU_function1516_decoder(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{

	return false;
}

bool ARC_MODBUS_RTU_check_1516_head(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{

	return false;
}

#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif


