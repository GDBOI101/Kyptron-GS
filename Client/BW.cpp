#pragma once

#include "BW.h"
void Writer::Write(std::vector<unsigned char>& Data)
{
	for (auto Byte : Data)
	{
		Bytes.push_back(Byte);
	}

	Bytes.push_back(0x0);
}

void Writer::WriteString(std::string Data)
{
	std::vector<unsigned char> chars;
	for (char c : Data)
	{
		chars.push_back(c);
	}
	this->Write(chars);
}

void Writer::Writefloat(float Data)
{
	std::vector<unsigned char> chars;
	for (char c : std::to_string(Data))
	{
		chars.push_back(c);
	}

	this->Write(chars);
}

void Writer::Writeint(int Data)
{
	std::vector<unsigned char> chars;
	for (char c : std::to_string(Data))
	{
		chars.push_back(c);
	}


	this->Write(chars);
}

void Writer::Writebool(bool Data)
{
	if (Data)
	{
		WriteString("True");
	}

	WriteString("False");

}

std::vector<unsigned char> Writer::Dump()
{
	return Bytes;
}
