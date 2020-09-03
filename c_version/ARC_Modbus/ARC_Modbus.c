#include "ARC_Modbus.h"
#include "ARC_Modbus_Decoder.h"
#include "ARC_Modbus_Encoder.h"

#include <string.h>

typedef union
{
	short wValue;
	char cValue[2];
}tu_convert16;

#if ARC_Modbus_RTU_ENABLED > 0
void ARC_MODBUS_RTU_RxByte(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte);

void ARC_MODBUS_RTU_Initial(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, uint8_t iSlaveID)
{
	iModbusHandel->slave_id = iSlaveID;
	iModbusHandel->interface.Event_TxWork = NULL;
	iModbusHandel->rx_handler.buffer_enable = false;
}


void ARC_MODBUS_RTU_InitialInterface(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, void* iEvent_TxWork, void* iCallBack_Coils, void* iCallBack_InputCoils, void* iCallBack_Register, void* iCallBack_InputRegs)
{
	iModbusHandel->interface.Event_TxWork = iEvent_TxWork;
	iModbusHandel->interface.CallBack_Coils = iCallBack_Coils;
	iModbusHandel->interface.CallBack_InputCoils = iCallBack_InputCoils;
	iModbusHandel->interface.CallBack_InputRegs = iCallBack_InputRegs;
	iModbusHandel->interface.CallBack_Register = iCallBack_Register;
}

void ARC_MODBUS_RTU_RxWork(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char* iBytes, int iLength)
{
	for (int idx = 0; idx < iLength; idx++)
	{
		ARC_MODBUS_RTU_RxByte(iModbusHandel, iBytes[idx]);
	}
}

void ARC_MODBUS_RTU_RxByte(ARC_MODBUS_RTU_HandleTypeDef* iModbusHandel, char iByte)
{
	tu_convert16 cTool;
	ARC_MODBUS_FunctionPoint* func_ptr = &iModbusHandel->interface;
	ARC_MODBUS_RTU_Rx_HandleTypeDef* h_rx = &iModbusHandel->rx_handler;
	ARC_MODBUS_RTU_Tx_HandleTypeDef* h_tx = &iModbusHandel->tx_handler;

	//if (this->decoder.RxByte(iByte) == true)
	if (ARC_MODBUS_RTU_Decoder_RxByte(iModbusHandel, iByte) == true)
	{
		ARC_MODBUS_Exception err = Ex_not_initial;

		switch (h_rx->result.FunctionCode)
		{
		case 1: /* Function Code: 01 */
			if (func_ptr->CallBack_Coils)
			{
				if (h_rx->result.Parameter <= 2000)// 01 modbus協定表示 最大的線圈讀取數是2000
				{
					err = func_ptr->CallBack_Coils(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						h_rx->result.Parameter,
						false);
				}
				else
				{
					err = Ex_IllegalDataValue; // 
				}
			}
			break;

		case 2: /* Function Code: 02 */
			if (func_ptr->CallBack_InputCoils)
			{
				if (h_rx->result.Parameter <= 2000)// 02 modbus協定表示 最大的線圈讀取數是2000
				{
					err = func_ptr->CallBack_InputCoils(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						h_rx->result.Parameter);
				}
				else
				{
					err = Ex_IllegalDataValue; // 
				}
			}
			break;

		case 3: /* Function Code: 03 */
			if (func_ptr->CallBack_Register)
			{
				if (h_rx->result.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
				{
					err = func_ptr->CallBack_Register(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						h_rx->result.Parameter,
						false);
				}
				else
				{
					err = Ex_IllegalDataValue; // 
				}
			}
			break;

		case 4: /* Function Code: 04 */
			if (func_ptr->CallBack_InputRegs)
			{
				if (h_rx->result.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
				{
					err = func_ptr->CallBack_InputRegs(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						h_rx->result.Parameter);
				}
				else
				{
					err = Ex_IllegalDataValue; // 
				}
			}
			break;

		case 5: /* Function Code: 05 */
			if (func_ptr->CallBack_Coils)
			{
				if (h_rx->result.Parameter == 0xFF00)
				{
					iModbusHandel->reg_buffer[0] = 0x1; // set bit
					err = func_ptr->CallBack_Coils(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						1, // one bit
						true);
				}
				else if (h_rx->result.Parameter == 0x0000)
				{
					iModbusHandel->reg_buffer[0] = 0x0; // reset bit
					err = func_ptr->CallBack_Coils(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						1, // one bit
						true);
				}
				else
				{
					err = Ex_IllegalDataValue;
				}
			}
			break;

		case 6: /* Function Code: 06 */
			if (func_ptr->CallBack_Register)
			{
				cTool.wValue = h_rx->result.Parameter;
				iModbusHandel->reg_buffer[0] = cTool.cValue[1];
				iModbusHandel->reg_buffer[1] = cTool.cValue[0];

				err = func_ptr->CallBack_Register(
					iModbusHandel,
					iModbusHandel->reg_buffer,
					h_rx->result.DataAddres,
					1, // one value
					true);
			}
			break;

		case 15: /* Function Code: 15 */
			if (func_ptr->CallBack_Coils)
			{
				if (h_rx->result.Parameter <= 2000)
				{
					memcpy(iModbusHandel->reg_buffer, ARC_MODBUS_RTU_GetMultipleWrite(iModbusHandel), sizeof(iModbusHandel->reg_buffer));
					err = func_ptr->CallBack_Coils(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						h_rx->result.Parameter, // 15's Parameter is number of coils to write
						true);
				}
				else
				{
					err = Ex_IllegalDataValue;
				}
			}

			break;

		case 16: /* Function Code: 16 */
			if (func_ptr->CallBack_Register)
			{
				if (h_rx->result.Parameter <= 125)
				{
					memcpy(iModbusHandel->reg_buffer, ARC_MODBUS_RTU_GetMultipleWrite(iModbusHandel), sizeof(iModbusHandel->reg_buffer));
					err = func_ptr->CallBack_Register(
						iModbusHandel,
						iModbusHandel->reg_buffer,
						h_rx->result.DataAddres,
						h_rx->result.Parameter, // 16's Parameter is number of registers to write 
						true);
				}
				else
				{
					err = Ex_IllegalDataValue;
				}
			}
			break;

		default:
			err = Ex_IllegalFunction; //收到還沒有實作的Function Code
			break;
		}


		if (err == Ex_not_initial)
			err = Ex_IllegalFunction;
		
		ARC_MODBUS_RTU_PacketMake(iModbusHandel, err); /* exceptions 錯誤訊息 */

		if (func_ptr->Event_TxWork && h_tx->tx_msg_length > 0)
		{
			func_ptr->Event_TxWork(iModbusHandel, h_tx->tx_msg_buffer, h_tx->tx_msg_length);
		}
	}
}
#endif

#if ARC_Modbus_ASCII_ENABLED > 0

#endif

#if ARC_Modbus_Master_TCP_ENABLED > 0

#endif





