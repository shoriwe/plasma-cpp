#include "reader.h"
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "test_vm.h"

#include <filesystem>

void test_vm(int *number_of_tests, int *success) {
    int vmTests = 0;
    int vmSuccess = 0;
    for (const auto &file : std::filesystem::directory_iterator("tests-samples")) {
        vmTests++;
        plasma::reader::string_reader scriptReader;
        if (!plasma::reader::string_reader_new_from_file(&scriptReader, file.path())) {
            continue;
        }
        plasma::lexer::lexer scriptLexer(&scriptReader);
        plasma::parser::parser scriptParser(&scriptLexer);
    }
}

