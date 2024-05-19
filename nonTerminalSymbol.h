#pragma once
#include "symbol.h"
class nonTerminalSymbol :
    public symbol
{
public:
    virtual bool isTerminal();
    nonTerminalSymbol(std::string data) :symbol(data) {};
    nonTerminalSymbol(symbol s);
};

