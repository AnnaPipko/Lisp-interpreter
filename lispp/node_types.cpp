#include "node_types.h"
#include "exceptions.h"
#include <algorithm>

NodeType Empty::Type() const {
    return NodeType ::EMPTY;
}

std::string Empty::ToString() const {
    return "()";
}

ValueType Empty::ComputeValue(std::shared_ptr<Scope> scope) {
    throw RuntimeError("() is not self evaluating");
}

Const::Const(ValueType value) : value_(value) {}

NodeType Const::Type() const {
    return NodeType ::CONST;
}

ValueType Const::ComputeValue(std::shared_ptr<Scope> scope) {
    return value_;
}

std::string Const::ToString() const {
    return value_.ToString();
}

Var::Var(const std::string& name) : name_(name){}

NodeType Var::Type() const {
    return NodeType ::VAR;
}

ValueType Var::ComputeValue(std::shared_ptr<Scope> scope) {
    return scope->GetValue(name_);
}

std::string Var::ToString() const {
    return name_;
}

Quote::Quote(NodePtr value) : value_(std::move(value)) {}

NodeType Quote::Type() const {
    return NodeType ::QUOTE;
}

ValueType Quote::ComputeValue(std::shared_ptr<Scope> scope) {
    return ValueType(value_);
}

std::string Quote::ToString() const {
    return "'" + value_->ToString();
}

Pair::Pair(NodePtr first, NodePtr second) :
        car_(std::move(first)), cdr_(std::move(second)){}

NodeType Pair::Type() const {
    return NodeType ::PAIR;
}

std::string Pair::ToString() const {
    std::string result("(");
    auto elements = ToVector();
    auto last = elements.end();
    --last;
    for (auto el = elements.begin(); el != last; ++el){
        result += (*el)->ToString();
        result += " ";
    }
    if ((*last)->Type() == NodeType::EMPTY){
        result.pop_back();
    } else {
        result += ". ";
        result += (*last)->ToString();
    }
    result += ")";
    return result;
}

std::vector<NodePtr> Pair::ToReverseVector() const {
    std::vector<NodePtr> res;
    if (cdr_->Type() == NodeType::PAIR){
        res = dynamic_cast<Pair *>(cdr_.get())->ToReverseVector();
    } else {
        res.push_back(cdr_);
    }
    res.push_back(car_);
    return res;
}

std::vector<NodePtr> Pair::ToVector() const {
    auto res = ToReverseVector();
    std::reverse(std::begin(res), std::end(res));
    return res;
}

NodePtr Pair::Car() const {
    return car_;
}

NodePtr Pair::Cdr() const {
    return cdr_;
}

void Pair::SetCar(NodePtr car) {
    car_ = car;
}

void Pair::SetCdr(NodePtr cdr) {
    cdr_ = cdr;
}

ValueType Pair::ComputeValue(std::shared_ptr<Scope> scope) {

    auto func = car_->ComputeValue(scope);
    if (func.GetType() != ValueType::ValueEnum::FUNC) {
        throw RuntimeError(func.ToString() + " is not self evaluating");
    }

    std::vector<NodePtr> args;
    if (cdr_->Type() == NodeType::PAIR) {
        args = dynamic_cast<Pair *>(cdr_.get())->ToVector();
    } else {
        args.push_back(cdr_);
    }
    if (args.back()->Type() != NodeType::EMPTY) {
        throw SyntaxError("dotted pair is not self evaluating");
    }
    args.pop_back();
    return dynamic_cast<Func*>(func.GetValue<NodePtr>().get())->Evaluate(args, scope);
}

NodeType Func::Type() const {
    return NodeType ::FUNC;
}

ValueType Func::ComputeValue(std::shared_ptr<Scope> scope) {
    throw SyntaxError("function is not self evaluating");
}

std::string Func::ToString() const {
    return "function";
}

FuncList::FuncList(const std::vector<NodePtr> &func_list) :
        func_list_(func_list) {}

NodeType FuncList::Type() const {
    return NodeType ::FUNCLIST;
}

std::string FuncList::ToString() const {
    std::string result;
    for (auto const& el : func_list_){
        result += el->ToString();
        result += " ";
    }
    result.pop_back();
    return result;
}

ValueType FuncList::ComputeValue(std::shared_ptr<Scope> scope) {
    auto last = func_list_.end();
    --last;
    for (auto el = func_list_.begin(); el != last; ++el){
        (*el)->ComputeValue(scope);
    }
    return (*last)->ComputeValue(scope);
}

Lambda::Lambda(std::vector<std::string> vars,
               NodePtr func,
               std::shared_ptr<Scope> inner_scope)  :
        vars_(std::move(vars)), func_(std::move(func)), inner_scope_(std::move(inner_scope)){}

NodeType Lambda::Type() const {
    return NodeType ::LAMBDA;
}

ValueType Lambda::Evaluate(std::vector<NodePtr> args,
                           std::shared_ptr<Scope> scope) {
    auto full_scope = ConcatScopes(inner_scope_, scope);
    auto new_scope = std::make_shared<Scope>(inner_scope_);
    size_t counter = 0;
    for (auto arg : args){
        new_scope->AddName(vars_[counter], arg->ComputeValue(full_scope));
        ++counter;
    }
    new_scope = ConcatScopes(new_scope, scope);
    return func_->ComputeValue(new_scope);
}

ValueType Define::Evaluate(std::vector<NodePtr> args, std::shared_ptr<Scope> scope) {
    if (args.size() < 2){
        throw SyntaxError("expected 2 arguments in define");
    }
    if (args[0]->Type() == NodeType::VAR){
        if (args.size() != 2){
            throw SyntaxError("expected 2 arguments in define");
        }
        auto value = args[1]->ComputeValue(scope);
        scope->AddName(args[0]->ToString(), value);
        return ValueType(NodePtr(new Empty()));
    }
    if (args[0]->Type() == NodeType::PAIR){
        auto decl = dynamic_cast<Pair*>(args[0].get())->ToVector();
        decl.pop_back();
        if (decl[0]->Type() != NodeType::VAR){
            throw SyntaxError("invalid function declaration");
        }
        std::vector<std::string> var_names;
        for (size_t i = 1; i < decl.size(); ++i){
            if (decl[i]->Type() != NodeType::VAR){
                throw SyntaxError("invalid function declaration");
            }
            var_names.push_back(decl[i]->ToString());
        }
        args.erase(args.begin());
        auto func = NodePtr(
                new Lambda(var_names, NodePtr(new FuncList(args)), scope));
        scope->AddName(decl[0]->ToString(), ValueType(func));
        return ValueType(NodePtr(new Empty()));
    }
    throw SyntaxError("invalid define syntax");
}

ValueType Set::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    if (args.size() != 2){
        throw SyntaxError("expected 2 arguments in set!");
    }
    scope->SetValue(args[0]->ToString(), args[1]->ComputeValue(scope));
    return ValueType(NodePtr(new Empty()));
}

ValueType LambdaForm::Evaluate(std::vector<NodePtr> args,
                               std::shared_ptr<Scope> scope) {
    if (args.size() < 2){
        throw SyntaxError("invalid lambda definition");
    }
    std::vector<std::string> var_names;
    if (args[0]->Type() == NodeType::PAIR) {
        auto decl = dynamic_cast<Pair *>(args[0].get())->ToVector();
        decl.pop_back();
        for (auto var : decl) {
            if (var->Type() != NodeType::VAR) {
                throw SyntaxError("invalid function declaration");
            }
            var_names.push_back(var->ToString());
        }
    }
    args.erase(args.begin());
    return ValueType(NodePtr(
            new Lambda(var_names, NodePtr(new FuncList(args)), scope)));
}

bool IsNull(const ValueType& value){
    if (value.GetType() == ValueType::ValueEnum::FUNC){
        if (value.GetValue<NodePtr>()->Type() == NodeType::EMPTY) {
            return true;
        }
    }
    return false;
}

bool IsBool(const ValueType& value){
    return value.GetType() == ValueType::ValueEnum::BOOL;
}

bool IsTrue(const ValueType& value){
    if (IsBool(value)){
        if (!value.GetValue<bool>()){
            return false;
        }
    }
    return true;
}

bool IsInt(const ValueType& value){
    return value.GetType() == ValueType::ValueEnum::INT;
}

bool IsPair(const NodePtr& node){
    return node->Type() == NodeType::PAIR;
}

bool IsList(const NodePtr& node){
    if (node->Type() == NodeType::EMPTY){
        return true;
    }
    if (IsPair(node)){
        if (dynamic_cast<Pair*>(node.get())->ToVector().back()->Type()==NodeType::EMPTY){
            return true;
        }
    }
    return false;
}

NodePtr NodeFromValue(ValueType value){
    if (IsInt(value)){
        return NodePtr(new Const(value));
    } else {
        if (IsBool(value)){
            return NodePtr(new Const(value));
        } else {
            if (value.GetType() == ValueType::ValueEnum::FUNC){
                return value.GetValue<NodePtr>();
            } else {
                throw RuntimeError("unexpectable argument type");
            }
        }
    }
}

NodePtr ListFromVector(std::vector<NodePtr> elements){
    if (elements.empty()){
        return NodePtr(new Empty());
    }
    elements.push_back(NodePtr(new Empty()));
    int it = elements.size() - 2;
    auto pair = NodePtr(new Pair(elements[it], elements.back()));
    --it;
    while (it >= 0){
        pair = NodePtr(new Pair(elements[it], pair));
        --it;
    }
    return pair;

}

ValueType Plus::Evaluate(std::vector<NodePtr> args,
                         std::shared_ptr<Scope> scope) {
    int64_t sum = 0;
    for (auto& arg : args){
        auto value = arg->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in +");
        }
        sum += value.GetValue<int64_t>();
    }
    return ValueType(sum);
}

ValueType Minus::Evaluate(std::vector<NodePtr> args,
                         std::shared_ptr<Scope> scope) {
    if (args.empty()){
        throw RuntimeError("expected at least 1 argument in -");
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in -");
    }
    int64_t res = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in -");
        }
        res -= value.GetValue<int64_t>();
    }
    return ValueType(res);
}

ValueType Mult::Evaluate(std::vector<NodePtr> args,
                         std::shared_ptr<Scope> scope) {
    int64_t res = 1;
    for (auto& arg : args){
        auto value = arg->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in *");
        }
        res *= value.GetValue<int64_t>();
    }
    return ValueType(res);
}

ValueType Div::Evaluate(std::vector<NodePtr> args,
                          std::shared_ptr<Scope> scope) {
    if (args.empty()){
        throw RuntimeError("expected at least 1 argument in /");
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in /");
    }
    int64_t res = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in /");
        }
        if (value.GetValue<int64_t>() == 0){
            throw RuntimeError("division by zero");
        }
        res /= value.GetValue<int64_t>();
    }
    return ValueType(res);
}

ValueType QuoteForm::Evaluate(std::vector<NodePtr> args,
                              std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw SyntaxError("expected 1 argument in quote");
    }
    return ValueType(args[0]);
}

    ValueType IfElse::Evaluate(std::vector<NodePtr> args,
                           std::shared_ptr<Scope> scope) {
    if (! (args.size() == 2 || args.size() == 3)){
        throw SyntaxError("expected 2 or 3 arguments in if");
    }
    bool condition = IsTrue(args[0]->ComputeValue(scope));
    if (condition){
        return args[1]->ComputeValue(scope);
    }
    if (args.size() == 2){
        return ValueType(NodePtr(new Empty()));
    }
    return args[2]->ComputeValue(scope);
}

ValueType And::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    bool res = true;
    if (args.empty()){
        return ValueType(res);
    }
    auto arg = args.begin();
    ValueType value;
    while (res && arg != args.end()){
        value = (*arg)->ComputeValue(scope);
        res = res && IsTrue(value);
        ++arg;
    }
    return value;
}

ValueType Or::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    bool res = false;
    if (args.empty()){
        return ValueType(res);
    }
    auto arg = args.begin();
    ValueType value;
    while (!res && arg != args.end()){
        value = (*arg)->ComputeValue(scope);
        res = res || IsTrue(value);
        ++arg;
    }
    return value;
}

ValueType Not::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in not");
    }
    return ValueType(!IsTrue(args[0]->ComputeValue(scope)));
}

ValueType NullPredicate::Evaluate(std::vector<NodePtr> args,
                                  std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in null?");
    }
    auto value = args[0]->ComputeValue(scope);
    return ValueType(IsNull(value));
}

ValueType PairPredicate::Evaluate(std::vector<NodePtr> args,
                                  std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in pair?");
    }
    auto value = args[0]->ComputeValue(scope);
    if (value.GetType() == ValueType::ValueEnum::FUNC){
        return ValueType(IsPair(value.GetValue<NodePtr>()));
    }
    return ValueType(false);
}

ValueType NumberPredicate::Evaluate(std::vector<NodePtr> args,
                                  std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in number?");
    }
    return ValueType(IsInt(args[0]->ComputeValue(scope)));
}

ValueType BoolPredicate::Evaluate(std::vector<NodePtr> args,
                                    std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in bool?");
    }
    return ValueType(IsBool(args[0]->ComputeValue(scope)));
}

ValueType SymbolPredicate::Evaluate(std::vector<NodePtr> args,
                                    std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in symbol?");
    }
    auto value = args[0]->ComputeValue(scope);
    if (value.GetType() == ValueType::ValueEnum::FUNC){
        return ValueType(value.GetValue<NodePtr>()->Type()==NodeType::VAR);
    }
    return ValueType(false);
}

ValueType ListPredicate::Evaluate(std::vector<NodePtr> args,
                                  std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in list?");
    }
    auto value = args[0]->ComputeValue(scope);
    if (value.GetType() == ValueType::ValueEnum::FUNC){
        return ValueType(IsList(value.GetValue<NodePtr>()));
    }
    return ValueType(false);
}

bool IsEqual(NodePtr first_ptr, NodePtr second_ptr, std::shared_ptr<Scope> scope){
    auto first = first_ptr->ComputeValue(scope);
    auto second = second_ptr->ComputeValue(scope);
    if (first.GetType() == second.GetType()){
        if (IsNull(first)){
            return true;
        }
        if (IsBool(first)){
            return first.GetValue<bool>() == second.GetValue<bool>();
        }
        if (IsInt(first)){
            return first.GetValue<int64_t>() == second.GetValue<int64_t>();
        }
        if (first.GetType() == ValueType::ValueEnum::FUNC){
            auto first_node = first.GetValue<NodePtr>();
            auto second_node = second.GetValue<NodePtr>();
            if (IsPair(first_node) && IsPair(second_node)){
                auto first_vector = dynamic_cast<Pair*>(first_node.get())->ToVector();
                auto second_vector = dynamic_cast<Pair*>(second_node.get())->ToVector();
                if (first_vector.size() != second_vector.size()){
                    return false;
                }
                if (IsList(first_node) && IsList(second_node)){
                    first_vector.pop_back();
                    second_vector.pop_back();
                } else {
                    if (IsList(first_node) || IsList(second_node)){
                        return false;
                    }
                }
                bool result = true;
                for (size_t i = 0; i < first_vector.size(); ++i){
                    result = result && IsEqual(first_vector[i], second_vector[i], scope);
                }
                return result;
            }
            if (first_node->Type() == NodeType::VAR && second_node->Type() == NodeType::VAR){
                return first_node->ToString() == second_node->ToString();
            }
            return first_node.get() == second_node.get();
        }
        throw RuntimeError("unknown type for compare");
    }
    return false;
}

ValueType EqualPredicate::Evaluate(std::vector<NodePtr> args,
                                  std::shared_ptr<Scope> scope) {
    if (args.size() != 2){
        throw RuntimeError("expected 2 arguments in equal?");
    }
    return ValueType(IsEqual(args[0], args[1], scope));
}

ValueType EqPredicate::Evaluate(std::vector<NodePtr> args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() != 2){
        throw RuntimeError("expected 2 arguments in equal?");
    }
    auto first = args[0]->ComputeValue(scope);
    auto second = args[1]->ComputeValue(scope);
    if (first.GetType() == second.GetType()){
        if (IsNull(first)){
            return ValueType(true);
        }
        if (IsBool(first)){
            return ValueType(first.GetValue<bool>() == second.GetValue<bool>());
        }
        if (IsInt(first)){
            return ValueType(first.GetValue<int64_t>() == second.GetValue<int64_t>());
        }
        if (first.GetType() == ValueType::ValueEnum::FUNC){
            if (first.GetValue<NodePtr>()->Type() == NodeType::VAR &&
                    second.GetValue<NodePtr>()->Type() == NodeType::VAR){
                return ValueType(first.GetValue<NodePtr>()->ToString() ==
                                         second.GetValue<NodePtr>()->ToString());
            }
            return ValueType(first.GetValue<NodePtr>().get() ==
                                     second.GetValue<NodePtr>().get());
        }
        throw RuntimeError("unknown type in equal?");
    }
    return ValueType(false);
}


ValueType IntEqualPredicate::Evaluate(std::vector<NodePtr> args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() != 2){
        throw RuntimeError("expected 2 arguments in equal?");
    }
    auto first = args[0]->ComputeValue(scope);
    auto second = args[1]->ComputeValue(scope);
    if (IsInt(first) && IsInt(second)) {
        return ValueType(first.GetValue<int64_t>() == second.GetValue<int64_t>());
    }
    throw RuntimeError("expected integers in integer-equal?");
}

ValueType Equal::Evaluate(std::vector<NodePtr> args,
                          std::shared_ptr<Scope> scope) {
    bool res = true;
    if (args.empty()){
        return ValueType(res);
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in =");
    }
    int64_t current = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in =");
        }
        res = res && (value.GetValue<int64_t>() == current);
    }
    return ValueType(res);
}

ValueType More::Evaluate(std::vector<NodePtr> args,
                          std::shared_ptr<Scope> scope) {
    bool res = true;
    if (args.empty()){
        return ValueType(res);
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in >");
    }
    int64_t current = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in >");
        }
        res = res && (current > value.GetValue<int64_t>());
        current = value.GetValue<int64_t>();
    }
    return ValueType(res);
}

ValueType Less::Evaluate(std::vector<NodePtr> args,
                         std::shared_ptr<Scope> scope) {
    bool res = true;
    if (args.empty()){
        return ValueType(res);
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in <");
    }
    int64_t current = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in <");
        }
        res = res && (current < value.GetValue<int64_t>());
        current = value.GetValue<int64_t>();
    }
    return ValueType(res);
}

ValueType MoreEqual::Evaluate(std::vector<NodePtr> args,
                         std::shared_ptr<Scope> scope) {
    bool res = true;
    if (args.empty()){
        return ValueType(res);
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in >=");
    }
    int64_t current = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in >=");
        }
        res = res && (current >= value.GetValue<int64_t>());
        current = value.GetValue<int64_t>();
    }
    return ValueType(res);
}

ValueType LessEqual::Evaluate(std::vector<NodePtr> args,
                         std::shared_ptr<Scope> scope) {
    bool res = true;
    if (args.empty()){
        return ValueType(res);
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in <=");
    }
    int64_t current = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in <=");
        }
        res = res && (current <= value.GetValue<int64_t>());
        current = value.GetValue<int64_t>();
    }
    return ValueType(res);
}

ValueType Min::Evaluate(std::vector<NodePtr> args,
                              std::shared_ptr<Scope> scope) {
    if (args.empty()){
        throw RuntimeError("expected at least 1 argument in min");
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in min");
    }
    int64_t res = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in min");
        }
        if (value.GetValue<int64_t>() < res){
            res = value.GetValue<int64_t>();
        }
    }
    return ValueType(res);
}

ValueType Max::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    if (args.empty()){
        throw RuntimeError("expected at least 1 argument in max");
    }
    auto arg = args.begin();
    auto value = (*arg)->ComputeValue(scope);
    if (! IsInt(value)){
        throw RuntimeError("required number in max");
    }
    int64_t res = value.GetValue<int64_t>();
    ++arg;
    for (arg; arg < args.end(); ++arg){
        value = (*arg)->ComputeValue(scope);
        if (! IsInt(value)){
            throw RuntimeError("required number in max");
        }
        if (value.GetValue<int64_t>() > res){
            res = value.GetValue<int64_t>();
        }
    }
    return ValueType(res);
}

ValueType Abs::Evaluate(std::vector<NodePtr> args,
                                    std::shared_ptr<Scope> scope) {
    if (args.size() != 1){
        throw RuntimeError("expected 1 argument in abs");
    }
    auto value = args[0]->ComputeValue(scope);
    if (IsInt(value)){
        return ValueType(static_cast<int64_t >(std::llabs(value.GetValue<int64_t>())));
    }
    throw RuntimeError("expected number in abs");
}

ValueType Cons::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    if (args.size() != 2){
        throw RuntimeError("expected 2 arguments in cons");
    }
    auto car_value = args[0]->ComputeValue(scope);
    auto cdr_value = args[1]->ComputeValue(scope);
    auto car = NodeFromValue(car_value);
    auto cdr = NodeFromValue(cdr_value);
    return ValueType(NodePtr(new Pair(car, cdr)));
}

ValueType Car::Evaluate(std::vector<NodePtr> args,
                         std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw RuntimeError("expected 1 argument in car");
    }
    auto list = NodeFromValue(args[0]->ComputeValue(scope));
    if (IsPair(list)){
        return ValueType(dynamic_cast<Pair*>(list.get())->Car());
    }
    throw RuntimeError("expected pair in car");
}

ValueType Cdr::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw RuntimeError("expected 1 argument in cdr");
    }
    auto list = NodeFromValue(args[0]->ComputeValue(scope));
    if (IsPair(list)){
        return ValueType(dynamic_cast<Pair*>(list.get())->Cdr());
    }
    throw RuntimeError("expected pair in cdr");
}

ValueType SetCar::Evaluate(std::vector<NodePtr> args,
                        std::shared_ptr<Scope> scope) {
    if (args.size() != 2) {
        throw RuntimeError("expected 2 arguments in set-car!");
    }
    auto list = NodeFromValue(args[0]->ComputeValue(scope));
    if (IsPair(list)){
        dynamic_cast<Pair*>(list.get())->SetCar(NodeFromValue(args[1]->ComputeValue(scope)));
        return ValueType(NodePtr(new Empty()));
    }
    throw RuntimeError("expected pair in set-car!");
}

ValueType SetCdr::Evaluate(std::vector<NodePtr> args,
                           std::shared_ptr<Scope> scope) {
    if (args.size() != 2) {
        throw RuntimeError("expected 2 arguments in set-cdr!");
    }
    auto list = NodeFromValue(args[0]->ComputeValue(scope));
    if (IsPair(list)){
        dynamic_cast<Pair*>(list.get())->SetCdr(NodeFromValue(args[1]->ComputeValue(scope)));
        return ValueType(NodePtr(new Empty()));
    }
    throw RuntimeError("expected pair in set-cdr!");
}

ValueType ListForm::Evaluate(std::vector<NodePtr> args,
                             std::shared_ptr<Scope> scope) {
    std::vector<NodePtr> values;
    for (auto& el : args){
        values.push_back(NodeFromValue(el->ComputeValue(scope)));
    }
    return ValueType(ListFromVector(values));
}

ValueType ListRef::Evaluate(std::vector<NodePtr> args,
                           std::shared_ptr<Scope> scope) {
    if (args.size() != 2) {
        throw RuntimeError("expected 2 arguments in list-ref");
    }
    auto list_node = NodeFromValue(args[0]->ComputeValue(scope));
    if (IsList(list_node)){
        auto list = dynamic_cast<Pair*>(list_node.get())->ToVector();
        list.pop_back();
        auto pos_value = args[1]->ComputeValue(scope);
        if (IsInt(pos_value)){
            int64_t pos = pos_value.GetValue<int64_t>();
            if (pos < list.size()){
                return ValueType(list[pos]);
            }
            throw RuntimeError("index out of range");
        }
        throw RuntimeError("expected number for index");
    }
    throw RuntimeError("expected list in list-ref");
}

ValueType ListTail::Evaluate(std::vector<NodePtr> args,
                            std::shared_ptr<Scope> scope) {
    if (args.size() != 2) {
        throw RuntimeError("expected 2 arguments in list-tail");
    }
    auto list_node = NodeFromValue(args[0]->ComputeValue(scope));
    if (IsList(list_node)){
        auto list = dynamic_cast<Pair*>(list_node.get())->ToVector();
        auto pos_value = args[1]->ComputeValue(scope);
        if (IsInt(pos_value)){
            int64_t pos = pos_value.GetValue<int64_t>();
            if (pos < list.size()){
                std::vector<NodePtr> elements;
                while (pos < list.size()){
                    elements.push_back(list[pos]);
                    ++pos;
                }
                elements.pop_back();
                return ValueType(ListFromVector(elements));
            }
            throw RuntimeError("index out of range");
        }
        throw RuntimeError("expected number for index");
    }
    throw RuntimeError("expected list in list-tail");
}

ValueType Eval::Evaluate(std::vector<NodePtr> args,
                             std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw RuntimeError("expected 1 argument in eval");
    }
    auto func_value = args[0]->ComputeValue(scope);
    if (func_value.GetType() == ValueType::ValueEnum::FUNC){
        auto func = func_value.GetValue<NodePtr>();
        return func->ComputeValue(scope);
    }
    throw RuntimeError("not self evaluating");
}
