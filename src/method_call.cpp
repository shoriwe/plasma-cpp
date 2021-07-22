#include "vm/virtual_machine.h"

plasma::vm::value *plasma::vm::virtual_machine::call_function(context *c, value *function,
                                                              const std::vector<value *> &arguments,
                                                              bool *success) {
    value *callFunction;
    if (function->typeId == Function) {
        callFunction = function;
    } else {
        callFunction = function->get(c, this, Call, success);
        if (!(*success)) {
            return callFunction;
        }
        if (callFunction->typeId != Function) {
            return this->NewInvalidTypeError(c, callFunction->get_type(this),
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
        for (size_t i = arguments.size() - 1; i > -1; i--) {
            c->push_value(arguments[i]);
        }
        bytecode bc = bytecode(callFunction->callable_.code);
        result = this->Execute(c, &bc, success);
    }
    c->pop_symbol_table();
    return result;
}