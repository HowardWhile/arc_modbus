﻿// modbus_tcp.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"

#include "ARC_Modbus.h"

#include <time.h>
#include <stdio.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace ARC_Modbus;

class my_example
{
public:
	ARC_Modbus::TCP modbus_slave;

public:
	// ----------------------------------
	// Initial
	// ----------------------------------
	my_example() : modbus_slave((void *)this) // 可以傳入class的指標 這樣所有事件函數的context就會等同於該this
	{
		// 初始化 modbus slave id
		this->modbus_slave.Initial(1);

		// event 不需回傳的事件
		this->modbus_slave.Event_TxWork = my_example::Tx;

		// callback 需回傳的事件
		this->modbus_slave.CallBack_Coils = my_example::Coils_CB;
		this->modbus_slave.CallBack_InputCoils = my_example::InputCoils_CB;
		this->modbus_slave.CallBack_Register = my_example::Register_CB;
		this->modbus_slave.CallBack_InputRegs = my_example::InputRegs_CB;

		// this->RW_Coils.resize(1000);
		this->RW_Regs.resize(100);
	}
	// ----------------------------------
	// Event
	// ----------------------------------
	static void Tx(void *iContext, char *iBytes, int iLength) // 有資料要傳遞
	{

		printf("[Tx %d ms] ", clock());

		my_example *pThis = (my_example *)iContext;
		pThis->printf_bytes(iBytes, iLength);

		printf("\r\n");
	}

	static Exception Coils_CB(
		void *iContext,
		char *regBuffer,		 // 傳遞的資料 8個bit拼成 1個byte
		unsigned short iAddress, // 0~FFFF
		unsigned short iNumber,	 // 線圈數量 (max:2000)
		bool IsWrite			 // 告知讀取或者寫入暫存器
	)
	{
		return ARC_Modbus::Ex_none;
	}
	static Exception InputCoils_CB(
		void *iContext,			 // parent
		char *regBuffer,		 // 傳遞的資料 8個bit拼成 1個byte
		unsigned short iAddress, // 0~FFFF
		unsigned short iNumber	 // 線圈數量 (max:2000)
	)
	{
		// 禁止存取可以回傳 Ex_IllegalFunction
		return ARC_Modbus::Ex_IllegalFunction;
		// 不正確的位置可以回傳 Ex_IllegalDataAddress
		// return ARC_Modbus::Ex_IllegalDataAddress;
	}

	static Exception Register_CB(
		void *iContext,			 // parent
		char *regBuffer,		 // 傳遞的資料 1個數值會由 2個byte拼成
		unsigned short iAddress, // 0~FFFF
		unsigned short iNumber,	 // 數值數量 (max:125)
		bool IsWrite			 // 告知讀取或者寫入暫存器
	)
	{
		// ----------------------------------
		// 事件的範例可以這樣寫
		// ----------------------------------
		ARC_Modbus::Exception err = ARC_Modbus::Ex_none;

		// 檢查存取的範圍
		my_example *pThis = (my_example *)iContext;
		if (iAddress + iNumber > (int)pThis->RW_Regs.size()) // 存取的範圍超過設定的記憶體大小
			return ARC_Modbus::Ex_IllegalDataAddress;

		if (IsWrite)
		{
			printf("[Register_CB (%d ms)][FC 06 16] addr[%d:%d]\r\n", clock(), iAddress, iNumber);

			ARC_Modbus::request_response_base mem_tool;
			for (int idx = 0; idx < iNumber; idx++)
			{
				// 從緩衝讀出
				short tmp = mem_tool.get_int16(regBuffer, idx);

				// 寫入到準備好的記憶體位置
				pThis->RW_Regs[iAddress + idx] = tmp;
			}
		}
		else
		{
			printf("[Register_CB (%d ms)][FC 03] addr[%d:%d]\r\n", clock(), iAddress, iNumber);
			ARC_Modbus::request_response_base mem_tool;
			for (int idx = 0; idx < iNumber; idx++)
			{
				// 由準備好的記憶體位置讀出
				short tmp = pThis->RW_Regs[iAddress + idx];

				mem_tool.set_int16(regBuffer, idx, tmp);
			}
		}

		return err;
	}

	static Exception InputRegs_CB(
		void *iContext,			 // parent
		char *regBuffer,		 // 傳遞的資料 1個數值會由 2個byte拼成
		unsigned short iAddress, // 0~FFFF
		unsigned short iNumber	 // 數值數量 (max:125)
	)
	{
		// 禁止存取可以回傳 Ex_IllegalFunction
		// return ARC_Modbus::Ex_IllegalFunction;
		// 不正確的位置可以回傳 Ex_IllegalDataAddress
		return ARC_Modbus::Ex_IllegalDataAddress;
	}

public:
	// ----------------------------------
	// 用於mapping的記憶體位置
	// ----------------------------------
	// std::vector<bool> RW_Coils;
	// std::vector<bool> RO_Coils;
	std::vector<short> RW_Regs;
	// std::vector<short> RO_Regs;

	// ----------------------------------
	// Do Work
	// ----------------------------------
	void main()
	{
		this->RW_Regs[5] = 55;
		this->RW_Regs[6] = 66;
		while (true)
		{
			std::vector<char> bytes;

			// 09 F4 00 00 00 06 01 03 00 00 00 0A, 封包 FC03 從記憶體 0 讀取10個 數值 // 其中 [5] = 55 [6] = 66
			bytes = this->toByte("00 0A 00 00 00 06 01 03 00 00 00 0A");
			printf("---------------\r\n");
			printf("[Rx %d ms] ", clock());
			printf_bytes(bytes);
			printf("\r\n");
			this->modbus_slave.RxWork(&bytes[0], bytes.size());

			// 0A A7 00 00 00 06 01 03 00 05 00 02 封包 FC03 從記憶體 5 讀取2個 數值 // [5] = 55 [6] = 66
			bytes = this->toByte("0A A7 00 00 00 06 01 03 00 05 00 02");
			printf("---------------\r\n");
			printf("[Rx %d ms] ", clock());
			printf_bytes(bytes);
			printf("\r\n");
			this->modbus_slave.RxWork(&bytes[0], bytes.size());
			system("PAUSE");
		}
	}

private:
	// ----------------------------------
	// 已下都是炫炮用功能不太重要
	// ----------------------------------
	std::vector<char> toByte(std::string iHex)
	{
		// https://stackoverflow.com/questions/3221170/how-to-turn-a-hex-string-into-an-unsigned-char-array
		std::istringstream hex_chars_stream(iHex);
		std::vector<char> bytes;
		int c;
		while (hex_chars_stream >> std::hex >> c)
		{
			bytes.push_back(c);
		}
		return bytes;
	}

	void printf_bytes(std::vector<char> iBytes)
	{
		this->printf_bytes(&iBytes[0], iBytes.size());
	}

	void printf_bytes(char *iBytes, int iLength)
	{
		for (int idx = 0; idx < iLength; idx++)
			printf("%02X ", iBytes[idx] & 0xFF);
	}
};

int main(int argc, _TCHAR *argv[])
{
	my_example ex;
	ex.main();

	system("PAUSE");
	return 0;
}
