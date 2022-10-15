//.h file code:
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>  

class Reader
{
private:
	int index = 0;
	std::vector<unsigned char> Data;
public:
	Reader(std::vector<unsigned char>& Data);

	std::vector<unsigned char> Read();

	std::string ReadString();

	float ReadFloat();

	int ReadInt();

	std::string PeekString();

	float PeekFloat();

	int PeekInt();

	auto from_vector_float(const std::vector<unsigned char>& data)
	{
		float f;
		// make sure the vector is the right size
		if (data.size() != sizeof(f))
			throw std::runtime_error{ "Size of data in vector and float do not match" };
		// copy the bytes into the float
		std::memcpy(&f, data.data(), sizeof(f));
		return f;
	}

	bool ReadBool();
};
