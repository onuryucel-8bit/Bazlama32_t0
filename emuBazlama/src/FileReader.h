#pragma once

#include "pch.h"

#include "utils/Radix.h"

namespace baz
{

	class FileReader
	{
	public:
		FileReader();
		~FileReader();
		
		void readFile(std::string path, std::vector<uint8_t>* ram);

	private:
		void placeFiletoRAM();
		void nextChar();
		std::string getNextLine();

		std::vector<uint8_t>* m_ram;

		std::string source;
		char m_currentChar;
		int m_position;
	};
}
