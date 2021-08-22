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

static bool
compileReturnStatement(const plasma::ast::ReturnStatement &returnStatement,
                       std::vector<plasma::vm::instruction> *result,
                       plasma::error::error *compilationError);

plasma::bytecode_compiler::compiler::compiler(plasma::parser::parser *p) {
    this->parser = p;
}

static bool compile_node(std::any node, std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError) {

    if (plasma::ast::isExpression(&node)) {
        return compile_expression(node, false, result, compilationError);
    }
    return compile_statement(node, result, compilationError);
}

static bool
compile_class_function_definition(const plasma::ast::FunctionDefinitionStatement &functionDefinitionStatement,
                                  std::vector<plasma::vm::instruction> *result,
                                  plasma::error::error *compilationError);

static bool compile_class_body(const std::vector<std::any> &body,
                               std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError) {
    for (const auto &node : body) {
        if (node.type() == typeid(plasma::ast::FunctionDefinitionStatement)) {
            // Do something to compile interfaces and class functions
            if (!compile_class_function_definition(
                    std::any_cast<plasma::ast::FunctionDefinitionStatement>(node), result, compilationError)) {
                return false;
            }
        } else if (!compile_node(node, result, compilationError)) {
            return false;
        }
    }
    return true;
}

static bool compile_body(const std::vector<std::any> &body,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError) {
    for (const auto &node : body) {
        if (!compile_node(node, result, compilationError)) {
            return false;
        }
    }
    return true;
}

static bool compile_to_array(const plasma::ast::Program &parsedProgram, std::vector<plasma::vm::instruction> *result,
                             plasma::error::error *compilationError) {
    if (!parsedProgram.Begin.Body.empty()) {
        if (!compile_body(parsedProgram.Begin.Body, result, compilationError)) {
            return false;
        }
    }
    if (!compile_body(parsedProgram.Body, result, compilationError)) {
        return false;
    }
    if (!parsedProgram.End.Body.empty()) {
        if (!compile_body(parsedProgram.End.Body, result, compilationError)) {
            return false;
        }
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
    int64_t integerValue;
    double floatValue;
    bool parsingSuccess;
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
            integerValue = plasma::general_tooling::parse_integer(basicLiteralExpression.Token.string,
                                                                  &parsingSuccess);
            if (!parsingSuccess) {
                plasma::error::new_unknown_vm_operation_error(compilationError, basicLiteralExpression.DirectValue);
                return false;
            }
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewIntegerOP,
                            .value = integerValue,
                            .line = static_cast<size_t>(basicLiteralExpression.Token.line),
                    }
            );
            break;
        case plasma::lexer::Float:
        case plasma::lexer::ScientificFloat:
            floatValue = plasma::general_tooling::parse_float(basicLiteralExpression.Token.string, &parsingSuccess);
            if (!parsingSuccess) {
                plasma::error::new_unknown_vm_operation_error(compilationError, basicLiteralExpression.DirectValue);
                return false;
            }
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewFloatOP,
                            .value = floatValue,
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
    for (auto tupleValue = tupleExpression.Values.rbegin();
         tupleValue != tupleExpression.Values.rend();
         tupleValue++) {
        if (!compile_expression((*tupleValue), true, result, compilationError)) {
            return false;
        }
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
    for (auto argument = arrayExpression.Values.rbegin();
         argument != arrayExpression.Values.rend();
         argument++) {

        if (!compile_expression((*argument), true, result, compilationError)) {
            return false;
        }
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewArrayOP,
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
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewHashOP,
                    .value = hashExpression.Values.size()
            }
    );
    return true;
}

static bool compile_unary_expression(const plasma::ast::UnaryExpression &unaryExpression,
                                     std::vector<plasma::vm::instruction> *result,
                                     plasma::error::error *compilationError) {
    if (!compile_expression(unaryExpression.X, true, result, compilationError)) {
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
        case plasma::lexer::BitwiseAnd:
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

static bool compile_if_one_liner_expression(
        const plasma::ast::IfOneLinerExpression &ifOneLinerExpression,
        bool push,
        std::vector<plasma::vm::instruction> *result,
        plasma::error::error *compilationError) {

    std::vector<plasma::vm::instruction> condition;
    if (!compile_expression(ifOneLinerExpression.Condition, true, &condition, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> ifResult;
    if (!compile_expression(ifOneLinerExpression.Result, push, &ifResult, compilationError)) {
        return false;
    }

    std::any elseResultAST;
    if (ifOneLinerExpression.ElseResult.has_value()) {
        elseResultAST = ifOneLinerExpression.ElseResult;
    } else {
        elseResultAST = plasma::ast::Identifier{
                .Token= plasma::lexer::token{
                        .string = plasma::vm::None,
                        .kind = plasma::lexer::IdentifierKind
                }
        };
    }
    std::vector<plasma::vm::instruction> elseResult;
    if (!compile_expression(elseResultAST, push, &elseResult, compilationError)) {
        return false;
    }

    result->insert(result->end(), condition.begin(), condition.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::IfJumpOP,
                    .value = ifResult.size() + 1
            }
    );
    result->insert(result->end(), ifResult.begin(), ifResult.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::JumpOP,
                    .value = elseResult.size()
            }
    );
    result->insert(result->end(), elseResult.begin(), elseResult.end());
    return true;
}

static bool compile_unless_one_liner_expression(
        const plasma::ast::UnlessOneLinerExpression &unlessOneLinerExpression,
        bool push,
        std::vector<plasma::vm::instruction> *result,
        plasma::error::error *compilationError) {
    std::vector<plasma::vm::instruction> condition;
    if (!compile_expression(unlessOneLinerExpression.Condition, true, &condition, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> unlessResult;
    if (!compile_expression(unlessOneLinerExpression.Result, push, &unlessResult, compilationError)) {
        return false;
    }

    std::any elseResultAST;
    std::vector<plasma::vm::instruction> elseResult;
    if (unlessOneLinerExpression.ElseResult.has_value()) {
        elseResultAST = unlessOneLinerExpression.ElseResult;
    } else {
        elseResultAST = plasma::ast::Identifier{
                .Token= plasma::lexer::token{
                        .string = plasma::vm::None,
                        .kind = plasma::lexer::IdentifierKind
                }
        };
    }
    if (!compile_expression(elseResultAST, push, &elseResult, compilationError)) {
        return false;
    }

    result->insert(result->end(), condition.begin(), condition.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::UnlessJumpOP,
                    .value = unlessResult.size() + 1
            }
    );
    result->insert(result->end(), unlessResult.begin(), unlessResult.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::JumpOP,
                    .value = elseResult.size()
            }
    );
    result->insert(result->end(), elseResult.begin(), elseResult.end());
    return true;
}

static bool compile_lambda_expression(const plasma::ast::LambdaExpression &lambdaExpression,
                                      std::vector<plasma::vm::instruction> *result,
                                      plasma::error::error *compilationError) {
    std::vector<std::string> arguments;
    for (const auto &argument : lambdaExpression.Arguments) {
        arguments.push_back(argument.Token.string);
    }
    std::vector<plasma::vm::instruction> body;
    body.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = arguments
            }
    );
    if (!compileReturnStatement(lambdaExpression.Output, &body, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewLambdaFunctionOP,
                    .value = plasma::vm::FunctionInformation{
                            .bodyLength = body.size(),
                            .numberOfArguments = arguments.size()
                    }
            }
    );
    result->insert(result->end(), body.begin(), body.end());
    return true;
}

static bool compile_generator_expression(
        const plasma::ast::GeneratorExpression &generatorExpression,
        std::vector<plasma::vm::instruction> *result,
        plasma::error::error *compilationError
) {
    if (!compile_expression(generatorExpression.Source, true, result, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> operation;
    std::vector<std::string> receivers;
    for (const auto &receiver : generatorExpression.Receivers) {
        receivers.push_back(receiver.Token.string);
    }
    operation.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = receivers
            }
    );

    std::vector<std::any> generatorOperation;
    generatorOperation.push_back(generatorExpression.Operation);
    if (
            !compileReturnStatement(
                    plasma::ast::ReturnStatement{
                            .Results = generatorOperation
                    },
                    &operation,
                    compilationError)
            ) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewGeneratorOP,
                    .value = plasma::vm::generator_information{
                            .numberOfReceivers = receivers.size(),
                            .operationLength = operation.size()
                    }
            }
    );
    result->insert(result->end(), operation.begin(), operation.end());
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
    } else if (node.type() == typeid(plasma::ast::IfOneLinerExpression)) {
        return compile_if_one_liner_expression(
                std::any_cast<plasma::ast::IfOneLinerExpression>(node),
                push,
                result,
                compilationError
        );
    } else if (node.type() == typeid(plasma::ast::UnlessOneLinerExpression)) {
        return compile_unless_one_liner_expression(
                std::any_cast<plasma::ast::UnlessOneLinerExpression>(node),
                push,
                result,
                compilationError
        );
    } else if (node.type() == typeid(plasma::ast::IndexExpression)) {
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
    } else if (node.type() == typeid(plasma::ast::LambdaExpression)) {
        success = compile_lambda_expression(
                std::any_cast<plasma::ast::LambdaExpression>(node),
                result,
                compilationError
        );
    } else if (node.type() == typeid(plasma::ast::GeneratorExpression)) {
        success = compile_generator_expression(
                std::any_cast<plasma::ast::GeneratorExpression>(node),
                result,
                compilationError
        );
    } else {
        // FIXME
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

// Statements

static bool compile_assignment(const plasma::ast::AssignStatement &assignStatement,
                               std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError) {
    // Pre-assignment
    if (assignStatement.AssignOperator.directValue == plasma::lexer::Assign) { // Basic Assign
        if (!compile_expression(assignStatement.RightHandSide, true, result, compilationError)) {
            return false;
        }
    } else { // AddAssign, SubAssign...
        uint8_t preOperation;
        switch (assignStatement.AssignOperator.directValue) {
            case plasma::lexer::AddAssign:
                preOperation = plasma::lexer::Add;
                break;
            case plasma::lexer::SubAssign:
                preOperation = plasma::lexer::Sub;
                break;
            case plasma::lexer::StarAssign:
                preOperation = plasma::lexer::Star;
                break;
            case plasma::lexer::DivAssign:
                preOperation = plasma::lexer::Div;
                break;
            case plasma::lexer::ModulusAssign:
                preOperation = plasma::lexer::Modulus;
                break;
            case plasma::lexer::PowerOfAssign:
                preOperation = plasma::lexer::PowerOf;
                break;
            case plasma::lexer::BitwiseXorAssign:
                preOperation = plasma::lexer::BitwiseXor;
                break;
            case plasma::lexer::BitwiseAndAssign:
                preOperation = plasma::lexer::BitwiseAnd;
                break;
            case plasma::lexer::BitwiseOrAssign:
                preOperation = plasma::lexer::BitwiseOr;
                break;
            case plasma::lexer::BitwiseLeftAssign:
                preOperation = plasma::lexer::BitwiseLeft;
                break;
            case plasma::lexer::BitwiseRightAssign:
                preOperation = plasma::lexer::BitwiseRight;
                break;
            default:
                // Fixme
                break;
        }
        if (
                !compile_expression(
                        plasma::ast::BinaryExpression{
                                .LeftHandSide = assignStatement.LeftHandSide,
                                .Operator = plasma::lexer::token{
                                        .directValue = preOperation,
                                        .kind = plasma::lexer::Operator,
                                        .line = assignStatement.AssignOperator.line
                                },
                                .RightHandSide = assignStatement.RightHandSide
                        }, true, result, compilationError
                )
                ) {
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

static bool
compile_function_definition(const plasma::ast::FunctionDefinitionStatement &functionDefinitionStatement,
                            std::vector<plasma::vm::instruction> *result,
                            plasma::error::error *compilationError) {
    // ImplementMe:
    std::vector<std::string> arguments;
    for (const auto &argument : functionDefinitionStatement.Arguments) {
        arguments.push_back(argument.Token.string);
    }

    std::vector<plasma::vm::instruction> body;

    body.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = arguments
            }
    );

    if (!compile_body(functionDefinitionStatement.Body, &body, compilationError)) {
        return false;
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewFunctionOP,
                    .value = plasma::vm::FunctionInformation{
                            .name = functionDefinitionStatement.Name.Token.string,
                            .bodyLength = body.size(),
                            .numberOfArguments = arguments.size()
                    }
            }
    );

    result->insert(result->end(), body.begin(), body.end());

    return true;
}

static bool
compile_class_function_definition(const plasma::ast::FunctionDefinitionStatement &functionDefinitionStatement,
                                  std::vector<plasma::vm::instruction> *result,
                                  plasma::error::error *compilationError) {
    // ImplementMe:
    std::vector<std::string> arguments;
    for (const auto &argument : functionDefinitionStatement.Arguments) {
        arguments.push_back(argument.Token.string);
    }

    std::vector<plasma::vm::instruction> body;

    body.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = arguments
            }
    );

    if (!compile_body(functionDefinitionStatement.Body, &body, compilationError)) {
        return false;
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewClassFunctionOP,
                    .value = plasma::vm::FunctionInformation{
                            .name = functionDefinitionStatement.Name.Token.string,
                            .bodyLength = body.size(),
                            .numberOfArguments = arguments.size()
                    }
            }
    );

    result->insert(result->end(), body.begin(), body.end());

    return true;
}

static bool
compile_class_statement(const plasma::ast::ClassStatement &classStatement, std::vector<plasma::vm::instruction> *result,
                        plasma::error::error *compilationError) {
    for (const auto &base : classStatement.Bases) {
        if (!compile_expression(base, true, result, compilationError)) {
            return false;
        }
    }
    std::vector<plasma::vm::instruction> body;
    if (!compile_class_body(classStatement.Body, &body, compilationError)) {
        return false;
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewClassOP,
                    .value = plasma::vm::ClassInformation{
                            .name = classStatement.Name.Token.string,
                            .bodyLength = body.size(),
                            .numberOfBases = classStatement.Bases.size()
                    }
            }
    );

    result->insert(result->end(), body.begin(), body.end());

    return true;
}

static bool
compileReturnStatement(const plasma::ast::ReturnStatement &returnStatement,
                       std::vector<plasma::vm::instruction> *result,
                       plasma::error::error *compilationError) {
    for (auto returnValue = returnStatement.Results.rbegin();
         returnValue != returnStatement.Results.rend();
         returnValue++) {
        if (!compile_expression((*returnValue), true, result, compilationError)) {
            return false;
        }
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::ReturnOP,
                    .value = returnStatement.Results.size()
            }
    );
    return true;
}

struct elif_information {
    std::vector<plasma::vm::instruction> condition;
    std::vector<plasma::vm::instruction> body;
};

static bool
compileIfStatement(const plasma::ast::IfStatement &ifStatement,
                   std::vector<plasma::vm::instruction> *result,
                   plasma::error::error *compilationError) {

    if (!compile_expression(ifStatement.Condition, true, result, compilationError)) {
        return false;
    }
    std::vector<plasma::vm::instruction> body;
    if (!compile_body(ifStatement.Body, &body, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::IfJumpOP,
                    .value = body.size() + 1
            }
    );
    result->insert(result->end(), body.begin(), body.end());

    size_t jump = 0;
    std::vector<elif_information> elifBlocks;
    for (auto const &elifBlock : ifStatement.ElifBlocks) {
        elif_information elifInformation;
        if (!compile_expression(elifBlock.Condition, true, &elifInformation.condition, compilationError)) {
            return false;
        }
        if (!compile_body(elifBlock.Body, &elifInformation.body, compilationError)) {
            return false;
        }
        elifBlocks.push_back(elifInformation);
        jump += elifInformation.condition.size() + 1 + elifInformation.body.size() + 1;
    }
    std::vector<plasma::vm::instruction> elseBody;
    if (!ifStatement.Else.empty()) {
        if (!compile_body(ifStatement.Else, &elseBody, compilationError)) {
            return false;
        }
    }
    jump += elseBody.size();
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::JumpOP,
                    .value = jump
            }
    );
    for (const auto &elifBlock : elifBlocks) {
        jump -= elifBlock.condition.size() + 1 + elifBlock.body.size() + 1;
        result->insert(result->end(), elifBlock.condition.begin(), elifBlock.condition.end());
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::IfJumpOP,
                        .value = elifBlock.body.size() + 1
                }
        );
        result->insert(result->end(), elifBlock.body.begin(), elifBlock.body.end());
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::JumpOP,
                        .value = jump
                }
        );
    }
    if (!elseBody.empty()) {
        result->insert(result->end(), elseBody.begin(), elseBody.end());
    }
    return true;
}

static bool
compileUnlessStatement(const plasma::ast::UnlessStatement &unlessStatement,
                       std::vector<plasma::vm::instruction> *result,
                       plasma::error::error *compilationError) {

    if (!compile_expression(unlessStatement.Condition, true, result, compilationError)) {
        return false;
    }
    std::vector<plasma::vm::instruction> body;
    if (!compile_body(unlessStatement.Body, &body, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::UnlessJumpOP,
                    .value = body.size() + 1
            }
    );
    result->insert(result->end(), body.begin(), body.end());

    size_t jump = 0;
    std::vector<elif_information> elifBlocks;
    for (auto const &elifBlock : unlessStatement.ElifBlocks) {
        elif_information elifInformation;
        if (!compile_expression(elifBlock.Condition, true, &elifInformation.condition, compilationError)) {
            return false;
        }
        if (!compile_body(elifBlock.Body, &elifInformation.body, compilationError)) {
            return false;
        }
        elifBlocks.push_back(elifInformation);
        jump += elifInformation.condition.size() + 1 + elifInformation.body.size() + 1;
    }
    std::vector<plasma::vm::instruction> elseBody;
    if (!unlessStatement.Else.empty()) {
        if (!compile_body(unlessStatement.Else, &elseBody, compilationError)) {
            return false;
        }
    }
    jump += elseBody.size();
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::JumpOP,
                    .value = jump
            }
    );
    for (const auto &elifBlock : elifBlocks) {
        jump -= elifBlock.condition.size() + 1 + elifBlock.body.size() + 1;
        result->insert(result->end(), elifBlock.condition.begin(), elifBlock.condition.end());
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::UnlessJumpOP,
                        .value = elifBlock.body.size() + 1
                }
        );
        result->insert(result->end(), elifBlock.body.begin(), elifBlock.body.end());
        result->push_back(
                plasma::vm::instruction{
                        .op_code = plasma::vm::JumpOP,
                        .value = jump
                }
        );
    }
    if (!elseBody.empty()) {
        result->insert(result->end(), elseBody.begin(), elseBody.end());
    }
    return true;
}

static bool
compileSwitchStatement(const plasma::ast::SwitchStatement &switchStatement,
                       std::vector<plasma::vm::instruction> *result,
                       plasma::error::error *compilationError) {
    plasma::ast::IfStatement ifEquivalent;
    bool first = true;
    for (const auto &switchCase : switchStatement.CaseBlocks) {
        if (first) {
            ifEquivalent.Condition = plasma::ast::BinaryExpression{
                    .LeftHandSide = switchStatement.Target,
                    .Operator = plasma::lexer::token{
                            .directValue = plasma::lexer::In,
                            .kind = plasma::lexer::Operator
                    },
                    .RightHandSide = plasma::ast::TupleExpression{
                            .Values = switchCase.Cases
                    }
            };
            ifEquivalent.Body = switchCase.Body;
            first = false;
        } else {
            plasma::ast::ElifBlock elifBlock;
            elifBlock.Condition = plasma::ast::BinaryExpression{
                    .LeftHandSide = switchStatement.Target,
                    .Operator = plasma::lexer::token{
                            .directValue = plasma::lexer::In,
                            .kind = plasma::lexer::Operator
                    },
                    .RightHandSide = plasma::ast::TupleExpression{
                            .Values = switchCase.Cases
                    }
            };
            elifBlock.Body = switchCase.Body;
            ifEquivalent.ElifBlocks.push_back(elifBlock);
        }
    }
    if (!switchStatement.Default.empty()) {
        ifEquivalent.Else = switchStatement.Default;
    }
    return compileIfStatement(ifEquivalent, result, compilationError);
}

static bool
compileWhileStatement(const plasma::ast::WhileStatement &whileStatement,
                      std::vector<plasma::vm::instruction> *result,
                      plasma::error::error *compilationError) {
    std::vector<plasma::vm::instruction> condition;
    if (!compile_expression(whileStatement.Condition, true, &condition, compilationError)) {
        return false;
    }
    std::vector<plasma::vm::instruction> body;
    if (!compile_body(whileStatement.Body, &body, compilationError)) {
        return false;
    }
    for (size_t index = 0; index < body.size(); index++) {
        switch (body[index].op_code) {
            case plasma::vm::RedoOP:
                body[index].op_code = plasma::vm::RJumpOP;
                body[index].value = index + 1;
                break;
            case plasma::vm::ContinueOP:
                body[index].op_code = plasma::vm::RJumpOP;
                body[index].value = index + 1 + 1 + condition.size();
                break;
            case plasma::vm::BreakOP:
                body[index].op_code = plasma::vm::JumpOP;
                body[index].value = body.size() - index;
                break;
            default:
                break;
        }
    }
    result->insert(result->end(), condition.begin(), condition.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::IfJumpOP,
                    .value = body.size() + 1
            }
    );
    result->insert(result->end(), body.begin(), body.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::RJumpOP,
                    .value = 1 + body.size() + condition.size() + 1
            }
    );
    return true;
}

static bool
compileUntilStatement(const plasma::ast::UntilStatement &untilStatement,
                      std::vector<plasma::vm::instruction> *result,
                      plasma::error::error *compilationError) {
    std::vector<plasma::vm::instruction> condition;
    if (!compile_expression(untilStatement.Condition, true, &condition, compilationError)) {
        return false;
    }
    std::vector<plasma::vm::instruction> body;
    if (!compile_body(untilStatement.Body, &body, compilationError)) {
        return false;
    }
    for (size_t index = 0; index < body.size(); index++) {
        switch (body[index].op_code) {
            case plasma::vm::RedoOP:
                body[index].op_code = plasma::vm::RJumpOP;
                body[index].value = index + 1;
                break;
            case plasma::vm::ContinueOP:
                body[index].op_code = plasma::vm::RJumpOP;
                body[index].value = index + 1 + 1 + condition.size();
                break;
            case plasma::vm::BreakOP:
                body[index].op_code = plasma::vm::JumpOP;
                body[index].value = body.size() - index;
                break;
            default:
                break;
        }
    }
    result->insert(result->end(), condition.begin(), condition.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::UnlessJumpOP,
                    .value = body.size() + 1
            }
    );
    result->insert(result->end(), body.begin(), body.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::RJumpOP,
                    .value = 1 + body.size() + condition.size() + 1
            }
    );
    return true;
}

static bool
compileDoWhileStatement(const plasma::ast::DoWhileStatement &doWhileStatement,
                        std::vector<plasma::vm::instruction> *result,
                        plasma::error::error *compilationError) {
    std::vector<plasma::vm::instruction> condition;
    if (!compile_expression(doWhileStatement.Condition, true, &condition, compilationError)) {
        return false;
    }
    std::vector<plasma::vm::instruction> body;
    if (!compile_body(doWhileStatement.Body, &body, compilationError)) {
        return false;
    }
    for (size_t index = 0; index < body.size(); index++) {
        switch (body[index].op_code) {
            case plasma::vm::RedoOP:
                body[index].op_code = plasma::vm::RJumpOP;
                body[index].value = index + 1;
                break;
            case plasma::vm::ContinueOP:
                body[index].op_code = plasma::vm::JumpOP;
                body[index].value = body.size() - index - 1;
                break;
            case plasma::vm::BreakOP:
                body[index].op_code = plasma::vm::JumpOP;
                body[index].value = body.size() - 1 - index + condition.size() + 1;
                break;
            default:
                break;
        }
    }
    result->insert(result->end(), body.begin(), body.end());
    result->insert(result->end(), condition.begin(), condition.end());
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::RUnlessJumpOP,
                    .value = 1 + condition.size() + body.size()
            }
    );
    return true;
}

static bool
compileRedoStatement(std::vector<plasma::vm::instruction> *result) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::RedoOP,
            }
    );
    return true;
}

static bool
compileContinueStatement(std::vector<plasma::vm::instruction> *result) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::ContinueOP,
            }
    );
    return true;
}

static bool
compileBreakStatement(std::vector<plasma::vm::instruction> *result) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::BreakOP,
            }
    );
    return true;
}

static bool
compilePassStatement(std::vector<plasma::vm::instruction> *result) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NOP,
            }
    );
    return true;
}

static bool
compileRaiseStatement(const plasma::ast::RaiseStatement &raiseStatement, std::vector<plasma::vm::instruction> *result,
                      plasma::error::error *compilationError) {
    if (!compile_expression(raiseStatement.X, true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::RaiseOP
            }
    );
    return true;
}

static bool compileModuleStatement(const plasma::ast::ModuleStatement &moduleStatement,
                                   std::vector<plasma::vm::instruction> *result,
                                   plasma::error::error *compilationError) {
    std::vector<plasma::vm::instruction> body;
    if (!compile_body(moduleStatement.Body, &body, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewModuleOP,
                    .value = plasma::vm::ClassInformation{
                            .name = moduleStatement.Name.Token.string,
                            .bodyLength = body.size()
                    }
            }
    );
    result->insert(result->end(), body.begin(), body.end());
    return true;
}

static bool compileInterfaceStatement(const plasma::ast::InterfaceStatement &interfaceStatement,
                                      std::vector<plasma::vm::instruction> *result,
                                      plasma::error::error *compilationError) {
    for (const auto &base : interfaceStatement.Bases) {
        if (!compile_expression(base, true, result, compilationError)) {
            return false;
        }
    }
    std::vector<plasma::vm::instruction> body;
    for (const auto &function : interfaceStatement.MethodDefinitions) {
        if (!compile_class_function_definition(function, &body, compilationError)) {
            return false;
        }
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewInterfaceOP,
                    .value = plasma::vm::ClassInformation{
                            .name = interfaceStatement.Name.Token.string,
                            .bodyLength = body.size(),
                            .numberOfBases = interfaceStatement.Bases.size()
                    }
            }
    );

    result->insert(result->end(), body.begin(), body.end());

    return true;
}

static bool compile_statement(std::any node, std::vector<plasma::vm::instruction> *result,
                              plasma::error::error *compilationError) {
    if (node.type() == typeid(plasma::ast::AssignStatement)) {
        return compile_assignment(std::any_cast<plasma::ast::AssignStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::FunctionDefinitionStatement)) {
        return compile_function_definition(std::any_cast<plasma::ast::FunctionDefinitionStatement>(node), result,
                                           compilationError);
    } else if (node.type() == typeid(plasma::ast::ReturnStatement)) {
        return compileReturnStatement(std::any_cast<plasma::ast::ReturnStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::IfStatement)) {
        return compileIfStatement(std::any_cast<plasma::ast::IfStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::UnlessStatement)) {
        return compileUnlessStatement(std::any_cast<plasma::ast::UnlessStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::SwitchStatement)) {
        return compileSwitchStatement(std::any_cast<plasma::ast::SwitchStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::WhileStatement)) {
        return compileWhileStatement(std::any_cast<plasma::ast::WhileStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::UntilStatement)) {
        return compileUntilStatement(std::any_cast<plasma::ast::UntilStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::DoWhileStatement)) {
        return compileDoWhileStatement(std::any_cast<plasma::ast::DoWhileStatement>(node), result, compilationError);
    } /* else if (node.type() == typeid(plasma::ast::ForStatement)) {
        return compileForLoopStatement(std::any_cast<plasma::ast::ForStatement>(node), result, compilationError);
    }*/ else if (node.type() == typeid(plasma::ast::RedoStatement)) {
        return compileRedoStatement(result);
    } else if (node.type() == typeid(plasma::ast::BreakStatement)) {
        return compileBreakStatement(result);
    } else if (node.type() == typeid(plasma::ast::ContinueStatement)) {
        return compileContinueStatement(result);
    } else if (node.type() == typeid(plasma::ast::PassStatement)) {
        return compilePassStatement(result);
    } /*else if (node.type() == typeid(plasma::ast::TryStatement)) {
        return compileTryStatement(std::any_cast<plasma::ast::TryStatement>(node), result, compilationError);
    } */ else if (node.type() == typeid(plasma::ast::ModuleStatement)) {
        return compileModuleStatement(std::any_cast<plasma::ast::ModuleStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::RaiseStatement)) {
        return compileRaiseStatement(std::any_cast<plasma::ast::RaiseStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::ClassStatement)) {
        return compile_class_statement(std::any_cast<plasma::ast::ClassStatement>(node), result, compilationError);
    } else if (node.type() == typeid(plasma::ast::InterfaceStatement)) {
        return compileInterfaceStatement(std::any_cast<plasma::ast::InterfaceStatement>(node), result,
                                         compilationError);
    }
    return true;
}