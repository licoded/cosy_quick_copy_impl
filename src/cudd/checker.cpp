#include "cudd/checker.hpp"
#include "cudd/mgr.hpp"
#include "formula/formula.hpp"

namespace Cosy {

bool BddChecker::checkImplies(CuddMgr& mgr, Formula* f1, Formula* f2)
{
    CUDD::BDD bdd1 = mgr.convertFormula2Bdd(f1);
    CUDD::BDD bdd2 = mgr.convertFormula2Bdd(f2);
    return checkImplies(bdd1, bdd2, mgr.falseBdd());
}

bool BddChecker::checkConflicts(CuddMgr& mgr, Formula* f1, Formula* f2)
{
    CUDD::BDD bdd1 = mgr.convertFormula2Bdd(f1);
    CUDD::BDD bdd2 = mgr.convertFormula2Bdd(f2);
    return checkConflicts(bdd1, bdd2, mgr.falseBdd());
}

}  // namespace Cosy
