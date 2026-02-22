#pragma once

#include "cudd/cudd_config.hpp"
#include "formula/formula.hpp"
#include <cudd/cuddObj.hh>
#include <spdlog/fmt/fmt.h>
#include <string>

// fmt formatter for Formula
template <>
struct fmt::formatter<Cosy::Formula> : fmt::formatter<std::string>
{
    auto format(Cosy::Formula af, format_context& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "{}", af.toString());
    }
};

namespace Cosy {

/**
 * @brief Formula 在 BDD 中的表示
 *
 * 值对象，绑定了 Formula 和其对应的 BDD。
 * 实现 IHashId 接口，可用于 hash-based 容器。
 */
class FormulaInBdd : public IHashId
{
private:
    Formula* formula_;
    CUDD::BDD bdd_;

public:
    FormulaInBdd(Formula* af, CUDD::BDD bdd)
        : formula_(af)
        , bdd_(std::move(bdd))
    {
    }

    CUDD::BDD getBdd() const { return bdd_; }
    DdNode* getBddNode() const { return bdd_.getNode(); }
    Formula* getAfP() const { return formula_; }

    uint64_t getHashId() const override
    {
        return reinterpret_cast<uint64_t>(bdd_.getNode());
    }

    std::string toString() const override
    {
        return fmt::format("[{}]: {}", getHashId(), getAfP()->toString());
    }

    ~FormulaInBdd() override = default;
};

}  // namespace Cosy
