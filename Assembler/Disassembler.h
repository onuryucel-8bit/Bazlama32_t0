#include "pch.h"


#include "Common.h"
#include "Lexer.h"

namespace asmc
{
	struct InstructionInfo
	{
		std::string m_opcode;		
	};

	class Disassembler
	{
	public:
		Disassembler();
		~Disassembler();

		void run(std::vector<asmc::MemoryLayout>& memlay);
	private:
		std::unordered_map<uint8_t, asmc::InstructionInfo> m_opcodeTable;
	};	
}