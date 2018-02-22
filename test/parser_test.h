#pragma once

#include <catch.hpp>
#include <lispp/parser.h>
#include <lispp/exceptions.h>
#include <lispp/scope.h>
#include <iostream>

struct ParserTest {

    std::shared_ptr<Tokenizer> tokenizer;
    std::shared_ptr<Parser> parser;
    std::shared_ptr<Scope> global_scope;
    std::stringstream in;

    ParserTest(){
        tokenizer = std::make_shared<Tokenizer>(&in);
        parser = std::make_shared<Parser>(tokenizer);
        global_scope = std::make_shared<Scope>();
        global_scope->AddName("define", ValueType(std::shared_ptr<ASTNode>(new Define())));
        global_scope->AddName("lambda", ValueType(std::shared_ptr<ASTNode>(new LambdaForm())));
        global_scope->AddName("+", ValueType(std::shared_ptr<ASTNode>(new Plus())));
        global_scope->AddName("-", ValueType(std::shared_ptr<ASTNode>(new Minus())));
        global_scope->AddName("*", ValueType(std::shared_ptr<ASTNode>(new Mult())));
        global_scope->AddName("/", ValueType(std::shared_ptr<ASTNode>(new Div())));
        global_scope->AddName("set!", ValueType(std::shared_ptr<ASTNode>(new Set())));
        global_scope->AddName("if", ValueType(std::shared_ptr<ASTNode>(new IfElse())));
        global_scope->AddName("quote", ValueType(std::shared_ptr<ASTNode>(new QuoteForm())));
        global_scope->AddName("and", ValueType(std::shared_ptr<ASTNode>(new And())));
        global_scope->AddName("or", ValueType(std::shared_ptr<ASTNode>(new Or())));
        global_scope->AddName("null?", ValueType(std::shared_ptr<ASTNode>(new NullPredicate())));
        global_scope->AddName("pair?", ValueType(std::shared_ptr<ASTNode>(new PairPredicate())));
        global_scope->AddName("number?", ValueType(std::shared_ptr<ASTNode>(new NumberPredicate())));
        global_scope->AddName("boolean?", ValueType(std::shared_ptr<ASTNode>(new BoolPredicate())));
        global_scope->AddName("symbol?", ValueType(std::shared_ptr<ASTNode>(new SymbolPredicate())));
        global_scope->AddName("list?", ValueType(std::shared_ptr<ASTNode>(new ListPredicate())));
        global_scope->AddName("eq?", ValueType(std::shared_ptr<ASTNode>(new EqualPredicate())));
        global_scope->AddName("equal?", ValueType(std::shared_ptr<ASTNode>(new EqPredicate())));
        global_scope->AddName("integer-equal?", ValueType(std::shared_ptr<ASTNode>(new IntEqualPredicate())));
        global_scope->AddName("not", ValueType(std::shared_ptr<ASTNode>(new Not())));
        global_scope->AddName("=", ValueType(std::shared_ptr<ASTNode>(new Equal())));
        global_scope->AddName("<", ValueType(std::shared_ptr<ASTNode>(new Less())));
        global_scope->AddName(">", ValueType(std::shared_ptr<ASTNode>(new More())));
        global_scope->AddName("<=", ValueType(std::shared_ptr<ASTNode>(new LessEqual())));
        global_scope->AddName(">=", ValueType(std::shared_ptr<ASTNode>(new MoreEqual())));
        global_scope->AddName("min", ValueType(std::shared_ptr<ASTNode>(new Min())));
        global_scope->AddName("max", ValueType(std::shared_ptr<ASTNode>(new Max())));
        global_scope->AddName("abs", ValueType(std::shared_ptr<ASTNode>(new Abs())));
        global_scope->AddName("cons", ValueType(NodePtr(new Cons())));
        global_scope->AddName("car", ValueType(NodePtr(new Car())));
        global_scope->AddName("cdr", ValueType(NodePtr(new Cdr())));
        global_scope->AddName("set-car!", ValueType(NodePtr(new SetCar())));
        global_scope->AddName("set-cdr!", ValueType(NodePtr(new SetCdr())));
        global_scope->AddName("list", ValueType(NodePtr(new ListForm())));
        global_scope->AddName("list-ref", ValueType(NodePtr(new ListRef())));
        global_scope->AddName("list-tail", ValueType(NodePtr(new ListTail())));
        global_scope->AddName("eval", ValueType(NodePtr(new Eval())));
    }

    void ExpectEqual(std::string expression, std::string expected) {
        in.clear();
        in.str(expression);
        auto node = parser->Parse();
        ValueType res_value = node->ComputeValue(global_scope);
        CHECK(res_value.ToString() == expected);
    }

    void ExpectNoError(std::string expression) {
        in.clear();
        in.str(expression);
        try {
            auto node = parser->Parse();
            ValueType res_value = node->ComputeValue(global_scope);
            CHECK(true);
        } catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }

    void ExpectNameError(std::string expression, std::string error_message) {
        in.clear();
        in.str(expression);
        try {
            auto node = parser->Parse();
            ValueType res_value = node->ComputeValue(global_scope);
            CHECK(false);
        } catch(const NameError& err){
            CHECK(error_message == err.what());
        } catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }

    void ExpectSyntaxError(std::string expression, std::string error_message) {
        in.clear();
        in.str(expression);
        try {
            while (!in.eof()) {
                auto node = parser->Parse();
                ValueType res_value = node->ComputeValue(global_scope);
            }
            CHECK(false);
        } catch(const SyntaxError& err){
            CHECK(error_message == err.what());
        } catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }

    void ExpectRuntimeError(std::string expression, std::string error_message) {
        in.clear();
        in.str(expression);
        try {
            while (!in.eof()) {
                auto node = parser->Parse();
                ValueType res_value = node->ComputeValue(global_scope);
            }
            CHECK(false);
        } catch(const RuntimeError& err){
            CHECK(error_message == err.what());
        } catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }
};
