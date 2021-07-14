#include "vm/virtual_machine.h"


void plasma::vm::symbol_table::set(const std::string &s, value *v) {
    this->symbols[s] = v;
}

plasma::vm::value *plasma::vm::symbol_table::get_self(const std::string &symbol) {
    if (this->symbols.contains(symbol)) {
        return this->symbols[symbol];
    }
    return nullptr;
}

plasma::vm::value *plasma::vm::symbol_table::get_any(const std::string &symbol) {
    if (this->symbols.contains(symbol)) {
        return this->symbols[symbol];
    }
    value *result = nullptr;
    symbol_table *current = this->parent;
    while (current != nullptr) {
        result = current->get_any(symbol);
        if (result != nullptr) {
            return result;
        }
        current = current->parent;
    }
    return result;
}

plasma::vm::symbol_table::symbol_table(symbol_table *parent) {
    if (parent != nullptr) {
        parent->count++;
    }
    this->parent = parent;
}

plasma::vm::symbol_table::~symbol_table() {
    if (parent != nullptr) {
        parent->count++;
    }
}