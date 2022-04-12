#ifndef ARC_Modbus_Encoder_h__
#define ARC_Modbus_Encoder_h__

#include "ARC_Modbus_Struct.h"

namespace ARC_Modbus
{
#if ARC_Modbus_RTU_ENABLED > 0
	class Encoder_RTU : public coder_base
	{
	public:
		Encoder_RTU();
		~Encoder_RTU();

		void GetResponse(
			char* oBuffer, int &oLength,	/* output */
			char* iRegBuffer,				
			const Command_Parameter &iPara,
			Exception iError				/* exceptions 錯誤訊息 */
			);		

	private:
		void packet_make_exception(int iFunctionCode, Exception iError, char* oBuffer, int &oLength);
		void packet_make_function_read(int iFunctionCode, int iRegsNumber, char* iRegBuffer, char* oBuffer, int &oLength);
		void packet_make_function_write(int iFunctionCode, int iAddress, int iParameter, char* oBuffer, int &oLength);
	};
#endif

#if ARC_Modbus_ASCII_ENABLED > 0
	class Encoder_ASCII : public coder_base
	{
	public:
		Encoder_ASCII();
		~Encoder_ASCII();

		void GetResponse(
			char* oBuffer, int &oLength,	/* output */
			char* iRegBuffer,
			const Command_Parameter &iPara,
			Exception iError				/* exceptions 錯誤訊息 */
			);
		
	private:
		void packet_make_exception(int iFunctionCode, Exception iError, char* oBuffer, int &oLength);
		void packet_make_function_read(int iFunctionCode, int iRegsNumber, char* iRegBuffer, char* oBuffer, int &oLength);
		void packet_make_function_write(int iFunctionCode, int iAddress, int iParameter, char* oBuffer, int &oLength);
		void msg_make(char* byte_buffer, int byte_number, char* oBuffer, int & oLength);

		char byte_buffer[254]; //先產生byte型態的資料 再轉換成字串(tx_msg)
	};
#endif

#if ARC_Modbus_TCP_ENABLED > 0
	class Encoder_TCP : public coder_base
	{
	public:
		Encoder_TCP();
		~Encoder_TCP();

		void GetResponse(
			char* oBuffer, int &oLength,	/* output */
			char* iRegBuffer,
			const Command_Parameter &iPara,
			Exception iError				/* exceptions 錯誤訊息 */
			);

	private:
		void packet_make_exception(int TransID, int iFunctionCode, Exception iError, char* oBuffer, int &oLength);
		void packet_make_function_read(int TransID, int iFunctionCode, int iRegsNumber, char* iRegBuffer, char* oBuffer, int &oLength);
		void packet_make_function_write(int TransID, int iFunctionCode, int iAddress, int iParameter, char* oBuffer, int &oLength);
	
		void header_make(int TransID, int iMsgLength, char* oBuffer);
	};
#endif
}
#endif // ARC_Modbus_Encoder_h__
