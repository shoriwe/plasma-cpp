#include <unordered_map>

#include "vm/virtual_machine.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::ObjectInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) {
        object->set_on_demand_symbol(
                Initialize,
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
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Negate,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        bool result = false;
                                        value *boolError = this->interpret_as_boolean(c, self, &result);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, !result);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                And,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->interpret_as_boolean(c, self, &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->interpret_as_boolean(c, arguments[0], &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, left && right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightAnd,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->interpret_as_boolean(c, arguments[0], &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->interpret_as_boolean(c, self, &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, left && right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Or,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->interpret_as_boolean(c, self, &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->interpret_as_boolean(c, arguments[0], &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, left || right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightOr,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->interpret_as_boolean(c, arguments[0], &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->interpret_as_boolean(c, self, &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, left || right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Xor,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->interpret_as_boolean(c, self, &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->interpret_as_boolean(c, arguments[0], &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, left != right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightXor,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->interpret_as_boolean(c, arguments[0], &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->interpret_as_boolean(c, self, &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, left != right);
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
                                        (*success) = true;
                                        return this->get_boolean(c, self->id == arguments[0]->id);
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
                                        (*success) = true;
                                        return this->get_boolean(c, arguments[0]->id == self->id);
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
                                        (*success) = true;
                                        return this->get_boolean(c, self->id != arguments[0]->id);
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
                                        (*success) = true;
                                        return this->get_boolean(c, arguments[0]->id != self->id);
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
                                            size_t objectHash = this->hash_string(
                                                    self->get_type(c, this)->name + "@" + std::to_string(self->id));
                                            self->hash = objectHash;
                                        }
                                        (*success) = true;
                                        return this->new_integer(c, false, self->hash);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Class,
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
                                        return self->get_type(c, this);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SubClasses,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        std::unordered_map<size_t, value *> classesMap;
                                        std::stack<value *> pendingTypes;
                                        pendingTypes.push(self->get_type(c, this));
                                        value *currentProcessingType;
                                        while (!pendingTypes.empty()) {
                                            currentProcessingType = pendingTypes.top();
                                            pendingTypes.pop();
                                            if (classesMap.contains(currentProcessingType->id)) {
                                                continue;
                                            }
                                            classesMap[currentProcessingType->id] = currentProcessingType;
                                            for (const auto &subSubType : currentProcessingType->subTypes) {
                                                if (classesMap.contains(subSubType->id)) {
                                                    continue;
                                                }
                                                pendingTypes.push(subSubType);
                                            }
                                        }
                                        std::vector<value *> classesResult;
                                        classesResult.reserve(classesMap.size());
                                        for (const auto &keyValue : classesMap) {
                                            classesResult.push_back(keyValue.second);
                                        }
                                        (*success) = true;
                                        return this->new_tuple(c, false, classesResult);
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
                                        return this->new_string(c, false, self->get_type(c, this)->name + "{" +
                                                                          std::to_string(self->id) + "}");
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
                                        return this->get_true(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetInteger,
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
                                        return this->new_integer(c, false, self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetFloat,
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
                                        return this->new_float(c, false, self->floating);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetString,
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
                GetBool,
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
                                        return this->new_integer(c, false, self->boolean);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetBytes,
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
                GetContent,
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
                                        return this->new_array(c, false, self->content);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetKeyValues,
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
                                        value *result = this->new_hash_table(c, false);
                                        result->keyValues = self->keyValues;
                                        return result;
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetInteger,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Integer) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(c, this),
                                                                             std::vector<std::string>{IntegerName});

                                        }
                                        self->integer = argument->integer;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetFloat,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Float) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(c, this),
                                                                             std::vector<std::string>{FloatName});

                                        }
                                        self->floating = argument->floating;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetString,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != String) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(c, this),
                                                                             std::vector<std::string>{StringName});

                                        }
                                        self->string = argument->string;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetBool,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Boolean) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(c, this),
                                                                             std::vector<std::string>{BoolName});

                                        }
                                        self->boolean = argument->boolean;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetBytes,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Bytes) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(c, this),
                                                                             std::vector<std::string>{BytesName});

                                        }
                                        self->bytes = argument->bytes;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetContent,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Array && argument->typeId != Tuple) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(c, this),
                                                                             std::vector<std::string>{ArrayName,
                                                                                                      TupleName});

                                        }
                                        self->content = argument->content;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetKeyValues,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != HashTable) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(c, this),
                                                                             std::vector<std::string>{HashTableName});

                                        }
                                        self->keyValues = argument->keyValues;
                                        (*success) = true;
                                        return this->get_none(c);
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}