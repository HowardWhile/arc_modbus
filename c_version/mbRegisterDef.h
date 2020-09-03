
#ifndef mbRegister_h__
#define mbRegister_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#define mbRegister_RW_BITS_ADDR_START 0
#define mbRegister_RW_BITS_NUM 100

#define mbRegister_RO_BITS_ADDR_START 0
#define mbRegister_RO_BITS_NUM 200

#define mbRegister_RW_REGS_ADDR_START 0
#define mbRegister_RW_REGS_NUM 50

#define mbRegister_RO_REGS_ADDR_START 0
#define mbRegister_RO_REGS_NUM 60


	typedef struct 
	{
		char RW_BITS[mbRegister_RW_BITS_NUM / 8 + 1];
		char RO_BITS[mbRegister_RO_BITS_NUM / 8 + 1];
		short RW_REGS[mbRegister_RW_REGS_NUM];
		short RO_REGS[mbRegister_RO_REGS_NUM];

	}mbRegister_HandleTypeDef;

	void mbRegister_Initial(mbRegister_HandleTypeDef* iRegsHandel);
	bool mbRegister_RW_Bits(mbRegister_HandleTypeDef* iRegsHandel, char* ioBuffer, uint16_t iAddress, uint16_t iNumber, bool IsWrite);
	bool mbRegister_RO_Bits(mbRegister_HandleTypeDef* iRegsHandel, char* oBuffer, uint16_t iAddress, uint16_t iNumber);
	bool mbRegister_RW_Regs(mbRegister_HandleTypeDef* iRegsHandel, char* ioBuffer, uint16_t iAddress, uint16_t iNumber, bool IsWrite);
	bool mbRegister_RO_Regs(mbRegister_HandleTypeDef* iRegsHandel, char* oBuffer, uint16_t iAddress, uint16_t iNumber);

#ifdef __cplusplus
}
#endif

#endif // mbRegister_h__