#pragma once
#include "formula/utils.hpp"
#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include "formula/operator.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <range/v3/algorithm/contains.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/concat.hpp>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Cosy {

class PartVar
{
  private:
    std::unordered_set<unsigned int> X_vars_, Y_vars_;
    std::vector<unsigned int> X_var_vec_;
    std::vector<unsigned int> Y_var_vec_;

    void initXY_var_vec();

  public:
    PartVar(const std::unordered_set<unsigned int> &X_parts, const std::unordered_set<unsigned int> &Y_parts)
        : X_vars_(std::move(X_parts)),
          Y_vars_(std::move(Y_parts))
    {
        initXY_var_vec();
    }

    static PartVar createEmptyPartVar()
    {
        std::unordered_set<unsigned int> X_parts {}, Y_parts {};
        PartVar ret(X_parts, Y_parts);
        return ret;
    }

    PartVar createCopy() const
    {
        std::unordered_set<unsigned int> X_parts_copy(X_vars_);
        std::unordered_set<unsigned int> Y_parts_copy(Y_vars_);
        PartVar ret(X_parts_copy, Y_parts_copy);
        return ret;
    }

    size_t getXVarNum() const { return X_vars_.size(); }
    size_t getYVarNum() const { return Y_vars_.size(); }
    size_t getAllVarNum() const { return getXVarNum() + getYVarNum(); }
    bool isXVar(unsigned int var) const { return ranges::contains(X_vars_, var); }
    bool isYVar(unsigned int var) const { return ranges::contains(Y_vars_, var); }
    auto getAllVarIds() const { return ranges::views::concat(X_vars_, Y_vars_); }
    std::unordered_set<unsigned int> const &getXVarIdSet() const { return X_vars_; }
    std::unordered_set<unsigned int> const &getYVarIdSet() const { return Y_vars_; }
    std::vector<unsigned int> const &getXVarIds() const { return X_var_vec_; }
    std::vector<unsigned int> const &getYVarIds() const { return Y_var_vec_; }

    std::pair<Formula*, Formula*> *split_XY_from_edgeAf(Formula* af, FormulaBuilder& builder)
    {
        std::unordered_set<int> lits;
        collect_literals(af, lits);
        std::vector<Formula*> af_X_vec;
        std::vector<Formula*> af_Y_vec;
        for (auto lit_id : lits)
        {
            Formula* cur_var = builder.make_literal(lit_id);
            if (ranges::contains(X_vars_, abs(lit_id)))
                af_X_vec.push_back(cur_var);
            else if (ranges::contains(Y_vars_, abs(lit_id)))
                af_Y_vec.push_back(cur_var);
            else
                assert(false && "Variable in edge_af not found in either X_vars or Y_vars");
        }
        auto *XY_af_pair = new std::pair<Formula*, Formula*>(builder.make_ands(af_X_vec), builder.make_ands(af_Y_vec));
        return XY_af_pair;
    }
};

class PartVarBuilder
{
  private:
    std::unordered_set<unsigned int> X_vars_, Y_vars_;

  public:
    PartVarBuilder() : X_vars_(), Y_vars_() {}
    void partitionAtoms(const Formula* af, const std::unordered_set<std::string> &env_var_names)
    {
        std::unordered_set<int> var_set;
        collect_literals(af, var_set);
        for (int lit_id : var_set) {
            const std::string& var_name = get_global_symbol_table().get_var_name(abs(lit_id));
            if (ranges::contains(env_var_names, var_name))
                X_vars_.insert(abs(lit_id));
            else
                Y_vars_.insert(abs(lit_id));
        }
    }
    PartVar build(const Formula* af, const std::unordered_set<std::string> &env_var_names)
    {
        X_vars_.clear();
        Y_vars_.clear();
        partitionAtoms(af, env_var_names);
        return PartVar(X_vars_, Y_vars_);
    }

    static PartVar create(Formula* state_af, FormulaBuilder& builder, const std::unordered_set<std::string> &env_var_names);
};

} // namespace Cosy
