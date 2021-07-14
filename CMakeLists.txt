cmake_minimum_required(VERSION 3.17)
project(plasma)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
set(SOURCE_FILES
        src/ast.cpp
        src/parser.cpp
        src/lexer.cpp
        src/reader.cpp
        src/error.cpp
        src/symbols.cpp
        src/value.cpp
        src/bytecode.cpp
        src/method_call.cpp
        src/context.cpp
        src/force.cpp
        src/memory.cpp
        src/hash.cpp
        src/construct.cpp
        src/tools.cpp
        )

set(TEST_SOURCE_FILES
        test/src/test_lexer.cpp
        test/src/test_parser.cpp
        test/src/print.cpp
        )

add_executable(plasma
        main.cpp
        ${SOURCE_FILES}
        )

add_executable(test
        test/main.cpp
        ${SOURCE_FILES}
        ${TEST_SOURCE_FILES}
        )

target_include_directories(test PRIVATE test/include)