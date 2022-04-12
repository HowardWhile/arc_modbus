#ifndef ARC_Modbus_h__
#define ARC_Modbus_h__

// 開發者版本 v0.2.1 by Howard 519-2131

// 具有 RTU, ASCII, TCP的通訊協定
// 下列的 Function Code 是有實作的
// 01: Read Coil Status
// 02: Read Input Coil Status (Read only)
// 03: Read Registers
// 04: Read Input Registers (Read only)
// 05: Force Single Coil
// 06: Preset Single Register
// 15: Force Multiple Coils
// 16: Preset Multiple Registers

// 如果想要節省 Code Size, 可以至ARC_Modbus_Struct.h, 將沒有用到的物件Disable

#include "ARC_Modbus_Decoder.h"
#include "ARC_Modbus_Encoder.h"
#include "ARC_Modbus_Request.h"
#include "ARC_Modbus_Response.h"
#include "ARC_Modbus_Struct.h"

namespace ARC_Modbus
{

#if ARC_Modbus_RTU_ENABLED > 0
	// modbus slave for RTU mode 
	class RTU : public modbus_slave_base
	{
	public:
		/* -------------------------------- */
		RTU();
		~RTU();
		/* -------------------------------- */
		void Initial(int iSlaveID);
		/* -------------------------------- */
		/* 與硬體層連接的函數 */
		// recv, Return true if decode is passed
		void RxWork(char* iBytes, int iLength);
		void RxWork(char iByte);
		/* -------------------------------- */
	private: 
		Decoder_RTU decoder;
		Encoder_RTU ecoder;

		// http://www.simplymodbus.ca/FC05.htm
		// 1 byte, The Slave Address
		// 1 byte, The Function Code
		// 1 byte, The number of data bytes
		// 1~250 byte, data
		// 2 byte, crc16
		// sum: 6~255 byte
		char tx_msg_buffer[255];
	};
#endif

#if ARC_Modbus_ASCII_ENABLED > 0
	// modbus slave for ascii mode 
	class ASCII : public modbus_slave_base
	{
	public:
		/* -------------------------------- */
		ASCII();
		~ASCII();
		/* -------------------------------- */
		void Initial(int iSlaveID);
		/* -------------------------------- */
		/* 與硬體層連接的函數 */
		// recv
		void RxWork(char* iBytes, int iLength);
		void RxWork(char iByte);
		/* -------------------------------- */
	private:
		Decoder_ASCII decoder;
		Encoder_ASCII ecoder;

		// http://www.simplymodbus.ca/ASCII.htm
		// 1 byte, Start character ':'
		// 2 byte, The Slave Address
		// 2 byte, The Function Code
		// 2 byte, The number of data bytes
		// 2~500 byte, Data
		// 2 byte, LRC
		// 2 byte, End character '\r\n'
		// 1 byte, '\0' 
		// sum: 13~512 byte
		char tx_msg_buffer[512];
	};
#endif

#if ARC_Modbus_TCP_ENABLED > 0
	// modbus slave for tcp mode 
	class TCP : public modbus_slave_base
	{
	public:
		TCP();
		~TCP();
		/* -------------------------------- */
		void Initial(int iSlaveID);
		/* -------------------------------- */
		/* 與硬體層連接的函數 */
		// recv
		void RxWork(char* iBytes, int iLength);
		void RxWork(char iByte);
		/* -------------------------------- */
		// 這是TCP才有的機制
		void FrameRenew(void); // 每當收取到一筆(幀) TCP/IP Wrapper 
		/* -------------------------------- */

	private:

		Decoder_TCP decoder;
		Encoder_TCP ecoder;
		
		// http://www.simplymodbus.ca/TCP.htm
		// 2 byte, Transaction Identifier
		// 2 byte, Protocol Identifier
		// 2 byte, Message Length

		// 1 byte, The Slave Address
		// 1 byte, The Function Code
		// 1 byte, The number of data bytes
		// 1~250 byte, data
		// sum: 10~259 byte
		char tx_msg_buffer[259];
	};
#endif

#if ARC_Modbus_Master_RTU_ENABLED > 0
	class RTU_Master : public modbus_master_base
	{
	public:
		RTU_Master();
		~RTU_Master();		
		void Polling(void); //主迴圈進入點

	private:
		Request_RTU request;
		Response_RTU response;
		
		// request(tx msg) max length
		// FC 01~06 : 8
		// FC 15 16 : 259 (!)

		// response(rx msg) max length
		// FC 01~04 : 255
		// FC 05 06 : 8
		// FC 15 16 : 8
		char byte_buffer[259];

	};
#endif // ARC_Modbus_Master_RTU_ENABLED

#if ARC_Modbus_Master_ASCII_ENABLED > 0

	class ASCII_Master : public modbus_master_base
	{
	public:
		ASCII_Master();
		~ASCII_Master();
		void Polling(void); //主迴圈進入點

	private:
		Request_ASCII request;
		Response_ASCII response;

		// request(tx msg) max length
		// FC 01~06 : 17
		// FC 15 16 : 519 (!)

		// response(rx msg) max length
		// FC 01~04 : 511
		// FC 05 06 : 17
		// FC 15 16 : 17
		char byte_buffer[520]; // 多一個byte來放'\0';

	};
#endif // ARC_Modbus_Master_ASCII_ENABLED

#if ARC_Modbus_Master_TCP_ENABLED > 0
	class TCP_Master : public modbus_master_base
	{
	public:
		TCP_Master();
		~TCP_Master();

		/* -------------------------------- */
		/* 與硬體層連接的函數 */
		// recv
		void RxWork(char* iBytes, int iLength);
		void RxWork(char iByte);
		/* -------------------------------- */
		void(*Event_Response)(
			ARC_Modbus::TCP_Master* context,
			int TaskPID,
			ARC_Modbus::PollingTaskRef TaskInfo,
			ARC_Modbus::Exception ex);
		/* -------------------------------- */
		void Polling(void); //主迴圈進入點
		/* -------------------------------- */
		bool GetCoil(int iIndex);
		short GetReg(int iIndex);
		/* -------------------------------- */
	private:
		Request_TCP request;
		Response_TCP response;
		// request(tx msg) max length
		// FC 01~06 : 12
		// FC 15 16 : 263 (!)

		// response(rx msg) max length
		// FC 01~04 : 259
		// FC 05 06 : 12
		// FC 15 16 : 12
		char byte_buffer[263];
		pkg packet;
	};
#endif // ARC_Modbus_Master_TCP_ENABLED

}

#endif // ARC_Modbus_h__
