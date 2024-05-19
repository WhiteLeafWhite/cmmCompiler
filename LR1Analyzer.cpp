#include "LR1Analyzer.h"
#include<stack>
#include <algorithm>
#include <cstdio>
void LR1Analyzer::genFirst()
{
	bool flag = false;
	while (1) {
		flag = false;
		for (auto s : symbols) {
			if (first.count(s) == false) {
				first[s] = std::set<symbol*>();
			}
			//如果s是一个终结符，那么它的first集是它本身
			if (s->isTerminal() == true&& first[s].size()==0) {
				//std::cout << "初始化一个终结符的FIRST集" << std::endl;
				flag = true;
				first[s].insert(s);
			}
			else {
				for (auto p : productions) {
					//对于非终结符，考虑以它为左部的产生式
					if (*p.left == *s) {
						for (auto rightsymbol : p.right) {
							//遍历其右部的每个符号
							//求并集
							std::set<symbol*> united = first[s];
							for (std::set<symbol*>::iterator it = first[rightsymbol].begin();
								it != first[rightsymbol].end(); ++it) {
								united.insert(*it);
							}
							//如果有改变，应用改变然后标记
							if (united.size() > first[s].size()) {
								flag = true;
								first[s] = std::move(united);
							}
							//如果空包含在这个符号的first集中，那么下一个符号的first集也要并进去
							if (firstContainsEmpty(rightsymbol)) {
								if (rightsymbol == p.right.back()) {
									first[s].insert(empty);
								}
								continue;
							}
							else {
								break;
							}
						}
					}
				}
			}
		}
		//...直到没有变化为止，到此first集计算完了
		if (!flag) break;
	}
}

void LR1Analyzer::getFollow()
{
	//往里填
	bool flag = false;
	while (1) {
		flag = false;
		//遍历所有产生式
		for (auto p : productions) {
			for (int i = 0; i < p.right.size(); ++i) {
				//遇到一个非终结符
				if (p.right[i]->isTerminal() == false) {
					std::set<symbol*> temp=getFirstStr(p.right,i+1);
					//情况一
					for (auto s : temp) {
						if (s == empty) continue;
						if (!follow[p.right[i]].count(s)) flag = true;
						follow[p.right[i]].insert(s);
					}
					//情况二
					if (temp.count(empty)||i==p.right.size()-1) {
						for (auto s : follow[p.left]) {
							if (!follow[p.right[i]].count(s)) flag = true;
							follow[p.right[i]].insert(s);
						}
					}
				}
			}
		}
		if (!flag) break;
	}
}

void LR1Analyzer::getProject()
{
	int num = 0;
	for (auto prod : productions) {
		Production paim = prod;
		std::vector<symbol*> temp;
		for (auto s : prod.right) {
			if (s == empty) continue;
			temp.push_back(s);
		}
		paim.right = temp;
		for (int i = 0; i < temp.size();++i) {
			projects.push_back(Project(num++,paim,i));
		}
		projects.push_back(Project(num++,paim, temp.size()));
	}
}

std::set<Project> LR1Analyzer::getclosures(const std::set<Project>& I)
{
	std::set<Project>res = I;
	while (1) {
		bool flag = false;
		for (auto p : res) {
			if (p.dot == p.production.right.size()) continue;//这是一个规约项目，不会有和它等价的
			symbol* aim = p.production.right[p.dot];
			if (aim->isTerminal()) continue;
			for (int i = 0; i < projects.size(); ++i) {
				if (aim == projects[i].production.left&&projects[i].dot==0) {
					if (!res.count(projects[i])) {
						res.insert(projects[i]);
						flag = true;
					}
				}
			}
		}
		if (!flag) break;
	}
	return res;
}

std::set<Project> LR1Analyzer::GOTO(const std::set<Project>& I, symbol* x)
{
	std::set<Project> res;
	for (auto i : I) {
		if (i.dot < i.production.right.size() && i.production.right[i.dot] == x) {
			Project temp = i;
			temp.dot++;
			res.insert(temp);
		}
	}
	return getclosures(res);
}

bool LR1Analyzer::firstContainsEmpty(symbol* s)
{
	for (auto sym : first[s]) {
		if (sym == empty) return true;
	}
	return false;
}

LR1Analyzer::LR1Analyzer()
{
	offset = 0;
	nextInstr = 100;
	for (auto s : symbols) {
		follow[s] = std::set<symbol*>();
	}
}

//求一个串的FIRST集
std::set<symbol*> LR1Analyzer::getFirstStr(std::vector<symbol*> str, int index)
{
	std::set<symbol*> res;
	for (int i = index; i < str.size(); ++i) {
		symbol* now = str[i];
		for (auto s : first[now]) {
			if (*s == *empty) continue;
			res.insert(s);
		}
		if (!firstContainsEmpty(now)) break;
		else if (i == str.size() - 1) {
			res.insert(empty);
		}
	}
	return res;
}

void LR1Analyzer::addProduction(symbol* ll, std::vector<symbol*> rr)
{
	productions.push_back(Production(ll, rr));
}

void LR1Analyzer::addSymbol(symbol* s)
{
	symbols.push_back(s);
	if (s->data == "empty") {
		empty = s;
	}
	else if (s->data == "START") {
		START = s;
	}
	else if (s->data == "$") {
		follow[START].insert(s);
		END = s;
	}
}

std::string LR1Analyzer::lookup(int n) {
	return initToken[n].lexeme;
}

std::string LR1Analyzer::newtemp()
{
	std::string res = "t";
	static int tnum = 0;
	res = res + std::to_string(tnum++);
	return res;
}

void LR1Analyzer::backpatch(std::string str, std::string aim)
{
	while (str.find("#") != std::string::npos) {
		std::string temp = str.substr(0, str.find("#"));
		str = str.substr(str.find("#") + 1);
		next[std::stoi(temp)-100] = std::stoi(aim);
		if (middle_res[std::stoi(temp) - 100] == "goto") middle_res[std::stoi(temp) - 100] += aim;
	}
	next[std::stoi(str)-100] = std::stoi(aim);
	if (middle_res[std::stoi(str) - 100] == "goto") middle_res[std::stoi(str) - 100] += aim;
}

void LR1Analyzer::productionAction(int n,int read_head)
{
	if (n == 2) {//D -> L id ; D
		//std::cout <<"read_head:" << read_head << std::endl;
		infos.pop();
		infos.pop();
		std::string idReal = infos.top();
		infos.pop();
		idTable.push_back(idInfo(infos.top(), idReal,offset));
		//std::cout << "向符号表中添加" << infos.top() << "类型的" << idReal << "，他的地址是" << offset << std::endl;
		infos.pop();
		infos.push("[]");
		offset++;
	}
	else if (n == 3) {
		infos.push("[]");
	}
	else if (n == 4) {
		infos.pop();
		infos.push("int");
	}
	else if (n == 5) {
		infos.pop();
		infos.push("float");
	}
	else if (n == 6) {//s -> id = E;
		infos.pop();
		std::string e2 = infos.top();
		infos.pop();
		infos.pop();
		std::string e1 = infos.top();
		infos.pop();
		infos.push(std::to_string(nextInstr));
		middle_res.push_back(e1 + " = " + e2);
		next.push_back(-1);
		nextInstr++;
	}
	else if (n == 7) {//S -> if(B) M1 S1
		std::string s1NextList = infos.top();
		infos.pop();//s1
		std::string m1instr = infos.top();
		infos.pop();//m1
		infos.pop();//)
		int b = infos.top().find('#');
		std::string bTrue = infos.top().substr(0, b);
		std::string bFalse = infos.top().substr(b + 1);
		infos.pop();//B
		infos.pop();//(
		infos.pop();//if
		middle_res[std::stoi(bTrue) - 100] += m1instr;
		next.push_back(-1);
		infos.push(bFalse + "#" + s1NextList);
	}
	else if(n==8){//S -> if(B) M1 S1 N else M2 S2
		std::string s2NextList=infos.top();
		infos.pop();//s2 out
		std::string m2instr = infos.top();
		infos.pop();//m2 out
		infos.pop();
		std::string nNextList = infos.top();
		infos.pop();//n
		std::string s1NextList = infos.top();
		infos.pop();//s1
		std::string m1instr = infos.top();
		infos.pop();//m1
		infos.pop();//)
		int b = infos.top().find('#');
		std::string bTrue = infos.top().substr(0, b);
		std::string bFalse = infos.top().substr(b+1);
		infos.pop();//B
		infos.pop();//(
		infos.pop();//if
		middle_res[std::stoi(bTrue)-100]+=m1instr;
		middle_res[std::stoi(bFalse)-100] += m2instr;
		next.push_back(-1);
		next.push_back(-1);
		infos.push(s2NextList + "#" + nNextList + "#" + s1NextList);
	}
	else if (n == 10) {//S -> S1 M S2
		std::string s2Nextlist = infos.top();
		infos.pop();
		std::string mInstr = infos.top();
		infos.pop();
		std::string s1Nextlist = infos.top();
		infos.pop();
		backpatch(s1Nextlist, mInstr);
		infos.push(s2Nextlist);
	}
	else if (n == 11) {//C -> E > E
		std::string e2 = infos.top();
		infos.pop();
		infos.pop();
		std::string e1 = infos.top();
		infos.pop();
		infos.push(std::to_string(nextInstr) + "#" + std::to_string(nextInstr + 1));
		std::string res = e1 + ">" + e2;
		middle_res.push_back("if " + res + " goto");
		nextInstr++;
		middle_res.push_back("goto");
		nextInstr++;
		next.push_back(-1);
		next.push_back(-1);
	}
	else if (n == 12) {//C -> E < E
		std::string e2 = infos.top();
		infos.pop();
		infos.pop();
		std::string e1 = infos.top();
		infos.pop();
		infos.push(std::to_string(nextInstr) + "#" + std::to_string(nextInstr + 1));
		std::string res = e1 + "<" + e2;
		middle_res.push_back("if " + res + " goto");
		nextInstr++;
		middle_res.push_back("goto");
		nextInstr++;
		next.push_back(-1);
		next.push_back(-1);
	}
	else if (n == 13) {//C -> E == E
		std::string e2 = infos.top();
		infos.pop();
		infos.pop();
		std::string e1 = infos.top();
		infos.pop();
		infos.push(std::to_string(nextInstr) + "#" + std::to_string(nextInstr + 1));
		std::string res = e1 + "==" + e2;
		middle_res.push_back("if " + res + " goto");
		nextInstr++;
		middle_res.push_back("goto");
		nextInstr++;
		next.push_back(-1);
		next.push_back(-1);
	}
	else if (n == 14) {
		std::string nt = newtemp();
		std::string e1 = infos.top();
		infos.pop();
		infos.pop();
		std::string e2 = infos.top();
		infos.pop();
		infos.push(nt);
		middle_res.push_back(nt + " = " + e1 + " + " + e2);
		nextInstr++;
		next.push_back(-1);
	}
	else if (n == 15) {
		std::string nt = newtemp();
		std::string e1 = infos.top();
		infos.pop();
		infos.pop();
		std::string e2 = infos.top();
		infos.pop();
		infos.push(nt);
		middle_res.push_back(nt + " = " + e1 + " - " + e2);
		nextInstr++;
		next.push_back(-1);
	}
	else if (n == 20 || n == 21) {//F -> id

	}
	else if (n == 22) {//F -> dights
		//std::cout << "值是" << infos.top() << std::endl;
	}
	else if (n == 23) {
		infos.push(std::to_string(nextInstr));
	}
	else if (n == 24) {
		infos.push(std::to_string(nextInstr));
		middle_res.push_back("goto");
		nextInstr++;
		next.push_back(-1);
	}
	/*std::stack<std::string> temp;
	while (infos.size()) {
		temp.push(infos.top());
		infos.pop();
	}
	while (temp.size()) {
		infos.push(temp.top());
		std::cout << temp.top()<<" ";
		temp.pop();
	}
	std::cout << std::endl;*/
}

void LR1Analyzer::init()
{
	genFirst();
	getFollow();
	for (auto f : follow) {
		std::cout << *f.first << " :" << std::endl;
		for(auto s:f.second)
			std::cout<< *s << " ";
		std::cout << std::endl;
	}
	getProject();
	getC();
	getTable();
	int i = 0;
	for (auto c : Collections) {
		std::cout << i++ << std::endl;
		for (auto p : c) {
			std::cout << p << std::endl;
		}
		std::cout << std::endl;
	}
	
	printf_s("    |");
	for (auto s : symbols) {
		printf_s("%-4s|", s->data.c_str());
	}
	std::cout << std::endl;
	for (int i = 0; i < Collections.size(); ++i) {
		printf_s("%-4d|",i);
		for (auto s : symbols) {
			if (!s->isTerminal()) {
				if (goTo.count(i) && goTo[i].count(s)) {
					printf_s("%-4d|", goTo[i][s]);
				}
				else {
					printf_s("    |");
				}
			}
			else {
				if (action.count(i) && action[i].count(s)) {
					printf_s("%c%-3d|", action[i][s].act, action[i][s].state);
				}
				else {
					printf_s("    |");
				}
			}
		}
		std::cout << std::endl;
	}
}

void LR1Analyzer::startAnalyze(const std::vector<Token>& ts)
{
	getTokens(ts);
	std::stack<symbol*> input_s;
	std::stack<int> state_s;
	state_s.push(0);
	//读头
	int i = 0;
	while (inputSymbol.size()) {
		//std::cout << "读头" << *inputSymbol[i] << std::endl;
		//查表
		if (!action.count(state_s.top()) || !action[state_s.top()].count(inputSymbol[i])) {
			std::cout << "error" << std::endl;
			break;
		}
		Action actType = action[state_s.top()][inputSymbol[i]];
		if (actType.act == 'r') {
			std::cout << productions[actType.state] << std::endl;
			productionAction(actType.state,i);
			if (!(productions[actType.state].right[0] == empty)) {
				//弹栈
				for (int i = 0; i < productions[actType.state].right.size(); ++i) {
					input_s.pop();
					state_s.pop();
				}
			}
			//规约
			///std::cout << *productions[actType.state].left << "进栈" << std::endl;
			input_s.push(productions[actType.state].left);
			//GOTO
			
			//std::cout << "栈顶状态" << state_s.top() << std::endl;
			//std::cout << "goto" << goTo[state_s.top()][input_s.top()] << std::endl;
			state_s.push(goTo[state_s.top()][input_s.top()]);
			
		}
		else if(actType.act == 's'){
			//std::cout << actType << std::endl;
			if (inputSymbol[i]->isTerminal()) infos.push(lookup(i));
			else infos.push("[]");
			input_s.push(inputSymbol[i++]);
			state_s.push(actType.state);
		}
		else if (actType.act == 'a') {
			break;
		}
	}
	int hangshu = 100;
	for (auto i : middle_res) {
		std::cout <<hangshu++<<":    " << i << std::endl;
	}
	for (auto i : idTable) {
		std::cout << i.type << " " << i.name << " " << i.addr << std::endl;
	}
}

void LR1Analyzer::getTokens(const std::vector<Token>& ts) {
	initToken = ts;
	for (auto t : ts) {
		if (t.type == TOK_ID) {
			for (auto s : symbols) {
				if ("id" == s->data) {
					inputSymbol.push_back(s);
				}
			}
		}
		else if (t.type == TOK_DIGITS) {
			for (auto s : symbols) {
				if ("digits" == s->data) {
					inputSymbol.push_back(s);
				}
			}
		}
		else {
			for (auto s : symbols) {
				if (t.lexeme == s->data) {
					inputSymbol.push_back(s);
				}
			}
		}
	}
	inputSymbol.push_back(END);
	for (auto i : inputSymbol) std::cout << *i << " ";
	std::cout << std::endl;
}



void LR1Analyzer::getC()
{
	std::set<std::set<Project>> collections;
	collections.insert(getclosures(std::set<Project>({ projects[0] })));
	while (1) {
		bool flag = false;
		for (auto I : collections) {
			for (auto s : symbols) {
				std::set<Project> temp = GOTO(I, s);
				if (collections.count(temp)) continue;
				collections.insert(temp);
				flag = true;
			}
		}
		if (!flag) break;
	}
	for (auto c : collections) {
		if (c.empty()) continue;
		Collections.push_back(c);
	}
}

void LR1Analyzer::getTable()
{
	int flag = 0;
	for (auto p : projects) {
		for (int i = 0; i < Collections.size(); ++i) {
			std::set<Project> s=Collections[i];
			if (s.count(p)) {
				//GOTO and ACTION
				if (p.dot < p.production.right.size()) {
					symbol* temp = p.production.right[p.dot];
					if (!temp->isTerminal()) {
						std::set<Project> aim = GOTO(s, temp);
						for (int j = 0; j < Collections.size(); ++j) {
							if (Collections[j] == aim) {
								goTo[i][temp] = j;
								break;
							}
						}
					}
					else {
						std::set<Project> aim = GOTO(s, temp);
						for (int j = 0; j < Collections.size(); ++j) {
							if (Collections[j] == aim) {
								if (action.count(i) && action[i].count(temp)) {
									std::cout << action[i][temp] << "will also exist in" << i << " " << *temp << std::endl;
								}
								action[i][temp] = Action('s',j);
								break;
							}
						}
					}
				}
				else if (p.dot == p.production.right.size()&&p.production.left!=START) {
					symbol* A = p.production.left;
					for (auto a : follow[A]) {
						for (int j = 0; j < productions.size(); ++j) {
							Project temp=p;
							if (temp.production.right.size() == 0) {
								temp.production.right.push_back(empty);
							}
							if (productions[j] == temp.production) {
								action[i][a] = Action('r', j);
								break;
							}
						}
					}
				}
				else if(p.dot == p.production.right.size() && p.production.left == START){
					action[i][END]=Action('a', 666);
				}
			}
		}
	}
}