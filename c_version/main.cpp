// c_version.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "ARC_Modbus.h"

#include <time.h>
#include <iostream>

void TxWork(void* iContext, char* iBytes, int iLength);

ARCMODBUS_Exception Coils_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber, // 線圈數量 (max:2000)
	bool IsWrite			// 告知讀取或者寫入暫存器
	);

/* Coils Status Function code 02,(1x) */
ARCMODBUS_Exception InputCoils_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber	// 線圈數量 (max:2000)
	);



/* Coils Status Function code 03,(4x) */
ARCMODBUS_Exception Register_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber,	// 數值數量 (max:125)
	bool IsWrite			// 告知讀取或者寫入暫存器
	);



/* Coils Status Function code 04,(3x) */
ARCMODBUS_Exception InputRegs_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber	// 數值數量 (max:125)
	);

int main()
{
	ARC_MODBUS_RTU_HandleTypeDef modbus_slave;
	ARC_MODBUS_RTU_Initial(&modbus_slave, 1);
	ARC_MODBUS_RTU_InitialInterface(&modbus_slave, TxWork, Coils_CB, InputCoils_CB, Register_CB, InputRegs_CB);
	printf("modbus_slave(0x%p) (%d ms) [Initial] size: %d byte\r\n", &modbus_slave, clock(), sizeof(modbus_slave));

	char pkg_FC03_a[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xCD }; // FC03 address 0 size 10
	ARC_MODBUS_RTU_RxWorkEx(&modbus_slave, pkg_FC03_a, sizeof(pkg_FC03_a));

	char pkg_FC03_b[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x64, 0x44, 0x21 }; // FC03 address 0 size 100
	ARC_MODBUS_RTU_RxWorkEx(&modbus_slave, pkg_FC03_b, sizeof(pkg_FC03_b));

	std::cout << "Hello World!\n";
}

void TxWork(void* iContext, char* iBytes, int iLength)
{
	printf("modbus_slave(0x%p) (%d ms) [TxWork] ", iContext, clock());
	for (int idx = 0; idx < iLength; idx++)
		printf("%02X ", iBytes[idx] & 0xFF);
	printf("\r\n");
}

ARCMODBUS_Exception Coils_CB(void* iContext, char* regBuffer, /* 傳遞的資料 8個bit拼成 1個byte */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber, /* 線圈數量 (max:2000) */ bool IsWrite /* 告知讀取或者寫入暫存器 */)
{
	return ARCMODBUS_Exception::Ex_IllegalFunction;

}

ARCMODBUS_Exception InputCoils_CB(void* iContext, char* regBuffer, /* 傳遞的資料 8個bit拼成 1個byte */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber /* 線圈數量 (max:2000) */)
{
	return ARCMODBUS_Exception::Ex_IllegalFunction;

}

ARCMODBUS_Exception Register_CB(void* iContext, char* regBuffer, /* 傳遞的資料 1個數值會由 2個byte拼成 */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber, /* 數值數量 (max:125) */ bool IsWrite /* 告知讀取或者寫入暫存器 */)
{
	return ARCMODBUS_Exception::Ex_IllegalFunction;
}

ARCMODBUS_Exception InputRegs_CB(void* iContext, char* regBuffer, /* 傳遞的資料 1個數值會由 2個byte拼成 */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber /* 數值數量 (max:125) */)
{
	return ARCMODBUS_Exception::Ex_IllegalFunction;

}