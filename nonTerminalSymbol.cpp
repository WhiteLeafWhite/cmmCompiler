#include "nonTerminalSymbol.h"

bool nonTerminalSymbol::isTerminal()
{
    return false;
}

nonTerminalSymbol::nonTerminalSymbol(symbol s)
{
    data = s.data;
}
