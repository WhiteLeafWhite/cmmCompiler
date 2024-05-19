#pragma once
#include <iostream>
class Action
{
public:
	char act;
	int state;
	Action() :act('g'), state(114514) {};
	Action(char a, int nxt) :act(a), state(nxt) {};
	friend std::ostream& operator <<(std::ostream& os, const Action one) {
		os << one.act << one.state;
		return os;
	}
};

