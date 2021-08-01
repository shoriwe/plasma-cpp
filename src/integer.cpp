#include <cmath>
#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::IntegerInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                NegBits,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_integer(c, false, ~self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Negative,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_integer(c, false, -self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Add,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer + right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->integer + right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightAdd,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer + self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating + self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Sub,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer - right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->integer - right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightSub,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer - self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating - self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Mul,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer * right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->integer * right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightMul,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer * self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating * self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Div,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_float(c, false,
                                                                       (0.0 + self->integer) / (0.0 + right->integer));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->integer / right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightDiv,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_float(c, false,
                                                                       (0.0 + left->integer) / (0.0 + self->integer));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating / self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                FloorDiv,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer / right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer / right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightFloorDiv,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer / self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->floating / self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Mod,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer % right->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightMod,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer % self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Pow,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false,
                                                                         std::pow(self->integer, right->integer));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false,
                                                                       std::pow(self->integer, right->floating));
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightPow,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false,
                                                                         std::pow(left->integer, self->integer));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false,
                                                                       std::pow(left->floating, self->integer));
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                BitXor,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer ^ right->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightBitXor,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer ^ self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                BitAnd,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer & right->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightBitAnd,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer & self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                BitOr,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer | right->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightBitOr,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer | self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                BitLeft,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer << right->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightBitLeft,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer << self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                BitRight,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, self->integer >> right->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightBitRight,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->new_integer(c, false, left->integer >> self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Equals,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer == right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer == right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightEquals,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, left->integer == self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating == self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                NotEquals,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer != right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer != right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightNotEquals,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, left->integer != self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating != self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GreaterThan,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer > right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer > right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightGreaterThan,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, left->integer > self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating > self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                LessThan,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer < right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer < right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightLessThan,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, left->integer < self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating < self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GreaterThanOrEqual,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer >= right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer >= right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightGreaterThanOrEqual,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, left->integer >= self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating >= self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                LessThanOrEqual,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer <= right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->integer <= right->floating);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, right->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightLessThanOrEqual,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                (*success) = true;
                                                return this->get_boolean(c, left->integer <= self->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating <= self->integer);
                                            default:
                                                (*success) = false;
                                                return this->NewInvalidTypeError(c, left->type,
                                                                                 std::vector<std::string>{IntegerName,
                                                                                                          FloatName});
                                        }
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Hash,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_integer(c, false, self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Copy,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_integer(c, false, self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToInteger,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_integer(c, false, self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToFloat,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_float(c, false, self->integer);
                                    }
                            )
                    );
                }
        );
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
                                        return this->new_string(c, false, std::to_string(self->integer));
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToBool,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn, object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->get_boolean(c, self->integer != 0);
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}

