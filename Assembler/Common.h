#pragma once

#include <cstdint>
#include <string>

namespace asmc
{
	enum class LabelStatus
	{
		/* STATUS examples
		*
		* -----------VALID-WritingStyle---
		*
		* CALL mult
		*	...
		* FUNC mult <= Called
		*  ...
		* RET
		*
		* -----------ERROR---------------
		*
		* CALL mult
		*   ...
		* FUNC mult <= Called_NoRet
		*  ...
		*  ...
		*
		* ------------WARNING---------------
		*
		* FUNC mult <= No_Call
		*  ...
		* RET
		*
		* ------------ERROR------------------
		* ...
		* RET
		* ...
		*
		* ------------ERROR------------------
		* ...
		* FUNC mult <= No_Ret
		* ...
		*
		*/


		Used,
		NotUsed,		//warning
		Undefined,		//error     !!

		No_FuncDef,		//error		!!
		Called_NoRet,	//error		!!
		No_Ret,			//error		!!
		No_Call,		//warning	
		Called
	};


	//struct InstructionBits
	//{
	//	uint8_t m_opcode;
	//	uint8_t m_RxRy;
	//	uint8_t m_mod;
	//};

	struct UnresolvedEntry
	{
		uint32_t m_opcode = 0;
		uint32_t m_secondPart = 0;
		int m_ramIndex = -1;
		//???
		char m_packetSize = 0;

		//for printing error or warning
		std::string m_fileName;
		int m_lineNumber = -1;

		asmc::LabelStatus m_status;
	};

	struct MemoryLayout
	{
		uint8_t m_opcode = 0;
		uint8_t m_secondPart = 0;
		uint8_t m_regPart = 0;
		//TODO add delete
		uint8_t* m_packet = nullptr;
		int m_ramIndex = -1;
		char m_packetSize = 0;
	};

	struct Symbol
	{
		//label,func => jump address
		//define => number
		int m_ramIndex = -1;

		//for printing error or warning
		std::string m_fileName;
		int m_lineNumber = -1;

		asmc::LabelStatus m_status;


	};

	struct PacketAdrPReg
	{
		uint32_t m_adrPart;
		uint32_t m_regPart;
	};
}