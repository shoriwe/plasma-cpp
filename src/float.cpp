#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include "vm/virtual_machine.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::float_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) -> value * {
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
                                        return this->new_float(c, false, -self->floating);
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
                                                return this->new_float(c, false, self->floating + right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->floating + right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false, left->integer + self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating + self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false, self->floating - right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->floating - right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false, left->integer - self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating - self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false, self->floating * right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->floating * right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false, left->integer * self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating * self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                                       self->floating / (0.0 + right->integer));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->floating / right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                                       (0.0 + left->integer) / self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating / self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false,
                                                                       self->floating / (0.0 + right->integer));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, self->floating / right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false,
                                                                       (0.0 + left->integer) / self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false, left->floating / self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
                                                                                            FloatName});
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
                                                return this->new_float(c, false,
                                                                       std::pow(self->floating, right->integer));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false,
                                                                       std::pow(self->floating, right->floating));
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->new_float(c, false,
                                                                       std::pow(left->integer, self->floating));
                                            case Float:
                                                (*success) = true;
                                                return this->new_float(c, false,
                                                                       std::pow(left->floating, self->floating));
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
                                                                                            FloatName});
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
                                                return this->get_boolean(c, self->floating == right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->floating == right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, left->integer == self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating == self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, self->floating != right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->floating != right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, left->integer != self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating != self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, self->floating > right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->floating > right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, left->integer > self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating > self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, self->floating < right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->floating < right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, left->integer < self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating < self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, self->floating >= right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->floating >= right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, left->integer >= self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating >= self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, self->floating <= right->integer);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, self->floating <= right->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, right->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                                return this->get_boolean(c, left->integer <= self->floating);
                                            case Float:
                                                (*success) = true;
                                                return this->get_boolean(c, left->floating <= self->floating);
                                            default:
                                                (*success) = false;
                                                return this->new_invalid_type_error(c, left->type,
                                                                                    std::vector<std::string>{
                                                                                            IntegerName,
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
                                        if (self->hash == 0) {
                                            self->hash = this->hash_string(
                                                    std::string(FloatName) + "-" + std::to_string(self->floating)
                                            );
                                        }
                                        (*success) = true;
                                        return this->new_integer(c, false, self->hash);
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
                                        return this->new_float(c, false, self->floating);
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
                                        return this->new_integer(c, false, self->floating);
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
                                        return this->new_float(c, false, self->floating);
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
                                        std::stringstream stringBuilder;
                                        stringBuilder << std::fixed << std::setprecision(20) << self->floating;
                                        (*success) = true;
                                        return this->new_string(c, false, stringBuilder.str());
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
                                        return this->get_boolean(c, self->floating != 0);
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}

