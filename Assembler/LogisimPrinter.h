#pragma once

#include <vector>
#include <fstream>

#include "utils/Radix.h"
#include "Common.h"

class LogisimPrinter
{
public:
	LogisimPrinter();
	~LogisimPrinter();

	void print(std::vector<asmc::MemoryLayout>& array);
private:

};