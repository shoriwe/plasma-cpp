#include "vm/virtual_machine.h"


void plasma::vm::virtual_machine::load_builtin_object(const std::string &symbol, const object_loader& loader) {
    this->masterContext->peek_symbol_table()->set(symbol, loader(this->masterContext, this));
}

void plasma::vm::virtual_machine::load_builtin_symbols(const std::unordered_map<std::string, object_loader> &symbols) {
    for (const auto &keyValue : symbols) {
        this->masterContext->peek_symbol_table()->set(keyValue.first, keyValue.second(this->masterContext, this));
    }
}

void plasma::vm::virtual_machine::initialize_context(context *c) {
    symbol_table *symbols = c->allocate_symbol_table(this->builtInSymbolTable);
    value *builtIn = this->new_object(c, true, ObjectName, nullptr);
    builtIn->symbols = this->builtInSymbolTable;
    symbols->set(
            "__built_in__",
            builtIn
    );
    value *self = this->new_object(c, true, ObjectName, nullptr);
    builtIn->symbols = symbols;
    symbols->set(
            Self,
            self
    );
}

// void plasma::vm::virtual_machine::initialize_builtin_symbols() {
//     // Initialize all the symbols that are set by default in the virtual machine
// }


uint64_t plasma::vm::virtual_machine::next_id() {
    uint64_t result = this->currentId;
    this->currentId++;
    return result;
}
