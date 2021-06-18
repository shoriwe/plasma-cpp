#include <iostream>

#include "print.h"

void LOG(std::string message) {
    std::cout << "---- " << message << " ----" << std::endl;
}

void TEST_FINISH(std::string message, int number_of_tests, int success) {
    std::cout << message << ": ";
    if (number_of_tests == success) {
        std::cout << "\u001b[32m" << success << "\u001b[0m";
    } else {
        std::cout << "\u001b[31m" << success << "\u001b[0m";
    }
    std::cout << "/" << "\u001b[32m" << number_of_tests << "\u001b[0m" << std::endl;
}

void SUCCESS(std::string message) {
    std::cout << "[\u001b[34m+\u001b[0m] " << message << " -> \u001b[32mSUCCESS\u001b[0m" << std::endl;
}

void FAIL(std::string message) {
    std::cout << "[\u001b[31m-\u001b[0m] " << message << " -> \u001b[31mFAIL\u001b[0m" << std::endl;
}