#include <fstream>

#include "magic_enum/magic_enum.hpp"

#include "Common.h"
#include "Lexer.h"

namespace dasm
{
	enum Komut
	{
		NOP =			   0x00,	LOAD_rx_sayi =		0x01,	ADD_rx_sayi =	 0x02,	SHL_rx_sayi	  = 0x03,	CALL_regadr = 0x04, FADD_rx_sayi    = 0x06,	FCMP_rx_sayi	= 0x07,	HLT = 0xff,
		STR_adr_sayi =     0x10,	LOAD_rx_regadr =    0x11,	ADD_rx_regadr =  0x12,	SHL_rx_ry	  = 0x13,	CALL_adr	= 0x14,	FADD_rx_regadr  = 0x16,	FCMP_rx_regadr  = 0x17,
		STR_adr_regadr =   0x20,	LOAD_rx_adr	=       0x21,	ADD_rx_adr =     0x22,	SHR_rx_sayi	  = 0x23,	PUSH_rx     = 0x24,	FADD_rx_adr	    = 0x26,	FCMP_rx_adr		= 0x27,
		STR_adr_rx	=      0x30,	LOAD_rx_adr_p_reg = 0x31,	ADD_rx_ry =      0x32,	SHR_rx_ry	  = 0x33,	POP_rx		= 0x34,	FADD_rx_ry		= 0x36,	FCMP_rx_ry		= 0x37,
		STR_adr_p_reg_ry = 0x40,	MOV =               0x41,	SUB_rx_sayi =    0x42,	AND_rx_ry	  = 0x43,						FSUB_rx_sayi	= 0x46,	ITOF_rx			= 0x47,
		JMP =			   0x50,								SUB_rx_regadr =  0x52,	NOT_rx		  = 0x53,	RET			= 0xe4,	FSUB_rx_regadr	= 0x56,	FTOI_rx			= 0x57,
		JNE =			   0x60,								SUB_rx_adr =     0x62,	OR_rx_ry	  = 0x63,						FSUB_rx_adr		= 0x66,								
		JE  =			   0x70,								SUB_rx_ry =      0x72,	XOR_rx_ry     = 0x73,						FSUB_rx_ry		= 0x76,
		JG  =			   0x80,								MUL_rx_sayi =    0x82,	CMP_rx_sayi   = 0x83,						FMUL_rx_sayi	= 0x86,
		JL  =			   0x90,								MUL_rx_regadr =  0x92,	CMP_rx_regadr = 0x93,						FMUL_rx_regadr  = 0x96,
																MUL_rx_adr =     0xa2,  CMP_rx_ry     = 0xa3,						FMUL_rx_adr		= 0xa6,
																MUL_rx_ry =      0xb2,												FMUL_rx_ry		= 0xb6,
																DIV_rx_sayi =    0xc2,												FDIV_rx_sayi	= 0xc6,
																DIV_rx_regadr =  0xd2,												FDIV_rx_regadr	= 0xd6,
																DIV_rx_adr =	 0xe2,												FDIV_rx_adr		= 0xe6,
																DIV_rx_ry =		 0xf2,												FDIV_rx_ry		= 0xf6,
	};		

	struct InstructionInfo
	{
		std::string m_opcode;		
	};

	class Disassembler
	{
	public:
		Disassembler();
		~Disassembler();

		void run(std::vector<asmc::MemoryLayout>& memlay);
	private:
		//std::unordered_map<uint8_t, dasm::InstructionInfo> m_opcodeTable;
	};	
}