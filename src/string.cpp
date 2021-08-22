#include "vm/virtual_machine.h"
#include "tools.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::string_initialize(bool isBuiltIn) {
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
                                        if (right->typeId != String) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(
                                                    c,
                                                    right->get_type(c, this),
                                                    std::vector<std::string>{StringName}
                                            );
                                        }
                                        std::string result;
                                        result.reserve(self->string.size() + right->string.size());
                                        result.insert(result.end(), self->string.begin(), self->string.end());
                                        result.insert(result.end(), right->string.begin(), right->string.end());

                                        (*success) = true;
                                        return this->new_string(c, false, result);
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
                                        if (left->typeId != String) {
                                            (*success) = false;
                                            return this->new_invalid_type_error(
                                                    c,
                                                    left->get_type(c, this),
                                                    std::vector<std::string>{StringName}
                                            );
                                        }
                                        std::string result;
                                        result.reserve(left->string.size() + self->string.size());
                                        result.insert(result.end(), left->string.begin(), left->string.end());
                                        result.insert(result.end(), self->string.begin(), self->string.end());

                                        (*success) = true;
                                        return this->new_string(c, false, result);
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
                                        std::string repeatedContent;
                                        value *multiplicationError = plasma::vm::virtual_machine::string_repeat(self,
                                                                                                                std::abs(
                                                                                                                        right->integer),
                                                                                                                &repeatedContent);
                                        if (multiplicationError != nullptr) {
                                            (*success) = false;
                                            return multiplicationError;
                                        }
                                        if (right->integer < 0) {
                                            std::reverse(repeatedContent.begin(), repeatedContent.end());
                                        }
                                        (*success) = true;
                                        return this->new_string(c, false, repeatedContent);
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
                                        std::string repeatedContent;
                                        value *multiplicationError = plasma::vm::virtual_machine::string_repeat(self,
                                                                                                                std::abs(
                                                                                                                        left->integer),
                                                                                                                &repeatedContent);
                                        if (multiplicationError != nullptr) {
                                            (*success) = false;
                                            return multiplicationError;
                                        }
                                        if (left->integer < 0) {
                                            std::reverse(repeatedContent.begin(), repeatedContent.end());
                                        }
                                        (*success) = true;
                                        return this->new_string(c, false, repeatedContent);
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
                                        if (right->typeId != String) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::string_equals(self, right,
                                                                                                            &comparison);
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
                                        if (left->typeId != String) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::string_equals(left, self,
                                                                                                            &comparison);
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
                                        if (right->typeId != String) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::string_equals(self, right,
                                                                                                            &comparison);
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
                                        if (left->typeId != String) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool comparison = false;
                                        value *comparisonError = plasma::vm::virtual_machine::string_equals(left, self,
                                                                                                            &comparison);
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
                                        value *containsError = plasma::vm::virtual_machine::string_contains(self,
                                                                                                            element,
                                                                                                            &contains);
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
                                            self->hash = this->hash_string(self->string);
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
                                        return this->new_string(c, false, self->string);
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
                                        return this->string_index(c, self, arguments[0], success);
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
                                        return this->string_iterator(c, self);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        bool parsingSuccess = false;
                                        auto result = plasma::general_tooling::parse_integer(self->string,
                                                                                             &parsingSuccess);
                                        if (parsingSuccess) {
                                            (*success) = true;
                                            return this->new_integer(c, false, result);
                                        }
                                        (*success) = false;
                                        return this->new_integer_parsing_error(c);
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
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        bool parsingSuccess = false;
                                        auto result = plasma::general_tooling::parse_float(self->string,
                                                                                           &parsingSuccess);
                                        if (parsingSuccess) {
                                            (*success) = true;
                                            return this->new_float(c, false, result);
                                        }
                                        (*success) = false;
                                        return this->NewFloatParsingError(c);
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
                                        return this->new_string(c, false, self->string);
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
                                        return this->get_boolean(c, !self->string.empty());
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
                                        return this->new_array(c, false, this->string_to_integer_content(c, self));
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
                                        return this->new_tuple(c, false, this->string_to_integer_content(c, self));
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}