
#include "pch.h"

#include "FileReader.h"

namespace baz
{
	FileReader::FileReader()
	{
		m_ram = nullptr;
		m_currentChar = 0;
		m_position = -1;
	}

	FileReader::~FileReader()
	{
	}

	void FileReader::readFile(std::string path, std::vector<uint8_t>* ram)
	{
		m_ram = ram;

		std::fstream file(path);

		if (!file.is_open())
		{
			std::cout << "ERROR:: couldnt open the file\n";
			return;
		}

		std::stringstream ss;

		ss << file.rdbuf();

		source = ss.str();

		m_currentChar = 0;
		m_position = -1;

		placeFiletoRAM();
	}

	void FileReader::nextChar()
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

	std::string FileReader::getNextLine()
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

	void FileReader::placeFiletoRAM()
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

			(*m_ram)[index] = (uint8_t)rdx::hexToDec(str);
		}

	}	
}