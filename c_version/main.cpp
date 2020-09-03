// c_version.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "ARC_Modbus.h"
#include "mbRegisterDef.h"

#include <time.h>
#include <iostream>

void TxWork(void* iContext, char* iBytes, int iLength);

ARC_MODBUS_Exception Coils_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber, // 線圈數量 (max:2000)
	bool IsWrite			// 告知讀取或者寫入暫存器
	);

/* Coils Status Function code 02,(1x) */
ARC_MODBUS_Exception InputCoils_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber	// 線圈數量 (max:2000)
	);

/* Coils Status Function code 03,(4x) */
ARC_MODBUS_Exception Register_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber,	// 數值數量 (max:125)
	bool IsWrite			// 告知讀取或者寫入暫存器
	);

/* Coils Status Function code 04,(3x) */
ARC_MODBUS_Exception InputRegs_CB(
	void* iContext,
	char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber	// 數值數量 (max:125)
	);

mbRegister_HandleTypeDef mb_reg;

int main()
{
	ARC_MODBUS_RTU_HandleTypeDef modbus_slave;
	ARC_MODBUS_RTU_Initial(&modbus_slave, 1);
	ARC_MODBUS_RTU_InitialInterface(&modbus_slave, TxWork, Coils_CB, InputCoils_CB, Register_CB, InputRegs_CB);
	mbRegister_Initial(&mb_reg);

	printf("modbus_slave(0x%p) (%d ms) [Initial] size: %d byte\r\n", &modbus_slave, clock(), sizeof(modbus_slave));

	char pkg_FC03_a[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xCD }; // FC03 address 0 size 10
	ARC_MODBUS_RTU_RxWork(&modbus_slave, pkg_FC03_a, sizeof(pkg_FC03_a));

	char pkg_FC03_b[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x64, 0x44, 0x21 }; // FC03 address 0 size 100
	ARC_MODBUS_RTU_RxWork(&modbus_slave, pkg_FC03_b, sizeof(pkg_FC03_b));

	std::cout << "Hello World!\n";
}

void TxWork(void* iContext, char* iBytes, int iLength)
{
	printf("modbus_slave(0x%p) (%d ms) [TxWork] ", iContext, clock());
	for (int idx = 0; idx < iLength; idx++)
		printf("%02X ", iBytes[idx] & 0xFF);
	printf("\r\n");
}

ARC_MODBUS_Exception Coils_CB(void* iContext, char* regBuffer, /* 傳遞的資料 8個bit拼成 1個byte */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber, /* 線圈數量 (max:2000) */ bool IsWrite /* 告知讀取或者寫入暫存器 */)
{
	/* ---------------------------------- */
	// Function code 01 (0x)
	printf("modbus_slave(0x%p) (%d ms) [RW Coils (01,0x)] addr[%d:%d](%d)\r\n", iContext, clock(), iAddress, iNumber, IsWrite);
	ARC_MODBUS_Exception err = ARC_MODBUS_Exception::Ex_none;
	/* ---------------------------------- */
	if (mbRegister_RW_Bits(&mb_reg, regBuffer, iAddress, iNumber, IsWrite))
	{
		err = ARC_MODBUS_Exception::Ex_IllegalDataAddress;
	}	
	return err;
}

ARC_MODBUS_Exception InputCoils_CB(void* iContext, char* regBuffer, /* 傳遞的資料 8個bit拼成 1個byte */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber /* 線圈數量 (max:2000) */)
{
	/* ---------------------------------- */
	// Function code 02 (1x)
	printf("modbus_slave(0x%p) (%d ms) [RO Coils(02,1x)] addr[%d:%d]\r\n", iContext, clock(), iAddress, iNumber);
	ARC_MODBUS_Exception err = ARC_MODBUS_Exception::Ex_none;
	/* ---------------------------------- */
	if (mbRegister_RO_Bits(&mb_reg, regBuffer, iAddress, iNumber))
	{
		err = ARC_MODBUS_Exception::Ex_IllegalDataAddress;
	}
	return err;
}

ARC_MODBUS_Exception Register_CB(void* iContext, char* regBuffer, /* 傳遞的資料 1個數值會由 2個byte拼成 */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber, /* 數值數量 (max:125) */ bool IsWrite /* 告知讀取或者寫入暫存器 */)
{
	/* ---------------------------------- */
	// Function code 03 (4x)
	printf("modbus_slave(0x%p) (%d ms) [RW Regs(03,4x)] addr[%d:%d](%d)\r\n", iContext, clock(), iAddress, iNumber, IsWrite);
	ARC_MODBUS_Exception err = ARC_MODBUS_Exception::Ex_none;
	/* ---------------------------------- */
	if (mbRegister_RW_Regs(&mb_reg, regBuffer, iAddress, iNumber, IsWrite))
	{
		err = ARC_MODBUS_Exception::Ex_IllegalDataAddress;
	}
	return err;
}

ARC_MODBUS_Exception InputRegs_CB(void* iContext, char* regBuffer, /* 傳遞的資料 1個數值會由 2個byte拼成 */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber /* 數值數量 (max:125) */)
{
	/* ---------------------------------- */
	// Function code 04 (3x)
	printf("modbus_slave(0x%p) (%d ms) [RO Regs(04,3x)] addr[%d:%d]\r\n", iContext, clock(), iAddress, iNumber);
	ARC_MODBUS_Exception err = ARC_MODBUS_Exception::Ex_none;
	/* ---------------------------------- */
	if (mbRegister_RO_Regs(&mb_reg, regBuffer, iAddress, iNumber))
	{
		err = ARC_MODBUS_Exception::Ex_IllegalDataAddress;
	}
	return err;


}