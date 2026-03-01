#include "pch.h"
#include "Parser.h"
namespace asmc
{

Parser::Parser(asmc::Lexer& lexer)
	:m_lexer(lexer)
{	
	m_tokenIndex = 0;
	m_tokenList = m_lexer.getTokenList();

	m_peekToken = m_tokenList[0];	

	//asmc::TokenElement* elem = m_lexer.getTokenList();

	m_ramLocation = 0;
	//TODO ????
	m_lineNumber = 1;
	m_errorCounter = 0;

	f_error = false;
	fd_printHexOutput = true;
	fd_scanTables = false;

#pragma region tables

	//29 used
	m_parserFuncs[asmc::TokenType::NOP] = &asmc::Parser::parseNOP;
	m_parserFuncs[asmc::TokenType::INCLUDE] = &asmc::Parser::parseINCLUDE;
	m_parserFuncs[asmc::TokenType::ORIGIN] = &asmc::Parser::parseORIGIN;
	m_parserFuncs[asmc::TokenType::DB] = &asmc::Parser::parseDB;
	m_parserFuncs[asmc::TokenType::DEFINE] = &asmc::Parser::parseDEFINE;

	//REG - RAM
	m_parserFuncs[asmc::TokenType::LOAD] = &asmc::Parser::parseLOAD;
	m_parserFuncs[asmc::TokenType::STR] = &asmc::Parser::parseSTR;
	m_parserFuncs[asmc::TokenType::MOV] = &asmc::Parser::parseMOV;

	//STACK
	m_parserFuncs[asmc::TokenType::CALL] = &asmc::Parser::parseCALL;
	m_parserFuncs[asmc::TokenType::RET]  = &asmc::Parser::parseRET;
	m_parserFuncs[asmc::TokenType::PUSH] = &asmc::Parser::parsePUSH;
	m_parserFuncs[asmc::TokenType::POP]  = &asmc::Parser::parsePOP;
	m_parserFuncs[asmc::TokenType::FUNC] = &asmc::Parser::parseFUNC;

	//IE	
	m_parserFuncs[asmc::TokenType::KWAIT] = &asmc::Parser::parseKWAIT;
	m_parserFuncs[asmc::TokenType::MWE] = &asmc::Parser::parseMWE;
	m_parserFuncs[asmc::TokenType::MR] = &asmc::Parser::parseMR;

	//ALU
	m_parserFuncs[asmc::TokenType::ADD] = &asmc::Parser::parseArithmeticPart;
	m_parserFuncs[asmc::TokenType::SUB] = &asmc::Parser::parseArithmeticPart;
	m_parserFuncs[asmc::TokenType::MUL] = &asmc::Parser::parseArithmeticPart;
	m_parserFuncs[asmc::TokenType::DIV] = &asmc::Parser::parseArithmeticPart;

	m_parserFuncs[asmc::TokenType::SHL] = &asmc::Parser::parseLogicPart;
	m_parserFuncs[asmc::TokenType::SHR] = &asmc::Parser::parseLogicPart;
	m_parserFuncs[asmc::TokenType::AND] = &asmc::Parser::parseLogicPart;
	m_parserFuncs[asmc::TokenType::OR]  = &asmc::Parser::parseLogicPart;
	m_parserFuncs[asmc::TokenType::XOR] = &asmc::Parser::parseLogicPart;

	m_parserFuncs[asmc::TokenType::NOT] = &asmc::Parser::parseNOT;
	m_parserFuncs[asmc::TokenType::CMP] = &asmc::Parser::parseArithmeticPart;

	//JUMP
	m_parserFuncs[asmc::TokenType::JMP] = &asmc::Parser::parseJMP;
	m_parserFuncs[asmc::TokenType::JNE] = &asmc::Parser::parseJMP;
	m_parserFuncs[asmc::TokenType::JE] = &asmc::Parser::parseJMP;
	m_parserFuncs[asmc::TokenType::JG] = &asmc::Parser::parseJMP;
	m_parserFuncs[asmc::TokenType::JL] = &asmc::Parser::parseJMP;	

	m_parserFuncs[asmc::TokenType::LABEL] = &asmc::Parser::parseLabel;

	//-------------------------------------------------//
	//-------------------------------------------------//
	//-------------------------------------------------//
	
	m_opcodeHexTable[asmc::TokenType::NOP] = 0x00;

	//REG-RAM
	m_opcodeHexTable[asmc::TokenType::LOAD] = 0x01;
	m_opcodeHexTable[asmc::TokenType::STR] =  0x10;		
	m_opcodeHexTable[asmc::TokenType::MOV] =  0x41;

	//STACK 
	m_opcodeHexTable[asmc::TokenType::CALL] = 0x04;
	m_opcodeHexTable[asmc::TokenType::RET] =  0xe4;
	m_opcodeHexTable[asmc::TokenType::PUSH] = 0x24;
	m_opcodeHexTable[asmc::TokenType::POP] =  0x34;

	//int	
	m_opcodeHexTable[asmc::TokenType::KWAIT] = 0x09;
	m_opcodeHexTable[asmc::TokenType::MWE] = 0x0A;
	m_opcodeHexTable[asmc::TokenType::MR] = 0x0B;

	//ALU
	m_opcodeHexTable[asmc::TokenType::ADD] = 0x02;
	m_opcodeHexTable[asmc::TokenType::SUB] = 0x42;
	m_opcodeHexTable[asmc::TokenType::MUL] = 0x82;
	m_opcodeHexTable[asmc::TokenType::DIV] = 0xc2;

	m_opcodeHexTable[asmc::TokenType::AND] = 0x43;
	m_opcodeHexTable[asmc::TokenType::OR] =  0x63;
	m_opcodeHexTable[asmc::TokenType::XOR] = 0x73;
	m_opcodeHexTable[asmc::TokenType::NOT] = 0x53;

	m_opcodeHexTable[asmc::TokenType::SHL] = 0x03;
	m_opcodeHexTable[asmc::TokenType::SHR] = 0x23;
			
	m_opcodeHexTable[asmc::TokenType::CMP] = 0x83;

	//JUMP
	m_opcodeHexTable[asmc::TokenType::JMP] = 0x50;
	m_opcodeHexTable[asmc::TokenType::JNE] = 0x60;
	m_opcodeHexTable[asmc::TokenType::JE] = 0x70;
	m_opcodeHexTable[asmc::TokenType::JG] = 0x80;
	m_opcodeHexTable[asmc::TokenType::JL] = 0x90;	
#pragma endregion

}

Parser::~Parser()
{
}

//-------------------------------------------------//
//-------------------------------------------------//
//-------------------------------------------------//

void Parser::run()
{	
	//--------------------------------------------------------------//
	//parser
	//--------------------------------------------------------------//
	uti::Timer timer;
	if (m_lexer.getDebugFlag())
	{
		std::cout << rang::bg::blue << "Running parser..." << rang::style::reset << "\n";
	}

	moveCurrentToken();
	

	while (m_currentToken.m_type != asmc::TokenType::ENDOFFILE)
	{							
		program();		

		if (m_peekToken.m_type == asmc::TokenType::ENDOFFILE)
		{
			//if input stream is empty close the loop
			if (m_lexer.popFile())
			{
				break;
			}
			else
			{							
				m_tokenList = m_lexer.getTokenList();

				m_peekToken = m_tokenList[0];
				m_tokenIndex = 0;
			}						
		}
		
		moveCurrentToken();		
	}
	
	//check for parser or lexer error
	if (f_error)
	{
		std::cout << rang::fg::red
			<< "Parser or lexer error detected\n"
			<< m_errorCounter 
			<< " Parser error\n"
			<< m_lexer.getErrorAmount()
			<< " Lexer error\n"			
			<< "Error found\n"
			<< rang::style::reset
			<< "\n";		
		return;
	}

	//--------------------------------------------------------------//
	//check tables / print opcode hex
	//--------------------------------------------------------------//


	checkTables();

	//----------DEBUG-----------------//
	if (m_lexer.getDebugFlag())
	{
		
		std::cout << rang::fg::blue
			<< "printing m_symbolTable"
			<< rang::style::reset
			<< "\n";

		for (const auto& [key, value] : m_symbolTable)
		{
			std::cerr << '[' << key.m_text << "] status [" << magic_enum::enum_name(value.m_status) << "]" << " address[" << value.m_ramIndex << "]\n";
		}
				
		std::cerr << rang::bg::green << "printBinHex() BEGIN"<< rang::style::reset << "\n";

		for (size_t i = 0; i < m_output.size(); i++)
		{
			std::cout << std::hex << "ram[" << m_output[i].m_ramIndex << "] | " << std::dec;
			printBinHex(m_output[i]);
		}



		std::cerr << rang::bg::green << "printBinHex() END" << rang::style::reset << "\n";
	}
	//----------------------------------//


	//if symbol info UNDEFINED
	if (f_error)
	{
		std::cout << rang::fg::red
			<< "Undefined symbol detected"
			<< rang::style::reset
			<< "\n";		
		return;
	}

	//--------------------------------------------------------------//
	//Generate output Emu/logisim RAM
	//--------------------------------------------------------------//

	//is file empty
	if (m_ramLocation != 0)
	{
		if (m_lexer.getDebugFlag())
		{
			std::cout << rang::bg::blue << "Generating output file..." << rang::style::reset << "\n";
		}
		writeOutput();
	}	

	m_disassembler.run(m_output);

	std::cerr <<"Total: " <<timer.getElapsedTime_milliseconds() << "ms passed..." << "\n";
}


void Parser::writeOutput()
{
	//sort std::vector
	std::sort(m_output.begin(), m_output.end(),
			  [](MemoryLayout& a, MemoryLayout& b)
			  {
				return a.m_ramIndex < b.m_ramIndex;
			  }
	);


	//--------------------------------------------------------------//
	//output EMU file
	//--------------------------------------------------------------//

	std::ofstream file("emuHex.txt");

	if (file.is_open())
	{		
		//write as hex
		file << std::hex;
		for (size_t i = 0; i < m_output.size(); i++)
		{
			//write opcode's ram location " " opcode
			size_t ramIndex = m_output[i].m_ramIndex;
			
			file << ramIndex << " " << (int)m_output[i].m_opcode << "\n";
			ramIndex++;
			
			if (m_output[i].m_regFlag == asmc::RegisterFlag::Reg)
			{
				file << ramIndex << " " << (int)m_output[i].m_regPart << "\n";
				ramIndex++;
			}
						
			for (size_t j = 0; j < m_output[i].m_packetSize; j++)
			{
				file << ramIndex << " " << (int)m_output[i].m_packet[j] << "\n";
				ramIndex++;
			}

			for (size_t j = 0; j < m_output[i].m_resPacketSize; j++)
			{
				file << ramIndex << " " << (int)m_output[i].m_reservedPacket[j] << "\n";
				ramIndex++;
			}
		}

		file.close();
	}
}

void Parser::checkTables()
{

	if (m_lexer.getDebugFlag())
	{
		std::cout << rang::bg::blue << "checkTables()...." << rang::style::reset << "\n";
	}

	for (const auto& [symKey, value] : m_symbolTable)
	{
		switch (value.m_status)
		{

			case asmc::LabelStatus::Undefined:
				printError("Undefined label/func");
			break;

			case asmc::LabelStatus::Called_NoRet:
				printError("Couldnt find RET");
			break;

			case asmc::LabelStatus::No_Call:
				printWarning("Func [" + symKey.m_text + "] was not called");
			break;
			
			case asmc::LabelStatus::No_Ret:
				printError("func(s) dont have RET");
			break;

			case asmc::LabelStatus::Called:
			case asmc::LabelStatus::NotUsed:
			case asmc::LabelStatus::Used:
				if (f_error == false || fd_scanTables == true)
				{
					//check jump table if label used
					for (const auto& [token, entry] : m_unresolvedTable)
					{
						if (token == symKey)
						{
							m_symbolTable[symKey].m_status = asmc::LabelStatus::Used;

							for (size_t i = 0; i < entry.size(); i++)
							{ 
								MemoryLayout memlay;

								memlay.m_opcode = entry[i].m_opcode;

								memlay.m_packetSize = asmc_WORD;
								memlay.m_packet = new uint8_t[asmc_WORD];
								memlay.m_ramIndex = entry[i].m_ramIndex;
								memlay.m_regFlag = asmc::RegisterFlag::NoReg;
							
								uint32_t funcAdr = m_symbolTable[symKey].m_ramIndex;
								for (size_t i = 0; i < 4; i++)
								{
									memlay.m_packet[i] = funcAdr & (0xff << (8 * i));
								}

								m_output.push_back(memlay);
							}							
						}
					}

					////for warning
					//if (value.m_status == asmc::LabelStatus::NotUsed)
					//{
					//	printWarning("Label not used[" + symKey + "]");
					//}
				}

				if (value.m_status == asmc::LabelStatus::NotUsed)
				{
					switch (symKey.m_type)
					{
					case asmc::TokenType::LABEL:
						printWarning("found not used label definition file[" + value.m_fileName + "] line number[" + std::to_string(value.m_lineNumber) + "]");
						break;
					case asmc::TokenType::FUNC:
						printWarning("found not used func definition file[" + value.m_fileName + "] line number[" + std::to_string(value.m_lineNumber) + "]");
						break;
					}
					
				}
			break;

		}//switch end
	}//FOR end
}

void Parser::program()
{
	
	if (m_currentToken.m_type > asmc::TokenType::LABEL || m_currentToken.m_type < 0)
	{
		printError("Undefined token");
	}
	else
	{
		(this->*m_parserFuncs[m_currentToken.m_type])();
	}


	switch (m_currentToken.m_type)
	{

	}

}


//-------------------------------------------------//
//------------------PRINT_X()----------------------//
//-------------------------------------------------//

#pragma region PRINT_X()

asmc::TokenType Parser::toToken(uint8_t opcode)
{	
		switch (opcode)
		{
			case 0x0:  
				return asmc::TokenType::NOP;
			// REG - RAM
			case 0x1:  
			case 0x11:
			case 0x21:
			case 0x31:
				return asmc::TokenType::LOAD;

			case 0x10:
			case 0x20:
			case 0x30:
			case 0x40:
				return asmc::TokenType::STR;

			case 0x02:
			case 0x12:
			case 0x22:
			case 0x32:
				return asmc::TokenType::ADD;

			//case 0x8:  return asmc::TokenType::MOV;

			//// STACK
			//case 0x3:  return asmc::TokenType::CALL;
			//case 0x4:  return asmc::TokenType::RET;
			//case 0x5:  return asmc::TokenType::PUSH;
			//case 0x6:  return asmc::TokenType::POP;
			//
			//case 0x9:  return asmc::TokenType::KWAIT;
			//case 0xa:  return asmc::TokenType::MWE;
			//case 0xb:  return asmc::TokenType::MR;

			//// ALU
			//case 0x10: return asmc::TokenType::ADD;
			//case 0x11: return asmc::TokenType::SUB;
			//case 0x12: return asmc::TokenType::MUL;
			//case 0x13: return asmc::TokenType::DIV;
			//case 0x19: return asmc::TokenType::SHL;
			//case 0x1a: return asmc::TokenType::SHR;

			//case 0x14: return asmc::TokenType::AND;
			//case 0x15: return asmc::TokenType::OR;
			//case 0x17: return asmc::TokenType::NOT;
			//case 0x16: return asmc::TokenType::XOR;

			//case 0x18: return asmc::TokenType::CMP;

			//// JUMP
			//case 0x1b: return asmc::TokenType::JMP;
			//case 0x1c: return asmc::TokenType::JNE;
			//case 0x1d: return asmc::TokenType::JE;
			//case 0x1e: return asmc::TokenType::JG;
			//case 0x1f: return asmc::TokenType::JL;			
		
			default: return asmc::TokenType::EMPTY;
		}
}

void Parser::printBinHex(asmc::MemoryLayout& memlay)
{		
	std::bitset<8> opcode = memlay.m_opcode;
	std::bitset<8> regPart = memlay.m_regPart;
	//TODO use log lib

	std::string str = opcode.to_string();
	std::string strRegPart = regPart.to_string();
	
	uint32_t uz = rdx::binToInt8(strRegPart.substr(0, 2));
	
	std::string uzreg;

	switch (uz)
	{
	case asmc::UzTip::REG_8:
		uzreg = "rs";
		break;

	case asmc::UzTip::REG_16:
		uzreg = "ra";
		break;

	case asmc::UzTip::REG_32:
		uzreg = "ro";
		break;
	}

	

	uint8_t command = rdx::binToDec(str);

	auto commandStr = magic_enum::enum_name(toToken(command));

	std::cout
		<< commandStr
		<< " uzreg[" << uzreg
		<< "] rx[" << (int)rdx::binToInt8(strRegPart.substr(2, 3))
		<< "] ry[" << (int)rdx::binToInt8(strRegPart.substr(5, 3))
		<< "]\n"
		<< "oooooooo uuxxxyyy\n"
		//opcode bits
		<< rang::fg::blue
		<< str << " "
		//reg bits
		<< rang::fg::yellow
		<< strRegPart << " ";

	std::cout 
		<< std::hex 
		<< " : " << opcode.to_ulong()
		<< " | " << regPart.to_ulong()
		<< std::dec
		<< "\n";

	std::cout << rang::fg::red << std::hex;
	for (size_t i = 0; i < memlay.m_packetSize; i++)
	{		
		std::cout << (int)memlay.m_packet[i] << "\n";		
	}
	for (size_t i = 0; i < memlay.m_resPacketSize; i++)
	{
		std::cout << (int)memlay.m_reservedPacket[i] << "\n";
	}

	std::cout << std::dec << rang::fgB::magenta<< "==========================\n";
	std::cout << rang::style::reset;
}

void Parser::printCurrentPeekToken()
{
	std::cout		
		<< "printCurrentPeekToken()\n"		
		<< "m_currentToken.text[" << m_currentToken.m_text << "]"
		<< "m_currentToken.type[" << magic_enum::enum_name(m_currentToken.m_type) << "]"
		<< "\n"
		<< "m_peekToken.text[" << m_peekToken.m_text << "]"
		<< "m_peekToken.type[" << magic_enum::enum_name(m_peekToken.m_type) << "]"		
		<< "\n";

}

void Parser::printError(std::string message)
{
	std::cout << rang::fg::red		
		<< "ERROR::Parser::" 
		<< "line number[" << m_currentToken.m_lineNumber << "]"
		<< message
		<< " |file name[" << m_lexer.getCurrentFileName() <<"]"
		<< " currentToken.text[" << m_currentToken.m_text << "]"
		<< " type[" << magic_enum::enum_name(m_currentToken.m_type) << "]"
		<< "\n-------"
		<< rang::style::reset
		<< "\n";

	f_error = true;
	m_errorCounter++;
}

void Parser::printWarning(std::string message)
{
	std::cout << rang::fg::yellow
		<< "WARNING::Parser:: " << message
		/*<< " line number [" << m_lexer.m_lineNumber << "]"
		<< " currentToken[" << m_currentToken.m_text << "]"
		<< " type	[" << magic_enum::enum_name(m_currentToken.m_type) << "]"*/
		<< rang::style::reset
		<< "\n";
}

#pragma endregion

//-------------------------------------------------//
//-------------------------------------------------//
//-------------------------------------------------//

void Parser::moveCurrentToken()
{
	m_currentToken = m_peekToken;

	m_tokenIndex++;
	if (m_tokenIndex >= m_tokenList.size())
	{
		m_tokenList = m_lexer.getTokenList();
		m_tokenIndex = 0;
	}

	m_peekToken = m_tokenList[m_tokenIndex];
}

asmc::MemoryLayout Parser::parseOperand(asmc::TokenType type)
{	
	asmc::MemoryLayout memlay;

	memlay.m_ramIndex = m_ramLocation;
	m_ramLocation++;

	moveCurrentToken();
	uint8_t rega, regb, opcode;

	asmc::UzTip regType = m_currentToken.m_regType;
	rega = rdx::hexToDec8(m_currentToken.m_text);
	m_ramLocation++;

	moveCurrentToken();
	switch (m_currentToken.m_type)
	{
	case asmc::TokenType::HEXNUMBER:
		memlay.m_opcode = asmc_CombineModBits(type, asmc_MOD_Number);

		//------/
		
		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, regType, rega, 0);
		//------/
		
		memlay.m_packetSize = regType + 1;
		memlay.m_packet = new uint8_t[regType + 1];

		convertToBytes(m_currentToken.m_text, regType, memlay.m_packet);

		break;

	case asmc::TokenType::REGISTER:
		memlay.m_opcode = asmc_CombineModBits(type, asmc_MOD_Rx_Ry);
		
		//TODO dynamic regtype add $rs1, $ra2 1byte + 2byte => r1
		//------/
		regb = rdx::hexToDec8(m_currentToken.m_text);
		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, asmc::UzTip::REG_32, rega, regb);
		//------/

		break;

	case asmc::TokenType::ADDRESS:
		memlay.m_opcode = asmc_CombineModBits(type, asmc_MOD_Adr);

		//TODO dynamic regtype add $rs1, $ra2 1byte + 2byte => r1
		//------/		
		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, asmc::UzTip::REG_32, rega, 0);
		m_ramLocation++;
		//------/

		memlay.m_packetSize = asmc::UzTip::REG_32 + 1;
		memlay.m_packet = new uint8_t[asmc::UzTip::REG_32 + 1];

		convertToBytes(m_currentToken.m_text, asmc::UzTip::REG_32, memlay.m_packet);

		break;

	case asmc::TokenType::REGADR:
		memlay.m_opcode = asmc_CombineModBits(type, asmc_MOD_RegAdr);

		//------/	
		regb = rdx::hexToDec8(m_currentToken.m_text);	
		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, asmc::UzTip::REG_32, rega, regb);
		m_ramLocation++;
		//------/

		break;
	}

	return memlay;
}

void Parser::convertToBytes(std::string& text, asmc::UzTip regtype, uint8_t* packet)
{	
	if (text.length() > 8)
	{
		printError("hex number out of range of 32bit value");
		return;
	}
	else if (text.length() < (regtype + 1) * 2)
	{
		printError("hex number MUST same length with register");
		return;
	}
	else if (text.length() > (regtype + 1) * 2)
	{
		printWarning("value is higher than register size lower bytes will be ignored");
	}

	int index = 0;
	for (size_t i = 0; i < (regtype + 1) * 2; i += 2)
	{
		std::string subStr = text.substr(i, 2);

		uint8_t retval = rdx::hexToDec8(subStr);

		packet[index] = retval;
		index++;
		m_ramLocation++;

	}	

}

PacketAdrPReg Parser::getAdr_P_RegPart(std::string& operand)
{
	//~~same thing as Lexer::getSubStr(int,int,int (*)(int))
	int index = 0;

	int startPos = 0;
	int length = 1;

	while (index + 1 < operand.length() && std::isxdigit(operand[index + 1]) )
	{
		index++;
		length++;
	}

	std::string addressPart = operand.substr(startPos, length);	
	

	startPos = index + 2;//cursor points the register number	
	length = 1;

	while (index + 1 < operand.length() && std::isdigit(operand[index + 1]))
	{
		index++;
		length++;
	}

	std::string regPart = operand.substr(startPos, length);

	PacketAdrPReg retVal;
	if (addressPart.length() <= 8)
	{
		retVal =
		{
			.m_adrPart = rdx::hexToDec(addressPart),
			.m_regPart = rdx::hexToDec(regPart)
		};
	}
	else
	{
		retVal = { 0,0 };
		printError("address exceeds 32bit limit");
	}
	

	return retVal;
}
 

//----------------MACRO-ASM----------------------//

#pragma region MACRO_ASM

void Parser::parseNOP()
{	
	asmc::MemoryLayout memlay;

	memlay.m_opcode = 0x0;
	memlay.m_packetSize = 1;
	memlay.m_ramIndex = m_ramLocation;

	m_ramLocation += 1;
	m_output.push_back(memlay);
}

void Parser::parseORIGIN()
{
	if (m_peekToken.m_type != asmc::TokenType::HEXNUMBER)
	{
		printError("Expected hex number after .origin");		
		return;
	}	
	
	moveCurrentToken();
	m_ramLocation = rdx::hexToDec(m_currentToken.m_text);	
}

void Parser::parseDB()
{
	if (m_peekToken.m_type != asmc::TokenType::HEXNUMBER &&
		m_peekToken.m_type != asmc::TokenType::STRING)
	{
		printError("Expected hex number or char after .db");
		return;
	}

	moveCurrentToken();

	while (m_currentToken.m_type == asmc::TokenType::HEXNUMBER ||
		m_currentToken.m_type == asmc::TokenType::STRING)
	{
		
		//TODO need optimization
		asmc::MemoryLayout memlay;
		
		//.db 0xff
		if (m_currentToken.m_type == asmc::TokenType::HEXNUMBER)
		{
			memlay.m_opcode = rdx::hexToDec(m_currentToken.m_text);
			memlay.m_regFlag = asmc::RegisterFlag::NoReg;
			memlay.m_ramIndex = m_ramLocation;

			m_ramLocation += 1;
			m_output.push_back(memlay);
		}
		else
		{

			for (size_t i = 0; i < m_currentToken.m_text.length(); i++)
			{
				uint8_t ch = (uint8_t)m_currentToken.m_text[i];
				memlay.m_opcode = ch;
				memlay.m_packetSize = 1;
				memlay.m_ramIndex = m_ramLocation;

				m_ramLocation += 1;
				m_output.push_back(memlay);
			}

			memlay.m_opcode = '\0';
			memlay.m_packetSize = 1;
			memlay.m_ramIndex = m_ramLocation;

			m_ramLocation += 1;
			m_output.push_back(memlay);

		}
		

		moveCurrentToken();

		if (m_currentToken.m_type == asmc::TokenType::NEWLINE)
		{
			moveCurrentToken();
		}
	}
}

void Parser::parseDEFINE()
{
	if (m_peekToken.m_type != asmc::TokenType::ID)
	{
		printError("Expected identifier after #define");
	}
	
	//m_currentToken => id
	moveCurrentToken();

	asmc::Token tempToken = m_currentToken;	

	//m_currentToken => decimal | hex
	moveCurrentToken();

	if (m_currentToken.m_type != asmc::TokenType::HEXNUMBER &&
		m_currentToken.m_type != asmc::TokenType::DECIMAL)
	{
		printError("macro value MUST be a hexnumber");
		return;
	}

	asmc::Symbol sym;

	// #define abc 0x54
	sym.m_fileName = m_lexer.getCurrentFileName();
	sym.m_lineNumber = m_lineNumber;

	
	switch (m_currentToken.m_type)
	{
	case asmc::TokenType::HEXNUMBER:
		sym.m_ramIndex = rdx::hexToDec(m_currentToken.m_text);
		break;

	case asmc::TokenType::DECIMAL:
		sym.m_ramIndex = std::stoi(m_currentToken.m_text);
		break;
	}

	m_symbolTable[tempToken] = sym;
	
}

void Parser::parseINCLUDE()
{
	if (m_peekToken.m_type != asmc::TokenType::DIRECTORY)
	{
		printError("Expected directory");
		return;
	}	

	
	m_lexer.pushFile(m_peekToken.m_text);

	//get new token list
	m_tokenList = m_lexer.getTokenList();
	m_peekToken = m_tokenList[0];
	m_tokenIndex = 0;
	m_currentToken = m_peekToken;	

	//currentToken points the token in newfile
	//called in run()
	//moveCurrentToken();  
}

#pragma endregion

//-------------REG/RAM---------------------//

#pragma region REG_RAM

void Parser::parseLOAD()
{
	if (m_peekToken.m_type != asmc::TokenType::REGISTER)
	{		
		printError("unexpected operand");
		return;
	}

	uint8_t opcode = m_opcodeHexTable[asmc::TokenType::LOAD];

	//next token rx
	moveCurrentToken();
	
	asmc::UzTip regType = m_currentToken.m_regType;
	uint8_t registerPart = rdx::hexToDec(m_currentToken.m_text);

	if (registerPart > 7)
	{
		printError("Register(s) must be in range [0-7]");
	}	

	//next token ry
	moveCurrentToken();

	asmc::MemoryLayout memlay;
	
	memlay.m_opcode = opcode;
	memlay.m_ramIndex = m_ramLocation;	
	m_ramLocation++;

	uint8_t rega, regb, reguz;

	switch (m_currentToken.m_type)
	{
		//load rx,sayi
		case asmc::TokenType::HEXNUMBER:			

			//----------//
			rega = registerPart;
			reguz = regType;
			m_ramLocation++;
			//----------//

			memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, 0);								
									
			memlay.m_packetSize = regType + 1;
			memlay.m_packet = new uint8_t[regType + 1];

			convertToBytes(m_currentToken.m_text, regType, memlay.m_packet);

		break;

		//load rx,@ry
		case asmc::TokenType::REGADR:
						
			memlay.m_opcode = asmc_CombineModBits(asmc::TokenType::LOAD, asmc_MOD_RegAdr);

			memlay.m_packetSize = 0;

			//----------//
			rega = registerPart;
			reguz = regType;
			regb = rdx::hexToDec8(m_currentToken.m_text);			
			m_ramLocation++;
			//----------//

			memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, regb);

		break;

		//load rx, @adr
		case asmc::TokenType::ADDRESS:

			memlay.m_opcode = asmc_CombineModBits(asmc::TokenType::LOAD, asmc_MOD_Adr);

			//4byte adr
			memlay.m_packetSize = asmc_WORD;
			memlay.m_packet = new uint8_t[asmc_WORD];	

			//----------//
			rega = registerPart;
			reguz = regType;
			m_ramLocation++;
			//----------//

			memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, 0);
			
			convertToBytes(m_currentToken.m_text, asmc::UzTip::REG_32, memlay.m_packet);

		break;
		
		//load rx, @adr + ry
		case asmc::TokenType::ADR_P_REG:
		{
			memlay.m_opcode = asmc_CombineModBits(asmc::TokenType::LOAD, asmc_MOD_Adr_P_Reg);

			memlay.m_packetSize = asmc_WORD;
			memlay.m_packet = new uint8_t[asmc_WORD];
								
			asmc::PacketAdrPReg padrpreg = getAdr_P_RegPart(m_currentToken.m_text);

			//----------//
			rega = registerPart;
			reguz = regType;
			regb = rdx::hexToDec8(m_currentToken.m_text);
			m_ramLocation++;
			//----------//

			memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, (uint8_t)padrpreg.m_regPart);	

			//TODO dokumana bak string_view substr() varmi?
			std::string adr = rdx::decToHex(padrpreg.m_adrPart);			
			
			//make the adr as 32 bit value str
			while (adr.length() < 8)
			{
				adr = "0" + adr;
			}

			convertToBytes(adr, asmc::UzTip::REG_32, memlay.m_packet);
		}
		break;

	}		
	
	m_output.push_back(memlay);
	/*if (m_currentToken.m_type == asmc::TokenType::REGISTER)
	{
		printError("unexpected register for second operand [LOAD rx, (..)!]");
	}
	else
	{
		memlay = parseOperand(opcode);

		m_output.push_back(memlay);
	}*/
	

}

void Parser::parseSTR()
{
	if (m_peekToken.m_type != asmc::TokenType::ADDRESS && 
		m_peekToken.m_type != asmc::TokenType::REGADR  &&
		m_peekToken.m_type != asmc::TokenType::ADR_P_REG)
	{
		printError("expected adr,regadr,adr_p_reg [STR (..)!, ...]");
	}

	
	uint8_t opcode = m_opcodeHexTable[asmc::TokenType::STR];

	/*
		STR @ff,r1
		STR @r2,r1
		STR @ff+r2,r1
	*/
	moveCurrentToken();//@ff,@r2,@ff+r2

	//uint8_t firstOperand = rdx::hexToDec(m_currentToken.m_text);

	asmc::Token adrPart = m_currentToken;	

	asmc::MemoryLayout memlay;
	memlay.m_opcode = opcode;
	memlay.m_ramIndex = m_ramLocation;
	m_ramLocation++;

	uint8_t reguz, rega, regb;

	//str @adr+rx, @ry
	if (adrPart.m_type == asmc::TokenType::ADR_P_REG)
	{
		
		opcode = asmc_CombineModBits(asmc::TokenType::STR, asmc_MOD_Adr_P_Reg);
		memlay.m_regFlag = asmc::RegisterFlag::NoReg;
		
		asmc::PacketAdrPReg padrpreg = getAdr_P_RegPart(m_currentToken.m_text);			

		moveCurrentToken();

		//----------//
		rega = (uint8_t)padrpreg.m_regPart;
		regb = rdx::hexToDec8(m_currentToken.m_text);
		//----------//
		m_ramLocation++;

		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, 0, rega, regb);

		memlay.m_packetSize = asmc_WORD;
		memlay.m_packet = new uint8_t[asmc_WORD];

		//TODO dokumana bak string_view substr() varmi?
		std::string adr = rdx::decToHex(padrpreg.m_adrPart);

		//make the adr as 32 bit value str
		while (adr.length() < 8)
		{
			adr = "0" + adr;
		}

		convertToBytes(adr, asmc::UzTip::REG_32, memlay.m_packet);

		//m_ramLocation += asmc_WORD * 2;
		
		
	}
	else
	{
		moveCurrentToken();

		switch (m_currentToken.m_type)
		{
			//TODO 1 bayt kullanilmiyor

		//str @adr,sayi
		case asmc::TokenType::HEXNUMBER:

			memlay.m_packetSize = asmc_WORD;
			memlay.m_packet = new uint8_t[asmc_WORD];
			memlay.m_regFlag = asmc::RegisterFlag::NoReg;

			//TODO dynamic sized hexvalue
			memlay.m_resPacketSize = asmc_WORD;
			memlay.m_reservedPacket = new uint8_t[asmc_WORD];

			//adr
			convertToBytes(adrPart.m_text, asmc::UzTip::REG_32, memlay.m_packet);
			//sayi
			convertToBytes(m_currentToken.m_text, asmc::UzTip::REG_32, memlay.m_reservedPacket);

			//m_ramLocation += asmc_WORD * 2;

			break;

		//str @adr,rx
		case asmc::TokenType::REGISTER:
		{
			opcode = asmc_CombineModBits(asmc::TokenType::STR, asmc_MOD_Rx);

			//----------//
			rega = rdx::hexToDec8(m_currentToken.m_text);
			reguz = m_currentToken.m_regType;
			//----------//
			m_ramLocation++;

			memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, 0);

		
			memlay.m_packetSize = asmc_WORD;
			memlay.m_packet = new uint8_t[asmc_WORD];

			convertToBytes(adrPart.m_text, asmc::UzTip::REG_32, memlay.m_packet);

			//m_ramLocation += asmc_WORD * 2;
		}
		break;

		//str @adr, @rx
		case asmc::TokenType::REGADR:
		{
			opcode = asmc_CombineModBits(asmc::TokenType::STR, asmc_MOD_RegAdr);


			//----------//
			rega = rdx::hexToDec8(m_currentToken.m_text);
			reguz = m_currentToken.m_regType;
			//----------//
			m_ramLocation++;

			memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, 0);						

			memlay.m_packetSize = asmc_WORD;
			memlay.m_packet = new uint8_t[asmc_WORD];

			convertToBytes(adrPart.m_text, asmc::UzTip::REG_32, memlay.m_packet);
			
		}
		break;
		}

	}
	//memlay.m_regPart = registerPart;
	memlay.m_opcode = opcode;
	
	m_output.push_back(memlay);
	
}

void Parser::parseMOV()
{
	if (m_peekToken.m_type != asmc::TokenType::REGISTER)
	{
		printError("expected register for first operand [MOV (rx)!, ry]");
		moveCurrentToken();//current token => rx
		moveCurrentToken();//current token => ry
		if (m_currentToken.m_type != asmc::TokenType::REGISTER)
		{
			printError("expected register for second operand [MOV rx, (ry)!]");
		}
		return;
	}

	asmc::MemoryLayout memlay;
	memlay.m_ramIndex = m_ramLocation;
	memlay.m_opcode = m_opcodeHexTable[asmc::TokenType::MOV];
	m_ramLocation++;

	uint8_t rx, ry;
	
	moveCurrentToken();
	
	rx = rdx::hexToDec(m_currentToken.m_text);
	
	//m_currentToken => ry
	moveCurrentToken();
	if (m_currentToken.m_type != asmc::TokenType::REGISTER)
	{
		printError("expected register for second operand [MOV rx, (ry)!]");		
	}
	else
	{		
		ry = rdx::hexToDec(m_currentToken.m_text);

		if (rx > 7 || ry > 7)
		{
			printError("Registers must in range [0-7]");
		}

		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, 0, rx, ry);
		m_ramLocation++;
	}
	
	
	
	m_output.push_back(memlay);	
}

#pragma endregion

//-----------------ALU---------------------//

#pragma region ALU

void Parser::parseArithmeticPart()
{
	if (m_peekToken.m_type != asmc::TokenType::REGISTER)
	{
		printError("expected register for second operand [ALU (rx)!, ...]");
		moveCurrentToken();
		moveCurrentToken();
		return;
	}
		
	asmc::MemoryLayout memlay = parseOperand(m_currentToken.m_type);
	
	m_output.push_back(memlay);
}

void Parser::parseLogicPart()
{
	if (m_peekToken.m_type != asmc::TokenType::REGISTER)
	{
		printError("Expected register as first operand");
	}
	asmc::TokenType tokentype = m_currentToken.m_type;
	moveCurrentToken();

	uint8_t rega = rdx::hexToDec(m_currentToken.m_text);
	uint8_t regb;
	asmc::UzTip regtype = m_currentToken.m_regType;

	asmc::MemoryLayout memlay;
	memlay.m_ramIndex = m_ramLocation;
	m_ramLocation++;

	moveCurrentToken();
	
	switch (m_currentToken.m_type)
	{
	case asmc::TokenType::HEXNUMBER:
		memlay.m_opcode = asmc_CombineModBits(tokentype, asmc_MOD_Number);
		

		//----------//		
		m_ramLocation++;
		//----------//

		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, regtype, rega, 0);

		memlay.m_packetSize = regtype + 1;
		memlay.m_packet = new uint8_t[regtype + 1];

		convertToBytes(m_currentToken.m_text, regtype, memlay.m_packet);

		break;

	case asmc::TokenType::REGISTER:
		if (tokentype == asmc::TokenType::SHL || tokentype == asmc::TokenType::SHR)
		{
			memlay.m_opcode = asmc_CombineModBits(tokentype, 1);
		}
		else
		{
			memlay.m_opcode = asmc_CombineModBits(tokentype, 0);
		}

		//----------//		
		regb = rdx::hexToDec8(m_currentToken.m_text);
		m_ramLocation++;
		//----------//

		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, regtype, rega, regb);
		break;

	}

	m_output.push_back(memlay);
}

void Parser::parseNOT()
{
	if (m_peekToken.m_type != asmc::TokenType::REGISTER &&
		m_peekToken.m_type != asmc::TokenType::ID)
	{
		printError("unexpected operand");
	}

	asmc::MemoryLayout memlay;

	memlay.m_opcode = asmc_CombineModBits(asmc::TokenType::NOT, 0);
	memlay.m_ramIndex = m_ramLocation;
	m_ramLocation++;

	moveCurrentToken();

	//-----------//
	uint8_t rega = rdx::hexToDec8(m_currentToken.m_text);
	memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, m_currentToken.m_regType, rega, 0);
	m_ramLocation++;
	//-----------//

	m_output.push_back(memlay);
}

#pragma endregion

//------------STACK--------------------//

#pragma region STACK

void Parser::parsePUSH()
{
	if (m_peekToken.m_type != asmc::TokenType::REGISTER)
	{
		printError("unexpected operand");
	}

	asmc::MemoryLayout memlay;

	memlay.m_opcode = m_opcodeHexTable[asmc::TokenType::PUSH];
	memlay.m_ramIndex = m_ramLocation;
	m_ramLocation++;

	moveCurrentToken();
	//------------//
	uint8_t reguz = m_currentToken.m_regType;
	uint8_t rega = rdx::hexToDec8(m_currentToken.m_text);
	memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, 0);
	m_ramLocation++;
	//------------//

	m_output.push_back(memlay);
}

void Parser::parsePOP()
{
	if (m_peekToken.m_type != asmc::TokenType::REGISTER)
	{
		printError("unexpected operand");
	}

	asmc::MemoryLayout memlay;
		
	memlay.m_opcode = m_opcodeHexTable[asmc::TokenType::POP];
	memlay.m_ramIndex = m_ramLocation;
	m_ramLocation++;

	moveCurrentToken();
	//------------//
	uint8_t reguz = m_currentToken.m_regType;
	uint8_t rega = rdx::hexToDec8(m_currentToken.m_text);
	memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, reguz, rega, 0);
	m_ramLocation++;
	//------------//

	

	m_output.push_back(memlay);
	
}

void Parser::parseKWAIT()
{
	uint32_t opcode = m_opcodeHexTable[asmc::TokenType::KWAIT];

	asmc::MemoryLayout memlay;

	memlay.m_opcode = opcode;
	memlay.m_packetSize = 1;
	memlay.m_ramIndex = m_ramLocation;

	m_output.push_back(memlay);

	m_ramLocation += 1;
}

void Parser::parseMWE()
{
	asmc::MemoryLayout memlay;

	memlay.m_opcode = m_opcodeHexTable[asmc::TokenType::MWE];
	memlay.m_packetSize = 1;
	memlay.m_ramIndex = m_ramLocation;

	m_ramLocation += 1;
	m_output.push_back(memlay);
}

void Parser::parseMR()
{
	asmc::MemoryLayout memlay;

	memlay.m_opcode = m_opcodeHexTable[asmc::TokenType::MR];
	memlay.m_packetSize = 1;
	memlay.m_ramIndex = m_ramLocation;

	m_ramLocation += 1;
	m_output.push_back(memlay);
}

void Parser::parseCALL()
{
	if (m_peekToken.m_type == asmc::TokenType::REGISTER)
	{
		printError("register used as address without @");
	}

	else if (m_peekToken.m_type != asmc::TokenType::ID &&
		m_peekToken.m_type != asmc::TokenType::REGADR &&
		m_peekToken.m_type != asmc::TokenType::ADDRESS)
	{
		printError("invalid CALL command ");
	}
	

	//uint8_t opcode = m_opcodeHexTable[asmc::TokenType::CALL];
	asmc::MemoryLayout memlay;	
	uint8_t rega;
	int lineNumber = m_currentToken.m_lineNumber;
	moveCurrentToken();

	switch (m_currentToken.m_type)	
	{
	case asmc::TokenType::ID:
		memlay.m_opcode = asmc_CombineModBits(asmc::TokenType::CALL, 1);
				
		//func is defined
		if (m_symbolTable.contains(m_currentToken) && m_symbolTable[m_currentToken].m_status != asmc::LabelStatus::No_FuncDef)
		{
			m_symbolTable[m_currentToken].m_status = asmc::LabelStatus::Called;			
			memlay.m_ramIndex = m_ramLocation;
			memlay.m_regFlag = asmc::RegisterFlag::NoReg;
			m_ramLocation++;

			memlay.m_packetSize = asmc::UzTip::REG_32 + 1;
			memlay.m_packet = new uint8_t[asmc::UzTip::REG_32 + 1];
			m_ramLocation += asmc_WORD;

			uint32_t funcAdr = m_symbolTable[m_currentToken].m_ramIndex;
			for (size_t i = 0; i < 4; i++)
			{
				memlay.m_packet[i] = funcAdr & (0xff << (8 * i));
			}

			m_output.push_back(memlay);
		}
		//func definition after the CALL command ?
		else
		{
			m_symbolTable[m_currentToken].m_status = asmc::LabelStatus::No_FuncDef;

			asmc::UnresolvedEntry entry;

			entry.m_opcode = memlay.m_opcode;
			entry.m_ramIndex = m_ramLocation;

		//	entry.m_secondPart = -1;
		//	entry.m_packetSize = 2;
			entry.m_fileName = m_lexer.getCurrentFileName();
			entry.m_lineNumber = lineNumber;
			entry.m_status = asmc::LabelStatus::No_Ret;

			m_ramLocation += asmc_WORD;

			m_unresolvedTable[m_currentToken].push_back(entry);
		}
		

		break;

	case asmc::TokenType::ADDRESS:
		memlay.m_opcode = asmc_CombineModBits(asmc::TokenType::CALL, 1);
		memlay.m_ramIndex = m_ramLocation;
		memlay.m_regFlag = asmc::RegisterFlag::NoReg;
		m_ramLocation++;

		memlay.m_packetSize = asmc::UzTip::REG_32 + 1;
		memlay.m_packet = new uint8_t[asmc::UzTip::REG_32 + 1];

		convertToBytes(m_currentToken.m_text, asmc::UzTip::REG_32, memlay.m_packet);

		m_output.push_back(memlay);
		break;

	case asmc::TokenType::REGADR:
		memlay.m_opcode = asmc_CombineModBits(asmc::TokenType::CALL, 0);
		memlay.m_ramIndex = m_ramLocation;
		m_ramLocation++;

		//----------//
		rega = rdx::hexToDec8(m_currentToken.m_text);
		memlay.m_regPart = asmc_CombineRegPart(memlay.m_regPart, asmc::UzTip::REG_32, rega, 0);
		m_ramLocation++;		
		//----------//

		
		m_output.push_back(memlay);
		break;
	}
}

void Parser::parseFUNC()
{	
	if (m_peekToken.m_type != asmc::TokenType::ID)
	{
		printError("FUNC must be followed by a function name");
	}

	moveCurrentToken();

	//FIX !! func def - call

	//is func definition in symbol table
	if (m_symbolTable.contains(m_currentToken))
	{
		//is func definition called?
		if (m_symbolTable[m_currentToken].m_status == asmc::LabelStatus::No_FuncDef)
		{
			m_symbolTable[m_currentToken].m_status = asmc::LabelStatus::Called_NoRet;

			//combine with CALL command
			for (const asmc::UnresolvedEntry& entry : m_unresolvedTable[m_currentToken])
			{
				asmc::MemoryLayout memlay;

				memlay.m_opcode = entry.m_opcode;
				memlay.m_ramIndex = entry.m_ramIndex;
				memlay.m_regFlag = asmc::RegisterFlag::NoReg;
				memlay.m_packetSize = asmc::UzTip::REG_32 + 1;
				memlay.m_packet = new uint8_t[asmc::UzTip::REG_32 + 1];

				m_ramLocation++;
				uint32_t funcAdr = m_ramLocation;
				for (size_t i = 0; i < 4; i++)
				{
					memlay.m_packet[i] = funcAdr & (0xff << (8 * i));
				}

				m_output.push_back(memlay);
			}
		}
		//else
			//print error func with same name twice or more
	}
	//Func called? later
	else
	{
		Symbol sym =
		{
			.m_ramIndex = m_ramLocation,
			.m_fileName = m_lexer.getCurrentFileName(),
			.m_lineNumber = m_lineNumber,
			.m_status = asmc::LabelStatus::No_Ret
		};

		m_symbolTable[m_currentToken] = sym;
	}
	//
	m_lastFuncName = { m_currentToken.m_text, m_currentToken.m_type};
	
}

void Parser::parseRET()
{
	if (m_lastFuncName.m_text == "")
	{		
		printError("FUNC definition required before using RET");
	}

	asmc::MemoryLayout memlay;

	memlay.m_opcode = m_opcodeHexTable[asmc::TokenType::RET];
	memlay.m_regFlag = asmc::RegisterFlag::NoReg;
	memlay.m_ramIndex = m_ramLocation;
	m_ramLocation++;

	if (m_symbolTable.contains(m_lastFuncName))
	{
		m_symbolTable[m_lastFuncName].m_status = asmc::LabelStatus::No_Call;
	}

	m_lastFuncName.m_text = "";
	
	m_output.push_back(memlay);
}

#pragma endregion

//----------------JUMP----------------//

void Parser::parseLabel()
{
	if (m_symbolTable.contains(m_currentToken))
	{		
		printError("label used twice");
	}
	else
	{
		m_symbolTable[m_currentToken] = { m_ramLocation, m_lexer.getCurrentFileName(), m_lineNumber, asmc::LabelStatus::NotUsed };
	}
	
	m_lineNumber++;
}

void Parser::parseJMP()
{
	if (m_peekToken.m_type != asmc::TokenType::LABEL)
	{
		printError("Expected label after jump instruction");
	}

	uint8_t opcode = 0;
	switch (m_currentToken.m_type)
	{
	case asmc::TokenType::JMP:
		opcode = m_opcodeHexTable[asmc::TokenType::JMP];
		break;

	case asmc::TokenType::JNE:
		opcode = m_opcodeHexTable[asmc::TokenType::JNE];
		break;

	case asmc::TokenType::JE:
		opcode = m_opcodeHexTable[asmc::TokenType::JE];
		break;

	case asmc::TokenType::JG:
		opcode = m_opcodeHexTable[asmc::TokenType::JG];
		break;

	case asmc::TokenType::JL:
		opcode = m_opcodeHexTable[asmc::TokenType::JL];
		break;

	}

	moveCurrentToken();

	if (m_symbolTable.contains(m_currentToken))
	{				
		m_symbolTable[m_currentToken].m_status = asmc::LabelStatus::Used;

		MemoryLayout memlay;

		memlay.m_opcode = opcode;
		memlay.m_packetSize = 2;
		memlay.m_ramIndex = m_ramLocation;
		m_ramLocation++;

		memlay.m_regFlag = asmc::RegisterFlag::NoReg;

		//----------------------//

		memlay.m_packetSize = asmc::UzTip::REG_32 + 1;
		memlay.m_packet = new uint8_t[asmc::UzTip::REG_32 + 1];
		m_ramLocation += asmc_WORD;

		uint32_t funcAdr = m_symbolTable[m_currentToken].m_ramIndex;
		for (size_t i = 0; i < 4; i++)
		{
			memlay.m_packet[i] = funcAdr & (0xff << (8 * i));
		}		

		m_output.push_back(memlay);
	}
	else
	{	
		asmc::UnresolvedEntry entry;

		entry.m_opcode = opcode;		
		entry.m_ramIndex = m_ramLocation;
		m_ramLocation++;
		
		entry.m_fileName = m_lexer.getCurrentFileName();
		entry.m_lineNumber = m_lineNumber;
		entry.m_status = asmc::LabelStatus::Undefined;

		m_ramLocation += asmc_WORD;

		m_unresolvedTable[m_currentToken].push_back(entry);
	}	
}

//------------------------------------//

}
