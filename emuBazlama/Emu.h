#pragma once

#include "pch.h"

#define baz_maskCommandBits 0b0011'1111
#define baz_maskModBits 0b1100'0000;
#define baz_maskRx 0b0011'1000;
#define baz_maskRy 0b0000'0111;
#define baz_maskUz 0b1100'0000;

#include "../libsLocal/Radix.h"

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
		void readFile();
		void op_LOAD();
		void op_ADD();

		uint32_t getBytes(uint8_t uz);

		const int RAM_Size = 256;

		std::string source;
		char m_currentChar;
		int m_position = -1;		

		uint32_t pc = 0;
		uint32_t registerFile[8] = {};
		uint8_t komut;
		uint8_t* ram = new uint8_t[RAM_Size];
	};	
}