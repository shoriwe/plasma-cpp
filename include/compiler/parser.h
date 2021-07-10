#ifndef PLASMA_PARSER_H
#define PLASMA_PARSER_H

#include <string>
#include <any>

#include "error.h"
#include "compiler/ast.h"
#include "compiler/lexer.h"

namespace plasma::parser {
    const char ForStatement[] = "For Statement";
    const char UntilStatement[] = "Until Statement";
    const char ModuleStatement[] = "Module Statement";
    const char FunctionDefinitionStatement[] = "Function Definition Statement";
    const char ClassStatement[] = "Class Statement";
    const char RaiseStatement[] = "Raise Statement";
    const char TryStatement[] = "Try Statement";
    const char ExceptBlock[] = "Except Block";
    const char ElseBlock[] = "Else Block";
    const char FinallyBlock[] = "Finally Block";
    const char BeginStatement[] = "Begin Statement";
    const char EndStatement[] = "End Statement";
    const char InterfaceStatement[] = "Interface Statement";
    const char BinaryExpression[] = "Binary Expression";
    const char PointerExpression[] = "Pointer Expression";
    const char LambdaExpression[] = "Lambda Expression";
    const char ParenthesesExpression[] = "Parentheses Expression";
    const char TupleExpression[] = "Tuple Expression";
    const char ArrayExpression[] = "Array Expression";
    const char HashExpression[] = "Hash Expression";
    const char WhileStatement[] = "While Statement";
    const char DoWhileStatement[] = "Do-While Statement";
    const char IfStatement[] = "If Statement";
    const char ElifBlock[] = "Elif Block";
    const char UnlessStatement[] = "Unless Statement";
    const char SwitchStatement[] = "Switch Statement";
    const char CaseBlock[] = "Case Block";
    const char DefaultBlock[] = "Default Block";
    const char ReturnStatement[] = "Return Statement";
    const char YieldStatement[] = "Yield Statement";
    const char SuperStatement[] = "Super Statement";
    const char SelectorExpression[] = "Selector Expression";
    const char MethodInvocationExpression[] = "Method Invocation Expression";
    const char IndexExpression[] = "Index Expression";
    const char IfOneLinerExpression[] = "If One Liner Expression";
    const char UnlessOneLinerExpression[] = "Unless One Liner Expression";
    const char OneLineElseBlock[] = "One Line Else Block";
    const char GeneratorExpression[] = "Generator Expression";
    const char AssignStatement[] = "Assign Statement";

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
        lexer::token currentToken;

        explicit parser(lexer::lexer *lexer_);

        int currentLine() const;

        bool hasNext() const;

        bool next(error::error *result_error);

        bool directValueMatch(uint8_t directValue) const;

        bool kindMatch(uint8_t kind) const;

        bool removeNewLines(error::error *result_error);

        /*
         * Statements
         */
        bool parseAssignStatement(std::any leftHandSide, std::any *result, error::error *result_error);

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

        bool parseContinueStatement(std::any *result, error::error *result_error);

        bool parseBreakStatement(std::any *result, error::error *result_error);

        bool parseRedoStatement(std::any *result, error::error *result_error);

        bool parsePassStatement(std::any *result, error::error *result_error);

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

        bool parseSelectorExpression(std::any expression, std::any *result, error::error *result_error);

        bool parseMethodInvocationExpression(std::any expression, std::any *result, error::error *result_error);

        bool parseIndexExpression(std::any expression, std::any *result, error::error *result_error);

        bool parseIfOneLinerExpression(std::any expression, std::any *result, error::error *result_error);

        bool parseUnlessOneLinerExpression(std::any expression, std::any *result, error::error *result_error);

        bool parseGeneratorExpression(std::any expression, std::any *result, error::error *result_error);

        /*
         * Parse to a Program Object
         */
        bool parse(ast::Program *result, error::error *result_error);
    };
}
#endif //PLASMA_PARSER_H
