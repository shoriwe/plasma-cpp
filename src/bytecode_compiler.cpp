#include "compiler/bytecode_compiler.h"
#include "tools.h"

plasma::bytecode_compiler::compiler::compiler(plasma::parser::parser *p) {
    this->parser = p;
}

static bool
compile_class_function_definition(plasma::ast::FunctionDefinitionStatement *functionDefinitionStatement,
                                  std::vector<plasma::vm::instruction> *result,
                                  plasma::error::error *compilationError);

static bool compile_class_body(const std::vector<plasma::ast::Node *> &body,
                               std::vector<plasma::vm::instruction> *result,
                               plasma::error::error *compilationError) {
    for (plasma::ast::Node *node : body) {
        if (node->TypeID == plasma::ast::FunctionDefinitionID) {
            // Do something to compile interfaces and class functions
            if (!compile_class_function_definition(dynamic_cast<plasma::ast::FunctionDefinitionStatement *>(node),
                                                   result, compilationError)) {
                return false;
            }
        } else if (!node->compile(result, compilationError)) {
            return false;
        }
    }
    return true;
}

static bool compile_body(std::vector<plasma::ast::Node *> body,
                         std::vector<plasma::vm::instruction> *result,
                         plasma::error::error *compilationError) {
    for (plasma::ast::Node *node : body) {
        if (!node->compile(result, compilationError)) {
            return false;
        }
    }
    return true;
}

static bool compile_to_array(const plasma::ast::Program &parsedProgram, std::vector<plasma::vm::instruction> *result,
                             plasma::error::error *compilationError) {
    if (!parsedProgram.Begin->Body.empty()) {
        if (!compile_body(parsedProgram.Begin->Body, result, compilationError)) {
            return false;
        }
    }
    if (!compile_body(parsedProgram.Body, result, compilationError)) {
        return false;
    }
    if (!parsedProgram.End->Body.empty()) {
        if (!compile_body(parsedProgram.End->Body, result, compilationError)) {
            return false;
        }
    }
    return true;
}

bool plasma::bytecode_compiler::compiler::compile(vm::bytecode *result, error::error *compilationError) const {
    plasma::ast::Program *parsedProgram = this->parser->parse();
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

bool plasma::ast::BasicLiteralExpression::compile(std::vector<vm::instruction> *result,
                                                  plasma::error::error *compilationError) {
    int64_t integerValue;
    double floatValue;
    bool parsingSuccess;
    switch (this->Token.directValue) {
        case plasma::lexer::SingleQuoteString:
        case plasma::lexer::DoubleQuoteString:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewStringOP,
                            .value = plasma::general_tooling::replace_escaped(this->Token.string),
                            .line = static_cast<size_t>(this->Token.line),
                    }
            );
            break;
        case plasma::lexer::ByteString:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewBytesOP,
                            .value = plasma::general_tooling::replace_escaped(this->Token.string),
                            .line = static_cast<size_t>(this->Token.line),
                    }
            );
            break;
        case plasma::lexer::Integer:
        case plasma::lexer::HexadecimalInteger:
        case plasma::lexer::BinaryInteger:
        case plasma::lexer::OctalInteger:
            integerValue = plasma::general_tooling::parse_integer(this->Token.string,
                                                                  &parsingSuccess);
            if (!parsingSuccess) {
                plasma::error::new_unknown_vm_operation_error(compilationError, this->Token.directValue);
                return false;
            }
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewIntegerOP,
                            .value = integerValue,
                            .line = static_cast<size_t>(this->Token.line),
                    }
            );
            break;
        case plasma::lexer::Float:
        case plasma::lexer::ScientificFloat:
            floatValue = plasma::general_tooling::parse_float(this->Token.string, &parsingSuccess);
            if (!parsingSuccess) {
                plasma::error::new_unknown_vm_operation_error(compilationError, this->Token.directValue);
                return false;
            }
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::NewFloatOP,
                            .value = floatValue,
                            .line = static_cast<size_t>(this->Token.line),
                    }
            );
            break;
        case plasma::lexer::True:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::GetTrueOP,
                            .line = static_cast<size_t>(this->Token.line),
                    }
            );
            break;
        case plasma::lexer::False:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::GetFalseOP,
                            .line = static_cast<size_t>(this->Token.line),
                    }
            );
            break;
        case plasma::lexer::None:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::GetNoneOP,
                            .line = static_cast<size_t>(this->Token.line),
                    }
            );
            break;
        default:
            plasma::error::new_unknown_vm_operation_error(compilationError, this->Token.directValue);
            return false;
    }
    return true;
}

bool plasma::ast::TupleExpression::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {
    for (auto tupleValue = this->Values.rbegin();
         tupleValue != this->Values.rend();
         tupleValue++) {
        if (!(*tupleValue)->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewTupleOP,
                    .value = this->Values.size()
            }
    );
    return true;
}

bool plasma::ast::ArrayExpression::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {
    for (auto argument = this->Values.rbegin();
         argument != this->Values.rend();
         argument++) {
        if (!(*argument)->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewArrayOP,
                    .value = this->Values.size()
            }
    );
    return true;
}

bool plasma::ast::HashExpression::compile(std::vector<vm::instruction> *result,
                                          plasma::error::error *compilationError) {
    for (KeyValue *keyValue : this->KeyValues) {
        if (!keyValue->Value->compile_and_push(true, result, compilationError)) {
            return false;
        }
        if (!keyValue->Key->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewHashOP,
                    .value = this->KeyValues.size()
            }
    );
    return true;
}

bool plasma::ast::UnaryExpression::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {
    if (!this->X->compile_and_push(true, result, compilationError)) {
        return false;
    }
    uint8_t instruction;
    switch (this->Operator.directValue) {
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
            plasma::error::new_unknown_vm_operation_error(compilationError, this->Operator.directValue);
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

bool plasma::ast::BinaryExpression::compile(std::vector<vm::instruction> *result,
                                            plasma::error::error *compilationError) {
    auto rightHandSide = this->RightHandSide;
    if (!rightHandSide->compile_and_push(true, result, compilationError)) {
        return false;
    }
    auto leftHandSide = this->LeftHandSide;
    if (!leftHandSide->compile_and_push(true, result, compilationError)) {
        return false;
    }
    uint8_t instruction;
    switch (this->Operator.directValue) {
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
            plasma::error::new_unknown_vm_operation_error(compilationError, this->Operator.directValue);
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

bool plasma::ast::Identifier::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code  = plasma::vm::GetIdentifierOP,
                    .value = this->Token.string,
                    .line = static_cast<size_t>(this->Token.line)
            }
    );
    return true;
}

bool plasma::ast::SelectorExpression::compile(std::vector<vm::instruction> *result,
                                              plasma::error::error *compilationError) {
    ;
    if (!this->X->compile_and_push(true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::SelectNameFromObjectOP,
                    .value = this->Identifier->Token.string
            }
    );
    return true;
}

bool plasma::ast::IndexExpression::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {
    if (!this->Source->compile_and_push(true, result, compilationError)) {
        return false;
    }
    if (!this->Index->compile_and_push(true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::IndexOP
            }
    );
    return true;
}

bool plasma::ast::MethodInvocationExpression::compile(std::vector<vm::instruction> *result,
                                                      plasma::error::error *compilationError) {
    for (auto argument = this->Arguments.rbegin();
         argument != this->Arguments.rend();
         argument++) {

        if (!(*argument)->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    if (!this->Function->compile_and_push(true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::MethodInvocationOP,
                    .value = this->Arguments.size()
            }
    );
    return true;
}

bool plasma::ast::ParenthesesExpression::compile(std::vector<vm::instruction> *result,
                                                 plasma::error::error *compilationError) {
    return this->X->compile(result, compilationError);
}

bool plasma::ast::IfOneLinerExpression::compile(std::vector<vm::instruction> *result,
                                                plasma::error::error *compilationError) {

    std::vector<plasma::vm::instruction> condition;
    if (!this->Condition->compile_and_push(true, &condition, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> ifResult;
    if (!this->Result->compile_and_push(true, &ifResult, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> elseResult;
    if (!this->ElseResult->compile_and_push(true, &elseResult, compilationError)) {
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

bool plasma::ast::UnlessOneLinerExpression::compile(std::vector<vm::instruction> *result,
                                                    plasma::error::error *compilationError) {
    std::vector<plasma::vm::instruction> condition;
    if (!this->Condition->compile_and_push(true, &condition, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> unlessResult;
    if (!this->Result->compile_and_push(true, &unlessResult, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> elseResult;
    if (!this->ElseResult->compile_and_push(true, &elseResult, compilationError)) {
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

bool plasma::ast::LambdaExpression::compile(std::vector<vm::instruction> *result,
                                            plasma::error::error *compilationError) {
    std::vector<std::string> arguments;
    for (Identifier *argument : this->Arguments) {
        arguments.push_back(argument->Token.string);
    }
    std::vector<plasma::vm::instruction> body;
    body.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = arguments
            }
    );
    if (!this->Output->compile(&body, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewLambdaFunctionOP,
                    .value = plasma::vm::function_information{
                            .bodyLength = body.size(),
                            .numberOfArguments = arguments.size()
                    }
            }
    );
    result->insert(result->end(), body.begin(), body.end());
    return true;
}

bool plasma::ast::GeneratorExpression::compile(std::vector<vm::instruction> *result,
                                               plasma::error::error *compilationError) {
    if (!this->Source->compile_and_push(true, result, compilationError)) {
        return false;
    }

    std::vector<plasma::vm::instruction> operation;
    std::vector<std::string> receivers;
    for (Identifier *receiver : this->Receivers) {
        receivers.push_back(receiver->Token.string);
    }
    operation.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = receivers
            }
    );

    std::vector<plasma::ast::Expression *> generatorOperation;
    generatorOperation.push_back(this->Operation);
    if (!(new ReturnStatement(generatorOperation))->compile(&operation, compilationError)) {
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

// Statements

bool plasma::ast::AssignStatement::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {
    // Pre-assignment
    if (this->AssignOperator.directValue == plasma::lexer::Assign) { // Basic Assign
        if (!this->RightHandSide->compile_and_push(true, result, compilationError)) {
            return false;
        }
    } else { // AddAssign, SubAssign...
        uint8_t preOperation;
        switch (this->AssignOperator.directValue) {
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
        if (!(new plasma::ast::BinaryExpression(this->LeftHandSide, plasma::lexer::token{
                .directValue = preOperation,
                .kind = plasma::lexer::Operator,
                .line = this->AssignOperator.line
        }, this->RightHandSide))->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    //
    // Determine the assignment target
    switch (this->LeftHandSide->TypeID) {
        case plasma::ast::IdentifierID:
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::AssignIdentifierOP,
                            .value = dynamic_cast<plasma::ast::Identifier *>(this->LeftHandSide)->Token.string
                    }
            );
            break;
        case plasma::ast::SelectorID:
            // Push owner
            if (!dynamic_cast<plasma::ast::SelectorExpression *>(this->LeftHandSide)->X->compile_and_push(true,
                                                                                                              result,
                                                                                                              compilationError)) {
                return false;
            }
            //
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::AssignSelectorOP,
                            .value = std::any_cast<plasma::ast::SelectorExpression>(
                                    this->LeftHandSide).Identifier->Token.string
                    }
            );
            break;
        case plasma::ast::IndexID:
            // Push source
            if (!dynamic_cast<IndexExpression *>(this->LeftHandSide)->Source->compile_and_push(true,
                                                                                                   result,
                                                                                                   compilationError)) {
                return false;
            }
            // Push Index
            if (!dynamic_cast<IndexExpression *>(this->LeftHandSide)->Index->compile_and_push(true,
                                                                                                  result,
                                                                                                  compilationError)) {
                return false;
            }
            //
            result->push_back(
                    plasma::vm::instruction{
                            .op_code = plasma::vm::AssignIndexOP,
                    }
            );
            break;
        default:
            // ToDo: Throw error
            break;
    }
    return true;
}

bool plasma::ast::FunctionDefinitionStatement::compile(std::vector<vm::instruction> *result,
                                                       plasma::error::error *compilationError) {
    // ImplementMe:
    std::vector<std::string> arguments;
    for (Identifier *argument : this->Arguments) {
        arguments.push_back(argument->Token.string);
    }

    std::vector<plasma::vm::instruction> body;

    body.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = arguments
            }
    );

    if (!compile_body(this->Body, &body, compilationError)) {
        return false;
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewFunctionOP,
                    .value = plasma::vm::function_information{
                            .name = this->Name->Token.string,
                            .bodyLength = body.size(),
                            .numberOfArguments = arguments.size()
                    }
            }
    );

    result->insert(result->end(), body.begin(), body.end());

    return true;
}

static bool
compile_class_function_definition(plasma::ast::FunctionDefinitionStatement *functionDefinitionStatement,
                                  std::vector<plasma::vm::instruction> *result,
                                  plasma::error::error *compilationError) {
    // ImplementMe:
    std::vector<std::string> arguments;
    for (plasma::ast::Identifier *argument : functionDefinitionStatement->Arguments) {
        arguments.push_back(argument->Token.string);
    }

    std::vector<plasma::vm::instruction> body;

    body.push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::LoadFunctionArgumentsOP,
                    .value = arguments
            }
    );

    if (!compile_body(functionDefinitionStatement->Body, &body, compilationError)) {
        return false;
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewClassFunctionOP,
                    .value = plasma::vm::function_information{
                            .name = functionDefinitionStatement->Name->Token.string,
                            .bodyLength = body.size(),
                            .numberOfArguments = arguments.size()
                    }
            }
    );

    result->insert(result->end(), body.begin(), body.end());

    return true;
}

bool plasma::ast::ClassStatement::compile(std::vector<vm::instruction> *result,
                                          plasma::error::error *compilationError) {
    for (Expression *base : this->Bases) {
        if (!base->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    std::vector<plasma::vm::instruction> body;
    if (!compile_class_body(this->Body, &body, compilationError)) {
        return false;
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewClassOP,
                    .value = plasma::vm::class_information{
                            .name = this->Name->Token.string,
                            .bodyLength = body.size(),
                            .numberOfBases = this->Bases.size()
                    }
            }
    );

    result->insert(result->end(), body.begin(), body.end());

    return true;
}

bool plasma::ast::ReturnStatement::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {
    for (auto returnValue = this->Results.rbegin();
         returnValue != this->Results.rend();
         returnValue++) {
        if (!(*returnValue)->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::ReturnOP,
                    .value = this->Results.size()
            }
    );
    return true;
}

struct elif_information {
    std::vector<plasma::vm::instruction> condition;
    std::vector<plasma::vm::instruction> body;
};

bool plasma::ast::IfStatement::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {

}

bool plasma::ast::UnlessStatement::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {

}

bool plasma::ast::SwitchStatement::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {

}

bool plasma::ast::WhileStatement::compile(std::vector<vm::instruction> *result,
                                          plasma::error::error *compilationError) {
}

bool plasma::ast::UntilStatement::compile(std::vector<vm::instruction> *result,
                                          plasma::error::error *compilationError) {

}

bool plasma::ast::DoWhileStatement::compile(std::vector<vm::instruction> *result,
                                            plasma::error::error *compilationError) {

}

bool plasma::ast::RedoStatement::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::RedoOP,
            }
    );
    return true;
}

bool plasma::ast::ContinueStatement::compile(std::vector<vm::instruction> *result,
                                             plasma::error::error *compilationError) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::ContinueOP,
            }
    );
    return true;
}

bool plasma::ast::BreakStatement::compile(std::vector<vm::instruction> *result,
                                          plasma::error::error *compilationError) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::BreakOP,
            }
    );
    return true;
}

bool plasma::ast::PassStatement::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NOP,
            }
    );
    return true;
}

bool plasma::ast::RaiseStatement::compile(std::vector<vm::instruction> *result,
                                          plasma::error::error *compilationError) {
    if (!this->X->compile_and_push(true, result, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::RaiseOP
            }
    );
    return true;
}

bool plasma::ast::ModuleStatement::compile(std::vector<vm::instruction> *result,
                                           plasma::error::error *compilationError) {
    std::vector<plasma::vm::instruction> body;
    if (!compile_body(this->Body, &body, compilationError)) {
        return false;
    }
    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewModuleOP,
                    .value = plasma::vm::class_information{
                            .name = this->Name->Token.string,
                            .bodyLength = body.size()
                    }
            }
    );
    result->insert(result->end(), body.begin(), body.end());
    return true;
}

bool plasma::ast::InterfaceStatement::compile(std::vector<vm::instruction> *result,
                                              plasma::error::error *compilationError) {
    for (Expression *base : this->Bases) {
        if (!base->compile_and_push(true, result, compilationError)) {
            return false;
        }
    }
    std::vector<plasma::vm::instruction> body;
    for (FunctionDefinitionStatement *function : this->MethodDefinitions) {
        if (!compile_class_function_definition(function, &body, compilationError)) {
            return false;
        }
    }

    result->push_back(
            plasma::vm::instruction{
                    .op_code = plasma::vm::NewInterfaceOP,
                    .value = plasma::vm::class_information{
                            .name = this->Name->Token.string,
                            .bodyLength = body.size(),
                            .numberOfBases = this->Bases.size()
                    }
            }
    );
    result->insert(result->end(), body.begin(), body.end());
    return true;
}

bool plasma::ast::ForStatement::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {

}

struct except_block {
    std::string captureName;
    std::vector<plasma::vm::instruction> targets;
    std::vector<plasma::vm::instruction> body;
};

bool plasma::ast::TryStatement::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {

}

bool
plasma::ast::BeginStatement::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {
}

bool
plasma::ast::EndStatement::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {
}

bool
plasma::ast::Program::compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) {
    if (this->Begin != nullptr) {
        if (!this->Begin->compile(result, compilationError)) {
            return false;
        }
    }

    for (Node *node : this->Body) {
        if (!node->compile(result, compilationError)) {
            return false;
        }
    }

    if (this->End != nullptr) {
        if (!this->End->compile(result, compilationError)) {
            return false;
        }
    }
    return true;
}