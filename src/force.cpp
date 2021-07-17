#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::force_get_from_source(struct context &c, const std::string &symbol,
                                                                      plasma::vm::value *source) {
    bool success = false;
    plasma::vm::value *result = source->get(c, this, symbol, &success);
    if (!success) {
        return this->get_none();
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::force_any_from_master(const std::string &symbol) {
    value *result = this->masterContext.peek_symbol_table()->get_self(symbol);
    if (result == nullptr) {
        return this->get_none();
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::force_construction(struct context &c, plasma::vm::value *type_) {
    bool success;
    value *result = this->construct_object(c, type_, &success);
    if (!success) {
        return this->get_none();
    }
    return result;
}

void plasma::vm::virtual_machine::force_initialization(const plasma::vm::context &c, plasma::vm::value *object,
                                                       const std::vector<plasma::vm::value *> &initArgument) {
    bool success = false;
    value *initialize = object->get(c, this, Initialize, &success);
    if (!success) {
        return;
    }
    success = false;
    this->call_function(c, initialize, initArgument, &success);
}