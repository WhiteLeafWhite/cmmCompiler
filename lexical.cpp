#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include "LR1Analyzer.h"

using namespace std;

// Token类型枚举


// Token结构体


// 判断字符是否是运算符
bool isOperator(char c) {
    string operators = "+-*/%=><;'!";
    return operators.find(c) != string::npos;
}

// 判断字符是否是分隔符
bool isSeparator(char c) {
    string separators = "();";
    return separators.find(c) != string::npos;
}

// 判断字符串是否是关键字
bool isKeyword(const string& str) {
    vector<string> keywords = { "int", "float", "double", "if", "else", "for", "while", "return" };
    return find(keywords.begin(), keywords.end(), str) != keywords.end();
}

// 分析源代码生成Token序列
vector<Token> tokenize(const string& code) {
    vector<Token> tokens;
    string currentLexeme;
    TokenType currentType = TOK_UNKNOWN;

    for (char c : code) {
        if (isspace(c)) {
            if (!currentLexeme.empty()) {
                Token token = { currentType, currentLexeme };
                tokens.push_back(token);
                currentLexeme.clear();
                currentType = TOK_UNKNOWN;
            }
            continue;
        }
        else if (isalpha(c)) {
            if (currentType == TOK_OPERATOR || currentType == TOK_SEPARATOR || currentType == TOK_KEYWORD || currentType == TOK_DIGITS) {
                Token token = { currentType, currentLexeme };
                tokens.push_back(token);
                currentLexeme.clear();
                currentType = TOK_UNKNOWN;
            }
            currentLexeme += c;
            currentType =  TOK_ID;
        }
        else if (isdigit(c)) {
            if (!currentLexeme.empty() && currentType != TOK_DIGITS) {
                Token token = { currentType, currentLexeme };
                    tokens.push_back(token);
                    currentLexeme.clear();
                    currentType = TOK_UNKNOWN;
            }
            currentLexeme += c;
            currentType = TOK_DIGITS;
        }
        else if (isOperator(c)) {
            if (!currentLexeme.empty()&&currentType != TOK_OPERATOR) {
                Token token = { currentType, currentLexeme };
                tokens.push_back(token);
                currentLexeme.clear();
                currentType = TOK_UNKNOWN;
            }
            currentLexeme += c;
            currentType = TOK_OPERATOR;
        }
        else if (isSeparator(c)) {
            if (!currentLexeme.empty()) {
                Token token = { currentType, currentLexeme };
                tokens.push_back(token);
                currentLexeme.clear();
                currentType = TOK_UNKNOWN;
            }
            currentLexeme += c;
            currentType = TOK_SEPARATOR;
        }
        else {
            if (!currentLexeme.empty()&&currentType!=TOK_ERROR) {
                Token token = { currentType, currentLexeme };
                tokens.push_back(token);
                currentLexeme.clear();
                currentType = TOK_UNKNOWN;
            }
            currentLexeme += c;
            currentType = TOK_ERROR;
        }
    }

    if (!currentLexeme.empty()) {
        Token token = { currentType, currentLexeme };
        tokens.push_back(token);
    }

    return tokens;
}

std::string remove_comments(const std::string& code) {
    bool in_comment_block = false;
    bool in_string = false;
    std::string result;

    size_t i = 0;
    while (i < code.length()) {
        if (!in_comment_block && code.substr(i, 2) == "/*" && !in_string) {
            in_comment_block = true;
            i += 2;
        }
        else if (in_comment_block && code.substr(i, 2) == "*/" && !in_string) {
            in_comment_block = false;
            i += 2;
        }
        else if (!in_comment_block && code[i] == '"' && (i == 0 || code[i - 1] != '\\')) {
            in_string = !in_string;
            result += code[i];
            i++;
        }
        else if (!in_comment_block && code.substr(i, 2) == "//" && !in_string) {
            // Skip single-line comment
            while (i < code.length() && code[i] != '\n') {
                i++;
            }
        }
        else if (!in_comment_block) {
            result += code[i];
            i++;
        }
        else {
            i++;
        }
    }

    return result;
}

LR1Analyzer lr1Analyzer;

int main() {
    string code =
        "int a; \n\
         int b;\n\
         float c;\n\
         a=2;//123\n\
         b = 1;\n\
         if(a>b)\n\
         c=a+b;\n\
         else\n\
         c=a-b;\n\
         if(c==0)\n\
         b=2;\n\
         if(b<1)\n\
         a=12;";

    code = remove_comments(code);
    std::cout << code << std::endl;
    vector<Token> tokens = tokenize(code);
    for (int i = 0; i < tokens.size();++i) {
        if (isKeyword(tokens[i].lexeme)) {
            tokens[i].type = TOK_KEYWORD;
        }
        std::cout << tokens[i].lexeme << " ";
    }
    std::cout << "www" << std::endl;
    //声明symbols
    symbol* START = new nonTerminalSymbol("START");
    lr1Analyzer.addSymbol(START);
    symbol* P = new nonTerminalSymbol("P"); 
    lr1Analyzer.addSymbol(P);
    symbol* D = new nonTerminalSymbol("D"); 
    lr1Analyzer.addSymbol(D);
    symbol* L = new nonTerminalSymbol("L"); 
    lr1Analyzer.addSymbol(L);
    symbol* S = new nonTerminalSymbol("S"); 
    lr1Analyzer.addSymbol(S);
    symbol* C = new nonTerminalSymbol("C"); 
    lr1Analyzer.addSymbol(C);
    symbol* E = new nonTerminalSymbol("E");
    lr1Analyzer.addSymbol(E);
    symbol* T = new nonTerminalSymbol("T"); 
    lr1Analyzer.addSymbol(T);
    symbol* F = new nonTerminalSymbol("F"); 
    lr1Analyzer.addSymbol(F);
    symbol* id = new TerminalSymbol("id"); 
    lr1Analyzer.addSymbol(id);
    symbol* semicolom = new TerminalSymbol(";"); 
    lr1Analyzer.addSymbol(semicolom);
    symbol* empty = new TerminalSymbol("empty"); 
    lr1Analyzer.addSymbol(empty);
    symbol* my_int = new TerminalSymbol("int"); 
    lr1Analyzer.addSymbol(my_int);
    symbol* my_float = new TerminalSymbol("float"); 
    lr1Analyzer.addSymbol(my_float);
    symbol* equal = new TerminalSymbol("="); 
    lr1Analyzer.addSymbol(equal);
    symbol* twoequal = new TerminalSymbol("==");
    lr1Analyzer.addSymbol(twoequal);
    symbol* leftParenthesis = new TerminalSymbol("("); 
    lr1Analyzer.addSymbol(leftParenthesis);
    symbol* rightParenthesis = new TerminalSymbol(")"); 
    lr1Analyzer.addSymbol(rightParenthesis);
    symbol* my_while = new TerminalSymbol("while"); 
    lr1Analyzer.addSymbol(my_while);
    symbol* my_if = new TerminalSymbol("if"); 
    lr1Analyzer.addSymbol(my_if);
    symbol* my_else = new TerminalSymbol("else"); 
    lr1Analyzer.addSymbol(my_else);
    symbol* bigger = new TerminalSymbol(">"); 
    lr1Analyzer.addSymbol(bigger);
    symbol* smaller = new TerminalSymbol("<"); 
    lr1Analyzer.addSymbol(smaller);
    symbol* plus = new TerminalSymbol("+"); 
    lr1Analyzer.addSymbol(plus);
    symbol* minus = new TerminalSymbol("-"); 
    lr1Analyzer.addSymbol(minus);
    symbol* multiply = new TerminalSymbol("*"); 
    lr1Analyzer.addSymbol(multiply);
    symbol* div = new TerminalSymbol("/"); 
    lr1Analyzer.addSymbol(div);
    symbol* digits = new TerminalSymbol("digits");
    lr1Analyzer.addSymbol(digits);
    symbol* end = new TerminalSymbol("$");
    lr1Analyzer.addSymbol(end);
    symbol* M = new nonTerminalSymbol("M");
    lr1Analyzer.addSymbol(M);
    symbol* N = new nonTerminalSymbol("N");
    lr1Analyzer.addSymbol(N);
    lr1Analyzer.addProduction(START, vector<symbol*>({ P }));               //0
    lr1Analyzer.addProduction(P, vector<symbol*>({ D,M,S }));                 //1
    lr1Analyzer.addProduction(D, vector<symbol*>({ L,id,semicolom,D }));    //2
    lr1Analyzer.addProduction(D, vector<symbol*>({ empty }));               //3
    lr1Analyzer.addProduction(L, vector<symbol*>({ my_int }));              //4
    lr1Analyzer.addProduction(L, vector<symbol*>({ my_float }));            //5
    lr1Analyzer.addProduction(S, vector<symbol*>({ id,equal,E,semicolom }));//6
    lr1Analyzer.addProduction(S, vector<symbol*>({ my_if,leftParenthesis,C,rightParenthesis,M,S }));//7
    lr1Analyzer.addProduction(S, vector<symbol*>({ my_if,leftParenthesis,C,rightParenthesis,M,S,N,my_else,M,S }));//8
    lr1Analyzer.addProduction(S, vector<symbol*>({ my_while,leftParenthesis,C,rightParenthesis,S }));//9
    lr1Analyzer.addProduction(S, vector<symbol*>({ S,M,S }));//10
    lr1Analyzer.addProduction(C, vector<symbol*>({ E,bigger,E }));//11
    lr1Analyzer.addProduction(C, vector<symbol*>({ E,smaller,E }));//12
    lr1Analyzer.addProduction(C, vector<symbol*>({ E,twoequal,E }));//13
    lr1Analyzer.addProduction(E, vector<symbol*>({ E,plus,T }));//14
    lr1Analyzer.addProduction(E, vector<symbol*>({ E,minus,T }));//15
    lr1Analyzer.addProduction(E, vector<symbol*>({ T }));//16
    lr1Analyzer.addProduction(T, vector<symbol*>({ F }));//17
    lr1Analyzer.addProduction(T, vector<symbol*>({ T,multiply,F }));//18
    lr1Analyzer.addProduction(T, vector<symbol*>({ T,div,F }));//19
    lr1Analyzer.addProduction(F, vector<symbol*>({ leftParenthesis,E,rightParenthesis }));//20
    lr1Analyzer.addProduction(F, vector<symbol*>({ id }));//21
    lr1Analyzer.addProduction(F, vector<symbol*>({ digits }));//22
    lr1Analyzer.addProduction(M, vector<symbol*>({ empty }));//23
    lr1Analyzer.addProduction(N, vector<symbol*>({ empty }));//24
    lr1Analyzer.init();

    lr1Analyzer.startAnalyze(tokens);

    return 0;
}