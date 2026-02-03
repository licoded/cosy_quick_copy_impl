#include "formula/formula_parser.h"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <formula>\n";
        return 1;
    }

    try {
        Cosy::Formula formula(argv[1]);
        std::cout << "Parsed formula: " << formula.toString() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
