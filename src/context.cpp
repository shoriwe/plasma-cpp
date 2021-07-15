#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::context::allocate_value() {
    // Check if there is space to allocate the object
    if (this->value_heap.empty()) {
        // If no space if available, collect garbage
        this->collect_values();
        // If there is still no space, allocate a new page
        if (this->value_heap.empty()) {
            this->value_heap.new_page(this->value_heap.max_page_length() * 2);
        }
    }
    auto resultPage = this->value_heap.allocate();
    value *result = resultPage.chunk;
    // Reset the object
    (*result) = value();
    //
    result->pageIndex = resultPage.pageIndex;
    return result;
}

plasma::vm::symbol_table *plasma::vm::context::allocate_symbol_table(vm::symbol_table *parentSymbolTable) {
    // Check if there is space to allocate the object
    if (this->symbol_table_heap.empty()) {
        // If no space if available, collect garbage
        this->collect_symbol_tables();
        // If there is still no space, allocate a new page
        if (this->symbol_table_heap.empty()) {
            this->symbol_table_heap.new_page(this->symbol_table_heap.max_page_length() * 2);
        }
    }
    auto resultPage = this->symbol_table_heap.allocate();
    symbol_table *result = resultPage.chunk;
    // Reset the object
    if (parentSymbolTable == nullptr) {
        (*result) = symbol_table(this->peek_symbol_table());
    } else {
        (*result) = symbol_table(parentSymbolTable);
    }
    //
    result->pageIndex = resultPage.pageIndex;
    return result;
}

static void mark(plasma::vm::value *v) {
    /*
     * Mark
     * - Self
     * - Objects in content array
     * - Object in hash table values
     * - Type
     * - Sub types
     * - ...?
     */
    if (v == nullptr) {
        return;
    }
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
     * - Objects in the stack
     * - Objects in the symbol tables that are in the symbol table stack
     * - ...?
     */
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
        for (size_t pageIndex = 0; pageIndex < keyValue.second.length; pageIndex++) {
            value *v = keyValue.second.content + pageIndex;
            if (!v->isSet) {
                continue;
            }
            if (v->marked) {
                v->marked = false;
                continue;
            }
            this->value_heap.deallocate(v->pageIndex, v);
        }
        if (keyValue.second.allocatedElements == 0) {
            toRemove.push_back(keyValue.first);
        }
    }
    // Remove empty pages
    for (auto page : toRemove) {
        this->value_heap.remove_page(page);
    }
}

void plasma::vm::context::collect_symbol_tables() {
    bool collected;
    do {
        collected = false;
        for (const auto &page : this->symbol_table_heap.pages) {
            for (size_t pageIndex = 0; pageIndex < page.second.length; pageIndex++) {
                symbol_table *symbolTable = page.second.content + pageIndex;
                if (symbolTable->count == 0) {
                    if (!collected) {
                        collected = true;
                    }
                    this->symbol_table_heap.deallocate(symbolTable->pageIndex, symbolTable);
                }
            }
        }
    } while (collected);
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
    this->symbol_table_stack.push_back(s);
}

plasma::vm::symbol_table *plasma::vm::context::pop_symbol_table() {
    symbol_table *result = this->symbol_table_stack.back();
    this->symbol_table_stack.pop_back();
    return result;
}

plasma::vm::symbol_table *plasma::vm::context::peek_symbol_table() {
    return this->symbol_table_stack.back();
}