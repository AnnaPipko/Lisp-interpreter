#pragma once

#include <unordered_map>
#include <memory>

#include "common_functions.h"

class ValueType;
class Scope;

enum class NodeType {
    EMPTY, QUOTE, CONST, VAR, PAIR, LAMBDA, FUNC, FUNCLIST
};

class ASTNode{
public:
    ASTNode() = default;
    virtual ~ASTNode() = default;
    virtual NodeType Type() const = 0;
    virtual ValueType ComputeValue(std::shared_ptr<Scope> scope) = 0;
    virtual std::string ToString() const = 0;
};

using NodePtr = std::shared_ptr<ASTNode>;

// int64_t, bool, std::shared_ptr<ASTNode>
class ValueType {
public:

    class InnerBase {
    public:
        virtual ~InnerBase() = default;
        virtual InnerBase *Clone() const = 0;
    };

    template <class T>
    class Inner : public InnerBase {
    public:
        explicit Inner(T value) : value_(value) {}

        InnerBase *Clone() const override {
            return new Inner<T>(value_);
        }

        T& operator*() {
            return value_;
        }

        const T& operator*() const {
            return value_;
        }
    private:
        T value_;
    };

    enum class ValueEnum {UNDEFINED, INT, BOOL, FUNC};

    ValueType() : type_(ValueEnum::UNDEFINED){}


    template<class T>
    explicit ValueType(const T &value) {
        inner_ = std::make_shared<Inner<T>>(value);
        SetType(value);
    }

    template<class T>
    ValueType& operator=(const T &value) {
        inner_.reset(new Inner<T>(value));
        SetType(value);
        return *this;
    }

    ValueType(const ValueType &rhs) : inner_(rhs.inner_->Clone()), type_(rhs.type_){}

    ValueType& operator=(const ValueType &rhs) {
        ValueType temp(rhs);
        Swap(temp);
        return *this;
    }

    ~ValueType() = default;

    bool Empty() const {
        if (inner_.get()) {
            return true;
        }
        return false;
    }

    void Clear() {
        inner_.reset();
        type_ = ValueEnum::UNDEFINED;
    }

    void Swap(ValueType &rhs) {
        std::swap(rhs.inner_, inner_);
        auto temp = rhs.type_;
        rhs.type_ = type_;
        type_ = temp;
    }

    template<class T>
    const T & GetValue() const {
        return *dynamic_cast<Inner<T>&>(*inner_);
    }

    ValueEnum GetType() const {
        return type_;
    }

    std::string ToString() const {
        if (type_ == ValueEnum::BOOL){
            return BoolToString(GetValue<bool>());
        }
        if (type_ == ValueEnum::INT){
            return IntToString(GetValue<int64_t >());
        }
        if (type_ == ValueEnum::FUNC){
            return GetValue<NodePtr>()->ToString();
        }
        return "UNDEFINED";
    }

private:
    std::shared_ptr<InnerBase> inner_;
    ValueEnum type_;
    void SetType(const int64_t& value) {
        type_ = ValueEnum::INT;
    };
    void SetType(const bool& value) {
        type_ = ValueEnum::BOOL;
    };
    void SetType(const NodePtr& value){
        type_ = ValueEnum::FUNC;
    };
    template <class T>
    void SetType(const T& value){
        type_ = ValueEnum::UNDEFINED;
    }

};

class Scope{
public:
    Scope();
    explicit Scope(std::shared_ptr<Scope> parent);
    ValueType GetValue(const std::string& name);
    void AddName(const std::string& name, const ValueType& value);
    void SetValue(const std::string& name, const ValueType& value);
    friend std::shared_ptr<Scope> ConcatScopes(std::shared_ptr<Scope> first, std::shared_ptr<Scope> second);

private:
    friend class Lispp;
    std::shared_ptr<Scope> parent_scope_;
    std::shared_ptr<std::unordered_map<std::string, ValueType>> table_;
    Scope *GetScope(const std::string& name);
};

















