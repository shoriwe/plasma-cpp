#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::get_none(context *c) {
    return this->force_any_from_master(c, None);
}

plasma::vm::value *plasma::vm::virtual_machine::get_false(context *c) {
    return this->force_any_from_master(c, False);
}

plasma::vm::value *plasma::vm::virtual_machine::get_true(context *c) {
    return this->force_any_from_master(c, True);
}

plasma::vm::value *plasma::vm::virtual_machine::get_boolean(context *c, bool condition) {
    if (condition) {
        return this->get_true(c);
    }
    return this->get_false(c);
}

