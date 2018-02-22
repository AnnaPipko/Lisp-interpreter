#include "parser.h"
#include "exceptions.h"

Parser::Parser() : tokenizer_(nullptr) {}

Parser::Parser(std::shared_ptr<Tokenizer> tokenizer) : tokenizer_(std::move(tokenizer)) {}

NodePtr Parser::Parse() {
    tokenizer_->Consume();
    return Expression();
}

NodePtr Parser::Expression() {
    auto token = tokenizer_->GetToken();
    if (token.GetType() == TokenType::NUMBER){
        return std::make_shared<Const>(ValueType(StringToInt(token.GetString())));
    }
    if (token.GetType() == TokenType::BOOL){
        return std::make_shared<Const>(ValueType(StringToBool(token.GetString())));
    }
    if (token.GetType() == TokenType::NAME){
        return std::make_shared<Var>(token.GetString());
    }
    if (token.GetType() == TokenType::QUOTE){
        tokenizer_->Consume();
        return std::make_shared<Quote>(Expression());
    }
    if (token.GetType() == TokenType::LEFT_PARENTHESES){
        tokenizer_->Consume();
        token = tokenizer_->GetToken();
        if (token.GetType() == TokenType::RIGHT_PARENTHESES) {
            return std::make_shared<Empty>();
        }
        std::vector<NodePtr> elements;
        elements.push_back(Expression());
        tokenizer_->Consume();
        token = tokenizer_->GetToken();
        while (token.GetType() != TokenType::END &&
                token.GetType() != TokenType::RIGHT_PARENTHESES &&
                token.GetType() != TokenType::DOT){
            elements.push_back(Expression());
            tokenizer_->Consume();
            token = tokenizer_->GetToken();
        }
        if (token.GetType() == TokenType::END) {
            throw SyntaxError(") or . expected");
        }
        if (token.GetType() == TokenType::DOT){
            tokenizer_->Consume();
            elements.push_back(Expression());
            tokenizer_->Consume();
            token = tokenizer_->GetToken();
            if (token.GetType() != TokenType::RIGHT_PARENTHESES) {
                throw SyntaxError("invalid pair");
            }
        } else {
            elements.push_back(std::make_shared<Empty>());
        }
        int it = elements.size() - 2;
        auto pair = std::make_shared<Pair>(elements[it], elements.back());
        --it;
        while (it >= 0){
            pair = std::make_shared<Pair>(elements[it], pair);
            --it;
        }
        return pair;
    }
    throw SyntaxError("unexpectable token " + token.GetString());
}
