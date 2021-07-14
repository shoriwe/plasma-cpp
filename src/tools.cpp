#include <vector>

#include "vm/virtual_machine.h"

plasma::vm::value *
plasma::vm::virtual_machine::equals(struct context &c, struct value *leftHandSide, struct value *rightHandSide,
                                    bool *result) {
    (*result) = false;
    bool success = false;
    value *resultValue;
    value *equalsFunction = leftHandSide->get(c, this, Equals, &success);
    if (!success) {
        // Try with the right equals function
        equalsFunction = rightHandSide->get(c, this, RightEquals, &success);
        if (!success) {
            return equalsFunction;
        }
        success = false;
        resultValue = this->call_function(c, equalsFunction, std::vector<value *>{leftHandSide,}, &success);
        if (!success) {
            return resultValue;
        }
        return this->quickGetBool(c, resultValue, result);
    }
    success = false;
    resultValue = this->call_function(c, equalsFunction, std::vector<value *>{rightHandSide,}, &success);
    if (!success) {
        return resultValue;
    }
    return this->quickGetBool(c, resultValue, result);
}

plasma::vm::value *
plasma::vm::virtual_machine::calculate_hash(plasma::vm::context &c, plasma::vm::value *v, int64_t *hash_) {
    bool success = false;
    value *hashFunction = v->get(c, this, Hash, &success);
    if (!success) {
        return hashFunction;
    }
    success = false;
    value *hashValue = this->call_function(c, hashFunction, std::vector<value *>(), &success);
    if (!success) {
        return hashValue;
    }
    if (hashValue->typeId != Integer) {
        return this->NewInvalidTypeError(c, hashValue->get_type(this), std::vector<std::string>{IntegerName});
    }
    (*hash_) = hashValue->integer;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::repeat(plasma::vm::context &c, const std::vector<plasma::vm::value *> &content,
                                    size_t times,
                                    std::vector<plasma::vm::value *> *result) {
    if (times == 0) {
        return nullptr;
    }

    result->clear();
    result->shrink_to_fit();
    result->reserve(content.size() * times);

    std::vector<value *> copyFunctions;
    copyFunctions.reserve(content.size());
    value *copyFunction;
    bool getSuccess;
    for (auto originalValue : content) {
        getSuccess = false;
        copyFunction = originalValue->get(c, this, Copy, &getSuccess);
        if (!getSuccess) {
            return copyFunction;
        }
        copyFunctions.push_back(copyFunction);
    }
    value *copyFunctionResult;
    bool copySuccess;
    for (size_t time = 0; time < times; time++) {
        for (size_t index = 0; index < content.size(); index++) {
            copySuccess = false;
            copyFunction = copyFunctions[index];
            copyFunctionResult = this->call_function(c, copyFunction, std::vector<value *>(), &copySuccess);
            if (!copySuccess) {
                return copyFunctionResult;
            }
            result->push_back(copyFunctionResult);
        }
    }
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::quickGetBool(plasma::vm::context &c, plasma::vm::value *v, bool *result) {
    (*result) = false;
    if (v->typeId == Boolean) {
        (*result) = v->boolean;
        return nullptr;
    }
    bool success = false;
    value *toBoolFunction = v->get(c, this, ToBool, &success);
    if (!success) {
        return toBoolFunction;
    }
    success = false;
    value *toBoolResult = this->call_function(c, toBoolFunction, std::vector<value *>(), &success);
    if (!success) {
        return toBoolResult;
    }
    if (toBoolResult->typeId != Boolean) {
        return this->NewInvalidTypeError(c, toBoolResult->get_type(this), std::vector<std::string>{BoolName,});
    }
    (*result) = toBoolResult->boolean;
    return nullptr;
}

