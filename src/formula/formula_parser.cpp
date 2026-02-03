#include "formula/formula_parser.hpp"

#include <cctype>
#include <iostream>

namespace formula {

FormulaParser::FormulaParser(std::unordered_map<std::string, int> variables)
    : variables_(std::move(variables)) {}

Formula::Ptr FormulaParser::parse(const std::string &input) {
    tokenize(input);
    pos_ = 0;
    error_.clear();
    auto result = parse_formula();
    if (!error_.empty() || peek().type != Token::Type::End) {
        if (error_.empty()) set_error("Unexpected tokens");
        return nullptr;
    }
    return result;
}

void FormulaParser::tokenize(const std::string &input) {
    tokens_.clear();
    size_t i = 0;
    while (i < input.size()) {
        char c = input[i];
        if (std::isspace(c)) {
            ++i;
            continue;
        }
        if (c == '!') {
            tokens_.push_back({Token::Type::Not, "!"});
            ++i;
        } else if (c == '&') {
            tokens_.push_back({Token::Type::And, "&"});
            ++i;
        } else if (c == '|') {
            tokens_.push_back({Token::Type::Or, "|"});
            ++i;
        } else if (c == '(') {
            tokens_.push_back({Token::Type::LParen, "("});
            ++i;
        } else if (c == ')') {
            tokens_.push_back({Token::Type::RParen, ")"});
            ++i;
        } else if (c == 'X') {
            tokens_.push_back({Token::Type::Next, "X"});
            ++i;
        } else if (c == 'U') {
            tokens_.push_back({Token::Type::Until, "U"});
            ++i;
        } else if (c == 'R') {
            tokens_.push_back({Token::Type::Release, "R"});
            ++i;
        } else if (std::isalpha(c) || c == '_') {
            std::string ident;
            while (i < input.size() && (std::isalnum(input[i]) || input[i] == '_')) {
                ident += input[i++];
            }
            if (ident == "true") {
                tokens_.push_back({Token::Type::True, ident});
            } else if (ident == "false") {
                tokens_.push_back({Token::Type::False, ident});
            } else {
                tokens_.push_back({Token::Type::Identifier, ident});
            }
        } else {
            set_error("Unknown token");
            break;
        }
    }
    tokens_.push_back({Token::Type::End, ""});
}

FormulaParser::Token FormulaParser::peek() const {
    if (pos_ < tokens_.size()) {
        return tokens_[pos_];
    }
    return {Token::Type::End, ""};
}

FormulaParser::Token FormulaParser::consume() {
    if (pos_ < tokens_.size()) {
        return tokens_[pos_++];
    }
    return {Token::Type::End, ""};
}

void FormulaParser::set_error(const char *msg) {
    error_ = msg;
}

Formula::Ptr FormulaParser::parse_formula() {
    return parse_or();
}

Formula::Ptr FormulaParser::parse_or() {
    auto left = parse_and();
    while (peek().type == Token::Type::Or) {
        consume();
        auto right = parse_and();
        left = Formula::make_binary(Formula::OpType::Or, left, right);
    }
    return left;
}

Formula::Ptr FormulaParser::parse_and() {
    auto left = parse_unary();
    while (peek().type == Token::Type::And) {
        consume();
        auto right = parse_unary();
        left = Formula::make_binary(Formula::OpType::And, left, right);
    }
    return left;
}

Formula::Ptr FormulaParser::parse_unary() {
    if (peek().type == Token::Type::Not) {
        consume();
        return Formula::make_unary(Formula::OpType::Not, parse_unary());
    } else if (peek().type == Token::Type::Next) {
        consume();
        return Formula::make_unary(Formula::OpType::Next, parse_unary());
    } else {
        return parse_postfix();
    }
}

Formula::Ptr FormulaParser::parse_postfix() {
    if (peek().type == Token::Type::LParen) {
        consume();
        auto expr = parse_formula();
        if (peek().type != Token::Type::RParen) {
            set_error("Expected ')'");
            return nullptr;
        }
        consume();
        return expr;
    } else {
        return parse_primary();
    }
}

Formula::Ptr FormulaParser::parse_primary() {
    auto token = peek();
    if (token.type == Token::Type::True) {
        consume();
        return Formula::make_true();
    } else if (token.type == Token::Type::False) {
        consume();
        return Formula::make_false();
    } else if (token.type == Token::Type::Identifier) {
        consume();
        return Formula::make_literal(token.value);
    } else {
        set_error("Expected primary expression");
        return nullptr;
    }
}

} // namespace formula