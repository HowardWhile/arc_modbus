#include "ARC_Modbus_Decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace ARC_Modbus
{
#if ARC_Modbus_RTU_ENABLED > 0
	Decoder_RTU::Decoder_RTU()
	{
		this->buffer_enable = false;
	}

	Decoder_RTU::~Decoder_RTU()
	{
	}

	bool Decoder_RTU::RxByte(char iByte)
	{
		/* -------------------------------------------- */
		if (this->function0106_decoder(iByte) == true)
			return true;
		/* -------------------------------------------- */
		if (this->function1516_decoder(iByte) == true)
			return true;
		/* -------------------------------------------- */

		return false;
	}

	bool Decoder_RTU::function0106_decoder(char iByte)
	{
		/* Function Code 01~06 長度固定用 memmove decode */
		memmove(this->rx_buffer_0106, this->rx_buffer_0106 + 1, sizeof(this->rx_buffer_0106));
		this->rx_buffer_0106[sizeof(rx_buffer_0106) - 1] = iByte;

		if (this->rx_buffer_0106[0] == this->slave_id)
		{
			tu_convert16 cTool;
			cTool.wValue = CRC16(this->rx_buffer_0106, 6);

			if (cTool.cValue[0] == this->rx_buffer_0106[6] &&
				cTool.cValue[1] == this->rx_buffer_0106[7]) // crc16
			{
				this->SlaveAddress = this->slave_id;
				this->FunctionCode = this->rx_buffer_0106[INDEX_FUNCTION_CODE];

				static const int idx_data_addr_l = INDEX_DATA_ADDRESS + 1;
				static const int idx_data_addr_h = INDEX_DATA_ADDRESS;
				cTool.cValue[0] = this->rx_buffer_0106[idx_data_addr_l];
				cTool.cValue[1] = this->rx_buffer_0106[idx_data_addr_h];
				this->DataAddres = cTool.wValue;

				static const int idx_para_l = INDEX_PARAMETER + 1;
				static const int idx_para_h = INDEX_PARAMETER;
				cTool.cValue[0] = this->rx_buffer_0106[idx_para_l];
				cTool.cValue[1] = this->rx_buffer_0106[idx_para_h];
				this->Parameter = cTool.wValue;

				return true;
			}
		}
		return false;
	}

	bool Decoder_RTU::function1516_decoder(char iByte)
	{
		if (this->buffer_enable)
		{
			this->byte_index++;
			this->rx_buffer_1516[this->buffer_index++] = iByte;

			if (this->byte_index >= 255)
			{
				this->buffer_enable = false;
			}
			else
			{
				// index 6: The number of data bytes to follow
				if (this->byte_index == (unsigned char)this->rx_buffer_1516[6] + 2) // 2: crc16 size
				{																	// 收取定義的長度
				  // 確認 crc16
					tu_convert16 cTool;
					cTool.wValue = CRC16(this->rx_buffer_1516, this->buffer_index - 2);
					if (cTool.cValue[0] == this->rx_buffer_1516[this->buffer_index - 2] &&
						cTool.cValue[1] == this->rx_buffer_1516[this->buffer_index - 1]) // crc16
					{
						/* --------------------------- */
						// 正確的封包
						/* --------------------------- */
						this->SlaveAddress = this->slave_id;
						this->FunctionCode = this->rx_buffer_1516[INDEX_FUNCTION_CODE];

						static const int idx_data_addr_l = INDEX_DATA_ADDRESS + 1;
						static const int idx_data_addr_h = INDEX_DATA_ADDRESS;
						cTool.cValue[0] = this->rx_buffer_1516[idx_data_addr_l];
						cTool.cValue[1] = this->rx_buffer_1516[idx_data_addr_h];
						this->DataAddres = cTool.wValue; // 位置

						static const int idx_para_l = INDEX_PARAMETER + 1;
						static const int idx_para_h = INDEX_PARAMETER;
						cTool.cValue[0] = this->rx_buffer_1516[idx_para_l];
						cTool.cValue[1] = this->rx_buffer_1516[idx_para_h];
						this->Parameter = cTool.wValue; // 數量

						/* --------------------------- */
						// 接收命令完成
						/* --------------------------- */
						this->buffer_enable = false;
						return true;
					}
				}
			}
		}

		/* --------------------------- */
		/* 判斷起始位元 */
		if (!this->buffer_enable)
		{
			if (this->check_1516_head(iByte))
			{
				this->buffer_enable = true;
				this->buffer_index = 7;
				this->byte_index = 0;
				memcpy(this->rx_buffer_1516, this->rx_buffer_1516_head, sizeof(this->rx_buffer_1516_head));
			}
		}
		/* --------------------------- */
		return false;
	}

	bool Decoder_RTU::check_1516_head(char iByte)
	{
		memmove(this->rx_buffer_1516_head, this->rx_buffer_1516_head + 1, sizeof(this->rx_buffer_1516_head));
		this->rx_buffer_1516_head[sizeof(rx_buffer_1516_head) - 1] = iByte;

		unsigned char function_code = this->rx_buffer_1516_head[INDEX_FUNCTION_CODE];
		if (this->rx_buffer_1516_head[0] == this->slave_id && /* Slave ID正確 */
			(function_code == 15 || function_code == 16))	  /* Function Code正確 */
		{
			tu_convert16 cTool;

			/* The number to written */
			static const int idx_num2write_l = INDEX_NUMBER_TO_WRITE + 1;
			static const int idx_num2write_h = INDEX_NUMBER_TO_WRITE;
			cTool.cValue[0] = this->rx_buffer_1516_head[idx_num2write_l];
			cTool.cValue[1] = this->rx_buffer_1516_head[idx_num2write_h];
			int number2written = cTool.wValue;

			if (function_code == 15 && number2written <= 2000)
			{
				unsigned char byte_length = number2written / 8 + (number2written % 8 != 0);
				if (byte_length == (unsigned char)this->rx_buffer_1516_head[INDEX_NUMBER_BYTES_FOLLOW_w])
					return true; // 是 function code 15 的開頭
			}
			else if (function_code == 16 && number2written <= 125)
			{
				unsigned char byte_length = number2written * 2;
				if (byte_length == (unsigned char)this->rx_buffer_1516_head[INDEX_NUMBER_BYTES_FOLLOW_w])
					return true; // 是 function code 16 的開頭
			}
		}
		return false;
	}

	char *Decoder_RTU::GetRegsBuffer(void)
	{
		return this->rx_buffer_1516 + INDEX_DATA_START_w;
	}
#endif

#if ARC_Modbus_ASCII_ENABLED > 0
	Decoder_ASCII::Decoder_ASCII()
	{
	}

	Decoder_ASCII::~Decoder_ASCII()
	{
	}

	bool Decoder_ASCII::RxByte(char iByte)
	{
		// printf("%c", iByte);
		if (this->buffer_enable)
		{
			this->rx_buffer[this->buffer_index++] = iByte;
			if (this->check_end(iByte) == true) /* "\r\n" */
			{
				//接收到一組命令(有開始字元':' 結束字元"\r\n")
				if (this->buffer_index >= 17 &&								  //最短的命令是17個字
					this->hexStr2Byte(this->rx_buffer + 1) == this->slave_id) // 判斷ID是否是本身
				{
					int idx_check_sum_str = this->buffer_index - 4;
					unsigned char check_sum = LRC_String(this->rx_buffer + 1, idx_check_sum_str - 1);

					// printf("[Debug] LRC: 0x%X\r\n", check_sum);
					if (check_sum == this->hexStr2Byte(this->rx_buffer + idx_check_sum_str))
					{																 // LRC 檢查通過
						this->FunctionCode = this->hexStr2Byte(this->rx_buffer + 3); // 3 = INDEX_FUNCTION_CODE * 2 + 1
						if (this->check_0106_packet())
						{
							this->buffer_enable = false;
							return true;
						}
						else if (this->check_1516_packet())
						{
							this->buffer_enable = false;
							return true;
						}
						else
						{
							// 錯誤的封包
							this->buffer_enable = false;
						}
					}
					else
					{
						// 錯誤的封包
						this->buffer_enable = false;
					}
				}
				else
				{
					// 錯誤的封包
					this->buffer_enable = false;
				}
			}
			else
			{
				if (this->buffer_index == sizeof(this->rx_buffer))
				{
					// 緩衝區滿了還收不到結束字元 拋棄該封包
					this->buffer_enable = false;
				}
			}
		}
		/* --------------------------- */
		/* 判斷起始位元 */
		/* --------------------------- */
		if (iByte == ':')
		{
			this->buffer_enable = true;
			this->buffer_index = 1;
			this->rx_buffer[0] = iByte;
		}
		return false;
	}

	char *Decoder_ASCII::GetRegsBuffer(void)
	{

		return this->reg_buffer;
	}

	bool Decoder_ASCII::check_end(char iByte)
	{
		// memmove(this->check_end_buffer, this->check_end_buffer + 1, sizeof(this->check_end_buffer));
		// this->check_end_buffer[sizeof(check_end_buffer) - 1] = iByte;
		this->check_end_buffer[0] = this->check_end_buffer[1];
		this->check_end_buffer[1] = iByte;
		if (this->check_end_buffer[0] == '\r' && this->check_end_buffer[1] == '\n')
		{
			return true;
		}
		return false;
	}

	unsigned char Decoder_ASCII::hexStr2Byte(const char iString[])
	{
		char str[3];

		memcpy(str, iString, 2);
		str[2] = 0;

		return (unsigned char)strtol(str, 0, 16);
	}

	unsigned short Decoder_ASCII::hexStr2Word(const char iString[])
	{
		char str[5];

		memcpy(str, iString, 4);
		str[4] = 0;

		return (unsigned short)strtol(str, 0, 16);
	}

	bool Decoder_ASCII::check_0106_packet()
	{
		this->FunctionCode = this->hexStr2Byte(this->rx_buffer + 3); // 3 = INDEX_FUNCTION_CODE * 2 + 1
		if (this->FunctionCode >= 1 && this->FunctionCode <= 6 && this->buffer_index == 17)
		{
			this->SlaveAddress = this->slave_id;

			// Data Address
			this->DataAddres = this->hexStr2Word(this->rx_buffer + 5); // 5 = INDEX_DATA_ADDRESS * 2 + 1

			// Parameter
			// (Function code = 1,2,3,4 : The total number of requested)
			// (Function code = 5,6 : The status to write)
			this->Parameter = this->hexStr2Word(this->rx_buffer + 9); // 9 = INDEX_PARAMETER * 2 + 1
			return true;
		}
		return false;
	}

	bool Decoder_ASCII::check_1516_packet()
	{
		if (this->FunctionCode == 15 || this->FunctionCode == 16)
		{
			/* 確認長度 */
			/* The number to written */
			static const int idx_num2write = INDEX_NUMBER_TO_WRITE * 2 + 1;
			unsigned short number2written = this->hexStr2Word(this->rx_buffer + idx_num2write);
			if (this->FunctionCode == 15 && number2written <= 2000)
			{
				unsigned char byte_length = number2written / 8 + (number2written % 8 != 0);

				return this->make_reg_buffer(byte_length, number2written);
			}
			else if (this->FunctionCode == 16 && number2written <= 125)
			{
				unsigned char byte_length = number2written * 2;

				return this->make_reg_buffer(byte_length, number2written);
			}
		}

		return false;
	}

	bool Decoder_ASCII::make_reg_buffer(unsigned char iByteFollow, unsigned short iNumber2Write)
	{
		if (iByteFollow == (unsigned char)this->hexStr2Byte(this->rx_buffer + 13) && // 13 = INDEX_NUMBER_BYTES_FOLLOW *2  + 1
			this->buffer_index == iByteFollow * 2 + 19)								 // 資料總長度也正確
		{
			this->SlaveAddress = this->slave_id;

			// Data Address
			this->DataAddres = this->hexStr2Word(this->rx_buffer + 5); // 5 = INDEX_DATA_ADDRESS * 2 + 1

			// Parameter
			this->Parameter = iNumber2Write;

			// make reg buffer
			for (int byte_idx = 0; byte_idx < iByteFollow; byte_idx++)
			{
				this->reg_buffer[byte_idx] = this->hexStr2Byte(this->rx_buffer + 15 + byte_idx * 2); // 15 =  INDEX_DATA_START * 2 + 1
			}

			return true;
		}

		return false;
	}
#endif

#if ARC_Modbus_TCP_ENABLED > 0
	Decoder_TCP::Decoder_TCP()
	{
		this->buffer_index = 0;
	}

	Decoder_TCP::~Decoder_TCP()
	{
	}

	void Decoder_TCP::RxByte(char iByte)
	{
		if (this->buffer_index < sizeof(this->rx_buffer))
		{
			this->rx_buffer[this->buffer_index++] = iByte;
		}
	}

	char *Decoder_TCP::GetRegsBuffer(void)
	{
		static const int idx_data_start = LENGTH_MBAP_HEADER + INDEX_DATA_START_w;
		return this->rx_buffer + idx_data_start;
	}

	bool Decoder_TCP::FrameRenew(void)
	{

		if (this->buffer_index > 0)
		{
			/*printf("[debug] size: (%d) ", this->buffer_index);
			for (int i = 0; i < this->buffer_index; i++)
			{
				printf("%02X ", this->rx_buffer[i]);
			}
			printf("\r\n");//*/

			if (this->check_0106_packet() == true)
			{
				this->buffer_index = 0;
				return true;
			}

			if (this->check_1516_packet() == true)
			{
				this->buffer_index = 0;
				return true;
			}
		}

		this->buffer_index = 0;
		return false;
	}

	bool Decoder_TCP::check_0106_packet()
	{
		static const int tcp_idx_slave_id = LENGTH_MBAP_HEADER + INDEX_SLAVE_ID;
		static const int tcp_idx_function_code = LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE;
		static const int tcp_idx_msg_len = INDEX_MESSAGE_LENGTH + 1;

		if (this->buffer_index == 12 &&							   // 01~06 的 封包長度一定是12
			this->rx_buffer[tcp_idx_slave_id] == this->slave_id && // slave ID 正不正確
			this->rx_buffer[tcp_idx_function_code] >= 1 &&		   // Function Code 1~6
			this->rx_buffer[tcp_idx_function_code] <= 6 &&		   // Function Code 1~6
			this->rx_buffer[tcp_idx_msg_len] == 6				   // 6 bytes to follow
		)
		{
			tu_convert16 cTool;
			static const int tcp_idx_protocol_id_l = INDEX_PROTOCOL_ID + 1;
			static const int tcp_idx_protocol_id_h = INDEX_PROTOCOL_ID;
			cTool.cValue[0] = this->rx_buffer[tcp_idx_protocol_id_l];
			cTool.cValue[1] = this->rx_buffer[tcp_idx_protocol_id_h];
			if (cTool.wValue == 0) // Protocol Identifier == 0
			{					   // 以上條件都成立 認定是 TCP Modbus Function Cdoe 01~06 之封包

				// Transaction Identifier
				static const int tcp_idx_trans_id_l = INDEX_TRANS_ID + 1;
				static const int tcp_idx_trans_id_h = INDEX_TRANS_ID;
				cTool.cValue[0] = this->rx_buffer[tcp_idx_trans_id_l];
				cTool.cValue[1] = this->rx_buffer[tcp_idx_trans_id_h];
				this->TransID = cTool.wValue;

				// UnitID
				this->SlaveAddress = this->slave_id;

				// Function Code
				this->FunctionCode = this->rx_buffer[tcp_idx_function_code];

				// 位置
				static const int tcp_idx_data_addr_l = LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1;
				static const int tcp_idx_data_addr_h = LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS;
				cTool.cValue[0] = this->rx_buffer[tcp_idx_data_addr_l];
				cTool.cValue[1] = this->rx_buffer[tcp_idx_data_addr_h];
				this->DataAddres = cTool.wValue;

				// Parameter
				static const int tcp_idx_para_l = LENGTH_MBAP_HEADER + INDEX_PARAMETER + 1;
				static const int tcp_idx_para_h = LENGTH_MBAP_HEADER + INDEX_PARAMETER;
				cTool.cValue[0] = this->rx_buffer[tcp_idx_para_l];
				cTool.cValue[1] = this->rx_buffer[tcp_idx_para_h];
				this->Parameter = cTool.wValue;

				return true;
			}
		}
		return false;
	}

	bool Decoder_TCP::check_1516_packet()
	{

		static const int tcp_idx_slave_id = LENGTH_MBAP_HEADER + INDEX_SLAVE_ID;
		static const int tcp_idx_function_code = LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE;

		if (this->buffer_index >= 14 &&							   // 最少的資料長度是14 byte
			this->rx_buffer[tcp_idx_slave_id] == this->slave_id && // slave ID 正不正確
			(this->rx_buffer[tcp_idx_function_code] == 15 ||	   // Function Code 15
			 this->rx_buffer[tcp_idx_function_code] == 16)		   // Function Code 16
		)
		{
			tu_convert16 cTool;
			static const int tcp_idx_protocol_id_l = INDEX_PROTOCOL_ID + 1;
			static const int tcp_idx_protocol_id_h = INDEX_PROTOCOL_ID;
			cTool.cValue[0] = this->rx_buffer[tcp_idx_protocol_id_l];
			cTool.cValue[1] = this->rx_buffer[tcp_idx_protocol_id_h];
			if (cTool.wValue == 0) // Protocol Identifier == 0
			{
				static const int tcp_idx_msg_len_l = INDEX_MESSAGE_LENGTH + 1;
				static const int tcp_idx_msg_len_h = INDEX_MESSAGE_LENGTH;
				static const int tcp_idx_data_follow = LENGTH_MBAP_HEADER + INDEX_NUMBER_BYTES_FOLLOW_w;
				cTool.cValue[0] = this->rx_buffer[tcp_idx_msg_len_l];
				cTool.cValue[1] = this->rx_buffer[tcp_idx_msg_len_h];

				// TCP封包Header中描述之後訊息長度部分
				unsigned short msg_follow = (unsigned short)cTool.wValue;

				// Modbus封包中描述之後資料的長度部分
				unsigned short data_follow = (unsigned short)rx_buffer[tcp_idx_data_follow];

				// (7 是底下幾個資料的長度)
				// 1 byte, The Unit Identifier (ID)
				// 1 byte, Function Code
				// 2 byte, The Data Address
				// 2 byte, The number to write
				// 1 byte, The number of data to follow
				if ((msg_follow - 7) == data_follow) // 確認第一個長度描述吻合
				{
					static const int tcp_num2write_l = LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE + 1;
					static const int tcp_num2write_h = LENGTH_MBAP_HEADER + INDEX_NUMBER_TO_WRITE;
					cTool.cValue[0] = this->rx_buffer[tcp_num2write_l];
					cTool.cValue[1] = this->rx_buffer[tcp_num2write_h];
					unsigned short number2written = cTool.wValue;
					unsigned char byte_length;

					// Function Code
					this->FunctionCode = this->rx_buffer[tcp_idx_function_code];

					if (this->FunctionCode == 15 && number2written <= 2000)
						byte_length = number2written / 8 + (number2written % 8 != 0);
					else if (this->FunctionCode == 16 && number2written <= 125)
						byte_length = number2written * 2;
					else
						return false; // 看樣子長度不太符合規範

					if (byte_length == data_follow) // 確認第二個長度描述吻合
					{
						// 以上條件都成立了 認定是 TCP Modbus Function Cdoe 15 or 16 之封包
						// Transaction Identifier
						static const int tcp_idx_trans_id_l = INDEX_TRANS_ID + 1;
						static const int tcp_idx_trans_id_h = INDEX_TRANS_ID;
						cTool.cValue[0] = this->rx_buffer[tcp_idx_trans_id_l];
						cTool.cValue[1] = this->rx_buffer[tcp_idx_trans_id_h];
						this->TransID = cTool.wValue;

						// UnitID
						this->SlaveAddress = this->slave_id;

						// 位置
						static const int tcp_idx_data_addr_l = LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1;
						static const int tcp_idx_data_addr_h = LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS;
						cTool.cValue[0] = this->rx_buffer[tcp_idx_data_addr_l];
						cTool.cValue[1] = this->rx_buffer[tcp_idx_data_addr_h];
						this->DataAddres = cTool.wValue;

						// 數量
						this->Parameter = number2written;
						return true;

					} // end of 以上條件都成立了 認定是 TCP Modbus Function Cdoe 15 or 16 之封包
				}	  // end of 確認長度是否吻合
			}		  // end of if (cTool.wValue == 0)
		}			  // end of 一拖拉庫封包正確性判斷
		return false;
	}

#endif

}
