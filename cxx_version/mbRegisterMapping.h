#ifndef mbRegisterMapping_h__
#define mbRegisterMapping_h__

// 定義暫存器mapping的地方
	class Register
	{
	public:
		/* ----------------------------------------*/
		// 有關暫存器位置的定義
		/* ----------------------------------------*/
		// 線圈(bit)類訊息 
		// 使用Modbus Function 01(0x) R/W
		static const int StartAddr_Coile = 0;
		static const int RegNumber_Coile = 100;

		// 使用Modbus Function 02(1x) Read only
		static const int StartAddr_ReadOnlyCoile = 0;
		static const int RegNumber_ReadOnlyCoile = 200;
		/* ----------------------------------------*/
		// 數值類暫存器訊息
		// 使用Modbus Function 03(4x) R/W
		static const int StartAddr_Register = 0;
		static const int RegNumber_Register = 50;

		// 使用Modbus Function 04(3x) Read only
		static const int StartAddr_ReadOnlyReg = 0;
		static const int RegNumber_ReadOnlyReg = 60;
		/* ----------------------------------------*/
		Register();
		~Register();
		void Initial( void );
		/* ----------------------------------------*/
		typedef enum {
			RegType_Coile = 0,		// _Coile
			RegType_InputCoile,		// _ReadOnlyCoile
			RegType_Register,		// _Register
			RegType_InputRegs	// _ReadOnlyReg
		}RegType;
		/* ----------------------------------------*/
		bool	GetRegsBuffer(RegType iRegType, unsigned short iAddress, unsigned short iNumber, char *regBuffer);
		bool	SetRegsBuffer(RegType iRegType, unsigned short iAddress, unsigned short iNumber, char *regBuffer);
		/* ----------------------------------------*/
		
	private:
		char	_Coile[RegNumber_Coile / 8 + 1];					// 0x (8 Coile 組合成一個char)
		char	_ReadOnlyCoile[RegNumber_ReadOnlyCoile / 8 + 1];	// 1x (8 Coile 組合成一個char)
		short	_Register[RegNumber_Register];						// 4x
		short	_ReadOnlyReg[RegNumber_ReadOnlyReg];				// 3x

		bool	check_access_region(unsigned short iAddress, RegType iRegType, unsigned short iNumber);

		void	set_bit(char* iPtr, unsigned short iStartAddr, unsigned short iIndex, bool iEnable);
		bool	get_bit(char* iPtr, unsigned short iStartAddr, unsigned short iIndex);

		typedef union
		{
			short wValue[2];
			float fValue;
			int	dwValue;
		}tu_convert32;

		typedef union
		{
			short wValue;
			char cValue[2];
		}tu_convert16;
	};


#endif // mbRegisterMapping_h__
