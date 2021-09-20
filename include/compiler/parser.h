#ifndef PLASMA_PARSER_H
#define PLASMA_PARSER_H

#include <string>
#include <any>

#include "plasma_error.h"
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
    error::error newSyntaxError(int line, const std::string &nodeType);

    error::error newNonExpressionReceivedError(int line, const std::string &nodeType);

    error::error newNonIdentifierReceivedError(int line, const std::string &nodeType);

    error::error newStatementNeverEndedError(int line, const std::string &nodeType);

    error::error newInvalidKindOfTokenError(int line);

    error::error newExpressionNeverClosed(int line, const std::string &nodeType);

    error::error newNonFunctionDefinitionReceived(int line, const std::string &nodeType);

    /*
     * parser object
     */
    struct parser {
        lexer::lexer *Lexer;
        lexer::token currentToken;

        explicit parser(lexer::lexer *lexer_);

        int currentLine() const;

        bool hasNext() const;

        void next();

        bool directValueMatch(uint8_t directValue) const;

        bool kindMatch(uint8_t kind) const;

        void removeNewLines();

        /*
         * Statements
         */
        ast::AssignStatement *parseAssignStatement(ast::Expression *leftHandSide);

        ast::ForStatement *parseForStatement();

        ast::UntilStatement *parseUntilStatement();

        ast::ModuleStatement *parseModuleStatement();

        ast::FunctionDefinitionStatement *parseFunctionDefinitionStatement();

        ast::ClassStatement *parseClassStatement();

        ast::RaiseStatement *parseRaiseStatement();

        ast::TryStatement *parseTryStatement();

        ast::EndStatement *parseEndStatement();

        ast::BeginStatement *parseBeginStatement();

        ast::InterfaceStatement *parseInterfaceStatement();

        ast::WhileStatement *parseWhileStatement();

        ast::DoWhileStatement *parseDoWhileStatement();

        ast::IfStatement *parseIfStatement();

        ast::UnlessStatement *parseUnlessStatement();

        ast::SwitchStatement *parseSwitchStatement();

        ast::ReturnStatement *parseReturnStatement();

        ast::ContinueStatement *parseContinueStatement();

        ast::BreakStatement *parseBreakStatement();

        ast::RedoStatement *parseRedoStatement();

        ast::PassStatement *parsePassStatement();

        /*
         * Expressions
         */
        ast::Node *parseBinaryExpression(uint8_t precedence);

        ast::Node *parseUnaryExpression();

        ast::Node *parsePrimaryExpression();

        ast::BasicLiteralExpression *parseLiteral();

        ast::Node *parseOperand();

        ast::LambdaExpression *parseLambdaExpression();

        ast::Node *parseParenthesesExpression();

        ast::ArrayExpression *parseArrayExpression();

        ast::HashExpression *parseHashExpression();

        ast::SelectorExpression *parseSelectorExpression(ast::Expression *source);

        ast::MethodInvocationExpression *parseMethodInvocationExpression(ast::Expression *method);

        ast::IndexExpression *parseIndexExpression(ast::Expression *source);

        ast::IfOneLinerExpression *parseIfOneLinerExpression(ast::Expression *onTrueResult);

        ast::UnlessOneLinerExpression *parseUnlessOneLinerExpression(ast::Expression *onFalseResult);

        ast::GeneratorExpression *parseGeneratorExpression(ast::Expression* expression);

        /*
         * Parse to a Program Object
         */
        ast::Program *parse();
    };
}
#endif //PLASMA_PARSER_H
