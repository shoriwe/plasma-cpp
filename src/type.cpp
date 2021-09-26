#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::type_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                ToString,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {

                                        (*success) = true;
                                        return this->new_string(c, false, "Type@" + self->name);
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}

