#include <cmath>
#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::IntegerInitialize(bool isBuiltIn) {
    return [=](context &c, value *object) -> value * {
        object->set_on_demand_symbol(
                NegBits,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_integer(c, false, ~self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Negative,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_integer(c, false, -self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Add,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, self->integer + right->integer);
                                            case Float:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, left->integer + self->integer);
                                            case Float:
                                                return this->new_float(c, false, left->integer + self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, self->integer - right->integer);
                                            case Float:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, left->integer - self->integer);
                                            case Float:
                                                return this->new_float(c, false, left->integer - self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, self->integer * right->integer);
                                            case Float:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, left->integer * self->integer);
                                            case Float:
                                                return this->new_float(c, false, left->integer * self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_float(c, false,
                                                                       (0.0 + self->integer) / (0.0 + right->integer));
                                            case Float:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_float(c, false,
                                                                       (0.0 + left->integer) / (0.0 + self->integer));
                                            case Float:
                                                return this->new_float(c, false, left->integer / self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, self->integer / right->integer);
                                            case Float:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false, left->integer / self->integer);
                                            case Float:
                                                return this->new_integer(c, false, left->integer / self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false,
                                                                         std::pow(self->integer, right->integer));
                                            case Float:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_integer(c, false,
                                                                         std::pow(left->integer, self->integer));
                                            case Float:
                                                return this->new_float(c, false,
                                                                       std::pow(left->integer, self->floating));
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, self->integer == right->integer);
                                            case Float:
                                                return this->new_bool(c, false, self->integer == right->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, left->integer == self->integer);
                                            case Float:
                                                return this->new_bool(c, false, left->integer == self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, self->integer != right->integer);
                                            case Float:
                                                return this->new_bool(c, false, self->integer != right->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, left->integer != self->integer);
                                            case Float:
                                                return this->new_bool(c, false, left->integer != self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, self->integer > right->integer);
                                            case Float:
                                                return this->new_bool(c, false, self->integer > right->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, left->integer > self->integer);
                                            case Float:
                                                return this->new_bool(c, false, left->integer > self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, self->integer < right->integer);
                                            case Float:
                                                return this->new_bool(c, false, self->integer < right->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, left->integer < self->integer);
                                            case Float:
                                                return this->new_bool(c, false, left->integer < self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, self->integer >= right->integer);
                                            case Float:
                                                return this->new_bool(c, false, self->integer >= right->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, left->integer >= self->integer);
                                            case Float:
                                                return this->new_bool(c, false, left->integer >= self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        switch (right->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, self->integer <= right->integer);
                                            case Float:
                                                return this->new_bool(c, false, self->integer <= right->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        switch (left->typeId) {
                                            case Integer:
                                                return this->new_bool(c, false, left->integer <= self->integer);
                                            case Float:
                                                return this->new_bool(c, false, left->integer <= self->floating);
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_integer(c, false, self->integer);
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
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_integer(c, false, self->integer);
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
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_integer(c, false, self->integer);
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
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_float(c, false, self->integer);
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
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_string(c, false, std::to_string(self->integer));
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
                            new_builtin_class_callable(
                                    object,
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->new_bool(c, false, self->integer != 0);
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}

