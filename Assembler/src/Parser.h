#pragma once

//TEST DEBUG
#include <bitset>
#include <fstream>

#include "utils/magic_enum/magic_enum.hpp"
#include "utils/rang.hpp"
#include "utils/Radix.h"

#include "Lexer.h"
#include "Disassembler.h"

#include "Common.h"

//TODO move to ..\utils
#include "Timer.h"

//MOD bits
//ozel bitler

//TODO enum ?
#define asmc_MOD_Number 0
//@ry
#define asmc_MOD_RegAdr 1
//rx => STR
#define asmc_MOD_Rx 2
//@adr
#define asmc_MOD_Adr 2
//@adr + ry
#define asmc_MOD_Adr_P_Reg 3
//rx, ry
#define asmc_MOD_Rx_Ry 3

//4byte => 32 bit
#define asmc_WORD 4

#define asmc_CombineModBits(opcode, modBits) \
	(m_opcodeHexTable[opcode]) + (modBits << 4)

#define asmc_CombineRegPart(regPart, reguz, rega, regb) \
	regPart | ((reguz << 6) | (rega << 3) | regb)									

namespace asmc
{	
	class Parser
	{
	public:

		Parser(asmc::Lexer& lexer);
		~Parser();

		void run();

	private:

		void program();
		void debug();

		void moveCurrentToken();
	
		void checkTables();

		void printError(std::string message);
		void printWarning(std::string message);

		void writeOutput();

		asmc::MemoryLayout parseOperand(asmc::TokenType type);				

		void convertToBytes(std::string& text, asmc::UzTip regtype, uint8_t* packet);

		PacketAdrPReg getAdr_P_RegPart(std::string& operand);

		asmc::TokenType toToken(uint8_t opcode);
		void printBinHex(asmc::MemoryLayout& memlay);
		void printCurrentPeekToken();

		//========================================================================================//
		//========================================================================================//
	
		int m_ramLocation;
		int m_lineNumber;

		asmc::Lexer& m_lexer;
		dasm::Disassembler m_disassembler;

		asmc::Token m_currentToken;
		asmc::Token m_peekToken;

		std::unordered_map<asmc::Token, asmc::Symbol> m_symbolTable;
		std::unordered_map<asmc::Token, std::vector<asmc::UnresolvedEntry>> m_unresolvedTable;

		std::unordered_map<asmc::TokenType, uint8_t> m_opcodeHexTable;

		std::vector<asmc::MemoryLayout> m_output;

		//8byte
		using funcPtr = void (asmc::Parser::*)();

		funcPtr m_parserFuncs[64];
		std::array<asmc::Token, asmc_MAX_TOKEN_LIST_SIZE> m_tokenList;
		size_t m_tokenIndex;

		asmc::Token m_lastFuncName;

		bool f_error;
		bool fd_printHexOutput;//flag debug fd
		bool fd_scanTables;
	
		size_t m_errorCounter;

		//========================================================================================//
		//========================================================================================//

		/*
		*	opcode = opcode_HEX_VAL;
		*	
		*	getSecondPart();
		* 
		*	//insert Mod bits
		*	switch()
		*		...
		* 
		*	output.push_back(opcode_val);
		*/
		void parseNOP();
		void parseORIGIN();
		void parseDB();
		void parseDEFINE();
		void parseINCLUDE();

		//-----------parseX()--------------//
	
		void parseLOAD();
		void parseMOV();
		void parseSTR();


		//------------------ALU-------------//
		void parseArithmeticPart();

		//----parse and,or,xor,shl,shr-------//
		void parseLogicPart();

		//----------------------------------//

		void parseNOT();
		//-----------------------------------//

		void parsePUSH();
		void parsePOP();

		void parseKWAIT();
		void parseMWE();
		void parseMR();
	
		void parseLabel();

		void parseCALL();
		void parseFUNC();
		void parseRET();

		//----parse jmp,jlz,jgz,jcf....-------//
		void parseJMP();

		void parseITOF();
		void parseFTOI();
		void parseHLT();
	
	};

}


