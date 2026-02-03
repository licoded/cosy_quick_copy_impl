#include "formula/formula.hpp"

#include <stdexcept>

namespace formula {

Formula::Formula(OpType op, std::string literal, Ptr left, Ptr right)
    : op_(op), literal_(std::move(literal)), left_(std::move(left)), right_(std::move(right)) {}

Formula::Ptr Formula::make_true() {
    return std::make_shared<Formula>(OpType::True, "", nullptr, nullptr);
}

Formula::Ptr Formula::make_false() {
    return std::make_shared<Formula>(OpType::False, "", nullptr, nullptr);
}

Formula::Ptr Formula::make_literal(std::string name) {
    return std::make_shared<Formula>(OpType::Literal, std::move(name), nullptr, nullptr);
}

Formula::Ptr Formula::make_unary(OpType op, Ptr operand) {
    if (op != OpType::Not && op != OpType::Next) {
        throw std::invalid_argument("Invalid unary operator");
    }
    return std::make_shared<Formula>(op, "", std::move(operand), nullptr);
}

Formula::Ptr Formula::make_binary(OpType op, Ptr left, Ptr right) {
    if (op != OpType::And && op != OpType::Or && op != OpType::Until && op != OpType::Release) {
        throw std::invalid_argument("Invalid binary operator");
    }
    return std::make_shared<Formula>(op, "", std::move(left), std::move(right));
}

bool Formula::is_binary() const {
    return op_ == OpType::And || op_ == OpType::Or || op_ == OpType::Until || op_ == OpType::Release;
}

bool Formula::is_unary() const {
    return op_ == OpType::Not || op_ == OpType::Next;
}

bool Formula::is_constant() const {
    return op_ == OpType::True || op_ == OpType::False;
}

std::string Formula::to_string() const {
    switch (op_) {
        case OpType::True: return "true";
        case OpType::False: return "false";
        case OpType::Literal: return literal_;
        case OpType::Not: return "!" + left_->to_string();
        case OpType::And: return "(" + left_->to_string() + " & " + right_->to_string() + ")";
        case OpType::Or: return "(" + left_->to_string() + " | " + right_->to_string() + ")";
        case OpType::Next: return "X(" + left_->to_string() + ")";
        case OpType::Until: return "(" + left_->to_string() + " U " + right_->to_string() + ")";
        case OpType::Release: return "(" + left_->to_string() + " R " + right_->to_string() + ")";
    }
    return "";
}

const char *Formula::op_name() const {
    switch (op_) {
        case OpType::True: return "True";
        case OpType::False: return "False";
        case OpType::Literal: return "Literal";
        case OpType::Not: return "Not";
        case OpType::And: return "And";
        case OpType::Or: return "Or";
        case OpType::Next: return "Next";
        case OpType::Until: return "Until";
        case OpType::Release: return "Release";
    }
    return "";
}

std::unordered_set<std::string> Formula::collect_variables(const Ptr &formula) {
    std::unordered_set<std::string> vars;
    if (!formula) return vars;
    if (formula->op_ == OpType::Literal) {
        vars.insert(formula->literal_);
    } else {
        if (formula->left_) {
            auto left_vars = collect_variables(formula->left_);
            vars.insert(left_vars.begin(), left_vars.end());
        }
        if (formula->right_) {
            auto right_vars = collect_variables(formula->right_);
            vars.insert(right_vars.begin(), right_vars.end());
        }
    }
    return vars;
}

} // namespace formula