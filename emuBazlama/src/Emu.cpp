#include "pch.h"

#include "Emu.h"

namespace baz
{
	Emu::Emu(std::string path)
	{
		m_ram.resize(baz::MB * 50);
		//800*600*2 => grafik karti
		fr.readFile(path, &m_ram);

		m_komut = 0;

		try
		{
			m_logger = spdlog::basic_logger_mt("basic_logger", cmake_PROJECT_LOGS "basic-log.txt");
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cout << "Log init failed: " << ex.what() << std::endl;
		}

		m_registerFile[baz::RegName::Sp] = baz_StackBaseADR;

#ifdef PRODUCTION_BUILD
		spdlog::set_level(spdlog::level::debug);
#endif // PRODUCTION_BUILD

	}

	Emu::~Emu()
	{
	}

	void Emu::run()
	{				
		while (m_ram[pc] != baz::Komut::HLT)
		{			
		
 			m_komut = m_ram[pc];

			if (m_komut == baz::Komut::FTOI_rx)
			{
				//std::cout << "!\n";
			}

			

#ifdef _DEBUG			
			auto enmKmt = magic_enum::enum_cast<baz::Komut>(m_komut);
			if (enmKmt.has_value())
			{								
				spdlog::info("komut : {}", magic_enum::enum_name(*enmKmt));
			}				
			/*
			std::stringstream ss;
			ss << "DEBUG :: RET adr: " << std::hex << m_registerFile[baz::RegName::Sp];
			spdlog::info(ss.str());
			*/
			//m_logger.get()->info(ss.str());
#endif // PRODUCTION_BUILD

			//CMP_rx_ry
			//if (pc == 0x95)
			//JL
			//if (pc == 0xa2)
			/*if (pc == 0x24)
			{
				std::cout << "debug\n";
			}*/

			

		
			switch (m_komut)
			{
				case baz::ADD_rx_sayi:
				case baz::ADD_rx_regadr:
				case baz::ADD_rx_adr:
				case baz::ADD_rx_ry:
				case baz::SUB_rx_sayi:
				case baz::SUB_rx_regadr:
				case baz::SUB_rx_adr:
				case baz::SUB_rx_ry:
				case baz::MUL_rx_sayi:
				case baz::MUL_rx_regadr:
				case baz::MUL_rx_adr:
				case baz::MUL_rx_ry:
				case baz::DIV_rx_sayi:
				case baz::DIV_rx_regadr:
				case baz::DIV_rx_adr:
				case baz::DIV_rx_ry:
					op_Arithmetic();
					break;

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
				case baz::Komut::SAR_rx_ry:
				case baz::Komut::SAR_rx_sayi:
				case baz::Komut::SAL_rx_ry:
				case baz::Komut::SAL_rx_sayi:
					op_Shift();
					break;

				case baz::Komut::CMP_rx_regadr:
				case baz::Komut::CMP_rx_ry:
				case baz::Komut::CMP_rx_sayi:
				case baz::Komut::CMP_rx_adr:
					op_CMP();
					break;

				case baz::Komut::JMP:
					op_JMP();
					break;

				case baz::Komut::JNE:
					op_JNE();
					break;

				case baz::Komut::JL:
					op_JL();
					break;

				case baz::Komut::FTOI_rx:
					op_FTOI();
					break;

				case baz::Komut::ITOF_rx:
					op_ITOF();
					break;

				case baz::Komut::FADD_rx_adr:
				case baz::Komut::FADD_rx_regadr:
				case baz::Komut::FADD_rx_ry:
				case baz::Komut::FADD_rx_sayi:
				case baz::Komut::FDIV_rx_sayi:
				case baz::Komut::FDIV_rx_regadr:
				case baz::Komut::FDIV_rx_adr:
				case baz::Komut::FDIV_rx_ry:
					op_floatArithmetic();
					break;

				case baz::Komut::XOR_rx_ry:
					op_XOR();
					break;

				case baz::Komut::OR_rx_ry:
					op_OR();
					break;

				case baz::Komut::AND_rx_ry:
					op_AND();
					break;

				case baz::Komut::NOT_rx:
					op_NOT();
					break;
			}
					
  			pc++;
		}

		for (size_t i = 0; i < 8; i++)
		{
			std::cout << "reg" << i << " : " << std::hex << m_registerFile[i] << "\n"
				<< "reg! + 1 : " << (~m_registerFile[i]) + 1
				<< "\n=======================================\n";					  
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

	void Emu::calculateFloat(baz::OperationType type, uint32_t& reg, uint32_t value)
	{
		float rx = std::bit_cast<float>(reg);
		float val = std::bit_cast<float>(value);


		switch (type)
		{
		case baz::OperationType::Add:
			rx += val;
			break;

		case baz::OperationType::Sub:
			rx -= val;
			break;

		case baz::OperationType::Mul:
			rx *= val;
			break;

		case baz::OperationType::Div:
			if (val == 0)
			{
				rx = 0;
			}
			else
			{
				rx /= val;
			}
			break;
		}

		reg = rdx::decToIEEE754_32(rx);
	}

	void Emu::printStack()
	{
						
		uint32_t sp = m_registerFile[baz::RegName::Sp];

		
		std::cout << std::hex;
		std::cout << "-----------------------------\n";
		std::cout << "PC: " << pc << "\n";
		std::cout << "----------- STACK -----------\n";
				
		int i = baz_StackBaseADR;
		int j = 1;
		while (i > sp)
		{			
			std::cout << "sp: " << sp + j << " stack[" << (int)m_ram[sp + j] << "]\n";
			i--;
			j++;
		}

		std::cout << "-----------------------------\n";
		std::cout << std::dec;

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

		//std::cout << (int)m_registerFile[regPart.m_rega] << "\n";
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
			//TODO hata var burda STR adr, reg turevini STR_adr_regadr saniyor
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
			{
				adr = getBytes(baz::UzTip::REG_32);

				uint32_t dest = adr + m_registerFile[regPart.m_rega];

				storeBytesToRam(m_registerFile[regPart.m_regb], dest , regPart.m_reguz);

				/*std::cout << "---------------------------------------------\n";
				std::cout << std::hex << dest << "|" << (int)m_ram[dest] << "\n";
				std::cout << std::hex << dest + 1<< "|" << (int)m_ram[dest + 1] << "\n";
				std::cout << "---------------------------------------------\n";*/

				break;
			}

			}
			break;
		}
	}

	void Emu::op_MOV()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[regPart.m_rega] = m_registerFile[regPart.m_regb];
	}


	//--------------------------------------------------------------------------//
	//--------------------------------------------------------------------------//
	//--------------------------------------------------------------------------//


	void Emu::op_CALL()
	{
#ifdef _DEBUG
		std::stringstream ss;
		
		ss << "CALL : sp[" << std::hex << m_registerFile[baz::RegName::Sp] 
			<< "], sp + 1[" << m_registerFile[baz::RegName::Sp] + 1 << "] sp["
			<< (int)m_ram[m_registerFile[baz::RegName::Sp]] << "]";

		spdlog::debug(ss.str());
#endif //PRODUCTION_BUILD

		switch (m_komut)
		{
			case baz::CALL_adr:
			{
				uint32_t adr = getBytes(baz::UzTip::REG_32);
				storeBytesToStack(pc, baz::UzTip::REG_32);							

				pc = adr - 1;

				break;
			}
			case baz::CALL_regadr:
			{
				baz::RegisterPart regPart = getRegisterPart();
				storeBytesToStack(pc, baz::UzTip::REG_32);				

				pc = m_registerFile[regPart.m_rega] - 1;
				break;
			}
		}

#ifdef _DEBUG
		printStack();
#endif
	}

	void Emu::op_RET()
	{
#ifdef _DEBUG
		std::stringstream ss;

		ss << "RET  : sp[" << std::hex << m_registerFile[baz::RegName::Sp] 
			<< "], sp + 1[" << m_registerFile[baz::RegName::Sp] - 1 << "] sp-1["
			<< (int)m_ram[m_registerFile[baz::RegName::Sp]] << "]";

		spdlog::debug(ss.str());
#endif //PRODUCTION_BUILD		

		if (m_registerFile[baz::RegName::Sp] >= m_ram.size())
		{
			std::cout << "ERROR!!\n";
		}
		else
		{
			pc = getBytesFromStack(baz::UzTip::REG_32);			
		}		
#ifdef _DEBUG
		printStack();
#endif
	}

	void Emu::op_IRET()
	{
	}

	void Emu::op_PUSH()
	{

#ifdef _DEBUG
		std::stringstream ss;
		ss << "PUSH : sp[" << std::hex << m_registerFile[baz::RegName::Sp] 
			<< "], sp + 1[" << m_registerFile[baz::RegName::Sp] + 1 << "] sp + 1["
			<< (int)m_ram[m_registerFile[baz::RegName::Sp]] << "]";
		spdlog::debug(ss.str());
#endif //PRODUCTION_BUILD

		baz::RegisterPart regPart = getRegisterPart();

		//TODO uztip reg16,8
		storeBytesToStack(m_registerFile[regPart.m_rega], baz::UzTip::REG_32);				

#ifdef _DEBUG
		printStack();
#endif
	}

	void Emu::op_POP()
	{
#ifdef _DEBUG
		std::stringstream ss;
		ss << "POP  : sp[" << std::hex << m_registerFile[baz::RegName::Sp]
			<< "], sp + 1[" << m_registerFile[baz::RegName::Sp] - 1 << "] sp -1["
			<< (int)m_ram[m_registerFile[baz::RegName::Sp]] <<"]";
		spdlog::debug(ss.str());
#endif //PRODUCTION_BUILD

		baz::RegisterPart regPart = getRegisterPart();
				
		m_registerFile[regPart.m_rega] = getBytesFromStack(baz::UzTip::REG_32);;

#ifdef _DEBUG
		printStack();
#endif
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


	//--------------------------------------------------------------------------//
	//--------------------------------------------------------------------------//
	//--------------------------------------------------------------------------//



	void Emu::op_JMP()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		pc = adr - 1;
	}

	void Emu::op_JNE()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_eqeq) != baz::FlagReg::Eqeq)
		{
			pc = adr - 1;
		}
	}

	void Emu::op_JE()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_eqeq) == baz::FlagReg::Eqeq)
		{
			pc = adr - 1;
		}
	}

	void Emu::op_JG()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_greater) == baz::FlagReg::Greater)
		{
			pc = adr - 1;
		}
	}

	void Emu::op_JL()
	{
		uint32_t adr = getBytes(baz::UzTip::REG_32);

		if ((m_registerFile[baz::RegName::Flag] & baz_maskFlagReg_less) == baz::FlagReg::Less)
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

		m_registerFile[regPart.m_rega] = ~m_registerFile[regPart.m_regb];
	}

	void Emu::op_XOR()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[regPart.m_rega] ^= m_registerFile[regPart.m_regb];
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

		case baz::Komut::SAR_rx_ry:
			std::cout << "SAR_rx_ry !!!\n";
			break;

		//TODO SAR_rx_sayi bit
		case baz::Komut::SAR_rx_sayi:
		{
			uint32_t bit = m_registerFile[regPart.m_rega] & 0x8000'0000;

			value = getBytes(regPart.m_reguz);

			m_registerFile[regPart.m_rega] >>= value;

			m_registerFile[regPart.m_rega] |= bit;

			break;
		}
		case baz::Komut::SAL_rx_ry:
			std::cout << "SAL_rx_ry !!!\n";
			break;

		case baz::Komut::SAL_rx_sayi:
			std::cout << "SAL_rx_sayi !!!\n";
			break;
		}
	}
	
	void Emu::op_CMP()
	{
		baz::RegisterPart regPart = getRegisterPart();

		m_registerFile[baz::RegName::Flag] = 0;

		switch (m_komut)
		{
			//TODO uztip
		case baz::Komut::CMP_rx_ry:
			if (m_registerFile[regPart.m_rega] == m_registerFile[regPart.m_regb])
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Eqeq;
			}
			if(m_registerFile[regPart.m_rega] < m_registerFile[regPart.m_regb])
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Less;
			}
			if (m_registerFile[regPart.m_rega] > m_registerFile[regPart.m_regb])
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Greater;
			}

			break;

		case baz::Komut::CMP_rx_regadr:
		{
			uint32_t adr = m_registerFile[regPart.m_regb];

			uint32_t value = getBytes(baz::UzTip::REG_32, adr);

			if (m_registerFile[regPart.m_rega] == value)
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Eqeq;
			}
			if (m_registerFile[regPart.m_rega] < value)
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Less;
			}
			if (m_registerFile[regPart.m_rega] > value)
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Greater;
			}
			break;
		}			
		case baz::Komut::CMP_rx_sayi:			
			uint32_t value = getBytes(regPart.m_reguz);

			if (m_registerFile[regPart.m_rega] == value)
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Eqeq;
			}
			if (m_registerFile[regPart.m_rega] < value)
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Less;
			}
			if (m_registerFile[regPart.m_rega] > value)
			{
				m_registerFile[baz::RegName::Flag] |= baz::FlagReg::Greater;
			}
			break;
		}
	}

	void Emu::op_FTOI()
	{
		baz::RegisterPart regPart = getRegisterPart();

		float test = std::bit_cast<float>(m_registerFile[regPart.m_rega]);

		float intPart, dlt;

		dlt = std::modf(test, &intPart);

		int intPARRT = (int)intPart;
		

		uint32_t rx = rdx::IEEE754_toInt(m_registerFile[regPart.m_rega]);
								
		m_registerFile[regPart.m_rega] = rx;
	}

	void Emu::op_ITOF()
	{		
		baz::RegisterPart regPart = getRegisterPart();

		float rx = std::bit_cast<float>(m_registerFile[regPart.m_rega]);

		m_registerFile[regPart.m_rega] = rdx::decToIEEE754_32(m_registerFile[regPart.m_rega]);
	}

	void Emu::op_floatArithmetic()
	{
		baz::RegisterPart regPart = getRegisterPart();

		uint32_t value = 0;
		baz::OperationType operationType = baz::OperationType::Add;

		switch (m_komut)
		{
		case baz::Komut::FADD_rx_sayi:
		case baz::Komut::FADD_rx_regadr:
		case baz::Komut::FADD_rx_adr:
		case baz::Komut::FADD_rx_ry:

			operationType = baz::OperationType::Add;
			break;

		case baz::Komut::FSUB_rx_sayi:
		case baz::Komut::FSUB_rx_regadr:
		case baz::Komut::FSUB_rx_adr:
		case baz::Komut::FSUB_rx_ry:

			operationType = baz::OperationType::Sub;
			break;

		case baz::Komut::FMUL_rx_sayi:
		case baz::Komut::FMUL_rx_regadr:
		case baz::Komut::FMUL_rx_adr:
		case baz::Komut::FMUL_rx_ry:

			operationType = baz::OperationType::Mul;
			break;

		case baz::Komut::FDIV_rx_sayi:
		case baz::Komut::FDIV_rx_regadr:
		case baz::Komut::FDIV_rx_adr:
		case baz::Komut::FDIV_rx_ry:

			operationType = baz::OperationType::Div;
			break;
		}

		switch (m_komut)
		{
			//add rx,sayi
		case baz::Komut::FADD_rx_sayi:
		case baz::Komut::FSUB_rx_sayi:
		case baz::Komut::FMUL_rx_sayi:
		case baz::Komut::FDIV_rx_sayi:

			value = getBytes(regPart.m_reguz);
			break;

			//add rx,@ry
		case baz::Komut::FADD_rx_regadr:
		case baz::Komut::FSUB_rx_regadr:
		case baz::Komut::FMUL_rx_regadr:
		case baz::Komut::FDIV_rx_regadr:

			value = getBytes(regPart.m_reguz, m_registerFile[regPart.m_regb]);
			break;

			//add rx,@adr
		case baz::Komut::FADD_rx_adr:
		case baz::Komut::FSUB_rx_adr:
		case baz::Komut::FMUL_rx_adr:
		case baz::Komut::FDIV_rx_adr:

			//adr
			value = getBytes(baz::UzTip::REG_32);
			m_registerFile[regPart.m_rega] += m_ram[value];
			break;

			//add rx,ry
		case baz::Komut::FADD_rx_ry:
		case baz::Komut::FSUB_rx_ry:
		case baz::Komut::FMUL_rx_ry:
		case baz::Komut::FDIV_rx_ry:
			value = m_registerFile[regPart.m_regb];
			break;
		}

		calculateFloat(operationType, m_registerFile[regPart.m_rega], value);
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

	void Emu::storeBytesToStack(uint32_t data, baz::UzTip uz)
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

		uint32_t& adr = m_registerFile[baz::RegName::Sp];

		for (; i >= 0; i--)
		{
			uint32_t mask = 0x0000'00ff << ((uz - 1 - i) * 8);

			m_ram[adr] = (data & mask) >> 8 * (uz - 1 - i);

			//std::cout << std::hex << ((data & mask) >> 8 * i) << "\n";
			//std::cout << adr << "|" << (int)m_ram[adr] << "\n";

			adr--;
		}
	}

	uint32_t Emu::getBytesFromStack(uint8_t uz)
	{
		uint32_t retval = 0;
		uint32_t temp;

		uint32_t& adr = m_registerFile[baz::RegName::Sp];
		
		if (uz == baz::UzTip::REG_8)
		{
			adr++;
			retval = m_ram[adr];
			return retval;
		}

		for (size_t i = 0; i < uz; i++)
		{
			adr++;
			temp = m_ram[adr];
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


}