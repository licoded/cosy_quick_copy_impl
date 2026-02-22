#pragma once

#include "cudd/checker.hpp"
#include "cudd/core.hpp"
#include "cudd/formula_cache.hpp"
#include "cudd/formula_in_bdd.hpp"
#include "cudd/var_manager.hpp"
#include "formula/formula.hpp"
#include "formula/operator.hpp"
#include "formula/utils.hpp"
#include <cudd/cuddObj.hh>
#include <spdlog/spdlog.h>
#include <vector>

namespace Cosy {

/**
 * @brief Cudd 管理器门面
 *
 * 组合所有 CUDD 相关组件，对外提供统一接口。
 * 替代原来的 ICuddMgr + FormulaInBddMgr。
 */
class CuddMgr
{
private:
    CuddCore core_;
    BddVarManager var_mgr_;
    FormulaBddCache cache_;
    std::vector<std::string> var_names_;

    // === 内部构建方法 ===

    void buildClauses(Formula* af)
    {
        if (af == nullptr)
            return;
        if (cache_.hasBuilt(af))
            return;

        Operator op = af->op();

        // 对于 literal，应该在 fixAtomOrder 中初始化
        if (op == Operator::Literal)
        {
            if (!cache_.hasBuilt(af))
            {
                exit_with_error("All atoms should be init in the beginning! Found uninitialized atom: " + af->toString());
            }
            return;
        }

        switch (op)
        {
            case Operator::True:
            case Operator::False:
                break;
            case Operator::Next:
            case Operator::WNext:
                cache_.buildIfMissing(af, core_);
                break;
            case Operator::Not:
                buildClauses(af->right());
                break;
            case Operator::Until:
            case Operator::Release:
                if (is_tail(af) || is_not_tail(af))
                    break;
                exit_with_error("Please convert the formula to XNF first!");
                break;
            case Operator::And:
            case Operator::Or:
                buildClauses(af->left());
                buildClauses(af->right());
                break;
            default:
                break;
        }
    }

    CUDD::BDD constructBdd(Formula* af)
    {
        if (af == nullptr)
            exit_with_error("[constructBdd] the formula is NULL!");

        auto it = cache_.getAfPToBddP().find(reinterpret_cast<uint64_t>(af));
        if (it != cache_.getAfPToBddP().end())
        {
            return it->second;
        }

        Operator op = af->op();
        CUDD::BDD res;

        switch (op)
        {
            case Operator::Not:
            {
                CUDD::BDD tmp = constructBdd(af->right());
                res = !tmp;
                break;
            }
            case Operator::And:
            {
                CUDD::BDD l_bdd = constructBdd(af->left());
                CUDD::BDD r_bdd = constructBdd(af->right());
                res = l_bdd & r_bdd;
                break;
            }
            case Operator::Or:
            {
                CUDD::BDD l_bdd = constructBdd(af->left());
                CUDD::BDD r_bdd = constructBdd(af->right());
                res = l_bdd | r_bdd;
                break;
            }
            default:
            {
                spdlog::error("[constructBdd] for {}", af->toString());
                exit_with_error("[constructBdd] Atom, Next, WNext should be already built!");
            }
        }

        cache_.recordWithoutVec(af, res);
        return res;
    }

    CUDD::BDD convertFormula2Bdd(Formula* af)
    {
        if (!cache_.hasBuilt(af))
            constructBdd(af);
        return cache_.getBdd(af);
    }

    // === 初始化方法 ===

    void fixAtomOrder()
    {
        std::vector<Formula*> atoms = var_mgr_.getAtoms();
        for (auto atom : atoms)
        {
            cache_.buildIfMissing(atom, core_);
        }
        for (auto atom : atoms)
        {
            var_names_.push_back(atom->toString());
        }
    }

    void initTailBdd()
    {
        Formula* tail = var_mgr_.makeTail();
        if (!cache_.hasBuilt(tail))
        {
            CUDD::BDD tail_bdd = core_.newBddVar();
            cache_.record(tail, tail_bdd);

            Formula* not_tail = var_mgr_.makeNotTail();
            CUDD::BDD not_tail_bdd = !tail_bdd;
            cache_.record(not_tail, not_tail_bdd);
        }
    }

    void initTrueFalse()
    {
        cache_.recordWithoutVec(var_mgr_.makeTrue(), core_.trueBdd());
        cache_.recordWithoutVec(var_mgr_.makeFalse(), core_.falseBdd());
    }

public:
    explicit CuddMgr(PartVar part_var, FormulaBuilder& builder)
        : core_()
        , var_mgr_(std::move(part_var), builder)
        , cache_()
        , var_names_()
    {
        fixAtomOrder();
        initTailBdd();
        initTrueFalse();
    }

    ~CuddMgr() = default;

    // 禁止拷贝
    CuddMgr(const CuddMgr&) = delete;
    CuddMgr& operator=(const CuddMgr&) = delete;

    // === CuddCore 委托 ===

    CUDD::Cudd& cudd() { return core_.cudd(); }
    DdManager* getManager() { return core_.getManager(); }
    CUDD::BDD trueBdd() const { return core_.trueBdd(); }
    CUDD::BDD falseBdd() const { return core_.falseBdd(); }
    CUDD::BDD newBddVar() { return core_.newBddVar(); }
    static DdNode* getNode(const CUDD::BDD& bdd) { return CuddCore::getNode(bdd); }

    // === BddVarManager 委托 ===

    PartVar& getPartVar() { return var_mgr_.getPartVar(); }
    FormulaBuilder& getBuilder() { return var_mgr_.getBuilder(); }
    auto getAllVarIds() const { return var_mgr_.getAllVarIds(); }
    std::pair<Formula*, Formula*>* splitXY(Formula* af) { return var_mgr_.splitXY(af); }

    // === 静态工具方法 ===

    static bool isYVar(const CUDD::BDD& bdd, int y_var_num)
    {
        return CuddCore::isYVar(bdd, y_var_num);
    }

    static bool isXYVar(const CUDD::BDD& bdd, int all_var_num)
    {
        return CuddCore::isXYVar(bdd, all_var_num);
    }

    // === FormulaInBdd 工厂 ===

    FormulaInBdd* createFormulaInBdd(Formula* af, Formula* xnf_af)
    {
        buildClauses(xnf_af);
        CUDD::BDD bdd = convertFormula2Bdd(xnf_af);
        spdlog::debug("CuddMgr::createFormulaInBdd\n{}\n{}\n{}",
                      af->toString(), xnf_af->toString(),
                      reinterpret_cast<uint64_t>(bdd.getNode()));
        return new FormulaInBdd(af, std::move(bdd));
    }

    // === 变量查询 ===

    Formula* getCurAfVar(DdNode* bddP) const
    {
        CUDD::BDD bdd(const_cast<CUDD::Cudd&>(core_.cudd()), bddP);
        if (!isXYVar(bdd, static_cast<int>(var_mgr_.getAllVarNum())))
            exit_with_error("[getCurAfVar] BDD node is not a variable!");
        return cache_.getFormulaByIndex(bdd.NodeReadIndex());
    }

    // === 逻辑检查 ===

    bool checkImplies(const CUDD::BDD& f1, const CUDD::BDD& f2) const
    {
        return BddChecker::checkImplies(f1, f2, core_.falseBdd());
    }

    bool checkConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2) const
    {
        return BddChecker::checkConflicts(f1, f2, core_.falseBdd());
    }

    bool checkImplies(Formula* f1, Formula* f2)
    {
        CUDD::BDD bdd1 = convertFormula2Bdd(f1);
        CUDD::BDD bdd2 = convertFormula2Bdd(f2);
        return checkImplies(bdd1, bdd2);
    }

    bool checkConflicts(Formula* f1, Formula* f2)
    {
        CUDD::BDD bdd1 = convertFormula2Bdd(f1);
        CUDD::BDD bdd2 = convertFormula2Bdd(f2);
        return checkConflicts(bdd1, bdd2);
    }

    // === 向后兼容别名（deprecated，后续可移除） ===

    // 为了兼容旧代码，提供大写开头的别名
    bool CheckImplies(const CUDD::BDD& f1, const CUDD::BDD& f2) const
    {
        return checkImplies(f1, f2);
    }

    bool CheckConflicts(const CUDD::BDD& f1, const CUDD::BDD& f2) const
    {
        return checkConflicts(f1, f2);
    }

    bool CheckImplies(Formula* f1, Formula* f2)
    {
        return checkImplies(f1, f2);
    }

    bool CheckConflicts(Formula* f1, Formula* f2)
    {
        return checkConflicts(f1, f2);
    }
};

}  // namespace Cosy
