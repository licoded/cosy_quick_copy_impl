#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <memory>
#include <string>
#include <unordered_set>

namespace formula {

class Formula final
{
  public:
    using Ptr = std::shared_ptr<Formula>;

    enum class OpType
    {
        True,
        False,
        Literal,
        Not,
        And,
        Or,
        Next,
        Until,
        Release
    };

    static Ptr make_true();
    static Ptr make_false();
    static Ptr make_literal(std::string name);
    static Ptr make_unary(OpType op, Ptr operand);
    static Ptr make_binary(OpType op, Ptr left, Ptr right);

    OpType op() const { return op_; }
    const Ptr &left() const { return left_; }
    const Ptr &right() const { return right_; }
    const std::string &literal() const { return literal_; }
    bool is_binary() const;
    bool is_unary() const;
    bool is_constant() const;
    std::string to_string() const;
    const char *op_name() const;

    static std::unordered_set<std::string> collect_variables(const Ptr &formula);

  public:
    Formula(OpType op, std::string literal, Ptr left, Ptr right);

    OpType op_;
    std::string literal_;
    Ptr left_;
    Ptr right_;
};

} // namespace formula

#endif // FORMULA_HPP
