#pragma once

#include <cstdint>
#include <string>

namespace dasm
{
	enum Komut : uint16_t
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
																MUL_rx_adr =     0xa2,  CMP_rx_adr    = 0xa3,						FMUL_rx_adr		= 0xa6,
																MUL_rx_ry =      0xb2,  CMP_rx_ry     = 0xb3,						FMUL_rx_ry		= 0xb6,
																DIV_rx_sayi =    0xc2,	SAR_rx_sayi	  = 0xc3,						FDIV_rx_sayi	= 0xc6,
																DIV_rx_regadr =  0xd2,	SAR_rx_ry	  = 0xd3,   					FDIV_rx_regadr	= 0xd6,
																DIV_rx_adr =	 0xe2,	SAL_rx_sayi   = 0xe3,   					FDIV_rx_adr		= 0xe6,
																DIV_rx_ry =		 0xf2,	SAL_rx_ry	  = 0xf3,   					FDIV_rx_ry		= 0xf6,
	};
}


namespace asmc
{
	enum class LabelStatus
	{
		/* STATUS examples
		*
		* -----------VALID-WritingStyle---
		*
		* CALL mult
		*	...
		* FUNC mult <= Called
		*  ...
		* RET
		*
		* -----------ERROR---------------
		*
		* CALL mult
		*   ...
		* FUNC mult <= Called_NoRet
		*  ...
		*  ...
		*
		* ------------WARNING---------------
		*
		* FUNC mult <= No_Call
		*  ...
		* RET
		*
		* ------------ERROR------------------
		* ...
		* RET
		* ...
		*
		* ------------ERROR------------------
		* ...
		* FUNC mult <= No_Ret
		* ...
		*
		*/


		Used,
		NotUsed,		//warning
		Undefined,		//error     !!

		No_FuncDef,		//error		!!
		Called_NoRet,	//error		!!
		No_Ret,			//error		!!
		No_Call,		//warning	
		Called
	};


	//struct InstructionBits
	//{
	//	uint8_t m_opcode;
	//	uint8_t m_RxRy;
	//	uint8_t m_mod;
	//};

	struct UnresolvedEntry
	{
		uint8_t m_opcode = 0;
		
		int m_ramIndex = -1;		

		//for printing error or warning
		std::string m_fileName;
		int m_lineNumber = -1;

		asmc::LabelStatus m_status;
	};

	enum class RegisterFlag
	{
		Reg,
		NoReg
	};

	struct MemoryLayout
	{
		uint8_t m_opcode = 0;
		uint8_t m_regPart = 0;

		//MEMORY LEAK freed by OS at exit
		uint8_t* m_packet = nullptr;
		uint8_t m_packetSize = 0;
		//starting of the ram location
		int m_ramIndex = -1;

		//MEMORY LEAK freed by OS at exit
		uint8_t* m_reservedPacket = nullptr;
		uint8_t  m_resPacketSize = 0;

		asmc::RegisterFlag m_regFlag = asmc::RegisterFlag::Reg;
	};

	struct Symbol
	{
		//label,func => jump address
		//define => number
		int m_ramIndex = -1;

		//for printing error or warning
		std::string m_fileName;
		int m_lineNumber = -1;

		asmc::LabelStatus m_status;

	};

	struct PacketAdrPReg
	{
		uint32_t m_adrPart;
		uint32_t m_regPart;
	};
}