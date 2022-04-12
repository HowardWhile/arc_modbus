#ifndef ARC_Modbus_Struct_h__
#define ARC_Modbus_Struct_h__

// 沒有用到可以 Disable 省點空間
#define ARC_Modbus_RTU_ENABLED                          ( 0 )
#define ARC_Modbus_ASCII_ENABLED                        ( 0 )
#define ARC_Modbus_TCP_ENABLED                          ( 1 ) 

#define ARC_Modbus_Master_RTU_ENABLED                   ( 0 ) // 還沒寫
#define ARC_Modbus_Master_ASCII_ENABLED                 ( 0 ) // 還沒寫
#define ARC_Modbus_Master_TCP_ENABLED                   ( 0 )

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

//#define DEBUG_CONTROL

namespace ARC_Modbus
{
	short CRC16(const char *iBytes, unsigned short iLength);
	char LRC(const char *iBytes, unsigned short iLength);
	char LRC_String(const char* iString, unsigned int iLength);
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

	typedef enum
	{
		PollingTaskType_None = 0,
		PollingTaskType_Read_RW_Coils = 1,
		PollingTaskType_Read_RO_Coils,
		PollingTaskType_Read_RW_Regs,
		PollingTaskType_Read_RO_Regs,
		PollingTaskType_Write_Coils = 15,
		PollingTaskType_Write_Regs
	}PollingTaskType;

	class modbus_slave_base
	{
	public:
		modbus_slave_base();
		~modbus_slave_base();

		/* -------------------------------------- */
		// Event Function
		void(*Event_TxWork)(char* iBytes, int iLength); // 有資料要傳遞
		/* -------------------------------------- */
		/* Callback Function */
		/* Coils Status Function code 01,(0x) */
		Exception(*CallBack_Coils)(
			void* iContext,			// parent
			char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber, // 線圈數量 (max:2000)
			bool IsWrite			// 告知讀取或者寫入暫存器
			);

		/* Coils Status Function code 02,(1x) */
		Exception(*CallBack_InputCoils)(
			void* iContext,			// parent
			char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber	// 線圈數量 (max:2000)
			);

		/* Coils Status Function code 03,(4x) */
		Exception(*CallBack_Register)(
			void* iContext,			// parent
			char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber,	// 數值數量 (max:125)
			bool IsWrite			// 告知讀取或者寫入暫存器
			);

		/* Coils Status Function code 04,(3x) */
		Exception(*CallBack_InputRegs)(
			void* iContext,			// parent
			char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
			unsigned short iAddress,// 0~FFFF
			unsigned short iNumber	// 數值數量 (max:125)
			);

	protected:
		char reg_buffer[250]; // 線圈最多2000(bit) 數值最多125(short) => size = 250(byte)

		typedef union
		{
			short wValue;
			char cValue[2];
		}tu_convert16;

		void* parent;
	};

	class PollingTaskRef
	{
	public:

		PollingTaskRef();
		~PollingTaskRef();

		int SlaveID;
		PollingTaskType Type;
		unsigned short Address;
		unsigned short Number;
	};

	class pkg
	{
	public:
		pkg();
		~pkg();

		char* datas;
		int size;
	};

	class modbus_master_base
	{
	public:
		modbus_master_base();
		~modbus_master_base();
		/* -------------------------------------- */
		// Event Function
		void(*Event_TxWork)(char* iBytes, int iLength); // 有資料要傳遞
		void(*CallBack_WriteCoilsRequest)(
			char** oRegBuffer,
			int TaskPID,
			ARC_Modbus::PollingTaskRef TaskInfo);
		void(*CallBack_WriteRegsRequest)(
			short** oRegsPtr,
			int TaskPID,
			ARC_Modbus::PollingTaskRef TaskInfo);
		/* -------------------------------------- */
		bool Add_PollingTask(ARC_Modbus::PollingTaskRef iTask);
		void Clear_PollingTask();

		static const int MAX_TASK_SIZE = 10;
	protected:
		typedef enum 
		{
			polling_step_send_request = 0,
			polling_step_wait_response,
			polling_step_task_delay,
		}polling_step;
		polling_step step;

		int task_size;
		int task_count;
		PollingTaskRef task_lists[10];

		int check_time;
		static const int timeout = 500; //ms
		static const int delay = 0; //ms, delay between polls

		int respones_len;
	};
	// decoder 架構
	class coder_base
	{
	public:
		coder_base();
		~coder_base();

		void Initial(int iSlaveID);

	protected:
		unsigned char	slave_id;

		typedef union
		{
			short wValue;
			char cValue[2];
		}tu_convert16; //轉換		
	};

	class request_response_base
	{
	public:
		char* ByteBuffer_ptr;

		void set_bit(char* iPtr, unsigned short iIndex, bool iEnable);
		bool get_bit(char* iPtr, unsigned short iIndex);

		void set_int16(char* iPtr, unsigned short iIndex, short iValue);
		short get_int16(char* iPtr, unsigned short iIndex);

		typedef union
		{
			short wValue;
			char cValue[2];
		}tu_convert16; //轉換	
	};

	class Command_Parameter
	{
	public:
		Command_Parameter();
		~Command_Parameter();

		unsigned char	SlaveAddress;	// 0 ~ 0xFF
		unsigned char	FunctionCode;	// 
		unsigned short	DataAddres;		// 
		unsigned short	Parameter;		// 
		unsigned short	TransID;		// Transaction Identifier (Modbus TCP才有這個東西)
	};


}

#endif // ARC_Modbus_Struct_h__
