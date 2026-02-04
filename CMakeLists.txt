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
    src/formula/formula_parser.cpp
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
