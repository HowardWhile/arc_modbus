#include "ARC_Modbus_Response.h"
//中文字

namespace ARC_Modbus
{

#if ARC_Modbus_Master_RTU_ENABLED > 0
	Response_RTU::Response_RTU()
	{

	}

	Response_RTU::~Response_RTU()
	{

	}
#endif // ARC_Modbus_Master_RTU_ENABLED

#if ARC_Modbus_Master_ASCII_ENABLED > 0
	Response_ASCII::Response_ASCII()
	{

	}

	Response_ASCII::~Response_ASCII()
	{

	}
#endif // ARC_Modbus_Master_ASCII_ENABLED

#if ARC_Modbus_Master_TCP_ENABLED > 0
	Response_TCP::Response_TCP()
	{

	}

	Response_TCP::~Response_TCP()
	{

	}

	int Response_TCP::CheckException(ARC_Modbus::pkg iPackeg, unsigned char iID)
	{
		if (iPackeg.size == 9) // TCP例外之Response長度都是9
		{
			char* pkg_ptr = iPackeg.datas;

			tu_convert16 cTool;
			cTool.cValue[1] = pkg_ptr[INDEX_MESSAGE_LENGTH];
			cTool.cValue[0] = pkg_ptr[INDEX_MESSAGE_LENGTH + 1];

			if (cTool.wValue == iPackeg.size - 6) // 確認長度訊息是否正確
			{
				if (pkg_ptr[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] == iID) // 回應ID正確
				{
					if ((pkg_ptr[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE] & 0x80) == 0x80)
					{//是例外回應
						return pkg_ptr[LENGTH_MBAP_HEADER + INDEX_EXCEPTION_CODE]; //這是個例外回應 回傳例外碼
					}//end of if ((pkg_ptr[1] & 0x80 == 0x80))
				}//end of if(pkg_ptr[0] == iID)
			}
		}

		return 0;
	}

	bool Response_TCP::CheckWrite(ARC_Modbus::pkg iPackeg, unsigned char iID, unsigned short iAddress)
	{
		if (iPackeg.size == 12) // TCP寫入的Response長度都是12
		{
			char* pkg_ptr = iPackeg.datas;
			tu_convert16 cTool;
			cTool.cValue[1] = pkg_ptr[INDEX_MESSAGE_LENGTH];
			cTool.cValue[0] = pkg_ptr[INDEX_MESSAGE_LENGTH + 1];
			if (cTool.wValue == iPackeg.size - 6) // 確認長度訊息是否正確
			{
				if (pkg_ptr[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] == iID) //回應ID正確
				{
					int fc = pkg_ptr[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE];
					if (fc == 5 || fc == 6 || fc == 15 || fc == 16) // FC確認是寫入的
					{ 
						cTool.cValue[1] = pkg_ptr[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS];
						cTool.cValue[0] = pkg_ptr[LENGTH_MBAP_HEADER + INDEX_DATA_ADDRESS + 1];
						if (iAddress == cTool.wValue) // 回應的address也正確
						{
							return true;
						}
					}
				} // end of if (pkg_ptr[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] == iID) //回應ID正確
			} // end of if (cTool.wValue == iPackeg.size - 6) // 確認長度訊息是否正確
		} //end of if (iPackeg.size == 12) // TCP寫入的Response長度都是12
		return false;
	}

	bool Response_TCP::CheckRead(ARC_Modbus::pkg iPackeg, unsigned char iID, unsigned short iNumber)
	{
		char* pkg_ptr = iPackeg.datas;
		tu_convert16 cTool;
		cTool.cValue[1] = pkg_ptr[INDEX_MESSAGE_LENGTH];
		cTool.cValue[0] = pkg_ptr[INDEX_MESSAGE_LENGTH + 1];
		if (cTool.wValue == iPackeg.size - 6) // 確認長度訊息是否正確
		{
			if (pkg_ptr[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] == iID) //回應ID正確
			{
				int fc = pkg_ptr[LENGTH_MBAP_HEADER + INDEX_FUNCTION_CODE];
				int byte_follow = pkg_ptr[LENGTH_MBAP_HEADER + INDEX_NUMBER_BYTES_FOLLOW_r];
				int check_len = this->ReponseLength(fc, iNumber);
				if ( (byte_follow == check_len - 9) && (check_len == iPackeg.size) ) // 確認總長度
				{
					return true;
				}
			} // end of if (pkg_ptr[LENGTH_MBAP_HEADER + INDEX_SLAVE_ID] == iID) //回應ID正確
		}// end of if (cTool.wValue == iPackeg.size - 6) // 確認長度訊息是否正確
		return false;
	}

	bool Response_TCP::GetCoil(ARC_Modbus::pkg iPackeg, int iIndex)
	{
		char *contents = &iPackeg.datas[LENGTH_MBAP_HEADER + INDEX_DATA_START_r];
		return this->get_bit(contents, iIndex);
	}

	short Response_TCP::GetReg(ARC_Modbus::pkg iPackeg, int iIndex)
	{
		char *contents = &iPackeg.datas[LENGTH_MBAP_HEADER + INDEX_DATA_START_r];
		int idx = iIndex * 2;
		tu_convert16 cTool;
		cTool.cValue[1] = contents[idx];
		cTool.cValue[0] = contents[idx+1];

		return cTool.wValue;
	}

	int Response_TCP::ReponseLength(int iFunctionCode, unsigned short iNumber)
	{
		if (iFunctionCode == 5 ||
			iFunctionCode == 6 ||
			iFunctionCode == 15 ||
			iFunctionCode == 16)
		{
			//寫入命令的回應長度都是 12
			return 12;
		}
		else if ((iFunctionCode == 1 || iFunctionCode == 2) && (0 < iNumber && iNumber < 2000))
		{
			int byte_follow = iNumber / 8 + (iNumber % 8 != 0);
			return byte_follow + 9;
		}
		else if ((iFunctionCode == 3 || iFunctionCode == 4) && (0 < iNumber && iNumber < 2000))
		{
			return iNumber * 2 + 9;
		}
		return 0;
	}
#endif // ARC_Modbus_Master_TCP_ENABLED

}
