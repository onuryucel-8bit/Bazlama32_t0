#include <iostream>
#include <optional>
#include <string>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>

#include "../libsLocal/Radix.h"

#define baz_maskCommandBits 0b0011'1111
#define baz_maskModBits 0b1100'0000;
#define baz_maskRx 0b0011'1000;
#define baz_maskRy 0b0000'0111;
#define baz_maskUz 0b1100'0000;


std::string source;
char m_currentChar;
int m_position = -1;
const int RAM_Size = 256;
uint8_t* ram = new uint8_t[RAM_Size];

std::string readFile(std::string path)
{
	std::fstream file(path);

	if (!file.is_open())
	{
		std::cout << "ERROR:: couldnt open the file\n";
		return "";
	}

	std::stringstream ss;

	ss << file.rdbuf();

	return ss.str();
}


void nextChar()
{
	m_position++;
	if (m_position > source.length())
	{
		m_currentChar = EOF;
	}
	else
	{
		m_currentChar = source[m_position];
	}

}

std::string getNextLine()
{
	std::string str;
	while (m_currentChar != ' ' && m_currentChar != '\n' && m_currentChar != EOF)
	{
		if (std::isxdigit(m_currentChar))
		{
			str += m_currentChar;
		}
		nextChar();
	}

	nextChar();

	return str;
}

void placeFiletoRAM()
{
	while (m_currentChar != EOF)
	{
		while (m_currentChar == '\n')
		{
			nextChar();
		}

		//get index
		std::string str = getNextLine();

		if (m_currentChar == EOF)
		{
			break;
		}

		uint32_t index = rdx::hexToDec(str);

		//get hex value of the command
		str = getNextLine();

		ram[index] = rdx::hexToDec(str);
	}

}

void printRam(size_t start = 0, size_t end = RAM_Size)
{
	if (start > end)
	{
		return;
	}

	for (size_t i = start; i < end; i++)
	{		
		std::cout << (int)ram[i] << "\n";	
	}
}

namespace baz
{
	class emu
	{
	public:
		emu();
		~emu();

	private:

		uint32_t getBytes(uint8_t uz);

		uint32_t pc = 0;
		uint32_t registerFile[8] = {};
		uint8_t komut;
		uint8_t* ram = new uint8_t[RAM_Size];

		
	};

	emu::emu()
	{
	}

	emu::~emu()
	{
	}

	uint32_t emu::getBytes(uint8_t uz)
	{
		uint32_t retval = 0;
		uint32_t temp;

		if (uz == 0)
		{
			retval = ram[pc];
			return retval;
		}

		for (size_t i = 0; i < uz; i++)
		{
			temp = ram[++pc];
			retval = (retval << 8) | temp;
		}

		return retval;
	}

}
uint32_t pc = 0;
uint32_t registerFile[8] = {};
uint8_t komut;

uint32_t getBytes(uint8_t uz)
{
	uint32_t retval = 0;
	uint32_t temp;

	if (uz == 0)
	{
		retval = ram[pc];
		return retval;
	}
	
	for (size_t i = 0; i < uz; i++)
	{
		temp = ram[++pc];
		retval = (retval << 8) | temp;		
	}

	return retval;
}

void op_LOAD()
{
	uint8_t type = komut & baz_maskModBits;
	uint8_t rx, ry, uz;
	uint32_t value;

	switch (type)
	{
	//load rx,sayi
	case 0:
		pc++;
		rx = ram[pc] & baz_maskRx;
		rx = rx >> 3;

		uz = ram[pc] & baz_maskUz;
		uz = uz >> 6;

		value = getBytes(uz);

		
		registerFile[rx] = value;

		break;
	//load rx,@ry
	case 1:

		pc++;
		rx = ram[pc] & baz_maskRx;
		rx = rx >> 3;

		uz = ram[pc] & baz_maskUz;
		uz = uz >> 6;

		value = getBytes(uz);

		registerFile[rx] = ram[value];
		break;

	}
}

void op_ADD()
{
	uint8_t type = komut & baz_maskModBits;
	uint8_t rx, ry, uz;

	switch (type)
	{
		//add rx,ry
	case 0:
		pc++;
		rx = ram[pc] & baz_maskRx;
		ry = ram[pc] & baz_maskRy;

		registerFile[rx] += registerFile[ry];

		break;
		//add rx,sayi
	case 1:
		break;

	}
}

//void op_STR();
//void op_MOV();
//void op_SUB();
//void op_MUL();
//void op_DIV();
//void op_AND();
//void op_OR();
//void op_NOT();
//void op_XOR();
//void op_SHL();
//void op_SHR();
//void op_CMP();
//void op_PUSH();
//void op_POP();

int main()
{
	for (size_t i = 0; i < RAM_Size; i++)
	{
		ram[i] = 0;
	}

	source = readFile("emu.txt");
	placeFiletoRAM();

	while (ram[pc] != 0)
	{
		komut = ram[pc];

		uint8_t mask = komut & baz_maskCommandBits;

		switch (mask)
		{
		case 0x01:
			op_LOAD();
			break;
		case 0x12:
			op_ADD();
			break;
		}

		pc++;
	}

	for (size_t i = 0; i < 8; i++)
	{
		std::cout << registerFile[i] << "\n";
	}

	delete[] ram;
}