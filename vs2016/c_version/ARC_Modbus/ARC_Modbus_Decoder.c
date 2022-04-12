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

bool ARC_MODBUS_RTU_function0106_decoder(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);
bool ARC_MODBUS_RTU_function1516_decoder(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);
bool ARC_MODBUS_RTU_check_1516_head(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);

bool ARC_MODBUS_RTU_Decoder_RxByte(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{
	if (ARC_MODBUS_RTU_function0106_decoder(iModbusHandel, iByte) == true)
		return true;

	if (ARC_MODBUS_RTU_function1516_decoder(iModbusHandel, iByte) == true)
		return true;

	return false;
}

char* ARC_MODBUS_RTU_GetMultipleWrite(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel)
{
	return iModbusHandel->rx_handler.rx_buffer_1516 + INDEX_DATA_START_w;
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
	ARC_MODBUS_RTU_Rx_HandleTypeDef* h_rx = &iModbusHandel->rx_handler;
	ARC_MODBUS_CommandParameter* result = &h_rx->result;
	tu_convert16 cTool;

	if (h_rx->buffer_enable)
	{
		h_rx->byte_index++;
		h_rx->rx_buffer_1516[h_rx->buffer_index++] = iByte;


		if (h_rx->byte_index >= 255)
		{
			h_rx->buffer_enable = false;
		}
		else
		{
			// index 6: The number of data bytes to follow
			if (h_rx->byte_index == (uint8_t)h_rx->rx_buffer_1516[6] + 2) // 2: crc16 size
			{// 收取定義的長度
			 // 確認 crc16
				cTool.wValue = ARC_MODBUS_CRC16(h_rx->rx_buffer_1516, h_rx->buffer_index - 2);
				if (cTool.cValue[0] == h_rx->rx_buffer_1516[h_rx->buffer_index - 2] &&
					cTool.cValue[1] == h_rx->rx_buffer_1516[h_rx->buffer_index - 1]) // crc16
				{
					/* --------------------------- */
					// 正確的封包
					/* --------------------------- */
					result->SlaveAddress = iModbusHandel->slave_id;
					result->FunctionCode = h_rx->rx_buffer_1516[INDEX_FUNCTION_CODE];

					static const int idx_data_addr_l = INDEX_DATA_ADDRESS + 1;
					static const int idx_data_addr_h = INDEX_DATA_ADDRESS;
					cTool.cValue[0] = h_rx->rx_buffer_1516[idx_data_addr_l];
					cTool.cValue[1] = h_rx->rx_buffer_1516[idx_data_addr_h];
					result->DataAddres = cTool.wValue; // 位置

					static const int idx_para_l = INDEX_PARAMETER + 1;
					static const int idx_para_h = INDEX_PARAMETER;
					cTool.cValue[0] = h_rx->rx_buffer_1516[idx_para_l];
					cTool.cValue[1] = h_rx->rx_buffer_1516[idx_para_h];
					result->Parameter = cTool.wValue; // 數量

					/* --------------------------- */
					// 接收命令完成
					/* --------------------------- */
					h_rx->buffer_enable = false;
					return true;
				}
			}
		}
	}

	/* --------------------------- */
	/* 判斷起始位元 */
	if (!h_rx->buffer_enable)
	{
		if (ARC_MODBUS_RTU_check_1516_head(iModbusHandel, iByte))
		{
			h_rx->buffer_enable = true;
			h_rx->buffer_index = 7;
			h_rx->byte_index = 0;
			memcpy(h_rx->rx_buffer_1516, h_rx->rx_buffer_1516_head, sizeof(h_rx->rx_buffer_1516_head));
		}
	}
	/* --------------------------- */
	return false;
}

bool ARC_MODBUS_RTU_check_1516_head(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{
	ARC_MODBUS_RTU_Rx_HandleTypeDef* h_rx = &iModbusHandel->rx_handler;
	tu_convert16 cTool;

	memmove(h_rx->rx_buffer_1516_head, h_rx->rx_buffer_1516_head + 1, sizeof(h_rx->rx_buffer_1516_head));
	h_rx->rx_buffer_1516_head[sizeof(h_rx->rx_buffer_1516_head) - 1] = iByte;

	uint8_t function_code = h_rx->rx_buffer_1516_head[INDEX_FUNCTION_CODE];
	if (h_rx->rx_buffer_1516_head[0] == iModbusHandel->slave_id && /* Slave ID正確 */
		(function_code == 15 || function_code == 16))/* Function Code正確 */
	{
		tu_convert16 cTool;

		/* The number to written */
		static const int idx_num2write_l = INDEX_NUMBER_TO_WRITE + 1;
		static const int idx_num2write_h = INDEX_NUMBER_TO_WRITE;
		cTool.cValue[0] = h_rx->rx_buffer_1516_head[idx_num2write_l];
		cTool.cValue[1] = h_rx->rx_buffer_1516_head[idx_num2write_h];
		int number2written = cTool.wValue;

		if (function_code == 15 && number2written <= 2000)
		{
			uint8_t byte_length = number2written / 8 + (number2written % 8 != 0);
			if (byte_length == (uint8_t)h_rx->rx_buffer_1516_head[INDEX_NUMBER_BYTES_FOLLOW_w])
				return true; // 是 function code 15 的開頭
		}
		else if (function_code == 16 && number2written <= 125)
		{
			uint8_t byte_length = number2written * 2;
			if (byte_length == (uint8_t)h_rx->rx_buffer_1516_head[INDEX_NUMBER_BYTES_FOLLOW_w])
				return true; // 是 function code 16 的開頭
		}
}
	return false;
}

#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif


