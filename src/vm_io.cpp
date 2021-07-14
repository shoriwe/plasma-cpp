#include "vm/virtual_machine.h"

plasma::vm::symbol_table *plasma::vm::virtual_machine::built_in_symbols() const {
    return this->builtInSymbolTable;
}

int64_t plasma::vm::virtual_machine::next_id() {
    auto result = this->currentId;
    this->currentId++;
    return result;
}