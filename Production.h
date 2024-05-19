#pragma once
#include "symbol.h"
#include <vector>
#include "nonTerminalSymbol.h"
#include <iostream>
class Production
{
public:
	symbol* left;
	std::vector<symbol*> right;
	Production() :left(NULL){}
	Production(symbol* l, std::vector<symbol*>r) :left(l), right(r) {};
	friend std::ostream& operator << (std::ostream & os, const Production & one) {
		os << *one.left << " -> ";
		for (auto rs : one.right) {
			os << *rs;
		}
		return os;
	}
	friend bool operator == (const Production& one,const Production& other) {
		if (!(*one.left == *other.left)) return false;
		if (one.right.size() != other.right.size()) return false;
		for (int i = 0; i < one.right.size(); ++i) {
			if (one.right[i] != other.right[i]) return false;
		}
		return true;
	}

	friend bool operator < (const Production& one, const Production& other) {
		if (one.right.size() == other.right.size()) {
			for (int i = 0; i < one.right.size(); ++i) {
				if (one.right[i] < other.right[i]) return true;
				else if (one.right[i] > other.right[i]) return false;
			}
			return one.left < other.left;
		}
		return one.right.size() < other.right.size();
	}
};

