#ifndef FORMULA_PARSER_HPP
#define FORMULA_PARSER_HPP

#include "formula/formula.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace formula {

class FormulaParser
{
  public:
    explicit FormulaParser(std::unordered_map<std::string, int> variables = {});
    Formula::Ptr parse(const std::string &input);
    const std::string &error() const { return error_; }

  private:
    struct Token
    {
        enum class Type
        {
            Identifier,
            True,
            False,
            Not,
            And,
            Or,
            Until,
            Release,
            Next,
            LParen,
            RParen,
            End,
        };

        Type type = Type::End;
        std::string value;
    };

    void tokenize(const std::string &input);
    Token peek() const;
    Token consume();
    void set_error(const char *msg);

    Formula::Ptr parse_formula();
    Formula::Ptr parse_or();
    Formula::Ptr parse_until();
    Formula::Ptr parse_and();
    Formula::Ptr parse_unary();
    Formula::Ptr parse_postfix();
    Formula::Ptr parse_primary();

    std::vector<Token> tokens_;
    size_t pos_ = 0;
    std::string error_;
    std::unordered_map<std::string, int> variables_;
};

} // namespace formula

#endif // FORMULA_PARSER_HPP
