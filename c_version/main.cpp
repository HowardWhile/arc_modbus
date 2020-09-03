// c_version.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "ARC_Modbus.h"

#include <time.h>
#include <iostream>

void TxWork(void* iContext, char* iBytes, int iLength);

int main()
{
	ARC_MODBUS_RTU_HandleTypeDef modbus_slave;
	ARC_MODBUS_RTU_Initial(&modbus_slave, 1);
	ARC_MODBUS_RTU_InitialInterface(&modbus_slave, TxWork);
	printf("modbus_slave(0x%p) (%d ms) [Initial]\r\n", &modbus_slave, clock());


	char pkg_FC03_a[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x0A, 0xC5, 0xCD }; // FC03 address 0 size 10
	ARC_MODBUS_RTU_RxWorkEx(&modbus_slave, pkg_FC03_a, sizeof(pkg_FC03_a));

	std::cout << "Hello World!\n";
}

void TxWork(void* iContext, char* iBytes, int iLength)
{
	printf("modbus_slave(0x%p) (%d ms) [TxWork] ", iContext, clock());
	for (int idx = 0; idx < iLength; idx++)
		printf("%02X ", iBytes[idx] & 0xFF);
	printf("\r\n");
}