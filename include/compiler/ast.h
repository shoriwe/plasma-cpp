#ifndef PLASMA_AST_H
#define PLASMA_AST_H

#include <utility>
#include <vector>
#include <cinttypes>
#include <any>

#include "lexer.h"
#include "vm/virtual_machine.h"

namespace plasma::ast {
    enum {
        // Expressions
        SelectorID,
        IndexID,
        UnaryID,
        BinaryID,
        ReturnID,
        HashID,
        IdentifierID,
        ArrayID,
        TupleID,
        BasicLiteralID,
        LambdaID,
        GeneratorID,
        MethodInvocationID,
        IfOneLinerID,
        UnlessOneLinerID,
        ParenthesesID,
        // Statements
        DoWhileID,
        WhileID,
        UntilID,
        AssignID,
        ForID,
        IfID,
        UnlessID,
        SwitchID,
        ModuleID,
        FunctionDefinitionID,
        InterfaceID,
        ClassID,
        TryID,
        RaiseID,
        BeginID,
        EndID,
        ContinueID,
        RedoID,
        BreakID,
        PassID,
        ProgramID
    };

    struct Node {
        size_t TypeID;

        virtual bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) = 0;

        virtual Node *copy() = 0;
    };

    struct Expression : Node {
        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) { return false; };

        Node *copy() { return nullptr; };

        bool
        compile_and_push(bool push, std::vector<vm::instruction> *result, plasma::error::error *compilationError) {
            if (!this->compile(result, compilationError)) {
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
    };

    /*
     * Expressions available in CPlasma
     */

    struct ArrayExpression : public Expression {
        explicit ArrayExpression(std::vector<Expression *> values) {
            this->TypeID = ArrayID;
            this->Values = std::move(values);
        }

        ~ArrayExpression() {
            for (Expression *value : this->Values) {
                delete value;
            }
            this->Values.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Expression *> Values;
    };

    struct TupleExpression : public Expression {
        explicit TupleExpression(std::vector<Expression *> values) {
            this->TypeID = TupleID;
            this->Values = std::move(values);
        }

        ~TupleExpression() {
            for (Expression *value : this->Values) {
                delete value;
            }
            this->Values.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Expression *> Values;
    };

    struct KeyValue {
        KeyValue(Expression *key, Expression *value) {
            this->Key = key;
            this->Value = value;
        }

        ~KeyValue() {
            delete this->Key;
            delete this->Value;
        }

        Expression *Key;
        Expression *Value;
    };

    struct HashExpression : public Expression {
        explicit HashExpression(std::vector<KeyValue *> keyValues) {
            this->TypeID = HashID;
            this->KeyValues = std::move(keyValues);
        }

        ~HashExpression() {
            for (KeyValue *keyValue : this->KeyValues) {
                delete keyValue;
            }
            this->KeyValues.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<KeyValue *> KeyValues;
    };

    struct Identifier : public Expression {
        explicit Identifier(lexer::token token) : Token(std::move(token)) { this->TypeID = IdentifierID; }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        lexer::token Token;
    };

    struct BasicLiteralExpression : public Expression {
        explicit BasicLiteralExpression(lexer::token token) : Token(std::move(token)) { this->TypeID = BasicLiteralID; }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        lexer::token Token;
    };

    struct BinaryExpression : public Expression {
        BinaryExpression(Expression *leftHandSide, lexer::token op, Expression *rightHandSide) : Operator(
                std::move(op)) {
            this->TypeID = BinaryID;
            this->LeftHandSide = leftHandSide;
            this->RightHandSide = rightHandSide;
        }

        ~BinaryExpression() {
            delete this->LeftHandSide;
            delete this->RightHandSide;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *LeftHandSide;
        lexer::token Operator;
        Expression *RightHandSide;
    };

    struct UnaryExpression : public Expression {
        UnaryExpression(lexer::token op, Expression *x) : Operator(std::move(op)) {
            this->TypeID = UnaryID;
            this->X = x;
        }

        ~UnaryExpression() {
            delete this->X;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        lexer::token Operator;
        Expression *X;
    };

    struct ReturnStatement : public Node {
        explicit ReturnStatement(std::vector<Expression *> results) {
            this->TypeID = ReturnID;
            this->Results = std::move(results);
        }

        ~ReturnStatement() {
            for (Expression *result : this->Results) {
                delete result;
            }
            this->Results.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Expression *> Results;
    };

    struct LambdaExpression : public Expression {
        LambdaExpression(std::vector<Identifier *> arguments, ReturnStatement *output) {
            this->TypeID = LambdaID;
            this->Arguments = arguments;
            this->Output = output;
        }

        ~LambdaExpression() {
            for (Identifier *argument: this->Arguments) {
                delete argument;
            }
            this->Arguments.clear();
            delete this->Output;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Identifier *> Arguments;
        ReturnStatement *Output;
    };

    struct GeneratorExpression : public Expression {
        GeneratorExpression(Expression *op, std::vector<Identifier *> receivers, Expression *source) {
            this->TypeID = GeneratorID;
            this->Operation = op;
            this->Receivers = receivers;
            this->Source = source;
        }

        ~GeneratorExpression() {
            delete this->Operation;
            for (Identifier *receiver : this->Receivers) {
                delete receiver;
            }
            this->Receivers.clear();
            delete this->Source;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Operation;
        std::vector<Identifier *> Receivers;
        Expression *Source;
    };

    struct SelectorExpression : public Expression {
        SelectorExpression(Expression *x, Identifier *name) {
            this->TypeID = SelectorID;
            this->X = x;
            this->Identifier = name;
        }

        ~SelectorExpression() {
            delete this->X;
            delete this->Identifier;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *X = nullptr;
        Identifier *Identifier;
    };

    struct MethodInvocationExpression : public Expression {
        MethodInvocationExpression(Expression *function, std::vector<Expression *> arguments) {
            this->TypeID = MethodInvocationID;
            this->Function = function;
            this->Arguments = arguments;
        }

        ~MethodInvocationExpression() {
            delete this->Function;
            for (Expression *argument : this->Arguments) {
                delete argument;
            }
            this->Arguments.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Function;
        std::vector<Expression *> Arguments;
    };

    struct IndexExpression : public Expression {
        IndexExpression(Expression *source, Expression *index) {
            this->TypeID = IndexID;
            this->Source = source;
            this->Index = index;
        }

        ~IndexExpression() {
            delete this->Source;
            delete this->Index;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Source;
        Expression *Index;
    };

    struct IfOneLinerExpression : public Expression {
        IfOneLinerExpression(Expression *result, Expression *condition) {
            this->TypeID = IfOneLinerID;
            this->Result = result;
            this->Condition = condition;
            this->ElseResult = new Identifier(
                    lexer::token{
                            .string = lexer::NoneString
                    }
            );
        }

        IfOneLinerExpression(Expression *result, Expression *condition, Expression *elseResult) {
            this->TypeID = IfOneLinerID;
            this->Result = result;
            this->Condition = condition;
            this->ElseResult = elseResult;
        }

        ~IfOneLinerExpression() {
            delete this->Result;
            delete this->Condition;
            delete this->ElseResult;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Result;
        Expression *Condition;
        Expression *ElseResult;
    };

    struct UnlessOneLinerExpression : public Expression {

        UnlessOneLinerExpression(Expression *result, Expression *condition) {
            this->TypeID = UnlessOneLinerID;
            this->Result = result;
            this->Condition = condition;
            this->ElseResult = new Identifier(
                    lexer::token{
                            .string = lexer::NoneString
                    }
            );
        }

        UnlessOneLinerExpression(Expression *result, Expression *condition, Expression *elseResult) {
            this->TypeID = UnlessOneLinerID;
            this->Result = result;
            this->Condition = condition;
            this->ElseResult = elseResult;
        }

        ~UnlessOneLinerExpression() {
            delete this->Result;
            delete this->Condition;
            delete this->ElseResult;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Result;
        Expression *Condition;
        Expression *ElseResult;
    };

    struct ParenthesesExpression : public Expression {
        ParenthesesExpression(Expression *x) {
            this->TypeID = ParenthesesID;
            this->X = x;
        }

        ~ParenthesesExpression() {
            delete this->X;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *X;
    };

    /*
     * Statements Available in CPlasma
     */

    struct AssignStatement : public Node {

        AssignStatement(Expression *leftHandSide, lexer::token assignOperator, Expression *rightHandSide)
                : AssignOperator(assignOperator) {
            this->TypeID = AssignID;
            this->LeftHandSide = leftHandSide;
            this->RightHandSide = rightHandSide;
        }

        ~AssignStatement() {
            delete this->LeftHandSide;
            delete this->RightHandSide;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *LeftHandSide;
        lexer::token AssignOperator;
        Expression *RightHandSide;
    };

    struct DoWhileStatement : public Node {
        DoWhileStatement(Expression *condition, std::vector<Node *> body) {
            this->TypeID = DoWhileID;
            this->Condition = condition;
            this->Body = body;
        }

        ~DoWhileStatement() {
            delete this->Condition;
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Condition;
        std::vector<Node *> Body;

    };

    struct WhileStatement : public Node {
        WhileStatement(Expression *condition, std::vector<Node *> body) {
            this->TypeID = WhileID;
            this->Condition = condition;
            this->Body = body;
        }

        ~WhileStatement() {
            delete this->Condition;
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Condition;
        std::vector<Node *> Body;

    };

    struct UntilStatement : public Node {
        UntilStatement(Expression *condition, std::vector<Node *> body) {
            this->TypeID = UntilID;
            this->Condition = condition;
            this->Body = body;
        }

        ~UntilStatement() {
            delete this->Condition;
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Condition;
        std::vector<Node *> Body;
    };

    struct ForStatement : public Node {
        ForStatement(std::vector<Identifier *> receivers, Expression *source, std::vector<Node *> body) {
            this->TypeID = ForID;
            this->Receivers = receivers;
            this->Source = source;
            this->Body = body;
        }

        ~ForStatement() {
            for (Identifier *receiver : this->Receivers) {
                delete receiver;
            }
            this->Receivers.clear();
            delete this->Source;
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }


        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Identifier *> Receivers;
        Expression *Source;
        std::vector<Node *> Body;
    };

    struct IfStatement : public Node {
        IfStatement(Expression *condition, std::vector<Node *> body,
                    std::vector<Node *> elseBody) {
            this->TypeID = IfID;
            this->Condition = condition;
            this->Body = body;
            this->Else = elseBody;
        }

        ~IfStatement() {
            delete this->Condition;
            for (Node *node: this->Body) {
                delete node;
            }
            this->Body.clear();

            for (Node *node: this->Else) {
                delete node;
            }
            this->Else.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Condition;
        std::vector<Node *> Body;
        std::vector<Node *> Else;
    };

    struct UnlessStatement : public Node {
        UnlessStatement(Expression *condition, std::vector<Node *> body,
                        std::vector<Node *> elseBody) {
            this->TypeID = UnlessID;
            this->Condition = condition;
            this->Body = body;
            this->Else = elseBody;
        }

        ~UnlessStatement() {
            delete this->Condition;

            for (Node *node: this->Body) {
                delete node;
            }
            this->Body.clear();

            for (Node *node: this->Else) {
                delete node;
            }
            this->Else.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Condition;
        std::vector<Node *> Body;
        std::vector<Node *> Else;
    };

    struct CaseBlock {
        CaseBlock(std::vector<Expression *> cases, std::vector<Node *> body) {
            this->Cases = cases;
            this->Body = body;
        }

        ~CaseBlock() {
            for (Expression *c : this->Cases) {
                delete c;
            }
            this->Cases.clear();

            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        std::vector<Expression *> Cases;
        std::vector<Node *> Body;
    };

    struct SwitchStatement : public Node {
        SwitchStatement(Expression *target, std::vector<CaseBlock *> caseBlocks, std::vector<Node *> defaultBody) {
            this->TypeID = SwitchID;
            this->Target = target;
            this->CaseBlocks = caseBlocks;
            this->Default = defaultBody;
        }

        ~SwitchStatement() {
            delete this->Target;
            for (CaseBlock *caseBlock : this->CaseBlocks) {
                delete caseBlock;
            }
            this->CaseBlocks.clear();

            for (Node *node : this->Default) {
                delete node;
            }
            this->Default.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *Target;
        std::vector<CaseBlock *> CaseBlocks;
        std::vector<Node *> Default;
    };

    struct ModuleStatement : public Node {
        ModuleStatement(Identifier *name, std::vector<Node *> body) {
            this->TypeID = ModuleID;
            this->Name = name;
            this->Body = body;
        }

        ~ModuleStatement() {
            delete this->Name;
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Identifier *Name;
        std::vector<Node *> Body;
    };

    struct FunctionDefinitionStatement : public Node {
        FunctionDefinitionStatement(Identifier *name, std::vector<Identifier *> arguments, std::vector<Node *> body) {
            this->TypeID = FunctionDefinitionID;
            this->Name = name;
            this->Arguments = arguments;
            this->Body = body;
        }

        ~FunctionDefinitionStatement() {
            delete this->Name;
            for (Identifier *argument : this->Arguments) {
                delete argument;
            }
            this->Arguments.clear();

            for (Node *node :this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Identifier *Name;
        std::vector<Identifier *> Arguments;
        std::vector<Node *> Body;
    };

    struct InterfaceStatement : public Node {
        InterfaceStatement(Identifier *name, std::vector<Expression *> bases,
                           std::vector<FunctionDefinitionStatement *> methods) {
            this->TypeID = InterfaceID;
            this->Name = name;
            this->Bases = bases;
            this->MethodDefinitions = methods;
        }

        ~InterfaceStatement() {
            delete this->Name;
            for (Expression *base : this->Bases) {
                delete base;
            }
            this->Bases.clear();

            for (FunctionDefinitionStatement *functionDefinitionStatement : this->MethodDefinitions) {
                delete functionDefinitionStatement;
            }
            this->MethodDefinitions.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Identifier *Name;
        std::vector<Expression *> Bases;
        std::vector<FunctionDefinitionStatement *> MethodDefinitions;
    };

    struct ClassStatement : public Node {
        ClassStatement(Identifier *name, std::vector<Expression *> bases,
                       std::vector<Node *> body) {
            this->TypeID = ClassID;
            this->Name = name;
            this->Bases = bases;
            this->Body = body;
        }

        ~ClassStatement() {
            delete this->Name;
            for (Expression *base : this->Bases) {
                delete base;
            }
            this->Bases.clear();

            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Identifier *Name;
        std::vector<Expression *> Bases;
        std::vector<Node *> Body;
    };

    struct ExceptBlock {
        ExceptBlock(TupleExpression *targets, Identifier *captureName, std::vector<Node *> body) {
            this->Targets = targets;
            this->CaptureName = captureName;
            this->Body = body;
        }

        ~ExceptBlock() {
            delete this->Targets;

            if (this->CaptureName != nullptr) {
                delete this->CaptureName;
            }

            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        TupleExpression *Targets;
        Identifier *CaptureName;
        std::vector<Node *> Body;
    };

    struct TryStatement : public Node {
        TryStatement(std::vector<Node *> body, std::vector<ExceptBlock *> exceptBlocks, std::vector<Node *> elseBody,
                     std::vector<Node *> finallyBody) {
            this->TypeID = TryID;
            this->Body = body;
            this->ExceptBlocks = exceptBlocks;
            this->Else = elseBody;
            this->Finally = finallyBody;
        }

        ~TryStatement() {
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();

            for (ExceptBlock *exceptBlock : this->ExceptBlocks) {
                delete exceptBlock;
            }
            this->ExceptBlocks.clear();

            for (Node *node : this->Else) {
                delete node;
            }
            this->Else.clear();

            for (Node *node : this->Finally) {
                delete node;
            }
            this->Finally.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Node *> Body;
        std::vector<ExceptBlock *> ExceptBlocks;
        std::vector<Node *> Else;
        std::vector<Node *> Finally;
    };

    struct RaiseStatement : public Node {
        RaiseStatement(Expression *x) {
            this->TypeID = RaiseID;
            this->X = x;
        }

        ~RaiseStatement() {
            delete this->X;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        Expression *X;
    };

    struct BeginStatement : public Node {
        BeginStatement(std::vector<Node *> body) {
            this->TypeID = BeginID;
            this->Body = body;
        }

        ~BeginStatement() {
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Node *> Body;
    };

    struct EndStatement : public Node {
        EndStatement(std::vector<Node *> body) {
            this->TypeID = EndID;
            this->Body = body;
        }

        ~EndStatement() {
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        std::vector<Node *> Body;
    };

    struct ContinueStatement : public Node {
        ContinueStatement() {
            this->TypeID = ContinueID;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;
    };

    struct BreakStatement : public Node {
        BreakStatement() {
            this->TypeID = BreakID;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;
    };

    struct RedoStatement : public Node {
        RedoStatement() {
            this->TypeID = RedoID;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;
    };

    struct PassStatement : public Node {
        PassStatement() {
            this->TypeID = PassID;
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;
    };

    /*
     * Program is an special node that is only used as the root of the entire program to be compiled
     */
    struct Program : public Node {
        Program() {
            this->TypeID = ProgramID;
        }

        ~Program() {
            if (this->Begin != nullptr) {
                delete this->Begin;
            }
            if (this->End != nullptr) {
                delete this->End;
            }
            for (Node *node : this->Body) {
                delete node;
            }
            this->Body.clear();
        }

        bool compile(std::vector<vm::instruction> *result, plasma::error::error *compilationError) override;

        Node *copy() override;

        BeginStatement *Begin = nullptr;
        EndStatement *End = nullptr;
        std::vector<Node *> Body;
    };

    /*
     * Tools to check the nodes
     */
    bool isExpression(Node *node);
}

#endif //PLASMA_AST_H
