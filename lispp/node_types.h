#pragma once

#include <memory>
#include <string>
#include <vector>

#include "scope.h"

class Empty : public ASTNode{
public:
    NodeType Type() const override;
    ValueType ComputeValue(std::shared_ptr<Scope> scope) override;
    std::string ToString() const override;
};

class Const : public ASTNode{
public:
    explicit Const(ValueType);
    NodeType Type() const override;
    ValueType ComputeValue(std::shared_ptr<Scope> scope) override;
    std::string ToString() const override;
private:
    ValueType value_;
};

class Var : public ASTNode{
public:
    explicit Var(const std::string& name);
    NodeType Type() const override;
    ValueType ComputeValue(std::shared_ptr<Scope> scope) override;
    std::string ToString() const override;

private:
    std::string name_;
};

class Quote : public ASTNode{
public:
    explicit Quote(NodePtr value);
    NodeType Type() const override;
    ValueType ComputeValue(std::shared_ptr<Scope> scope) override;
    std::string ToString() const override;

private:
    NodePtr value_;
};

class Pair : public ASTNode{
public:
    Pair() = default;
    Pair(NodePtr first, NodePtr second);
    NodeType Type() const override;
    ValueType ComputeValue(std::shared_ptr<Scope> scope) override;
    std::string ToString() const override;
    std::vector<NodePtr> ToReverseVector() const;
    std::vector<NodePtr> ToVector() const;
    NodePtr Car() const;
    NodePtr Cdr() const;
    void SetCar(NodePtr car);
    void SetCdr(NodePtr cdr);
private:
    NodePtr car_;
    NodePtr cdr_;
};

class Func : public ASTNode{
public:
    Func() = default;
    NodeType Type() const override;
    ValueType ComputeValue(std::shared_ptr<Scope> scope) override;

    virtual ValueType Evaluate(std::vector<NodePtr> args,
                               std::shared_ptr<Scope> scope) = 0;
    std::string ToString() const override;
};

class FuncList : public ASTNode{
public:
    FuncList() = default;
    explicit FuncList(const std::vector<NodePtr>& func_list);
    NodeType Type() const override;
    ValueType ComputeValue(std::shared_ptr<Scope> scope) override;
    std::string ToString() const override;

private:
    std::vector<NodePtr> func_list_;
};

class Lambda : public Func{
public:
    Lambda(std::vector<std::string> vars,
           NodePtr func,
           std::shared_ptr<Scope> inner_scope);
    NodeType Type() const override;
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;

private:
    friend class Lispp;
    std::vector<std::string> vars_;
    NodePtr func_;
    std::shared_ptr<Scope> inner_scope_;
};

class Define : public Func {
public:
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Set : public Func{ValueType Evaluate(std::vector<NodePtr> args,
                                           std::shared_ptr<Scope> scope) override ;
};

class LambdaForm : public Func{
public:
    LambdaForm() = default;
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

bool IsNull(const ValueType& value);

bool IsBool(const ValueType& value);

bool IsTrue(const ValueType& value);

bool IsInt(const ValueType& value);

bool IsPair(const NodePtr& node);

bool IsList(const NodePtr& node);

NodePtr NodeFromValue(ValueType value);

NodePtr ListFromVector(std::vector<NodePtr> elements);

class Plus : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Minus : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Mult : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Div : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class QuoteForm : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class IfElse : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class And : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Or : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Not : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class NullPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class PairPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class NumberPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class BoolPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class SymbolPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class ListPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class EqualPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class EqPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class IntEqualPredicate : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Equal : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class More : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Less : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class MoreEqual : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class LessEqual : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Min : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Max : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Abs : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Cons : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Car : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Cdr : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class SetCar : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};


class SetCdr : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};


class ListForm : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};


class ListRef : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class ListTail : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};

class Eval : public Func{
    ValueType Evaluate(std::vector<NodePtr> args,
                       std::shared_ptr<Scope> scope) override ;
};
