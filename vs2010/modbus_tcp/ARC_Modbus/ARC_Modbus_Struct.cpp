﻿#include "ARC_Modbus_Struct.h"

#include <string.h>
#include <stdlib.h>

namespace ARC_Modbus
{


	Command_Parameter::Command_Parameter()
	{

	}

	Command_Parameter::~Command_Parameter()
	{

	}

	modbus_slave_base::modbus_slave_base()
	{
		this->Event_TxWork = 0;

		this->CallBack_Coils = 0;
		this->CallBack_InputCoils = 0;
		this->CallBack_Register = 0;
		this->CallBack_InputRegs = 0;
	}

	modbus_slave_base::~modbus_slave_base()
	{

	}

	coder_base::coder_base()
	{

	}

	coder_base::~coder_base()
	{

	}

	void coder_base::Initial(int iSlaveID)
	{
		this->slave_id = iSlaveID;
	}

	short CRC16(const char *iBytes, unsigned short iLength)
	{
		// ref: free modbus lib mbcrc.c
		static const unsigned char aucCRCHi[] = {
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
			0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
			0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
			0x00, 0xC1, 0x81, 0x40
		};

		static const unsigned char  aucCRCLo[] = {
			0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
			0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
			0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
			0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
			0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
			0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
			0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
			0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
			0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
			0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
			0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
			0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
			0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
			0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
			0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
			0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
			0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
			0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
			0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
			0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
			0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
			0x41, 0x81, 0x80, 0x40
		};

		unsigned char ucCRCHi = 0xFF;
		unsigned char ucCRCLo = 0xFF;
		int             iIndex;

		while (iLength--)
		{
			iIndex = ucCRCLo ^ *(iBytes++);
			ucCRCLo = (unsigned char)(ucCRCHi ^ aucCRCHi[iIndex]);
			ucCRCHi = aucCRCLo[iIndex];
		}
		return (unsigned short)(ucCRCHi << 8 | ucCRCLo);
	}

	char LRC_String(const char* iString, unsigned int iLength)
	{
		// https://just9x.wordpress.com/2009/09/03/modbus-asscii-lrc/
		if (iLength % 2 != 0)
		{
			// 字串不是2的倍數沒辦法全部解成byte(2個字解成1個byte)
			return 0;
		}
		else
		{
			unsigned char ucLRC = 0;  /* LRC char initialized */

			char temp_str[3];
			temp_str[2] = 0;

			char str2byte;
			while (iLength)
			{
				memcpy(temp_str, iString, 2);
				str2byte = (unsigned char)strtol(temp_str, 0, 16);
				iString = iString + 2;
				iLength -= 2;
				ucLRC += str2byte;   /* Add buffer byte without carry */
			}

			ucLRC = (char)(-((char)ucLRC));
			return ucLRC;
		}
	}

	char LRC(const char *iBytes, unsigned short iLength)
	{
		unsigned char ucLRC = 0;  /* LRC char initialized */

		while (iLength--)
		{
			ucLRC += *iBytes++;   /* Add buffer byte without carry */
		}

		/* Return twos complement */
		ucLRC = (char)(-((char)ucLRC));
		return ucLRC;
	}

	modbus_master_base::modbus_master_base()
	{
		this->step = polling_step_send_request;
		this->check_time = 0;
		this->task_size = 0;
		this->task_count = 0;
		this->Event_TxWork = 0;
		this->CallBack_WriteCoilsRequest = 0;
		this->CallBack_WriteRegsRequest = 0;
	}

	modbus_master_base::~modbus_master_base()
	{

	}

	bool modbus_master_base::Add_PollingTask(ARC_Modbus::PollingTaskRef iTask)
	{
		if (this->task_size >= MAX_TASK_SIZE)
		{
			return false;
		}
		else
		{
			this->task_lists[this->task_size++] = iTask;
			return true;
		}
	}

	void modbus_master_base::Clear_PollingTask()
	{
		this->task_size = 0;
		this->task_count = 0;
		for (int i = 0; i < MAX_TASK_SIZE; i++)
		{
			this->task_lists[i].Address = 0;
			this->task_lists[i].Number = 0;
			this->task_lists[i].SlaveID = 0;
			this->task_lists[i].Type = PollingTaskType_None;
		}
	}

	pkg::pkg()
	{

	}

	pkg::~pkg()
	{

	}

	void request_response_base::set_bit(char* iPtr, unsigned short iIndex, bool iEnable)
	{
		// http://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit-in-c-c
		int temp_idx = iIndex;
		if (iEnable)
		{
			// Setting a bit, number |= 1 << x;
			iPtr[temp_idx / 8] |= (1 << (temp_idx % 8));
		}
		else
		{
			// Clearing a bit, number ^= 1 << x;
			iPtr[temp_idx / 8] &= ~(1 << (temp_idx % 8));
		}
	}

	bool request_response_base::get_bit(char* iPtr, unsigned short iIndex)
	{
		// http://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit-in-c-c
		// Checking a bit, bit = (number >> x) & 1;
		int temp_idx = iIndex;
		return (iPtr[temp_idx / 8] >> (temp_idx % 8)) & 1;
	}

	void request_response_base::set_int16(char* iPtr, unsigned short iIndex, short iValue)
	{
		tu_convert16 cTool;
		cTool.wValue = iValue;
		iPtr[iIndex*2] = cTool.cValue[1];
		iPtr[iIndex*2+1] = cTool.cValue[0];
	}

	short request_response_base::get_int16(char* iPtr, unsigned short iIndex)
	{
		tu_convert16 cTool;
		cTool.cValue[1] = iPtr[iIndex*2];
		cTool.cValue[0] = iPtr[iIndex*2+1];
		return cTool.wValue;
	}

	PollingTaskRef::PollingTaskRef()
	{
		this->Address = 0;
		this->Number = 0;
		this->SlaveID = 0;
		this->Type = PollingTaskType_None;
	}


	PollingTaskRef::~PollingTaskRef()
	{

	}

}
