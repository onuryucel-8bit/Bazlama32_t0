#include "pch.h"
#include "Disassembler.h"

namespace dasm
{
	Disassembler::Disassembler()
	{

	}

	Disassembler::~Disassembler()
	{
	}

	void Disassembler::run(std::vector<asmc::MemoryLayout>& memlay)
	{
		std::ofstream file(cmake_PROJECT_OUTPUT "disassembler.txt");

		if (!file.is_open())
		{
			return;
		}

		file << std::hex;

		for (auto const& item : memlay)
		{
			auto itemEnum = magic_enum::enum_cast<dasm::Komut>(item.m_opcode);
			

			if (itemEnum.has_value())
			{
				size_t ramIndex = item.m_ramIndex;

				file << ramIndex << "|" << magic_enum::enum_name(itemEnum.value()) << "\n";
				ramIndex++;
				
				if (item.m_regFlag == asmc::RegisterFlag::Reg)
				{
					file << ramIndex << "|[" << (int)item.m_regPart << "]\n";
					ramIndex++;
				}

				for (size_t i = 0; i < item.m_packetSize; i++)
				{
					file << ramIndex << "|" << (int)item.m_packet[i] << "\n";
					ramIndex++;
				}

				for (size_t i = 0; i < item.m_resPacketSize; i++)
				{
					file << ramIndex << "|" << (int)item.m_reservedPacket[i] << "\n";
					ramIndex++;
				}
			}

			file << "====================\n";
		}

		file.close();
	}
}