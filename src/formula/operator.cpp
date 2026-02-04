#include "formula/operator.hpp"
#include <stdexcept>

namespace Cosy {

namespace {
    SymbolTable global_symbol_table;
}

SymbolTable& get_global_symbol_table() {
    return global_symbol_table;
}

void SymbolTable::initialize_operators() {
    if (names_.empty()) {
        names_.push_back("true");
        names_.push_back("false");
        names_.push_back("Literal");
        names_.push_back("!");
        names_.push_back("|");
        names_.push_back("&");
        names_.push_back("X[!]");
        names_.push_back("X");
        names_.push_back("U");
        names_.push_back("R");
        names_.push_back("Undefined");
    }
}

unsigned int SymbolTable::get_or_create_variable_id(const std::string& var_name) {
    auto it = ids_.find(var_name);
    if (it == ids_.end()) {
        unsigned int id = names_.size();
        ids_[var_name] = id;
        names_.push_back(var_name);
        return id;
    } else {
        return it->second;
    }
}

const std::string& SymbolTable::get_name(unsigned int id) const {
    if (id >= names_.size()) {
        throw std::out_of_range("Invalid symbol ID");
    }
    return names_[id];
}

} // namespace Cosy
