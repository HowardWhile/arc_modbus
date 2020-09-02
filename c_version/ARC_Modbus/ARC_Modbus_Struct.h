#ifndef ARC_Modbus_Struct_h__
#define ARC_Modbus_Struct_h__

#ifdef __cplusplus
extern "C" {
#endif

#define ARC_Modbus_RTU_ENABLED                          ( 1 )
#define ARC_Modbus_ASCII_ENABLED                        ( 0 ) // 待測試
#define ARC_Modbus_TCP_ENABLED                          ( 0 ) // 待測試


#include <stdint.h>
#include <stdbool.h>

	typedef enum
	{
		Ex_not_initial = -1,
		Ex_none = 0,
		Ex_IllegalFunction,
		Ex_IllegalDataAddress,
		Ex_IllegalDataValue,
		Ex_SlaveDeviceFailure,
		Ex_Acknowledge
	}Exception;

	typedef struct
	{
		/* -------------------------------------- */
		// Event Function
		/* -------------------------------------- */
		void(*Event_TxWork)(char* iBytes, int iLength); // 有資料要傳遞
		/* -------------------------------------- */
		// Callback Function
		/* -------------------------------------- */
	}ARC_MODUBS_FUNC_POINT;

#if ARC_Modbus_RTU_ENABLED > 0
	typedef struct
	{
		// RTU Frame Format:		
		// 0:	The Slave Address
		// 1:	The Function Code
		// 2~3:	The Data Address 
		// (F:01~06)
		// 4~5: Parameter 
		// 6~7: CRC 
		// total size: 8 Bytes
		char rx_buffer_0106[8];

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

	}ARC_MODUBS_RTU_Rx_HandleTypeDef;

	typedef struct
	{
		// http://www.simplymodbus.ca/FC05.htm
		// 1 byte, The Slave Address
		// 1 byte, The Function Code
		// 1 byte, The number of data bytes
		// 1~250 byte, data
		// 2 byte, crc16
		// sum: 6~255 byte
		char tx_msg_buffer[255];
	}ARC_MODUBS_RTU_Tx_HandleTypeDef;

	typedef struct
	{
		//ARC_MODUBS_RTU_HandleTypeDef* context;
		uint8_t slave_id;
		ARC_MODUBS_RTU_Rx_HandleTypeDef rx_handler;
		ARC_MODUBS_RTU_Tx_HandleTypeDef tx_handler;
		ARC_MODUBS_FUNC_POINT interface; // 事件與callback的接口
	}ARC_MODUBS_RTU_HandleTypeDef;

#endif
#if ARC_Modbus_ASCII_ENABLED > 0
	// ...施工中
#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0
	// ...施工中
#endif

#ifdef __cplusplus
}
#endif

#endif // ARC_Modbus_Struct_h__
