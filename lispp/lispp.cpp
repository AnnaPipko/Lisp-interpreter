#include "lispp.h"

Lispp::Lispp() : in_(&std::cin), out_(&std::cout) {}

Lispp::~Lispp() {
    for (auto it = global_scope_->table_->begin(); it != global_scope_->table_->end(); ++it){
        if (it->second.GetType() == ValueType::ValueEnum::FUNC){
            auto node = it->second.GetValue<NodePtr>();
            if (node->Type() == NodeType::LAMBDA){
                dynamic_cast<Lambda*>(node.get())->inner_scope_.reset();
            }
        }
    }
}

Lispp::Lispp(std::istream *in, std::ostream *out) :
        in_(in), out_(out) {
    tokenizer_ = std::make_shared<Tokenizer>(in_);
    parser_ = std::make_shared<Parser>(tokenizer_);
    global_scope_ = std::make_shared<Scope>();
    global_scope_->AddName("define", ValueType(NodePtr(new Define())));
    global_scope_->AddName("lambda", ValueType(NodePtr(new LambdaForm())));
    global_scope_->AddName("+", ValueType(NodePtr(new Plus())));
    global_scope_->AddName("-", ValueType(NodePtr(new Minus())));
    global_scope_->AddName("*", ValueType(NodePtr(new Mult())));
    global_scope_->AddName("/", ValueType(NodePtr(new Div())));
    global_scope_->AddName("set!", ValueType(NodePtr(new Set())));
    global_scope_->AddName("if", ValueType(NodePtr(new IfElse())));
    global_scope_->AddName("quote", ValueType(NodePtr(new QuoteForm())));
    global_scope_->AddName("and", ValueType(NodePtr(new And())));
    global_scope_->AddName("or", ValueType(NodePtr(new Or())));
    global_scope_->AddName("null?", ValueType(NodePtr(new NullPredicate())));
    global_scope_->AddName("pair?", ValueType(NodePtr(new PairPredicate())));
    global_scope_->AddName("number?", ValueType(NodePtr(new NumberPredicate())));
    global_scope_->AddName("boolean?", ValueType(NodePtr(new BoolPredicate())));
    global_scope_->AddName("symbol?", ValueType(NodePtr(new SymbolPredicate())));
    global_scope_->AddName("list?", ValueType(NodePtr(new ListPredicate())));
    global_scope_->AddName("eq?", ValueType(NodePtr(new EqPredicate())));
    global_scope_->AddName("equal?", ValueType(NodePtr(new EqualPredicate())));
    global_scope_->AddName("integer-equal?", ValueType(NodePtr(new IntEqualPredicate())));
    global_scope_->AddName("not", ValueType(NodePtr(new Not())));
    global_scope_->AddName("=", ValueType(NodePtr(new Equal())));
    global_scope_->AddName("<", ValueType(NodePtr(new Less())));
    global_scope_->AddName(">", ValueType(NodePtr(new More())));
    global_scope_->AddName("<=", ValueType(NodePtr(new LessEqual())));
    global_scope_->AddName(">=", ValueType(NodePtr(new MoreEqual())));
    global_scope_->AddName("min", ValueType(NodePtr(new Min())));
    global_scope_->AddName("max", ValueType(NodePtr(new Max())));
    global_scope_->AddName("abs", ValueType(NodePtr(new Abs())));
    global_scope_->AddName("cons", ValueType(NodePtr(new Cons())));
    global_scope_->AddName("car", ValueType(NodePtr(new Car())));
    global_scope_->AddName("cdr", ValueType(NodePtr(new Cdr())));
    global_scope_->AddName("set-car!", ValueType(NodePtr(new SetCar())));
    global_scope_->AddName("set-cdr!", ValueType(NodePtr(new SetCdr())));
    global_scope_->AddName("list", ValueType(NodePtr(new ListForm())));
    global_scope_->AddName("list-ref", ValueType(NodePtr(new ListRef())));
    global_scope_->AddName("list-tail", ValueType(NodePtr(new ListTail())));
    global_scope_->AddName("eval", ValueType(NodePtr(new Eval())));
}

void Lispp::Run() {
    auto node = parser_->Parse();
    ValueType value = node->ComputeValue(global_scope_);
    auto value_string = value.ToString();
    if (value_string != "") {
        (*out_) << "     >> " << value.ToString() << std::endl;
    };
}