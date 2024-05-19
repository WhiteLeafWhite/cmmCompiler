#include "TerminalSymbol.h"

bool TerminalSymbol::isTerminal()
{
    return true;
}

TerminalSymbol::TerminalSymbol(symbol s)
{
    data = s.data;
}
