#include "Disassembler.h"

namespace asmc
{
	Disassembler::Disassembler()
	{
		m_opcodeTable[0x01] = { .m_opcode = "load" };
		m_opcodeTable[0x11] = { .m_opcode = "load" };
		m_opcodeTable[0x21] = { .m_opcode = "load" };
		m_opcodeTable[0x31] = { .m_opcode = "load" };
	}

	Disassembler::~Disassembler()
	{
	}

	void Disassembler::run(std::vector<asmc::MemoryLayout>& memlay)
	{
		for (const auto& item : memlay)
		{
			char uzreg = 'x';
			switch ((item.m_regPart & 0b1100'0000) >> 6)
			{
			case asmc::UzTip::REG_8:
				uzreg = 's';
				break;

			case asmc::UzTip::REG_16:
				uzreg = 'a';
				break;

			case asmc::UzTip::REG_32:
				uzreg = 'o';
				break;
			}

			std::cout
				<< m_opcodeTable[item.m_opcode].m_opcode
				<< " r" << uzreg << ((item.m_regPart & 0b0011'1000) >> 3);

			//TODO !!
			std::cout << ", 0x";
			for (size_t i = 0; i < item.m_packetSize; i++)
			{
				std::cout << std::hex << (int)item.m_packet[i];
			}
			std::cout << "\n";
			
				
		}
	}
}