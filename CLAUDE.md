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
# Recommended: Complete rebuild without changing directories
rm -rf build && cmake -S . -B build && cmake --build build --target all -j

# Alternative: Configure and build without changing directories
cmake -S . -B build  # Configure
cmake --build build --target all -j  # Build all targets with parallel jobs

# Build specific target
cmake --build build --target formula_cli -j

# Run all tests
cmake --build build --target test -- -V  # Run tests with verbose output

# Alternative test command
cd build && ctest --output-on-failure
```

# Build Directory Policy

- ALWAYS perform builds using out-of-source build directory (`build/`)
- Use `cmake -S . -B build` to configure without changing directories
- Use `cmake --build build` to build without changing directories
- NEVER run `cmake .` or `make` in the project root directory
- NEVER run builds in any subdirectories of the project (like `tests/`)
- If temporary builds are needed elsewhere, use `/tmp` or similar external temporary directories
- Clean up temporary build directories after use
- The `build/` directory is already in `.gitignore` and is the designated location for all build artifacts
- Testing artifacts and temporary files (including Testing/Temporary/) will be placed in build directory, not in source directories

## Testing

Tests are organized in the `tests/` directory using Catch2 framework:
- `tests/formula/parser/*`: Parser tests
- `tests/formula/simplify/*`: Simplification tests for individual operators
- `tests/formula/visitor_test.cpp`: Visitor pattern tests
- `tests/formula/hash_consing.cpp`: Hash consing tests

Test executables are built in the `build/` directory:
```bash
# Build and run all tests (recommended)
cmake --build build --target test -- -V

# Alternative: Run specific test using ctest
cd build && ctest -R formula_visitor_test

# Direct execution of test executable
./build/tests/formula_visitor_test
```

## Development Guidelines

- Use Chinese for comments and English for variable names in mixed-language contexts
- Follow the visitor pattern for traversing and manipulating formulas
- Formula objects are immutable after creation (except for hash calculation)
- Simplification follows specific mathematical rules for each operator
- Hash-consing is used to avoid duplicate formulas
- NEVER perform builds in the project root directory or in subdirectories like `tests/`
- All builds must be done in the designated `build/` directory to keep source clean
- Test output and temporary files will be placed in build directory, not in source directories
- If temporary builds are necessary elsewhere, use external directories like `/tmp` and clean up afterward
- Remove any temporary build artifacts immediately after they're no longer needed

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