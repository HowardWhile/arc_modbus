#include "ARC_Modbus_Request.h"
//中文字
namespace ARC_Modbus {

#if ARC_Modbus_Master_RTU_ENABLED > 0
	Request_RTU::Request_RTU()
	{

	}

	Request_RTU::~Request_RTU()
	{

	}
#endif // ARC_Modbus_Master_RTU_ENABLED

#if ARC_Modbus_Master_ASCII_ENABLED  > 0
	Request_ASCII::Request_ASCII()
	{

	}

	Request_ASCII::~Request_ASCII()
	{

	}
#endif // ARC_Modbus_Master_ASCII_ENABLED

#if ARC_Modbus_Master_TCP_ENABLED > 0
	Request_TCP::Request_TCP()
	{
		this->trans_id = 0;
	}

	Request_TCP::~Request_TCP()
	{

	}

	ARC_Modbus::pkg Request_TCP::Read(unsigned char iID, unsigned char iFunctionCode, unsigned short iAddress, unsigned short iNumber)
	{
		pkg rValue;
		rValue.datas = this->ByteBuffer_ptr;
		if (((iFunctionCode == 1 || iFunctionCode == 2) && iNumber <= 2000) ||
			((iFunctionCode == 3 || iFunctionCode == 4) && iNumber <= 125))
		{
			tu_convert16 cTool;
			cTool.wValue = this->trans_id++;
			rValue.datas[INDEX_TRANS_ID] = cTool.cValue[1];
			rValue.datas[INDEX_TRANS_ID + 1] = cTool.cValue[0];

			rValue.datas[INDEX_PROTOCOL_ID] = 0;
			rValue.datas[INDEX_PROTOCOL_ID + 1] = 0;

			cTool.wValue = 6; // msg_len;
			rValue.datas[INDEX_MESSAGE_LENGTH] = cTool.cValue[1];
			rValue.datas[INDEX_MESSAGE_LENGTH + 1] = cTool.cValue[0];

			rValue.datas[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] = iID;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE] = iFunctionCode;

			cTool.wValue = iAddress;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1] = cTool.cValue[0];

			cTool.wValue = iNumber;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER + 1] = cTool.cValue[0];

			rValue.size = 12;
		}
		else
		{
			rValue.size = 0;
		}
		return rValue;
	}

	ARC_Modbus::pkg Request_TCP::Write(unsigned char iID, unsigned short iAddress, bool iValue)
	{
		pkg rValue;
		rValue.datas = this->ByteBuffer_ptr;
		tu_convert16 cTool;
		cTool.wValue = this->trans_id++;
		rValue.datas[INDEX_TRANS_ID] = cTool.cValue[1];
		rValue.datas[INDEX_TRANS_ID + 1] = cTool.cValue[0];

		rValue.datas[INDEX_PROTOCOL_ID] = 0;
		rValue.datas[INDEX_PROTOCOL_ID + 1] = 0;

		cTool.wValue = 6; // msg_len;
		rValue.datas[INDEX_MESSAGE_LENGTH] = cTool.cValue[1];
		rValue.datas[INDEX_MESSAGE_LENGTH + 1] = cTool.cValue[0];

		rValue.datas[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] = iID;
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE] = 05;

		cTool.wValue = iAddress;
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS] = cTool.cValue[1];
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1] = cTool.cValue[0];

		if (iValue)
		{
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER] = 0xFF;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER + 1] = 0x00;
		}
		else
		{
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER] = 0x00;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER + 1] = 0x00;
		}
		rValue.size = 12;

		return rValue;
	}

	ARC_Modbus::pkg Request_TCP::Write(unsigned char iID, unsigned short iAddress, short iValue)
	{
		pkg rValue;
		rValue.datas = this->ByteBuffer_ptr;

		tu_convert16 cTool;
		cTool.wValue = this->trans_id++;
		rValue.datas[INDEX_TRANS_ID] = cTool.cValue[1];
		rValue.datas[INDEX_TRANS_ID + 1] = cTool.cValue[0];

		rValue.datas[INDEX_PROTOCOL_ID] = 0;
		rValue.datas[INDEX_PROTOCOL_ID + 1] = 0;

		cTool.wValue = 6; // msg_len;
		rValue.datas[INDEX_MESSAGE_LENGTH] = cTool.cValue[1];
		rValue.datas[INDEX_MESSAGE_LENGTH + 1] = cTool.cValue[0];

		rValue.datas[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] = iID;
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE] = 06;

		cTool.wValue = iAddress;
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS] = cTool.cValue[1];
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1] = cTool.cValue[0];

		cTool.wValue = iValue;
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER] = cTool.cValue[1];
		rValue.datas[LENGTH_MBAP_HEADER + INDEX_PARAMETER + 1] = cTool.cValue[0];

		rValue.size = 12;

		return rValue;
	}

	ARC_Modbus::pkg Request_TCP::Write(unsigned char iID, unsigned short iAddress, bool* iValues, unsigned short iNumber)
	{
		pkg rValue;
		rValue.datas = this->ByteBuffer_ptr;
		if (0 < iNumber && iNumber <= 2000)// 最多bit寫入數量是2000
		{
			tu_convert16 cTool;
			cTool.wValue = this->trans_id++;
			rValue.datas[INDEX_TRANS_ID] = cTool.cValue[1];
			rValue.datas[INDEX_TRANS_ID + 1] = cTool.cValue[0];

			rValue.datas[INDEX_PROTOCOL_ID] = 0;
			rValue.datas[INDEX_PROTOCOL_ID + 1] = 0;

			unsigned char byte_follow = iNumber / 8 + (iNumber % 8 != 0);
			
			cTool.wValue = 7 + byte_follow; // msg_len;
			rValue.datas[INDEX_MESSAGE_LENGTH] = cTool.cValue[1];
			rValue.datas[INDEX_MESSAGE_LENGTH + 1] = cTool.cValue[0];
			
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] = iID;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE] = 15;

			cTool.wValue = iAddress;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1] = cTool.cValue[0];

			cTool.wValue = iNumber;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE + 1] = cTool.cValue[0];

			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_BYTES_FOLLOW_w] = byte_follow;

			char* coils_byte = &rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_START_w];
			for (int i = 0; i < iNumber; i++)
			{
				this->set_bit(coils_byte, i, iValues[i]);
			}

			//LENGTH_MBAP_HEADER(6) + ID(1) + FC(1) + Addr(2) + Num(2) + byte_follow(1) = 13;
			rValue.size = 13 + byte_follow;
		}
		else
		{
			rValue.size = 0;
		}
		return rValue;
	}

	ARC_Modbus::pkg Request_TCP::Write(unsigned char iID, unsigned short iAddress, char* iValues, unsigned short iNumber)
	{
		pkg rValue;
		rValue.datas = this->ByteBuffer_ptr;
		if (0 < iNumber && iNumber <= 2000)// 最多bit寫入數量是2000
		{
			tu_convert16 cTool;
			cTool.wValue = this->trans_id++;
			rValue.datas[INDEX_TRANS_ID] = cTool.cValue[1];
			rValue.datas[INDEX_TRANS_ID + 1] = cTool.cValue[0];

			rValue.datas[INDEX_PROTOCOL_ID] = 0;
			rValue.datas[INDEX_PROTOCOL_ID + 1] = 0;

			unsigned char byte_follow = iNumber / 8 + (iNumber % 8 != 0);

			cTool.wValue = 7 + byte_follow; // msg_len;
			rValue.datas[INDEX_MESSAGE_LENGTH] = cTool.cValue[1];
			rValue.datas[INDEX_MESSAGE_LENGTH + 1] = cTool.cValue[0];

			rValue.datas[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] = iID;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE] = 15;

			cTool.wValue = iAddress;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1] = cTool.cValue[0];

			cTool.wValue = iNumber;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE + 1] = cTool.cValue[0];

			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_BYTES_FOLLOW_w] = byte_follow;

			char* coils_byte = &rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_START_w];
			for (int i = 0; i < byte_follow; i++)
			{
				coils_byte[i] = iValues[i];
			}

			//LENGTH_MBAP_HEADER(6) + ID(1) + FC(1) + Addr(2) + Num(2) + byte_follow(1) = 13;
			rValue.size = 13 + byte_follow;
		}
		else
		{
			rValue.size = 0;
		}
		return rValue;
	}


	ARC_Modbus::pkg Request_TCP::Write(unsigned char iID, unsigned short iAddress, short* iValues, unsigned short iNumber)
	{
		pkg rValue;
		rValue.datas = this->ByteBuffer_ptr;
		if (0 < iNumber && iNumber <= 125)		// 最多數值寫入數量是125
		{
			tu_convert16 cTool;
			cTool.wValue = this->trans_id++;
			rValue.datas[INDEX_TRANS_ID] = cTool.cValue[1];
			rValue.datas[INDEX_TRANS_ID + 1] = cTool.cValue[0];

			rValue.datas[INDEX_PROTOCOL_ID] = 0;
			rValue.datas[INDEX_PROTOCOL_ID + 1] = 0;

			unsigned char byte_follow = iNumber*2;

			cTool.wValue = 7 + byte_follow; // msg_len;
			rValue.datas[INDEX_MESSAGE_LENGTH] = cTool.cValue[1];
			rValue.datas[INDEX_MESSAGE_LENGTH + 1] = cTool.cValue[0];

			rValue.datas[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] = iID;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE] = 16;

			cTool.wValue = iAddress;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1] = cTool.cValue[0];

			cTool.wValue = iNumber;
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE] = cTool.cValue[1];
			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE + 1] = cTool.cValue[0];

			rValue.datas[LENGTH_MBAP_HEADER + INDEX_NUMBER_BYTES_FOLLOW_w] = byte_follow;

			int regs_idx = LENGTH_MBAP_HEADER + INDEX_DATA_START_w;
			for (int i = 0; i < iNumber; i++)
			{
				cTool.wValue = iValues[i];
				rValue.datas[regs_idx++] = cTool.cValue[1];
				rValue.datas[regs_idx++] = cTool.cValue[0];
			}

			rValue.size = 13 + byte_follow;
		}
		else
		{
			rValue.size = 0;
		}

		return rValue;
	}
#endif // ARC_Modbus_Master_TCP_ENABLED

}