#include "cudd/cudd_mgr_base.hpp"
#include "cudd/part_var.hpp"
#include "formula/utils.hpp"
#include "formula/builder.hpp"

namespace Cosy {

ICuddMgr::ICuddMgr(PartVar part_var, FormulaBuilder& builder)
    : part_var_(part_var)
    , builder_(builder)
    , cudd_(0, 0, CUDD_UNIQUE_SLOTS * CUDD_TIMES_UNIQUE_SLOTS, CUDD_CACHE_SLOTS * CUDD_TIMES_CACHE_SLOTS)
    , true_bdd_(cudd_.bddOne())
    , false_bdd_(cudd_.bddZero())
{
}

ICuddMgr::~ICuddMgr()
{
    // CUDD::Cudd 自动管理资源，无需手动释放
}

std::pair<Formula*, Formula*>* ICuddMgr::split_XY_from_edgeAf(Formula* af)
{
    return part_var_.split_XY_from_edgeAf(af, builder_);
}

}  // namespace Cosy
