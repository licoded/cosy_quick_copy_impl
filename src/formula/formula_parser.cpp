#include "formula/formula_parser.hpp"
#include "ltlparser/trans.h"
#include <stdexcept>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_map>

namespace Cosy {

std::vector<std::string> Formula::names_;
std::unordered_map<std::string, int> Formula::ids_;

Formula::Formula() = default;

Formula::Formula(const char* input) {
    if (names_.empty()) {
        names_.push_back("true");
        names_.push_back("false");
        names_.push_back("Literal");
        names_.push_back("!");
        names_.push_back("|");
        names_.push_back("&");
        names_.push_back("X[!]");
        names_.push_back("X");
        names_.push_back("U");
        names_.push_back("R");
        names_.push_back("Undefined");
    }
    if (input == nullptr || std::strlen(input) == 0) {
        throw std::invalid_argument("Input formula cannot be empty");
    }
    ltl_formula* formula = getAST(input);
    if (formula == nullptr) {
        throw std::runtime_error("Failed to parse formula");
    }
    build(formula);
    destroy_formula(formula);
}

Formula::Formula(const ltl_formula* formula) {
    build(formula);
}

Formula::~Formula() {
    delete left_;
    delete right_;
}

void Formula::build(const ltl_formula* formula) {
    if (formula == nullptr) {
        op_ = Operator::Undefined;
        return;
    }

    switch (formula->_type) {
        case eTRUE:
            op_ = Operator::True;
            break;
        case eFALSE:
            op_ = Operator::False;
            break;
        case eLITERAL:
            build_atom(formula->_var);
            break;
        case eNOT:
            op_ = Operator::Not;
            right_ = new Formula(formula->_right);
            break;
        case eNEXT:
            op_ = Operator::Next;
            right_ = new Formula(formula->_right);
            break;
        case eWNEXT:
            op_ = Operator::WNext;
            right_ = new Formula(formula->_right);
            break;
        case eGLOBALLY:
            op_ = Operator::Release;
            left_ = new Formula();
            left_->op_ = Operator::False;
            right_ = new Formula(formula->_right);
            break;
        case eFUTURE:
            op_ = Operator::Until;
            left_ = new Formula();
            left_->op_ = Operator::True;
            right_ = new Formula(formula->_right);
            break;
        case eUNTIL:
            op_ = Operator::Until;
            left_ = new Formula(formula->_left);
            right_ = new Formula(formula->_right);
            break;
        case eRELEASE:
            op_ = Operator::Release;
            left_ = new Formula(formula->_left);
            right_ = new Formula(formula->_right);
            break;
        case eAND:
            op_ = Operator::And;
            left_ = new Formula(formula->_left);
            right_ = new Formula(formula->_right);
            break;
        case eOR:
            op_ = Operator::Or;
            left_ = new Formula(formula->_left);
            right_ = new Formula(formula->_right);
            break;
        case eIMPLIES: {
            op_ = Operator::Or;
            ltl_formula* not_left = create_operation(eNOT, NULL, formula->_left);
            left_ = new Formula(not_left);
            right_ = new Formula(formula->_right);
            destroy_node(not_left);
            break;
        }
        case eEQUIV: {
            ltl_formula* not_a = create_operation(eNOT, NULL, formula->_left);
            ltl_formula* not_b = create_operation(eNOT, NULL, formula->_right);
            ltl_formula* new_left = create_operation(eOR, not_a, formula->_right);
            ltl_formula* new_right = create_operation(eOR, not_b, formula->_left);
            ltl_formula* now = create_operation(eAND, new_left, new_right);
            build(now);
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

void Formula::build_atom(const char* name) {
    auto it = ids_.find(name);
    int id;
    if (it == ids_.end()) {
        id = names_.size();
        ids_[name] = id;
        names_.push_back(name);
    } else {
        id = it->second;
    }
    op_ = static_cast<Operator>(id);
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

