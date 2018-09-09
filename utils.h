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

	template <typename T>
	struct Array
	{
		Array(size_t capacity) noexcept : data(new T[capacity]), count(0)  {}
		Array(const Array&) = delete;
		Array(Array&& arr) noexcept
		{
			data = arr.data;
			arr.data = nullptr;
			count = arr.count;
			arr.count = 0;
		}
		~Array() { delete[] data; }

		T& operator[](int index) const { return data[index]; }
		T& operator[](int index) { return data[index]; }
		T* data;
		size_t count;
	};
}

