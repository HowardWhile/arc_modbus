#ifndef ARC_Modbus_Request_h__
#define ARC_Modbus_Request_h__
#include "ARC_Modbus_Struct.h"
//中文字
namespace ARC_Modbus
{

#if ARC_Modbus_Master_RTU_ENABLED > 0
	class Request_RTU : public request_response_base
	{
	public:
		Request_RTU();
		~Request_RTU();


	private:

	};
#endif

#if ARC_Modbus_Master_ASCII_ENABLED > 0
	class Request_ASCII : public request_response_base
	{
	public:
		Request_ASCII();
		~Request_ASCII();


	private:

	};
#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0
	class Request_TCP : public request_response_base
	{
	public:
		Request_TCP();
		~Request_TCP();

		// 做成 function code 01～04 的讀取命令
		ARC_Modbus::pkg Read(
			unsigned char iID,
			unsigned char iFunctionCode,
			unsigned short iAddress,
			unsigned short iNumber);

		// 做成 function code 05 的寫入命令 bit單一寫入
		ARC_Modbus::pkg Write(
			unsigned char iID,
			unsigned short iAddress,
			bool iValue);

		// 做成 function code 06 的寫入命令 數值單一寫入
		ARC_Modbus::pkg Write(
			unsigned char iID,
			unsigned short iAddress,
			short iValue);

		// 做成 function code 15 的寫入命令 多個bit寫入
		ARC_Modbus::pkg Write(
			unsigned char iID,
			unsigned short iAddress,
			bool* iValues,
			unsigned short iNumber);
		ARC_Modbus::pkg Write(
			unsigned char iID,
			unsigned short iAddress,
			char* iValues, // 8bits = 1byte
			unsigned short iNumber);

		// 做成 function code 16 的寫入命令 多個bit寫入
		ARC_Modbus::pkg Write(
			unsigned char iID,
			unsigned short iAddress,
			short* iValues,
			unsigned short iNumber);

	private:
		int trans_id;
	};
#endif
}

#endif // ARC_Modbus_Request_h__
