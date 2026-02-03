#include "formula/formula_parser.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <formula>\n";
        return 1;
    }

    formula::FormulaParser parser;
    auto formula = parser.parse(argv[1]);
    if (!formula) {
        std::cerr << "Parse error: " << parser.error() << "\n";
        return 1;
    }

    std::cout << "Parsed formula: " << formula->to_string() << "\n";

    auto vars = formula::Formula::collect_variables(formula);
    std::cout << "Variables: ";
    for (const auto &v : vars) {
        std::cout << v << " ";
    }
    std::cout << "\n";

    return 0;
}