#include "vm/virtual_machine.h"

plasma::vm::value *plasma::vm::virtual_machine::call_function(context *c, value *function,
                                                              const std::vector<value *> &arguments,
                                                              bool *success) {
    c->objectsInUse.push_back(function);
    for (const auto &argument : arguments) {
        c->objectsInUse.push_back(argument);
    }
    bool isType = function->typeId == Type;
    value *constructedObject;
    value *callFunction;
    if (function->typeId == Function) {
        callFunction = function;
    } else if (isType) {
        constructedObject = this->construct_object(c, function, success);
        if (!(*success)) {
            c->objectsInUse.pop_back();
            for (const auto &argument : arguments) {
                c->objectsInUse.pop_back();
            }
            return constructedObject;
        }
        c->objectsInUse.push_back(constructedObject);
        (*success) = false;
        callFunction = constructedObject->get(c, this, Initialize, success);
        if (!(*success)) {
            c->objectsInUse.pop_back();
            for (const auto &argument : arguments) {
                c->objectsInUse.pop_back();
            }
            c->objectsInUse.pop_back();
            return callFunction;
        }
    } else { // Request get Call Function of the object
        callFunction = function->get(c, this, Call, success);
        if (!(*success)) {
            c->objectsInUse.pop_back();
            for (const auto &argument : arguments) {
                c->objectsInUse.pop_back();
            }
            return callFunction;
        }
        if (callFunction->typeId != Function) {
            c->objectsInUse.pop_back();
            for (const auto &argument : arguments) {
                c->objectsInUse.pop_back();
            }
            return this->NewInvalidTypeError(c, callFunction->get_type(c, this),
                                             std::vector<std::string>{FunctionName, CallableName});
        }
    }

    // Allocate a new symbol table
    // The parent of this symbol table it always will be the parent symbol table  of the function
    symbol_table *symbolTable = c->allocate_symbol_table(callFunction->symbols->parent);

    value *self;
    if (callFunction->self != nullptr) {
        self = callFunction->self;
        symbolTable->set(Self, callFunction->self);
    } else {
        self = callFunction;
        symbolTable->set(Self, callFunction);
    }

    c->push_symbol_table(symbolTable);

    value *result;
    if (callFunction->callable_.isBuiltIn) {
        result = callFunction->callable_.callback(self, arguments, success);
    } else {
        for (auto argument = arguments.rbegin();
             argument != arguments.rend();
             argument++) {

            c->push_value(*argument);
        }
        // Fixme
        bytecode bc = bytecode{
                .instructions = callFunction->callable_.code,
                .index = 0
        };
        result = this->execute(c, &bc, success);
    }

    c->pop_symbol_table();

    if (isType) {
        c->objectsInUse.pop_back();
        for (const auto &argument : arguments) {
            c->objectsInUse.pop_back();
        }
        c->objectsInUse.pop_back();
        return constructedObject;
    }
    c->objectsInUse.pop_back();
    for (const auto &argument : arguments) {
        c->objectsInUse.pop_back();
    }
    return result;
}