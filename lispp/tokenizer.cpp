#include "tokenizer.h"
#include "exceptions.h"

#include <iostream>

bool IsNameSymbol(char ch){
    return !(ch == '(' || ch == ')' || ch == '.' || ch == '\'');
}

bool IsNameSymbol(int ch_int){
    return IsNameSymbol(static_cast<char>(ch_int));
}

bool IsDivider(char ch) {
    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
        return true;
    }
    return false;
}

bool IsDivider(int ch_int) {
    return IsDivider(static_cast<char>(ch_int));
}

Token::Token() : type_(TokenType::UNKNOWN) {}

Token::Token(TokenType type, std::string str) : type_(type), str_(str) {}

TokenType Token::GetType() {
    return type_;
}

std::string Token::GetString() {
    return str_;
}

Tokenizer::Tokenizer() : in_(nullptr), token_type_(TokenType::UNKNOWN) {}

Tokenizer::Tokenizer(std::istream *in) : in_(in), token_type_(TokenType::UNKNOWN) {}

void Tokenizer::SkipDividers() {
    while (IsDivider(in_->peek())){
        in_->get();
    }
}

void Tokenizer::Consume() {

    SkipDividers();
    if (in_->peek() == EOF) {
        token_type_ = TokenType::END;
        token_str_ = "";
        return;
    }
    char ch = in_->get();
    if (ch == '(') {
        token_type_ = TokenType::LEFT_PARENTHESES;
        token_str_ = std::string(1, ch);
        return;
    }
    if (ch == ')') {
        token_type_ = TokenType::RIGHT_PARENTHESES;
        token_str_ = std::string(1, ch);
        return;
    }
    if (ch == '\'') {
        token_type_ = TokenType::QUOTE;
        token_str_ = std::string(1, ch);
        return;
    }
    if (ch == '.') {
        token_type_ = TokenType::DOT;
        token_str_ = std::string(1, ch);
        return;
    }
    if (ch == '+' || ch == '-') {
        if (IsDivider(in_->peek()) || !IsNameSymbol(in_->peek())) {
            token_type_ = TokenType::NAME;
            token_str_ = std::string(1, ch);
            return;
        }
        if (std::isdigit(in_->peek())){
            token_type_ = TokenType::NUMBER;
            token_str_ = std::string(1, ch);
            token_str_.push_back(in_->get());
            while (std::isdigit(in_->peek())){
                token_str_.push_back(in_->get());
            }
            return;
        }
        throw SyntaxError("variable name starting with +/-");
    }
    token_str_ = std::string(1, ch);
    bool is_number = false;
    if (std::isdigit(static_cast<int>(ch))) {
        is_number = true;
    }
    while (in_->peek() != EOF && !IsDivider(in_->peek()) && IsNameSymbol(in_->peek())){
        if (!std::isdigit(in_->peek())){
            is_number = false;
        }
        token_str_.push_back(in_->get());
    }
    if (is_number){
        token_type_ = TokenType ::NUMBER;
        return;
    }
    if (token_str_ == "#t" || token_str_ == "#f"){
        token_type_ = TokenType::BOOL;
        return;
    }
    token_type_ = TokenType::NAME;
}

Token Tokenizer::GetToken() {
    return {token_type_, token_str_};
}
