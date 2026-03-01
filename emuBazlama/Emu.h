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

	enum InstructionHexVal
	{
		NOP =			   0x00,	LOAD_rx_sayi =		0x01,	ADD_rx_sayi =	 0x02,	SHL_rx_sayi	  = 0x03,	CALL_regadr = 0x04,
		STR_adr_sayi =     0x10,	LOAD_rx_regadr =    0x11,	ADD_rx_regadr =  0x12,	SHL_rx_ry	  = 0x13,	CALL_adr	= 0x14,
		STR_adr_regadr =   0x20,	LOAD_rx_adr	=       0x21,	ADD_rx_adr =     0x22,	SHR_rx_sayi	  = 0x23,	PUSH_rx     = 0x24,
		STR_adr_rx	=      0x30,	LOAD_rx_adr_p_reg = 0x31,	ADD_rx_ry =      0x32,	SHR_rx_ry	  = 0x33,	POP_rx		= 0x34,
		STR_adr_p_reg_ry = 0x40,	MOV =               0x41,	SUB_rx_sayi =    0x42,	AND_rx_ry	  = 0x43,	
		JMP =			   0x50,								SUB_rx_regadr =  0x52,	NOT_rx		  = 0x53,	RET			= 0xe4,
		JNE =			   0x60,								SUB_rx_adr =     0x62,	OR_rx_ry	  = 0x63,	
		JE  =			   0x70,								SUB_rx_ry =      0x72,	XOR_rx_ry     = 0x73,	
		JG  =			   0x80,								MUL_rx_sayi =    0x82,	CMP_rx_sayi   = 0x83,	
		JL  =			   0x90,								MUL_rx_regadr =  0x92,	CMP_rx_regadr = 0x93,	
																MUL_rx_adr =     0xa2,  CMP_rx_ry     = 0xa3,	
																MUL_rx_ry =      0xb2,
																DIV_rx_sayi =    0xc2,
																DIV_rx_regadr =  0xd2,
																DIV_rx_adr =	 0xe2,
																DIV_rx_ry =		 0xf2,


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

	private:

		baz::FileReader fr;
		
		const int RAM_Size = 256;
				
		uint32_t pc = 0;
		uint32_t m_registerFile[8] = {};
		uint8_t m_komut;
		uint8_t* m_ram = new uint8_t[RAM_Size];

		//pc++ returns reguz,rega,regb
		baz::RegisterPart getRegisterPart();

		void calculate(baz::OperationType type, uint32_t& reg, uint32_t value);

		uint32_t getBytes(uint8_t uz, uint32_t adr);

		//pc++
		uint32_t getBytes(uint8_t uz);

		void storeBytesToRam(uint32_t data, uint32_t adr);
		
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
		void op_SHL();
		void op_SHR();
		void op_CMP();

	};	
}