#include "vm/virtual_machine.h"


void plasma::vm::symbol_table::set(const std::string &s, value *v) {
    this->symbols.insert(std::pair<std::string, value *>(s, v));
}

plasma::vm::value *plasma::vm::symbol_table::get_self(const std::string &symbol) {
    auto entry = this->symbols.find(symbol);
    if (entry != this->symbols.end()) {
        return entry->second;
    }
    return nullptr;
}

plasma::vm::value *plasma::vm::symbol_table::get_any(const std::string &symbol) {
    auto entry = this->symbols.find(symbol);
    if (entry != this->symbols.end()) {
        return entry->second;
    }
    symbol_table *current = this->parent;
    while (current != nullptr) {
        entry = current->symbols.find(symbol);
        if (entry != current->symbols.end()) {
            return entry->second;
        }
        current = current->parent;
    }
    return nullptr;
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

plasma::vm::symbol_table::symbol_table() = default;
