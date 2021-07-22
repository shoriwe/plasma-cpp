#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::RuntimeErrorInitialize(context *c, struct value *object) {
    object->set_on_demand_symbol(
            Initialize,
            [=]() -> value * {
                return this->new_function(
                        c,
                        false,
                        object,
                        new_builtin_callable(
                                1,
                                [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                    value *message = arguments[0];
                                    if (message->typeId != String) {
                                        (*success) = false;
                                        return this->NewInvalidTypeError(
                                                c,
                                                message->get_type(this),
                                                std::vector<std::string>{StringName}
                                        );
                                    }
                                    self->string = message->string;
                                    (*success) = true;
                                    return this->get_none();
                                }
                        )
                );
            }
    );
    object->set_on_demand_symbol(
            ToString,
            [=]() -> value * {
                return this->new_function(
                        c,
                        false,
                        object,
                        new_builtin_callable(
                                0,
                                [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                    (*success) = true;
                                    return this->new_string(
                                            c,
                                            false,
                                            self->get_type(this)->name + ": " + self->string
                                    );
                                }
                        )
                );
            }
    );
    return nullptr;
}