#pragma once

#include "pch.h"

#include "../libsLocal/utils/Radix.h"

namespace baz
{

	class FileReader
	{
	public:
		FileReader();
		~FileReader();
		
		void placeFiletoRAM();
		void readFile(std::string path, uint8_t* ram);

		uint32_t getBytes(uint8_t uz, uint32_t& adres);

	private:
		void nextChar();
		std::string getNextLine();

		uint8_t* m_ram;

		std::string source;
		char m_currentChar;
		int m_position = -1;
	};
}
