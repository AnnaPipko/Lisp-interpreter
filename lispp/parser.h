#pragma once

#include "tokenizer.h"
#include "node_types.h"

class Parser{
public:
    Parser();
    explicit Parser(std::shared_ptr<Tokenizer>);
    NodePtr Parse();
    NodePtr Expression();

private:
    std::shared_ptr<Tokenizer> tokenizer_;
};