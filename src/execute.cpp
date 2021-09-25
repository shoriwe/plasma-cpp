#include "compiler/lexer.h"
#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::new_tuple_op(context *c, size_t numberOfElements) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    std::vector<value *> elements;
    elements.reserve(numberOfElements);
    for (size_t index = 0; index < numberOfElements; index++) {
        if (c->value_stack.empty()) {
            return this->new_invalid_number_of_arguments_error(c, numberOfElements, index + 1);
        }
        elements.push_back(c->pop_value());
        c->protect_value(elements.back());
    }
    c->lastObject = this->new_tuple(c, false, elements);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_array_op(context *c, size_t numberOfElements) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    std::vector<value *> elements;
    elements.reserve(numberOfElements);

    for (size_t index = 0; index < numberOfElements; index++) {
        if (c->value_stack.empty()) {
            return this->new_invalid_number_of_arguments_error(c, numberOfElements, index + 1);
        }
        elements.push_back(c->pop_value());
        c->protect_value(elements.back());
    }
    c->lastObject = this->new_array(c, false, elements);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_hash_op(context *c, size_t numberOfElements) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    std::unordered_map<value *, value *> elements;
    elements.reserve(numberOfElements);

    for (size_t index = 0; index < numberOfElements; index++) {
        value *key = c->pop_value();
        value *v = c->pop_value();
        elements[key] = v;
        c->protect_value(key);
        c->protect_value(v);
    }

    c->lastObject = this->new_hash_table(c, false);
    for (const auto &kValue : elements) {
        value *addError = c->lastObject->add_key_value(c, this, kValue.first, kValue.second);
        if (addError != nullptr) {
            return addError;
        }
    }
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_string_op(context *c, const std::string &string) {
    c->lastObject = this->new_string(c, false, string);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_bytes_op(context *c, const std::string &bytes) {
    std::vector<uint8_t> bytesVector;
    bytesVector.reserve(bytes.size());
    bytesVector.insert(bytesVector.end(), bytes.begin(), bytes.end());
    c->lastObject = this->new_bytes(c, false, bytesVector);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_integer_op(context *c, int64_t integer) {
    c->lastObject = this->new_integer(c, false, integer);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_float_op(context *c, double floating) {
    c->lastObject = this->new_float(c, false, floating);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::unary_op(context *c, uint8_t instruction) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    std::string operationName;
    switch (instruction) {
        case NegateBitsOP:
            operationName = NegBits;
            break;
        case BoolNegateOP:
            operationName = Negate;
            break;
        case NegativeOP:
            operationName = Negative;
            break;
            //case PositiveOP: // Fixme: Complete this
            //    operationName = plasma::vm::Positive;
            //    break;
        default:
            // Fixme:
            break;
    }
    bool found = false;
    value *target = c->pop_value();
    c->protect_value(target);
    value *operation = target->get(c, this, operationName, &found);
    if (!found) {
        return this->new_object_with_name_not_found_error(c, target, operationName);
    }
    c->protect_value(operation);
    bool success = false;
    value *result = this->call_function(c, operation, std::vector<value *>(), &success);
    if (success) {
        c->lastObject = result;
        return nullptr;
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::binary_op(context *c, uint8_t instruction) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    std::string leftHandSideFunction, rightHandSideFunction;
    switch (instruction) {
        case AddOP:
            leftHandSideFunction = Add;
            rightHandSideFunction = RightAdd;
            break;
        case SubOP:
            leftHandSideFunction = Sub;
            rightHandSideFunction = RightSub;
            break;
        case MulOP:
            leftHandSideFunction = Mul;
            rightHandSideFunction = RightMul;
            break;
        case DivOP:
            leftHandSideFunction = Div;
            rightHandSideFunction = RightDiv;
            break;
        case FloorDivOP:
            leftHandSideFunction = FloorDiv;
            rightHandSideFunction = RightFloorDiv;
            break;
        case ModOP:
            leftHandSideFunction = Mod;
            rightHandSideFunction = RightMod;
            break;
        case PowOP:
            leftHandSideFunction = Pow;
            rightHandSideFunction = RightPow;
            break;
        case BitXorOP:
            leftHandSideFunction = BitXor;
            rightHandSideFunction = RightBitXor;
            break;
        case BitAndOP:
            leftHandSideFunction = BitAnd;
            rightHandSideFunction = RightBitAnd;
            break;
        case BitOrOP:
            leftHandSideFunction = BitOr;
            rightHandSideFunction = RightBitOr;
            break;
        case BitLeftOP:
            leftHandSideFunction = BitLeft;
            rightHandSideFunction = RightBitLeft;
            break;
        case BitRightOP:
            leftHandSideFunction = BitRight;
            rightHandSideFunction = RightBitRight;
            break;
        case AndOP:
            leftHandSideFunction = And;
            rightHandSideFunction = RightAnd;
            break;
        case OrOP:
            leftHandSideFunction = Or;
            rightHandSideFunction = RightOr;
            break;
        case XorOP:
            leftHandSideFunction = Xor;
            rightHandSideFunction = RightXor;
            break;
        case EqualsOP:
            leftHandSideFunction = Equals;
            rightHandSideFunction = RightEquals;
            break;
        case NotEqualsOP:
            leftHandSideFunction = NotEquals;
            rightHandSideFunction = RightNotEquals;
            break;
        case GreaterThanOP:
            leftHandSideFunction = GreaterThan;
            rightHandSideFunction = RightGreaterThan;
            break;
        case LessThanOP:
            leftHandSideFunction = LessThan;
            rightHandSideFunction = RightLessThan;
            break;
        case GreaterThanOrEqualOP:
            leftHandSideFunction = GreaterThanOrEqual;
            rightHandSideFunction = RightGreaterThanOrEqual;
            break;
        case LessThanOrEqualOP:
            leftHandSideFunction = LessThanOrEqual;
            rightHandSideFunction = RightLessThanOrEqual;
            break;
        case ContainsOP:
            leftHandSideFunction = RightContains;
            rightHandSideFunction = Contains;
            break;
        default:
            // Fixme
            throw std::exception("OP NOT IMPLEMENTED");
            break;
    }
    auto leftHandSide = c->pop_value();
    auto rightHandSide = c->pop_value();
    bool found = false;
    bool success = false;
    c->protect_value(leftHandSide);
    c->protect_value(rightHandSide);
    value *result;
    value *operation = leftHandSide->get(c, this, leftHandSideFunction, &found);
    c->protect_value(operation);
    if (found) {
        result = this->call_function(c, operation, std::vector<value *>{rightHandSide}, &success);
        if (success) {
            c->lastObject = result;
            return nullptr;
        }
    }
    // Try the right hand side
    found = false;
    success = false;
    operation = rightHandSide->get(c, this, rightHandSideFunction, &found);
    c->protect_value(operation);
    if (!found) {
        return this->new_object_with_name_not_found_error(c, rightHandSide, rightHandSideFunction);
    }
    result = this->call_function(c, operation, std::vector<value *>{leftHandSide}, &success);
    if (success) {
        c->lastObject = result;
        return nullptr;
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::select_name_from_object_op(context *c, const std::string &identifier) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    value *object = c->pop_value();
    c->protect_value(object);

    bool found = false;
    value *result = object->get(c, this, identifier, &found);
    if (!found) {
        return result;
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::get_identifier_op(context *c, const std::string &identifier) {

    value *result = c->peek_symbol_table()->get_any(identifier);
    if (result == nullptr) {
        return this->new_object_with_name_not_found_error(c, identifier);
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::index_op(context *c) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    value *index = c->pop_value();
    value *source = c->pop_value();
    c->protect_value(index);
    c->protect_value(source);
    bool success = false;
    value *indexFunc = source->get(c, this, Index, &success);
    if (!success) {
        return indexFunc;
    }
    c->protect_value(indexFunc);
    success = false;
    value *result = this->call_function(c, indexFunc, std::vector<value *>{index}, &success);
    if (!success) {
        return result;
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::assign_identifier_op(context *c, const std::string &symbol) {
    c->peek_symbol_table()->set(symbol, c->pop_value());
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::assign_selector_op(context *c, const std::string &symbol) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto receiver = c->pop_value();
    c->protect_value(receiver);
    receiver->set(symbol, c->pop_value());
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::assign_index_op(context *c) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto index = c->pop_value();
    auto receiver = c->pop_value();
    auto element = c->pop_value();

    c->protect_value(index);
    c->protect_value(receiver);
    c->protect_value(element);

    bool found = false;

    auto assignFunc = receiver->get(c, this, Assign, &found);
    c->protect_value(assignFunc);
    if (!found) {
        return assignFunc;
    }
    bool success = false;
    auto result = this->call_function(c, assignFunc, std::vector<value *>{index, element}, &success);
    c->protect_value(result);
    if (!success) {
        return result;
    }
    return nullptr;

}

plasma::vm::value *plasma::vm::virtual_machine::method_invocation_op(context *c, size_t numberOfArguments) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    value *function = c->pop_value();
    c->protect_value(function);
    std::vector<value *> arguments;
    arguments.reserve(numberOfArguments);

    for (size_t argumentIndex = 0; argumentIndex < numberOfArguments; argumentIndex++) {
        auto argument = c->pop_value();
        arguments.push_back(argument);
        c->protect_value(argument);
    }
    bool success = false;

    value *result = this->call_function(c, function, arguments, &success);
    if (!success) {
        return result;
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::new_class_op(context *c, bytecode *bc, const class_information &classInformation) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    std::vector<value *> bases;
    bases.reserve(classInformation.numberOfBases);

    for (size_t baseIndex = 0; baseIndex < classInformation.numberOfBases; baseIndex++) {
        auto base = c->pop_value();
        bases.push_back(base);
        c->protect_value(base);
    }
    auto classCode = bc->nextN(classInformation.bodyLength);
    c->peek_symbol_table()->set(classInformation.name,
                                this->new_type(c, false, classInformation.name, bases,
                                               constructor{
                                                       .isBuiltIn = false,
                                                       .code = classCode
                                               }
                                )
    );
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::new_function_op(context *c, bytecode *bc,
                                             const function_information &functionInformation) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto functionInstructions = bc->nextN(functionInformation.bodyLength);
    c->peek_symbol_table()->set(
            functionInformation.name,
            this->new_function(
                    c,
                    false,
                    nullptr,
                    new_plasma_callable(
                            functionInformation.numberOfArguments,
                            functionInstructions
                    )
            )
    );
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::new_class_function_op(context *c, bytecode *bc,
                                                   const function_information &functionInformation) {


    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto self = c->peek_value();

    c->protect_value(self);

    auto functionInstructions = bc->nextN(functionInformation.bodyLength);
    self->set(
            functionInformation.name,
            this->new_function(
                    c,
                    false,
                    self,
                    new_plasma_callable(
                            functionInformation.numberOfArguments,
                            functionInstructions
                    )
            )
    );
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::load_function_arguments_op(context *c, const std::vector<std::string> &arguments) {

    for (const auto &argument : arguments) {
        c->peek_symbol_table()->set(argument, c->pop_value());
    }

    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::return_op(context *c, size_t numberOfReturnValues) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });
    c->lastState = Return;
    if (numberOfReturnValues == 0) {
        return this->get_none(c);
    } else if (numberOfReturnValues == 1) {
        return c->pop_value();
    }
    std::vector<value *> content;
    content.reserve(numberOfReturnValues);
    for (size_t index = 0; index < numberOfReturnValues; index++) {
        auto returnValue = c->pop_value();
        content.push_back(returnValue);
        c->protect_value(returnValue);
    }
    auto result = this->new_tuple(c, false, content);
    return result;
}

plasma::vm::value *
plasma::vm::virtual_machine::if_op(context *c, const condition_information& conditionInformation) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto conditionBytecode = bytecode{
            .instructions = conditionInformation.condition,
            .index = 0
    };
    bool success = false;
    auto condition = this->execute(c, &conditionBytecode, &success);
    if (!success) {
        return condition;
    }
    c->protect_value(condition);
    bool isTrue = false;
    auto interpretationError = this->interpret_as_boolean(c, condition, &isTrue);
    if (interpretationError != nullptr) {
        return interpretationError;
    }
    success = false;
    value *result;
    if (isTrue) {
        auto body = bytecode{
                .instructions = conditionInformation.body,
                .index = 0
        };
        result = this->execute(c, &body, &success);
    } else {
        auto elseBody = bytecode{
                .instructions = conditionInformation.body,
                .index = 0
        };
        result = this->execute(c, &elseBody, &success);
    }
    if (!success) {
        return result;
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::unless_op(context *c, const condition_information& conditionInformation) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto bc = bytecode{
            .instructions = conditionInformation.condition,
            .index = 0
    };
    bool success = false;
    auto condition = this->execute(c, &bc, &success);
    if (!success) {
        return condition;
    }
    c->protect_value(condition);
    bool isTrue = false;
    auto interpretationError = this->interpret_as_boolean(c, condition, &isTrue);
    if (interpretationError != nullptr) {
        return interpretationError;
    }

    success = false;
    value *result;
    if (!isTrue) {
        auto body = bytecode{
                .instructions = conditionInformation.body,
                .index = 0
        };
        result = this->execute(c, &body, &success);
    } else {
        auto elseBody = bytecode{
                .instructions = conditionInformation.body,
                .index = 0
        };
        result = this->execute(c, &elseBody, &success);
    }
    if (!success) {
        return result;
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::if_one_liner_op(context *c, const condition_information& conditionInformation) {
    bool asBool = false;
    auto interpretationError = this->interpret_as_boolean(c, c->pop_value(), &asBool);
    if (interpretationError != nullptr) {
        return interpretationError;
    }
    bytecode toExecute{
            .index = 0
    };
    if (asBool) {
        toExecute.instructions = conditionInformation.body;
    } else {
        toExecute.instructions = conditionInformation.elseBody;
    }
    bool success;
    auto result = this->execute(c, &toExecute, &success);
    if (!success) {
        return result;
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *
plasma::vm::virtual_machine::unless_one_liner_op(context *c, const condition_information& conditionInformation) {
    bool asBool = false;
    auto interpretationError = this->interpret_as_boolean(c, c->pop_value(), &asBool);
    if (interpretationError != nullptr) {
        return interpretationError;
    }
    bytecode toExecute{
            .index = 0
    };
    if (!asBool) {
        toExecute.instructions = conditionInformation.body;
    } else {
        toExecute.instructions = conditionInformation.elseBody;
    }
    bool success;
    auto result = this->execute(c, &toExecute, &success);
    if (!success) {
        return result;
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_lambda_function_op(context *c, bytecode *bc,
                                                                       const function_information &functionInformation) {
    auto functionInstructions = bc->nextN(functionInformation.bodyLength);
    c->lastObject = this->new_function(
            c,
            false,
            nullptr,
            new_plasma_callable(
                    functionInformation.numberOfArguments,
                    functionInstructions
            )
    );
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::new_generator_op(context *c, bytecode *bc,
                                                                 const generator_information &generatorInformation) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto operationCode = bc->nextN(generatorInformation.operationLength);

    auto result = new_iterator(c, false);
    c->protect_value(result);

    auto rawSource = c->pop_value();
    c->protect_value(rawSource);

    bool interpretationSuccess = false;
    auto source = this->interpret_as_iterator(c, rawSource, &interpretationSuccess);
    if (!interpretationSuccess) {
        return source;
    }
    c->protect_value(source);

    bool nameFound = false;
    auto hasNext = source->get(c, this, HasNext, &nameFound);
    if (!nameFound) {
        return hasNext;
    }
    c->protect_value(hasNext);

    auto next = source->get(c, this, Next, &nameFound);
    if (!nameFound) {
        return next;
    }
    c->protect_value(next);

    result->source = source;

    auto operationFunction = this->new_function(
            c,
            false,
            result,
            new_plasma_callable(
                    generatorInformation.numberOfReceivers,
                    operationCode
            )
    );
    c->protect_value(operationFunction);

    result->set(
            "Operation",
            operationFunction
    );


    result->set(
            HasNext,
            this->new_function(
                    c,
                    false,
                    result,
                    new_builtin_callable(
                            0,
                            [c, this, hasNext](value *self, const std::vector<value *> &arguments,
                                               bool *success) -> value * {
                                return this->call_function(c, hasNext, std::vector<value *>{}, success);
                            }
                    )
            )
    );

    result->set(
            Next,
            this->new_function(
                    c,
                    false,
                    result,
                    new_builtin_callable(
                            0,
                            [c, this, next, operationFunction, generatorInformation](value *self,
                                                                                     const std::vector<value *> &arguments,
                                                                                     bool *success) -> value * {
                                bool callSuccess;
                                // Receive the next value
                                auto nextValue = this->call_function(c, next, std::vector<value *>{}, &callSuccess);
                                if (!callSuccess) {
                                    (*success) = false;
                                    return nextValue;
                                }

                                // If is only one receiver pass it directly to the operation function
                                if (generatorInformation.numberOfReceivers == 1) {
                                    return this->call_function(c, operationFunction, std::vector<value *>{nextValue},
                                                               success);
                                }
                                // Unpack the values
                                std::vector<value *> operationArguments;
                                auto unpackError = this->unpack_values(c, nextValue,
                                                                       generatorInformation.numberOfReceivers,
                                                                       &operationArguments);
                                if (unpackError != nullptr) {
                                    (*success) = false;
                                    return unpackError;
                                }
                                // Pass the arguments to the operation function
                                return this->call_function(c, operationFunction, operationArguments, success);
                            }
                    )
            )
    );

    c->lastObject = result;

    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::raise_op(context *c) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto raisedError = c->pop_value();
    c->protect_value(raisedError);

    if (!raisedError->implements(c, this, this->force_any_from_master(c, RuntimeError))) {
        // Raise that the output is not and RuntimeError
        return this->new_invalid_type_error(c, raisedError->get_type(c, this), std::vector<std::string>{RuntimeError});
    }
    return raisedError;
}

plasma::vm::value *plasma::vm::virtual_machine::new_module_op(context *c, bytecode *bc,
                                                              const class_information &moduleInformation) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto moduleBody = bc->nextN(moduleInformation.bodyLength);
    bytecode moduleCode = {
            .instructions = moduleBody,
            .index = 0
    };

    auto result = this->new_module(c, false);
    c->protect_value(result);

    c->push_symbol_table(result->symbols);

    bool success = false;
    auto executionError = this->execute(c, &moduleCode, &success);
    c->pop_symbol_table();

    if (!success) {
        return executionError;
    }

    c->peek_symbol_table()->set(moduleInformation.name, result);
    return nullptr;
}

struct except_block_code {
    std::vector<plasma::vm::instruction> targets;
    std::vector<plasma::vm::instruction> body;
    std::string captureName;
};

plasma::vm::value *
plasma::vm::virtual_machine::execute_try_block(context *c, bytecode *bc,
                                               const try_block_information &tryBlockInformation) {
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    bool success = false;
    bytecode bodyByteCode{
            .instructions = bc->nextN(tryBlockInformation.bodySize),
            .index = 0
    };
    std::vector<except_block_code> exceptBlocksCode;
    for (const auto &exceptBlock : tryBlockInformation.exceptBlocks) {
        exceptBlocksCode.push_back(
                except_block_code{
                        .targets = bc->nextN(exceptBlock.targetsSize),
                        .body = bc->nextN(exceptBlock.bodySize),
                        .captureName = exceptBlock.captureName
                }
        );
    }
    bytecode finallyCode{
            .instructions = bc->nextN(tryBlockInformation.finallySize),
            .index = 0
    };
    value *finallyExecutionError;
    auto executionError = this->execute(c, &bodyByteCode, &success);
    c->protect_value(executionError);

    if (success) {
        if (executionError->typeId != NoneType) {

            c->push_value(executionError);
            return nullptr;
        }
        // Execute finally block
        if (finallyCode.instructions.empty()) {
            return nullptr;
        }
        success = false;
        finallyExecutionError = this->execute(c, &finallyCode, &success);
        if (!success) {
            return finallyExecutionError;
        }
        if (finallyExecutionError->typeId != NoneType) {
            c->push_value(finallyExecutionError);
        }
        return nullptr;
    }
    // Execute except blocks
    for (const auto &exceptBlock : exceptBlocksCode) {
        success = false;
        bytecode targetsCode{
                .instructions = exceptBlock.targets,
                .index = 0
        };
        auto targets = this->execute(c, &targetsCode, &success);
        if (!success) {
            return targets;
        }
        c->protect_value(targets);

        bool containsErrorResult = false;
        if (targets->content.empty()) {

            containsErrorResult = true;
        } else {

            auto containsError = this->content_contains(
                    c,
                    targets,
                    executionError->get_type(c, this),
                    &containsErrorResult
            );
            if (containsError != nullptr) {

                return containsError;
            }
        }

        if (containsErrorResult) {

            if (!exceptBlock.captureName.empty()) {

                c->peek_symbol_table()->set(exceptBlock.captureName, executionError);
            }
            // Execute body
            bytecode exceptBody{
                    .instructions = exceptBlock.body,
                    .index = 0
            };
            success = false;
            auto exceptExecutionError = this->execute(c, &exceptBody, &success);
            if (!success) {
                return exceptExecutionError;
            }
            c->protect_value(exceptExecutionError);

            if (exceptExecutionError->typeId != NoneType) {

                c->push_value(exceptExecutionError);
                return nullptr;
            }

            // Execute finally

            if (finallyCode.instructions.empty()) {
                return nullptr;
            }
            success = false;
            finallyExecutionError = this->execute(c, &finallyCode, &success);
            c->protect_value(finallyExecutionError);

            if (!success) {
                return finallyExecutionError;
            }
            if (finallyExecutionError->typeId != NoneType) {
                c->push_value(finallyExecutionError);
            }
            return nullptr;
        }
    }

    return executionError;
}

plasma::vm::value *plasma::vm::virtual_machine::execute(context *c, bytecode *bc, bool *success) {
    value *executionError = nullptr;
    value *result = nullptr;
    while (bc->has_next()) {
        c->lastState = NoState;
        instruction instruct = bc->next();
        // 
        switch (instruct.op_code) {
            case NewStringOP:
                executionError = this->new_string_op(c, std::any_cast<std::string>(instruct.value));
                break;
            case NewFloatOP:
                executionError = this->new_float_op(c, std::any_cast<double>(instruct.value));
                break;
            case NewIntegerOP:
                executionError = this->new_integer_op(c, std::any_cast<int64_t>(instruct.value));
                break;
            case NewBytesOP:
                executionError = this->new_bytes_op(c, std::any_cast<std::string>(instruct.value));
                break;
            case GetTrueOP:
                c->lastObject = this->get_true(c);
                break;
            case GetFalseOP:
                c->lastObject = this->get_false(c);
                break;
            case GetNoneOP:
                c->lastObject = this->get_none(c);
                break;
            case NewTupleOP:
                executionError = this->new_tuple_op(c, std::any_cast<size_t>(instruct.value));
                break;
            case NewArrayOP:
                executionError = this->new_array_op(c, std::any_cast<size_t>(instruct.value));
                break;
            case NewHashOP:
                executionError = this->new_hash_op(c, std::any_cast<size_t>(instruct.value));
                break;
            case UnaryOP:
                executionError = this->unary_op(c, std::any_cast<uint8_t>(instruct.value));
                break;
            case BinaryOP:
                executionError = this->binary_op(c, std::any_cast<uint8_t>(instruct.value));
                break;
            case GetIdentifierOP:
                executionError = this->get_identifier_op(c, std::any_cast<std::string>(instruct.value));
                break;
            case SelectNameFromObjectOP:
                executionError = this->select_name_from_object_op(c, std::any_cast<std::string>(instruct.value));
                break;
            case IndexOP:
                executionError = this->index_op(c);
                break;
            case MethodInvocationOP:
                executionError = this->method_invocation_op(c, std::any_cast<size_t>(instruct.value));
                break;
            case AssignIdentifierOP:
                executionError = this->assign_identifier_op(c, std::any_cast<std::string>(instruct.value));
                break;
            case AssignSelectorOP:
                executionError = this->assign_selector_op(c, std::any_cast<std::string>(instruct.value));
                break;
            case AssignIndexOP:
                executionError = this->assign_index_op(c);
                break;
            case NewInterfaceOP:
            case NewClassOP:
                executionError = this->new_class_op(c, bc, std::any_cast<class_information>(instruct.value));
                break;
            case IfOP:
                executionError = this->if_op(c, std::any_cast<condition_information>(instruct.value));
                if (executionError != nullptr) {
                    (*success) = false;
                    return executionError;
                }
                switch (c->lastState) {
                    case Continue:
                    case Break:
                    case Redo:
                        return this->get_none(c);
                    case Return:
                        // Leave alone so it get propagated
                        result = c->lastObject;
                        c->lastObject = nullptr;
                        return c->lastObject;
                }
                break;
            case UnlessOP:
                executionError = this->unless_op(c, std::any_cast<condition_information>(instruct.value));
                if (executionError != nullptr) {
                    (*success) = false;
                    return executionError;
                }
                switch (c->lastState) {
                    case Continue:
                    case Break:
                    case Redo:
                        return this->get_none(c);
                    case Return:
                        // Leave alone so it get propagated
                        result = c->lastObject;
                        c->lastObject = nullptr;
                        return c->lastObject;
                }
                break;
            case IfOneLinerOP:
                executionError = this->if_one_liner_op(c, std::any_cast<condition_information>(instruct.value));
                break;
            case UnlessOneLinerOP:
                executionError = this->unless_one_liner_op(c, std::any_cast<condition_information>(instruct.value));
                break;
            case NewClassFunctionOP:
                executionError = this->new_class_function_op(c, bc,
                                                             std::any_cast<function_information>(instruct.value));
                break;
            case LoadFunctionArgumentsOP:
                executionError = this->load_function_arguments_op(c,
                                                                  std::any_cast<std::vector<std::string>>(
                                                                          instruct.value));
                break;
            case NewFunctionOP:
                executionError = this->new_function_op(c, bc, std::any_cast<function_information>(instruct.value));
                break;
            case NewLambdaFunctionOP:
                executionError = this->new_lambda_function_op(c, bc,
                                                              std::any_cast<function_information>(instruct.value));
                break;
            case NewGeneratorOP:
                executionError = this->new_generator_op(c, bc, std::any_cast<generator_information>(instruct.value));
                break;
            case PushOP:
                if (c->lastObject != nullptr) {
                    c->push_value(c->lastObject);
                }
                break;
            case NOP:
                break;
            case ReturnOP:
                (*success) = true;
                return this->return_op(c, std::any_cast<size_t>(instruct.value));
            case RaiseOP:
                executionError = this->raise_op(c);
                break;
            case NewModuleOP:
                executionError = this->new_module_op(c, bc, std::any_cast<class_information>(instruct.value));
                break;
            case TryOP:
                executionError = this->execute_try_block(c, bc, std::any_cast<try_block_information>(instruct.value));
                break;
            default:
                // FixMe: Do something when
                throw std::exception("OP NOT IMPLEMENTED");
                break;
        }
        if (executionError != nullptr) {
            // Handle the error
            // Return the error
            (*success) = false;
            return executionError;
        }
    }
    (*success) = true;
    return this->get_none(c);
}
