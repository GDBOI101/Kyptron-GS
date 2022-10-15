#pragma once

#include <string>
#include <vector>

class Writer
{
private:
	std::vector<unsigned char> Bytes = std::vector<unsigned char>();

public:
	void Write(std::vector<unsigned char>& Data);
	void WriteString(std::string Data);

	void Writefloat(float Data);

	void Writeint(int Data);

	void Writebool(bool Data);

	std::vector<unsigned char> Dump();
};

