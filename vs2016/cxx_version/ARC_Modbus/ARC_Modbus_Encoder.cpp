#include "ARC_Modbus_Encoder.h"

#include <stdio.h>
#include <string.h>
namespace ARC_Modbus{

#if ARC_Modbus_RTU_ENABLED > 0
	Encoder_RTU::Encoder_RTU()
	{

	}

	Encoder_RTU::~Encoder_RTU()
	{

	}

	void Encoder_RTU::GetResponse(char* oBuffer, int &oLength, /* output */
		char* iRegBuffer,
		const Command_Parameter &iPara,
		Exception iError /* exceptions 錯誤訊息 */)
	{
		if (iError != Ex_none)
		{//有錯誤訊息 製作相對應的例外訊息
			this->packet_make_exception(iPara.FunctionCode, iError, oBuffer, oLength);
		}
		else
		{
			int function_code = iPara.FunctionCode;
			if (function_code == 1 || function_code == 2 || function_code == 3 || function_code == 4)
			{
				// 讀取
				this->packet_make_function_read(function_code, iPara.Parameter, iRegBuffer, oBuffer, oLength);
			}
			else
			{
				// 寫入
				this->packet_make_function_write(function_code, iPara.DataAddres, iPara.Parameter, oBuffer, oLength);
			}
		}
	}

	void Encoder_RTU::packet_make_exception(int iFunctionCode, Exception iError, char* oBuffer, int &oLength)
	{
		oBuffer[INDEX_SLAVE_ID] = (char)this->slave_id;
		oBuffer[INDEX_FUNCTION_CODE] = iFunctionCode | 0x80;
		oBuffer[INDEX_EXCEPTION_CODE] = (char)iError;

		// crc 16
		tu_convert16 cTool;
		cTool.wValue = CRC16(oBuffer, 3);
		oBuffer[3] = cTool.cValue[0];
		oBuffer[4] = cTool.cValue[1];
		oLength = 5;
	}

	void Encoder_RTU::packet_make_function_read(int iFunctionCode, int iRegsNumber, char* iRegBuffer, char* oBuffer, int &oLength)
	{
		oBuffer[INDEX_SLAVE_ID] = (char)this->slave_id;
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
		//for (int i = 0; i < bytes_number; i++)
		//{
		//	this->msg_buffer[3 + i] = iRegBuffer[i];
		//}
		/* ---------------------------------- */
		// crc 16
		tu_convert16 cTool;
		cTool.wValue = CRC16(oBuffer, 3 + bytes_number);
		oBuffer[bytes_number + 3] = cTool.cValue[0];
		oBuffer[bytes_number + 4] = cTool.cValue[1];
		/* ---------------------------------- */
		oLength = bytes_number + 5; // 3(id, code, num) + bytes_number + 2(crc16)
		/* ---------------------------------- */
	}

	void Encoder_RTU::packet_make_function_write(int iFunctionCode, int iAddress, int iParameter, char* oBuffer, int &oLength)
	{
		oBuffer[INDEX_SLAVE_ID] = (char)this->slave_id;
		oBuffer[INDEX_FUNCTION_CODE] = iFunctionCode;

		tu_convert16 cTool;
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
		cTool.wValue = CRC16(oBuffer, 6);
		oBuffer[6] = cTool.cValue[0];
		oBuffer[7] = cTool.cValue[1];
		/* --------------------------------- */
		oLength = 8;
		/* --------------------------------- */
	}
#endif

#if ARC_Modbus_ASCII_ENABLED > 0
	Encoder_ASCII::Encoder_ASCII()
	{

	}

	Encoder_ASCII::~Encoder_ASCII()
	{

	}

	void Encoder_ASCII::GetResponse(char* oBuffer, int &oLength, /* output */
		char* iRegBuffer,
		const Command_Parameter &iPara,
		Exception iError /* exceptions 錯誤訊息 */)
	{
		int byte_number = 0;
		if (iError != Ex_none)
		{//有錯誤訊息 製作相對應的例外訊息
			this->packet_make_exception(iPara.FunctionCode, iError, this->byte_buffer, byte_number);
		}
		else
		{
			int function_code = iPara.FunctionCode;
			if (function_code == 1 || function_code == 2 || function_code == 3 || function_code == 4)
			{
				// 讀取
				this->packet_make_function_read(function_code, iPara.Parameter, iRegBuffer, this->byte_buffer, byte_number);
			}
			else
			{
				// 寫入
				this->packet_make_function_write(function_code, iPara.DataAddres, iPara.Parameter, this->byte_buffer, byte_number);
			}
		}
		this->msg_make(this->byte_buffer, byte_number, oBuffer, oLength);
	}

	void Encoder_ASCII::packet_make_exception(int iFunctionCode, Exception iError, char* oBuffer, int &oLength)
	{
		oBuffer[INDEX_SLAVE_ID] = (char)this->slave_id;
		oBuffer[INDEX_FUNCTION_CODE] = iFunctionCode | 0x80;
		oBuffer[INDEX_EXCEPTION_CODE] = (char)iError;

		// LRC
		oBuffer[3] = LRC(oBuffer, 3);
		oLength = 4;
	}

	void Encoder_ASCII::packet_make_function_read(int iFunctionCode, int iRegsNumber, char* iRegBuffer, char* oBuffer, int &oLength)
	{
		oBuffer[INDEX_SLAVE_ID] = (char)this->slave_id;
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
		// LRC
		oBuffer[bytes_number + 3] = LRC(oBuffer, 3 + bytes_number);
		/* ---------------------------------- */
		oLength = bytes_number + 4; // 3(id, code, num) + bytes_number + 1(LRC)
		/* ---------------------------------- */
	}

	void Encoder_ASCII::packet_make_function_write(int iFunctionCode, int iAddress, int iParameter, char* oBuffer, int &oLength)
	{
		oBuffer[INDEX_SLAVE_ID] = (char)this->slave_id;
		oBuffer[INDEX_FUNCTION_CODE] = iFunctionCode;

		tu_convert16 cTool;
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
		// LRC
		oBuffer[6] = LRC(oBuffer, 6);
		/* ---------------------------------- */
		oLength = 7;
		/* --------------------------------- */
	}

	void Encoder_ASCII::msg_make(char* byte_buffer, int byte_number, char* oBuffer, int & oLength)
	{
		sprintf(oBuffer, ":");
		char cat_str[3];
		for (int byte_idx = 0; byte_idx < byte_number; byte_idx++)
		{
			sprintf(cat_str, "%02X", *byte_buffer++);
			strcat(oBuffer, cat_str);
		}
		strcat(oBuffer, "\r\n");
		oLength = byte_number * 2 + 3;
	}
#endif

#if ARC_Modbus_TCP_ENABLED > 0
	Encoder_TCP::Encoder_TCP()
	{

	}

	Encoder_TCP::~Encoder_TCP()
	{

	}

	void Encoder_TCP::GetResponse(char* oBuffer, int &oLength, /* output */ char* iRegBuffer, const Command_Parameter &iPara, Exception iError /* exceptions 錯誤訊息 */)
	{
		if (iError != Ex_none)
		{//有錯誤訊息 製作相對應的例外訊息
			this->packet_make_exception(iPara.TransID, iPara.FunctionCode, iError, oBuffer, oLength);
		}
		else
		{
			int function_code = iPara.FunctionCode;
			if (function_code == 1 || function_code == 2 || function_code == 3 || function_code == 4)
			{
				// 讀取
				this->packet_make_function_read(iPara.TransID, function_code, iPara.Parameter, iRegBuffer, oBuffer, oLength);
			}
			else
			{
				// 寫入
				this->packet_make_function_write(iPara.TransID, function_code, iPara.DataAddres, iPara.Parameter, oBuffer, oLength);
			}
		}
	}

	void Encoder_TCP::packet_make_exception(int TransID, int iFunctionCode, Exception iError, char* oBuffer, int &oLength)
	{
		this->header_make(TransID, 3, oBuffer);
		
		// Function Code
		static const int tcp_idx_function_code = LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE;
		oBuffer[tcp_idx_function_code] = iFunctionCode | 0x80;

		// Exception Code
		static const int tcp_idx_except_code = LENGTH_MBAP_HEADER + INDEX_EXCEPTION_CODE;
		oBuffer[tcp_idx_except_code] = (char)iError;

		oLength = 9;
	}

	void Encoder_TCP::packet_make_function_read(int TransID, int iFunctionCode, int iRegsNumber, char* iRegBuffer, char* oBuffer, int &oLength)
	{
		// Function Code
		static const int tcp_idx_function_code = LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE;
		oBuffer[tcp_idx_function_code] = iFunctionCode;

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

		static const int idx_num_byte_follow = LENGTH_MBAP_HEADER + INDEX_NUMBER_BYTES_FOLLOW_r;
		oBuffer[idx_num_byte_follow] = bytes_number;

		static const int idx_data_start = LENGTH_MBAP_HEADER + INDEX_DATA_START_r;
		memcpy(&oBuffer[idx_data_start], iRegBuffer, bytes_number);

		/* ---------------------------------- */
		/*	2(TransID),
			2(ProtocolID),
			2(MsgLength)
			3(id, code, num)
			+ bytes_number */
		this->header_make(TransID, bytes_number + 3, oBuffer);
		oLength = bytes_number + 9; 
		/* ---------------------------------- */
	}

	void Encoder_TCP::packet_make_function_write(int TransID, int iFunctionCode, int iAddress, int iParameter, char* oBuffer, int &oLength)
	{
		tu_convert16 cTool;

		this->header_make(TransID, 6, oBuffer);

		// Function Code
		static const int tcp_idx_function_code = LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE;
		oBuffer[tcp_idx_function_code] = iFunctionCode;

		/* --------------------------------- */
		// The Data Address
		cTool.wValue = iAddress;
		static const int tcp_idx_data_addr_l = LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1;
		static const int tcp_idx_data_addr_h = LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS;
		oBuffer[tcp_idx_data_addr_l] = cTool.cValue[0];
		oBuffer[tcp_idx_data_addr_h] = cTool.cValue[1];
		/* --------------------------------- */
		// The Parameter
		cTool.wValue = iParameter;
		static const int tcp_idx_para_l = LENGTH_MBAP_HEADER + INDEX_PARAMETER + 1;
		static const int tcp_idx_para_h = LENGTH_MBAP_HEADER + INDEX_PARAMETER;
		oBuffer[tcp_idx_para_l] = cTool.cValue[0];
		oBuffer[tcp_idx_para_h] = cTool.cValue[1];
		/* --------------------------------- */
		oLength = 12;
		/* --------------------------------- */
	}

	void Encoder_TCP::header_make(int TransID, int iMsgLength, char* oBuffer)
{
		tu_convert16 cTool;

		// Transaction Identifier
		cTool.wValue = TransID;
		static const int idx_trans_id_l = INDEX_TRANS_ID + 1;
		static const int idx_trans_id_h = INDEX_TRANS_ID;
		oBuffer[idx_trans_id_l] = cTool.cValue[0];
		oBuffer[idx_trans_id_h] = cTool.cValue[1];

		// Protocol Identifier (always 0)
		static const int idx_protocol_id_l = INDEX_PROTOCOL_ID + 1;
		static const int idx_protocol_id_h = INDEX_PROTOCOL_ID;
		oBuffer[idx_protocol_id_l] = 0;
		oBuffer[idx_protocol_id_h] = 0;

		// Message Length
		cTool.wValue = iMsgLength;
		static const int idx_msg_len_l = INDEX_MESSAGE_LENGTH + 1;
		static const int idx_msg_len_h = INDEX_MESSAGE_LENGTH;
		oBuffer[idx_msg_len_l] = cTool.cValue[0];
		oBuffer[idx_msg_len_h] = cTool.cValue[1];

		// Slave ID
		static const int tcp_idx_slave_id = LENGTH_MBAP_HEADER + INDEX_SLAVE_ID;
		oBuffer[tcp_idx_slave_id] = (char)this->slave_id;
	}

#endif

	

}
