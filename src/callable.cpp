#include <utility>

#include "vm/virtual_machine.h"

plasma::vm::callable plasma::vm::new_builtin_callable(size_t number_of_arguments, function_callback callback) {
    return plasma::vm::callable{
            .isBuiltIn = true,
            .numberOfArguments = number_of_arguments,
            .callback = std::move(callback),
    };
}

plasma::vm::constructor_callback plasma::vm::virtual_machine::CallableInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                Call,
                [this, isBuiltIn, c, object]() -> value * {
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