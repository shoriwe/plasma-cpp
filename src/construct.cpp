#include "vm/virtual_machine.h"

/*
 * ToDo:
 * Determine all the locations in which the symbol-table reference increment should happen
 */
plasma::vm::value *plasma::vm::virtual_machine::construct_subtype(plasma::vm::context *c, value *subType, value *self) {
    value *subTypeConstructionError;
    for (auto subSubType : subType->subTypes) {
        self->symbols->parent = subSubType->symbols->parent;
        subTypeConstructionError = this->construct_subtype(c, subSubType, self);
        if (subTypeConstructionError != nullptr) {
            return subTypeConstructionError;
        }
    }
    self->symbols->parent = subType->symbols->parent;
    return subType->constructor_.construct(c, this, self);

}

plasma::vm::value *
plasma::vm::virtual_machine::construct_object(plasma::vm::context *c, plasma::vm::value *type, bool *success) {
    auto result = this->new_object(c, false, type->name, type);
    value *subTypeInitializationError;
    for (auto subType : type->subTypes) {
        // First initialize every child type
        subTypeInitializationError = this->construct_subtype(c, subType, result);
        if (subTypeInitializationError != nullptr) {
            (*success) = false;
            return subTypeInitializationError;
        }
    }
    result->symbols->parent = type->symbols->parent;
    result->type = type;
    // Initialize the object type
    value *initializationError = type->constructor_.construct(c, this, result);
    if (initializationError != nullptr) {
        (*success) = false;
        return initializationError;
    }
    (*success) = true;
    return result;
}

plasma::vm::value *plasma::vm::constructor::construct(context *c, virtual_machine *vm, value *self) const {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    c->protect_value(self);
    if (this->isBuiltIn) {
        return this->callback(c, self);
    }
    c->push_symbol_table(self->symbols);
    c->push_value(self);
    bool success = false;
    bytecode bc = bytecode{
            .instructions = this->code,
            .index = 0
    };
    value *result = vm->execute(c, &bc, &success);
    c->pop_symbol_table();
    c->pop_value();
    if (success) {
        return nullptr;
    }
    return result;
}