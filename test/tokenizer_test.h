#pragma once

#include <catch.hpp>
#include <lispp/tokenizer.h>
#include <lispp/exceptions.h>
#include <vector>

struct TokenizerTest {

    std::stringstream in;
    Tokenizer tokenizer;

    TokenizerTest() : tokenizer(&in) {}


    void ExpectEq(std::string expression, std::vector<Token> tokens) {
        in.clear();
        in.str(expression);
        tokenizer.Consume();
        auto current_token = tokenizer.GetToken();
        size_t it = 0;
        while (!in.eof()){
            CHECK(current_token.GetType() == tokens[it].GetType());
            CHECK(current_token.GetString() == tokens[it].GetString());
            ++it;
            tokenizer.Consume();
            current_token = tokenizer.GetToken();
        }
        CHECK(current_token.GetType() == tokens[it].GetType());
        CHECK(current_token.GetString() == tokens[it].GetString());
    }

    void ExpectNoError(std::string expression) {
        try {
            in.clear();
            in.str(expression);
            tokenizer.Consume();
            tokenizer.GetToken();
            while (!in.eof()) {
                tokenizer.Consume();
                tokenizer.GetToken();
            }
        } catch (...){
            CHECK(false);
        }
    }

    void ExpectSyntaxError(std::string expression, std::string error_message) {
        try {
            in.clear();
            in.str(expression);
            tokenizer.Consume();
            tokenizer.GetToken();
            while (!in.eof()) {
                tokenizer.Consume();
                tokenizer.GetToken();
            }
            CHECK(false);
        } catch(const SyntaxError& err){
            CHECK(error_message.compare(err.what()) == 0);
        } catch (...){
            CHECK(false);
        }
    }
};
