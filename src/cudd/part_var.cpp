#include "synutil/part_var.h"

namespace syn_util {

void sortVarsByNames(std::vector<int> &varId_vec)
{
    std::unordered_map<int, std::string> var_str_map;
    for (int varId : varId_vec)
        var_str_map.insert({varId, aalta::aalta_formula(varId, NULL, NULL).unique()->to_string()});
    std::sort(varId_vec.begin(), varId_vec.end(),
              [var_str_map](int varId1, int varId2) { return var_str_map.at(varId1) < var_str_map.at(varId2); });
}

PartVar makePartVar(aalta::aalta_formula *state_af, const std::unordered_set<std::string> &env_var_names)
{
    aalta::aalta_formula *stateAf_and_notTail
        = aalta::aalta_formula(aalta::aalta_formula::And, state_af, aalta::aalta_formula::NOT_TAIL()).unique();
    return syn_util::PartVarBuilder().build(stateAf_and_notTail, env_var_names);
}

} // namespace syn_util