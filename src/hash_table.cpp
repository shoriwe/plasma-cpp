#include "vm/virtual_machine.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::HashTableInitialize(bool isBuiltIn) {
    return [this, isBuiltIn](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                Equals,
                [this, isBuiltIn, c, object]() -> value * {
                    return this->new_function(
                            c,
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != HashTable) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool result;
                                        value *equalsError = this->hashtable_equals(
                                                c,
                                                self,
                                                right,
                                                &result
                                        );
                                        if (equalsError != nullptr) {
                                            (*success) = false;
                                            return equalsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, result);
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
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId != HashTable) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool result;
                                        value *equalsError = this->hashtable_equals(
                                                c,
                                                left,
                                                self,
                                                &result
                                        );
                                        if (equalsError != nullptr) {
                                            (*success) = false;
                                            return equalsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, result);
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
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId == HashTable) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool result;
                                        value *equalsError = this->hashtable_equals(
                                                c,
                                                self,
                                                right,
                                                &result
                                        );
                                        if (equalsError != nullptr) {
                                            (*success) = false;
                                            return equalsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, !result);
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
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId == HashTable) {
                                            (*success) = true;
                                            return this->get_false(c);
                                        }
                                        bool result;
                                        value *equalsError = this->hashtable_equals(c, left, self,
                                                                                    &result);
                                        if (equalsError != nullptr) {
                                            (*success) = false;
                                            return equalsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(c, !result);
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
                                        value *containsError = this->hashtable_contains(c, self, element,
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
                                        value *containsError = this->hashtable_contains(c, source, self,
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
                                        (*success) = false;
                                        return this->NewUnhashableTypeError(c, self->get_type(c, this));
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
                                        return this->hashtable_copy(c, self, success);
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
                                        return this->hashtable_index(c, self, arguments[0], success);
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
                                    1,
                                    [this, c](value *self, const std::vector<value *> &arguments,
                                              bool *success) -> value * {
                                        return this->hashtable_assign(c, self, arguments[0], arguments[1], success);
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
                                        return this->hashtable_iterator(c, self);
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
                                        return this->hashtable_to_string(c, self, success);
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
                                        return this->get_boolean(c, !self->keyValues.empty());
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
                                        return this->new_array(c, false, hashtable_to_content(self));
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
                                        return this->new_tuple(c, false, hashtable_to_content(self));
                                    }
                            )
                    );
                }
        );
        return nullptr;
    };
}