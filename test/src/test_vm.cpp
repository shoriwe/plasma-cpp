#include "reader.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/bytecode_compiler.h"
#include "test_vm.h"
#include "print.h"

#include <filesystem>
#include <chrono>

const size_t initialMemory = 1;

static void test_success_expression(int *number_of_tests, int *success) {
    int vmTests = 0;
    int vmSuccess = 0;
    for (const auto &expressionDirectory : std::filesystem::directory_iterator("tests-samples/success/expressions")) {
        for (const auto &script : std::filesystem::directory_iterator(expressionDirectory.path())) {
            std::cout << "[?] Testing: " << script.path().string() << std::endl;
            vmTests++;
            plasma::reader::string_reader scriptReader;
            if (!plasma::reader::string_reader_new_from_file(&scriptReader, script.path().string())) {
                continue;
            }
            plasma::lexer::lexer scriptLexer(&scriptReader);
            plasma::parser::parser scriptParser(&scriptLexer);
            plasma::bytecode_compiler::compiler compiler(&scriptParser);
            plasma::error::error compilationError;
            plasma::vm::bytecode sourceCode;
            if (!compiler.compile(&sourceCode, &compilationError)) {
                FAIL(compilationError.string() + ": " + script.path().string());
                continue;
            }
            std::istringstream stdinFile;
            std::stringstream stdoutFile;
            std::stringstream stderrFile;
            plasma::vm::virtual_machine plasmaVM(stdinFile, stdoutFile, stderrFile);
            bool executionSuccess = false;
            // Initialize the context
            plasma::vm::context c(initialMemory);
            plasmaVM.initialize_context(&c);
            auto start = std::chrono::steady_clock::now();
            plasma::vm::value *result = plasmaVM.execute(&c, &sourceCode, &executionSuccess);
            auto end = std::chrono::steady_clock::now();
            if (!executionSuccess) {
                std::cout << stdoutFile.str() << " - ";
                FAIL(result->typeName + ": " + result->string);
                continue;
            }
            auto output = stdoutFile.str();
            if (output.find("False") != std::string::npos) {
                std::cout << output;
                FAIL(script.path().string());
                continue;
            }
            SUCCESS(script.path().string() + " -> " +
                    BLUE(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) +
                         " milliseconds"));
            vmSuccess++;
        }
    }
    (*number_of_tests) += vmTests;
    (*success) += vmSuccess;
}

static void test_success_statements(int *number_of_tests, int *success) {
    int vmTests = 0;
    int vmSuccess = 0;
    for (const auto &expressionDirectory : std::filesystem::directory_iterator("tests-samples/success/statements")) {
        for (const auto &script : std::filesystem::directory_iterator(expressionDirectory.path())) {
            std::cout << "[?] Testing: " << script.path().string() << std::endl;
            vmTests++;
            plasma::reader::string_reader scriptReader;
            if (!plasma::reader::string_reader_new_from_file(&scriptReader, script.path().string())) {
                continue;
            }
            plasma::lexer::lexer scriptLexer(&scriptReader);
            plasma::parser::parser scriptParser(&scriptLexer);
            plasma::bytecode_compiler::compiler compiler(&scriptParser);
            plasma::error::error compilationError;
            plasma::vm::bytecode sourceCode;
            if (!compiler.compile(&sourceCode, &compilationError)) {
                FAIL(compilationError.string() + ": " + script.path().string());
                continue;
            }
            std::istringstream stdinFile;
            std::stringstream stdoutFile;
            std::stringstream stderrFile;
            plasma::vm::virtual_machine plasmaVM(stdinFile, stdoutFile, stderrFile);
            bool executionSuccess = false;
            // Initialize the context
            plasma::vm::context c(initialMemory);
            plasmaVM.initialize_context(&c);
            auto start = std::chrono::steady_clock::now();
            plasma::vm::value *result = plasmaVM.execute(&c, &sourceCode, &executionSuccess);
            auto end = std::chrono::steady_clock::now();
            if (!executionSuccess) {
                std::cout << stdoutFile.str() << " - ";
                FAIL(result->typeName + ": " + result->string);
                continue;
            }
            auto output = stdoutFile.str();
            if (output.find("False") != std::string::npos) {
                std::cout << output;
                FAIL(script.path().string());
                continue;
            }
            SUCCESS(script.path().string() + " -> " +
                    BLUE(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) +
                         " milliseconds"));
            vmSuccess++;
        }
    }
    (*number_of_tests) += vmTests;
    (*success) += vmSuccess;
}

void test_vm(int *number_of_tests, int *success) {
    test_success_expression(number_of_tests, success);
    test_success_statements(number_of_tests, success);
}

