#ifndef ARC_Modbus_Response_h__
#define ARC_Modbus_Response_h__
#include "ARC_Modbus_Struct.h"
//中文字
namespace ARC_Modbus
{

#if ARC_Modbus_Master_RTU_ENABLED > 0
	class Response_RTU : public request_response_base
	{
	public:
		Response_RTU();
		~Response_RTU();

	private:

	};
#endif // ARC_Modbus_Master_RTU_ENABLED

#if ARC_Modbus_Master_ASCII_ENABLED > 0
	class Response_ASCII : public request_response_base
	{
	public:
		Response_ASCII();
		~Response_ASCII();


	private:

	};
#endif // ARC_Modbus_Master_ASCII_ENABLED

#if ARC_Modbus_Master_TCP_ENABLED > 0
	class Response_TCP : public request_response_base
	{
	public:
		Response_TCP();
		~Response_TCP();

		// 緩衝到 9 byte 就可以確認是否為例外回應
		// return:
		//  0不是Exception Responses
		// >0有錯誤 錯誤碼可以查找 exception code
		// http://www.simplymodbus.ca/exceptions.htm
		int CheckException(
			ARC_Modbus::pkg iPackeg,
			unsigned char iID);

		// return:
		// true 寫入成功的回應
		// false 無法辨識
		bool CheckWrite(
			ARC_Modbus::pkg iPackeg,
			unsigned char iID,
			unsigned short iAddress);
		
		// return:
		// true 讀取的回應
		// false 無法辨識
		// iNumber: 製作Request的Read命令時,讀取的數量
		bool CheckRead(
			ARC_Modbus::pkg iPackeg, 
			unsigned char iID,
			unsigned short iNumber);

		// 搭配CheckRead使用 當回傳ture時使用底下的函數來取得數值
		bool GetCoil(ARC_Modbus::pkg iPackeg, int iIndex);
		short GetReg(ARC_Modbus::pkg iPackeg, int iIndex);

		// 預期的回應長度
		int ReponseLength(int iFunctionCode, unsigned short iNumber);

	private:

	};
#endif // ARC_Modbus_Master_TCP_ENABLED
}


#endif // ARC_Modbus_Response_h__
