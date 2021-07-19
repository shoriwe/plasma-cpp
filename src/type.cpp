#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::TypeInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                ToString,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *>& arguments, bool *success) -> value * {
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

