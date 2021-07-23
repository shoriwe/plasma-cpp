#ifndef PLASMA_BYTECODE_COMPILER_H
#define PLASMA_BYTECODE_COMPILER_H

#include "plasma_error.h"
#include "compiler/parser.h"
#include "vm/virtual_machine.h"

namespace plasma::bytecode_compiler {
    struct compiler {
        plasma::parser::parser *parser;

        explicit compiler(plasma::parser::parser *p);

        bool compile(vm::bytecode *result, error::error *compilationError);

        bool compile_to_array(std::vector<vm::instruction> *result, error::error *compilationError);

        bool compile_body(const std::vector<std::any> &body, std::vector<vm::instruction> *result,
                          error::error *compilationError);

        static bool compile_node(std::any *node, std::vector<vm::instruction> *result,
                          error::error *compilationError);
    };
}

#endif //PLASMA_BYTECODE_COMPILER_H
