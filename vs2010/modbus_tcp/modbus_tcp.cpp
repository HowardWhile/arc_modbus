// modbus_tcp.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"

#include "ARC_Modbus.h"

#include <time.h>
#include <stdio.h>

#include <iostream>
#include <vector>

using namespace ARC_Modbus;

class my_example
{
public:
	ARC_Modbus::TCP modbus_slave;

public:
	// ----------------------------------
	// Initial
	// ----------------------------------
	my_example() : modbus_slave(this) // 可以傳入class的指標 這樣事件函數的context就會等同this
	{
		// 初始化 modbus slave id
		this->modbus_slave.Initial(1);

		//
		this->modbus_slave.CallBack_Coils = my_example::Coils_CB;
		this->modbus_slave.CallBack_InputCoils = my_example::InputCoils_CB;
		this->modbus_slave.CallBack_Register = my_example::Register_CB;
		this->modbus_slave.CallBack_InputRegs = my_example::InputRegs_CB;

		this->RW_Coils.resize(1000);
		this->RW_Regs.resize(100);
	}
	// ----------------------------------
	// Event
	// ----------------------------------
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
		ARC_Modbus::Exception err = ARC_Modbus::Ex_none;

		my_example *pThis = (my_example *)iContext;

		if (iAddress + iNumber > (int)pThis->RW_Regs.size()) // 存取的範圍超過設定的記憶體大小
			return ARC_Modbus::Ex_IllegalDataAddress;

		if (IsWrite)
		{
			printf("[Register_CB][FC 06 or 16] %d ms addr[%d:%d](%d)\r\n", clock(), iAddress, iNumber);

			ARC_Modbus::request_response_base mem_tool;
			for (int idx = 0; idx < iNumber; idx++)
			{
				short tmp = mem_tool.get_int16(regBuffer, idx);
				pThis->RW_Regs[idx] = tmp;
			}
		}
		else
		{
			printf("[Register_CB][FC 03] %d ms addr[%d:%d](%d)\r\n", clock(), iAddress, iNumber);
			ARC_Modbus::request_response_base mem_tool;
			for (int idx = 0; idx < iNumber; idx++)
			{
				short tmp = pThis->RW_Regs[idx];
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

	// ----------------------------------
	// Do Work
	// ----------------------------------
	void main()
	{
		printf("Hello world\r\n");
	}

public:
	// ----------------------------------
	// 用於mapping的記憶體位置
	// ----------------------------------
	std::vector<bool> RW_Coils;
	// std::vector<bool> RO_Coils;
	std::vector<short> RW_Regs;
	// std::vector<short> RO_Regs;
};

int main(int argc, _TCHAR *argv[])
{
	my_example ex;
	ex.main();

	system("PAUSE");
	return 0;
}
