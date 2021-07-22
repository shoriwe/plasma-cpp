#include <algorithm>
#include "vm/virtual_machine.h"


plasma::vm::constructor_callback plasma::vm::virtual_machine::TupleInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                Mul,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != Integer) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(
                                                    c,
                                                    right->get_type(this),
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
                                        return this->new_tuple(c, false, repeatedContent);
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
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId != Integer) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(
                                                    c,
                                                    left->get_type(this),
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
                                        return this->new_tuple(c, false, repeatedContent);
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
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != Tuple) {
                                            (*success) = true;
                                            return this->get_false();
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(
                                                c,
                                                self,
                                                right,
                                                &comparison
                                        );
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(comparison);
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
                                        if (left->typeId != Tuple) {
                                            (*success) = true;
                                            return this->get_false();
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(
                                                c,
                                                left,
                                                self,
                                                &comparison
                                        );
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(comparison);
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
                                        if (right->typeId != Tuple) {
                                            (*success) = true;
                                            return this->get_false();
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(
                                                c,
                                                self,
                                                right,
                                                &comparison
                                        );
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(!comparison);
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
                                        if (left->typeId != Tuple) {
                                            (*success) = true;
                                            return this->get_false();
                                        }
                                        bool comparison = false;
                                        value *comparisonError = this->content_equals(
                                                c,
                                                left,
                                                self,
                                                &comparison
                                        );
                                        if (comparisonError != nullptr) {
                                            (*success) = false;
                                            return comparisonError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(!comparison);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Contains,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        bool contains = false;
                                        value *containsError = this->content_contains(
                                                c,
                                                self,
                                                right,
                                                &contains
                                        );
                                        if (containsError != nullptr) {
                                            (*success) = false;
                                            return containsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(contains);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightContains,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        bool contains = false;
                                        value *containsError = this->content_contains(
                                                c,
                                                left,
                                                self,
                                                &contains
                                        );
                                        if (containsError != nullptr) {
                                            (*success) = false;
                                            return containsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(contains);
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
                                        (*success) = false;
                                        return this->NewUnhashableTypeError(c, self->get_type(this));
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
        object->set_on_demand_symbol(
                Index,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->content_index(c, self, arguments[0], success);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Assign,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        return this->content_assign(c, self, arguments[0], arguments[1], success);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->content_iterator(c, self);
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
                                        return this->content_to_string(c, self, success);
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
                                        return this->get_boolean(!self->content.empty());
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                ToArray,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
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



