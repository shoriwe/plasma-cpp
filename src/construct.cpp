#include "vm/virtual_machine.h"

plasma::vm::value *plasma::vm::constructor::construct(struct context &c, virtual_machine *vm, value *self) const {
    if (this->isBuiltIn) {
        return this->callback(c, self);
    }
    c.push_symbol_table(self->symbols);
    c.push_value(self);
    bool success = false;
    bytecode bc = bytecode(this->code);
    value *result = vm->Execute(c, &bc, &success);
    c.pop_symbol_table();
    if (success) {
        return nullptr;
    }
    return result;
}