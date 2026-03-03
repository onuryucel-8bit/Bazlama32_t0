#include "pch.h"

#include "Emu.h"

namespace baz
{
	Emu::Emu(std::string path)
	{
		m_ram.resize(baz::MB * 1);
		//800*600*2 => grafik karti
		fr.readFile(path, &m_ram);

		m_komut = 0;
	}

	Emu::~Emu()
	{
	}

	void Emu::run()
	{		
		while (m_ram[pc] != 0)
		{
			m_komut = m_ram[pc];			
			
			if ((m_komut & 0b0000'1111) == 0x02)
			{
				op_Arithmetic();
			}
			else
			{
				switch (m_komut)
				{
				case baz::Komut::LOAD_rx_adr:
				case baz::Komut::LOAD_rx_adr_p_reg:
				case baz::Komut::LOAD_rx_regadr:
				case baz::Komut::LOAD_rx_sayi:
					op_LOAD();
					break;

				case baz::Komut::STR_adr_p_reg_ry:
				case baz::Komut::STR_adr_regadr:
				case baz::Komut::STR_adr_rx:
				case baz::Komut::STR_adr_sayi:
					op_STR();
					break;

				case baz::Komut::MOV:
					op_MOV();
					break;

				case baz::Komut::CALL_adr:
				case baz::Komut::CALL_regadr:
					op_CALL();
					break;

				case baz::Komut::RET:
					op_RET();
					break;

				case baz::Komut::PUSH_rx:
					op_PUSH();
					break;

				case baz::Komut::POP_rx:
					op_POP();
					break;

				case baz::Komut::SHL_rx_ry:
				case baz::Komut::SHL_rx_sayi:
				case baz::Komut::SHR_rx_ry:
				case baz::Komut::SHR_rx_sayi:
					op_Shift();
					break;

				case baz::Komut::CMP_rx_regadr:
				case baz::Komut::CMP_rx_ry:
				case baz::Komut::CMP_rx_sayi:
					op_CMP();
					break;

				case baz::Komut::JMP:
					op_JMP();
					break;

				case baz::Komut::JNE:
					op_JNE();
					break;

				}
			}			
			pc++;
		}

		for (size_t i = 0; i < 8; i++)
		{
			std::cout << "reg" << i << " : " << m_registerFile[i] << "\n";
		}
	}
	
	void Emu::op_LOAD()
	{		
		baz::RegisterPart regPart = getRegisterPart();
		
		uint32_t value;

		switch (m_komut)
		{

		//load rx,sayi
		case baz::Komut::LOAD_rx_sayi:

			value = getBytes(regPart.m_reguz);
			m_registerFile[regPart.m_rega] = value;
			break;

		//load rx,@ry
		case baz::Komut::LOAD_rx_regadr:
			m_registerFile[regPart.m_rega] = m_ram[regPart.m_regb];
			break;

		//load rx,@adr
		case baz::Komut::LOAD_rx_adr:
			//adr
			value = getBytes(baz::UzTip::REG_32);
			m_registerFile[regPart.m_rega] = getBytes(regPart.m_reguz, value);
			break;

		//load rx,@adr+ry
		case baz::Komut::LOAD_rx_adr_p_reg:
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
			case baz::Komut::ADD_rx_sayi:
			case baz::Komut::ADD_rx_regadr:
			case baz::Komut::ADD_rx_adr:
			case baz::Komut::ADD_rx_ry:
				operationType = baz::OperationType::Add;
				break;

			case baz::Komut::SUB_rx_sayi:
			case baz::Komut::SUB_rx_regadr:
			case baz::Komut::SUB_rx_adr:
			case baz::Komut::SUB_rx_ry:
				operationType = baz::OperationType::Sub;
				break;

			case baz::Komut::MUL_rx_sayi:
			case baz::Komut::MUL_rx_regadr:
			case baz::Komut::MUL_rx_adr:
			case baz::Komut::MUL_rx_ry:
				operationType = baz::OperationType::Mul;
				break;

			case baz::Komut::DIV_rx_sayi:
			case baz::Komut::DIV_rx_regadr:
			case baz::Komut::DIV_rx_adr:
			case baz::Komut::DIV_rx_ry:
				operationType = baz::OperationType::Div;
				break;
		}

		switch (m_komut)
		{ 
			//add rx,sayi
		case baz::Komut::ADD_rx_sayi:
		case baz::Komut::SUB_rx_sayi:
		case baz::Komut::MUL_rx_sayi:
		case baz::Komut::DIV_rx_sayi:

			value = getBytes(regPart.m_reguz);			
			break;

			//add rx,@ry
		case baz::Komut::ADD_rx_regadr:
		case baz::Komut::SUB_rx_regadr:
		case baz::Komut::MUL_rx_regadr:
		case baz::Komut::DIV_rx_regadr:

			value = getBytes(regPart.m_reguz, m_registerFile[regPart.m_regb]);			
			break;

			//add rx,@adr
		case baz::Komut::ADD_rx_adr:
		case baz::Komut::SUB_rx_adr:
		case baz::Komut::MUL_rx_adr:
		case baz::Komut::DIV_rx_adr:

			//adr
			value = getBytes(baz::UzTip::REG_32);
			m_registerFile[regPart.m_rega] += m_ram[value];
			break;

			//add rx,ry
		case baz::Komut::ADD_rx_ry:
		case baz::Komut::SUB_rx_ry:
		case baz::Komut::MUL_rx_ry:
		case baz::Komut::DIV_rx_ry:

			value = m_registerFile[regPart.m_regb];
			break;
		}

		calculate(operationType, m_registerFile[regPart.m_rega], value);
	}

	void Emu::op_STR()
	{
		uint32_t adr = 0;
					
		switch (m_komut)
		{
			//str @adr, sayi
		case baz::Komut::STR_adr_sayi:
		{
			adr = getBytes(baz::UzTip::REG_32);
			uint32_t value = getBytes(baz::UzTip::REG_32);

			//32bit => ram			
			storeBytesToRam(value, adr, baz::UzTip::REG_32);
			break;
		}
		default:
			baz::RegisterPart regPart = getRegisterPart();

			switch (m_komut)
			{
			case baz::Komut::STR_adr_regadr:
			{
				adr = getBytes(baz::UzTip::REG_32);

				uint32_t mdr = getBytes(regPart.m_reguz, m_registerFile[regPart.m_rega]);

				storeBytesToRam(mdr, adr, regPart.m_reguz);	

				break;
			}			
			case baz::Komut::STR_adr_rx:

				adr = getBytes(baz::UzTip::REG_32);

				storeBytesToRam(m_registerFile[regPart.m_rega], adr, regPart.m_reguz);
				break;

			case baz::Komut::STR_adr_p_reg_ry:

				adr = getBytes(baz::UzTip::REG_32);

				storeBytesToRam(m_registerFile[regPart.m_regb], adr + m_registerFile[regPart.m_rega], regPart.m_reguz);
				break;
			}
			break;
		}
	}

	void Emu::op_MOV()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[regPart.m_rega] = m_registerFile[regPart.m_regb];
	}

	void Emu::op_CALL()
	{
		switch (m_komut)
		{
			case baz::CALL_adr:
			{
				uint32_t adr = getBytes(baz::UzTip::REG_32);
				m_ram[m_registerFile[baz::RegName::Sp]] = pc;
			
				m_registerFile[baz::RegName::Sp]++;

				pc = adr;

				break;
			}
			case baz::CALL_regadr:
			{
				baz::RegisterPart regPart = getRegisterPart();

				m_ram[m_registerFile[baz::RegName::Sp]] = pc;

				m_registerFile[baz::RegName::Sp]++;

				pc = m_registerFile[regPart.m_rega];
				break;
			}
		}
	}

	void Emu::op_RET()
	{
		m_registerFile[baz::RegName::Sp]--;
		
		pc = m_ram[m_registerFile[baz::RegName::Sp]];
	}

	void Emu::op_IRET()
	{
	}

	void Emu::op_PUSH()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_ram[m_registerFile[baz::RegName::Sp]] = m_registerFile[regPart.m_rega];

		m_registerFile[baz::RegName::Sp]++;
	}

	void Emu::op_POP()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[baz::RegName::Sp]--;

		m_registerFile[regPart.m_rega] = m_ram[m_registerFile[baz::RegName::Sp]];		
	}

	void Emu::op_PUSHA()
	{
		//r0,r1,r2,r3,r4,r5,r6 => ram
		for (size_t i = 0; i < 7; i++)
		{
			m_ram[m_registerFile[baz::RegName::Sp]] = m_registerFile[i];
			m_registerFile[baz::RegName::Sp]++;
		}
		
	}

	void Emu::op_POPA()
	{
		for (int i = 6; i >= 0; i--)
		{
			m_registerFile[baz::RegName::Sp]--;			
			m_registerFile[i] = m_ram[m_registerFile[baz::RegName::Sp]];
		}
	}

	void Emu::op_JMP()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		pc = adr - 1;
	}

	void Emu::op_JNE()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_eqeq) == baz_false)
		{
			pc = adr - 1;
		}
	}

	void Emu::op_JE()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_eqeq) == baz_true)
		{
			pc = adr - 1;
		}
	}

	void Emu::op_JG()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_greater) == baz_true)
		{
			pc = adr - 1;
		}
	}

	void Emu::op_JL()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_less) == baz_true)
		{
			pc = adr - 1;
		}
	}

	void Emu::op_MWE()
	{
	}

	void Emu::op_MD()
	{
	}

	void Emu::op_AND()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[regPart.m_rega] &= m_registerFile[regPart.m_regb];
	}

	void Emu::op_OR()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[regPart.m_rega] |= m_registerFile[regPart.m_regb];
	}

	void Emu::op_NOT()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[regPart.m_rega] = ~m_registerFile[regPart.m_rega];
	}

	void Emu::op_XOR()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[regPart.m_rega] ^= m_registerFile[regPart.m_rega];
	}

	void Emu::op_Shift()
	{
		baz::RegisterPart regPart = getRegisterPart();

		uint32_t value;

		switch (m_komut)
		{
		case baz::Komut::SHL_rx_ry:
			m_registerFile[regPart.m_rega] <<= m_registerFile[regPart.m_regb];
			break;

		case baz::Komut::SHL_rx_sayi:
			value = getBytes(regPart.m_reguz);

			m_registerFile[regPart.m_rega] <<= value;
			break;

		case baz::Komut::SHR_rx_ry:
			m_registerFile[regPart.m_rega] >>= m_registerFile[regPart.m_regb];
			break;

		case baz::Komut::SHR_rx_sayi:
			value = getBytes(regPart.m_reguz);

			m_registerFile[regPart.m_rega] >>= value;
			break;
		}
	}
	
	void Emu::op_CMP()
	{
		baz::RegisterPart regPart = getRegisterPart();

		switch (m_komut)
		{
			//TODO uztip
		case baz::Komut::CMP_rx_ry:
			if (m_registerFile[regPart.m_rega] == m_registerFile[regPart.m_regb])
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Eqeq;
			}
			if(m_registerFile[regPart.m_rega] < m_registerFile[regPart.m_regb])
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Less;
			}
			if (m_registerFile[regPart.m_rega] > m_registerFile[regPart.m_regb])
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Greater;
			}

			break;

		case baz::Komut::CMP_rx_regadr:
		{
			uint32_t adr = m_registerFile[regPart.m_regb];

			uint32_t value = getBytes(baz::UzTip::REG_32, adr);

			if (m_registerFile[regPart.m_rega] == value)
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Eqeq;
			}
			if (m_registerFile[regPart.m_rega] < value)
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Less;
			}
			if (m_registerFile[regPart.m_rega] > value)
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Greater;
			}
			break;
		}			
		case baz::Komut::CMP_rx_sayi:			
			uint32_t value = getBytes(regPart.m_reguz);

			if (m_registerFile[regPart.m_rega] == value)
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Eqeq;
			}
			if (m_registerFile[regPart.m_rega] < value)
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Less;
			}
			if (m_registerFile[regPart.m_rega] > value)
			{
				m_registerFile[baz::RegName::Flag] = baz::FlagReg::Greater;
			}
			break;
		}
	}

	//============================================================================================================//
	//============================================================================================================//
	//============================================================================================================//

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
		regPart.m_rega = (m_ram[pc] & baz_maskRx) >> 3;
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

	void Emu::storeBytesToRam(uint32_t data, uint32_t adr, baz::UzTip uz)
	{
		int i = 0;

		switch (uz)
		{
		case baz::UzTip::REG_32:
			i = 3;
			break;

		case baz::UzTip::REG_16:
			i = 1;
			break;

		case baz::UzTip::REG_8:
			i = 0;
			break;
		}
		

		for (; i >= 0; i--)
		{
			uint32_t mask = 0xff00'0000 >> ((3 - i) * 8);

			m_ram[adr] = (data & mask) >> 8 * i;

			//std::cout << std::hex << ((data & mask) >> 8 * i) << "\n";
			//std::cout << adr << "|" << (int)m_ram[adr] << "\n";
			adr++;
		}
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

}