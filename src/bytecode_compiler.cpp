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

static bool compile_expression(std::any node, bool push, std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError);

static bool compile_statement(std::any node, std::vector<plasma::vm::instruction> *result,
                              plasma::error::error *compilationError);

static bool compile_hash(const plasma::ast::HashExpression &hashExpression,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError);

static bool compile_binary_expression(const plasma::ast::BinaryExpression &binaryExpression,
                                      std::vector<plasma::vm::instruction> *result,
                                      plasma::error::error *compilationError);

static bool compile_unary_expression(const plasma::ast::UnaryExpression &unaryExpression,
                                     std::vector<plasma::vm::instruction> *result,
                                     plasma::error::error *compilationError);

static bool
compile_identifier_expression(const plasma::ast::Identifier &identifier, std::vector<plasma::vm::instruction> *result);

static bool
compile_selector_expression(const plasma::ast::SelectorExpression &selector,
                            std::vector<plasma::vm::instruction> *result,
                            plasma::error::error *compilationError);


static bool
compile_index_expression(const plasma::ast::IndexExpression &indexExpression,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError);

static bool
compile_method_invocation(const plasma::ast::MethodInvocationExpression &methodInvocationExpression,
                          std::vector<plasma::vm::instruction> *result,
                          plasma::error::error *compilationError);

plasma::bytecode_compiler::compiler::compiler(plasma::parser::parser *p) {
    this->parser = p;
}

static bool compile_node(std::any node, std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError) {
    std::cout << "Type: " << node.type().name() << std::endl;
    if (plasma::ast::isExpression(&node)) {
        return compile_expression(node, false, result, compilationError);
    }
    return compile_statement(node, result, compilationError);
}

static bool compile_body(const std::vector<std::any> &body,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError) {
    for (size_t index = 0; index < body.size(); index++) {
        std::cout << "Type: " << body[index].type().name() << std::endl;
        if (!compile_node(body[index], result, compilationError)) {
            return false;
        }
    }
    return true;
}

static bool compile_to_array(const plasma::ast::Program &parsedProgram, std::vector<plasma::vm::instruction> *result,
                             plasma::error::error *compilationError) {
    if (!parsedProgram.Begin.Body.empty()) {
        // ToDo: Compile Begin
    }
    if (!compile_body(parsedProgram.Body, result, compilationError)) {
        return false;
    }
    if (!parsedProgram.End.Body.empty()) {
        // ToDo: Compile End
    }
    return true;
}

bool plasma::bytecode_compiler::compiler::compile(vm::bytecode *result, error::error *compilationError) const {
    plasma::ast::Program parsedProgram;
    if (!this->parser->parse(&parsedProgram, compilationError)) {
        return false;
    }
    std::vector<plasma::vm::instruction> arrayResult;
    bool success = compile_to_array(parsedProgram, &arrayResult, compilationError);
    if (!success) {
        return false;
    }
    (*result) = vm::bytecode{
            .instructions = arrayResult,
            .index = 0
    };
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
                            .value = (int64_t) std::stoll(
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
    for (size_t index = tupleExpression.Values.size() - 1; index > -1; index--) {
        auto node = tupleExpression.Values[index];
        if (!compile_expression(node, true, result, compilationError)) {
            return false;
        }
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::PushOP
                }
        );
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
    for (size_t index = arrayExpression.Values.size() - 1; index > -1; index--) {
        std::any node = arrayExpression.Values[index];
        if (!compile_expression(node, true, result, compilationError)) {
            return false;
        }
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::PushOP
                }
        );
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
        if (!compile_expression(value, true, result, compilationError)) {
            return false;
        }
        if (!compile_expression(key, true, result, compilationError)) {
            return false;
        }
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::PushOP
                }
        );
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewTupleOP,
                    .value = hashExpression.Values.size()
            }
    );
    return true;
}

static bool compile_unary_expression(const plasma::ast::UnaryExpression &unaryExpression,
                                     std::vector<plasma::vm::instruction> *result,
                                     plasma::error::error *compilationError) {
    auto expression = unaryExpression.X;
    if (!compile_expression(expression, true, result, compilationError)) {
        return false;
    }
    uint8_t instruction;
    switch (unaryExpression.Operator.directValue) {
        case plasma::lexer::NegateBits:
            instruction = plasma::vm::NegateBitsOP;
            break;
        case plasma::lexer::Not:
        case plasma::lexer::SignNot:
            instruction = plasma::vm::BoolNegateOP;
            break;
        case plasma::lexer::Sub:
            instruction = plasma::vm::NegativeOP;
            break;
            //case plasma::lexer::Add: // Fixme: Complete this
            //    instruction = plasma::vm::PositiveOP;
            //    break;
        default:
            plasma::error::new_unknown_vm_operation_error(compilationError, unaryExpression.Operator.directValue);
            return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::UnaryOP,
                    .value = instruction
            }
    );
    return true;
}

static bool compile_binary_expression(const plasma::ast::BinaryExpression &binaryExpression,
                                      std::vector<plasma::vm::instruction> *result,
                                      plasma::error::error *compilationError) {
    auto rightHandSide = binaryExpression.RightHandSide;
    if (!compile_expression(rightHandSide, true, result, compilationError)) {
        return false;
    }
    auto leftHandSide = binaryExpression.LeftHandSide;
    if (!compile_expression(leftHandSide, true, result, compilationError)) {
        return false;
    }
    uint8_t instruction;
    switch (binaryExpression.Operator.directValue) {
        case plasma::lexer::Add:
            instruction = plasma::vm::AddOP;
            break;
        case plasma::lexer::Sub:
            instruction = plasma::vm::SubOP;
            break;
        case plasma::lexer::Star:
            instruction = plasma::vm::MulOP;
            break;
        case plasma::lexer::Div:
            instruction = plasma::vm::DivOP;
            break;
        case plasma::lexer::FloorDiv:
            instruction = plasma::vm::FloorDivOP;
            break;
        case plasma::lexer::Modulus:
            instruction = plasma::vm::ModOP;
            break;
        case plasma::lexer::PowerOf:
            instruction = plasma::vm::PowOP;
            break;
        case plasma::lexer::BitwiseXor:
            instruction = plasma::vm::BitXorOP;
            break;
        case plasma::lexer::BitWiseAnd:
            instruction = plasma::vm::BitAndOP;
            break;
        case plasma::lexer::BitwiseOr:
            instruction = plasma::vm::BitOrOP;
            break;
        case plasma::lexer::BitwiseLeft:
            instruction = plasma::vm::BitLeftOP;
            break;
        case plasma::lexer::BitwiseRight:
            instruction = plasma::vm::BitRightOP;
            break;
        case plasma::lexer::And:
            instruction = plasma::vm::AndOP;
            break;
        case plasma::lexer::Or:
            instruction = plasma::vm::OrOP;
            break;
        case plasma::lexer::Xor:
            instruction = plasma::vm::XorOP;
            break;
        case plasma::lexer::Equals:
            instruction = plasma::vm::EqualsOP;
            break;
        case plasma::lexer::NotEqual:
            instruction = plasma::vm::NotEqualsOP;
            break;
        case plasma::lexer::GreaterThan:
            instruction = plasma::vm::GreaterThanOP;
            break;
        case plasma::lexer::LessThan:
            instruction = plasma::vm::LessThanOP;
            break;
        case plasma::lexer::GreaterOrEqualThan:
            instruction = plasma::vm::GreaterThanOrEqualOP;
            break;
        case plasma::lexer::LessOrEqualThan:
            instruction = plasma::vm::LessThanOrEqualOP;
            break;
        case plasma::lexer::In:
            instruction = plasma::vm::ContainsOP;
            break;
        default:
            plasma::error::new_unknown_vm_operation_error(compilationError, binaryExpression.Operator.directValue);
            return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::BinaryOP,
                    .value = instruction
            }
    );
    return true;
}

static bool
compile_identifier_expression(const plasma::ast::Identifier &identifier, std::vector<plasma::vm::instruction> *result) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code  = plasma::vm::GetIdentifierOP,
                    .value = identifier.Token.string,
                    .line = static_cast<size_t>(identifier.Token.line)
            }
    );
    return true;
}

static bool
compile_selector_expression(const plasma::ast::SelectorExpression &selector,
                            std::vector<plasma::vm::instruction> *result,
                            plasma::error::error *compilationError) {
    std::any expression = selector.X;
    if (!compile_expression(expression, true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::SelectNameFromObjectOP,
                    .value = selector.Identifier.Token.string
            }
    );
    return true;
}

static bool
compile_index_expression(const plasma::ast::IndexExpression &indexExpression,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError) {
    if (!compile_expression(indexExpression.Source, true, result, compilationError)) {
        return false;
    }
    if (!compile_expression(indexExpression.Index, true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::IndexOP
            }
    );
    return true;
}

static bool compile_method_invocation(
        const plasma::ast::MethodInvocationExpression &methodInvocationExpression,
        std::vector<plasma::vm::instruction> *result,
        plasma::error::error *compilationError
) {
    for (auto argument = methodInvocationExpression.Arguments.rbegin();
         argument != methodInvocationExpression.Arguments.rend();
         argument++) {
        std::cout << "Argument Type: " << (*argument).type().name() << std::endl;
        if (!compile_expression((*argument), true, result, compilationError)) {
            return false;
        }
    }
    if (!compile_expression(methodInvocationExpression.Function, true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::MethodInvocationOP,
                    .value = methodInvocationExpression.Arguments.size()
            }
    );
    return true;
}

static bool compile_parentheses_expression(
        const plasma::ast::ParenthesesExpression &parenthesesExpression,
        std::vector<plasma::vm::instruction> *result,
        plasma::error::error *compilationError) {
    return compile_expression(parenthesesExpression.X, false, result, compilationError);
}

// Statements

static bool compile_assignment(const plasma::ast::AssignStatement &assignStatement,
                               std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError) {
    // Pre-assignment
    if (assignStatement.AssignOperator.directValue == plasma::lexer::Assign) {
        if (!compile_expression(assignStatement.RightHandSide, true, result, compilationError)) {
            return false;
        }
    } else { // AddAssign, SubAssign...
        uint8_t preOperation;
        switch (assignStatement.AssignOperator.directValue) {
            case plasma::lexer::AddAssign:
                preOperation = plasma::vm::AddOP;
                break;
            case plasma::lexer::SubAssign:
                preOperation = plasma::vm::SubOP;
                break;
            case plasma::lexer::StarAssign:
                preOperation = plasma::vm::MulOP;
                break;
            case plasma::lexer::DivAssign:
                preOperation = plasma::vm::DivOP;
                break;
            case plasma::lexer::ModulusAssign:
                preOperation = plasma::vm::ModOP;
                break;
            case plasma::lexer::PowerOfAssign:
                preOperation = plasma::vm::PowOP;
                break;
            case plasma::lexer::BitwiseXorAssign:
                preOperation = plasma::vm::BitXorOP;
                break;
            case plasma::lexer::BitWiseAndAssign:
                preOperation = plasma::vm::BitAndOP;
                break;
            case plasma::lexer::BitwiseOrAssign:
                preOperation = plasma::vm::BitOrOP;
                break;
            case plasma::lexer::BitwiseLeftAssign:
                preOperation = plasma::vm::BitLeftOP;
                break;
            case plasma::lexer::BitwiseRightAssign:
                preOperation = plasma::vm::BitRightOP;
                break;
            default:
                // Fixme
                break;
        }
        if (!compile_expression(
                plasma::ast::BinaryExpression{
                        .LeftHandSide = assignStatement.LeftHandSide,
                        .Operator = plasma::lexer::token{
                                .directValue = preOperation,
                                .kind = plasma::lexer::Operator,
                                .line = assignStatement.AssignOperator.line
                        },
                        .RightHandSide = assignStatement.RightHandSide
                }, true, result, compilationError
        )) {
            return false;
        }
    }
    //
    // Determine the assignment target
    if (assignStatement.LeftHandSide.type() == typeid(plasma::ast::Identifier)) {
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::AssignIdentifierOP,
                        .value = std::any_cast<plasma::ast::Identifier>(assignStatement.LeftHandSide).Token.string
                }
        );
    } else if (assignStatement.LeftHandSide.type() == typeid(plasma::ast::SelectorExpression)) {
        // Push owner
        if (!compile_expression(std::any_cast<plasma::ast::SelectorExpression>(assignStatement.LeftHandSide).X, true,
                                result, compilationError)) {
            return false;
        }
        //
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::AssignSelectorOP,
                        .value = std::any_cast<plasma::ast::SelectorExpression>(
                                assignStatement.LeftHandSide).Identifier.Token.string
                }
        );
    } else { // Index assign
        // Push source
        if (!compile_expression(std::any_cast<plasma::ast::IndexExpression>(assignStatement.LeftHandSide).Source, true,
                                result, compilationError)) {
            return false;
        }
        // Push Index
        if (!compile_expression(std::any_cast<plasma::ast::IndexExpression>(assignStatement.LeftHandSide).Index, true,
                                result, compilationError)) {
            return false;
        }
        //
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::AssignIndexOP,
                }
        );
    }
    return true;
}

static bool compile_expression(std::any node, bool push, std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError) {
    bool success = false;
    if (node.type() == typeid(plasma::ast::BasicLiteralExpression)) {
        success = compile_basic_literal(std::any_cast<plasma::ast::BasicLiteralExpression>(node), result,
                                        compilationError);
    } else if (node.type() == typeid(plasma::ast::TupleExpression)) {
        success = compile_tuple(std::any_cast<plasma::ast::TupleExpression>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::ArrayExpression)) {
        success = compile_array(std::any_cast<plasma::ast::ArrayExpression>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::HashExpression)) {
        success = compile_hash(std::any_cast<plasma::ast::HashExpression>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::UnaryExpression)) {
        success = compile_unary_expression(std::any_cast<plasma::ast::UnaryExpression>(node), result,
                                           compilationError);
    } else if (node.type() == typeid(plasma::ast::BinaryExpression)) {
        success = compile_binary_expression(std::any_cast<plasma::ast::BinaryExpression>(node), result,
                                            compilationError);
    } else if (node.type() == typeid(plasma::ast::ParenthesesExpression)) {
        success = compile_parentheses_expression(std::any_cast<plasma::ast::ParenthesesExpression>(node),
                                                 result, compilationError);
    }/* else if (node.type() == typeid(plasma::ast::IfOneLinerExpression)) {
        success = compile_if_one_linear_expression(std::any_cast<plasma::ast::IfOneLinerExpression>(node), result,
                                                   compilationError);
    } else if (node.type() == typeid(plasma::ast::UnlessOneLinerExpression)) {
        success = compile_unless_one_linear_expression(std::any_cast<plasma::ast::UnlessOneLinerExpression>(node),
                                                       result, compilationError);
    } */else if (node.type() == typeid(plasma::ast::IndexExpression)) {
        success = compile_index_expression(std::any_cast<plasma::ast::IndexExpression>(node), result,
                                           compilationError);
    } else if (node.type() == typeid(plasma::ast::SelectorExpression)) {
        success = compile_selector_expression(std::any_cast<plasma::ast::SelectorExpression>(node), result,
                                              compilationError);
    } else if (node.type() == typeid(plasma::ast::MethodInvocationExpression)) {
        success = compile_method_invocation(
                std::any_cast<plasma::ast::MethodInvocationExpression>(node), result, compilationError
        );
    } else if (node.type() == typeid(plasma::ast::Identifier)) {
        success = compile_identifier_expression(std::any_cast<plasma::ast::Identifier>(node), result);
    }/* else if (node.type() == typeid(plasma::ast::LambdaExpression)) {
        success = compile_lambda_expression(std::any_cast<plasma::ast::LambdaExpression>(node), result,
                                            compilationError);
    } else if (node.type() == typeid(plasma::ast::GeneratorExpression)) {
        success = compile_generator_expression(std::any_cast<plasma::ast::GeneratorExpression>(node), result,
                                               compilationError);
    }*/ else {
        std::cout << "None math\n";
    }
    if (!success) {
        return false;
    }
    if (push) {
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::PushOP,
                }
        );
    }
    return true;
}

static bool compile_statement(std::any node, std::vector<plasma::vm::instruction> *result,
                              plasma::error::error *compilationError) {
//     if (node.type() == typeid(plasma::ast::AssignStatement)) {
//         return compile_assignment(std::any_cast<plasma::ast::AssignStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::FunctionDefinitionStatement)) {
//         return compileFunctionDefinition(std::any_cast<plasma::ast::FunctionDefinitionStatement>(node), result,
//                                          compilationError);
//     } else if (node.type() == typeid(plasma::ast::ReturnStatement)) {
//         return compileReturnStatement(std::any_cast<plasma::ast::ReturnStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::IfStatement)) {
//         return compileIfStatement(std::any_cast<plasma::ast::IfStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::UnlessStatement)) {
//         return compileUnlessStatement(std::any_cast<plasma::ast::UnlessStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::DoWhileStatement)) {
//         return compileDoWhileStatement(std::any_cast<plasma::ast::DoWhileStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::RedoStatement)) {
//         return compileRedoStatement()
//     } else if (node.type() == typeid(plasma::ast::BreakStatement)) {
//         return compileBreakStatement()
//     } else if (node.type() == typeid(plasma::ast::ContinueStatement)) {
//         return compileContinueStatement()
//     } else if (node.type() == typeid(plasma::ast::PassStatement)) {
//         return compilePassStatement()
//     } else if (node.type() == typeid(plasma::ast::WhileStatement)) {
//         return compileWhileLoopStatement(std::any_cast<plasma::ast::WhileStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::UntilStatement)) {
//         return compileUntilLoopStatement(std::any_cast<plasma::ast::UntilStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::ForStatement)) {
//         return compileForLoopStatement(std::any_cast<plasma::ast::ForStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::TryStatement)) {
//         return compileTryStatement(std::any_cast<plasma::ast::TryStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::ModuleStatement)) {
//         return compileModuleStatement(std::any_cast<plasma::ast::ModuleStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::RaiseStatement)) {
//         return compileRaiseStatement(std::any_cast<plasma::ast::RaiseStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::ClassStatement)) {
//         return compileClassStatement(std::any_cast<plasma::ast::ClassStatement>(node), result, compilationError);
//     } else if (node.type() == typeid(plasma::ast::InterfaceStatement)) {
//         return compileInterfaceStatement(std::any_cast<plasma::ast::InterfaceStatement>(node), result,
//                                          compilationError);
//     } else if (node.type() == typeid(plasma::ast::SwitchStatement)) {
//         return compileSwitchStatement(std::any_cast<plasma::ast::SwitchStatement>(node), result, compilationError);
//     }
    return true;
}