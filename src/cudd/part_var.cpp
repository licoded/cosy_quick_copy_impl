#include "cudd/part_var.hpp"

namespace Cosy {

void sortVarsByNames(std::vector<int> &varId_vec)
{
    std::sort(varId_vec.begin(), varId_vec.end(),
              [](int varId1, int varId2) {
                  return get_global_symbol_table().get_var_name(varId1) < get_global_symbol_table().get_var_name(varId2);
              });
}

PartVar makePartVar(Formula* state_af, FormulaBuilder& builder, const std::unordered_set<std::string> &env_var_names)
{
    Formula* not_tail = builder.make_not_tail();
    Formula* stateAf_and_notTail = builder.make_binary(Operator::And, state_af, not_tail);
    return PartVarBuilder().build(stateAf_and_notTail, env_var_names);
}

} // namespace Cosy
