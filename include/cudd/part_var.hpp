#pragma once
#include "cudd/formula_utils.hpp"
#include "formula/formula.hpp"
#include "formula/builder.hpp"
#include "formula/variable_collector.hpp"
#include "formula/operator.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Cosy {

void sortVarsByNames(std::vector<int> &varId_vec);

class PartVar
{
  private:
    std::unordered_set<int> X_vars_, Y_vars_;
    std::unordered_set<int> vars_;
    int X_var_num_, Y_var_num_;
    int var_num;

    std::vector<int> X_var_vec_;
    std::vector<int> Y_var_vec_;

  public:
    PartVar(std::unordered_set<int> &X_parts, std::unordered_set<int> &Y_parts)
        : X_vars_(std::move(X_parts)),
          Y_vars_(std::move(Y_parts)),
          vars_(),
          X_var_num_(X_vars_.size()),
          Y_var_num_(Y_vars_.size()),
          var_num(X_var_num_ + Y_var_num_)
    {
        vars_.insert(X_vars_.begin(), X_vars_.end());
        vars_.insert(Y_vars_.begin(), Y_vars_.end());
        initXY_var_vec();
    }

    static PartVar createEmptyPartVar()
    {
        std::unordered_set<int> X_parts {}, Y_parts {};
        PartVar ret(X_parts, Y_parts);
        return ret;
    }

    PartVar createCopy() const
    {
        std::unordered_set<int> X_parts_copy(X_vars_);
        std::unordered_set<int> Y_parts_copy(Y_vars_);
        PartVar ret(X_parts_copy, Y_parts_copy);
        return ret;
    }

    void initXY_var_vec()
    {
        X_var_vec_.clear();
        X_var_vec_.insert(X_var_vec_.end(), X_vars_.begin(), X_vars_.end());
        sortVarsByNames(X_var_vec_);
        Y_var_vec_.clear();
        Y_var_vec_.insert(Y_var_vec_.end(), Y_vars_.begin(), Y_vars_.end());
        sortVarsByNames(Y_var_vec_);
    }

    int getAllVarNum() const { return var_num; }
    int getXVarNum() const { return X_var_num_; }
    int getYVarNum() const { return Y_var_num_; }
    bool isXVar(int var) const { return X_vars_.find(var) != X_vars_.end(); }
    bool isYVar(int var) const { return Y_vars_.find(var) != Y_vars_.end(); }
    std::unordered_set<int> const &getAllVarIds() const { return vars_; }
    std::unordered_set<int> const &getXVarIdSet() const { return X_vars_; }
    std::unordered_set<int> const &getYVarIdSet() const { return Y_vars_; }
    std::vector<int> const &getXVarIds() const { return X_var_vec_; }
    std::vector<int> const &getYVarIds() const { return Y_var_vec_; }

    std::pair<Formula*, Formula*> *split_XY_from_edgeAf(Formula* af, FormulaBuilder& builder)
    {
        std::unordered_set<int> edge_var_set;
        collect_var_ids(af, edge_var_set);
        std::vector<Formula*> af_X_vec;
        std::vector<Formula*> af_Y_vec;
        for (auto it : edge_var_set)
        {
            // Create literal formula for this variable
            Formula* cur_var = builder.make_literal(get_global_symbol_table().get_var_name(abs(it)));
            if (it < 0)
                cur_var = builder.make_unary(Operator::Not, cur_var);
            assert(X_vars_.find(abs(it)) != X_vars_.end() || Y_vars_.find(abs(it)) != Y_vars_.end());
            if (X_vars_.find(abs(it)) != X_vars_.end())
                af_X_vec.push_back(cur_var);
            else
                af_Y_vec.push_back(cur_var);
        }
        std::pair<Formula*, Formula*> *XY_af_pair
            = new std::pair<Formula*, Formula*>(formula_conjunction(builder, af_X_vec), formula_conjunction(builder, af_Y_vec));
        return XY_af_pair;
    }
};

class PartVarBuilder
{
  private:
    std::unordered_set<int> X_vars_, Y_vars_;

  public:
    PartVarBuilder() : X_vars_(), Y_vars_() {}
    void partitionAtoms(const Formula* af, const std::unordered_set<std::string> &env_var_names)
    {
        if (af == nullptr) return;

        Operator op = af->op();
        switch (op)
        {
        case Operator::True:
        case Operator::False:
            break;
        case Operator::Not:
        case Operator::Next:
        case Operator::WNext:
            partitionAtoms(af->right(), env_var_names);
            break;
        case Operator::And:
        case Operator::Or:
        case Operator::Until:
        case Operator::Release:
            partitionAtoms(af->left(), env_var_names);
            partitionAtoms(af->right(), env_var_names);
            break;
        case Operator::Literal:
            /* For literal, check if it's an environment variable */
            {
                std::string var_name = get_global_symbol_table().get_var_name(af->var_id());
                if (env_var_names.find(var_name) != env_var_names.end())
                    X_vars_.insert(af->var_id());
                else
                    Y_vars_.insert(af->var_id());
            }
            break;
        default:
            break;
        }
    }
    PartVar build(const Formula* af, const std::unordered_set<std::string> &env_var_names)
    {
        partitionAtoms(af, env_var_names);
        return PartVar(X_vars_, Y_vars_);
    }
};

PartVar makePartVar(Formula* state_af, FormulaBuilder& builder, const std::unordered_set<std::string> &env_var_names);

} // namespace Cosy
