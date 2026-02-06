# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++ implementation of a logical formula library with focus on Linear Temporal Logic (LTL) operations. The project includes a formula parser, simplification engine, and visitor pattern implementation for formula manipulation.

## Architecture

### Core Components
- **Formula**: Represents logical formulas with operators like And, Or, Next, WNext, Until, Release
- **Operator**: Enum and metadata system for different logical operators with arities
- **SynthesisContext**: Manages formula creation and parsing
- **Visitor Pattern**: Enables traversal and manipulation of formula trees
- **Simplification Engine**: Implements rules to simplify formulas

### Key Files
- `include/formula/formula.hpp`: Core Formula class definition
- `include/formula/operator.hpp`: Operator definitions and metadata
- `include/formula/visitor.hpp`: Visitor pattern interface
- `src/formula/simplify/*.cpp`: Individual operator simplification logic
- `include/formula/simplification_visitor.hpp`: Simplification visitor implementation

### Formula Types
- Nullary: True, False, Literal (0 operands)
- Unary: Not, Strong Next(X[!]), Weak Next(X) (1 operand)
- Binary: And(&), Or(|), Until(U), Release(R) (2 operands)

## Build System

The project uses CMake with the following targets:
- `formula`: Static library containing the core logic
- `formula_cli`: Executable for parsing and simplifying formulas
- Various test executables generated from tests/CMakeLists.txt

## Build Commands

```bash
# Configure and build
cmake .
make

# Alternative build process
mkdir build && cd build
cmake ..
make

# Run all tests
ctest --output-on-failure

# Run specific test
ctest -R formula_visitor_test
```

## Testing

Tests are organized in the `tests/` directory using Catch2 framework:
- `tests/formula/parser/*`: Parser tests
- `tests/formula/simplify/*`: Simplification tests for individual operators
- `tests/formula/visitor_test.cpp`: Visitor pattern tests
- `tests/formula/hash_consing.cpp`: Hash consing tests

To run a specific test:
```bash
# Build and run a specific test
./build/tests/formula_visitor_test
```

## Development Guidelines

- Use Chinese for comments and English for variable names in mixed-language contexts
- Follow the visitor pattern for traversing and manipulating formulas
- Formula objects are immutable after creation (except for hash calculation)
- Simplification follows specific mathematical rules for each operator
- Hash-consing is used to avoid duplicate formulas

## Git Commit Convention

The project uses Vue.js-style commit convention:
```
<type>(<scope>): <description>
```

Types include:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation update
- `refactor`: Refactoring
- `perf`: Performance improvement
- `test`: Test related
- `build`: Build system related
- `chore`: Miscellaneous changes

Example: `feat(formula): 实现 Formula 类的 to_string 方法`