#include <utility>

#include "vm/virtual_machine.h"

size_t plasma::vm::bytecode::length() const {
    return this->instructions.size();
}

bool plasma::vm::bytecode::has_next() const {
    return this->index < this->instructions.size();
}

plasma::vm::instruction plasma::vm::bytecode::peek() {
    return this->instructions[this->index];
}

plasma::vm::instruction plasma::vm::bytecode::next() {
    instruction result = this->instructions[this->index];
    this->index++;
    return result;
}

std::vector<plasma::vm::instruction> plasma::vm::bytecode::nextN(size_t n) {
    std::vector<instruction> result(
            &this->instructions[this->index],
            &this->instructions[this->index + n]
    );
    this->index += n;
    return result;
}

void plasma::vm::bytecode::jump(size_t offset) {
    this->index += offset;
}
