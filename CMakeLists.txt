cmake_minimum_required(VERSION 3.22)

project(CosyQuickCopyImpl VERSION 0.1 LANGUAGES CXX C)

# ------------------------------------------------------------------
# Build options
# ------------------------------------------------------------------
option(BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------------------
# Formula library
# ------------------------------------------------------------------
add_library(formula STATIC
    src/formula/formula.cpp
    src/formula/operator.cpp
    src/formula/builder.cpp
    src/formula/stringifier.cpp
    src/formula/synthesis_context.cpp
    src/formula/hash.cpp
    src/formula/simplify/util.cpp
    src/formula/simplify/and.cpp
    src/formula/simplify/or.cpp
    src/formula/simplify/next.cpp
    src/formula/simplify/wnext.cpp
    src/formula/simplify/until.cpp
    src/formula/simplify/release.cpp
    src/formula/simplify/simplifier.cpp
    src/formula/trans/xnf.cpp
    src/formula/utils.cpp
    deps/internal/ltlparser/ltl_formula.c
    deps/internal/ltlparser/ltllexer.c
    deps/internal/ltlparser/ltlparser.c
    deps/internal/ltlparser/trans.c
)
target_include_directories(formula PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/external
)
target_compile_features(formula PUBLIC cxx_std_20)

# ------------------------------------------------------------------
# Cudd utils library
# ------------------------------------------------------------------
add_library(cudd_utils STATIC
    src/cudd/part_var.cpp
)
target_include_directories(cudd_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/external
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/external/range-v3
)
target_link_libraries(cudd_utils PUBLIC formula)
target_compile_features(cudd_utils PUBLIC cxx_std_20)

# ------------------------------------------------------------------
# Main executable
# ------------------------------------------------------------------
add_executable(formula_cli src/main.cpp)
target_include_directories(formula_cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(formula_cli PUBLIC formula)

if(BUILD_TESTS)
    enable_testing()
    find_package(Catch2 REQUIRED)
    add_subdirectory(tests)
endif()
