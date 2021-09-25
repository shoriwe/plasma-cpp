#include <iostream>
#include <utility>
#include "compiler/parser.h"

plasma::error::error plasma::parser::newSyntaxError(int line, const std::string &nodeType) {
    return error::error(error::SyntaxError, "invalid definition of " + nodeType, line);
}

plasma::error::error plasma::parser::newNonExpressionReceivedError(int line, const std::string &nodeType) {
    return error::error(error::SyntaxError, "received a non expression in " + nodeType, line);
}

plasma::error::error plasma::parser::newNonIdentifierReceivedError(int line, const std::string &nodeType) {
    return error::error(error::SyntaxError, "received a non identifier in " + nodeType, line);
}

plasma::error::error plasma::parser::newStatementNeverEndedError(int line, const std::string &nodeType) {
    return error::error(error::SyntaxError, nodeType + " never ended", line);
}

plasma::error::error plasma::parser::newInvalidKindOfTokenError(int line) {
    return error::error(error::ParsingError, "invalid kind of token", line);
}

plasma::error::error plasma::parser::newExpressionNeverClosed(int line, const std::string &nodeType) {
    return error::error(error::SyntaxError, nodeType + " never closed", line);
}

plasma::error::error plasma::parser::newNonFunctionDefinitionReceived(int line, const std::string &nodeType) {
    return error::error(error::SyntaxError, "non function definition received in " + nodeType, line);
}

plasma::parser::parser::parser(lexer::lexer *lexer_) {
    this->Lexer = lexer_;
    this->currentToken = lexer::token{
            .directValue = lexer::Unknown,
            .kind = lexer::Unknown,
            .line = 0
    };
}

int plasma::parser::parser::currentLine() const {
    return this->currentToken.line;
}

bool plasma::parser::parser::hasNext() const {
    return this->Lexer->hasNext();
}

void plasma::parser::parser::next() {
    lexer::token result;
    error::error nextError;
    if (!this->Lexer->next(&result, &nextError)) {
        throw nextError;
    }
    this->currentToken = result;
}

bool plasma::parser::parser::directValueMatch(uint8_t directValue) const {
    return this->currentToken.directValue == directValue;
}

bool plasma::parser::parser::kindMatch(uint8_t kind) const {
    return this->currentToken.kind == kind;
}

void plasma::parser::parser::removeNewLines() {
    while (this->directValueMatch(lexer::NewLine)) {
        this->next();
    }
}

/*
 * Statements
 */

plasma::ast::AssignStatement *
plasma::parser::parser::parseAssignStatement(ast::Expression *leftHandSide) {
    lexer::token assignmentToken = this->currentToken;
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *rightHandSide = this->parseBinaryExpression(0);
    if (!ast::isExpression(rightHandSide)) {
        throw newNonExpressionReceivedError(line, AssignStatement);
    }
    return new ast::AssignStatement(
            leftHandSide,
            assignmentToken,
            dynamic_cast<ast::Expression *>(rightHandSide)
    );
}

plasma::ast::ForStatement *plasma::parser::parser::parseForStatement() {
    this->next();
    this->removeNewLines();
    std::vector<ast::Identifier *> receivers;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::In)) {
            break;
        } else if (!this->kindMatch(lexer::IdentifierKind)) {
            throw newSyntaxError(this->currentLine(), ForStatement);
        }
        this->removeNewLines();
        receivers.push_back(new ast::Identifier(this->currentToken));
        this->next();
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        } else if (this->directValueMatch(lexer::In)) {
            break;
        } else {
            throw newSyntaxError(this->currentLine(), ForStatement);
        }
    }
    this->removeNewLines();
    if (!this->directValueMatch(lexer::In)) {
        throw newSyntaxError(this->currentLine(), ForStatement);
    }
    this->next();
    this->removeNewLines();
    ast::Node *source = this->parseBinaryExpression(0);
    if (!ast::isExpression(source)) {
        throw newNonExpressionReceivedError(this->currentLine(), ForStatement);
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), ForStatement);
    }
    this->next();
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), ForStatement);
    }
    this->next();
    return new ast::ForStatement(receivers, dynamic_cast<ast::Expression *>(source), body);
}

plasma::ast::UntilStatement *plasma::parser::parser::parseUntilStatement() {
    this->next();
    this->removeNewLines();
    ast::Node *condition = this->parseBinaryExpression(0);
    if (!ast::isExpression(condition)) {
        throw newSyntaxError(this->currentLine(), UntilStatement);
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), UntilStatement);
    }
    this->next();
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), UntilStatement);
    }
    this->next();
    return new ast::UntilStatement(dynamic_cast<ast::Expression *>(condition), body);
}

plasma::ast::WhileStatement *plasma::parser::parser::parseWhileStatement() {
    this->next();
    this->removeNewLines();
    ast::Node *condition = this->parseBinaryExpression(0);
    if (!ast::isExpression(condition)) {
        throw newSyntaxError(this->currentLine(), WhileStatement);
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), WhileStatement);
    }
    this->next();
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), WhileStatement);
    }
    this->next();
    return new ast::WhileStatement(dynamic_cast<ast::Expression *>(condition), body);
}

plasma::ast::DoWhileStatement *plasma::parser::parser::parseDoWhileStatement() {
    this->next();
    std::vector<ast::Node *> body;
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), DoWhileStatement);
    }
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::While)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::While)) {
        throw newSyntaxError(this->currentLine(), DoWhileStatement);
    }
    this->next();
    this->removeNewLines();
    ast::Node *condition = this->parseBinaryExpression(0);
    if (!ast::isExpression(condition)) {
        throw newNonExpressionReceivedError(this->currentLine(), DoWhileStatement);
    }
    return new ast::DoWhileStatement(
            dynamic_cast<ast::Expression *>(condition),
            body
    );
}

plasma::ast::ModuleStatement *plasma::parser::parser::parseModuleStatement() {
    this->next();
    this->removeNewLines();
    if (!this->kindMatch(lexer::IdentifierKind)) {
        throw newSyntaxError(this->currentLine(), ModuleStatement);
    }
    ast::Identifier *name = new ast::Identifier(this->currentToken);
    this->next();
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), ModuleStatement);
    }
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), ModuleStatement);
    }
    this->next();
    return new ast::ModuleStatement(
            name,
            body
    );
}

plasma::ast::FunctionDefinitionStatement *plasma::parser::parser::parseFunctionDefinitionStatement() {
    this->next();
    this->removeNewLines();
    if (!this->kindMatch(lexer::IdentifierKind)) {
        throw newSyntaxError(this->currentLine(), FunctionDefinitionStatement);
    }
    ast::Identifier *name = new ast::Identifier(this->currentToken);
    this->next();
    this->removeNewLines();
    if (!this->directValueMatch(lexer::OpenParentheses)) {
        throw newSyntaxError(this->currentLine(), FunctionDefinitionStatement);
    }
    this->next();
    std::vector<ast::Identifier *> arguments;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        }
        this->removeNewLines();
        if (!this->kindMatch(lexer::IdentifierKind)) {
            throw newSyntaxError(this->currentLine(), FunctionDefinitionStatement);
        }
        arguments.push_back(new ast::Identifier(this->currentToken));
        this->next();
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        } else if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        } else {
            throw newSyntaxError(this->currentLine(), FunctionDefinitionStatement);
        }
    }
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        throw newSyntaxError(this->currentLine(), FunctionDefinitionStatement);
    }
    this->next();
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), FunctionDefinitionStatement);
    }
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), FunctionDefinitionStatement);
    }
    this->next();
    return new ast::FunctionDefinitionStatement(name, arguments, body);
}

plasma::ast::RaiseStatement *plasma::parser::parser::parseRaiseStatement() {
    this->next();
    ast::Node *x = this->parseBinaryExpression(0);
    if (!ast::isExpression(x)) {
        throw newNonExpressionReceivedError(this->currentLine(), RaiseStatement);
    }
    return new ast::RaiseStatement(dynamic_cast<ast::Expression *>(x));
}

plasma::ast::TryStatement *plasma::parser::parser::parseTryStatement() {
    this->next();
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), TryStatement);
    }
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End) ||
                this->directValueMatch(lexer::Except) ||
                this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::Finally)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    std::vector<ast::ExceptBlock *> exceptBlocks;
    while (this->hasNext()) {
        if (!this->directValueMatch(lexer::Except)) {
            break;
        }
        this->next();
        std::vector<ast::Expression *> targets;
        ast::Node *target;
        while (this->hasNext()) {
            if (this->directValueMatch(lexer::NewLine) ||
                this->directValueMatch(lexer::As)) {
                break;
            }
            target = this->parseBinaryExpression(0);
            if (!ast::isExpression(target)) {
                throw newSyntaxError(this->currentLine(), ExceptBlock);
            }
            targets.push_back(dynamic_cast<ast::Expression *>(target));
            if (this->directValueMatch(lexer::Comma)) {
                this->next();
            }
        }
        ast::Identifier *captureName = nullptr;
        if (this->directValueMatch(lexer::As)) {
            this->next();
            this->removeNewLines();
            if (!this->kindMatch(lexer::IdentifierKind)) {
                throw newSyntaxError(this->currentLine(), ExceptBlock);
            }
            captureName = new ast::Identifier(this->currentToken);
            this->next();
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            throw newSyntaxError(this->currentLine(), ExceptBlock);
        }
        std::vector<ast::Node *> exceptBody;
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::End) ||
                    this->directValueMatch(lexer::Except) ||
                    this->directValueMatch(lexer::Else) ||
                    this->directValueMatch(lexer::Finally)) {
                    break;
                }
                continue;
            }
            exceptBody.push_back(this->parseBinaryExpression(0));
        }
        exceptBlocks.push_back(new ast::ExceptBlock(targets, captureName, exceptBody));
    }
    std::vector<ast::Node *> elseBody;
    if (this->directValueMatch(lexer::Else)) {
        this->next();
        if (!this->directValueMatch(lexer::NewLine)) {
            throw newSyntaxError(this->currentLine(), TryStatement);
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::End) || this->directValueMatch(lexer::Finally)) {
                    break;
                }
                continue;
            }
            elseBody.push_back(this->parseBinaryExpression(0));
        }
    }
    std::vector<ast::Node *> finallyBody;
    if (this->directValueMatch(lexer::Finally)) {
        this->next();
        if (!this->directValueMatch(lexer::NewLine)) {
            throw newSyntaxError(this->currentLine(), FinallyBlock);
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            finallyBody.push_back(this->parseBinaryExpression(0));
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newSyntaxError(this->currentLine(), TryStatement);
    }
    this->next();
    return new ast::TryStatement(body, exceptBlocks, elseBody, finallyBody);
}

plasma::ast::BeginStatement *plasma::parser::parser::parseBeginStatement() {
    this->next();
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), BeginStatement);
    }
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), BeginStatement);
    }
    this->next();
    return new ast::BeginStatement(body);
}

plasma::ast::EndStatement *plasma::parser::parser::parseEndStatement() {
    this->next();
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), EndStatement);
    }
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), EndStatement);
    }
    this->next();
    return new ast::EndStatement(body);
}

plasma::ast::ClassStatement *plasma::parser::parser::parseClassStatement() {
    this->next();
    this->removeNewLines();
    if (!this->kindMatch(lexer::IdentifierKind)) {
        throw newSyntaxError(this->currentLine(), ClassStatement);
    }
    ast::Identifier *name = new ast::Identifier(this->currentToken);
    this->next();
    std::vector<ast::Expression *> bases;
    ast::Node *base;
    if (this->directValueMatch(lexer::OpenParentheses)) {
        this->next();
        while (this->hasNext()) {
            this->removeNewLines();
            base = this->parseBinaryExpression(0);
            if (!ast::isExpression(base)) {
                throw newNonExpressionReceivedError(this->currentLine(), ClassStatement);
            }
            bases.push_back(dynamic_cast<ast::Expression *>(base));
            this->removeNewLines();
            if (this->directValueMatch(lexer::Comma)) {
                this->next();
            } else if (this->directValueMatch(lexer::CloseParentheses)) {
                break;
            } else {
                throw newSyntaxError(this->currentLine(), ClassStatement);
            }
        }
        if (!this->directValueMatch(lexer::CloseParentheses)) {
            throw newSyntaxError(this->currentLine(), ClassStatement);
        }
        this->next();
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), ClassStatement);
    }
    this->next();
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::End)) {
            break;
        }
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), ClassStatement);
    }
    this->next();
    return new ast::ClassStatement(
            name,
            bases,
            body
    );
}

plasma::ast::InterfaceStatement *plasma::parser::parser::parseInterfaceStatement() {
    this->next();
    this->removeNewLines();
    if (!this->kindMatch(lexer::IdentifierKind)) {
        throw newSyntaxError(this->currentLine(), InterfaceStatement);
    }
    ast::Identifier *name = new ast::Identifier(this->currentToken);
    this->next();
    std::vector<ast::Expression *> bases;
    ast::Node *base;
    if (this->directValueMatch(lexer::OpenParentheses)) {
        this->next();
        while (this->hasNext()) {
            this->removeNewLines();
            base = this->parseBinaryExpression(0);
            if (!ast::isExpression(base)) {
                throw newNonExpressionReceivedError(this->currentLine(), InterfaceStatement);
            }
            bases.push_back(dynamic_cast<ast::Expression *>(base));
            this->removeNewLines();
            if (this->directValueMatch(lexer::Comma)) {
                this->next();
            } else if (this->directValueMatch(lexer::CloseParentheses)) {
                break;
            } else {
                throw newSyntaxError(this->currentLine(), InterfaceStatement);
            }
        }
        if (!this->directValueMatch(lexer::CloseParentheses)) {
            throw newSyntaxError(this->currentLine(), InterfaceStatement);
        }
        this->next();
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), InterfaceStatement);
    }
    this->removeNewLines();
    std::vector<ast::FunctionDefinitionStatement *> methods;
    ast::Node *node;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::End)) {
            break;
        }
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            continue;
        }
        node = this->parseBinaryExpression(0);
        if (node->TypeID != ast::FunctionDefinitionID) {
            throw newNonFunctionDefinitionReceived(this->currentLine(), InterfaceStatement);
        }
        methods.push_back(dynamic_cast<ast::FunctionDefinitionStatement *>(node));
    }
    this->removeNewLines();
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), InterfaceStatement);
    }
    this->next();
    return new ast::InterfaceStatement(name, bases, methods);
}

plasma::ast::IfStatement *plasma::parser::parser::parseIfStatement() {
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *condition = this->parseBinaryExpression(0);
    if (!ast::isExpression(condition)) {
        throw newNonExpressionReceivedError(this->currentLine(), IfStatement);
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), IfStatement);
    }
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::Elif) ||
                this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    auto rootIf = new ast::IfStatement(
            dynamic_cast<ast::Expression *>(condition),
            body,
            std::vector<ast::Node *>()
    );
    ast::IfStatement *lastIfBlock = rootIf;
    if (this->directValueMatch(lexer::Elif)) {
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::Else) ||
                    this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->directValueMatch(lexer::Elif)) {
                throw newSyntaxError(this->currentLine(), IfStatement);
            }
            this->next();
            this->removeNewLines();
            ast::Node *elifCondition = this->parseBinaryExpression(0);
            if (!ast::isExpression(elifCondition)) {
                throw newNonExpressionReceivedError(this->currentLine(), ElifBlock);
            }
            if (!this->directValueMatch(lexer::NewLine)) {
                throw newSyntaxError(this->currentLine(), ElifBlock);
            }
            std::vector<ast::Node *> elifBody;
            while (this->hasNext()) {
                if (this->kindMatch(lexer::Separator)) {
                    this->next();
                    if (this->directValueMatch(lexer::Elif) ||
                        this->directValueMatch(lexer::Else) ||
                        this->directValueMatch(lexer::End)) {
                        break;
                    }
                    continue;
                }
                elifBody.push_back(this->parseBinaryExpression(0));
            }
            auto newLastIf = new ast::IfStatement(
                    dynamic_cast<ast::Expression *>(elifCondition),
                    elifBody,
                    std::vector<ast::Node *>()
            );
            lastIfBlock->Else.push_back(newLastIf);
            lastIfBlock = newLastIf;
            if (this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::End)) {
                break;
            }
        }
    }
    if (this->directValueMatch(lexer::Else)) {
        this->next();
        if (!this->directValueMatch(lexer::NewLine)) {
            throw newSyntaxError(this->currentLine(), ElseBlock);
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            lastIfBlock->Else.push_back(this->parseBinaryExpression(0));
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), IfStatement);
    }
    this->next();
    return rootIf;
}


plasma::ast::UnlessStatement *plasma::parser::parser::parseUnlessStatement() {
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *condition = this->parseBinaryExpression(0);
    if (!ast::isExpression(condition)) {
        throw newNonExpressionReceivedError(this->currentLine(), UnlessStatement);
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), UnlessStatement);
    }
    std::vector<ast::Node *> body;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            if (this->directValueMatch(lexer::Elif) ||
                this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        body.push_back(this->parseBinaryExpression(0));
    }
    auto rootUnless = new ast::UnlessStatement(
            dynamic_cast<ast::Expression *>(condition),
            body,
            std::vector<ast::Node *>()
    );
    ast::UnlessStatement *lastUnlessBlock = rootUnless;
    if (this->directValueMatch(lexer::Elif)) {
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::Else) ||
                    this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->directValueMatch(lexer::Elif)) {
                throw newSyntaxError(this->currentLine(), UnlessStatement);
            }
            this->next();
            this->removeNewLines();
            ast::Node *elifCondition = this->parseBinaryExpression(0);
            if (!ast::isExpression(elifCondition)) {
                throw newNonExpressionReceivedError(this->currentLine(), ElifBlock);
            }
            if (!this->directValueMatch(lexer::NewLine)) {
                throw newSyntaxError(this->currentLine(), ElifBlock);
            }
            std::vector<ast::Node *> elifBody;
            while (this->hasNext()) {
                if (this->kindMatch(lexer::Separator)) {
                    this->next();
                    if (this->directValueMatch(lexer::Elif) ||
                        this->directValueMatch(lexer::Else) ||
                        this->directValueMatch(lexer::End)) {
                        break;
                    }
                    continue;
                }
                elifBody.push_back(this->parseBinaryExpression(0));
            }
            auto newLastUnless = new ast::UnlessStatement(
                    dynamic_cast<ast::Expression *>(elifCondition),
                    elifBody,
                    std::vector<ast::Node *>()
            );
            lastUnlessBlock->Else.push_back(newLastUnless);
            lastUnlessBlock = newLastUnless;
            if (this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::End)) {
                break;
            }
        }
    }
    if (this->directValueMatch(lexer::Else)) {
        this->next();
        if (!this->directValueMatch(lexer::NewLine)) {
            throw newSyntaxError(this->currentLine(), ElseBlock);
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            lastUnlessBlock->Else.push_back(this->parseBinaryExpression(0));
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newStatementNeverEndedError(this->currentLine(), UnlessStatement);
    }
    this->next();
    return rootUnless;
}

plasma::ast::SwitchStatement *plasma::parser::parser::parseSwitchStatement() {
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *target = this->parseBinaryExpression(0);
    if (!ast::isExpression(target)) {
        throw newNonExpressionReceivedError(this->currentLine(), SwitchStatement);
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        throw newSyntaxError(this->currentLine(), SwitchStatement);
    }
    this->next();
    std::vector<ast::CaseBlock *> caseBlocks;
    if (this->directValueMatch(lexer::Case)) {
        while (this->hasNext()) {
            if (this->directValueMatch(lexer::Default) || this->directValueMatch(lexer::End)) {
                break;
            }
            this->next();
            this->removeNewLines();
            std::vector<ast::Expression *> cases;
            ast::Node *caseTarget;
            while (this->hasNext()) {
                caseTarget = this->parseBinaryExpression(0);
                if (!ast::isExpression(caseTarget)) {
                    throw newNonExpressionReceivedError(this->currentLine(), CaseBlock);
                }
                cases.push_back(dynamic_cast<ast::Expression *>(caseTarget));
                if (this->directValueMatch(lexer::NewLine)) {
                    break;
                } else if (this->directValueMatch(lexer::Comma)) {
                    this->next();
                } else {
                    throw newSyntaxError(line, CaseBlock);
                }
            }
            if (!this->directValueMatch(lexer::NewLine)) {
                throw newSyntaxError(this->currentLine(), CaseBlock);
            }
            std::vector<ast::Node *> caseBody;
            while (this->hasNext()) {
                if (this->kindMatch(lexer::Separator)) {
                    this->next();
                    if (this->directValueMatch(lexer::Case) || this->directValueMatch(lexer::Default) ||
                        this->directValueMatch(lexer::End)) {
                        break;
                    }
                    continue;
                }
                caseBody.push_back(this->parseBinaryExpression(0));
            }
            caseBlocks.push_back(new ast::CaseBlock(cases, caseBody));
        }
    }
    std::vector<ast::Node *> defaultBody;
    if (this->directValueMatch(lexer::Default)) {
        this->next();
        if (!this->directValueMatch(lexer::NewLine)) {
            throw newSyntaxError(this->currentLine(), DefaultBlock);
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                this->next();
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            defaultBody.push_back(this->parseBinaryExpression(0));
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        throw newSyntaxError(this->currentLine(), SwitchStatement);
    }
    this->next();
    return new ast::SwitchStatement(dynamic_cast<ast::Expression *>(target), caseBlocks, defaultBody);
}

plasma::ast::ReturnStatement *plasma::parser::parser::parseReturnStatement() {
    this->next();
    this->removeNewLines();
    std::vector<ast::Expression *> return_results;
    ast::Node *return_result;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator) || this->kindMatch(lexer::EndOfFile)) {
            break;
        }
        int line = this->currentLine();
        return_result = this->parseBinaryExpression(0);
        if (!ast::isExpression(return_result)) {
            throw newNonExpressionReceivedError(line, ReturnStatement);
        }
        return_results.push_back(dynamic_cast<ast::Expression *>(return_result));
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        } else if (!(this->kindMatch(lexer::Separator) || this->kindMatch(lexer::EndOfFile))) {
            throw newSyntaxError(this->currentLine(), ReturnStatement);
        }
    }
    return new ast::ReturnStatement(return_results);
}

plasma::ast::ContinueStatement *plasma::parser::parser::parseContinueStatement() {
    this->next();
    return new ast::ContinueStatement();
}

plasma::ast::BreakStatement *plasma::parser::parser::parseBreakStatement() {
    this->next();
    return new ast::BreakStatement();
}

plasma::ast::RedoStatement *plasma::parser::parser::parseRedoStatement() {
    this->next();
    return new ast::RedoStatement();
}

plasma::ast::PassStatement *plasma::parser::parser::parsePassStatement() {
    this->next();
    return new ast::PassStatement();
}

/*
 * Expressions
 */

plasma::ast::Node *plasma::parser::parser::parseBinaryExpression(uint8_t precedence) {
    ast::Node *leftHandSide = this->parseUnaryExpression();
    if (!ast::isExpression(leftHandSide)) {
        return leftHandSide;
    }
    ast::Node *rightHandSide;
    while (this->hasNext()) {
        if (!this->kindMatch(lexer::Operator) && !this->kindMatch(lexer::Operator)) {
            break;
        }
        this->removeNewLines();
        lexer::token operator_ = this->currentToken;
        uint8_t operatorPrecedence = this->currentToken.directValue;
        if (operatorPrecedence < precedence) {
            return leftHandSide;
        }
        this->next();
        int line = this->currentLine();
        rightHandSide = this->parseBinaryExpression(operatorPrecedence + 1);
        if (!ast::isExpression(rightHandSide)) {
            throw newNonExpressionReceivedError(line, BinaryExpression);
        }
        leftHandSide = new ast::BinaryExpression(dynamic_cast<ast::Expression *>(leftHandSide), operator_,
                                                 dynamic_cast<ast::Expression *>(rightHandSide));
    }
    return leftHandSide;
}

plasma::ast::Node *plasma::parser::parser::parseUnaryExpression() {
    if (this->kindMatch(lexer::Operator)) {
        lexer::token operator_;
        int line;
        ast::Node *x;
        switch (this->currentToken.directValue) {
            case lexer::Sub:
            case lexer::Add:
            case lexer::NegateBits:
            case lexer::SignNot:
            case lexer::Not:
                operator_ = this->currentToken;
                this->next();
                line = this->currentLine();
                x = this->parseUnaryExpression();
                if (!ast::isExpression(x)) {
                    throw newNonExpressionReceivedError(line, PointerExpression);
                }
                return new ast::UnaryExpression(operator_, dynamic_cast<ast::Expression *>(x));
            default:
                break;
        }
    }
    return this->parsePrimaryExpression();
}

plasma::ast::Node *plasma::parser::parser::parsePrimaryExpression() {
    ast::Node *parsedNode = this->parseOperand();
    // FixMe
    while (true) {
        switch (this->currentToken.directValue) {
            case lexer::Dot:
                parsedNode = this->parseSelectorExpression(dynamic_cast<ast::Expression *>(parsedNode));
                break;
            case lexer::OpenParentheses:
                parsedNode = this->parseMethodInvocationExpression(dynamic_cast<ast::Expression *>(parsedNode));
                break;
            case lexer::OpenSquareBracket:
                parsedNode = this->parseIndexExpression(dynamic_cast<ast::Expression *>(parsedNode));
                break;
            case lexer::If:
                parsedNode = this->parseIfOneLinerExpression(dynamic_cast<ast::Expression *>(parsedNode));
                break;
            case lexer::Unless:
                parsedNode = this->parseUnlessOneLinerExpression(dynamic_cast<ast::Expression *>(parsedNode));
                break;
            default:
                if (this->kindMatch(lexer::Assignment)) {
                    parsedNode = this->parseAssignStatement(dynamic_cast<ast::Expression *>(parsedNode));
                }
                goto breakLoop;
        }
    }
    breakLoop:
    return parsedNode;
}

plasma::ast::BasicLiteralExpression *plasma::parser::parser::parseLiteral() {
    if (!this->kindMatch(lexer::Literal) && !this->kindMatch(lexer::Boolean) && !this->kindMatch(lexer::NoneType)) {
        throw newInvalidKindOfTokenError(this->currentLine());
    }
    lexer::token tok;
    switch (this->currentToken.directValue) {
        case lexer::SingleQuoteString:
        case lexer::DoubleQuoteString:
        case lexer::ByteString:
        case lexer::Integer:
        case lexer::HexadecimalInteger:
        case lexer::BinaryInteger:
        case lexer::OctalInteger:
        case lexer::Float:
        case lexer::ScientificFloat:
        case lexer::True:
        case lexer::False:
        case lexer::None:
            tok = this->currentToken;
            this->next();
            return new ast::BasicLiteralExpression(tok);
        default:
            break;
    }
    throw newInvalidKindOfTokenError(this->currentLine());
}

plasma::ast::Node *plasma::parser::parser::parseOperand() {
    lexer::token identifier;
    switch (currentToken.kind) {
        case lexer::Literal:
        case lexer::Boolean:
        case lexer::NoneType:
            return this->parseLiteral();
        case lexer::IdentifierKind:
            identifier = this->currentToken;
            this->next();
            return new ast::Identifier(identifier);
        case lexer::Keyboard:
            switch (this->currentToken.directValue) {
                case lexer::Lambda:
                    return this->parseLambdaExpression();
                case lexer::While:
                    return this->parseWhileStatement();
                case lexer::For:
                    return this->parseForStatement();
                case lexer::Until:
                    return this->parseUntilStatement();
                case lexer::If:
                    return this->parseIfStatement();
                case lexer::Unless:
                    return this->parseUnlessStatement();
                case lexer::Switch:
                    return this->parseSwitchStatement();
                case lexer::Module:
                    return this->parseModuleStatement();
                case lexer::Def:
                    return this->parseFunctionDefinitionStatement();
                case lexer::Interface:
                    return this->parseInterfaceStatement();
                case lexer::Class:
                    return this->parseClassStatement();
                case lexer::Raise:
                    return this->parseRaiseStatement();
                case lexer::Try:
                    return this->parseTryStatement();
                case lexer::Return:
                    return this->parseReturnStatement();
                case lexer::Continue:
                    return this->parseContinueStatement();
                case lexer::Break:
                    return this->parseBreakStatement();
                case lexer::Redo:
                    return this->parseRedoStatement();
                case lexer::Pass:
                    return this->parsePassStatement();
                case lexer::Do:
                    return this->parseDoWhileStatement();
                default:
                    break;
            }
        case lexer::Punctuation:
            switch (this->currentToken.directValue) {
                case lexer::OpenParentheses:
                    return this->parseParenthesesExpression();
                case lexer::OpenSquareBracket:
                    return this->parseArrayExpression();
                case lexer::OpenBrace:
                    return this->parseHashExpression();
                default:
                    break;
            }
        default:
            break;
    }
    // FixMe: Why is this a step behind?
    std::cout << "Token: " << this->currentToken.string << " Kind: " << std::to_string(this->currentToken.kind)
              << " Direct Value: " << std::to_string(this->currentToken.directValue) << std::endl;
    throw error::error(error::LexingError, "Unknown Token", this->currentLine());
}

plasma::ast::LambdaExpression *plasma::parser::parser::parseLambdaExpression() {
    std::vector<ast::Identifier *> arguments;
    this->next();
    int line;
    ast::Node *identifier;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::Colon)) {
            break;
        }
        this->removeNewLines();
        line = this->currentLine();
        identifier = this->parseBinaryExpression(0);
        if (identifier->TypeID != ast::IdentifierID) {
            throw newNonIdentifierReceivedError(line, LambdaExpression);
        }
        arguments.push_back(dynamic_cast<ast::Identifier *>(identifier));
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        } else if (!this->directValueMatch(lexer::Colon)) {
            throw newSyntaxError(line, LambdaExpression);
        }
    }
    this->removeNewLines();
    if (!this->directValueMatch(lexer::Colon)) {
        throw newSyntaxError(this->currentLine(), LambdaExpression);
    }
    this->next();
    this->removeNewLines();
    line = this->currentLine();
    ast::Node *code = this->parseBinaryExpression(0);
    if (!ast::isExpression(code)) {
        throw newNonExpressionReceivedError(line, LambdaExpression);
    }
    std::vector<ast::Expression *> results;
    results.push_back(dynamic_cast<ast::Expression *>(code));
    return new ast::LambdaExpression(
            arguments,
            new ast::ReturnStatement(results)
    );
}

plasma::ast::Node *plasma::parser::parser::parseParenthesesExpression() {
    this->next();
    this->removeNewLines();
    if (this->directValueMatch(lexer::CloseParentheses)) {
        throw newSyntaxError(this->currentLine(), ParenthesesExpression);
    }
    int line = this->currentLine();
    ast::Node *firstExpression = this->parseBinaryExpression(0);
    if (!ast::isExpression(firstExpression)) {
        throw newNonExpressionReceivedError(line, ParenthesesExpression);
    }
    this->removeNewLines();
    if (this->directValueMatch(lexer::For)) {
        return this->parseGeneratorExpression(dynamic_cast<ast::Expression *>(firstExpression));
    }
    if (this->directValueMatch(lexer::CloseParentheses)) {
        this->next();
        return new ast::ParenthesesExpression(dynamic_cast<ast::Expression *>(firstExpression));
    } else if (!this->directValueMatch(lexer::Comma)) {
        throw newSyntaxError(this->currentLine(), ParenthesesExpression);
    }
    std::vector<ast::Expression *> values;
    values.push_back(dynamic_cast<ast::Expression *>(firstExpression));
    this->next();
    ast::Node *nextValue;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        }
        this->removeNewLines();
        line = currentLine();
        nextValue = this->parseBinaryExpression(0);
        if (!ast::isExpression(nextValue)) {
            throw newNonExpressionReceivedError(line, TupleExpression);
        }
        values.push_back(dynamic_cast<ast::Expression *>(nextValue));
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        } else if (!this->directValueMatch(lexer::CloseParentheses)) {
            throw newSyntaxError(this->currentLine(), TupleExpression);
        }
    }
    this->removeNewLines();
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        throw newExpressionNeverClosed(this->currentLine(), TupleExpression);
    }
    this->next();
    return new ast::TupleExpression(values);
}

plasma::ast::ArrayExpression *plasma::parser::parser::parseArrayExpression() {
    this->next();
    this->removeNewLines();
    ast::Node *value;
    std::vector<ast::Expression *> values;
    int line;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseSquareBracket)) {
            break;
        }
        this->removeNewLines();
        line = this->currentLine();
        value = this->parseBinaryExpression(0);
        if (!ast::isExpression(value)) {
            throw newNonExpressionReceivedError(line, ArrayExpression);
        }
        values.push_back(dynamic_cast<ast::Expression *>(value));
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        } else if (!this->directValueMatch(lexer::CloseSquareBracket)) {
            throw newSyntaxError(this->currentLine(), ArrayExpression);
        }
    }
    this->next();
    return new ast::ArrayExpression(values);
}

plasma::ast::HashExpression *plasma::parser::parser::parseHashExpression() {
    this->next();
    this->removeNewLines();
    int line;
    std::vector<ast::KeyValue *> keyValues;
    ast::Node *leftHandSide;
    ast::Node *rightHandSide;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseBrace)) {
            break;
        }
        this->removeNewLines();
        line = this->currentLine();
        leftHandSide = this->parseBinaryExpression(0);
        if (!ast::isExpression(leftHandSide)) {
            throw newNonExpressionReceivedError(line, HashExpression);
        }
        this->removeNewLines();
        if (!this->directValueMatch(lexer::Colon)) {
            throw newSyntaxError(this->currentLine(), HashExpression);
        }
        this->next();
        this->removeNewLines();
        line = this->currentLine();
        rightHandSide = this->parseBinaryExpression(0);
        if (!ast::isExpression(rightHandSide)) {
            throw newNonExpressionReceivedError(this->currentLine(), HashExpression);
        }
        keyValues.push_back(
                new ast::KeyValue(dynamic_cast<ast::Expression *>(leftHandSide),
                                  dynamic_cast<ast::Expression *>(rightHandSide))
        );
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        }
        this->removeNewLines();
    }
    if (!this->directValueMatch(lexer::CloseBrace)) {
        throw newSyntaxError(this->currentLine(), HashExpression);
    }
    this->next();
    return new ast::HashExpression(keyValues);
}

plasma::ast::SelectorExpression *
plasma::parser::parser::parseSelectorExpression(ast::Expression *source) {
    ast::Expression *selector = source;
    lexer::token identifier;
    while (this->hasNext()) {
        if (!this->directValueMatch(lexer::Dot)) {
            break;
        }
        this->next();
        identifier = this->currentToken;
        if (identifier.kind != lexer::IdentifierKind) {
            throw newSyntaxError(this->currentLine(), SelectorExpression);
        }
        selector = new ast::SelectorExpression(selector, new ast::Identifier(identifier));
        this->next();
    }
    return dynamic_cast<ast::SelectorExpression *>(selector);
}

plasma::ast::MethodInvocationExpression *
plasma::parser::parser::parseMethodInvocationExpression(ast::Expression *method) {
    std::vector<ast::Expression *> arguments;
    this->next();
    this->removeNewLines();
    int line;
    ast::Node *argument;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        }
        this->removeNewLines();
        line = this->currentLine();
        argument = this->parseBinaryExpression(0);
        if (!ast::isExpression(argument)) {
            throw newNonExpressionReceivedError(line, MethodInvocationExpression);
        }
        arguments.push_back(dynamic_cast<ast::Expression *>(argument));
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        }
    }
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        throw newSyntaxError(this->currentLine(), MethodInvocationExpression);
    }
    this->next();
    return new ast::MethodInvocationExpression(method, arguments);
}

plasma::ast::IndexExpression *plasma::parser::parser::parseIndexExpression(ast::Expression *source) {
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *index = this->parseBinaryExpression(0);
    if (!ast::isExpression(index)) {
        throw newNonExpressionReceivedError(line, IndexExpression);
    }
    this->removeNewLines();
    if (!this->directValueMatch(lexer::CloseSquareBracket)) {
        throw newSyntaxError(this->currentLine(), IndexExpression);
    }
    this->next();
    return new ast::IndexExpression(
            source,
            dynamic_cast<ast::Expression *>(index)
    );
}

plasma::ast::IfOneLinerExpression *
plasma::parser::parser::parseIfOneLinerExpression(ast::Expression *onTrueResult) {
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *condition = this->parseBinaryExpression(0);
    if (!ast::isExpression(condition)) {
        throw newNonExpressionReceivedError(line, IfOneLinerExpression);
    }
    if (!this->directValueMatch(lexer::Else)) {
        return new ast::IfOneLinerExpression(onTrueResult, dynamic_cast<ast::Expression *>(condition));
    }
    this->next();
    this->removeNewLines();
    line = this->currentLine();
    ast::Node *elseResult = this->parseBinaryExpression(0);
    if (!ast::isExpression(elseResult)) {
        throw newNonExpressionReceivedError(line, OneLineElseBlock);
    }
    return new ast::IfOneLinerExpression(
            onTrueResult,
            dynamic_cast<ast::Expression *>(condition),
            dynamic_cast<ast::Expression *>(elseResult)
    );
}

plasma::ast::UnlessOneLinerExpression *
plasma::parser::parser::parseUnlessOneLinerExpression(ast::Expression *onFalseResult) {
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *condition = this->parseBinaryExpression(0);
    if (!ast::isExpression(condition)) {
        throw newNonExpressionReceivedError(line, UnlessOneLinerExpression);
    }
    if (!this->directValueMatch(lexer::Else)) {
        return new ast::UnlessOneLinerExpression(onFalseResult, dynamic_cast<ast::Expression *>(condition));
    }
    this->next();
    this->removeNewLines();
    line = this->currentLine();
    ast::Node *elseResult = this->parseBinaryExpression(0);
    if (!ast::isExpression(elseResult)) {
        throw newNonExpressionReceivedError(line, OneLineElseBlock);
    }
    return new ast::UnlessOneLinerExpression(
            onFalseResult,
            dynamic_cast<ast::Expression *>(condition),
            dynamic_cast<ast::Expression *>(elseResult)
    );
}

plasma::ast::GeneratorExpression *plasma::parser::parser::parseGeneratorExpression(ast::Expression *expression) {
    this->next();
    this->removeNewLines();
    std::vector<ast::Identifier *> variables;
    int numberOfVariables = 0;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::In)) {
            break;
        }
        this->removeNewLines();
        if (!this->kindMatch(lexer::IdentifierKind)) {
            throw newNonIdentifierReceivedError(this->currentLine(), GeneratorExpression);
        }
        variables.push_back(new ast::Identifier(this->currentToken));
        numberOfVariables++;
        this->next();
        this->removeNewLines();
        if (this->directValueMatch(lexer::Comma)) {
            this->next();
        }
    }
    if (numberOfVariables == 0) {
        throw newSyntaxError(this->currentLine(), GeneratorExpression);
    }
    if (!this->directValueMatch(lexer::In)) {
        throw newSyntaxError(this->currentLine(), GeneratorExpression);
    }
    this->next();
    this->removeNewLines();
    int line = this->currentLine();
    ast::Node *source = this->parseBinaryExpression(0);
    if (!ast::isExpression(source)) {
        throw newNonExpressionReceivedError(line, GeneratorExpression);
    }
    this->removeNewLines();
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        throw newSyntaxError(line, GeneratorExpression);
    }
    this->next();
    return new ast::GeneratorExpression(expression, variables, dynamic_cast<ast::Expression *>(source));
}

plasma::ast::Program *plasma::parser::parser::parse() {
    auto result = new ast::Program();
    this->next();
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            this->next();
            continue;
        }
        if (this->directValueMatch(lexer::BEGIN)) {
            result->Begin = dynamic_cast<ast::BeginStatement *>(this->parseBeginStatement());
        } else if (this->directValueMatch(lexer::END)) {
            result->End = std::any_cast<ast::EndStatement *>(this->parseEndStatement());
        } else {
            result->Body.push_back(this->parseBinaryExpression(0));
        }
    }
    return result;
}