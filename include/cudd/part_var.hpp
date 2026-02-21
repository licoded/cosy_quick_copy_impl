#pragma once
#include <algorithm>
#include <cassert>
#include <cstring>
#include <formula/aalta_formula.h>
#include <formula/af_utils.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace syn_util {

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

    std::pair<aalta::aalta_formula *, aalta::aalta_formula *> *split_XY_from_edgeAf(aalta::aalta_formula *af)
    {
        std::unordered_set<int> edge_var_set;
        af->to_set(edge_var_set);
        std::vector<aalta::aalta_formula *> af_X_vec;
        std::vector<aalta::aalta_formula *> af_Y_vec;
        for (auto it : edge_var_set)
        {
            aalta::aalta_formula *cur_var = aalta::aalta_formula(abs(it), NULL, NULL).unique();
            if (it < 0)
                cur_var = aalta::aalta_formula(aalta::aalta_formula::Not, NULL, cur_var).unique();
            assert(X_vars_.find(abs(it)) != X_vars_.end() || Y_vars_.find(abs(it)) != Y_vars_.end());
            if (X_vars_.find(abs(it)) != X_vars_.end())
                af_X_vec.push_back(cur_var);
            else
                af_Y_vec.push_back(cur_var);
        }
        std::pair<aalta::aalta_formula *, aalta::aalta_formula *> *XY_af_pair
            = new std::pair<aalta::aalta_formula *, aalta::aalta_formula *>(formula_from(af_X_vec), formula_from(af_Y_vec));
        return XY_af_pair;
    }
};

class PartVarBuilder
{
  private:
    std::unordered_set<int> X_vars_, Y_vars_;

  public:
    PartVarBuilder() : X_vars_(), Y_vars_() {}
    void partitioinAtoms(const aalta::aalta_formula *af, const std::unordered_set<std::string> &env_var_names)
    {
        int op = af->oper();
        switch (op)
        {
        case aalta::aalta_formula::True:
        case aalta::aalta_formula::False:
            break;
        case aalta::aalta_formula::Not:
        case aalta::aalta_formula::Next:
        case aalta::aalta_formula::WNext:
            partitioinAtoms(af->r_af(), env_var_names);
            break;
        case aalta::aalta_formula::And:
        case aalta::aalta_formula::Or:
        case aalta::aalta_formula::Until:
        case aalta::aalta_formula::Release:
            partitioinAtoms(af->l_af(), env_var_names);
            partitioinAtoms(af->r_af(), env_var_names);
            break;
        default: // Atom
            /* For atom, _op is idx of atom in aalta_formula::names */
            if (env_var_names.find(af->to_string()) != env_var_names.end())
                X_vars_.insert(op);
            else
                Y_vars_.insert(op);
            break;
        }
    }
    PartVar build(const aalta::aalta_formula *af, const std::unordered_set<std::string> &env_var_names)
    {
        partitioinAtoms(af, env_var_names);
        // std::string s_x = "";
        // for (auto x_var : X_vars_)
        // {
        //     auto af = aalta::aalta_formula(x_var, NULL, NULL).unique();
        //     auto af_s = af->to_string();
        //     s_x += af_s + ", ";
        // }
        // std::string s_y = "";
        // for (auto y_var : Y_vars_)
        // {
        //     auto af = aalta::aalta_formula(y_var, NULL, NULL).unique();
        //     auto af_s = af->to_string();
        //     s_y += af_s + ", ";
        // }
        // spdlog::warn("X_vars:\t{}", s_x);
        // spdlog::warn("Y_vars:\t{}", s_y);
        return PartVar(X_vars_, Y_vars_);
    }
};

PartVar makePartVar(aalta::aalta_formula *state_af, const std::unordered_set<std::string> &env_var_names);

} // namespace syn_util