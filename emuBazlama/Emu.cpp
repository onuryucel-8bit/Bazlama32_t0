#include "Emu.h"

namespace baz
{
	Emu::Emu(std::string path)
	{
	}

	Emu::~Emu()
	{
	}

	void Emu::run()
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
			std::cout << "reg" << i << " : " << registerFile[i] << "\n";
		}
	}

	std::string Emu::readFile(std::string path)
	{
		std::fstream file(path);

		if (!file.is_open())
		{
			std::cout << "ERROR:: couldnt open the file\n";
			//return "";
		}

		std::stringstream ss;

		ss << file.rdbuf();

		return ss.str();
	}

	void Emu::nextChar()
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

	std::string Emu::getNextLine()
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

	void Emu::placeFiletoRAM()
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

	uint32_t Emu::getBytes(uint8_t uz, uint32_t& adres)
	{
		uint32_t retval = 0;
		uint32_t temp;

		if (uz == 0)
		{
			retval = ram[++adres];
			return retval;
		}

		for (size_t i = 0; i < uz; i++)
		{
			temp = ram[++adres];
			retval = (retval << 8) | temp;
		}

		return retval;
	}



	void Emu::op_LOAD()
	{
		uint8_t type = komut & baz_maskModBits;
		uint8_t rx, ry, uz;
		uint32_t value;
		uint32_t adr;

		switch (type)
		{

		//load rx,sayi
		case 0:
			pc++;
			rx = ram[pc] & baz_maskRx;
			rx = rx >> 3;

			uz = ram[pc] & baz_maskUz;
			uz = uz >> 6;

			value = getBytes(uz, pc);


			registerFile[rx] = value;

			break;

		//load rx,@ry
		case 1:

			pc++;
			rx = ram[pc] & baz_maskRx;
			rx = rx >> 3;

			ry = ram[pc] & baz_maskRy;			

			uz = ram[pc] & baz_maskUz;
			uz = uz >> 6;

			value = getBytes(uz, pc);

			registerFile[rx] = ram[value];
			break;

		//load rx,@adr
		case 2:
			pc++;
			rx = ram[pc] & baz_maskRx;
			rx = rx >> 3;

			uz = ram[pc] & baz_maskUz;
			uz = uz >> 6;

			value = getBytes(uz, pc);



			break;

		}
	}

	void Emu::op_ADD()
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

	void Emu::op_STR()
	{
	}

	void Emu::op_MOV()
	{
	}

	void Emu::op_CALL()
	{
	}

	void Emu::op_RET()
	{
	}

	void Emu::op_IRET()
	{
	}

	void Emu::op_PUSH()
	{
	}

	void Emu::op_POP()
	{
	}

	void Emu::op_PUSHA()
	{
	}

	void Emu::op_POPA()
	{
	}

	void Emu::op_JMP()
	{
	}

	void Emu::op_JNE()
	{
	}

	void Emu::op_JE()
	{
	}

	void Emu::op_JG()
	{
	}

	void Emu::op_JL()
	{
	}

	void Emu::op_MWE()
	{
	}

	void Emu::op_MD()
	{
	}

	void Emu::op_SUB()
	{
	}

	void Emu::op_MUL()
	{
	}

	void Emu::op_DIV()
	{
	}

	void Emu::op_AND()
	{
	}

	void Emu::op_OR()
	{
	}

	void Emu::op_NOT()
	{
	}

	void Emu::op_XOR()
	{
	}

	void Emu::op_SHL()
	{
	}

	void Emu::op_SHR()
	{
	}

	void Emu::op_CMP()
	{
	}
}