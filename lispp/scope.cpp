#include "scope.h"
#include "exceptions.h"

Scope::Scope() {
    table_ = std::make_shared<std::unordered_map<std::string, ValueType>>();
}

Scope::Scope(std::shared_ptr<Scope> parent) : parent_scope_(std::move(parent)) {
    table_ = std::make_shared<std::unordered_map<std::string, ValueType>>();
}

void Scope::AddName(const std::string& name, const ValueType& value) {
    (*table_)[name] = value;
}

Scope *Scope::GetScope(const std::string &name) {
    auto res = table_->find(name);
    if (res != table_->end()){
        return this;
    }
    auto scope = parent_scope_;
    while (scope){
        res = scope->table_->find(name);
        if (res != scope->table_->end()){
            return scope.get();
        }
        scope = scope->parent_scope_;
    }
    return nullptr;
}

ValueType Scope::GetValue(const std::string &name) {
    auto scope = GetScope(name);
    if (scope){
        return (*scope->table_)[name];
    }
    throw NameError("undefined name " + name);
}

void Scope::SetValue(const std::string &name, const ValueType &value) {
    auto scope = GetScope(name);
    if (scope){
        (*scope->table_)[name] = value;
        return;
    }
    throw NameError("undefined name " + name);
}

std::shared_ptr<Scope> ConcatScopes(std::shared_ptr<Scope> first, std::shared_ptr<Scope> second){
    auto res = std::make_shared<Scope>();
    auto copy = res;
    auto scope = first;
    copy->table_ = scope->table_;
    while (scope->parent_scope_){
        copy->parent_scope_ = std::make_shared<Scope>();
        scope = scope->parent_scope_;
        copy = copy->parent_scope_;
        copy->table_ = scope->table_;
    }
    scope = second;
    copy->table_ = scope->table_;
    while (scope->parent_scope_){
        copy->parent_scope_ = std::make_shared<Scope>();
        scope = scope->parent_scope_;
        copy = copy->parent_scope_;
        copy->table_ = scope->table_;
    }
    return res;
}
