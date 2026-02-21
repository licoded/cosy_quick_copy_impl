#pragma once
#include "debug.h"
#include "formula/aalta_formula.h"
#include "synutil/part_var.h"
#include <algorithm>
#include <cudd/cuddObj.hh>
#include <spdlog/spdlog.h>
#include <vector>

namespace syn_util {

void print_succinct_info(DdManager *mgr);

class ICuddMgr
{
    // === for part_var ===
  protected:
    PartVar part_var_;

  public:
    bool need_print_ = true;
    PartVar &getPartVar() { return part_var_; }
    bool isYVar(DdNode *addP) const { return Cudd_NodeReadIndex(addP) < part_var_.getYVarNum(); }
    bool isXYVar(DdNode *addP) const { return Cudd_NodeReadIndex(addP) < part_var_.getAllVarNum(); }
    std::unordered_set<int> const &getAllVarIds() const { return part_var_.getAllVarIds(); }
    std::pair<aalta::aalta_formula *, aalta::aalta_formula *> *split_XY_from_edgeAf(aalta::aalta_formula *af)
    {
        return part_var_.split_XY_from_edgeAf(af);
    }

    // === for cudd_mgr ===
  protected:
    std::vector<std::string> var_names_;

  private:
    DdManager *cudd_mgr_;

    void createCuddMgr()
    {
        cudd_mgr_ = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS * aalta::times_UNIQUE_SLOTS, CUDD_CACHE_SLOTS * aalta::times_CACHE_SLOTS, 0);
        Cudd_RegisterOutOfMemoryCallback(cudd_mgr_, [](size_t size) {
            exit_with_error("CUDD ran out of memory while trying to allocate " + std::to_string(size) + " bytes!");
        });
        if (cudd_mgr_ == NULL)
            exit_with_error("Failed to initialize CUDD cudd_mgr_!");
    }

    void initTrueFalseBdd()
    {
        TRUE_bddP_ = Cudd_ReadOne(cudd_mgr_);
        FALSE_bddP_ = Cudd_ReadLogicZero(cudd_mgr_);
        Cudd_Ref(TRUE_bddP_);
        Cudd_Ref(FALSE_bddP_);
    }

  public:
    DdNode *TRUE_bddP_;
    DdNode *FALSE_bddP_;

    DdManager *GetCuddMgr() { return cudd_mgr_; };

    void Cudd_Ref(DdNode *node) { ::Cudd_Ref(node); }
    void Cudd_Unref(DdNode *node) { Cudd_RecursiveDeref(cudd_mgr_, node); }
    DdNode *Cudd_Ref_Wrapper(DdNode *node)
    {
        Cudd_Ref(node);
        return node;
    }

    void exportDot(const std::string &filename, DdNode *node) const
    {
        std::vector<char *> var_names_cstr;
        std::transform(var_names_.begin(), var_names_.end(), std::back_inserter(var_names_cstr),
                       [](const std::string &var_name) { return const_cast<char *>(var_name.c_str()); });
        FILE *output = fopen(filename.c_str(), "w");
        if (output)
        {
            Cudd_DumpDot(cudd_mgr_, 1, &node, var_names_cstr.data(), NULL, output);
            fclose(output);
        }
        else
        {
            perror("Error output file");
        }
    }

    // === for Bdd ===
    DdNode *newBddVar()
    {
        DdNode *var = Cudd_bddNewVar(cudd_mgr_);
        return var;
    }
    DdNode *Cudd_bddAnd(DdNode *a, DdNode *b) { return ::Cudd_bddAnd(cudd_mgr_, a, b); }
    DdNode *Cudd_bddOr(DdNode *a, DdNode *b) { return ::Cudd_bddOr(cudd_mgr_, a, b); }
    DdNode *Cudd_bddNot(DdNode *a) { return Cudd_Not(a); }

    // === for Add ===
    DdNode *newAddVar()
    {
        DdNode *var = Cudd_addNewVar(cudd_mgr_);
        Cudd_Ref(var);
        return var;
    }
    DdNode *getAddConst(int value)
    {
        DdNode *ret = Cudd_addConst(cudd_mgr_, value);
        Cudd_Ref(ret);
        return ret;
    }
    u_int64_t getAddConstValue(DdNode *addP) { return Cudd_V(addP); }

    DdNode *ADD_Apply(DdNode *a, DdNode *b, DD_AOP op)
    {
        DdNode *ret = Cudd_addApply(cudd_mgr_, op, a, b);
        Cudd_Ref(ret);
        Cudd_Unref(a);
        Cudd_Unref(b);
        return ret;
    }
    DdNode *ADD_Plus(DdNode *a, DdNode *b) { return ADD_Apply(a, b, Cudd_addPlus); }
    DdNode *ADD_Times(DdNode *a, DdNode *b) { return ADD_Apply(a, b, Cudd_addTimes); }
    DdNode *ADD_Not(DdNode *a)
    {
        DdNode *ret = Cudd_addCmpl(cudd_mgr_, a);
        Cudd_Ref(ret);
        Cudd_Unref(a);
        return ret;
    }

    // === fixAtomOrder ===
  protected:
    std::vector<aalta::aalta_formula *> getAtoms()
    {
        std::vector<aalta::aalta_formula *> atoms;
        std::transform(part_var_.getYVarIds().begin(), part_var_.getYVarIds().end(), std::back_inserter(atoms),
                       [](int varId) { return aalta::aalta_formula(varId, NULL, NULL).unique(); });
        std::transform(part_var_.getXVarIds().begin(), part_var_.getXVarIds().end(), std::back_inserter(atoms),
                       [](int varId) { return aalta::aalta_formula(varId, NULL, NULL).unique(); });
        return atoms;
    }
    virtual void fixAtomOrder(const std::vector<aalta::aalta_formula *> &atoms) = 0;

    // === ctor and dtor
  public:
    explicit ICuddMgr(PartVar part_var) : part_var_(part_var)
    {
        createCuddMgr();
        initTrueFalseBdd();
    }
    void releaseMgr()
    {
        if (cudd_mgr_ != NULL)
        {
            if (aalta::PRINT_CUDD_FLAG && need_print_)
            {
                spdlog::critical("=== BEGIN ------ CUDD manager info === 0x{:x}", u_int64_t(this));
                print_succinct_info(cudd_mgr_);
                spdlog::critical("=== END   ------ CUDD detail  info ===");
            }
            Cudd_Quit(cudd_mgr_);
            cudd_mgr_ = NULL;
        }
    }
    ~ICuddMgr() { releaseMgr(); }

    // === trans in cudd tree
  protected:
    std::vector<unsigned int> af_atomOper_vec_;

  public:
    DdNode *transByEdgeAf(DdNode *root_ddP, aalta::aalta_formula *edge_af)
    {
        std::unordered_set<int> lit_set;
        edge_af->to_set(lit_set);
        DdNode *cur_ddP = Cudd_Ref_Wrapper(root_ddP);
        while (isXYVar(cur_ddP))
        {
            unsigned int varId = af_atomOper_vec_.at(Cudd_NodeReadIndex(cur_ddP));
            DdNode *true_addP = Cudd_IsComplement(cur_ddP) ? ADD_Not(cur_ddP) : cur_ddP;
            if (lit_set.find(varId) != lit_set.end())
                cur_ddP = Cudd_T(true_addP);
            else
                cur_ddP = Cudd_E(true_addP);
            Cudd_Unref(true_addP);
            Cudd_Ref(cur_ddP);
        }
        return cur_ddP;
    }

    // === for edge_cons_builder
  public:
    virtual aalta::aalta_formula *getCurAfVar(DdNode *cuddP) const = 0;
    virtual DdNode *getRealCuddP(DdNode *raw_cuddP) = 0;
};

} // namespace syn_util