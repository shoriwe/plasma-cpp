#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::NewFloatParsingError(plasma::vm::context *c) {
    value *errorType = this->force_any_from_master(c, FloatParsingError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>());
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::NewIntegerParsingError(plasma::vm::context *c) {
    value *errorType = this->force_any_from_master(c, IntegerParsingError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>());
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::NewKeyNotFoundError(plasma::vm::context *c, plasma::vm::value *key) {
    value *errorType = this->force_any_from_master(c, KeyNotFoundError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{key});
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewIndexOutOfRange(plasma::vm::context *c, size_t length, size_t requestedIndex) {
    value *errorType = this->force_any_from_master(c, IndexOutOfRangeError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            this->new_integer(c, false, length),
            this->new_integer(c, false, requestedIndex)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewUnhashableTypeError(plasma::vm::context *c, plasma::vm::value *objectType) {
    value *errorType = this->force_any_from_master(c, UnhashableTypeError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            objectType
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewNotImplementedCallableError(plasma::vm::context *c, std::string symbol) {
    value *errorType = this->force_any_from_master(c, NotImplementedCallableError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            this->new_string(c, false, symbol)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewInvalidNumberOfArgumentsError(plasma::vm::context *c, size_t expected,
                                                              size_t received) {
    value *errorType = this->force_any_from_master(c, InvalidNumberOfArgumentsError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            this->new_integer(c, false, received),
            this->new_integer(c, false, expected)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewObjectWithNameNotFoundError(context *c, value *source,
                                                            const std::string &symbol) {
    value *errorType = this->force_any_from_master(c, ObjectWithNameNotFoundError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            source->get_type(c, this), this->new_string(c, false, symbol)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewObjectWithNameNotFoundError(context *c, const std::string &symbol) {
    value *errorType = this->force_any_from_master(c, ObjectWithNameNotFoundError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            get_none(c)->get_type(c, this), this->new_string(c, false, symbol)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewInvalidTypeError(plasma::vm::context *c, plasma::vm::value *receivedType,
                                                 const std::vector<std::string> &expectedTypes) {
    value *errorType = this->force_any_from_master(c, InvalidTypeError);
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
    value *errorType = this->force_any_from_master(c, ObjectConstructionError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            type, this->new_string(c, false, errorMessage)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewBuiltInSymbolProtectionError(plasma::vm::context *c, plasma::vm::value *source,
                                                             const std::string &symbol) {
    value *errorType = this->force_any_from_master(c, BuiltInSymbolProtectionError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            source->type, this->new_string(c, false, symbol)
    });
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::NewObjectNotCallableError(plasma::vm::context *c, plasma::vm::value *objectType) {
    value *errorType = this->force_any_from_master(c, ObjectNotCallableError);
    value *result = this->force_construction(c, errorType);
    this->force_initialization(c, result, std::vector<value *>{
            objectType
    });
    return result;
}

