#include "vm/virtual_machine.h"


void plasma::vm::value::set_on_demand_symbol(const std::string &s, on_demand_loader loader) {
    this->onDemandSymbols[s] = loader;
}

void plasma::vm::value::set(const std::string &s, value *v) const {
    this->symbols->set(s, v);
}

plasma::vm::value *
plasma::vm::value::get(context *c, virtual_machine *vm, const std::string &symbol, bool *success) {
    value *result = this->symbols->get_self(symbol);
    if (result == nullptr) {
        // Try to get the value from the onDemand map
        if (this->onDemandSymbols.contains(symbol)) {
            result = this->onDemandSymbols[symbol]();
            this->set(symbol, result);
            (*success) = true;
            return result;
        }
        (*success) = false;
        return vm->NewObjectWithNameNotFoundError(c, this, symbol);
    }
    (*success) = true;
    return result;
}

plasma::vm::value *plasma::vm::value::get_type(virtual_machine *vm) const {
    if (this->type == nullptr) {
        return vm->force_any_from_master(this->typeName);
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

static bool type_implements(plasma::vm::virtual_machine *vm, plasma::vm::value *self, plasma::vm::value *v) {
    if (self == v) {
        return true;
    }
    for (plasma::vm::value *subType: self->subTypes) {
        if (subType->implements(vm, v)) {
            return true;
        }
    }
    return false;
}

static bool object_implements(plasma::vm::virtual_machine *vm, plasma::vm::value *self, plasma::vm::value *v) {
    return type_implements(vm, self->get_type(vm), v);
}

bool plasma::vm::value::implements(virtual_machine *vm, value *v) {
    if (this->typeId == Type) {
        return type_implements(vm, this, v);
    }
    return object_implements(vm, this, v);
}

plasma::vm::value *plasma::vm::value::add_key_value(context *c, virtual_machine *vm, value *key, value *v) {
    int64_t hash_ = 0;
    value *hashingError = vm->calculate_hash(c, key, &hash_);
    if (hashingError != nullptr) {
        return hashingError;
    }
    if (!this->keyValues.contains(hash_)) {
        this->keyValues[hash_] = std::vector(1,
                                             key_value{
                                                     .key = key,
                                                     .value = v,
                                             }
        );
        return nullptr;
    }
    for (size_t index = 0; index < this->keyValues[hash_].size(); index++) {
        bool equals = false;
        value *equalsError = vm->equals(c, key, this->keyValues[hash_][index].key, &equals);
        if (equalsError != nullptr) {
            return equalsError;
        }
        if (equals) {
            this->keyValues[hash_][index].value = v;
            return nullptr;
        }
    }
    this->keyValues[hash_].push_back(
            key_value{
                    .key = key,
                    .value = v,
            }
    );
    return nullptr;
}