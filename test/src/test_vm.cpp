#include "reader.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "compiler/bytecode_compiler.h"
#include "test_vm.h"
#include "print.h"

#include <filesystem>

void test_success_expression(int *number_of_tests, int *success) {
    int vmTests = 0;
    int vmSuccess = 0;
    for (const auto &expressionDirectory : std::filesystem::directory_iterator("tests-samples/success/expressions")) {
        for (const auto &script : std::filesystem::directory_iterator(expressionDirectory.path())) {
            std::cout << "[?] Testing: " << script.path().string() << std::endl;
            vmTests++;
            plasma::reader::string_reader scriptReader;
            if (!plasma::reader::string_reader_new_from_file(&scriptReader, script.path())) {
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
            plasma::vm::context c;
            plasmaVM.initialize_context(&c);
            plasma::vm::value *result = plasmaVM.execute(&c, &sourceCode, &executionSuccess);
            if (!executionSuccess) {
                FAIL(result->typeName + ": " + result->string);
                continue;
            }
            auto output = stdoutFile.str();
            std::cout << output << std::endl;
            if (output.find("False") != std::string::npos) {
                FAIL(script.path().string() + "\n" + output);
                continue;
            }
            SUCCESS(script.path().string());
        }
    }
    (*number_of_tests) += vmTests;
    (*success) += vmSuccess;
}

void test_vm(int *number_of_tests, int *success) {
    test_success_expression(number_of_tests, success);
}

