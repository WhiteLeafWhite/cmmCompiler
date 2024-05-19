#pragma once
enum TokenType {
    TOK_ID,
    TOK_DIGITS,
    TOK_OPERATOR,
    TOK_SEPARATOR,
    TOK_KEYWORD,
    TOK_UNKNOWN,
    TOK_ERROR
};
struct Token {
    TokenType type;
    std::string lexeme;
};