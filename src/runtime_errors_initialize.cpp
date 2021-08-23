#include "vm/virtual_machine.h"

// FIXME
plasma::vm::constructor_callback plasma::vm::virtual_machine::float_parsing_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::integer_parsing_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::key_not_found_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::index_out_of_range_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::unhashable_type_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback
plasma::vm::virtual_machine::not_implemented_callable_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback
plasma::vm::virtual_machine::invalid_number_of_arguments_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback
plasma::vm::virtual_machine::object_with_name_not_found_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::invalid_type_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::object_construction_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback
plasma::vm::virtual_machine::builtin_symbol_protection_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::object_not_callable_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        return nullptr;
    };
}