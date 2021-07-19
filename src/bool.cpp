#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::BoolInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                Equals,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        (*success) = true;
                                        if (right->typeId != Boolean) {
                                            return this->get_false();
                                        }
                                        return this->get_boolean(self->boolean == right->boolean);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightEquals,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        (*success) = true;
                                        if (left->typeId != Boolean) {
                                            return this->get_false();
                                        }
                                        return this->get_boolean(left->boolean == self->boolean);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                NotEquals,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        (*success) = true;
                                        if (right->typeId != Boolean) {
                                            return this->get_true();
                                        }
                                        return this->get_boolean(self->boolean != right->boolean);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightNotEquals,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        (*success) = true;
                                        if (left->typeId != Boolean) {
                                            return this->get_true();
                                        }
                                        return this->get_boolean(left->boolean != self->boolean);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Copy,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->get_boolean(self->boolean);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Hash,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        if (self->boolean) {
                                            return this->new_integer(c, false, 1);
                                        }
                                        return this->new_integer(c, false, 0);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToInteger,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        if (self->boolean) {
                                            return this->new_integer(c, false, 1);
                                        }
                                        return this->new_integer(c, false, 0);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToFloat,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        if (self->boolean) {
                                            return this->new_float(c, false, 1);
                                        }
                                        return this->new_float(c, false, 0);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        if (self->boolean) {
                                            return this->new_string(c, false, True);
                                        }
                                        return this->new_string(c, false, False);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToBool,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->get_boolean(self->boolean);
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}

