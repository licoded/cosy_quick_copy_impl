function(add_formula_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE formula Catch2::Catch2WithMain)

    # Output to build/tests/
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endfunction()

add_formula_test(formula_parser_test formula/formula_parser_test.cpp)

add_test(NAME formula_parser_test COMMAND formula_parser_test)