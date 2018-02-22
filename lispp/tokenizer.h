#pragma once

#include <string>

enum class TokenType {
    UNKNOWN,
    NUMBER, BOOL, NAME,
    QUOTE, DOT,
    LEFT_PARENTHESES, RIGHT_PARENTHESES,
    END
};

class Token{
public:
    Token();
    Token(TokenType, std::string);
    TokenType GetType();
    std::string GetString();
private:
    TokenType type_;
    std::string str_;
};

class Tokenizer{
public:
    Tokenizer();
    explicit Tokenizer(std::istream* in);
    void Consume();
    Token GetToken();

private:
    std::istream* in_;
    TokenType token_type_;
    std::string token_str_;
    void SkipDividers();
};