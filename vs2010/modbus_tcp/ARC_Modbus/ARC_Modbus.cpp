#include "ARC_Modbus.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

namespace ARC_Modbus
{
#if ARC_Modbus_RTU_ENABLED > 0
	RTU::RTU()
	{
	}


	RTU::~RTU()
	{
	}

	void RTU::Initial(int iSlaveID)
	{
		this->decoder.Initial(iSlaveID);
		this->ecoder.Initial(iSlaveID);
	}

	void RTU::RxWork(char* iBytes, int iLength)
	{		
		for (int i = 0; i < iLength; i++)
		{
			 this->RxWork(iBytes[i]);
		}		
	}

	void RTU::RxWork(char iByte)
	{		
		if (this->decoder.RxByte(iByte) == true)
		{
			Exception err = Ex_not_initial;

			switch (this->decoder.FunctionCode)
			{
			case 1: /* Function Code: 01 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter <= 2000)// 01 modbus協定表示 最大的線圈讀取數是2000
					{
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter,
							false);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 2: /* Function Code: 02 */
				if (this->CallBack_InputCoils)
				{
					if (this->decoder.Parameter <= 2000)// 02 modbus協定表示 最大的線圈讀取數是2000
					{
						err = this->CallBack_InputCoils(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 3: /* Function Code: 03 */
				if (this->CallBack_Register)
				{
					if (this->decoder.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
					{
						err = this->CallBack_Register(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter,
							false);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 4: /* Function Code: 04 */
				if (this->CallBack_InputRegs)
				{
					if (this->decoder.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
					{
						err = this->CallBack_InputRegs(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 5: /* Function Code: 05 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter == 0xFF00)
					{
						this->reg_buffer[0] = 0x1; // set bit
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
							1, // one bit
							true);
					}
					else if (this->decoder.Parameter == 0x0000)
					{
						this->reg_buffer[0] = 0x0; // reset bit
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
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
				if (this->CallBack_Register)
				{
					tu_convert16 cTool;
					cTool.wValue = this->decoder.Parameter;
					this->reg_buffer[0] = cTool.cValue[1];
					this->reg_buffer[1] = cTool.cValue[0];

					err = this->CallBack_Register(
						this->reg_buffer,
						this->decoder.DataAddres,
						1, // one value
						true);
				}
				break;

			case 15: /* Function Code: 15 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter <= 2000)
					{
						memcpy(this->reg_buffer, this->decoder.GetRegsBuffer(), sizeof(this->reg_buffer));
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter, // 15's Parameter is number of coils to write
							true);
					}
					else
					{
						err = Ex_IllegalDataValue;
					}
				}

				break;

			case 16: /* Function Code: 16 */
				if (this->CallBack_Register)
				{
					if (this->decoder.Parameter <= 125)
					{
						memcpy(this->reg_buffer, this->decoder.GetRegsBuffer(), sizeof(this->reg_buffer));
						err = this->CallBack_Register(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter, // 16's Parameter is number of registers to write 
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

			int tx_msg_length = 0;
			if (err != Ex_not_initial)
			{
				this->ecoder.GetResponse(
					this->tx_msg_buffer, tx_msg_length,/* 回應的封包寫在這個buffer */
					this->reg_buffer,	/* iFunctionCode = 01,02,03,04 會用到 */
					this->decoder,		/* 把 decoder 的參數傳遞給 ecoder*/
					err);				/* exceptions 錯誤訊息 */
			}
			else
			{
#ifdef DEBUG_CONTROL
				printf("[RTU::RxWork] callback function (F:%d) not initial \r\n", this->decoder.FunctionCode);
#endif // DEBUG_CONTROL
				err = Ex_IllegalFunction;
				this->ecoder.GetResponse(
					this->tx_msg_buffer, tx_msg_length,/* 回應的封包寫在這個buffer */
					this->reg_buffer,	/* iFunctionCode = 01,02,03,04 會用到 */
					this->decoder,		/* 把 decoder 的參數傳遞給 ecoder*/
					err);				/* exceptions 錯誤訊息 */
			}

			if (this->Event_TxWork && tx_msg_length > 0)
			{
				this->Event_TxWork(this->tx_msg_buffer, tx_msg_length);
			}
		}		
	}

#endif

#if ARC_Modbus_ASCII_ENABLED > 0
	ASCII::ASCII()
	{

	}

	ASCII::~ASCII()
	{

	}

	void ASCII::Initial(int iSlaveID)
	{
		this->decoder.Initial(iSlaveID);
		this->ecoder.Initial(iSlaveID);
	}

	void ASCII::RxWork(char* iBytes, int iLength)
	{
		for (int i = 0; i < iLength; i++)
		{
			this->RxWork(iBytes[i]);
		}
	}

	void ASCII::RxWork(char iByte)
	{
		if (this->decoder.RxByte(iByte) == true)
		{
			Exception err = Ex_not_initial;

			switch (this->decoder.FunctionCode)
			{
			case 1: /* Function Code: 01 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter <= 2000)// 01 modbus協定表示 最大的線圈讀取數是2000
					{
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter,
							false);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 2: /* Function Code: 02 */
				if (this->CallBack_InputCoils)
				{
					if (this->decoder.Parameter <= 2000)// 02 modbus協定表示 最大的線圈讀取數是2000
					{
						err = this->CallBack_InputCoils(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 3: /* Function Code: 03 */
				if (this->CallBack_Register)
				{
					if (this->decoder.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
					{
						err = this->CallBack_Register(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter,
							false);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 4: /* Function Code: 04 */
				if (this->CallBack_InputRegs)
				{
					if (this->decoder.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
					{
						err = this->CallBack_InputRegs(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 5: /* Function Code: 05 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter == 0xFF00)
					{
						this->reg_buffer[0] = 0x1; // set bit
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
							1, // one bit
							true);
					}
					else if (this->decoder.Parameter == 0x0000)
					{
						this->reg_buffer[0] = 0x0; // reset bit
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
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
				if (this->CallBack_Register)
				{
					tu_convert16 cTool;
					cTool.wValue = this->decoder.Parameter;
					this->reg_buffer[0] = cTool.cValue[1];
					this->reg_buffer[1] = cTool.cValue[0];

					err = this->CallBack_Register(
						this->reg_buffer,
						this->decoder.DataAddres,
						1, // one value
						true);
				}
				break;

			case 15: /* Function Code: 15 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter <= 2000)
					{
						memcpy(this->reg_buffer, this->decoder.GetRegsBuffer(), sizeof(this->reg_buffer));
						err = this->CallBack_Coils(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter, // 15's Parameter is number of coils to write
							true);
					}
					else
					{
						err = Ex_IllegalDataValue;
					}
				}

				break;

			case 16: /* Function Code: 16 */
				if (this->CallBack_Register)
				{
					if (this->decoder.Parameter <= 125)
					{
						memcpy(this->reg_buffer, this->decoder.GetRegsBuffer(), sizeof(this->reg_buffer));
						err = this->CallBack_Register(
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter, // 16's Parameter is number of registers to write 
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

			int tx_msg_length = 0;
			if (err != Ex_not_initial)
			{
				this->ecoder.GetResponse(
					this->tx_msg_buffer, tx_msg_length,/* 回應的封包寫在這個buffer */
					this->reg_buffer,	/* iFunctionCode = 01,02,03,04 會用到 */
					this->decoder,		/* 把 decoder 的參數傳遞給 ecoder*/
					err);				/* exceptions 錯誤訊息 */
			}
			else
			{
#ifdef DEBUG_CONTROL
				printf("[ASCII::RxWork] callback function (F:%d) not initial \r\n", this->decoder.FunctionCode);
#endif // DEBUG_CONTROL
			}

			if (this->Event_TxWork && tx_msg_length > 0)
			{
				this->Event_TxWork(this->tx_msg_buffer, tx_msg_length);
			}
		}
	}
#endif

#if ARC_Modbus_TCP_ENABLED > 0
	TCP::TCP()
	{

	}

	TCP::TCP(void* iContext)
	{
		this->parent = iContext;
	}

	TCP::~TCP()
	{

	}

	void TCP::Initial(int iSlaveID)
	{
		this->decoder.Initial(iSlaveID);
		this->ecoder.Initial(iSlaveID);
	}

	void TCP::RxWork(char* iBytes, int iLength)
	{
		for (int i = 0; i < iLength; i++)
		{
			this->RxWork(iBytes[i]);
		}
	}

	void TCP::RxWork(char iByte)
	{
		this->decoder.RxByte(iByte);
	}

	void TCP::FrameRenew(void)
	{
		if (this->decoder.FrameRenew() == true)
		{
			Exception err = Ex_not_initial;

			switch (this->decoder.FunctionCode)
			{
			case 1: /* Function Code: 01 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter <= 2000)// 01 modbus協定表示 最大的線圈讀取數是2000
					{
						err = this->CallBack_Coils(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter,
							false);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 2: /* Function Code: 02 */
				if (this->CallBack_InputCoils)
				{
					if (this->decoder.Parameter <= 2000)// 02 modbus協定表示 最大的線圈讀取數是2000
					{
						err = this->CallBack_InputCoils(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 3: /* Function Code: 03 */
				if (this->CallBack_Register)
				{
					if (this->decoder.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
					{
						err = this->CallBack_Register(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter,
							false);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 4: /* Function Code: 04 */
				if (this->CallBack_InputRegs)
				{
					if (this->decoder.Parameter <= 125)// modbus協定表示 最大的數值讀取數是125
					{
						err = this->CallBack_InputRegs(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter);
					}
					else
					{
						err = Ex_IllegalDataValue; // 
					}
				}
				break;

			case 5: /* Function Code: 05 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter == 0xFF00)
					{
						this->reg_buffer[0] = 0x1; // set bit
						err = this->CallBack_Coils(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
							1, // one bit
							true);
					}
					else if (this->decoder.Parameter == 0x0000)
					{
						this->reg_buffer[0] = 0x0; // reset bit
						err = this->CallBack_Coils(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
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
				if (this->CallBack_Register)
				{
					tu_convert16 cTool;
					cTool.wValue = this->decoder.Parameter;
					this->reg_buffer[0] = cTool.cValue[1];
					this->reg_buffer[1] = cTool.cValue[0];

					err = this->CallBack_Register(
						this->parent,
						this->reg_buffer,
						this->decoder.DataAddres,
						1, // one value
						true);
				}
				break;

			case 15: /* Function Code: 15 */
				if (this->CallBack_Coils)
				{
					if (this->decoder.Parameter <= 2000)
					{
						memcpy(this->reg_buffer, this->decoder.GetRegsBuffer(), sizeof(this->reg_buffer));
						err = this->CallBack_Coils(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter, // 15's Parameter is number of coils to write
							true);
					}
					else
					{
						err = Ex_IllegalDataValue;
					}
				}

				break;

			case 16: /* Function Code: 16 */
				if (this->CallBack_Register)
				{
					if (this->decoder.Parameter <= 125)
					{
						memcpy(this->reg_buffer, this->decoder.GetRegsBuffer(), sizeof(this->reg_buffer));
						err = this->CallBack_Register(
							this->parent,
							this->reg_buffer,
							this->decoder.DataAddres,
							this->decoder.Parameter, // 16's Parameter is number of registers to write 
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
			}// end of switch (this->decoder.FunctionCode)

			int tx_msg_length = 0;
			if (err != Ex_not_initial)
			{
				this->ecoder.GetResponse(
					this->tx_msg_buffer, tx_msg_length,/* 回應的封包寫在這個buffer */
					this->reg_buffer,	/* iFunctionCode = 01,02,03,04 會用到 */
					this->decoder,		/* 把 decoder 的參數傳遞給 ecoder*/
					err);				/* exceptions 錯誤訊息 */
			}
			else
			{
#ifdef DEBUG_CONTROL
				printf("[TCP::FrameRenew] callback function (F:%d) not initial \r\n", this->decoder.FunctionCode);
#endif // DEBUG_CONTROL
			}

			if (this->Event_TxWork && tx_msg_length > 0)
			{
				this->Event_TxWork(this->tx_msg_buffer, tx_msg_length);
			}

		}// end of if ( this->decoder.FrameRenew() == true)
	}

#endif

#if ARC_Modbus_Master_RTU_ENABLED > 0
	RTU_Master::RTU_Master()
	{
		this->request.ByteBuffer_ptr = this->byte_buffer;
		this->response.ByteBuffer_ptr = this->byte_buffer;

	}

	RTU_Master::~RTU_Master()
	{

	}

	void RTU_Master::Polling(void)
	{

	}
#endif // ARC_Modbus_Master_RTU_ENABLED

#if ARC_Modbus_Master_ASCII_ENABLED > 0
	ASCII_Master::ASCII_Master()
	{
		this->request.ByteBuffer_ptr = this->byte_buffer;
		this->response.ByteBuffer_ptr = this->byte_buffer;
	}

	ASCII_Master::~ASCII_Master()
	{

	}

	void ASCII_Master::Polling(void)
	{

	}
#endif // ARC_Modbus_Master_ASCII_ENABLED

#if ARC_Modbus_Master_TCP_ENABLED > 0
	TCP_Master::TCP_Master()
	{
		this->request.ByteBuffer_ptr = this->byte_buffer;
		this->response.ByteBuffer_ptr = this->byte_buffer;
		this->packet.datas = this->byte_buffer;
		this->packet.size = 0;
		
		this->Event_Response = 0;
	}

	TCP_Master::~TCP_Master()
	{

	}

	void TCP_Master::RxWork(char* iBytes, int iLength)
	{
		for (int i = 0; i < iLength; i++)
		{
			this->RxWork(iBytes[i]);
		}
	}

	void TCP_Master::RxWork(char iByte)
	{
		if (this->step == polling_step_wait_response && 
			this->packet.size != sizeof(this->byte_buffer))
		{
			this->byte_buffer[this->packet.size++] = iByte;
			this->Polling();
		}
		else
		{
			this->packet.size  = 0;
		}
	}

	void TCP_Master::Polling(void)
	{
		if (this->task_size != 0)
		{
			switch (this->step)
			{
			case polling_step_send_request:

				if (this->task_lists[this->task_count].Type != PollingTaskType_None)
				{
					PollingTaskRef task = this->task_lists[this->task_count];
					if (task.Type != PollingTaskType_None && task.Number != 0)
					{
						pkg packet;
						if (PollingTaskType_Read_RW_Coils <= task.Type && task.Type <= PollingTaskType_Read_RO_Regs)
						{
							packet = this->request.Read(task.SlaveID, task.Type, task.Address, task.Number);
							if (this->Event_TxWork && packet.size > 0)
								this->Event_TxWork(packet.datas, packet.size);
						}
						else if (task.Type == PollingTaskType_Write_Coils)
						{
							if (this->CallBack_WriteCoilsRequest)
							{
								char* oRegs;
								this->CallBack_WriteCoilsRequest(&oRegs, this->task_count, task);
								packet = this->request.Write(task.SlaveID, task.Address, oRegs, task.Number);
								if (this->Event_TxWork && packet.size > 0)
									this->Event_TxWork(packet.datas, packet.size);
							}
							else
							{
								printf("[Modbus TCP Master] CallBack_WriteCoilsRequest not initial!\r\n");
							}
						}
						else if (task.Type == PollingTaskType_Write_Regs)
						{
							if (this->CallBack_WriteCoilsRequest)
							{
								short* oRegs;
								this->CallBack_WriteRegsRequest(&oRegs, this->task_count, task);
								packet = this->request.Write(task.SlaveID, task.Address, oRegs, task.Number);
								if (this->Event_TxWork && packet.size > 0)
									this->Event_TxWork(packet.datas, packet.size);
							}
							else
							{
								printf("[Modbus TCP Master] CallBack_WriteRegsRequest not initial!\r\n");
							}
						}
						else
						{
							// unknown task
							printf("[debug]Modbus TCP Master unknown request task\r\n");
						}

						this->check_time = clock();
						this->packet.size = 0;
						this->respones_len = this->response.ReponseLength(task.Type, task.Number);
						this->step = polling_step_wait_response;
					} // end if (task.Type != PollingTaskType_None && task.Number != 0)
					else
					{
						this->task_count++;
						this->task_count = this->task_count % this->task_size;
					}
				}// end of if (this->task_lists[this->task_count].Type != PollingTaskType_None)
				else
				{
					this->task_count++;
					this->task_count = this->task_count % this->task_size;
				}
				break;

			case polling_step_wait_response:
				if (clock() - this->check_time > timeout)
				{
					PollingTaskRef task = this->task_lists[this->task_count];

					if (this->Event_Response)
					{
						this->Event_Response(this, this->task_count, task, Ex_Acknowledge);
					}

					this->task_count++;
					this->task_count = this->task_count % this->task_size;

					this->step = polling_step_send_request;
				}
				else
				{
					if (this->packet.size == 9)
					{
						PollingTaskRef task = this->task_lists[this->task_count];
						int ex = this->response.CheckException(this->packet, task.SlaveID);
						if (ex > 0)
						{//偵測到例外訊息
						 //printf("[debug] response.CheckException : %d\r\n", ex);
							if (this->Event_Response)
								this->Event_Response(this, this->task_count, task, (Exception)ex);

							this->task_count++;
							this->task_count = this->task_count % this->task_size;

							this->check_time = clock();
							this->step = polling_step_task_delay;
						}
					}
					else if (this->packet.size == this->respones_len)
					{
						PollingTaskRef task = this->task_lists[this->task_count];
						if (PollingTaskType_Read_RW_Coils <= task.Type && task.Type <= PollingTaskType_Read_RO_Regs)
						{
							if (this->response.CheckRead(this->packet, task.SlaveID, task.Number) == true)
							{
								if (this->Event_Response)
									this->Event_Response(this, this->task_count, task, Ex_none);
							}
						}
						else if (task.Type == PollingTaskType_Write_Coils || task.Type == PollingTaskType_Write_Regs)
						{
							if (this->response.CheckWrite(this->packet, task.SlaveID, task.Address) == true)
							{
								if (this->Event_Response)
									this->Event_Response(this, this->task_count, task, Ex_none);
							}
						}

						this->task_count++;
						this->task_count = this->task_count % this->task_size;

						this->check_time = clock();
						this->step = polling_step_task_delay;
					}
				}
				break;
			case polling_step_task_delay:
				if (clock() - this->check_time >= this->delay)
				{
					this->step = polling_step_send_request;
				}
				break;
			default:
				break;
			}// end of switch (this->step)
		}// end of if (this->task_size != 0)		
	}

	bool TCP_Master::GetCoil(int iIndex)
	{
		return this->response.GetCoil(this->packet, iIndex);
	}

	short TCP_Master::GetReg(int iIndex)
	{
		return this->response.GetReg(this->packet, iIndex);
	}
#endif // ARC_Modbus_Master_TCP_ENABLED
}
