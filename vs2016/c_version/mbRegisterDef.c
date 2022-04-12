#include "mbRegisterDef.h"
#include "mbRegisterProcess.h"

#include <string.h>

typedef union
{
	short wValue;
	char cValue[2];
}tu_convert16;

void mbRegister_Initial(mbRegister_HandleTypeDef* iRegsHandel)
{
	memset(iRegsHandel->RW_BITS, 0, sizeof(iRegsHandel->RW_BITS));
	memset(iRegsHandel->RO_BITS, 0, sizeof(iRegsHandel->RO_BITS));
	memset(iRegsHandel->RW_REGS, 0, sizeof(iRegsHandel->RW_REGS));
	memset(iRegsHandel->RO_REGS, 0, sizeof(iRegsHandel->RO_REGS));
}

bool mbRegister_RW_Bits(mbRegister_HandleTypeDef* iRegsHandel, char* ioBuffer, uint16_t iAddress, uint16_t iNumber, bool IsWrite)
{
	char* regs = iRegsHandel->RW_BITS;
	bool bit_value;
	int bit_idx;

	// 先檢查範圍
	if (!mbRegister_CheckAccessRegion(mbRegister_RW_BITS_ADDR_START, mbRegister_RW_BITS_NUM, iAddress, iNumber))
		return false;

	if (IsWrite)
	{
		for (bit_idx = 0; bit_idx < iNumber; bit_idx++)
		{
			bit_value = mbRegister_GetBit(ioBuffer, 0, bit_idx);
			mbRegister_SetBit(regs, iAddress, bit_idx, bit_value);
		}
	}
	else
	{
		for (bit_idx = 0; bit_idx < iNumber; bit_idx++)
		{
			bit_value = mbRegister_GetBit(regs, iAddress, bit_idx);
			mbRegister_SetBit(ioBuffer, 0, bit_idx, bit_value);
		}
	}

	return true;
}

bool mbRegister_RO_Bits(mbRegister_HandleTypeDef* iRegsHandel, char* oBuffer, uint16_t iAddress, uint16_t iNumber)
{
	char* regs = iRegsHandel->RO_BITS;
	bool bit_value;
	int bit_idx;

	// 先檢查範圍
	if (!mbRegister_CheckAccessRegion(mbRegister_RO_BITS_ADDR_START, mbRegister_RO_BITS_NUM, iAddress, iNumber))
		return false;

	for (bit_idx = 0; bit_idx < iNumber; bit_idx++)
	{
		bit_value = mbRegister_GetBit(regs, iAddress, bit_idx);
		mbRegister_SetBit(oBuffer, 0, bit_idx, bit_value);
	}

	return true;
}

bool mbRegister_RW_Regs(mbRegister_HandleTypeDef* iRegsHandel, char* ioBuffer, uint16_t iAddress, uint16_t iNumber, bool IsWrite)
{
	short* regs = iRegsHandel->RW_REGS;
	tu_convert16 cTool;
	int value_idx;

	// 先檢查範圍
	if (!mbRegister_CheckAccessRegion(mbRegister_RW_REGS_ADDR_START, mbRegister_RW_REGS_NUM, iAddress, iNumber))
		return false;

	if (IsWrite)
	{
		for (value_idx = 0; value_idx < iNumber; value_idx++)
		{
			cTool.cValue[1] = *ioBuffer++;
			cTool.cValue[0] = *ioBuffer++;
			*regs++ = cTool.wValue;
		}
	}
	else
	{
		for (value_idx = 0; value_idx < iNumber; value_idx++)
		{
			cTool.wValue = *regs++;
			*ioBuffer++ = cTool.cValue[1];
			*ioBuffer++ = cTool.cValue[0];
		}
	}

	return true;

}

bool mbRegister_RO_Regs(mbRegister_HandleTypeDef* iRegsHandel, char* oBuffer, uint16_t iAddress, uint16_t iNumber)
{
	short* regs = iRegsHandel->RO_REGS;
	tu_convert16 cTool;
	int value_idx;

	// 先檢查範圍
	if (!mbRegister_CheckAccessRegion(mbRegister_RO_REGS_ADDR_START, mbRegister_RO_REGS_NUM, iAddress, iNumber))
		return false;
	
	for (value_idx = 0; value_idx < iNumber; value_idx++)
	{
		cTool.wValue = *regs++;
		*oBuffer++ = cTool.cValue[1];
		*oBuffer++ = cTool.cValue[0];
	}

	return true;
}
