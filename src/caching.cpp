#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::get_none() {
    return this->force_any_from_master(None);
}

plasma::vm::value *plasma::vm::virtual_machine::get_false() {
    return this->force_any_from_master(False);
}

plasma::vm::value *plasma::vm::virtual_machine::get_true() {
    return this->force_any_from_master(True);
}

plasma::vm::value *plasma::vm::virtual_machine::get_boolean(bool condition) {
    if (condition) {
        return this->get_true();
    }
    return this->get_false();
}

