#include "compiler/ast.h"

plasma::ast::Node *plasma::ast::ArrayExpression::copy() {
    std::vector<Expression *> newContent;
    newContent.reserve(this->Values.size());
    for (Expression *expression : this->Values) {
        newContent.push_back(dynamic_cast<Expression *>(expression->copy()));
    }
    return new ArrayExpression(newContent);
}

plasma::ast::Node *plasma::ast::TupleExpression::copy() {
    std::vector<Expression *> newContent;
    newContent.reserve(this->Values.size());
    for (Expression *expression : this->Values) {
        newContent.push_back(dynamic_cast<Expression *>(expression->copy()));
    }
    return new TupleExpression(newContent);
}

plasma::ast::Node *plasma::ast::HashExpression::copy() {
    std::vector<KeyValue *> newContent;
    newContent.reserve(this->KeyValues.size());
    for (KeyValue *keyValue : this->KeyValues) {
        newContent.push_back(
                new KeyValue(
                        dynamic_cast<Expression *>(keyValue->Key->copy()),
                        dynamic_cast<Expression *>(keyValue->Value->copy())
                )
        );
    }
    return new HashExpression(newContent);
}

plasma::ast::Node *plasma::ast::Identifier::copy() {
    return new Identifier(this->Token);
}

plasma::ast::Node *plasma::ast::BasicLiteralExpression::copy() {
    return new BasicLiteralExpression(this->Token);
}

plasma::ast::Node *plasma::ast::BinaryExpression::copy() {
    return new BinaryExpression(
            dynamic_cast<Expression *>(this->LeftHandSide->copy()),
            this->Operator,
            dynamic_cast<Expression *>(this->RightHandSide->copy())
    );
}

plasma::ast::Node *plasma::ast::UnaryExpression::copy() {
    return new UnaryExpression(
            this->Operator,
            dynamic_cast<Expression *>(this->X->copy())
    );
}

plasma::ast::Node *plasma::ast::ReturnStatement::copy() {
    std::vector<Expression *> newResults;
    newResults.reserve(this->Results.size());
    for (Expression *result: this->Results) {
        newResults.push_back(dynamic_cast<Expression *>(result->copy()));
    }
    return new ReturnStatement(newResults);
}

plasma::ast::Node *plasma::ast::LambdaExpression::copy() {
    std::vector<Identifier *> newArguments;
    newArguments.reserve(this->Arguments.size());
    for (Identifier *argument: this->Arguments) {
        newArguments.push_back(dynamic_cast<Identifier *>(argument->copy()));
    }
    return new LambdaExpression(newArguments, dynamic_cast<ReturnStatement *>(this->Output->copy()));
}

plasma::ast::Node *plasma::ast::GeneratorExpression::copy() {
    std::vector<Identifier *> newReceivers;
    newReceivers.reserve(this->Receivers.size());
    for (Identifier *argument: this->Receivers) {
        newReceivers.push_back(dynamic_cast<Identifier *>(argument->copy()));
    }
    return new GeneratorExpression(
            dynamic_cast<Expression *>(this->Operation->copy()),
            newReceivers,
            dynamic_cast<Expression *>(this->Source->copy())
    );
}

plasma::ast::Node *plasma::ast::SelectorExpression::copy() {
    return new SelectorExpression(dynamic_cast<Expression *>(this->X->copy()),
                                  dynamic_cast<ast::Identifier *>(this->Identifier->copy()));
}

plasma::ast::Node *plasma::ast::MethodInvocationExpression::copy() {
    std::vector<Expression *> newArguments;
    newArguments.reserve(this->Arguments.size());
    for (Expression *argument: this->Arguments) {
        newArguments.push_back(dynamic_cast<Expression *>(argument->copy()));
    }
    return new MethodInvocationExpression(dynamic_cast<Expression *>(this->Function->copy()), newArguments);
}

plasma::ast::Node *plasma::ast::IndexExpression::copy() {
    return new IndexExpression(dynamic_cast<Expression *>(this->Source->copy()),
                               dynamic_cast<Expression *>(this->Index->copy()));
}

plasma::ast::Node *plasma::ast::IfOneLinerExpression::copy() {
    return new IfOneLinerExpression(dynamic_cast<Expression *>(this->Result->copy()),
                                    dynamic_cast<Expression *>(this->Condition->copy()),
                                    dynamic_cast<Expression *>(this->ElseResult->copy()));
}

plasma::ast::Node *plasma::ast::UnlessOneLinerExpression::copy() {
    return new UnlessOneLinerExpression(dynamic_cast<Expression *>(this->Result->copy()),
                                        dynamic_cast<Expression *>(this->Condition->copy()),
                                        dynamic_cast<Expression *>(this->ElseResult->copy()));
}

plasma::ast::Node *plasma::ast::ParenthesesExpression::copy() {
    return new ParenthesesExpression(dynamic_cast<Expression *>(this->X->copy()));
}

plasma::ast::Node *plasma::ast::AssignStatement::copy() {
    return new AssignStatement(
            dynamic_cast<Expression *>(this->LeftHandSide->copy()),
            this->AssignOperator,
            dynamic_cast<Expression *>(this->RightHandSide->copy())
    );
}

plasma::ast::Node *plasma::ast::DoWhileStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new DoWhileStatement(dynamic_cast<Expression *>(this->Condition->copy()), newBody);
}

plasma::ast::Node *plasma::ast::WhileStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new WhileStatement(dynamic_cast<Expression *>(this->Condition->copy()), newBody);
}

plasma::ast::Node *plasma::ast::UntilStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new UntilStatement(dynamic_cast<Expression *>(this->Condition->copy()), newBody);
}

plasma::ast::Node *plasma::ast::ForStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    std::vector<Identifier *> newReceivers;
    newReceivers.reserve(this->Receivers.size());
    for (Identifier *receiver : this->Receivers) {
        newReceivers.push_back(dynamic_cast<Identifier *>(receiver->copy()));
    }
    return new ForStatement(newReceivers, dynamic_cast<Expression *>(this->Source->copy()), newBody);
}

plasma::ast::Node *plasma::ast::IfStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    std::vector<Node *> newElseBody;
    newElseBody.reserve(this->Else.size());
    for (Node *receiver : this->Else) {
        newElseBody.push_back(receiver->copy());
    }
    return new IfStatement(dynamic_cast<Expression *>(this->Condition->copy()), newBody, newElseBody);
}

plasma::ast::Node *plasma::ast::UnlessStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    std::vector<Node *> newElseBody;
    newElseBody.reserve(this->Else.size());
    for (Node *receiver : this->Else) {
        newElseBody.push_back(receiver->copy());
    }
    return new UnlessStatement(dynamic_cast<Expression *>(this->Condition->copy()), newBody, newElseBody);
}

plasma::ast::Node *plasma::ast::SwitchStatement::copy() {
    std::vector<CaseBlock *> newCaseBlocks;
    newCaseBlocks.reserve(this->CaseBlocks.size());
    for (CaseBlock *caseBlock : this->CaseBlocks) {
        std::vector<Expression *> newCases;
        newCases.reserve(caseBlock->Cases.size());
        for (Expression *node : caseBlock->Cases) {
            newCases.push_back(dynamic_cast<Expression *>(node->copy()));
        }
        std::vector<Node *> newCaseBody;
        newCaseBody.reserve(caseBlock->Body.size());
        for (Node *node : caseBlock->Body) {
            newCaseBody.push_back(node->copy());
        }
        newCaseBlocks.push_back(new CaseBlock(newCases, newCaseBody));
    }
    std::vector<Node *> newDefault;
    newDefault.reserve(this->Default.size());
    for (Node *node : this->Default) {
        newDefault.push_back(node->copy());
    }
    return new SwitchStatement(dynamic_cast<Expression *>(this->Target->copy()), newCaseBlocks, newDefault);
}

plasma::ast::Node *plasma::ast::ModuleStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new ModuleStatement(dynamic_cast<Identifier *>(this->Name->copy()), newBody);
}

plasma::ast::Node *plasma::ast::FunctionDefinitionStatement::copy() {
    std::vector<Identifier *> newArguments;
    newArguments.reserve(this->Arguments.size());
    for (Identifier *argument : this->Arguments) {
        newArguments.push_back(dynamic_cast<Identifier *>(argument->copy()));
    }
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new FunctionDefinitionStatement(dynamic_cast<Identifier *>(this->Name->copy()), newArguments, newBody);
}

plasma::ast::Node *plasma::ast::InterfaceStatement::copy() {
    std::vector<Expression *> newBases;
    newBases.reserve(this->Bases.size());
    for (Expression *base : this->Bases) {
        newBases.push_back(dynamic_cast<Expression *>(base->copy()));
    }
    std::vector<FunctionDefinitionStatement *> newBody;
    newBody.reserve(this->MethodDefinitions.size());
    for (FunctionDefinitionStatement *node : this->MethodDefinitions) {
        newBody.push_back(dynamic_cast<FunctionDefinitionStatement *>(node->copy()));
    }
    return new InterfaceStatement(dynamic_cast<Identifier *>(this->Name->copy()), newBases, newBody);
}

plasma::ast::Node *plasma::ast::ClassStatement::copy() {
    std::vector<Expression *> newBases;
    newBases.reserve(this->Bases.size());
    for (Expression *base : this->Bases) {
        newBases.push_back(dynamic_cast<Expression *>(base->copy()));
    }
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new ClassStatement(dynamic_cast<Identifier *>(this->Name->copy()), newBases, newBody);
}

plasma::ast::Node *plasma::ast::TryStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }

    std::vector<ExceptBlock *> newExceptBlocks;
    newExceptBlocks.reserve(this->ExceptBlocks.size());
    for (ExceptBlock *exceptBlock : this->ExceptBlocks) {
        std::vector<Expression *> newTargets;
        newTargets.reserve(exceptBlock->Targets.size());
        for (Expression *target : exceptBlock->Targets) {
            newTargets.push_back(dynamic_cast<Expression *>(target->copy()));
        }

        std::vector<Node *> newExceptBody;
        newExceptBody.reserve(exceptBlock->Body.size());
        for (Node *node : exceptBlock->Body) {
            newExceptBody.push_back(node->copy());
        }

        Identifier *newCaptureName = nullptr;
        if (exceptBlock->CaptureName != nullptr) {
            newCaptureName = dynamic_cast<Identifier *>(exceptBlock->CaptureName->copy());
        }
        newExceptBlocks.push_back(new ExceptBlock(newTargets, newCaptureName, newExceptBody));
    }

    std::vector<Node *> newElseBody;
    newElseBody.reserve(this->Else.size());
    for (Node *node : this->Else) {
        newElseBody.push_back(node->copy());
    }
    std::vector<Node *> newFinallyBody;
    newFinallyBody.reserve(this->Finally.size());
    for (Node *node : this->Finally) {
        newFinallyBody.push_back(node->copy());
    }
    return new TryStatement(newBody, newExceptBlocks, newElseBody, newFinallyBody);
}

plasma::ast::Node *plasma::ast::RaiseStatement::copy() {
    return new RaiseStatement(dynamic_cast<Expression *>(this->X->copy()));
}

plasma::ast::Node *plasma::ast::PassStatement::copy() {
    return new PassStatement();
}

plasma::ast::Node *plasma::ast::BreakStatement::copy() {
    return new BreakStatement();
}

plasma::ast::Node *plasma::ast::ContinueStatement::copy() {
    return new ContinueStatement();
}

plasma::ast::Node *plasma::ast::RedoStatement::copy() {
    return new RedoStatement();
}

plasma::ast::Node *plasma::ast::BeginStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new BeginStatement(newBody);
}

plasma::ast::Node *plasma::ast::EndStatement::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    return new EndStatement(newBody);
}

plasma::ast::Node *plasma::ast::Program::copy() {
    std::vector<Node *> newBody;
    newBody.reserve(this->Body.size());
    for (Node *node : this->Body) {
        newBody.push_back(node->copy());
    }
    BeginStatement *begin = nullptr;
    if (this->Begin != nullptr) {
        begin = dynamic_cast<BeginStatement *>(this->Begin->copy());
    }
    EndStatement *end = nullptr;
    if (this->End != nullptr) {
        end = dynamic_cast<EndStatement *>(this->End->copy());
    }
    auto result = new Program();
    result->Begin = begin;
    result->Body = newBody;
    result->End = end;
    return result;
}