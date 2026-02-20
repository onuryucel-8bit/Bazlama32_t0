#pragma once

#include "pch.h"

#define baz_maskCommandBits 0b0011'1111
#define baz_maskModBits 0b1100'0000;
#define baz_maskRx 0b0011'1000;
#define baz_maskRy 0b0000'0111;
#define baz_maskUz 0b1100'0000;

#include "../libsLocal/utils/Radix.h"

namespace baz
{
	class Emu
	{
	public:
		Emu(std::string path);
		~Emu();

		void run();

	private:

		//TODO okuma sistemini ayir
		void nextChar();
		std::string getNextLine();
		void placeFiletoRAM();
		std::string readFile(std::string path);
		
		uint32_t getBytes(uint8_t uz, uint32_t& adres);

		const int RAM_Size = 256;

		std::string source;
		char m_currentChar;
		int m_position = -1;		

		uint32_t pc = 0;
		uint32_t registerFile[8] = {};
		uint8_t komut;
		uint8_t* ram = new uint8_t[RAM_Size];

		//-----------------------------------//
		//-----------------------------------//
		//-----------------------------------//
		void op_LOAD();
		void op_ADD();
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
		void op_SUB();
		void op_MUL();
		void op_DIV();
		void op_AND();
		void op_OR();
		void op_NOT();
		void op_XOR();
		void op_SHL();
		void op_SHR();
		void op_CMP();

	};	
}