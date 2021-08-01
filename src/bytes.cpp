#include "vm/virtual_machine.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::BytesInitialize(bool isBuiltIn) {
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
                                        if (right->typeId != Bytes) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(
                                                    c,
                                                    right->get_type(c, this),
                                                    std::vector<std::string>{BytesName}
                                            );
                                        }
                                        std::vector<uint8_t> result;
                                        result.reserve(self->bytes.size() + right->bytes.size());
                                        result.insert(result.end(), self->bytes.begin(), self->bytes.end());
                                        result.insert(result.end(), right->bytes.begin(), right->bytes.end());

                                        (*success) = true;
                                        return this->new_bytes(c, false, result);
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
                                        if (left->typeId != Bytes) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(
                                                    c,
                                                    left->get_type(c, this),
                                                    std::vector<std::string>{BytesName}
                                            );
                                        }
                                        std::vector<uint8_t> result;
                                        result.reserve(left->bytes.size() + self->bytes.size());
                                        result.insert(result.end(), left->bytes.begin(), left->bytes.end());
                                        result.insert(result.end(), self->bytes.begin(), self->bytes.end());

                                        (*success) = true;
                                        return this->new_bytes(c, false, result);
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
                                            return this->NewInvalidTypeError(
                                                    c,
                                                    right->get_type(c, this),
                                                    std::vector<std::string>{IntegerName}
                                            );
                                        }
                                        std::vector<uint8_t> repeatedContent;
                                        value *multiplicationError = plasma::vm::virtual_machine::bytes_repeat(
                                                self,
                                                std::abs(right->integer),
                                                &repeatedContent
                                        );
                                        if (multiplicationError != nullptr) {
                                            (*success) = false;
                                            return multiplicationError;
                                        }
                                        if (right->integer < 0) {
                                            std::reverse(repeatedContent.begin(), repeatedContent.end());
                                        }
                                        (*success) = true;
                                        return this->new_bytes(c, false, repeatedContent);
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
                                            return this->NewInvalidTypeError(
                                                    c,
                                                    left->get_type(c, this),
                                                    std::vector<std::string>{IntegerName}
                                            );
                                        }
                                        std::vector<uint8_t> repeatedContent;
                                        value *multiplicationError = plasma::vm::virtual_machine::bytes_repeat(
                                                self,
                                                std::abs(left->integer),
                                                &repeatedContent
                                        );
                                        if (multiplicationError != nullptr) {
                                            (*success) = false;
                                            return multiplicationError;
                                        }
                                        if (left->integer < 0) {
                                            std::reverse(repeatedContent.begin(), repeatedContent.end());
                                        }
                                        (*success) = true;
                                        return this->new_bytes(c, false, repeatedContent);
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
                                        if (right->typeId != Bytes) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::bytes_equals(
                                                self,
                                                right,
                                                &comparison
                                        );
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
                                        if (left->typeId != Bytes) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::bytes_equals(
                                                left,
                                                self,
                                                &comparison
                                        );
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
                                        if (right->typeId != Bytes) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::bytes_equals(
                                                self,
                                                right,
                                                &comparison
                                        );
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
                                        if (left->typeId != Bytes) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::bytes_equals(
                                                left,
                                                self,
                                                &comparison
                                        );
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
                                        value *element = arguments[0];
                                        bool contains = false;
                                        value *containsError = this->bytes_contains(
                                                c,
                                                self,
                                                element,
                                                &contains
                                        );
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
                RightContains,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *source = arguments[0];
                                        bool contains = false;
                                        value *containsError = this->bytes_contains(
                                                c,
                                                source,
                                                self,
                                                &contains
                                        );
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
                                        if (self->hash == 0) {
                                            self->hash = this->hash_bytes(self->bytes);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_bytes(c, false, self->bytes);
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
                                        return this->bytes_index(c, self, arguments[0], success);
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
                                        return this->bytes_iterator(c, self);
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

                                        (*success) = true;
                                        return this->bytes_to_string(c, self);
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
                                        return this->get_boolean(c, !self->bytes.empty());
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
                                        (*success) = true;
                                        return this->new_array(c, false, this->bytes_to_integer_content(c, self));
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
                                        (*success) = true;
                                        return this->new_tuple(c, false, this->bytes_to_integer_content(c, self));
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}