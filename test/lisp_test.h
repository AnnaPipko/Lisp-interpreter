#pragma once

#include <catch.hpp>
#include <lispp/lispp.h>
#include <lispp/exceptions.h>

struct LispTest {

    Lispp lisp;
    std::stringstream in;
    std::stringstream out;

    LispTest() : lisp(&in, &out) {}

    void ExpectEq(std::string expression, std::string expected) {
        in.clear();
        out.clear();
        in.str(expression);
        out.str("");
        lisp.Run();
        std::string result;
        getline(out, result);
        CHECK(result == std::string("     >> ") + expected);
    }

    void ExpectNoError(std::string expression) {
        in.clear();
        out.clear();
        in.str(expression);
        out.str("");
        try {
            while(in.peek() != EOF) {
                lisp.Run();
            }
        }  catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }

    void ExpectSyntaxError(std::string expression) {
        in.clear();
        out.clear();
        in.str(expression);
        out.str("");
        try {
            while (in.peek() != EOF) {
                lisp.Run();
            }
        } catch(const SyntaxError& err){
            CHECK(true);
        } catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }

    void ExpectRuntimeError(std::string expression) {
        in.clear();
        out.clear();
        in.str(expression);
        out.str("");
        try {
            while (in.peek() != EOF) {
                lisp.Run();
            }
        } catch(const RuntimeError& err){
            CHECK(true);
        } catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }

    void ExpectNameError(std::string expression) {
        in.clear();
        out.clear();
        in.str(expression);
        out.str("");
        try {
            while (in.peek() != EOF) {
                lisp.Run();
            }
        } catch(const NameError& err){
            CHECK(true);
        } catch (const std::exception& err){
            std::cout << err.what();
            CHECK(false);
        }
    }
};
