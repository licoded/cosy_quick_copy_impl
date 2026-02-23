function(add_formula_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE formula Catch2::Catch2WithMain)
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)

    # Output to build/tests/
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endfunction()

add_formula_test(formula_parser_base_test formula/parser/base.cpp)
add_formula_test(formula_parser_unary_test formula/parser/unary.cpp)
add_formula_test(formula_parser_precedence_test formula/parser/precedence.cpp)
add_formula_test(formula_parser_errors_test formula/parser/errors.cpp)
add_formula_test(formula_parser_manual_test formula/parser/manual.cpp)
add_formula_test(formula_hash_consing_test formula/hash_consing.cpp)
add_formula_test(formula_simplify_and_test formula/simplify/and.cpp)
add_formula_test(formula_simplify_or_test formula/simplify/or.cpp)
add_formula_test(formula_simplify_next_test formula/simplify/next.cpp)
add_formula_test(formula_simplify_wnext_test formula/simplify/wnext.cpp)
add_formula_test(formula_simplify_until_test formula/simplify/until.cpp)
add_formula_test(formula_simplify_release_test formula/simplify/release.cpp)
add_formula_test(formula_visitor_test formula/visitor_test.cpp)
add_formula_test(formula_unique_tail_test formula/unique/tail_not_tail.cpp)

add_test(NAME formula_parser_base_test COMMAND formula_parser_base_test)
add_test(NAME formula_parser_unary_test COMMAND formula_parser_unary_test)
add_test(NAME formula_parser_precedence_test COMMAND formula_parser_precedence_test)
add_test(NAME formula_parser_errors_test COMMAND formula_parser_errors_test)
add_test(NAME formula_parser_manual_test COMMAND formula_parser_manual_test)
add_test(NAME formula_hash_consing_test COMMAND formula_hash_consing_test)
add_test(NAME formula_simplify_and_test COMMAND formula_simplify_and_test)
add_test(NAME formula_simplify_or_test COMMAND formula_simplify_or_test)
add_test(NAME formula_simplify_next_test COMMAND formula_simplify_next_test)
add_test(NAME formula_simplify_wnext_test COMMAND formula_simplify_wnext_test)
add_test(NAME formula_simplify_until_test COMMAND formula_simplify_until_test)
add_test(NAME formula_simplify_release_test COMMAND formula_simplify_release_test)
add_test(NAME formula_visitor_test COMMAND formula_visitor_test)
add_test(NAME formula_unique_tail_test COMMAND formula_unique_tail_test)

# Tarjan SCC test
add_executable(tarjan_scc_test tarjan/tarjan_scc_test.cpp)
target_link_libraries(tarjan_scc_test PRIVATE Catch2::Catch2WithMain)
target_include_directories(tarjan_scc_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(tarjan_scc_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
add_test(NAME tarjan_scc_test COMMAND tarjan_scc_test)

# Tarjan JSONL demo
add_executable(tarjan_jsonl_demo tarjan/jsonl_demo.cpp)
target_include_directories(tarjan_jsonl_demo PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/deps/external
)
set_target_properties(tarjan_jsonl_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# DFA minimize test
add_executable(dfa_minimize_test dfa/minimize/test.cpp)
target_link_libraries(dfa_minimize_test PRIVATE dfa Catch2::Catch2WithMain)
target_include_directories(dfa_minimize_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/deps/external
)
set_target_properties(dfa_minimize_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
add_test(NAME dfa_minimize_test COMMAND dfa_minimize_test)

# DFA label test
add_executable(dfa_label_test dfa/minimize/test_label.cpp)
target_link_libraries(dfa_label_test PRIVATE dfa formula Catch2::Catch2WithMain)
target_include_directories(dfa_label_test PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/deps/external
)
set_target_properties(dfa_label_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
add_test(NAME dfa_label_test COMMAND dfa_label_test)