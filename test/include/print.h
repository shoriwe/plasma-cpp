#ifndef PLASMA_PRINT_H
#define PLASMA_PRINT_H

#include <string>

void LOG(std::string message);

void TEST_FINISH(std::string message, int number_of_test, int success);

void SUCCESS(std::string message);

void FAIL(std::string message);


#endif //PLASMA_PRINT_H
