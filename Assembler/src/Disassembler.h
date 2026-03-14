#include <fstream>


#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 255
#include "magic_enum/magic_enum.hpp"

#include "rang.hpp"

#include "Common.h"
#include "Lexer.h"

namespace dasm
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
		//std::unordered_map<uint8_t, dasm::InstructionInfo> m_opcodeTable;
	};	
}