#pragma once
#include"symbol.h"
class TerminalSymbol:public symbol
{
public:
	virtual bool isTerminal();
	TerminalSymbol(std::string data) :symbol(data){};
	TerminalSymbol(symbol s);
};

