# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

IMPORTANT: When making git commits, DO NOT add "Co-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>" line to commit messages.

## Project Overview

This project provides a high-performance C++17 implementation of Linear Temporal Logic on finite traces (LTLf) synthesis. It is designed as a robust framework for LTLf operations, encompassing everything from formula manipulation to game-based synthesis solving.

## Core Features

- Immutable Formula Representation (Hash Consing): Ensures formula uniqueness and enables $O(1)$ equality checks through structural hashing.
- Comprehensive Formula Manipulation: Supports parsing, simplification, and transformation of temporal logic formulas with rich operator support.
- Extensive Test Coverage: Includes a comprehensive suite of tests to ensure the reliability and correctness of the formula operations.

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
cmake --build build --target test
cmake --build build --target test -- ARGS="-V"  # Run all tests with verbose output

# Alternative test command
ctest --test-dir build --output-on-failure
```

## Build Directory Policy

- Mandate out-of-source builds: All build artifacts must be isolated in a dedicated directory (typically named `build/`)
- Prohibit in-source builds: Never run `cmake .` or `make` in the project root to avoid polluting the source tree
- Do not invoke builds from source subdirectories: Avoid running build commands within directories containing source code (e.g., `src/`, `tests/`)
- Use `cmake -S . -B build` to configure from project root into dedicated build directory
- Use `cmake --build build` to build from project root without changing directories
- Clean up temporary build directories after use if created in other locations
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
cmake --build build --target test
cmake --build build --target test -- ARGS="-V" # if need verbose output

# Alternative: Run specific test using ctest
ctest --test-dir build -R formula_visitor_test

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

本项目采用基于 Vue.js 的 commit 约定，格式如下：

```
^(revert: )?(feat|fix|docs|dx|style|refactor|perf|test|workflow|build|ci|chore|types|wip)(\(.+\))?: .{1,50}
```

### 规则
- **type**: 必需。表示提交的类型。
  - `feat`: 新功能
  - `fix`: 修复 bug
  - `docs`: 文档更新
  - `dx`: 开发者体验改进 (Developer Experience)
  - `style`: 代码风格调整 (不影响逻辑)
  - `refactor`: 重构 (不新增功能)
  - `perf`: 性能优化
  - `test`: 测试相关
  - `workflow`: 工作流相关
  - `build`: 构建系统相关
  - `ci`: CI/CD 相关
  - `chore`: 杂项 (如依赖更新)
  - `types`: 类型定义相关
  - `wip`: 工作进行中
- **scope**: 可选。中英文都可以，表示影响的模块，如 `formula`, `parser`, `git`, `CMake` 等。
- **description**: 必需。中文 + 英文专有名词/术语，描述变更内容，长度 1-50 字符。
- **revert**: 可选，用于撤销之前的提交。

### 示例
- `feat(formula): 实现 Formula 类的 to_string 方法`
- `fix(parser): 修复 unary 运算符解析优先级`
- `docs: 更新 README.md`
- `refactor(build): 重构 CMake 配置以支持多平台`
- `revert: feat(formula): 撤销新增的 simplify 方法`