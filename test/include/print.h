#ifndef PLASMA_PRINT_H
#define PLASMA_PRINT_H

#include <string>

std::string BLUE(const std::string &s);

void LOG(const std::string &message);

void TEST_FINISH(const std::string &message, int number_of_test, int success);

void SUCCESS(std::string message);

void FAIL(std::string message);


#endif //PLASMA_PRINT_H
