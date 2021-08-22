#include <algorithm>
#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::array_initialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) -> value * {

        object->set_on_demand_symbol(
                Add,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != Array) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(
                                                    c,
                                                    right->get_type(c, this),
                                                    std::vector<std::string>{ArrayName}
                                            );
                                        }
                                        std::vector<value *> result;
                                        result.reserve(self->content.size() + right->content.size());
                                        result.insert(result.end(), self->content.begin(), self->content.end());
                                        result.insert(result.end(), right->content.begin(), right->content.end());

                                        (*success) = true;
                                        return this->new_array(c, false, result);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId != Array) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(
                                                    c,
                                                    left->get_type(c, this),
                                                    std::vector<std::string>{ArrayName}
                                            );
                                        }
                                        std::vector<value *> result;
                                        result.reserve(left->content.size() + self->content.size());
                                        result.insert(result.end(), left->content.begin(), left->content.end());
                                        result.insert(result.end(), self->content.begin(), self->content.end());

                                        (*success) = true;
                                        return this->new_array(c, false, result);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != Integer) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(
                                                    c,
                                                    right->get_type(c, this),
                                                    std::vector<std::string>{IntegerName}
                                            );
                                        }
                                        std::vector<value *> repeatedContent;
                                        value *multiplicationError = this->content_repeat(c, self->content,
                                                                                          std::abs(right->integer),
                                                                                          &repeatedContent);
                                        if (multiplicationError != nullptr) {
                                            (*success) = false;
                                            return multiplicationError;
                                        }
                                        if (right->integer < 0) {
                                            std::reverse(repeatedContent.begin(), repeatedContent.end());
                                        }
                                        (*success) = true;
                                        return this->new_array(c, false, repeatedContent);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId != Integer) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(
                                                    c,
                                                    left->get_type(c, this),
                                                    std::vector<std::string>{IntegerName}
                                            );
                                        }
                                        std::vector<value *> repeatedContent;
                                        value *multiplicationError = this->content_repeat(c, self->content,
                                                                                          std::abs(left->integer),
                                                                                          &repeatedContent);
                                        if (multiplicationError != nullptr) {
                                            (*success) = false;
                                            return multiplicationError;
                                        }
                                        if (left->integer < 0) {
                                            std::reverse(repeatedContent.begin(), repeatedContent.end());
                                        }
                                        (*success) = true;
                                        return this->new_array(c, false, repeatedContent);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != Array) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(c, self, right, &comparison);
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, comparison);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId != Array) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(c, left, self, &comparison);
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, comparison);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != Array) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(c, self, right, &comparison);
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, !comparison);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId != Array) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(c, left, self, &comparison);
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, !comparison);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Contains,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        bool contains = false;
                                        value *containsError = this->content_contains(c, self, right, &contains);
                                        if (containsError != nullptr) {
                                            (*success) = false;
                                            return containsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, contains);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = false;
                                        return this->new_unhashable_type_error(c, self->get_type(c, this));
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        std::vector<value *> copy;
                                        value *copyError = this->content_repeat(c, self->content, 1, &copy);
                                        if (copyError != nullptr) {
                                            (*success) = false;
                                            return copyError;
                                        }
                                        (*success) = true;
                                        return this->new_array(c, false, copy);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Index,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        return this->content_index(c, arguments[0], self, success);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Assign,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    2,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        return this->content_assign(c, self, arguments[0], arguments[1], success);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Iter,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->content_iterator(c, self);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {

                                        return this->content_to_string(c, self, success);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->get_boolean(c, !self->content.empty());
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToArray,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        std::vector<value *> copy;
                                        value *copyError = this->content_repeat(c, self->content, 1, &copy);
                                        if (copyError != nullptr) {
                                            (*success) = false;
                                            return copyError;
                                        }
                                        (*success) = true;
                                        return this->new_array(c, false, copy);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToTuple,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        std::vector<value *> copy;
                                        value *copyError = this->content_repeat(c, self->content, 1, &copy);
                                        if (copyError != nullptr) {
                                            (*success) = false;
                                            return copyError;
                                        }
                                        (*success) = true;
                                        return this->new_tuple(c, false, copy);
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}

