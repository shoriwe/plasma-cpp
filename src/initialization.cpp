#include <random>
#include <ostream>
#include <iostream>
#include "vm/virtual_machine.h"


plasma::vm::virtual_machine::virtual_machine(std::istream &stdinFile,
                                             std::ostream &stdoutFile,
                                             std::ostream &stderrFile) :
        stdin_file(stdinFile),
        stdout_file(stdoutFile),
        stderr_file(stderrFile) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> dis(
            std::numeric_limits<std::uint64_t>::min(),
            std::numeric_limits<std::uint64_t>::max()
    );
    this->seed = dis(gen);
    this->currentId = 1;
}


void
plasma::vm::virtual_machine::load_builtin_object(context *c, const std::string &symbol, const object_loader &loader) {
    c->peek_symbol_table()->set(symbol, loader(c, this));
}

void plasma::vm::virtual_machine::load_builtin_symbols(context *c,
                                                       const std::unordered_map<std::string, object_loader> &symbols) {
    for (const auto &keyValue : symbols) {
        c->peek_symbol_table()->set(keyValue.first, keyValue.second(c, this));
    }
}

void plasma::vm::virtual_machine::initialize_context(context *c) {
    c->master = c->allocate_symbol_table(nullptr);
    c->push_symbol_table(c->master);
    this->initialize_builtin_symbols(c);
}

void plasma::vm::virtual_machine::initialize_builtin_symbols(context *c) {
    // Initialize all the symbols that are set by default in the virtual machine
    //// Classes
    c->master->set(
            ObjectName,
            this->new_type(c, true, ObjectName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->ObjectInitialize(false)
                           }
            )
    );
    c->master->set(
            TypeName,
            this->new_type(c, true, TypeName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->TypeInitialize(false)
                           }
            )
    );
    c->master->set(
            StringName,
            this->new_type(c, true, StringName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->StringInitialize(false)
                           }
            )
    );
    c->master->set(
            BytesName,
            this->new_type(c, true, BytesName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->BytesInitialize(false)
                           }
            )
    );
    c->master->set(
            IntegerName,
            this->new_type(c, true, IntegerName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->IntegerInitialize(false)
                           }
            )
    );
    c->master->set(
            FloatName,
            this->new_type(c, true, FloatName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->FloatInitialize(false)
                           }
            )
    );
    c->master->set(
            ArrayName,
            this->new_type(c, true, ArrayName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->ArrayInitialize(false)
                           }
            )
    );
    c->master->set(
            TupleName,
            this->new_type(c, true, TupleName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->TupleInitialize(false)
                           }
            )
    );
    c->master->set(
            HashTableName,
            this->new_type(c, true, HashTableName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->HashTableInitialize(false)
                           }
            )
    );
    c->master->set(
            ModuleName,
            this->new_type(c, true, ModuleName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->ObjectInitialize(false)
                           }
            )
    );
    c->master->set(
            IteratorName,
            this->new_type(c, true, IteratorName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->IteratorInitialize(false)
                           }
            )
    );
    c->master->set(
            BoolName,
            this->new_type(c, true, BoolName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->BoolInitialize(false)
                           }
            )
    );
    c->master->set(
            FunctionName,
            this->new_type(c, true, FunctionName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->ObjectInitialize(false)
                           }
            )
    );
    c->master->set(
            NoneName,
            this->new_type(c, true, NoneName, std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->NoneInitialize(false)
                           }
            )
    );
    //// Error types
    auto runtimeError = this->new_type(
            c,
            true,
            RuntimeError,
            std::vector<value *>(),
            constructor{
                    .isBuiltIn = true,
                    .callback = this->RuntimeErrorInitialize(false)
            }
    );
    c->master->set(RuntimeError, runtimeError);
    c->master->set(InvalidTypeError,
                   this->new_type(
                           c,
                           true,
                           InvalidTypeError,
                           std::vector<value *>(),
                           constructor{
                                   .isBuiltIn = true,
                                   .callback = this->InvalidTypeErrorInitialize(false)
                           }
                   )
    );
    c->master->set(NotImplementedCallableError,
                   this->new_type(c, true, NotImplementedCallableError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = NotImplementedCallableErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(ObjectConstructionError,
                   this->new_type(c, true, ObjectConstructionError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = ObjectConstructionErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(ObjectWithNameNotFoundError,
                   this->new_type(c, true, ObjectWithNameNotFoundError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = ObjectWithNameNotFoundErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(InvalidNumberOfArgumentsError,
                   this->new_type(c, true, InvalidNumberOfArgumentsError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = InvalidNumberOfArgumentsErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(UnhashableTypeError,
                   this->new_type(c, true, UnhashableTypeError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = UnhashableTypeErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(IndexOutOfRangeError,
                   this->new_type(c, true, IndexOutOfRangeError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = IndexOutOfRangeErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(KeyNotFoundError,
                   this->new_type(c, true, KeyNotFoundError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = KeyNotFoundErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(IntegerParsingError,
                   this->new_type(c, true, IntegerParsingError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = IntegerParsingErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(FloatParsingError,
                   this->new_type(c, true, FloatParsingError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = FloatParsingErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(BuiltInSymbolProtectionError,
                   this->new_type(c, true, BuiltInSymbolProtectionError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = BuiltInSymbolProtectionErrorInitialize(false)
                                  }
                   )
    );
    c->master->set(ObjectNotCallableError,
                   this->new_type(c, true, ObjectNotCallableError, std::vector<value *>(), constructor{
                                          .isBuiltIn = true,
                                          .callback = ObjectNotCallableErrorInitialize(false)
                                  }
                   )
    );
    //// Functions
    c->master->set(
            "println",
            this->new_function(
                    c,
                    true,
                    nullptr,
                    new_builtin_callable(
                            1,
                            [this, c](value *self, const std::vector<value *> &arguments, bool *success) {
                                value *resultAsString;
                                auto target = arguments[0];
                                if (target->typeId == String) {
                                    resultAsString = target;
                                } else {
                                    bool found = false;
                                    value *resultToString = target->get(c, this, ToString, &found);
                                    if (!found) {
                                        (*success) = false;
                                        return this->NewObjectWithNameNotFoundError(c, target, ToString);
                                    }
                                    bool callSuccess = false;

                                    resultAsString = this->call_function(c, resultToString,
                                                                         std::vector<value *>(), &callSuccess);
                                    if (!callSuccess) {

                                        (*success) = false;
                                        return resultAsString;
                                    }
                                    if (resultAsString->typeId != String) {
                                        (*success) = false;
                                        return this->NewInvalidTypeError(c,
                                                                         resultAsString->get_type(c, this),
                                                                         std::vector<std::string>{StringName});
                                    }
                                }
                                this->stdout_file << resultAsString->string << std::endl;
                                (*success) = true;
                                return this->get_none(c);
                            }
                    )
            )
    );
    c->master->set(
            "print",
            this->new_function(
                    c,
                    true,
                    nullptr,
                    new_builtin_callable(
                            1,
                            [this, c](value *self, const std::vector<value *> &arguments, bool *success) {

                                value *resultAsString;
                                auto target = arguments[0];

                                if (target->typeId == String) {
                                    resultAsString = target;
                                } else {
                                    bool found = false;
                                    value *resultToString = target->get(c, this, ToString, &found);
                                    if (!found) {
                                        (*success) = false;
                                        return this->NewObjectWithNameNotFoundError(c, target, ToString);
                                    }
                                    bool callSuccess = false;

                                    resultAsString = this->call_function(c, resultToString,
                                                                         std::vector<value *>(), &callSuccess);
                                    if (!callSuccess) {

                                        (*success) = false;
                                        return resultAsString;
                                    }
                                    if (resultAsString->typeId != String) {
                                        (*success) = false;
                                        return this->NewInvalidTypeError(c,
                                                                         resultAsString->get_type(c, this),
                                                                         std::vector<std::string>{StringName});
                                    }

                                }
                                this->stdout_file << resultAsString->string;
                                (*success) = true;
                                return this->get_none(c);
                            }
                    )
            )
    );
    //// Objects
    c->master->set(
            None,
            this->new_none(c, true)
    );
    c->master->set(
            True,
            this->new_bool(c, true, true)
    );
    c->master->set(
            False,
            this->new_bool(c, true, false)
    );
}

uint64_t plasma::vm::virtual_machine::next_id() {
    uint64_t result = this->currentId;
    this->currentId++;
    return result;
}
