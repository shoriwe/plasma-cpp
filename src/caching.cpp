#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::get_none() {
    return this->force_any_from_master(None);
}

plasma::vm::value *plasma::vm::virtual_machine::get_false() {
    return this->force_any_from_master(FalseName);
}

plasma::vm::value *plasma::vm::virtual_machine::get_true() {
    return this->force_any_from_master(TrueName);
}

