#include "formula/formula_parser.hpp"
#include "ltlparser/trans.h"
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_map>

namespace Cosy {

std::vector<std::string> Formula::names_;
std::unordered_map<std::string, int> Formula::ids_;

Formula::Formula() = default;

Formula::Formula(const char* input, bool is_ltlf) {
    if (names_.empty()) {
        names_.push_back("true");
        names_.push_back("false");
        names_.push_back("Literal");
        names_.push_back("!");
        names_.push_back("|");
        names_.push_back("&");
        names_.push_back("X[!]");
        names_.push_back("X"); // weak Next, for LTLf
        names_.push_back("U");
        names_.push_back("R");
        names_.push_back("Undefined");
    }
    std::cout << "Before getAST: " << input << std::endl;
    if (input == nullptr || std::strlen(input) == 0) {
        throw std::invalid_argument("Input formula cannot be empty");
    }
    ltl_formula* formula = getAST(input);
    std::cout << "After getAST: " << input << std::endl;
    std::cout << "Parsing: " << input << std::endl;
build(formula, false, is_ltlf);
    destroy_formula(formula);
}

Formula::Formula(const ltl_formula* formula, bool is_not, bool is_ltlf) {
    build(formula, is_not, is_ltlf);
}

Formula::~Formula() {
    delete left_;
    delete right_;
}

void Formula::build(const ltl_formula* formula, bool is_not, bool is_ltlf) {
    if (formula == nullptr) {
        op_ = Operator::Undefined;
        return;
    }

    switch (formula->_type) {
        case eTRUE:
            op_ = is_not ? Operator::False : Operator::True;
            break;
        case eFALSE:
            op_ = is_not ? Operator::True : Operator::False;
            break;
        case eLITERAL:
            build_atom(formula->_var, is_not);
            break;
        case eNOT:
            build(formula->_right, !is_not, is_ltlf);
            break;
        case eNEXT:
            op_ = (is_ltlf && is_not) ? Operator::WNext : Operator::Next;
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eWNEXT:
            if (!is_ltlf) throw std::runtime_error("is_ltlf must be true for weak next operator (WNEXT)!");
            op_ = is_not ? Operator::Next : Operator::WNext;
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eGLOBALLY: // G a = false R a -- [!(G a) = true U !a]
            op_ = is_not ? Operator::Until : Operator::Release;
            left_ = new Formula();
            left_->op_ = is_not ? Operator::True : Operator::False;
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eFUTURE: // F a = true U a -- [!(F a) = false R !a]
            op_ = is_not ? Operator::Release : Operator::Until;
            left_ = new Formula();
            left_->op_ = is_not ? Operator::False : Operator::True;
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eUNTIL:
            op_ = is_not ? Operator::Release : Operator::Until;
            left_ = new Formula(formula->_left, is_not, is_ltlf);
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eRELEASE:
            op_ = is_not ? Operator::Until : Operator::Release;
            left_ = new Formula(formula->_left, is_not, is_ltlf);
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eAND:
            op_ = is_not ? Operator::Or : Operator::And;
            left_ = new Formula(formula->_left, is_not, is_ltlf);
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eOR:
            op_ = is_not ? Operator::And : Operator::Or;
            left_ = new Formula(formula->_left, is_not, is_ltlf);
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eIMPLIES: // a->b = !a | b -- [!(a->b) = a & !b]
            op_ = is_not ? Operator::And : Operator::Or;
            left_ = new Formula(formula->_left, !is_not, is_ltlf);
            right_ = new Formula(formula->_right, is_not, is_ltlf);
            break;
        case eEQUIV: {
            ltl_formula* not_a = create_operation(eNOT, NULL, formula->_left);
            ltl_formula* not_b = create_operation(eNOT, NULL, formula->_right);
            ltl_formula* new_left = create_operation(eOR, not_a, formula->_right);
            ltl_formula* new_right = create_operation(eOR, not_b, formula->_left);
            ltl_formula* now = create_operation(eAND, new_left, new_right);
            build(now, is_not, is_ltlf);
            destroy_node(not_a);
            destroy_node(not_b);
            destroy_node(new_left);
            destroy_node(new_right);
            destroy_node(now);
            break;
        }
        default:
            throw std::runtime_error("The formula cannot be recognized by Cosy!");
    }
}

void Formula::build_atom(const char* name, bool is_not) {
    auto it = ids_.find(name);
    int id;
    if (it == ids_.end()) {
        id = names_.size();
        ids_[name] = id;
        names_.push_back(name);
    } else {
        id = it->second;
    }

    if (is_not) {
        op_ = Operator::Not;
        right_ = new Formula();
        right_->op_ = static_cast<Operator>(id);

    } else {
        op_ = static_cast<Operator>(id);
    }
}


std::string Formula::toString() const {
    if (left_ == nullptr && right_ == nullptr) {
        if (static_cast<int>(op_) >= names_.size()) {
            return "unknown_literal";
        }
        return names_[static_cast<int>(op_)];
    }
    if (op_ == Operator::Not) {
        return "!" + right_->toString();
    }
    if (left_ == nullptr) {
        return "(" + names_[static_cast<int>(op_)] + " " + right_->toString() + ")";
    }
    if (right_ == nullptr) {
        return "(" + left_->toString() + " " + names_[static_cast<int>(op_)] + ")";
    }

    if (left_->op_ == Operator::True && op_ == Operator::Until)
        return "(F " + right_->toString() + ")";
    if (left_->op_ == Operator::False && op_ == Operator::Release)
        return "(G " + right_->toString() + ")";

    return "(" + left_->toString() + " " + names_[static_cast<int>(op_)] + " " + right_->toString() + ")";
}

} // namespace Cosy

