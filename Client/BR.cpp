#pragma once
#include "BR.h"
#include <algorithm>

Reader::Reader(std::vector<unsigned char>& Data)
{
	this->Data = Data;
}

std::vector<unsigned char> Reader::Read()
{
	int index_ = 0;
	std::vector<unsigned char> Bytes;
	for (auto Byte : Data)
	{
		if (Byte == 0)
		{
			if (index != index_)
			{
				index_++;
			}
			else
			{
				index++;
				return Bytes;
			}
		}
		else if (index == index_)
		{
			Bytes.push_back(Byte);
		}
	}
	return std::vector<unsigned char>();
}

std::string Reader::ReadString()
{
	auto Bytes = Read();

	std::string s;
	std::transform(Bytes.begin(), Bytes.end(), std::back_inserter(s),
		[](char c)
		{
			return c;
		});

	return s;
}

float Reader::ReadFloat()
{

	float f = std::stof(ReadString());

	return f;
}

int Reader::ReadInt()
{
	int i2 = std::stoi(ReadString());
	return i2;
}

std::string Reader::PeekString()
{
	auto Return = ReadString();
	index--;
	return Return;
}

float Reader::PeekFloat()
{
	auto Return = ReadFloat();
	index--;
	return Return;
}

int Reader::PeekInt()
{
	auto Return = ReadInt();
	index--;
	return Return;
}

bool Reader::ReadBool()
{
	auto string = ReadInt();

	if (string == 1)
	{
		return true;
	}

	return false;
}