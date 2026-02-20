#pragma once

//TODO pch ekle
#include "LexerPCH.h"

#include "../LibsLocal/magic_enum/magic_enum.hpp"
#include "../libsLocal/rang.hpp"

#include "Tokens.h"

#define asmc_MAX_TOKEN_LIST_SIZE 30
#define asmc_CLOSE_DEBUG_WORD "CLOSE_DEBUG"

namespace asmc
{

enum UzTip
{
	REG_8,		//s
	REG_16,		//a
	REG_32 = 3	//o
};

struct Token
{
	std::string m_text = "";
	asmc::TokenType m_type = TokenType::EMPTY;
	asmc::UzTip m_regType = asmc::UzTip::REG_8;
	size_t m_lineNumber = 0;

	bool operator==(const Token& other) const
	{
		return m_text == other.m_text && m_type == other.m_type;
	}
};

//For reading multiple files
/*
	//main.asm						   stdmath.asm
	#include "stdmath.asm"'\n' ------> add r0,r1
	...					  ^			   ....
	...					  |			   ...
	..					  \------- (after reading stdmath assembler points to '\n')
*/
struct FileData
{
	std::string m_path;
	std::streamoff m_lastPosition = 0;
	char m_currentChar = 0;
	size_t m_lineNumber = 0;
};

}

namespace std
{
	//for hash table
	template<>
	struct hash<asmc::Token>
	{
		size_t operator() (const asmc::Token& key) const
		{
			return std::hash<std::string>()(key.m_text);
		}
	};
}

namespace asmc
{

class Lexer
{
public:
	
	//TODO use filesystem
	Lexer(std::string path);
	
	[[nodiscard]] std::array<asmc::Token, asmc_MAX_TOKEN_LIST_SIZE> getTokenList();

	bool getDebugFlag();
	size_t getErrorAmount();

	bool isInputStreamEmpty();

	//pushes new file top of the stack(inputStream)
	void pushFile(std::string path);

	//if input stream is empty return s true
	bool popFile();

	std::string getCurrentFileName();

	//TODO private?
	size_t m_lineNumber;
private:

	[[nodiscard]] Token getToken();
	void toUpper(std::string& str);

	void nextChar();

	char peek();

	//is 0xfa
	bool isNumberHex();
	//checks second part of mnemonic 0xfa or register
	bool isOperand();

	//sets error flag true
	void printError(std::string message);
	
	void skipWhiteSpace();
	//skip ;
	void skipComments();
	//skip ',' '\n'
	void skipNonEssential();

	
	std::string readFile(std::string path, std::streamoff starpos = 0);

	//TODO remove or fix
	//returns sub part of m_program
	std::string getSubStr(int startPos, int length, bool upper = true);
	
	bool checkIfKeyword(std::string token);

	//------------------------------------------------------//
	//---------------------lex[X]()-------------------------//
	//------------------------------------------------------//

	//returns token starting with '.' .ORIGIN .DB
	[[nodiscard]] asmc::Token lexDotPart();

	//returns register hex part r0,r2,r3, .. => 0,2,3, ...	
	[[nodiscard]] asmc::Token lexRegPart();

	//0xff
	[[nodiscard]] asmc::Token lexHexNumberPart();
	
	//+,-,@,\n,EOF.... single char
	[[nodiscard]] asmc::Token lexSingleChar();

	//returns keywords LOAD,STR,ADD, LABEL or ID
	[[nodiscard]] asmc::Token lexWord();

	//returns token starting with '#' #define, #include
	[[nodiscard]] asmc::Token lexMacro();

	//------------------------------------------------------//
	//------------------------------------------------------//
	//------------------------------------------------------//

	/*
		used for return after reading another file
	
		#include "stdmath.asm"
		add r0, r1			 ^
		add r0, r1			 \-----m_returnPosition / m_returnCurrentChar / m_retLineNumber
				 ^
				 \---- m_position/m_currentChar/m_lineNumber
	*/
	size_t m_returnPosition;
	char m_returnCurrentChar;
	size_t m_retLineNumber;

	int m_position;
	char m_currentChar;

	size_t m_errorCounter;
	bool f_error;
	bool f_newline;
	bool fd_debug;

	std::array<asmc::Token, asmc_MAX_TOKEN_LIST_SIZE> m_tokenArr;

	std::string m_program;
	std::string m_currentFileName;
	asmc::Token m_lastToken;
	

	std::vector<FileData> m_inputStream;
};

}