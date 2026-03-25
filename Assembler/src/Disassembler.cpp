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

	void Disassembler::run(std::vector<asmc::MemoryLayout>& memlay, 
		std::unordered_map<asmc::Token, std::vector<asmc::UnresolvedEntry>>& unresolvedTable,
		std::unordered_map<asmc::Token, asmc::Symbol> symbolTable)
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

				for (const auto& [key, value] : symbolTable)
				{
					if (value.m_ramIndex == ramIndex)
					{
						file << "*--------------------------*\n";
						file << "|" << key.m_text << "|\n";
						file << "*--------------------------*\n";
					}
				}

				//std::cout << ramIndex << "|" << magic_enum::enum_name(itemEnum.value()) << "\n";
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
			else
			{
				std::cout 
					<< "ERROR::Disassembler invalid opcode [" 
					<< std::hex 					
					<< (int)item.m_opcode 
					<< "]\n";
				
			}

			file << "====================\n";
		}

		file.close();
	}
}