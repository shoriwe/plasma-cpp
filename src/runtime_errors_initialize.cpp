#include "vm/virtual_machine.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::FloatParsingErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::IntegerParsingErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::KeyNotFoundErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::IndexOutOfRangeErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::UnhashableTypeErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::NotImplementedCallableErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::InvalidNumberOfArgumentsErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::ObjectWithNameNotFoundErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::InvalidTypeErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::ObjectConstructionErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::BuiltInSymbolProtectionErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::ObjectNotCallableErrorInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value*object) {
        return nullptr;
    };
}