#include "vm/virtual_machine.h"


void plasma::vm::value::set_on_demand_symbol(const std::string &s, const on_demand_loader &loader) {
    this->onDemandSymbols[s] = loader;
}

void plasma::vm::value::set_symbols(symbol_table *symbolTable) {
    if (this->symbols != nullptr) {
        // Decrement count when un-assign
        this->symbols->count--;
    }
    this->symbols = symbolTable;
    // Increment count when assigned to object
    this->symbols->count++;
}

void plasma::vm::value::set(const std::string &s, value *v) const {
    this->symbols->set(s, v);

}

plasma::vm::value *
plasma::vm::value::get(context *c, virtual_machine *vm, const std::string &symbol, bool *success) {
    value *result = this->symbols->get_self(symbol);
    if (result == nullptr) {

        auto onDemandResult = this->onDemandSymbols.find(symbol);
        // Try to get the value from the onDemand map
        if (onDemandResult != this->onDemandSymbols.end()) {
            c->objectsInUse.push_back(this);
            result = onDemandResult->second();
            this->set(symbol, result);
            (*success) = true;
            c->objectsInUse.pop_back();
        } else {

            (*success) = false;
            result = vm->NewObjectWithNameNotFoundError(c, this, symbol);
        }
    } else {
        (*success) = true;
    }
    return result;
}

plasma::vm::value *plasma::vm::value::get_type(context *c, virtual_machine *vm) const {
    if (this->type == nullptr) {
        return vm->force_any_from_master(c, this->typeName);
    }
    return this->type;
}

std::unordered_map<std::string, uint8_t> plasma::vm::value::dir() {
    std::unordered_map<std::string, uint8_t> result;
    for (auto &onDemandSymbol : this->onDemandSymbols) {
        result[onDemandSymbol.first] = 0;
    }
    for (auto &symbol : this->symbols->symbols) {
        result[symbol.first] = 0;
    }
    return result;
}

static bool type_implements(plasma::vm::context *c, plasma::vm::virtual_machine *vm, plasma::vm::value *self,
                            plasma::vm::value *v) {
    if (self == v) {
        return true;
    }
    for (plasma::vm::value *subType: self->subTypes) {
        if (subType->implements(c, vm, v)) {
            return true;
        }
    }
    return false;
}

static bool
object_implements(plasma::vm::context *c, plasma::vm::virtual_machine *vm, plasma::vm::value *self,
                  plasma::vm::value *v) {
    return type_implements(c, vm, self->get_type(c, vm), v);
}

bool plasma::vm::value::implements(context *c, virtual_machine *vm, value *v) {
    if (this->typeId == Type) {
        return type_implements(c, vm, this, v);
    }
    return object_implements(c, vm, this, v);
}

plasma::vm::value *plasma::vm::value::add_key_value(context *c, virtual_machine *vm, value *key, value *v) {
    int64_t hash_ = 0;
    value *hashingError = vm->calculate_hash(c, key, &hash_);
    if (hashingError != nullptr) {
        return hashingError;
    }
    auto kValues = this->keyValues.find(hash_);
    if (kValues == this->keyValues.end()) {
        this->keyValues[hash_] = std::vector(1,
                                             key_value{
                                                     .key = key,
                                                     .value = v,
                                             }
        );
        return nullptr;
    }
    for (auto &kValueEntry : kValues->second) {
        bool equals = false;
        value *equalsError = vm->equals(c, key, kValueEntry.key, &equals);
        if (equalsError != nullptr) {
            return equalsError;
        }
        if (equals) {
            kValueEntry.key = key;
            kValueEntry.value = v;
            return nullptr;
        }
    }
    kValues->second.push_back(
            key_value{
                    .key = key,
                    .value = v,
            }
    );
    return nullptr;
}