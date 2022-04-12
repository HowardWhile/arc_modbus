#ifndef mbRegisterProcess_h__
#define mbRegisterProcess_h__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	bool mbRegister_CheckAccessRegion(uint16_t DefStartAddr, uint16_t DefNumber, uint16_t iAddr, uint16_t iNumber);

	void mbRegister_SetBit(char* iPtr, uint16_t iStartAddr, uint16_t iIndex, bool iEnable);
	bool mbRegister_GetBit(char* iPtr, uint16_t iStartAddr, uint16_t iIndex);

#ifdef __cplusplus
}
#endif
#endif // mbRegisterProcess_h__
