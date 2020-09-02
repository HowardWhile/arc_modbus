// c_version.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include "ARC_Modbus.h"

int main()
{
    ARC_MODBUS_RTU_HandleTypeDef modbus_slave;
    ARC_MODBUS_RTU_Initial(&modbus_slave, 1);

    std::cout << "Hello World!\n";
}