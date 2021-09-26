#include "vm/virtual_machine.h"

plasma::vm::context::context(size_t initialPageLength) {
    if (initialPageLength == 0) {
        throw std::out_of_range("initialPageLength can't be zero");
    }
    this->symbol_table_heap = memory::memory<symbol_table>(initialPageLength);
    this->value_heap = memory::memory<value>(initialPageLength);
}

plasma::vm::context::~context() {
    this->objectsInUse.clear();
    this->symbol_table_stack.clear();
    this->value_stack.clear();
    this->master->symbols.clear();
}

plasma::vm::value *plasma::vm::context::allocate_value() {
    // Check if there is space to allocate the object
    if (this->value_heap.empty()) {
        // If no space if available, collect garbage
        this->collect_values();
        // If there is still no space, allocate a new page
        if (this->value_heap.empty()) {
            auto newPageLength = this->value_heap.max_page_length() * 2;
            this->value_heap.new_page(newPageLength);
        }
    }
    auto resultPage = this->value_heap.allocate();
    value *result = resultPage.object;
    // Reset the object
    (*result) = value();
    //
    result->pageIndex = resultPage.page_index;
    result->isSet = true;
    return result;
}

plasma::vm::symbol_table *plasma::vm::context::allocate_symbol_table(vm::symbol_table *parentSymbolTable) {
    // Check if there is space to allocate the object
    if (this->symbol_table_heap.empty()) {
        // If no space if available, collect garbage
        this->collect_symbol_tables();
        // If there is still no space, allocate a new page
        if (this->symbol_table_heap.empty()) {
            auto newPageLength = this->symbol_table_heap.max_page_length() * 2;
            this->symbol_table_heap.new_page(newPageLength);
        }
    }
    auto resultPage = this->symbol_table_heap.allocate();
    symbol_table *result = resultPage.object;
    // Reset the object
    if (parentSymbolTable == nullptr) {
        if (this->symbol_table_stack.empty()) {
            if (this->master == nullptr) {
                (*result) = symbol_table(nullptr);
            } else {
                (*result) = symbol_table(this->master);
            }
        } else {
            (*result) = symbol_table(this->peek_symbol_table());
        }
    } else {
        (*result) = symbol_table(parentSymbolTable);
    }
    if (result->parent != nullptr) {
        // Increment parent when symbol table creation
        result->parent->count++;
    }
    //
    result->pageIndex = resultPage.page_index;
    result->isSet = true;
    return result;
}

static void mark(plasma::vm::value *v) {
    /*
     * Mark:
     * - Self
     * - Source
     * - Objects in content array
     * - Object in hash table values
     * - Type
     * - Sub types
     * - ...?
     */
    if (v == nullptr) {
        return;
    }
    mark(v->source);
    if (v->marked) {
        return;
    }
    v->marked = true;
    mark(v->self);
    for (const auto &sym : v->symbols->symbols) {
        mark(sym.second);
    }
    for (auto arrayValue : v->content) {
        mark(arrayValue);
    }
    for (const auto &kValue : v->keyValues) {
        for (auto kvEntry: kValue.second) {
            mark(kvEntry.key);
            mark(kvEntry.value);
        }
    }
    mark(v->type);
    for (auto arrayValue : v->subTypes) {
        mark(arrayValue);
    }
}

void plasma::vm::context::collect_values() {

    /*
     * Roots:
     * - Current objects in use
     * - Master symbols
     * - Last Object
     * - Objects in the stack
     * - Objects in the symbol tables that are in the symbol table stack
     * - ...?
     */
    for (const auto &v : this->objectsInUse) {
        mark(v);
    }
    for (const auto &keyValue : this->master->symbols) {
        mark(keyValue.second);
    }
    mark(this->lastObject);
    for (auto v : this->value_stack) {
        mark(v);
    }
    for (auto symbolTable : this->symbol_table_stack) {
        for (const auto &sym : symbolTable->symbols) {
            mark(sym.second);
        }
    }
    std::vector<size_t> toRemove;
    toRemove.reserve(this->value_heap.pages.size());
    // Collect all not marked values
    for (const auto &keyValue: this->value_heap.pages) {
        for (size_t index = 0; index < keyValue.second->length; index++) {
            value *v = keyValue.second->index(index);// keyValue.second->index(index);
            if (!v->isSet) {
                continue;
            }
            if (v->marked || v->isBuiltIn) {
                v->marked = false;
                continue;
            }
            v->isSet = false;
            // Object was destroyed, decrement count of its symbol table
            v->symbols->count--;
            this->value_heap.deallocate(v->pageIndex, v);
        }
    }
    this->value_heap.shrink();
}

void plasma::vm::context::collect_symbol_tables() {
    bool collected;
    do {
        collected = false;
        for (const auto &keyValue : this->symbol_table_heap.pages) {
            for (size_t index = 0; index < keyValue.second->length; index++) {
                symbol_table *symbolTable = keyValue.second->index(index);// keyValue.second.index(index);
                if (symbolTable->count == 0 && symbolTable->isSet) {
                    symbolTable->isSet = false;
                    this->symbol_table_heap.deallocate(symbolTable->pageIndex, symbolTable);
                    collected = true;
                }
            }
        }
    } while (collected);
    this->symbol_table_heap.shrink();
}

void plasma::vm::context::push_value(value *v) {
    this->value_stack.push_back(v);
}

plasma::vm::value *plasma::vm::context::peek_value() {
    return this->value_stack.back();
}

plasma::vm::value *plasma::vm::context::pop_value() {
    value *result = this->value_stack.back();
    this->value_stack.pop_back();
    return result;
}

void plasma::vm::context::push_symbol_table(symbol_table *s) {
    // Increment on push to stack
    s->count++;
    this->symbol_table_stack.push_back(s);
}

plasma::vm::symbol_table *plasma::vm::context::pop_symbol_table() {
    symbol_table *result = this->symbol_table_stack.back();
    this->symbol_table_stack.pop_back();
    // Decrement when pop from the stack
    result->count--;
    return result;
}

plasma::vm::symbol_table *plasma::vm::context::peek_symbol_table() {
    return this->symbol_table_stack.back();
}

void plasma::vm::context::protect_value(value *v) {
    this->objectsInUse.push_back(v);
}

void plasma::vm::context::restore_protected_state(size_t state) {
    if (state < this->objectsInUse.size()) {
        this->objectsInUse.erase(this->objectsInUse.begin() + state, this->objectsInUse.end());
    }
}

size_t plasma::vm::context::protected_values_state() const {
    return this->objectsInUse.size();
}