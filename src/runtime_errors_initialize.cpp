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
        object->set(Initialize,
                    this->new_function(
                            c, isBuiltIn, object,
                            new_builtin_callable(
                                    2,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        if (arguments[0]->typeId != Integer) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(c, arguments[0]->get_type(c,
                                                                                                          this),
                                                                                std::vector<std::string>{
                                                                                        IntegerName});
                                        }
                                        if (arguments[1]->typeId != Integer) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(c, arguments[1]->get_type(c,
                                                                                                          this),
                                                                                std::vector<std::string>{
                                                                                        IntegerName});
                                        }

                                        self->string = "Expecting " + std::to_string(arguments[1]->integer) +
                                                       " but received " + std::to_string(arguments[0]->integer);
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    )
        );
        return nullptr;
    };
}

plasma::vm::constructor_callback
plasma::vm::virtual_machine::object_with_name_not_found_error_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) {
        object->set(Initialize,
                    this->new_function(
                            c, isBuiltIn, object,
                            new_builtin_callable(
                                    2,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        if (arguments[1]->typeId != String) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(c, arguments[1]->get_type(c,
                                                                                                          this),
                                                                                std::vector<std::string>{
                                                                                        StringName});
                                        }

                                        self->string = "Could not found name \"" + arguments[1]->string +
                                                       "\" in object of type " + arguments[0]->name;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    )
        );
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