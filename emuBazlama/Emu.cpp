#include "pch.h"

#include "Emu.h"

namespace baz
{
	Emu::Emu(std::string path)
	{
		for (size_t i = 0; i < RAM_Size; i++)
		{
			m_ram[i] = 0;
		}		

		fr.readFile(path, m_ram);
	}

	Emu::~Emu()
	{
	}

	void Emu::run()
	{		
		//source = readFile("emu.txt");
		//placeFiletoRAM();		

		while (m_ram[pc] != 0)
		{
			m_komut = m_ram[pc];			

			switch (m_komut)
			{
			case 0x01:
				op_LOAD();
				break;							
			}

			if ((m_komut & 0b0000'1111) == 0x02)
			{
				op_Arithmetic();
			}

			pc++;
		}

		for (size_t i = 0; i < 8; i++)
		{
			std::cout << "reg" << i << " : " << m_registerFile[i] << "\n";
		}
	}

	



	baz::RegisterPart Emu::getRegisterPart()
	{
		pc++;
		baz::RegisterPart regPart;

		switch ((m_ram[pc] & baz_maskUz) >> 6)
		{
		case 0:
			regPart.m_reguz = baz::REG_8;
			break;

		case 1:
			regPart.m_reguz = baz::REG_16;
			break;

		case 3:
			regPart.m_reguz = baz::REG_32;
			break;
		}
		regPart.m_rega =  (m_ram[pc] & baz_maskRx) >> 3;
		regPart.m_regb = m_ram[pc] & baz_maskRy;

		return regPart;
	}

	uint32_t Emu::getBytes(uint8_t uz)
	{		
		uint32_t retval = 0;
		uint32_t temp;
		
		if (uz == baz::UzTip::REG_8)
		{			
			pc++;
			retval = m_ram[pc];
			return retval;
		}

		for (size_t i = 0; i < uz; i++)
		{
			pc++;
			temp = m_ram[pc];
			retval = (retval << 8) | temp;
		}

		return retval;		
	}

	uint32_t Emu::getBytes(uint8_t uz, uint32_t adr)
	{
		uint32_t retval = 0;
		uint32_t temp;

		if (uz == baz::UzTip::REG_8)
		{
			retval = m_ram[adr];
			adr++;
			return retval;
		}

		for (size_t i = 0; i < uz; i++)
		{
			temp = m_ram[adr];
			adr++;
			retval = (retval << 8) | temp;
		}

		return retval;
	}

	void Emu::op_LOAD()
	{		
		baz::RegisterPart regPart = getRegisterPart();
		
		uint32_t value;

		switch (m_komut)
		{

		//load rx,sayi
		case baz::LOAD_rx_sayi:
			pc++;
			value = getBytes(regPart.m_reguz);
			m_registerFile[regPart.m_rega] = value;
			break;

		//load rx,@ry
		case baz::LOAD_rx_regadr:						
			m_registerFile[regPart.m_rega] = m_ram[regPart.m_regb];
			break;

		//load rx,@adr
		case baz::LOAD_rx_adr:
			pc++;
			
			//adr
			value = getBytes(baz::UzTip::REG_32);
			m_registerFile[regPart.m_rega] = getBytes(regPart.m_reguz, value);
			break;

		//load rx,@adr+ry
		case baz::LOAD_rx_adr_p_reg:
			pc++;

			//adr
			value = getBytes(baz::UzTip::REG_32);
			m_registerFile[regPart.m_rega] = getBytes(regPart.m_reguz, value + regPart.m_regb); 
			break;
		}
	}

	//TODO template
	void Emu::calculate(baz::OperationType type, uint32_t& reg, uint32_t value)
	{
		switch (type)
		{
		case baz::OperationType::Add:
			reg += value;			
			break;

		case baz::OperationType::Sub:
			reg -= value;
			break;

		case baz::OperationType::Mul:
			reg *= value;
			break;

		case baz::OperationType::Div:
			reg /= value;
			break;
		}
	}

	void Emu::op_Arithmetic()
	{	
		baz::RegisterPart regPart = getRegisterPart();

		uint32_t value = 0;
		baz::OperationType operationType = baz::OperationType::Add;

		switch (m_komut)
		{
			case baz::ADD_rx_sayi:
			case baz::ADD_rx_regadr:
			case baz::ADD_rx_adr:
			case baz::ADD_rx_ry:
				operationType = baz::OperationType::Add;
				break;

			case baz::SUB_rx_sayi:
			case baz::SUB_rx_regadr:
			case baz::SUB_rx_adr:
			case baz::SUB_rx_ry:
				operationType = baz::OperationType::Sub;
				break;

			case baz::MUL_rx_sayi:
			case baz::MUL_rx_regadr:
			case baz::MUL_rx_adr:
			case baz::MUL_rx_ry:
				operationType = baz::OperationType::Mul;
				break;

			case baz::DIV_rx_sayi:
			case baz::DIV_rx_regadr:
			case baz::DIV_rx_adr:
			case baz::DIV_rx_ry:
				operationType = baz::OperationType::Div;
				break;
		}

		switch (m_komut)
		{ 
			//add rx,sayi
		case baz::ADD_rx_sayi:
		case baz::SUB_rx_sayi:
		case baz::MUL_rx_sayi:
		case baz::DIV_rx_sayi:			

			value = getBytes(regPart.m_reguz);			
			break;

			//add rx,@ry
		case baz::ADD_rx_regadr:
		case baz::SUB_rx_regadr:
		case baz::MUL_rx_regadr:
		case baz::DIV_rx_regadr:		

			value = getBytes(regPart.m_reguz, m_registerFile[regPart.m_regb]);			
			break;

			//add rx,@adr
		case baz::ADD_rx_adr:
		case baz::SUB_rx_adr:
		case baz::MUL_rx_adr:
		case baz::DIV_rx_adr:

			//adr
			value = getBytes(baz::UzTip::REG_32);
			m_registerFile[regPart.m_rega] += m_ram[value];
			break;

			//add rx,ry
		case baz::ADD_rx_ry:
		case baz::SUB_rx_ry:
		case baz::MUL_rx_ry:
		case baz::DIV_rx_ry:			

			value = m_registerFile[regPart.m_regb];
			break;
		}

		calculate(operationType, m_registerFile[regPart.m_rega], value);
	}

	void Emu::op_STR()
	{
	}

	void Emu::op_MOV()
	{
	}

	void Emu::op_CALL()
	{
	}

	void Emu::op_RET()
	{
	}

	void Emu::op_IRET()
	{
	}

	void Emu::op_PUSH()
	{
	}

	void Emu::op_POP()
	{
	}

	void Emu::op_PUSHA()
	{
	}

	void Emu::op_POPA()
	{
	}

	void Emu::op_JMP()
	{
	}

	void Emu::op_JNE()
	{
	}

	void Emu::op_JE()
	{
	}

	void Emu::op_JG()
	{
	}

	void Emu::op_JL()
	{
	}

	void Emu::op_MWE()
	{
	}

	void Emu::op_MD()
	{
	}

	void Emu::op_AND()
	{
	}

	void Emu::op_OR()
	{
	}

	void Emu::op_NOT()
	{
	}

	void Emu::op_XOR()
	{
	}

	void Emu::op_SHL()
	{
	}

	void Emu::op_SHR()
	{
	}

	void Emu::op_CMP()
	{
	}
}