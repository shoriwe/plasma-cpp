#ifndef PLASMA_PLASMA_ERROR_H
#define PLASMA_PLASMA_ERROR_H

#include <string>
#include <iostream>

namespace plasma::error {
    const int UnknownLine = 0;
    const std::string LexingError = "LexingError";
    const std::string SyntaxError = "SyntaxError";
    const std::string ParsingError = "ParsingError";
    const std::string UnknownVmOperationError = "UnknownVMOperationError";
    const std::string NameNotFoundError = "NameNotFoundError";
    const std::string IndexError = "IndexError";
    const std::string GoRuntimeError = "GoRuntimeError";
    const std::string UnknownTokenKindMessage = "Unknown token kind";
    const std::string NameNotFoundMessage = "Name not found";
    const std::string UnknownVmOperationMessage = "unknown operation with value ";

    struct error {
        std::string type;
        std::string message;
        int line;

        std::string string();

        error();

        error(std::string type_, std::string message, int line);
    };

    void new_unknown_token_lind(error *receiver, int line);

    void new_index_out_of_range_error(error *receiver, int line, int length, int index);

    void new_name_not_found(error *receiver);

    void new_unknown_vm_operation_error(error *receiver, uint8_t operation);
}


#endif //PLASMA_PLASMA_ERROR_H
