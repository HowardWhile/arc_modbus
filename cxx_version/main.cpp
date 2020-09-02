// cxx_version.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//


#include "ARC_Modbus.h"

#include "mbRegisterMapping.h" // modbus slave 定義的記憶體

#include <iostream>
#include <time.h>


using namespace ARC_Modbus;

Exception Coils_CB(
	char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber, // 線圈數量 (max:2000)
	bool IsWrite			// 告知讀取或者寫入暫存器
	);

/* Coils Status Function code 02,(1x) */
Exception InputCoils_CB(
	char* regBuffer,		// 傳遞的資料 8個bit拼成 1個byte
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber	// 線圈數量 (max:2000)
	);

/* Coils Status Function code 03,(4x) */
Exception Register_CB(
	char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber,	// 數值數量 (max:125)
	bool IsWrite			// 告知讀取或者寫入暫存器
	);


/* Coils Status Function code 04,(3x) */
Exception InputRegs_CB(
	char* regBuffer,		// 傳遞的資料 1個數值會由 2個byte拼成
	unsigned short iAddress,// 0~FFFF
	unsigned short iNumber	// 數值數量 (max:125)
	);

void TxWork(char* iBytes, int iLength);

Register mb_reg;

int main()
{
    ARC_Modbus::RTU modbus_slave;
    modbus_slave.Initial(1); //modbus slave id = 1
	modbus_slave.Event_TxWork = TxWork;
	modbus_slave.CallBack_Coils = Coils_CB;
	modbus_slave.CallBack_InputCoils = InputCoils_CB;
	modbus_slave.CallBack_Register = Register_CB;
	modbus_slave.CallBack_InputRegs = InputRegs_CB;

	
	char pkg_FC03_a[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xCD }; // FC03 address 0 size 10
	modbus_slave.RxWork(pkg_FC03_a, sizeof(pkg_FC03_a));

	char pkg_FC03_b[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x64, 0x44, 0x21 }; // FC03 address 0 size 100
	modbus_slave.RxWork(pkg_FC03_b, sizeof(pkg_FC03_b));

    std::cout << "Hello World!\n";
}

ARC_Modbus::Exception Coils_CB(char* regBuffer, /* 傳遞的資料 8個bit拼成 1個byte */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber, /* 線圈數量 (max:2000) */ bool IsWrite /* 告知讀取或者寫入暫存器 */)
{
	/* ---------------------------------- */
	// Function code 01 (0x)
	ARC_Modbus::Exception err = ARC_Modbus::Ex_none;
	printf("[RW Coils (01,0x)] %d ms addr[%d:%d](%d)\r\n", clock(), iAddress, iNumber, IsWrite);
	/* ---------------------------------- */
	if (IsWrite)
	{
		if (mb_reg.SetRegsBuffer(mb_reg.RegType_Coile, iAddress, iNumber, regBuffer) == false)
		{
			err = ARC_Modbus::Ex_IllegalDataAddress; // 無法寫入的位置
		}
	}
	else
	{
		if (mb_reg.GetRegsBuffer(mb_reg.RegType_Coile, iAddress, iNumber, regBuffer) == false)
		{
			err = ARC_Modbus::Ex_IllegalDataAddress; // 無法寫入的位置
		}
	}
	return err;
}

ARC_Modbus::Exception InputCoils_CB(char* regBuffer, /* 傳遞的資料 8個bit拼成 1個byte */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber /* 線圈數量 (max:2000) */)
{
	/* ---------------------------------- */
	// Function code 02 (1x)
	ARC_Modbus::Exception err = ARC_Modbus::Ex_none;
	printf("[RO Coils(02,1x)] %d ms addr[%d:%d]\r\n", clock(), iAddress, iNumber);
	/* ---------------------------------- */
	if (mb_reg.GetRegsBuffer(mb_reg.RegType_InputCoile, iAddress, iNumber, regBuffer) == false)
	{
		err = ARC_Modbus::Ex_IllegalDataAddress; // 無法寫入的位置
	}
	return err;
}

ARC_Modbus::Exception Register_CB(char* regBuffer, /* 傳遞的資料 1個數值會由 2個byte拼成 */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber, /* 數值數量 (max:125) */ bool IsWrite /* 告知讀取或者寫入暫存器 */)
{
	/* ---------------------------------- */
	// Function code 03 (4x)
	ARC_Modbus::Exception err = ARC_Modbus::Ex_none;
	printf("[RW Regs(03,4x)] %d ms addr[%d:%d](%d)\r\n", clock(), iAddress, iNumber, IsWrite);
	/* ---------------------------------- */
	if (IsWrite)
	{
		if (mb_reg.SetRegsBuffer(mb_reg.RegType_Register, iAddress, iNumber, regBuffer) == false)
		{
			err = ARC_Modbus::Ex_IllegalDataAddress; // 無法寫入的位置
		}
	}
	else
	{
		if (mb_reg.GetRegsBuffer(mb_reg.RegType_Register, iAddress, iNumber, regBuffer) == false)
		{
			err = ARC_Modbus::Ex_IllegalDataAddress; // 無法寫入的位置
		}
	}
	return err;
}

ARC_Modbus::Exception InputRegs_CB(char* regBuffer, /* 傳遞的資料 1個數值會由 2個byte拼成 */ unsigned short iAddress,/* 0~FFFF */ unsigned short iNumber /* 數值數量 (max:125) */)
{
	/* ---------------------------------- */
	// Function code 04 (3x)
	ARC_Modbus::Exception err = ARC_Modbus::Ex_none;
	printf("[RO Regs(04,3x)] %d ms addr[%d:%d]\r\n", clock(), iAddress, iNumber);
	/* ---------------------------------- */
	if (mb_reg.GetRegsBuffer(mb_reg.RegType_InputRegs, iAddress, iNumber, regBuffer) == false)
	{
		err = ARC_Modbus::Ex_IllegalDataAddress; // 無法寫入的位置
	}
	return err;
	//return ARC_Modbus::Ex_IllegalFunction; //禁止使用 Function Code 04
}

void TxWork(char* iBytes, int iLength)
{
	printf("[TxWork] %d ms ", clock());
	for (int idx = 0; idx < iLength; idx++)
	{
		printf("%02X ", iBytes[idx]&0xFF);
	}

	printf("\r\n");
}