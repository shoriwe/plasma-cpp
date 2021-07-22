#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::NewFloatParsingError(plasma::vm::context *c) {
    value *errorType = this->force_any_from_master(FloatParsingError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>());
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::NewIntegerParsingError(plasma::vm::context *c) {
    value *errorType = this->force_any_from_master(IntegerParsingError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>());
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::NewKeyNotFoundError(plasma::vm::context *c, plasma::vm::value *key) {
    value *errorType = this->force_any_from_master(KeyNotFoundError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{key});
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewIndexOutOfRange(plasma::vm::context *c, size_t length, size_t requestedIndex) {
    value *errorType = this->force_any_from_master(IndexOutOfRangeError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            this->new_integer(c, false, length),
            this->new_integer(c, false, requestedIndex)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewUnhashableTypeError(plasma::vm::context *c, plasma::vm::value *objectType) {
    value *errorType = this->force_any_from_master(UnhashableTypeError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            objectType
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewNotImplementedCallableError(plasma::vm::context *c, std::string symbol) {
    value *errorType = this->force_any_from_master(NotImplementedCallableError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            this->new_string(c, false, symbol)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewInvalidNumberOfArgumentsError(plasma::vm::context *c, size_t expected,
                                                              size_t received) {
    value *errorType = this->force_any_from_master(InvalidNumberOfArgumentsError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            this->new_integer(c, false, received),
            this->new_integer(c, false, expected)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewObjectWithNameNotFoundError(plasma::vm::context *c, plasma::vm::value *source,
                                                            const std::string &symbol) {
    value *errorType = this->force_any_from_master(ObjectWithNameNotFoundError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            source->get_type(this), this->new_string(c, false, symbol)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewInvalidTypeError(plasma::vm::context *c, plasma::vm::value *receivedType,
                                                 const std::vector<std::string> &expectedTypes) {
    value *errorType = this->force_any_from_master(InvalidTypeError);
    value *result = this->force_construction(c, errorType);
    std::string expecting;
    bool first = false;
    for (const auto &expectedType : expectedTypes) {
        if (first) {
            expecting += ", ";
        } else {
            first = true;
        }
        expecting += expectedType;
    }
    this->force_initialization(c, result, std::vector<value *>{
            receivedType,
            this->new_string(c, false, expecting)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewObjectConstructionError(plasma::vm::context *c, value *type,
                                                        const std::string &errorMessage) {
    value *errorType = this->force_any_from_master(ObjectConstructionError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            type, this->new_string(c, false, errorMessage)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewBuiltInSymbolProtectionError(plasma::vm::context *c, plasma::vm::value *source,
                                                             const std::string &symbol) {
    value *errorType = this->force_any_from_master(BuiltInSymbolProtectionError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            source->type, this->new_string(c, false, symbol)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewObjectNotCallableError(plasma::vm::context *c, plasma::vm::value *objectType) {
    value *errorType = this->force_any_from_master(ObjectNotCallableError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            objectType
    });
    return result;
}

