#include "error.h"

#include <utility>

std::string plasma::error::error::string() {
    return this->type + ": " + this->message + " at line " + std::to_string(this->line);
}

plasma::error::error::error() {
    this->type = "";
    this->message = "";
    this->line = UnknownLine;
}

plasma::error::error::error(std::string type, std::string message, int line) {
    this->type = std::move(type);
    this->message = std::move(message);
    this->line = line;
}

void plasma::error::new_unknown_token_lind(error *receiver, int line) {
    (*receiver) = error(
            UnknownVmOperationError,
            UnknownTokenKindMessage,
            line
    );
}

void plasma::error::new_index_out_of_range_error(error *receiver, int line, int length, int index) {
    (*receiver) = error(
            IndexError,
            "index " + std::to_string(index) + "out of bound for a " + std::to_string(length) + " container",
            line);

}

void plasma::error::new_name_not_found(error *receiver) {
    (*receiver) = error(NameNotFoundError, NameNotFoundMessage, UnknownLine);
}

void plasma::error::new_unknown_vm_operation_error(error *receiver, uint8_t operation) {
    (*receiver) = error(UnknownVmOperationError, UnknownTokenKindMessage, UnknownLine);
}