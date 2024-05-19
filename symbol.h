#pragma once
#include<string>
#include<iostream>
class symbol
{
private:
	
public:
	std::string data;
	virtual bool isTerminal();
	symbol() {};
	symbol(std::string d) :data(d) {};
	friend std::ostream& operator <<(std::ostream & os,const symbol one) {
		os << one.data;
		return os;
	}
	bool operator == (const symbol &other) {
		return data == other.data;
	}
};

