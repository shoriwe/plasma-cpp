#include "vm/virtual_machine.h"

plasma::vm::value *plasma::vm::virtual_machine::call_function(context *c, value *function,
                                                              const std::vector<value *> &arguments,
                                                              bool *success) {
    std::cout << "HERE 1\n";
    bool isType = function->typeId == Type;
    value *constructedObject;
    value *callFunction;
    if (function->typeId == Function) {
        callFunction = function;
    } else if (isType) {
        constructedObject = this->construct_object(c, function, success);
        if (!(*success)) {
            return constructedObject;
        }
        (*success) = false;
        callFunction = constructedObject->get(c, this, Initialize, success);
    } else {
        callFunction = function->get(c, this, Call, success);
        if (!(*success)) {
            return callFunction;
        }
        if (callFunction->typeId != Function) {
            return this->NewInvalidTypeError(c, callFunction->get_type(c, this),
                                             std::vector<std::string>{FunctionName, CallableName});
        }
    }
    std::cout << "HERE 2\n";
    // Allocate a new symbol table
    // The parent of this symbol table it always will be the parent symbol table  of the function
    symbol_table *symbolTable = c->allocate_symbol_table(callFunction->symbols->parent);
    std::cout << "HERE 3\n";
    value *self;
    if (callFunction->self != nullptr) {
        self = callFunction->self;
        symbolTable->set(Self, callFunction->self);
    } else {
        self = callFunction;
        symbolTable->set(Self, callFunction);
    }
    std::cout << "HERE 4\n";
    c->push_symbol_table(symbolTable);
    std::cout << "HERE 5\n";
    value *result;
    if (callFunction->callable_.isBuiltIn) {
        std::cout << "HERE 5.1\n";
        result = callFunction->callable_.callback(self, arguments, success);
        std::cout << "HERE 5.2\n";
    } else {
        for (size_t i = arguments.size() - 1; i > -1; i--) {
            c->push_value(arguments[i]);
        }
        // Fixme
        // bytecode bc = bytecode(callFunction->callable_.code);
        // result = this->execute(c, &bc, success);
    }
    std::cout << "HERE 6\n";
    c->pop_symbol_table();
    std::cout << "HERE 7\n";
    if (isType) {
        std::cout << "HERE 8\n";
        return constructedObject;
    }
    std::cout << "HERE 9\n";
    return result;
}