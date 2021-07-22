#include "vm/virtual_machine.h"

plasma::vm::constructor_callback plasma::vm::virtual_machine::HashTableInitialize(bool isBuiltIn) {
    return [=](context *c, value *object) -> value * {
        object->set_on_demand_symbol(
                Equals,
                [=]() -> value * {
                    return this->new_function(
                            c,
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId != HashTable) {
                                            (*success) = true;
                                            return this->get_false();
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
                                        return this->get_boolean(result);
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
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId != HashTable) {
                                            (*success) = true;
                                            return this->get_false();
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
                                        return this->get_boolean(result);
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
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *right = arguments[0];
                                        if (right->typeId == HashTable) {
                                            (*success) = true;
                                            return this->get_false();
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
                                        return this->get_boolean(!result);
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
                            false,
                            object,
                            new_builtin_callable(
                                    1,
                                    [=](value *self, const std::vector<value *> &arguments, bool *success) -> value * {
                                        value *left = arguments[0];
                                        if (left->typeId == HashTable) {
                                            (*success) = true;
                                            return this->get_false();
                                        }
                                        bool result;
                                        value *equalsError = this->hashtable_equals(c, left, self,
                                                                                    &result);
                                        if (equalsError != nullptr) {
                                            (*success) = false;
                                            return equalsError;
                                        }
                                        (*success) = true;
                                        return this->get_boolean(!result);
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
                                        value *element = arguments[0];
                                        bool contains = false;
                                        value *containsError = this->hashtable_contains(c, self, element,
                                                                                        &contains);
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
                                        value *source = arguments[0];
                                        bool contains = false;
                                        value *containsError = this->hashtable_contains(c, source, self,
                                                                                        &contains);
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
                                        return this->hashtable_copy(c, self, success);
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
                                        return this->hashtable_index(c, self, arguments[0], success);
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
                                        return this->hashtable_assign(c, self, arguments[0], arguments[1], success);
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
                                        return this->hashtable_iterator(c, self);
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
                                        return this->hashtable_to_string(c, self, success);
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
                                        return this->get_boolean(!self->keyValues.empty());
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
                                        (*success) = true;
                                        return this->new_array(c, false, hashtable_to_content(self));
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