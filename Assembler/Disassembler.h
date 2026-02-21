#include <iostream>
#include <vector>

#include "Common.h"

namespace asmc
{
	class Dissambler
	{
	public:
		Dissambler();
		~Dissambler();

		void run(std::vector<asmc::MemoryLayout>& memlay);
	private:

	};	
}