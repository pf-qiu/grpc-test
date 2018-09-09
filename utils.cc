#include "utils.h"

using namespace std;
namespace GeneralUtils
{
	static const char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	IDGenerator::IDGenerator() : e(random_device()())
	{
	}


	void FillChar(unsigned char data, string& id)
	{
		id.push_back(hexTable[data >> 4]);
		id.push_back(hexTable[data & 0xF]);
	}
	void FillInt(unsigned int data, string& id)
	{
		FillChar(data & 0xFF, id);
		data >>= 8;
		FillChar(data & 0xFF, id);
		data >>= 8;
		FillChar(data & 0xFF, id);
		data >>= 8;
		FillChar(data & 0xFF, id);
	}

	string IDGenerator::Next()
	{
		string id;
		id.reserve(20);
		FillInt(e(), id);
		id.push_back('-');
		FillInt(e(), id);
		id.push_back('-');
		FillInt(e(), id);
		id.push_back('-');
		FillInt(e(), id);
		return id;
	}
}