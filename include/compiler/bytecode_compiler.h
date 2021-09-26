#ifndef PLASMA_BYTECODE_COMPILER_H
#define PLASMA_BYTECODE_COMPILER_H

#include "plasma_error.h"
#include "compiler/parser.h"
#include "vm/virtual_machine.h"

namespace plasma::bytecode_compiler {
    struct compiler {
        plasma::parser::parser *parser;

        explicit compiler(plasma::parser::parser *p);

        bool compile(vm::bytecode *result, error::error *compilationError) const;
    };
}

#endif //PLASMA_BYTECODE_COMPILER_H
