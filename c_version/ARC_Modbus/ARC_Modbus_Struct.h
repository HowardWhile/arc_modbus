#ifndef ARC_Modbus_Struct_h__
#define ARC_Modbus_Struct_h__

#define ARC_Modbus_RTU_ENABLED                          ( 1 )
#define ARC_Modbus_ASCII_ENABLED                        ( 0 ) // 待測試
#define ARC_Modbus_TCP_ENABLED                          ( 0 ) // 待測試

#ifdef __cplusplus
extern "C" {
#endif
#if ARC_Modbus_RTU_ENABLED > 0
	typedef struct
	{


	}ARC_MODUBS_RTU_HandleTypeDef;
#endif
#if ARC_Modbus_ASCII_ENABLED > 0

#endif
#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif

#ifdef __cplusplus
}
#endif

#endif // ARC_Modbus_Struct_h__
