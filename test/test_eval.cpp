#include "lisp_test.h"

TEST_CASE_METHOD(LispTest, "Quote") {
    ExpectEq("(quote (1 2))", "(1 2)");
}

TEST_CASE_METHOD(LispTest, "Eval") {
    ExpectEq("(eval '(max 1 2 3 4 5))", "5");

}
