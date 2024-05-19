#pragma once
#include<map>
#include<vector>
#include<set>
#include<stack>
#include"symbol.h"
#include"TerminalSymbol.h"
#include"Production.h"
#include"Project.h"
#include "TokenType.h"
#include"Action.h"
#include "idInfo.h"
class LR1Analyzer
{
private:
	std::vector<symbol*> symbols;
	std::map<symbol*, std::set<symbol*>> first;
	std::map<symbol*, std::set<symbol*>> follow;
	std::vector<Production> productions;
	std::vector<Project> projects;
	std::vector<std::set<Project>> Collections;//Ïî¼¯×å
	std::map<int, std::map<symbol*, Action>> action;
	std::map<int, std::map<symbol*, int>> goTo;
	std::vector<idInfo> idTable;
	std::stack<std::string> infos;
	int offset,nextInstr;
	void genFirst();
	void getFollow();
	void getProject();
	void getC();
	void getTable();
	std::vector<std::string> middle_res;
	std::set<Project> getclosures(const std::set<Project> &I);
	std::set<Project> GOTO(const std::set<Project>& I, symbol* x);
	bool firstContainsEmpty(symbol* s);
	std::set<symbol*> getFirstStr(std::vector<symbol*> str, int index);
	symbol *empty,*START,*END;
	std::vector<symbol*> inputSymbol;
	std::vector<Token> initToken;
	void getTokens(const std::vector<Token>& ts);
	void productionAction(int n,int read_head);
	std::string lookup(int n);
	std::string newtemp();
	void backpatch(std::string str, std::string aim);
	std::vector<int> next;
public:
	LR1Analyzer();
	void addProduction(symbol* ll, std::vector<symbol*>rr);
	void addSymbol(symbol* s);
	void init();
	void startAnalyze(const std::vector<Token>& ts);
};

