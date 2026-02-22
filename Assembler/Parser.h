#pragma once

//TEST DEBUG
#include <bitset>
#include <fstream>

#include "../libsLocal/rang.hpp"
#include "../libsLocal/utils/Radix.h"
#include "../libsLocal/magic_enum/magic_enum.hpp"

#include "Lexer.h"
#include "Disassembler.h"

#include "Common.h"

//TODO move to ..\utils
#include "Timer.h"


#define PARSER_TEST_FUNCS

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
#define asmc_MOD_Rx_Ry 1

//4byte => 32 bit
#define asmc_WORD 4

#define asmc_CombineModBits(opcode, modBits) \
	(opcode | (modBits << 4))

#define asmc_CombineModBits_t0(opcode, modBits) \
	(m_opcodeHexTable[opcode]) + (modBits << 4)

//TODO remove
#define asmc_ShiftAmount_Opcode 24

#define asmc_ShiftAmount_RegA 3
#define asmc_ShiftAmount_RegB 0

#define asmc_CombineRegA(regPart) \
	(regPart) << asmc_ShiftAmount_RegA

#define asmc_CombineRegB(regPart, ry) \
	(regPart) | (ry) << asmc_ShiftAmount_RegB

#define asmc_CombineRegUz(regPart, uz) \
	(regPart) | ( (uz) << 6 )


namespace asmc
{

	enum modBits
	{
		number,
		adr,
		regAdr,
		adr_p_reg,
	};

	class Parser
	{
	public:
		Parser(asmc::Lexer& lexer);
		~Parser();

		void run();
	private:
	
	
		void program();

		void moveCurrentToken();
	
		void checkTables();

		void printError(std::string message);
		void printWarning(std::string message);

		void writeOutput();

		MemoryLayout parseOperand(uint32_t opcode);

		void convertToBytes(std::string& text, asmc::UzTip regtype, uint8_t* packet);

		PacketAdrPReg getAdr_P_RegPart(std::string& operand);

	#ifdef PARSER_TEST_FUNCS
		asmc::TokenType toToken(uint8_t opcode);
		void printBinHex(asmc::MemoryLayout& memlay);
		void printCurrentPeekToken();
	#endif // PARSER_TEST_FUNCS

		int m_ramLocation;
		int m_lineNumber;

		asmc::Lexer& m_lexer;
		asmc::Disassembler m_disassembler;

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

		void parseCMP();
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

	};

}


