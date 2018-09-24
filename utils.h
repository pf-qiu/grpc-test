#pragma once

#include <string>
#include <random>

namespace GeneralUtils
{
	class IDGenerator
	{
	public:
		IDGenerator();
		std::string Next();
	private:
		std::default_random_engine e;
	};
}

