#pragma once

#define baz_maskCommandBits 0b0011'1111
#define baz_maskModBits 0b1100'0000
#define baz_maskRx 0b0011'1000
#define baz_maskRy 0b0000'0111
#define baz_maskUz 0b1100'0000

#define baz_maskFlagReg_eqeq	0x0000'0001
#define baz_maskFlagReg_less	0x0000'0002
#define baz_maskFlagReg_greater 0x0000'0004

#define baz_false 0
#define baz_true 1

#include "../libsLocal/utils/Radix.h"

#include "FileReader.h"

namespace baz
{
	constexpr size_t KB = 1024;
	constexpr size_t MB = 1024 * KB;
	constexpr size_t GB = 1024 * MB;

	enum FlagReg
	{
		Eqeq = 1,
		Less = 2,
		Greater = 4
	};

	enum RegName
	{
		Sp = 7,	//stack pointer
		Flag = 6
	};

	enum OperationType
	{
		Add,
		Sub,
		Mul,
		Div
	};

	enum UzTip
	{
		REG_8,		//s
		REG_16 = 2,	//a
		REG_32 = 4	//o
	};

	enum Komut
	{
		NOP =			   0x00,	LOAD_rx_sayi =		0x01,	ADD_rx_sayi =	 0x02,	SHL_rx_sayi	  = 0x03,	CALL_regadr = 0x04, FADD_rx_sayi    = 0x06,	FCMP_rx_sayi	= 0x07,	HLT = 0xff
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

	struct RegisterPart
	{
		baz::UzTip m_reguz;
		uint8_t m_rega;
		uint8_t m_regb;
	};

	class Emu
	{
	public:
		Emu(std::string path);
		~Emu();

		void run();
		std::vector<uint8_t> m_ram;
		uint32_t getBytes(uint8_t uz, uint32_t adr);
	private:
				
		baz::FileReader fr;
								
		uint32_t pc = 0;
		uint32_t m_registerFile[8] = {};
		uint8_t m_komut;
		

		//pc++ returns reguz,rega,regb
		baz::RegisterPart getRegisterPart();

		void calculate(baz::OperationType type, uint32_t& reg, uint32_t value);

		

		//pc++
		uint32_t getBytes(uint8_t uz);

		void storeBytesToRam(uint32_t data, uint32_t adr, baz::UzTip uz);
		
		//-----------------------------------//
		//-----------------------------------//
		//-----------------------------------//
		void op_LOAD();
		
		void op_STR();
		void op_MOV();
		void op_CALL();
		void op_RET();
		void op_IRET();
		void op_PUSH();
		void op_POP();
		void op_PUSHA();
		void op_POPA();
		void op_JMP();
		void op_JNE();
		void op_JE();
		void op_JG();
		void op_JL();
		void op_MWE();
		void op_MD();

		void op_Arithmetic();		

		void op_AND();
		void op_OR();
		void op_NOT();
		void op_XOR();
		void op_Shift();
		void op_CMP();

	};	
}