#ifndef ARC_Modbus_Packet_h__
#define ARC_Modbus_Packet_h__

#include "ARC_Modbus_Struct.h"

namespace ARC_Modbus
{
#if ARC_Modbus_RTU_ENABLED > 0
	class Decoder_RTU : public coder_base, public Command_Parameter
	{
	public:
		Decoder_RTU();
		~Decoder_RTU();
		bool RxByte(char iByte);
		char* GetRegsBuffer(void); // 多重寫入(15,16)的資料由此取得
	private:
		// RTU Frame Format:		
		// 0:	The Slave Address
		// 1:	The Function Code
		// 2~3:	The Data Address 

		// (F:01~06)
		// 4~5: Parameter 
		// 6~7: CRC 
		// total size: 8 Bytes
		bool function0106_decoder(char iByte); // return true: get one new frame
		char rx_buffer_0106[8]; //給Function code 01~06使用的buffer

		// (F:15~16)
		// 4~5:			Number to write
		// 6:			The number of data bytes to follow (max: 250)
		// 7 ~ 6+n:		write data (n = 1~250)
		// 6+n (+1~+2):	CRC 
		// total size:	10 ~ 259 byte
		char rx_buffer_1516_head[7]; // 記錄前7個byte 用於判斷起始位元
		char rx_buffer_1516[259];
		int buffer_index;
		int byte_index;
		bool buffer_enable;
		bool function1516_decoder(char iByte);
		bool check_1516_head(char iByte);
	};
#endif

#if ARC_Modbus_ASCII_ENABLED > 0
	class Decoder_ASCII : public coder_base, public Command_Parameter
	{
	public:
		Decoder_ASCII();
		~Decoder_ASCII();
		bool RxByte(char iByte);
		char* GetRegsBuffer(void); // 多重寫入(15,16)的資料由此取得

	private:

		// http://www.simplymodbus.ca/ASCII.htm
		// 1 byte, Start character ':'
		// 2 byte, The Slave Address
		// 2 byte, The Function Code
		// 4 byte, The Data Address
		// 4 byte, The number to write
		// 2 byte, The number of data to follow
		// 2~500 byte, Data
		// 1 byte, LRC
		// 2 byte, End character '\r\n'
		// total: 20 ~ 518 byte
		char rx_buffer[518];
		int buffer_index;
		bool buffer_enable;
		bool check_0106_packet();  //辨識 rx_buffer 中的01~06命令
		bool check_1516_packet();  //辨識 rx_buffer 中的15,16命令
		bool make_reg_buffer(unsigned char iByteFollow, unsigned short iNumber2Write);

		char check_end_buffer[2]; // "\r\n"
		bool check_end(char iByte);

		char reg_buffer[250];
		unsigned char hexStr2Byte(const char iString[]); // 將byte字元(2個字)合併成一個byte資料
		unsigned short hexStr2Word(const char iString[]); // 將word字元(4個字)合併成一個word資料
	};
#endif

#if ARC_Modbus_TCP_ENABLED > 0
	class Decoder_TCP : public coder_base, public Command_Parameter
	{
	public:
		Decoder_TCP();
		~Decoder_TCP();
		void RxByte(char iByte);
		char* GetRegsBuffer(void); // 多重寫入(15,16)的資料由此取得
		bool FrameRenew(void); // TCP decoder 開始運作

	private:
		// 2 byte, Transaction Identifier 
		// 2 byte, Protocol Identifier (0x0000)
		// 2 byte, Message Length
		// 1 byte, The Unit Identifier (ID)
		// 1 byte, Function Code
		// 2 byte, The Data Address
		// 2 byte, The number to write
		// 1 byte, The number of data to follow
		// 1~250 byte, Data
		// total: 14 ~ 263 bytes
		char rx_buffer[263];
		int buffer_index;
		bool check_0106_packet();  //辨識 rx_buffer 中的01~06命令
		bool check_1516_packet();  //辨識 rx_buffer 中的15,16命令
	};
#endif

}


#endif // ARC_Modbus_Packet_h__
