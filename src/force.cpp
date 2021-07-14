#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::ForceGetSelf(plasma::vm::context &c, const std::string& symbol, plasma::vm::value *source) {
    bool success = false;
    plasma::vm::value *result = source->get(c, this, symbol, &success);
    if  (!success) {
        return this->GetNone();
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::ForceMasterGetAny(const std::string& symbol) {
    value*result = this->masterContext.peek_symbol_table()->get_self(symbol);
    if (result == nullptr) {
        return this->GetNone();
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::ForceConstruction(plasma::vm::context &c, plasma::vm::value *type_) {
    this->construct_object(c, type_, )
}

void plasma::vm::virtual_machine::ForceInitialization(plasma::vm::context &c, plasma::vm::value *object,
                                                      const std::vector<plasma::vm::value *>& initArgument) {
    
}