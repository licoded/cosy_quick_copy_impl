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

add_test(NAME formula_parser_test COMMAND formula_parser_test)