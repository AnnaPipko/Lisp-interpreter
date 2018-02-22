#include "lisp_test.h"

TEST_CASE_METHOD(LispTest, "Equal? predicate") {

    ExpectEq("(equal? 5 5)", "#t");
    ExpectEq("(equal? 5 6)", "#f");
    ExpectEq("(equal? #t #t)", "#t");
    ExpectEq("(equal? (+ 4 1) 5)", "#t");
    ExpectEq("(equal? '(1 . 2) '(1 . 2))", "#t");
    ExpectEq("(equal? '(1 2 3) '(1 2 3))", "#t");
    ExpectEq("(equal? '(1 2 3) '(1 2 . 3))", "#f");
    ExpectEq("(equal? '() '())", "#t");
    ExpectEq("(equal? (cdr '(a)) '())", "#t");
    ExpectEq("(equal? 'a 'a)", "#t");
    ExpectEq("(equal? 'a 'b)", "#f");
    ExpectNoError("(define a 1)");
    ExpectNoError("(define b 2)");
    ExpectEq("(equal? (cons 'a 'b) (cons 'a 'b))", "#t");

    ExpectNoError("(define test (lambda (x) (set! x (* x 2)) (+ 1 x)))");
    ExpectEq("(equal? test test)", "#t");
}

TEST_CASE_METHOD(LispTest, "Eq? predicate") {

    ExpectEq("(eq? 5 5)", "#t");
    ExpectEq("(eq? 5 6)", "#f");
    ExpectEq("(eq? #t #t)", "#t");
    ExpectEq("(eq? '(1 . 2) '(1 . 2))", "#f");
    ExpectEq("(eq? '(1 2 3) '(1 2 3))", "#f");
    ExpectEq("(eq? '() '())", "#t");
    ExpectEq("(eq? (cdr '(a)) '())", "#t");
    ExpectEq("(eq? 'a 'a)", "#t");
    ExpectNoError("(define a 1)");
    ExpectNoError("(define b 2)");
    ExpectEq("(eq? (cons 'a 'b) (cons 'a 'b))", "#f");

    ExpectNoError("(define test (lambda (x) (set! x (* x 2)) (+ 1 x)))");
    ExpectEq("(eq? test test)", "#t");
}
