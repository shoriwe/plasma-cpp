#ifndef PLASMA_AST_H
#define PLASMA_AST_H

#include <vector>
#include <cinttypes>
#include <any>

#include "lexer.h"

namespace plasma::ast {

    /*
     * Expressions available in CPlasma
     */

    struct ArrayExpression {
        std::vector<std::any> Values;
    };

    struct TupleExpression {
        std::vector<std::any> Values;
    };

    struct KeyValue {
        std::any Key;
        std::any Value;
    };

    struct HashExpression {
        std::vector<KeyValue> Values;
    };

    struct Identifier {
        lexer::token Token;
    };

    struct BasicLiteralExpression {
        lexer::token Token;
        uint8_t Kind;
        uint8_t DirectValue;
    };

    struct BinaryExpression {
        std::any LeftHandSide;
        lexer::token Operator;
        std::any RightHandSide;
    };

    struct UnaryExpression {
        lexer::token Operator;
        std::any X;
    };

    struct LambdaExpression {
        std::vector<Identifier> Arguments;
        std::any Code;
    };

    struct GeneratorExpression {
        std::any Operation;
        std::vector<Identifier> Receivers;
        std::any Source;
    };

    struct SelectorExpression {
        std::any X;
        Identifier Identifier;
    };

    struct MethodInvocationExpression {
        std::any Function;
        std::vector<std::any> Arguments;
    };

    struct IndexExpression {
        std::any Source;
        std::any Index;
    };

    struct IfOneLinerExpression {
        std::any Result;
        std::any Condition;
        std::any ElseResult;
    };

    struct UnlessOneLinerExpression {
        std::any Result;
        std::any Condition;
        std::any ElseResult;
    };

    /*
     * Statements Available in CPlasma
     */

    struct AssignStatement {
        std::any LeftHandSide;
        lexer::token AssignOperator;
        std::any RightHandSide;
    };

    struct DoWhileStatement {
        std::any Condition;
        std::vector<std::any> Body;

    };

    struct WhileStatement {
        std::any Condition;
        std::vector<std::any> Body;

    };

    struct UntilStatement {
        std::any Condition;
        std::vector<std::any> Body;
    };

    struct ForStatement {
        std::vector<Identifier> Receivers;
        std::any Source;
        std::vector<std::any> Body;
    };

    struct ElifBlock {
        std::any Condition;
        std::vector<std::any> Body;
    };

    struct IfStatement {
        std::any Condition;
        std::vector<std::any> Body;
        std::vector<ElifBlock> ElifBlocks;
        std::vector<std::any> Else;
    };

    struct UnlessStatement {
        std::any Condition;
        std::vector<std::any> Body;
        std::vector<ElifBlock> ElifBlocks;
        std::vector<std::any> Else;
    };

    struct CaseBlock {
        std::vector<std::any> Cases;
        std::vector<std::any> Body;
    };

    struct SwitchStatement {
        std::any Target;
        std::vector<CaseBlock> CaseBlocks;
        std::vector<std::any> Default;
    };

    struct ModuleStatement {
        Identifier Name;
        std::vector<std::any> Body;
    };

    struct FunctionDefinitionStatement {
        Identifier Name;
        std::vector<Identifier> Arguments;
        std::vector<std::any> Body;
    };

    struct InterfaceStatement {
        Identifier Name;
        std::vector<std::any> Bases;
        std::vector<FunctionDefinitionStatement> MethodDefinitions;
    };

    struct ClassStatement {
        Identifier Name;
        std::vector<std::any> Bases;
        std::vector<std::any> Body;
    };

    struct ExceptBlock {
        std::vector<std::any> Targets;
        Identifier CaptureName;
        std::vector<std::any> Body;
    };

    struct TryStatement {
        std::vector<std::any> Body;
        std::vector<ExceptBlock> ExceptBlocks;
        std::vector<std::any> Else;
        std::vector<std::any> Finally;
    };

    struct RaiseStatement {
        std::any X;
    };

    struct BeginStatement {
        std::vector<std::any> Body;
    };

    struct EndStatement {
        std::vector<std::any> Body;
    };

    struct ReturnStatement {
        std::vector<std::any> Results;
    };

    struct YieldStatement {
        std::vector<std::any> Results;
    };

    struct SuperInvocationStatement {
        std::vector<std::any> Arguments;
    };

    struct ContinueStatement {
    };

    struct BreakStatement {
    };

    struct RedoStatement {
    };

    struct PassStatement {
    };

    /*
     * Program is an special node that is only used as the root of the entire program to be compiled
     */
    struct Program {
        BeginStatement Begin;
        EndStatement End;
        std::vector<std::any> Body;
    };

    /*
     * Tools to check the nodes
     */
    bool isExpression(std::any *node);
}

#endif //PLASMA_AST_H
