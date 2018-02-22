#include "parser_test.h"

TEST_CASE_METHOD(ParserTest, "Parser: Const no error test") {
    ExpectNoError("5");
    ExpectNoError("+5");
    ExpectNoError("-5");
    ExpectNoError("#t");
    ExpectNoError("#f");

    ExpectEqual("5", "5");
    ExpectEqual("+5", "5");
    ExpectEqual("-5", "-5");
    ExpectEqual("#t", "#t");
    ExpectEqual("#f", "#f");
}

TEST_CASE_METHOD(ParserTest, "Parser: Name error test") {
    ExpectNameError("x", "NameError: undefined name x");
}

TEST_CASE_METHOD(ParserTest, "Parser: Syntax error test") {
    ExpectSyntaxError(")", "SyntaxError: unexpectable token )");
    ExpectSyntaxError("')", "SyntaxError: unexpectable token )");
    ExpectSyntaxError("''", "SyntaxError: unexpectable token ");

    ExpectSyntaxError("(1 . 2 3)", "SyntaxError: invalid pair");
    ExpectSyntaxError("((1)", "SyntaxError: ) or . expected");
    ExpectSyntaxError(")(1)", "SyntaxError: unexpectable token )");

    ExpectSyntaxError("(.)", "SyntaxError: unexpectable token .");
    ExpectSyntaxError("(1 .)", "SyntaxError: unexpectable token )");
    ExpectSyntaxError("(. 2)", "SyntaxError: unexpectable token .");

}

TEST_CASE_METHOD(ParserTest, "Parser: Quote test") {
    ExpectEqual("'x", "x");
    ExpectEqual("''x", "'x");
    ExpectEqual("'5", "5");
}

TEST_CASE_METHOD(ParserTest, "Parser: Pair test") {

    ExpectEqual("'()", "()");
    ExpectEqual("'(1)", "(1)");
    ExpectEqual("'(1 2)", "(1 2)");

    ExpectEqual("'(1 . 2)", "(1 . 2)");
    ExpectEqual("'(1 2 . 3)", "(1 2 . 3)");
    ExpectEqual("'(1 2 . ())", "(1 2)");
    ExpectEqual("'(1 . (2 . ()))", "(1 2)");

    ExpectNoError("'(1 2 3 4 5)");
    ExpectNoError("'(1 2 3 4 . 5)");
}

TEST_CASE_METHOD(ParserTest, "Parser: Define test") {

    ExpectNoError("(define x 1)");
    ExpectEqual("x" , "1");
    ExpectNoError("(define x '(1 2))");
    ExpectEqual("x" , "(1 2)");

}

TEST_CASE_METHOD(ParserTest, "Parser: Plus test") {

    ExpectEqual("(+ 1 2)" , "3");
    ExpectEqual("(+)" , "0");
    ExpectEqual("(+ 1 2 3)" , "6");

}

TEST_CASE_METHOD(ParserTest, "Parser: Lambda test") {

    ExpectEqual("((lambda (x) (+ 1 x)) 5)", "6");

}

TEST_CASE_METHOD(ParserTest, "Parser: Set! test") {

    ExpectNameError("(set! x 1)", "NameError: undefined name x");
    ExpectNoError("(define x 1)");
    ExpectEqual("x" , "1");
    ExpectEqual("(+ x 1)", "2");
    ExpectNoError("(set! x 5)");
    ExpectEqual("x" , "5");
    ExpectSyntaxError("(set! x)", "SyntaxError: expected 2 arguments in set!");
    ExpectSyntaxError("(set! x 3 4)", "SyntaxError: expected 2 arguments in set!");

}

TEST_CASE_METHOD(ParserTest, "Parser: IntegerArithmetics") {
    ExpectEqual("(+ 1 2)", "3");
    ExpectEqual("(+ 1)", "1");
    ExpectEqual("(+ 1 (+ 3 4 5))", "13");
    ExpectEqual("(- 1 2)", "-1");
    ExpectEqual("(- 2 1)", "1");
    ExpectEqual("(* 5 6)", "30");
    ExpectEqual("(/ 4 2)", "2");
    ExpectEqual("(/ 4 2 2)", "1");

    ExpectEqual("(+)", "0");
    ExpectEqual("(*)", "1");
    ExpectRuntimeError("(/)", "RuntimeError: expected at least 1 argument in /");
    ExpectRuntimeError("(-)", "RuntimeError: expected at least 1 argument in -");
}

