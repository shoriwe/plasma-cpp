#include "compiler/lexer.h"
#include "vm/virtual_machine.h"


plasma::vm::value *plasma::vm::virtual_machine::newTupleOP(context *c, size_t numberOfElements) {
    std::vector<value *> elements;
    elements.reserve(numberOfElements);
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });
    for (size_t index = 0; index < numberOfElements; index++) {
        if (c->value_stack.empty()) {
            return this->NewInvalidNumberOfArgumentsError(c, numberOfElements, index + 1);
        }
        elements.push_back(c->pop_value());
        c->protect_value(elements.back());
    }
    c->lastObject = this->new_tuple(c, false, elements);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::newArrayOP(context *c, size_t numberOfElements) {
    std::vector<value *> elements;
    elements.reserve(numberOfElements);
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });
    for (size_t index = 0; index < numberOfElements; index++) {
        if (c->value_stack.empty()) {
            return this->NewInvalidNumberOfArgumentsError(c, numberOfElements, index + 1);
        }
        elements.push_back(c->pop_value());
        c->protect_value(elements.back());
    }
    c->lastObject = this->new_array(c, false, elements);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::newHashOP(context *c, size_t numberOfElements) {
    std::unordered_map<value *, value *> elements;
    elements.reserve(numberOfElements);
    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });
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

plasma::vm::value *plasma::vm::virtual_machine::newStringOP(context *c, const std::string &string) {
    c->lastObject = this->new_string(c, false, string);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::newBytesOP(context *c, const std::string &bytes) {
    std::vector<uint8_t> bytesVector;
    bytesVector.reserve(bytes.size());
    bytesVector.insert(bytesVector.end(), bytes.begin(), bytes.end());
    c->lastObject = this->new_bytes(c, false, bytesVector);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::newIntegerOP(context *c, int64_t integer) {
    c->lastObject = this->new_integer(c, false, integer);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::newFloatOP(context *c, double floating) {
    c->lastObject = this->new_float(c, false, floating);
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::unaryOP(context *c, uint8_t instruction) {
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
        return this->NewObjectWithNameNotFoundError(c, target, operationName);
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

plasma::vm::value *plasma::vm::virtual_machine::binaryOP(context *c, uint8_t instruction) {
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
        return this->NewObjectWithNameNotFoundError(c, rightHandSide, rightHandSideFunction);
    }
    result = this->call_function(c, operation, std::vector<value *>{leftHandSide}, &success);
    if (success) {
        c->lastObject = result;
        return nullptr;
    }
    return result;
}

plasma::vm::value *plasma::vm::virtual_machine::selectNameFromObjectOP(context *c, const std::string &identifier) {
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

plasma::vm::value *plasma::vm::virtual_machine::getIdentifierOP(context *c, const std::string &identifier) {

    value *result = c->peek_symbol_table()->get_any(identifier);
    if (result == nullptr) {
        return this->NewObjectWithNameNotFoundError(c, identifier);
    }
    c->lastObject = result;
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::indexOP(context *c) {
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

plasma::vm::value *plasma::vm::virtual_machine::assignIdentifierOP(context *c, const std::string &symbol) {
    c->peek_symbol_table()->set(symbol, c->pop_value());
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::assignSelectorOP(context *c, const std::string &symbol) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto receiver = c->pop_value();
    c->protect_value(receiver);
    receiver->set(symbol, c->pop_value());
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::assignIndexOP(context *c) {

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

plasma::vm::value *plasma::vm::virtual_machine::methodInvocationOP(context *c, size_t numberOfArguments) {
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
plasma::vm::virtual_machine::newClassOP(context *c, bytecode *bc, const ClassInformation &classInformation) {

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
plasma::vm::virtual_machine::newFunctionOP(context *c, bytecode *bc,
                                           const FunctionInformation &functionInformation) {

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
plasma::vm::virtual_machine::newClassFunctionOP(context *c, bytecode *bc,
                                                const FunctionInformation &functionInformation) {


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
plasma::vm::virtual_machine::loadFunctionArgumentsOP(context *c, const std::vector<std::string> &arguments) {

    for (const auto &argument : arguments) {
        c->peek_symbol_table()->set(argument, c->pop_value());
    }

    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::returnOP(context *c, size_t numberOfReturnValues) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

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

plasma::vm::value *plasma::vm::virtual_machine::ifJumpOP(context *c, bytecode *bc, size_t jump) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto condition = c->pop_value();

    c->protect_value(condition);

    bool result = false;
    auto interpretationError = interpret_as_boolean(c, condition, &result);
    if (interpretationError != nullptr) {
        return interpretationError;
    }
    if (!result) {
        bc->jump(jump);
    }
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::rIfJumpOP(context *c, bytecode *bc, size_t jump) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto condition = c->pop_value();

    c->protect_value(condition);

    bool result = false;
    auto interpretationError = interpret_as_boolean(c, condition, &result);
    if (interpretationError != nullptr) {
        return interpretationError;
    }
    if (!result) {
        bc->rjump(jump);
    }
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::unlessJumpOP(context *c, bytecode *bc, size_t jump) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto condition = c->pop_value();

    c->protect_value(condition);

    bool result = false;
    auto interpretationError = interpret_as_boolean(c, condition, &result);
    if (interpretationError != nullptr) {
        return interpretationError;
    }
    if (result) {
        bc->jump(jump);
    }
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::rUnlessJumpOP(context *c, bytecode *bc, size_t jump) {

    auto state = c->protected_values_state();
    defer _(nullptr, [c, state](...) { c->restore_protected_state(state); });

    auto condition = c->pop_value();
    c->protect_value(condition);

    bool result = false;
    auto interpretationError = interpret_as_boolean(c, condition, &result);
    if (interpretationError != nullptr) {
        return interpretationError;
    }
    if (result) {
        bc->rjump(jump);
    }
    return nullptr;
}

plasma::vm::value *plasma::vm::virtual_machine::execute(context *c, bytecode *bc, bool *success) {
    value *executionError = nullptr;
    while (bc->has_next()) {
        instruction instruct = bc->next();
        // std::cout << std::to_string(instruct.op_code) << std::endl;
        switch (instruct.op_code) {
            case NewStringOP:
                executionError = this->newStringOP(c, std::any_cast<std::string>(instruct.value));
                break;
            case NewFloatOP:
                executionError = this->newFloatOP(c, std::any_cast<double>(instruct.value));
                break;
            case NewIntegerOP:
                executionError = this->newIntegerOP(c, std::any_cast<int64_t>(instruct.value));
                break;
            case NewBytesOP:
                executionError = this->newBytesOP(c, std::any_cast<std::string>(instruct.value));
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
                executionError = this->newTupleOP(c, std::any_cast<size_t>(instruct.value));
                break;
            case NewArrayOP:
                executionError = this->newArrayOP(c, std::any_cast<size_t>(instruct.value));
                break;
            case NewHashOP:
                executionError = this->newHashOP(c, std::any_cast<size_t>(instruct.value));
                break;
            case UnaryOP:
                executionError = this->unaryOP(c, std::any_cast<uint8_t>(instruct.value));
                break;
            case BinaryOP:
                executionError = this->binaryOP(c, std::any_cast<uint8_t>(instruct.value));
                break;
            case GetIdentifierOP:
                executionError = this->getIdentifierOP(c, std::any_cast<std::string>(instruct.value));
                break;
            case SelectNameFromObjectOP:
                executionError = this->selectNameFromObjectOP(c, std::any_cast<std::string>(instruct.value));
                break;
            case IndexOP:
                executionError = this->indexOP(c);
                break;
            case MethodInvocationOP:
                executionError = this->methodInvocationOP(c, std::any_cast<size_t>(instruct.value));
                break;
            case JumpOP:
                bc->jump(std::any_cast<size_t>(instruct.value));
                break;
            case RJumpOP:
                bc->rjump(std::any_cast<size_t>(instruct.value));
                break;
            case AssignIdentifierOP:
                executionError = this->assignIdentifierOP(c, std::any_cast<std::string>(instruct.value));
                break;
            case AssignSelectorOP:
                executionError = this->assignSelectorOP(c, std::any_cast<std::string>(instruct.value));
                break;
            case AssignIndexOP:
                executionError = this->assignIndexOP(c);
                break;
            case NewClassOP:
                executionError = this->newClassOP(c, bc, std::any_cast<ClassInformation>(instruct.value));
                break;
            case NewClassFunctionOP:
                executionError = this->newClassFunctionOP(c, bc, std::any_cast<FunctionInformation>(instruct.value));
                break;
            case LoadFunctionArgumentsOP:
                executionError = this->loadFunctionArgumentsOP(c,
                                                               std::any_cast<std::vector<std::string>>(instruct.value));
                break;
            case NewFunctionOP:
                executionError = this->newFunctionOP(c, bc, std::any_cast<FunctionInformation>(instruct.value));
                break;
            case PushOP:
                if (c->lastObject != nullptr) {
                    c->push_value(c->lastObject);
                }
                break;
            case IfJumpOP:
                executionError = this->ifJumpOP(c, bc, std::any_cast<size_t>(instruct.value));
                break;
            case UnlessJumpOP:
                executionError = this->unlessJumpOP(c, bc, std::any_cast<size_t>(instruct.value));
                break;
            case RIfJumpOP:
                executionError = this->rIfJumpOP(c, bc, std::any_cast<size_t>(instruct.value));
                break;
            case RUnlessJumpOP:
                executionError = this->rUnlessJumpOP(c, bc, std::any_cast<size_t>(instruct.value));
                break;
            case NOP:
                break;
            case ReturnOP:
                (*success) = true;
                return this->returnOP(c, std::any_cast<size_t>(instruct.value));
            default:
                // FixMe: Do something when
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
