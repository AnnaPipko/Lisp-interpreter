#include "tokenizer_test.h"

TEST_CASE_METHOD(TokenizerTest, "Simple test") {
    std::vector<Token> expected;
    expected.emplace_back(TokenType::LEFT_PARENTHESES, "(");
    expected.emplace_back(TokenType::RIGHT_PARENTHESES, ")");
    expected.emplace_back(TokenType::DOT, ".");
    expected.emplace_back(TokenType::QUOTE, "'");
    expected.emplace_back(TokenType::NAME, "+");
    expected.emplace_back(TokenType::NAME, "-");
    expected.emplace_back(TokenType::NUMBER, "+5");
    expected.emplace_back(TokenType::NUMBER, "-3");
    expected.emplace_back(TokenType::NUMBER, "10");
    expected.emplace_back(TokenType::BOOL, "#t");
    expected.emplace_back(TokenType::BOOL, "#f");
    expected.emplace_back(TokenType::NAME, "abc");
    expected.emplace_back(TokenType::NAME, "#abc");
    expected.emplace_back(TokenType::NAME, "x");
    expected.emplace_back(TokenType::END, "");
    ExpectEq(" ( ) . ' + - +5 -3 10 #t #f abc #abc x", expected);
}

TEST_CASE_METHOD(TokenizerTest, "Crazy name test") {
    std::vector<Token> expected;
    expected.emplace_back(TokenType::NAME, "12a");
    expected.emplace_back(TokenType::LEFT_PARENTHESES, "(");
    expected.emplace_back(TokenType::NUMBER, "12");
    expected.emplace_back(TokenType::DOT, ".");
    expected.emplace_back(TokenType::NUMBER, "3");
    expected.emplace_back(TokenType::NAME, "12>5");
    expected.emplace_back(TokenType::END, "");
    ExpectEq("12a( 12.3 12>5", expected);
}

TEST_CASE_METHOD(TokenizerTest, "Divider test") {
    std::vector<Token> expected;
    expected.emplace_back(TokenType::NUMBER, "1");
    expected.emplace_back(TokenType::NUMBER, "2");
    expected.emplace_back(TokenType::NUMBER, "3");
    expected.emplace_back(TokenType::NUMBER, "4");
    expected.emplace_back(TokenType::END, "");
    ExpectEq("\n1 2\t  3 \n4", expected);
}

TEST_CASE_METHOD(TokenizerTest, "Syntax error test") {
    ExpectSyntaxError("+abc", "SyntaxError: variable name starting with +/-");
}

TEST_CASE_METHOD(TokenizerTest, "No error test") {
    ExpectNoError(R"(
            (define range
              (lambda (x)
                (lambda ()
                  (set! x (+ x 1))
                  x)))
                        )");
}
