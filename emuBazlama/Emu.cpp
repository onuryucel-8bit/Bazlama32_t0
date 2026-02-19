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
	}

	void Emu::readFile()
	{

	}

	void Emu::op_LOAD()
	{
	}

	void Emu::op_ADD()
	{
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

	uint32_t Emu::getBytes(uint8_t uz)
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