#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::execute(context *c, bytecode *bc, bool *success) {
    value *executionError = nullptr;
    value *lastObject = nullptr;
    while (bc->has_next()) {
        instruction instruct = bc->next();
        switch (instruct.op_code) {
            case NewStringOP:
                executionError = this->newStringOP(c, std::any_cast<std::string>(instruct.value), &lastObject);
                break;
            case NewFloatOP:
                executionError = this->newFloatOP(c, std::any_cast<long double>(instruct.value), &lastObject);
                break;
            case NewIntegerOP:
                executionError = this->newIntegerOP(c, std::any_cast<int64_t>(instruct.value), &lastObject);
                break;
            case NewBytesOP:
                executionError = this->newBytesOP(c, std::any_cast<std::vector<uint8_t>>(instruct.value), &lastObject);
                break;
            case GetTrueOP:
                lastObject = this->new_bool(c, false, true);
                break;
            case GetFalseOP:
                lastObject = this->new_bool(c, false, false);
                break;

        }
        if (executionError != nullptr) {
            //  Handle the error
        }
    }
}

