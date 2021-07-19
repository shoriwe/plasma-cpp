#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::IteratorInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                HasNext,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->get_false();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Next,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Iter,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return self;
                                    }
                            )
                    );
                }
        );
    };
}

