#include "LogisimPrinter.h"

LogisimPrinter::LogisimPrinter()
{
}

LogisimPrinter::~LogisimPrinter()
{
}

void LogisimPrinter::print(std::vector<asmc::MemoryLayout>& array)
{
	std::ofstream file("logisimRAM_output.txt");

	file << "v3.0 hex words addressed\n";

	file << std::hex;

	size_t row;
	size_t col;

	for (size_t i = 0; i < array.size(); i++)
	{	
		size_t address = array[i].m_ramIndex;
		
		//write hex address
		file << rdx::decToHex(address) << ": ";


		//write hex values to the file
		//----------------------------------------------//
		//----------------------------------------------//
		file << array[i].m_opcode << "\n";
		
		if (array[i].m_packetSize == 2)
		{	
			address += 1;
						
			//write hex address
			file << rdx::decToHex(address) << ": ";			

			file << array[i].m_secondPart << " ";
			file << "\n";			
		}
		//----------------------------------------------//
		//----------------------------------------------//
		
	}

	file.close();
}
