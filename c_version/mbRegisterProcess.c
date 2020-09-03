#include "mbRegisterProcess.h"

bool mbRegister_CheckAccessRegion(uint16_t DefStartAddr, uint16_t DefNumber, uint16_t iAddr, uint16_t iNumber)
{
	return (iAddr >= DefStartAddr) && ((iAddr + iNumber) <= (DefStartAddr + DefNumber));
}

void mbRegister_SetBit(char* iPtr, uint16_t iStartAddr, uint16_t iIndex, bool iEnable)
{
	// http://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit-in-c-c
	int temp_idx = iStartAddr + iIndex;
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

bool mbRegister_GetBit(char* iPtr, uint16_t iStartAddr, uint16_t iIndex)
{
	// http://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit-in-c-c
	// Checking a bit, bit = (number >> x) & 1;
	int temp_idx = iStartAddr + iIndex;
	return (iPtr[temp_idx / 8] >> (temp_idx % 8)) & 1;
}
