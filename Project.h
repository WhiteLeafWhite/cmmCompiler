#pragma once
#include "Production.h"
#include "symbol.h"
class Project
{
public:
	int id;
	Production production;
	int dot;
	Project():id(-1),dot(-1),production(Production::Production()){};
	Project(int ii,Production pp, int dd) :id(ii),production(pp), dot(dd){};
	friend bool operator == (const Project& one,const Project& other) {
		if (one.dot != other.dot) return false;
		if (!(one.production == other.production)) return false;
		return true;
	}
	friend bool operator < (const Project&one,const Project& other) {
		if (one.production == other.production) {
			return one.dot < other.dot;
		}
		else {
			return one.production < other.production;
		}
	}
	friend std::ostream& operator <<(std::ostream& os, const Project& one) {
		os << *one.production.left << " -> ";
		if (one.production.right.size() == 0) os << ".";
		else {
			for (int i = 0; i < one.production.right.size(); ++i) {
				if (one.dot == i) os << ".";
				os << *one.production.right[i];
			}
		}
		return os;
	}
};

