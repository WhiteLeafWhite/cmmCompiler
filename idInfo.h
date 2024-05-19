#pragma once
#include<string>
class idInfo
{
public:
	std::string type;
	std::string name;
	int addr;
	idInfo(std::string tt, std::string nn, int aa) :type(tt), name(nn), addr(aa) {}
};

