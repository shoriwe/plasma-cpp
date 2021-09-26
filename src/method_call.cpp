#include "vm/virtual_machine.h"

plasma::vm::value *plasma::vm::virtual_machine::call_function(context *c, value *function,
                                                              const std::vector<value *> &arguments,
                                                              bool *success) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });
    c->protect_value(function);
    for (const auto &argument : arguments) {
        c->protect_value(argument);
    }
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
        c->protect_value(constructedObject);
        (*success) = false;
        callFunction = constructedObject->get(c, this, Initialize, success);
        if (!(*success)) {
            return callFunction;
        }
    } else { // Request get Call Function of the object
        callFunction = function->get(c, this, Call, success);
        if (!(*success)) {
            return callFunction;
        }
        if (callFunction->typeId != Function) {
            return this->new_invalid_type_error(c, callFunction->get_type(c, this),
                                                std::vector<std::string>{FunctionName, CallableName});
        }
    }

    if (callFunction->callable_.numberOfArguments != arguments.size()) {
        (*success) = false;
        return this->new_invalid_number_of_arguments_error(
                c, callFunction->callable_.numberOfArguments, arguments.size()
        );
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
        return constructedObject;
    }
    return result;
}