#include "LexerPCH.h"

#include "Lexer.h"

#include "Parser.h"

int main(int argc, char* argv[])
{

	//TODO add filesystem

	std::string path;
	if (argc < 2)
	{
		std::cout << "Expected file path...\n";

#ifdef _DEBUG		
		std::cout << "Loading default main.asm\n";
		path = "main.asm";
#else
		return 0;
#endif // DEBUG_FILE
	}
	else
	{
		path = argv[1];
	}

	asmc::Lexer lexer(path);

	asmc::Parser parser(lexer);

	parser.run();

}
