#ifndef PLASMA_PARSER_H
#define PLASMA_PARSER_H

#include <string>
#include <any>

#include "error.h"
#include "compiler/ast.h"
#include "compiler/lexer.h"

namespace plasma::parser {
    const std::string ForStatement = "For Statement";
    const std::string UntilStatement = "Until Statement";
    const std::string ModuleStatement = "Module Statement";
    const std::string FunctionDefinitionStatement = "Function Definition Statement";
    const std::string ClassStatement = "Class Statement";
    const std::string RaiseStatement = "Raise Statement";
    const std::string TryStatement = "Try Statement";
    const std::string ExceptBlock = "Except Block";
    const std::string ElseBlock = "Else Block";
    const std::string FinallyBlock = "Finally Block";
    const std::string BeginStatement = "Begin Statement";
    const std::string EndStatement = "End Statement"
    const std::string InterfaceStatement = "Interface Statement";
    const std::string BinaryExpression = "Binary Expression";
    const std::string PointerExpression = "Pointer Expression";
    const std::string LambdaExpression = "Lambda Expression";
    const std::string ParenthesesExpression = "Parentheses Expression";
    const std::string TupleExpression = "Tuple Expression";
    const std::string ArrayExpression = "Array Expression";
    const std::string HashExpression = "Hash Expression";
    const std::string WhileStatement = "While Statement";
    const std::string DoWhileStatement = "Do-While Statement";
    const std::string IfStatement = "If Statement";
    const std::string ElifBlock = "Elif Block";
    const std::string UnlessStatement = "Unless Statement";
    const std::string SwitchStatement = "Switch Statement";
    const std::string CaseBlock = "Case Block";
    const std::string DefaultBlock = "Default Block";
    const std::string DeferStatement = "Defer Statement";
    const std::string ReturnStatement = "Return Statement";
    const std::string YieldStatement = "Yield Statement";
    const std::string SuperStatement = "Super Statement";
    const std::string SelectorExpression = "Selector Expression";
    const std::string MethodInvocationExpression = "Method Invocation Expression";
    const std::string IndexExpression = "Index Expression";
    const std::string IfOneLinerExpression = "If One Liner Expression";
    const std::string UnlessOneLinerExpression = "Unless One Liner Expression";
    const std::string OneLineElseBlock = "One Line Else Block";
    const std::string GeneratorExpression = "Generator Expression";
    const std::string AssignStatement = "Assign Statement";

    /*
     * Quick error creation
     */
    void newSyntaxError(int line, const std::string &nodeType, error::error *result_error);

    void newNonExpressionReceivedError(int line, const std::string &nodeType, error::error *result_error);

    void newNonIdentifierReceivedError(int line, const std::string &nodeType, error::error *result_error);

    void newStatementNeverEndedError(int line, const std::string &nodeType, error::error *result_error);

    void newInvalidKindOfTokenError(int line, error::error *result_error);

    void newExpressionNeverClosed(int line, const std::string &nodeType, error::error *result_error);

    void newNonFunctionDefinitionReceived(int line, const std::string &nodeType, error::error *result_error);

    /*
     * parser object
     */
    struct parser {
        lexer::lexer *Lexer;
        bool Complete;
        lexer::token *currentToken;

        explicit parser(lexer::lexer *lexer_);

        int currentLine();

        bool hasNext() const;

        bool next(error::error *result_error) const;

        bool directValueMatch(uint8_t directValue) const;

        bool kindMatch(uint8_t kind) const;

        bool removeNewLines(error::error *result_error) const;

        /*
         * Statements
         */
        bool parseAssignStatement(std::any *leftHandSide, std::any *result, error::error *result_error);

        bool parseForStatement(std::any *result, error::error *result_error);

        bool parseUntilStatement(std::any *result, error::error *result_error);

        bool parseModuleStatement(std::any *result, error::error *result_error);

        bool parseFunctionDefinitionStatement(std::any *result, error::error *result_error);

        bool parseClassStatement(std::any *result, error::error *result_error);

        bool parseRaiseStatement(std::any *result, error::error *result_error);

        bool parseTryStatement(std::any *result, error::error *result_error);

        bool parseBeginStatement(std::any *result, error::error *result_error);

        bool parseEndStatement(std::any *result, error::error *result_error);

        bool parseInterfaceStatement(std::any *result, error::error *result_error);

        bool parseWhileStatement(std::any *result, error::error *result_error);

        bool parseDoWhileStatement(std::any *result, error::error *result_error);

        bool parseIfStatement(std::any *result, error::error *result_error);

        bool parseUnlessStatement(std::any *result, error::error *result_error);

        bool parseSwitchStatement(std::any *result, error::error *result_error);

        bool parseReturnStatement(std::any *result, error::error *result_error);

        bool parseYieldStatement(std::any *result, error::error *result_error);

        bool parseSuperStatement(std::any *result, error::error *result_error);

        bool parseContinueStatement(std::any *result, error::error *result_error) const;

        bool parseBreakStatement(std::any *result, error::error *result_error) const;

        bool parseRedoStatement(std::any *result, error::error *result_error) const;

        bool parsePassStatement(std::any *result, error::error *result_error) const;

        /*
         * Expressions
         */
        bool parseBinaryExpression(uint8_t precedence, std::any *result, error::error *result_error);

        bool parseUnaryExpression(std::any *result, error::error *result_error);

        bool parsePrimaryExpression(std::any *result, error::error *result_error);

        bool parseLiteral(std::any *result, error::error *result_error);

        bool parseOperand(std::any *result, error::error *result_error);

        bool parseLambdaExpression(std::any *result, error::error *result_error);

        bool parseParenthesesExpression(std::any *result, error::error *result_error);

        bool parseArrayExpression(std::any *result, error::error *result_error);

        bool parseHashExpression(std::any *result, error::error *result_error);

        bool parseSelectorExpression(std::any *result, error::error *result_error);

        bool parseMethodInvocationExpression(std::any *result, error::error *result_error);

        bool parseIndexExpression(std::any *result, error::error *result_error);

        bool parseIfOneLinerExpression(std::any *result, error::error *result_error);

        bool parseUnlessOneLinerExpression(std::any *result, error::error *result_error);

        bool parseGeneratorExpression(std::any *result, error::error *result_error);

        /*
         * Parse to a Program Object
         */
        bool parse(ast::Program *result, error::error *result_error);
    };
}
#endif //PLASMA_PARSER_H
