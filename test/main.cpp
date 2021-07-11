#include <iostream>

#include "print.h"
#include "test_lexer.h"
#include "test_parser.h"


int main() {
    int number_of_tests = 0;
    int success = 0;

    LOG("Lexer");
    test_lexer(&number_of_tests, &success);
    LOG("Parser");
    test_parser(&number_of_tests,  &success);

    LOG("Results");
    TEST_FINISH("Resume", number_of_tests, success);
}
