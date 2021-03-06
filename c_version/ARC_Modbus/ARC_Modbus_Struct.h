﻿#ifndef ARC_Modbus_Struct_h__
#define ARC_Modbus_Struct_h__

#ifdef __cplusplus
extern "C" {
#endif

#define ARC_Modbus_RTU_ENABLED                          ( 1 )
#define ARC_Modbus_ASCII_ENABLED                        ( 0 ) // ...施工中
#define ARC_Modbus_TCP_ENABLED                          ( 0 ) // ...施工中

#include <stdint.h>
#include <stdbool.h>

// NULL define
//#ifndef NULL
//#ifdef __cplusplus
//#define NULL 0
//#else
//#define NULL ((void *)0)
//#endif
//#endif

// Modbus Packet Define
#define INDEX_SLAVE_ID				0
#define INDEX_FUNCTION_CODE			1
#define INDEX_DATA_ADDRESS			2
#define INDEX_EXCEPTION_CODE		2
#define INDEX_PARAMETER				4

#define INDEX_NUMBER_BYTES_FOLLOW_r 2 // F:01 ~ 04 response
#define INDEX_DATA_START_r			3 // F:01 ~ 04 response

#define INDEX_NUMBER_TO_WRITE		4 // F:15,16
#define INDEX_NUMBER_BYTES_FOLLOW_w	6 // F:15,16
#define INDEX_DATA_START_w			7 // F:15,16

#define LENGTH_MBAP_HEADER			6 // TCP: MBAP Header
#define INDEX_TRANS_ID				0 // TCP: MBAP Header
#define INDEX_PROTOCOL_ID			2 // TCP: MBAP Header
#define INDEX_MESSAGE_LENGTH		4 // TCP: MBAP Header

	typedef enum
	{
		Ex_not_initial = -1,
		Ex_none = 0,
		Ex_IllegalFunction,
		Ex_IllegalDataAddress,
		Ex_IllegalDataValue,
		Ex_SlaveDeviceFailure,
		Ex_Acknowledge
	}ARC_MODBUS_Exception;

	typedef struct
	{
		/* -------------------------------------- */
		// Event Function
		/* -------------------------------------- */
		void(*Event_TxWork)(void* iContext, char* iBytes, int iLength); // 有資料要傳遞
		/* -------------------------------------- */
		// Callback Function
		/* -------------------------------------- */
		/* Coils Status Function code 01,(0x) */
		ARC_MODBUS_Exception(*CallBack_Coils)(
			void* iContext,
			char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber, // 線圈數量 (max:2000)
			bool IsWrite			// 告知讀取或者寫入暫存器
			);

		/* Coils Status Function code 02,(1x) */
		ARC_MODBUS_Exception(*CallBack_InputCoils)(
			void* iContext,
			char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber	// 線圈數量 (max:2000)
			);

		/* Coils Status Function code 03,(4x) */
		ARC_MODBUS_Exception(*CallBack_Register)(
			void* iContext,
			char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber,	// 數值數量 (max:125)
			bool IsWrite			// 告知讀取或者寫入暫存器
			);

		/* Coils Status Function code 04,(3x) */
		ARC_MODBUS_Exception(*CallBack_InputRegs)(
			void* iContext,
			char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber	// 數值數量 (max:125)
			);

	}ARC_MODBUS_FunctionPoint;

	typedef struct
	{
		uint8_t	SlaveAddress;	// 0 ~ 0xFF
		uint8_t	FunctionCode;	// 
		uint16_t DataAddres;	// 
		uint16_t Parameter;		// 
		uint16_t TransID;		// Transaction Identifier (Modbus TCP才有這個東西)
	}ARC_MODBUS_CommandParameter;

	short ARC_MODBUS_CRC16(const char* iBytes, unsigned short iLength);

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

		// decode result
		ARC_MODBUS_CommandParameter result; // 解碼成功後 解碼的結果放這邊

	}ARC_MODBUS_RTU_Rx_HandleTypeDef;

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
		uint16_t tx_msg_length;

	}ARC_MODBUS_RTU_Tx_HandleTypeDef; // 回應的封包結構

	typedef struct
	{
		//ARC_MODUBS_RTU_HandleTypeDef* context;
		uint8_t slave_id;
		char reg_buffer[250]; // 線圈最多2000(bit) 數值最多125(short) => size = 250(byte)
		ARC_MODBUS_RTU_Rx_HandleTypeDef rx_handler;
		ARC_MODBUS_RTU_Tx_HandleTypeDef tx_handler;
		ARC_MODBUS_FunctionPoint interface; // 事件與callback的接口
	}ARC_MODBUS_RTU_HandleTypeDef;

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
