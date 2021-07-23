#include "compiler/bytecode_compiler.h"
#include "tools.h"

static bool compile_basic_literal(const plasma::ast::BasicLiteralExpression &basicLiteralExpression,
                                  std::vector<plasma::vm::instruction> *result,
                                  plasma::error::error *compilationError);

static bool compile_tuple(const plasma::ast::TupleExpression &tupleExpression,
                          std::vector<plasma::vm::instruction> *result,
                          plasma::error::error *compilationError);

static bool compile_array(const plasma::ast::ArrayExpression &arrayExpression,
                          std::vector<plasma::vm::instruction> *result,
                          plasma::error::error *compilationError);

static bool compile_expression(std::any *node, bool push, std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError);

static bool compile_statement(std::any *node, std::vector<plasma::vm::instruction> *result,
                              plasma::error::error *compilationError);

static bool compile_hash(const plasma::ast::HashExpression &hashExpression,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError);

plasma::bytecode_compiler::compiler::compiler(plasma::parser::parser *p) {
    this->parser = p;
}

bool plasma::bytecode_compiler::compiler::compile(vm::bytecode *result, error::error *compilationError) {
    std::vector<plasma::vm::instruction> arrayResult;
    bool success = this->compile_to_array(&arrayResult, compilationError);
    if (!success) {
        return false;
    }
    (*result) = vm::bytecode(arrayResult);
    return true;
}

bool plasma::bytecode_compiler::compiler::compile_to_array(std::vector<vm::instruction> *result,
                                                           error::error *compilationError) {
    ast::Program parsedProgram;
    if (!this->parser->parse(&parsedProgram, compilationError)) {
        return false;
    }
    std::vector<vm::instruction> beginBody;
    if (!parsedProgram.Begin.Body.empty()) {
        // ToDo: Compile Begin
    }
    std::vector<vm::instruction> programBody;
    if (!this->compile_body(parsedProgram.Body, &programBody, compilationError)) {
        return false;
    }
    std::vector<vm::instruction> endBody;
    if (!parsedProgram.End.Body.empty()) {
        // ToDo: Compile End
    }
    // Combine everything
    result->resize(beginBody.size() + programBody.size() + endBody.size());
    result->insert(result->end(), beginBody.begin(), beginBody.end());
    result->insert(result->end(), programBody.begin(), programBody.end());
    result->insert(result->end(), endBody.begin(), endBody.end());
    return true;
}

bool plasma::bytecode_compiler::compiler::compile_body(const std::vector<std::any> &body,
                                                       std::vector<vm::instruction> *result,
                                                       error::error *compilationError) {
    for (std::any node : body) {
        std::vector<vm::instruction> nodeCode;
        if (!plasma::bytecode_compiler::compiler::compile_node(&node, &nodeCode, compilationError)) {
            return false;
        }
        result->insert(result->end(), nodeCode.begin(), nodeCode.end());
    }
    return true;
}

static bool compile_basic_literal(const plasma::ast::BasicLiteralExpression &basicLiteralExpression,
                                  std::vector<plasma::vm::instruction> *result,
                                  plasma::error::error *compilationError) {
    switch (basicLiteralExpression.DirectValue) {
        case plasma::lexer::SingleQuoteString:
        case plasma::lexer::DoubleQuoteString:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewStringOP,
                            .value = plasma::general_tooling::replace_escaped(basicLiteralExpression.Token.string),
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        case plasma::lexer::ByteString:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewBytesOP,
                            .value = plasma::general_tooling::replace_escaped(basicLiteralExpression.Token.string),
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        case plasma::lexer::Integer:
        case plasma::lexer::HexadecimalInteger:
        case plasma::lexer::BinaryInteger:
        case plasma::lexer::OctalInteger:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewIntegerOP,
                            .value = std::stoll(
                                    plasma::general_tooling::remove_floor(basicLiteralExpression.Token.string), nullptr,
                                    0),
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        case plasma::lexer::Float:
        case plasma::lexer::ScientificFloat:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewFloatOP,
                            .value = std::stod(
                                    plasma::general_tooling::remove_floor(basicLiteralExpression.Token.string)),
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        case plasma::lexer::True:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::GetTrueOP,
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        case plasma::lexer::False:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::GetFalseOP,
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        case plasma::lexer::None:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::GetNoneOP,
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        default:
            plasma::error::new_unknown_vm_operation_error(compilationError, basicLiteralExpression.DirectValue);
            return false;
    }
    return true;
}

static bool compile_tuple(const plasma::ast::TupleExpression &tupleExpression,
                          std::vector<plasma::vm::instruction> *result,
                          plasma::error::error *compilationError) {
    for (size_t index = tupleExpression.Values.size(); index > -1; index--) {
        std::any node = tupleExpression.Values[index];
        std::vector<plasma::vm::instruction> nodeCode;
        if (!compile_expression(&node, true, &nodeCode, compilationError)) {
            return false;
        }
        result->insert(result->end(), nodeCode.begin(), nodeCode.end());
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewTupleOP,
                    .value = tupleExpression.Values.size()
            }
    );
    return true;
}

static bool compile_array(const plasma::ast::ArrayExpression &arrayExpression,
                          std::vector<plasma::vm::instruction> *result,
                          plasma::error::error *compilationError) {
    for (size_t index = arrayExpression.Values.size(); index > -1; index--) {
        std::any node = arrayExpression.Values[index];
        std::vector<plasma::vm::instruction> nodeCode;
        if (!compile_expression(&node, true, &nodeCode, compilationError)) {
            return false;
        }
        result->insert(result->end(), nodeCode.begin(), nodeCode.end());
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewTupleOP,
                    .value = arrayExpression.Values.size()
            }
    );
    return true;
}

static bool compile_hash(const plasma::ast::HashExpression &hashExpression,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError) {
    for (const auto &keyValue : hashExpression.Values) {
        std::any key = keyValue.Key;
        std::any value = keyValue.Value;
        {
            std::vector<plasma::vm::instruction> nodeCode;
            if (!compile_expression(&value, true, &nodeCode, compilationError)) {
                return false;
            }
            result->insert(result->end(), nodeCode.begin(), nodeCode.end());
        }
        {
            std::vector<plasma::vm::instruction> nodeCode;
            if (!compile_expression(&key, true, &nodeCode, compilationError)) {
                return false;
            }
            result->insert(result->end(), nodeCode.begin(), nodeCode.end());
        }
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewTupleOP,
                    .value = hashExpression.Values.size()
            }
    );
    return true;
}

static bool compile_expression(std::any *node, bool push, std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError) {
    bool compiledNode;
    if (node->type() == typeid(plasma::ast::BasicLiteralExpression)) {
        compiledNode = compile_basic_literal(std::any_cast<plasma::ast::BasicLiteralExpression>(*node), result,
                                             compilationError);
    } else if (node->type() == typeid(plasma::ast::TupleExpression)) {
        compiledNode = compile_tuple(std::any_cast<plasma::ast::TupleExpression>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::ArrayExpression)) {
        compiledNode = compile_array(std::any_cast<plasma::ast::ArrayExpression>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::HashExpression)) {
        compiledNode = compile_hash(std::any_cast<plasma::ast::HashExpression>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::UnaryExpression)) {
        compiledNode = compileUnaryExpression(std::any_cast<plasma::ast::UnaryExpression>(*node), result,
                                              compilationError);
    } else if (node->type() == typeid(plasma::ast::BinaryExpression)) {
        compiledNode = compileBinaryExpression(std::any_cast<plasma::ast::BinaryExpression>(*node), result,
                                               compilationError);
    } else if (node->type() == typeid(plasma::ast::ParenthesesExpression)) {
        compiledNode = compileParenthesesExpression(std::any_cast<plasma::ast::ParenthesesExpression>(*node),
                                                    result, compilationError);
    } else if (node->type() == typeid(plasma::ast::IfOneLinerExpression)) {
        compiledNode = compileIfOneLinerExpression(std::any_cast<plasma::ast::IfOneLinerExpression>(*node), result,
                                                   compilationError);
    } else if (node->type() == typeid(plasma::ast::UnlessOneLinerExpression)) {
        compiledNode = compileUnlessOneLinerExpression(std::any_cast<plasma::ast::UnlessOneLinerExpression>(*node),
                                                       result, compilationError);
    } else if (node->type() == typeid(plasma::ast::IndexExpression)) {
        compiledNode = compileIndexExpression(std::any_cast<plasma::ast::IndexExpression>(*node), result,
                                              compilationError);
    } else if (node->type() == typeid(plasma::ast::SelectorExpression)) {
        compiledNode = compileSelectorExpression(std::any_cast<plasma::ast::SelectorExpression>(*node), result,
                                                 compilationError);
    } else if (node->type() == typeid(plasma::ast::MethodInvocationExpression)) {
        compiledNode = compileMethodInvocationExpression(
                std::any_cast<plasma::ast::MethodInvocationExpression>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::Identifier)) {
        compiledNode = compileIdentifierExpression(std::any_cast<plasma::ast::Identifier>(*node), result,
                                                   compilationError);
    } else if (node->type() == typeid(plasma::ast::LambdaExpression)) {
        compiledNode = compileLambdaExpression(std::any_cast<plasma::ast::LambdaExpression>(*node), result,
                                               compilationError);
    } else if (node->type() == typeid(plasma::ast::GeneratorExpression)) {
        compiledNode = compileGeneratorExpression(std::any_cast<plasma::ast::GeneratorExpression>(*node), result,
                                                  compilationError);
    }
    if (push) {

    }

    return compiledNode;
}

static bool compile_statement(std::any *node, std::vector<plasma::vm::instruction> *result,
                              plasma::error::error *compilationError) {
    if (node->type() == typeid(plasma::ast::AssignStatement)) {
        return compileAssignStatement(std::any_cast<plasma::ast::AssignStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::FunctionDefinitionStatement)) {
        return compileFunctionDefinition(std::any_cast<plasma::ast::FunctionDefinitionStatement>(*node), result,
                                         compilationError);
    } else if (node->type() == typeid(plasma::ast::ReturnStatement)) {
        return compileReturnStatement(std::any_cast<plasma::ast::ReturnStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::IfStatement)) {
        return compileIfStatement(std::any_cast<plasma::ast::IfStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::UnlessStatement)) {
        return compileUnlessStatement(std::any_cast<plasma::ast::UnlessStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::DoWhileStatement)) {
        return compileDoWhileStatement(std::any_cast<plasma::ast::DoWhileStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::RedoStatement)) {
        return compileRedoStatement()
    } else if (node->type() == typeid(plasma::ast::BreakStatement)) {
        return compileBreakStatement()
    } else if (node->type() == typeid(plasma::ast::ContinueStatement)) {
        return compileContinueStatement()
    } else if (node->type() == typeid(plasma::ast::PassStatement)) {
        return compilePassStatement()
    } else if (node->type() == typeid(plasma::ast::WhileStatement)) {
        return compileWhileLoopStatement(std::any_cast<plasma::ast::WhileStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::UntilStatement)) {
        return compileUntilLoopStatement(std::any_cast<plasma::ast::UntilStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::ForStatement)) {
        return compileForLoopStatement(std::any_cast<plasma::ast::ForStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::TryStatement)) {
        return compileTryStatement(std::any_cast<plasma::ast::TryStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::ModuleStatement)) {
        return compileModuleStatement(std::any_cast<plasma::ast::ModuleStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::RaiseStatement)) {
        return compileRaiseStatement(std::any_cast<plasma::ast::RaiseStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::ClassStatement)) {
        return compileClassStatement(std::any_cast<plasma::ast::ClassStatement>(*node), result, compilationError);
    } else if (node->type() == typeid(plasma::ast::InterfaceStatement)) {
        return compileInterfaceStatement(std::any_cast<plasma::ast::InterfaceStatement>(*node), result,
                                         compilationError);
    } else if (node->type() == typeid(plasma::ast::SwitchStatement)) {
        return compileSwitchStatement(std::any_cast<plasma::ast::SwitchStatement>(*node), result, compilationError);
    }
}

bool plasma::bytecode_compiler::compiler::compile_node(std::any *node, std::vector<vm::instruction> *result,
                                                       error::error *compilationError) {
    if (ast::isExpression(node)) {
        return compile_expression(node, false, result, compilationError);
    }
    return compile_statement(node, result, compilationError);
}