#include "Lexer.h"
namespace asmc 
{

Lexer::Lexer(std::string path)
{

	FileData filedData;

	filedData.m_path = path;

	m_currentFileName = path;

	m_inputStream.push_back(filedData);

	m_program = readFile(path);


	m_position = -1;
	m_currentChar = 0;
	m_lineNumber = 1;
	

	f_error = false;	
	fd_debug = true;
	m_errorCounter = 0;

	nextChar();
}

std::string Lexer::readFile(std::string path, std::streamoff startpos)
{
	std::fstream file(path);
	
	if (!file.is_open())
	{
		std::cout << "ERROR:: couldnt open the file\n";
		return "";
	}
	
	file.seekg(startpos, std::ios_base::beg);
		
	std::stringstream ss;

	ss << file.rdbuf();

	return ss.str();
}

Token Lexer::getToken()
{
	skipWhiteSpace();
	skipComments();
	skipNonEssential();
	//TODO bosluklari es gecme mekanizmasi ekle
	skipWhiteSpace();

	Token token;

	//.origin .db
	if (m_currentChar == '.')
	{
		token = lexDotPart();		
	}
	//#define, #include
	else if (m_currentChar == '#')
	{
		token = lexMacro();		
	}
	//Register,[keyword,label] check
	else if (std::isalpha(m_currentChar))
	{
		//Register?
		//isdigit() checks next char is it number
		if (m_currentChar == 'r' && (
			peek() == 's' || 
			peek() == 'a' ||
			peek() == 'o')
		)
		{
			token = lexRegPart();
		}
		//keyword,label
		else
		{
			token = lexWord();
		}		
	}
	//number check
	else if (std::isdigit(m_currentChar))
	{
		//HEX NUMBER
		if (isNumberHex())
		{			
			token = lexHexNumberPart();
		}
		//DECIMAL
		else
		{
			size_t length = 1;
			size_t startpos = m_position;

			while (std::isxdigit(peek()))
			{
				nextChar();
				length++;
			}

			std::string tokenStr = m_program.substr(startpos, length);

			token = { tokenStr, asmc::TokenType::DECIMAL, asmc::UzTip::REG_8, m_lineNumber };
		}
	}
	else
	{
		token = lexSingleChar();		
	}

	nextChar();

	m_lastToken = token;

	return token;
}

std::array<asmc::Token, asmc_MAX_TOKEN_LIST_SIZE> Lexer::getTokenList()
{
	int i = 0;
	while (i < asmc_MAX_TOKEN_LIST_SIZE)
	{
		asmc::Token token = getToken();

		if (token.m_type == asmc::TokenType::ENDOFFILE)
		{												
			m_tokenArr[i] = { "EOF", asmc::TokenType::ENDOFFILE, asmc::UzTip::REG_8, m_lineNumber};
		}

		if (token.m_type != asmc::TokenType::NEWLINE && token.m_type != asmc::TokenType::DEBUG_TOKEN)
		{
			m_tokenArr[i] = token;
			i++;
		}
		else if (token.m_type == asmc::TokenType::DEBUG_TOKEN)
		{
			fd_debug = false;
		}
	}

	return m_tokenArr;
}

char Lexer::peek()
{
	if (m_position + 1 >= m_program.length())
	{
		return asmc::TokenType::ENDOFFILE;
	}
	return m_program[m_position + 1];
}

//--------------------------------------------//
//-------------lexPartX()---------------------//
//--------------------------------------------//

asmc::Token Lexer::lexDotPart()
{
	asmc::Token token;

	//.origin 0xff
	//returns "origin"
	std::string tokenStr = getSubStr(m_position + 1, 0);

	toUpper(tokenStr);

	if (checkIfKeyword(tokenStr))
	{
		//returns ORIGIN or DB token
		std::optional<asmc::TokenType> enumVal = magic_enum::enum_cast<asmc::TokenType>(tokenStr);
		token = { tokenStr, enumVal.value() , asmc::UzTip::REG_8, m_lineNumber };
	}
	else
	{
		f_error = true;
	}
	

	return token;
}

asmc::Token Lexer::lexRegPart()
{	
	asmc::Token token;

	//input rs,ra,ro
	//skip 'r' part
	nextChar();			

	

	char regType = m_currentChar;	
	
	nextChar();
	

	//register MUST be range 0-7 and next char CANNOT be alphanumeric
	if (!std::isdigit(static_cast<uint8_t>(m_currentChar)))
	{		
		printError("invalid reg operand it MUST be r[0-7]");

		//empty token => error
		token = { std::string(1,m_currentChar), asmc::TokenType::EMPTY, asmc::UzTip::REG_8, m_lineNumber};

		return token;
	}

	switch (regType)
	{
	case 's':
		token.m_regType = asmc::UzTip::REG_8;
		break;

	case 'a':
		token.m_regType = asmc::UzTip::REG_16;
		break;

	case 'o':
		token.m_regType = asmc::UzTip::REG_32;
		break;
	}

	token.m_text = std::string(1, m_currentChar);
	token.m_type = asmc::TokenType::REGISTER;
	token.m_lineNumber = m_lineNumber;

	return token;
}

asmc::Token Lexer::lexHexNumberPart()
{
	asmc::Token token;

	//input 0xff
	//skips hex suffix '0x'
	nextChar();//skip 0
	nextChar();//skip x


	//get ff part
	//-----------------------//
	size_t length = 1;
	size_t startpos = m_position;

	while (std::isxdigit(peek()))
	{
		nextChar();
		length++;
	}

	std::string tokenStr = m_program.substr(startpos, length);
	//-----------------------//

	return token = {tokenStr, asmc::TokenType::HEXNUMBER, asmc::UzTip::REG_8, m_lineNumber };
}

asmc::Token Lexer::lexSingleChar()
{
	asmc::Token token;

	std::string tokenStr;
	size_t length = 1;
	size_t startPos = 0;
	 
	switch (m_currentChar)
	{
	
	case '\n':
		//std::cout << "LEXER newline detected\n";
		token = { std::string(1,m_currentChar), asmc::TokenType::NEWLINE };
		break;

	//ADDRESS
		//regadr => @r0, @r3
		//TODO check
		//adr_p_reg => ?
		//address => @ff, @f214fa
	case '@':		

		nextChar();//move cursor to number OR r

		
		if (m_currentChar == 'r')
		{
			nextChar();
			token = { std::string(1, m_currentChar), asmc::TokenType::REGADR, asmc::UzTip::REG_8, m_lineNumber};
		}
		else
		{
			
			tokenStr = getSubStr(m_position, 1);
			
			nextChar();//move cursor to '+' or ' '
			
			skipWhiteSpace();

			//peek() == '+' || '-'
			if (m_currentChar == '+' || m_currentChar == '-')
			{				
				tokenStr += m_currentChar;
				
				nextChar();//move cursor to 'r' or ' '
				skipWhiteSpace();
				
				if (isOperand())
				{
					nextChar();//get register hex value
					tokenStr += m_currentChar;

					token = { tokenStr, asmc::TokenType::ADR_P_REG, asmc::UzTip::REG_8, m_lineNumber };
				}
			}
			else
			{
				token = { tokenStr, asmc::TokenType::ADDRESS, asmc::UzTip::REG_8, m_lineNumber };
			}

			
		}



		break;

	//string => "qweqrt" or DIRECTORY => "stdmath.asm"
	case '"':
		nextChar();
		
		startPos = m_position;
		while (std::isalnum(peek()) || peek() == ' ' || peek() == '_')
		{
			nextChar();
			length++;
		}

		tokenStr = m_program.substr(startPos, length);

		nextChar(); 

		//is it "abcd.asm" path?
		if (m_currentChar == '.')
		{
			tokenStr += m_currentChar;

			nextChar();
			while (m_currentChar != '"')
			{
				tokenStr += m_currentChar;
				nextChar();
			}
			nextChar();//skip "

			token = { tokenStr, asmc::TokenType::DIRECTORY, asmc::UzTip::REG_8, m_lineNumber };
			m_returnPosition = m_position;
			m_returnCurrentChar = m_currentChar;
			m_retLineNumber = m_lineNumber;
		}
		else
		{
			token = { tokenStr, asmc::TokenType::STRING, asmc::UzTip::REG_8, m_lineNumber };
		}		
		
		break;

	case ENDOFFILE:
		token = { std::string(1,m_currentChar), asmc::TokenType::ENDOFFILE, asmc::UzTip::REG_8, m_lineNumber };
		break;

	default:
		printError("LEXER Default CASE! ");				
		break;
	}

	return token;
}

asmc::Token Lexer::lexWord()
{
	asmc::Token token;
	
	std::string tokenStr = getSubStr(m_position, 1);

	//closes debug info
	if (tokenStr == asmc_CLOSE_DEBUG_WORD)
	{
		token = { tokenStr, asmc::TokenType::DEBUG_TOKEN, asmc::UzTip::REG_8, m_lineNumber };
	}
	//check if tokenStr is a keyword(LOAD,XOR,AND, ...)
	else if (checkIfKeyword(tokenStr))
	{
		std::optional<TokenType> enumVal = magic_enum::enum_cast<TokenType>(tokenStr);
		token = { tokenStr, enumVal.value(), asmc::UzTip::REG_8, m_lineNumber };
	}
	else
	{
		//valid label examples LOOP: CLEAR: ...
		if (peek() == ':')
		{
			token = { tokenStr, asmc::TokenType::LABEL, asmc::UzTip::REG_8, m_lineNumber };
			nextChar();

		}
		//else [label] or [id], [id] used for func_name or define
		else
		{
			//if the lastToken is a jump instruction, next token should be a label
			//	loop: 
			//	JMP loop
			//lastToken => JMP		
			//loop => label
			if (m_lastToken.m_type >= asmc::TokenType::JMP &&
				m_lastToken.m_type <= asmc::TokenType::JUMP_INST)
			{
				token = { tokenStr, TokenType::LABEL, asmc::UzTip::REG_8, m_lineNumber };
			}
			else
			{
				token = { tokenStr, TokenType::ID, asmc::UzTip::REG_8, m_lineNumber };
			}
		}
	}

	return token;
}

asmc::Token Lexer::lexMacro()
{
	asmc::Token token;

	nextChar();

	std::string tokenStr = getSubStr(m_position, 1);

	if (checkIfKeyword(tokenStr))
	{
		std::optional<TokenType> enumVal = magic_enum::enum_cast<TokenType>(tokenStr);
		token = { tokenStr, enumVal.value(), asmc::UzTip::REG_8, m_lineNumber };
	}
	else
	{
		printError("unknown macro");
	}

	return token;
}

//--------------------------------------------//
//--------------------------------------------//
//--------------------------------------------//

bool Lexer::checkIfKeyword(std::string token)
{
	//checks str is it keyword or not
	std::optional<TokenType> tempToken = magic_enum::enum_cast<TokenType>(token);
	if (tempToken.has_value())
	{
		return true;
	}

	return false;
}

//is 0xfa
bool Lexer::isNumberHex()
{
	char peekoverX;

	if (m_position + 2 >= m_program.length())
	{
		peekoverX = ENDOFFILE;
	}
	else
	{
		peekoverX = m_program[m_position + 2];
	}
	

	if (m_currentChar == '0' && peek() == 'x' && std::isxdigit(static_cast<uint8_t>(peekoverX)))
	{
		return true;
	}

	return false;
}

bool Lexer::isOperand()
{
	//isxdigit() checks next char is it hex?
	//register ?
	if (m_currentChar == 'r' && std::isdigit(static_cast<uint8_t>(peek())))
	{
		return true;
	}
	//is 0xfa
	else if (isNumberHex())
	{
		return true;
	}

	return false;
}

//--------------------------------------------//
//--------------------------------------------//
//--------------------------------------------//
void Lexer::skipComments()
{
	if (m_currentChar == ';')
	{
		while (m_currentChar != '\n' && m_currentChar != ENDOFFILE)
		{
			nextChar();
		}	
	}

	//TODO aciklama satiri icerisinde / olunca hata veriyor duzelt bunu 
	if (m_currentChar == '/')
	{
		if (peek() == '*')
		{
			nextChar();// eat(*)
			nextChar();// eat(\n)
			while (m_currentChar != asmc::TokenType::ENDOFFILE)
			{
				if (m_currentChar == '*' && peek() == '/')
				{					
					nextChar(); // eat(*)
					nextChar(); // eat(/)
					break;
				}
				nextChar();
			}
			
			nextChar();// eat(\n)
		}
		else
		{
			printError("LEXER::Comment is not defined ::error ");

		}

	}
}

void Lexer::skipWhiteSpace()
{
	while (m_currentChar == ' ' || m_currentChar == '\t')
	{
		nextChar();
	}
}

//skip ',' 
void Lexer::skipNonEssential()
{
	while (m_currentChar == ',')
	{				
		nextChar();
	}
}

void Lexer::pushFile(std::string path)
{	
	for (size_t i = 0; i < m_inputStream.size(); i++)
	{
		if (m_inputStream[i].m_path == path)
		{
			std::cout << rang::fg::red 
					  << "ERROR::Same file name detected in inputStream"
					  << rang::style::reset
					  <<"\n";
			return;
		}
	}

	asmc::FileData& filed = m_inputStream.back();

	filed.m_currentChar = m_returnCurrentChar;
	filed.m_lastPosition = m_returnPosition;
	filed.m_lineNumber = m_lineNumber;

	m_inputStream.push_back({ path });
	

	m_program = readFile(path);
	m_currentChar = 0;
	m_lineNumber = 0;
	m_position = -1;

	nextChar();

	m_currentFileName = m_inputStream.back().m_path;

}

bool Lexer::popFile()
{	
	//get file name from top
	m_currentFileName = m_inputStream.back().m_path;

	//pop
	m_inputStream.pop_back();	

	if (m_inputStream.empty())
	{
		return true;
	}

	asmc::FileData filed;
	filed = m_inputStream.back();

	//open file
	m_program = readFile(filed.m_path, filed.m_lastPosition);
	m_currentChar = filed.m_currentChar;
	m_position = 0;
	m_lineNumber = m_retLineNumber;

	return false;
}

std::string Lexer::getCurrentFileName()
{
	return m_currentFileName;
}

std::string Lexer::getSubStr(int startPos, int length, bool upper)
{
	//allows label like
	//loop_t0, l0235235, l____t0 ....
	while (std::isalpha(peek()) || std::isdigit(peek()) || peek() == '_')
	{
		nextChar();
		length++;
	}

	std::string tokenStr = m_program.substr(startPos, length);

	if (upper)
	{
		toUpper(tokenStr);
	}
	
	return tokenStr;
}

void Lexer::toUpper(std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		str[i] = std::toupper(str[i]);
	}
}

//returns next char if pos >= program.length returns enum::EOF
void Lexer::nextChar()
{	
	m_position++;
	if (m_position >= m_program.length())
	{
		m_currentChar = ENDOFFILE;
	}
	else
	{
		m_currentChar = m_program[m_position];
	}

	//for printError
	if (m_currentChar == '\n')
	{
		m_lineNumber++;
	}
}

//--------------------------------------------//
//--------------------------------------------//
//--------------------------------------------//

void Lexer::printError(std::string message)
{
	std::cout << rang::fg::cyan
		<< "*****************************\n"
		<< "ERROR::Lexer::" << message		
		<< "current char[" << ((m_currentChar == '\n') ? "/n" : std::string(1, m_currentChar)) << "] "
		<< "current pos [" << std::to_string(m_position) << "] "
		<< "line number [" << m_lineNumber <<"]\n"
		<< "*****************************\n"
	<< rang::style::reset;

	m_errorCounter++;
	f_error = true;
}

bool Lexer::getDebugFlag()
{
	return fd_debug;
}

size_t Lexer::getErrorAmount()
{
	return m_errorCounter;
}

bool Lexer::isInputStreamEmpty()
{
	return m_inputStream.empty();
}

}