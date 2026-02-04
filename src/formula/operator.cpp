#include "formula/operator.hpp"

namespace Cosy {

void initialize_operator_names(std::vector<std::string>& names) {
    if (names.empty()) {
        names.push_back("true");
        names.push_back("false");
        names.push_back("Literal");
        names.push_back("!");
        names.push_back("|");
        names.push_back("&");
        names.push_back("X[!]");
        names.push_back("X");
        names.push_back("U");
        names.push_back("R");
        names.push_back("Undefined");
    }
}

unsigned int register_variable_name(const std::string& var_name,
                                     std::vector<std::string>& names,
                                     std::unordered_map<std::string, int>& ids) {
    auto it = ids.find(var_name);
    unsigned int id;
    if (it == ids.end()) {
        id = names.size();
        ids[var_name] = id;
        names.push_back(var_name);
    } else {
        id = it->second;
    }
    return id;
}

} // namespace Cosy
