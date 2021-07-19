#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::CallableInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                Call,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, std::vector<value *> arguments, bool *success) -> value * {
                                        return this->NewNotImplementedCallableError(
                                                c,
                                                Call
                                        );
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}