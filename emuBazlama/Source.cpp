#include "pch.h"

#include "Emu.h"

#include <bitset>

uint32_t decToIEEE754_32(float decf)
{
	uint32_t result;
	std::memcpy(&result, &decf, sizeof(result));
	return result;
}

uint32_t decToIEEE754_32(std::string decf)
{
	uint32_t result;

	float value = std::stof(decf);

	std::memcpy(&result, &value, sizeof(result));
	return result;
}

//uint32_t decToIEEE754_32(std::string dec)
//{
//	size_t dotPos = dec.find('.');
//
//	std::string valbin = rdx::decToBin(std::stoi(dec.substr(0, dotPos)));
//
//	std::cout << "valbin " << valbin << "\n";
//
//	float fractPart = std::stof(dec.substr(dotPos, dec.size()));
//
//	std::string fractStr = "";
//	for (int i = 0; i < 22 - dec.substr(dotPos, dec.size()).size(); i++)
//	{
//		fractPart *= 2;
//
//		if (fractPart >= 1.0f)
//		{
//			fractStr += "1";
//			fractPart = fractPart - 1.0f;
//		}
//		else
//		{
//			fractStr += "0";
//		}		
//	}	
//
//	std::string result_FractPart = valbin + "." + fractStr;
//	std::string mantissa = valbin.substr(1, valbin.size()) + fractStr;
//
//	dotPos = result_FractPart.find('.');	
//
//	std::string exponent = rdx::decToBin(dotPos - 1 + 127);
//
//	std::cout << "result " << exponent << " " << result_FractPart << "\n";
//	
//
//	std::cout << "0" << exponent << mantissa << "\n";
//
//	return 0;
//}

int main()
{	

	baz::Emu emu("emu.txt");

	emu.run();

	/*std::string val = "18.75";

	

	std::cout << std::bitset<32>(decToIEEE754_32(18.75f)).to_string() << "\n";
	std::cout << std::bitset<32>(decToIEEE754_32("18.75")).to_string();*/

	/*std::string valbin = rdx::decToBin(std::stoi(val.substr(0, 2)));

	std::cout << valbin << "\n";

	float fractPart = std::stof(val.substr(2, 3));

	std::cout << "::" << fractPart << "\n";
	std::string fractStr = "";
	for(int i = 0; i < 23; i++)
	{
		fractPart *= 2;

		if (fractPart >= 1.0f)
		{
			fractStr += "1";
		}
		else
		{
			fractStr += "0";
		}

		fractPart = fractPart - 1.0;		
	}

	std::cout << "fracpart = " << fractStr << "\n";		

	std::string result_FractPart = valbin + "." + fractStr;

	size_t dotPos = result_FractPart.find('.');

	std::cout << "result " << result_FractPart << "\n";

	std::cout << dotPos - 1<< "\n";

	std::cout << "expo" << rdx::decToBin(dotPos - 1 + 127) << "\n";*/
}