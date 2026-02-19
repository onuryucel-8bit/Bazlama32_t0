#include "pch.h"

#include "Emu.h"

//
//std::string readFile(std::string path)
//{
//	std::fstream file(path);
//
//	if (!file.is_open())
//	{
//		std::cout << "ERROR:: couldnt open the file\n";
//		return "";
//	}
//
//	std::stringstream ss;
//
//	ss << file.rdbuf();
//
//	return ss.str();
//}

//void op_STR();
//void op_MOV();
//void op_CALL();
//void op_RET();
//void op_IRET();
//void op_PUSH();
//void op_POP();
//void op_PUSHA();
//void op_POPA();
//void op_JMP();
//void op_JNE();
//void op_JE();
//void op_JG();
//void op_JL();
//void op_MWE();
//void op_MD();
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

int main()
{
	baz::Emu emu("emu.txt");

	emu.run();
}