#include <unordered_map>

#include "vm/virtual_machine.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::ObjectInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) {
        object->set_on_demand_symbol(
                Initialize,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
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
                Negate,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        bool result = false;
                                        value *boolError = this->quick_get_bool(c, self, &result);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(!result);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                And,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->quick_get_bool(c, self, &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->quick_get_bool(c, arguments[0], &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(left && right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightAnd,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->quick_get_bool(c, arguments[0], &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->quick_get_bool(c, self, &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(left && right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Or,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->quick_get_bool(c, self, &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->quick_get_bool(c, arguments[0], &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(left || right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightOr,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->quick_get_bool(c, arguments[0], &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->quick_get_bool(c, self, &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(left || right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                Xor,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->quick_get_bool(c, self, &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->quick_get_bool(c, arguments[0], &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(left != right);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                RightXor,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *boolError;
                                        bool left;
                                        boolError = this->quick_get_bool(c, arguments[0], &left);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        bool right;
                                        boolError = this->quick_get_bool(c, self, &right);
                                        if (boolError != nullptr) {
                                            (*success) = false;
                                            return boolError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(left != right);
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
                                        (*success) = true;
                                        return this->get_boolean(self->id == arguments[0]->id);
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
                                        (*success) = true;
                                        return this->get_boolean(arguments[0]->id == self->id);
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
                                        (*success) = true;
                                        return this->get_boolean(self->id != arguments[0]->id);
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
                                        (*success) = true;
                                        return this->get_boolean(arguments[0]->id != self->id);
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
                                        if (self->hash == 0) {
                                            size_t objectHash = this->hash_string(
                                                    self->get_type(this)->name + "@" + std::to_string(self->id));
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return self->get_type(this);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SubClasses,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        std::unordered_map<size_t, value *> classesMap;
                                        std::stack<value *> pendingTypes;
                                        pendingTypes.push(self->get_type(this));
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_string(c, false, self->get_type(this)->name + "{" +
                                                                          std::to_string(self->id) + "}");
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
                                        return this->get_true();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetInteger,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_integer(c, false, self->integer);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetFloat,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_float(c, false, self->floating);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetString,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_string(c, false, self->string);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetBool,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_integer(c, false, self->boolean);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetBytes,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_bytes(c, false, self->bytes);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetContent,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        (*success) = true;
                                        return this->new_array(c, false, self->content);
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                GetKeyValues,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    0,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
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
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Integer) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(this),
                                                                             std::vector<std::string>{IntegerName});

                                        }
                                        self->integer = argument->integer;
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetFloat,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Float) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(this),
                                                                             std::vector<std::string>{FloatName});

                                        }
                                        self->floating = argument->floating;
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetString,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != String) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(this),
                                                                             std::vector<std::string>{StringName});

                                        }
                                        self->string = argument->string;
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetBool,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Boolean) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(this),
                                                                             std::vector<std::string>{BoolName});

                                        }
                                        self->boolean = argument->boolean;
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetBytes,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Bytes) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(this),
                                                                             std::vector<std::string>{BytesName});

                                        }
                                        self->bytes = argument->bytes;
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetContent,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != Array && argument->typeId != Tuple) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(this),
                                                                             std::vector<std::string>{ArrayName,
                                                                                                      TupleName});

                                        }
                                        self->content = argument->content;
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        object->set_on_demand_symbol(
                SetKeyValues,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            isBuiltIn,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *argument = arguments[0];
                                        if (argument->typeId != HashTable) {
                                            (*success) = false;
                                            return this->NewInvalidTypeError(c, argument->get_type(this),
                                                                             std::vector<std::string>{HashTableName});

                                        }
                                        self->keyValues = argument->keyValues;
                                        (*success) = true;
                                        return this->get_none();
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}