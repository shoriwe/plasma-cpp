#include <random>
#include <ostream>
#include <iostream>
#include "vm/virtual_machine.h"


plasma::vm::virtual_machine::virtual_machine(std::istream *stdinFile,
                                             std::ostream *stdoutFile,
                                             std::ostream *stderrFile) {
    this->stdin_file = stdinFile;
    this->stdout_file = stdoutFile;
    this->stderr_file = stderrFile;
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
    //// Functions
    c->master->set(
            "println",
            this->new_function(
                    c,
                    true,
                    nullptr,
                    new_builtin_callable(
                            1,
                            [=](value *self, const std::vector<value *> &arguments, bool *success) {
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

                                (*this->stdout_file) << resultAsString->string << std::endl;
                                (*success) = true;
                                return this->get_none(c);
                            }
                    )
            )
    );
    // Objects
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
