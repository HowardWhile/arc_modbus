#include "mbRegisterMapping.h"
	Register::Register()
	{

	}

	Register::~Register()
	{
		
	}
	void Register::Initial( void )
	{
		for (int i = 0; i < this->RegNumber_Coile; i++)
			this->_Coile[i] = false;

		for (int i = 0; i < this->RegNumber_ReadOnlyCoile; i++)
			this->_ReadOnlyCoile[i] = false;

		for (int i = 0; i < this->RegNumber_Register; i++)
			this->_Register[i] = 0;

		for (int i = 0; i < this->RegNumber_ReadOnlyReg; i++)
			this->_ReadOnlyReg[i] = 0;
	}
	
	void Register::set_bit(char* iPtr, unsigned short iStartAddr, unsigned short iIndex, bool iEnable)
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

	bool Register::get_bit(char* iPtr, unsigned short iStartAddr, unsigned short iIndex)
	{
		// http://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit-in-c-c
		// Checking a bit, bit = (number >> x) & 1;
		int temp_idx = iStartAddr + iIndex;
		return (iPtr[temp_idx / 8] >> (temp_idx % 8)) & 1;
	}

	bool Register::GetRegsBuffer(RegType iRegType, unsigned short iAddress, unsigned short iNumber, char *regBuffer)
	{
		/* --------------------------------- */
		// 先確定會不會超過吧...
		if (this->check_access_region(iAddress, iRegType, iNumber) == false)
		{
			return false;
		}
		/* --------------------------------- */
		static const void* list_ptr[4] =
		{
			this->_Coile,
			this->_ReadOnlyCoile,
			this->_Register,
			this->_ReadOnlyReg
		};

		if (iRegType == this->RegType_Coile || iRegType == this->RegType_InputCoile)
		{ 
			char* reg_list = (char*)list_ptr[iRegType];

			// 雖然是通式 但可能會很耗費效能
			bool bit_value;			
			for (int bit_idx = 0; bit_idx < iNumber; bit_idx++)
			{				
				bit_value = this->get_bit(reg_list, iAddress, bit_idx);
				this->set_bit(regBuffer, 0, bit_idx, bit_value);
			}

			return true;
		}
		else if (iRegType == this->RegType_Register || iRegType == this->RegType_InputRegs)
		{
			short* reg_list = ((short*)list_ptr[iRegType]) + iAddress;
		
			tu_convert16 cTool;
			for (int value_idx = 0; value_idx < iNumber; value_idx ++)
			{
				cTool.wValue = *reg_list++;
				*regBuffer++ = cTool.cValue[1];
				*regBuffer++ = cTool.cValue[0];
			}
			return true;
		}
		else
		{
			// 給錯Type了
			return false;
		}
		return true;
	}

	bool Register::SetRegsBuffer(RegType iRegType, unsigned short iAddress, unsigned short iNumber, char *regBuffer)
	{
		/* --------------------------------- */
		// 先確定會不會超過吧...
		if (this->check_access_region(iAddress, iRegType, iNumber) == false)
		{
			return false;
		}
		/* --------------------------------- */
		static const void* list_ptr[4] =
		{
			this->_Coile,
			this->_ReadOnlyCoile,
			this->_Register,
			this->_ReadOnlyReg
		};

		if (iRegType == this->RegType_Coile || iRegType == this->RegType_InputCoile)
		{
			char* reg_list = (char*)list_ptr[iRegType];

			bool bit_value;
			for (int bit_idx = 0; bit_idx < iNumber; bit_idx++)
			{
				bit_value = this->get_bit(regBuffer, 0 , bit_idx);
				this->set_bit(reg_list, iAddress, bit_idx, bit_value);
			}

			return true;
		}
		else if (iRegType == this->RegType_Register || iRegType == this->RegType_InputRegs)
		{
			short* reg_list = ((short*)list_ptr[iRegType]) + iAddress;

			tu_convert16 cTool;
			for (int value_idx = 0; value_idx < iNumber; value_idx++)
			{
				cTool.cValue[1] = *regBuffer++;
				cTool.cValue[0] = *regBuffer++;
				*reg_list++ = cTool.wValue;
			}
			return true;
		}
		else
		{
			// 給錯Type了
			return false;
		}

		return true;
	}

	bool Register::check_access_region(unsigned short iAddress, RegType iRegType, unsigned short iNumber)
	{
		static const int list_start_addr[4] = {
			this->StartAddr_Coile,
			this->StartAddr_ReadOnlyCoile,
			this->StartAddr_Register,
			this->StartAddr_ReadOnlyReg
		};

		static const int list_reg_number[4] = {
			this->RegNumber_Coile,
			this->RegNumber_ReadOnlyCoile,
			this->RegNumber_Register,
			this->RegNumber_ReadOnlyReg
		};

		static const int list_end_addr[4] = {
			list_start_addr[0] + list_reg_number[0],
			list_start_addr[1] + list_reg_number[1],
			list_start_addr[2] + list_reg_number[2],
			list_start_addr[3] + list_reg_number[3]
		};

		if ((iAddress < list_start_addr[iRegType]) ||
			(iAddress + iNumber) > list_end_addr[iRegType])
		{
			return false;
		}
		else
		{
			return true;
		}
	}
