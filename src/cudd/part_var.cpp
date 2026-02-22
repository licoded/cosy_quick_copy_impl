#include "cudd/part_var.hpp"

namespace Cosy {

void sortVarsByNames(std::vector<unsigned int> &varId_vec)
{
    std::sort(varId_vec.begin(), varId_vec.end(),
              [](unsigned int varId1, unsigned int varId2) {
                  return get_global_symbol_table().get_var_name(varId1) < get_global_symbol_table().get_var_name(varId2);
              });
}

void PartVar::initXY_var_vec()
{
    auto sync_and_sort = [this](const std::unordered_set<unsigned int>& src, std::vector<unsigned int>& dest) {
        dest.assign(src.begin(), src.end());
        sortVarsByNames(dest);
    };
    sync_and_sort(X_vars_, X_var_vec_);
    sync_and_sort(Y_vars_, Y_var_vec_);
}

PartVar PartVarBuilder::create(Formula* state_af, FormulaBuilder& builder, const std::unordered_set<std::string> &env_var_names)
{
    Formula* stateAf_and_notTail = builder.make_binary(Operator::And, state_af, builder.make_not_tail());
    return PartVarBuilder().build(stateAf_and_notTail, env_var_names);
}

} // namespace Cosy
