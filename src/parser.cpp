#include "compiler/parser.h"

void plasma::parser::newSyntaxError(int line, const std::string &nodeType, error::error *result_error) {
    (*result_error) = error::error(error::SyntaxError, "invalid definition of " + nodeType, line);
}

void plasma::parser::newNonExpressionReceivedError(int line, const std::string &nodeType, error::error *result_error) {
    (*result_error) = error::error(error::SyntaxError, "received a non expression in " + nodeType, line);
}

void plasma::parser::newNonIdentifierReceivedError(int line, const std::string &nodeType, error::error *result_error) {
    (*result_error) = error::error(error::SyntaxError, "received a non identifier in " + nodeType, line);
}

void plasma::parser::newStatementNeverEndedError(int line, const std::string &nodeType, error::error *result_error) {
    (*result_error) = error::error(error::SyntaxError, nodeType + " never ended", line);
}

void plasma::parser::newInvalidKindOfTokenError(int line, error::error *result_error) {
    (*result_error) = error::error(error::ParsingError, "invalid kind of token", line);
}

void plasma::parser::newExpressionNeverClosed(int line, const std::string &nodeType, error::error *result_error) {
    (*result_error) = error::error(error::SyntaxError, nodeType + " never closed", line);
}

void
plasma::parser::newNonFunctionDefinitionReceived(int line, const std::string &nodeType, error::error *result_error) {
    (*result_error) = error::error(error::SyntaxError, "non function definition received in " + nodeType, line);
}

plasma::parser::parser::parser(lexer::lexer *lexer_) {
    this->Lexer = lexer_;
    this->Complete = false;
    this->currentToken = nullptr;
}

int plasma::parser::parser::currentLine() {
    if (this->currentToken == nullptr) {
        return 0;
    }
    return currentToken->line;
}

bool plasma::parser::parser::hasNext() const {
    return !this->Complete;
}

bool plasma::parser::parser::next(error::error *result_error) const {
    return this->Lexer->next(this->currentToken, result_error);
}

bool plasma::parser::parser::directValueMatch(uint8_t directValue) const {
    if (this->currentToken == nullptr) {
        return false;
    }
    return this->currentToken->directValue == directValue;
}

bool plasma::parser::parser::kindMatch(uint8_t kind) const {
    if (this->currentToken == nullptr) {
        return false;
    }
    return this->currentToken->kind == kind;
}

bool plasma::parser::parser::removeNewLines(error::error *result_error) const {
    while (this->directValueMatch(lexer::NewLine)) {
        if (!this->next(result_error)) {
            return false;
        }
    }
    return true;
}

/*
 * Statements
 */

bool
plasma::parser::parser::parseAssignStatement(std::any *leftHandSide, std::any *result, error::error *result_error) {
    lexer::token *assignmentToken = this->currentToken;
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line = this->currentLine();
    std::any rightHandSide;
    if (!this->parseBinaryExpression(0, &rightHandSide, result_error)) {
        return false;
    }
    if (!ast::isExpression(&rightHandSide)) {
        newNonExpressionReceivedError(line, AssignStatement, result_error);
        return false;
    }
    (*result) = ast::AssignStatement{*leftHandSide};
    return true;
}

bool plasma::parser::parser::parseForStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::vector<ast::Identifier> receivers;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::In)) {
            break;
        } else if (!this->kindMatch(lexer::IdentifierKind)) {
            newSyntaxError(this->currentLine(), ForStatement, result_error);
            return false;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        receivers.push_back(ast::Identifier{*this->currentToken});
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        } else if (this->directValueMatch(lexer::In)) {
            break;
        } else {
            newSyntaxError(this->currentLine(), ForStatement, result_error);
            return false;
        }
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (this->directValueMatch(lexer::In)) {
        newSyntaxError(this->currentLine(), ForStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::any source;
    if (!this->parseBinaryExpression(0, &source, result_error)) {
        return false;
    }
    if (!ast::isExpression(&source)) {
        newNonExpressionReceivedError(this->currentLine(), ForStatement, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), ForStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), ForStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::ForStatement{receivers, source, body};
    return true;
}

bool plasma::parser::parser::parseUntilStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::any condition;
    if (!this->parseBinaryExpression(0, &condition, result_error)) {
        return false;
    }
    if (!ast::isExpression(&condition)) {
        newSyntaxError(this->currentLine(), UntilStatement, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), UntilStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;

    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), UntilStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::UntilStatement{condition, body};
    return true;
}

bool plasma::parser::parser::parseWhileStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::any condition;
    if (!this->parseBinaryExpression(0, &condition, result_error)) {
        return false;
    }
    if (!ast::isExpression(&condition)) {
        newSyntaxError(this->currentLine(), WhileStatement, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), WhileStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;

    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), WhileStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::WhileStatement{condition, body};
    return true;
}

bool plasma::parser::parser::parseDoWhileStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::any condition;
    if (!this->parseBinaryExpression(0, &condition, result_error)) {
        return false;
    }
    if (!ast::isExpression(&condition)) {
        newSyntaxError(this->currentLine(), DoWhileStatement, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), DoWhileStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;

    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), DoWhileStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::DoWhileStatement{condition, body};
    return true;
}

bool plasma::parser::parser::parseModuleStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->kindMatch(lexer::IdentifierKind)) {
        newSyntaxError(this->currentLine(), ModuleStatement, result_error);
        return false;
    }
    ast::Identifier name{*this->currentToken};
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), ModuleStatement, result_error);
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), ModuleStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::ModuleStatement{name, body};
    return true;
}

bool plasma::parser::parser::parseFunctionDefinitionStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->kindMatch(lexer::IdentifierKind)) {
        newSyntaxError(this->currentLine(), FunctionDefinitionStatement, result_error);
        return false;
    }
    ast::Identifier name{*this->currentToken};
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::OpenParentheses)) {
        newSyntaxError(this->currentLine(), FunctionDefinitionStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<ast::Identifier> arguments;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (!this->kindMatch(lexer::IdentifierKind)) {
            newSyntaxError(this->currentLine(), FunctionDefinitionStatement, result_error);
            return false;
        }
        arguments.push_back(ast::Identifier{*this->currentToken});
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        } else if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        } else {
            newSyntaxError(this->currentLine(), FunctionDefinitionStatement, result_error);
            return false;
        }
    }
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        newSyntaxError(this->currentLine(), FunctionDefinitionStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), FunctionDefinitionStatement, result_error);
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), FunctionDefinitionStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::FunctionDefinitionStatement{name, arguments, body};
    return true;
}

bool plasma::parser::parser::parseRaiseStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    std::any x;
    if (!this->parseBinaryExpression(0, &x, result_error)) {
        return false;
    }
    if (!ast::isExpression(&x)) {
        newNonExpressionReceivedError(this->currentLine(), RaiseStatement, result_error);
        return false;
    }
    (*result) = ast::RaiseStatement{x};
    return true;
}

bool plasma::parser::parser::parseTryStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), TryStatement, result_error);
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::End) ||
                this->directValueMatch(lexer::Except) ||
                this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::Finally)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    std::vector<ast::ExceptBlock> exceptBlocks;
    while (this->hasNext()) {
        if (!this->directValueMatch(lexer::Except)) {
            break;
        }
        if (!this->next(result_error)) {
            return false;
        }
        std::vector<std::any> targets;
        std::any target;
        while (this->hasNext()) {
            if (this->directValueMatch(lexer::NewLine) ||
                this->directValueMatch(lexer::As)) {
                break;
            }
            if (this->parseBinaryExpression(0, &target, result_error)) {
                return false;
            }
            if (!ast::isExpression(&target)) {
                newSyntaxError(this->currentLine(), TryStatement, result_error);
                return false;
            }
            targets.push_back(target);
            if (this->directValueMatch(lexer::Comma)) {
                if (!this->next(result_error)) {
                    return false;
                }
            }
        }
        ast::Identifier captureName;
        if (this->directValueMatch(lexer::As)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (!this->removeNewLines(result_error)) {
                return false;
            }
            if (!this->kindMatch(lexer::IdentifierKind)) {
                newSyntaxError(this->currentLine(), TryStatement, result_error);
                return false;
            }
            captureName.Token = *this->currentToken;
            if (!this->next(result_error)) {
                return false;
            }
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), TryStatement, result_error);
            return false;
        }
        std::vector<std::any> exceptBody;
        std::any exceptBodyNode;
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::End) ||
                    this->directValueMatch(lexer::Except) ||
                    this->directValueMatch(lexer::Else) ||
                    this->directValueMatch(lexer::Finally)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &exceptBodyNode, result_error)) {
                return false;
            }
            exceptBody.push_back(exceptBodyNode);
        }
        exceptBlocks.push_back(ast::ExceptBlock{targets, captureName, exceptBody});
    }
    std::vector<std::any> elseBody;
    std::any elseBodyNode;
    if (this->directValueMatch(lexer::Else)) {
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), TryStatement, result_error);
            return false;
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::End) || this->directValueMatch(lexer::Finally)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &elseBodyNode, result_error)) {
                return false;
            }
            elseBody.push_back(elseBodyNode);
        }
    }
    std::vector<std::any> finallyBody;
    std::any finallyBodyNode;
    if (this->directValueMatch(lexer::Finally)) {
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), TryStatement, result_error);
            return false;
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &finallyBodyNode, result_error)) {
                return false;
            }
            finallyBody.push_back(finallyBodyNode);
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        newSyntaxError(this->currentLine(), TryStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::TryStatement{body, exceptBlocks, elseBody, finallyBody};
    return true;
}

bool plasma::parser::parser::parseBeginStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), BeginStatement, result_error);
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (!this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), BeginStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::BeginStatement{body};
    return true;
}

bool plasma::parser::parser::parseEndStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), EndStatement, result_error);
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (!this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), EndStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::EndStatement{body};
    return true;
}

bool plasma::parser::parser::parseClassStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->kindMatch(lexer::IdentifierKind)) {
        newSyntaxError(this->currentLine(), ClassStatement, result_error);
        return false;
    }
    ast::Identifier name{*this->currentToken};
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<std::any> bases;
    std::any base;
    if (this->directValueMatch(lexer::OpenParentheses)) {
        if (!this->next(result_error)) {
            return false;
        }
        while (this->hasNext()) {
            if (!this->removeNewLines(result_error)) {
                return false;
            }
            if (!this->parseBinaryExpression(0, &base, result_error)) {
                return false;
            }
            if (!ast::isExpression(&base)) {
                newNonExpressionReceivedError(this->currentLine(), ClassStatement, result_error);
                return false;
            }
            bases.push_back(base);
            if (!this->removeNewLines(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::Comma)) {
                if (!this->next(result_error)) {
                    return false;
                }
            } else if (this->directValueMatch(lexer::CloseParentheses)) {
                break;
            } else {
                newSyntaxError(this->currentLine(), ClassStatement, result_error);
                return false;
            }
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), ClassStatement, result_error);
            return false;
        }
        std::vector<std::any> body;
        std::any bodyNode;
        while (this->hasNext()) {
            if (!this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
                return false;
            }
            body.push_back(bodyNode);
        }
        if (!this->directValueMatch(lexer::End)) {
            newStatementNeverEndedError(this->currentLine(), ClassStatement, result_error);
            return false;
        }
        if (!this->next(result_error)) {
            return false;
        }
        (*result) = ast::ClassStatement{name, bases, body};
        return true;
    }
}

bool plasma::parser::parser::parseInterfaceStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->kindMatch(lexer::IdentifierKind)) {
        newSyntaxError(this->currentLine(), InterfaceStatement, result_error);
        return false;
    }
    ast::Identifier name{*this->currentToken};
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<std::any> bases;
    std::any base;
    if (this->directValueMatch(lexer::OpenParentheses)) {
        if (!this->next(result_error)) {
            return false;
        }
        while (this->hasNext()) {
            if (this->removeNewLines(result_error)) {
                return false;
            }
            if (!this->parseBinaryExpression(0, &base, result_error)) {
                return false;
            }
            if (!ast::isExpression(&base)) {
                newNonExpressionReceivedError(this->currentLine(), InterfaceStatement, result_error);
                return false;
            }
            bases.push_back(base);
            if (!this->removeNewLines(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::Comma)) {
                if (!this->next(result_error)) {
                    return false;
                }
            } else if (this->directValueMatch(lexer::CloseParentheses)) {
                break;
            } else {
                newSyntaxError(this->currentLine(), InterfaceStatement, result_error);
                return false;
            }
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), InterfaceStatement, result_error);
            return false;
        }
        std::vector<ast::FunctionDefinitionStatement> methods;
        std::any node;
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (!this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &node, result_error)) {
                return false;
            }
            if (node.type() != typeid(ast::FunctionDefinitionStatement)) {
                newNonFunctionDefinitionReceived(this->currentLine(), InterfaceStatement, result_error);
                return false;
            }
            methods.push_back(std::any_cast<ast::FunctionDefinitionStatement>(node));
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::End)) {
            newStatementNeverEndedError(this->currentLine(), InterfaceStatement, result_error);
            return false;
        }
        if (!this->next(result_error)) {
            return false;
        }
        (*result) = ast::InterfaceStatement{name, bases, methods};
        return true;
    }
}

bool plasma::parser::parser::parseIfStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line = this->currentLine();
    std::any condition;
    if (!this->parseBinaryExpression(0, &condition, result_error)) {
        return false;
    }
    if (!ast::isExpression(&condition)) {
        newNonExpressionReceivedError(this->currentLine(), IfStatement, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), IfStatement, result_error);
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::Elif) || this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    std::vector<ast::ElifBlock> elifBlocks;
    if (this->directValueMatch(lexer::Elif)) {
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::Else) || this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->directValueMatch(lexer::Elif)) {
                newSyntaxError(this->currentLine(), IfStatement, result_error);
                return false;
            }
            if (!this->next(result_error)) {
                return false;
            }
            if (!this->removeNewLines(result_error)) {
                return false;
            }
            std::any elifCondition;
            if (!this->parseBinaryExpression(0, &elifCondition, result_error)) {
                return false;
            }
            if (!ast::isExpression(&elifCondition)) {
                newNonExpressionReceivedError(this->currentLine(), ElifBlock, result_error);
                return false;
            }
            if (!this->directValueMatch(lexer::NewLine)) {
                newSyntaxError(this->currentLine(), ElifBlock, result_error);
                return false;
            }
            std::vector<std::any> elifBody;
            std::any elifBodyNode;
            while (this->hasNext()) {
                if (this->kindMatch(lexer::Separator)) {
                    if (!this->next(result_error)) {
                        return false;
                    }
                    if (this->directValueMatch(lexer::Else) || this->directValueMatch(lexer::End)) {
                        break;
                    }
                    continue;
                }
                if (!this->parseBinaryExpression(0, &elifBodyNode, result_error)) {
                    return false;
                }
                elifBody.push_back(elifBodyNode);
            }
            elifBlocks.push_back(ast::ElifBlock{elifCondition, elifBody});
            if (this->directValueMatch(lexer::Else) || this->directValueMatch(lexer::End)) {
                break;
            }
        }
    }
    std::vector<std::any> elseBody;
    std::any elseBodyNode;
    if (this->directValueMatch(lexer::Else)) {
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), ElseBlock, result_error);
            return false;
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &elseBodyNode, result_error)) {
                return false;
            }
            elseBody.push_back(elseBodyNode);
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), IfStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::IfStatement{condition, body, elifBlocks, elseBody};
    return true;
}


bool plasma::parser::parser::parseUnlessStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line = this->currentLine();
    std::any condition;
    if (!this->parseBinaryExpression(0, &condition, result_error)) {
        return false;
    }
    if (!ast::isExpression(&condition)) {
        newNonExpressionReceivedError(this->currentLine(), UnlessStatement, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), UnlessStatement, result_error);
        return false;
    }
    std::vector<std::any> body;
    std::any bodyNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            if (this->directValueMatch(lexer::Elif) || this->directValueMatch(lexer::Else) ||
                this->directValueMatch(lexer::End)) {
                break;
            }
            continue;
        }
        if (!this->parseBinaryExpression(0, &bodyNode, result_error)) {
            return false;
        }
        body.push_back(bodyNode);
    }
    std::vector<ast::ElifBlock> elifBlocks;
    if (this->directValueMatch(lexer::Elif)) {
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::Else) || this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->directValueMatch(lexer::Elif)) {
                newSyntaxError(this->currentLine(), UnlessStatement, result_error);
                return false;
            }
            if (!this->next(result_error)) {
                return false;
            }
            if (!this->removeNewLines(result_error)) {
                return false;
            }
            std::any elifCondition;
            if (!this->parseBinaryExpression(0, &elifCondition, result_error)) {
                return false;
            }
            if (!ast::isExpression(&elifCondition)) {
                newNonExpressionReceivedError(this->currentLine(), ElifBlock, result_error);
                return false;
            }
            if (!this->directValueMatch(lexer::NewLine)) {
                newSyntaxError(this->currentLine(), ElifBlock, result_error);
                return false;
            }
            std::vector<std::any> elifBody;
            std::any elifBodyNode;
            while (this->hasNext()) {
                if (this->kindMatch(lexer::Separator)) {
                    if (!this->next(result_error)) {
                        return false;
                    }
                    if (this->directValueMatch(lexer::Else) || this->directValueMatch(lexer::End)) {
                        break;
                    }
                    continue;
                }
                if (!this->parseBinaryExpression(0, &elifBodyNode, result_error)) {
                    return false;
                }
                elifBody.push_back(elifBodyNode);
            }
            elifBlocks.push_back(ast::ElifBlock{elifCondition, elifBody});
            if (this->directValueMatch(lexer::Else) || this->directValueMatch(lexer::End)) {
                break;
            }
        }
    }
    std::vector<std::any> elseBody;
    std::any elseBodyNode;
    if (this->directValueMatch(lexer::Else)) {
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), ElseBlock, result_error);
            return false;
        }
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &elseBodyNode, result_error)) {
                return false;
            }
            elseBody.push_back(elseBodyNode);
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        newStatementNeverEndedError(this->currentLine(), UnlessStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::UnlessStatement{condition, body, elifBlocks, elseBody};
    return true;
}

bool plasma::parser::parser::parseSwitchStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line = this->currentLine();
    std::any target;
    if (!this->parseBinaryExpression(0, &target, result_error)) {
        return false;
    }
    if (!ast::isExpression(&target)) {
        newNonExpressionReceivedError(this->currentLine(), SwitchStatement, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::NewLine)) {
        newSyntaxError(this->currentLine(), SwitchStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    std::vector<ast::CaseBlock> caseBlocks;
    if (this->directValueMatch(lexer::Case)) {
        while (this->hasNext()) {
            if (this->directValueMatch(lexer::Default) || this->directValueMatch(lexer::End)) {
                break;
            }
            if (!this->next(result_error)) {
                return false;
            }
            if (!this->removeNewLines(result_error)) {
                return false;
            }
            std::vector<std::any> cases;
            std::any caseTarget;
            while (this->hasNext()) {
                if (!this->parseBinaryExpression(0, &caseTarget, result_error)) {
                    return false;
                }
                if (!ast::isExpression(&caseTarget)) {
                    newNonExpressionReceivedError(this->currentLine(), CaseBlock, result_error);
                    return false;
                }
                cases.push_back(caseTarget);
                if (this->directValueMatch(lexer::NewLine)) {
                    break;
                } else if (this->directValueMatch(lexer::Comma)) {
                    if (!this->next(result_error)) {
                        return false;
                    }
                } else {
                    newSyntaxError(line, CaseBlock, result_error);
                    return false;
                }
            }
            if (!this->directValueMatch(lexer::NewLine)) {
                newSyntaxError(this->currentLine(), CaseBlock, result_error);
                return false;
            }
            std::vector<std::any> caseBody;
            std::any caseBodyNode;
            while (this->hasNext()) {
                if (this->kindMatch(lexer::Separator)) {
                    if (this->next(result_error)) {
                        return false;
                    }
                    if (this->directValueMatch(lexer::Case) || this->directValueMatch(lexer::Default) ||
                        this->directValueMatch(lexer::End)) {
                        break;
                    }
                    continue;
                }
                if (!this->parseBinaryExpression(0, &caseBodyNode, result_error)) {
                    return false;
                }
                caseBody.push_back(caseBodyNode);
            }
            caseBlocks.push_back(ast::CaseBlock{cases, caseBody});
        }
    }
    std::vector<std::any> defaultBody;
    if (this->directValueMatch(lexer::Default)) {
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), DefaultBlock, result_error);
            return false;
        }
        std::any defaultBodyNode;
        while (this->hasNext()) {
            if (this->kindMatch(lexer::Separator)) {
                if (!this->next(result_error)) {
                    return false;
                }
                if (this->directValueMatch(lexer::End)) {
                    break;
                }
                continue;
            }
            if (!this->parseBinaryExpression(0, &defaultBodyNode, result_error)) {
                return false;
            }
            defaultBody.push_back(defaultBodyNode);
        }
    }
    if (!this->directValueMatch(lexer::End)) {
        newSyntaxError(this->currentLine(), SwitchStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::SwitchStatement{target, caseBlocks, defaultBody};
    return true;
}

bool plasma::parser::parser::parseReturnStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::vector<std::any> return_results;
    std::any return_result;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator) || this->kindMatch(lexer::EndOfFile)) {
            break;
        }
        int line = this->currentLine();
        if (!this->parseBinaryExpression(0, &return_result, result_error)) {
            return false;
        }
        if (!ast::isExpression(&return_result)) {
            newNonExpressionReceivedError(line, ReturnStatement, result_error);
            return false;
        }
        return_results.push_back(return_result);
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        } else if (!this->kindMatch(lexer::Separator) || this->kindMatch(lexer::EndOfFile)) {
            newSyntaxError(this->currentLine(), ReturnStatement, result_error);
            return false;
        }
    }
    (*result) = ast::ReturnStatement{return_results};
    return true;
}

bool plasma::parser::parser::parseYieldStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::vector<std::any> yield_results;
    std::any yield_result;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator) || this->kindMatch(lexer::EndOfFile)) {
            break;
        }
        int line = this->currentLine();
        if (!this->parseBinaryExpression(0, &yield_result, result_error)) {
            return false;
        }
        if (!ast::isExpression(&yield_result)) {
            newNonExpressionReceivedError(line, YieldStatement, result_error);
            return false;
        }
        yield_results.push_back(yield_result);
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        } else if (!this->kindMatch(lexer::Separator) || this->kindMatch(lexer::EndOfFile)) {
            newSyntaxError(this->currentLine(), YieldStatement, result_error);
            return false;
        }
    }
    (*result) = ast::YieldStatement{yield_results};
    return true;
}

bool plasma::parser::parser::parseSuperStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::OpenParentheses)) {
        newSyntaxError(this->currentLine(), SuperStatement, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::vector<std::any> arguments;
    std::any argument;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        int line = this->currentLine();
        if (!this->parseBinaryExpression(0, &argument, result_error)) {
            return false;
        }
        if (!ast::isExpression(&argument)) {
            newNonExpressionReceivedError(line, SuperStatement, result_error);
            return false;
        }
        arguments.push_back(argument);
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::Comma)) {
            break;
        }
        if (!this->next(result_error)) {
            return false;
        }
    }
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        newSyntaxError(this->currentLine(), SuperStatement, result_error);
        return false;
    }
    if  (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::SuperInvocationStatement{arguments};
    return true;
}

bool plasma::parser::parser::parseContinueStatement(std::any *result, error::error *result_error) const {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::ContinueStatement{};
    return true;
}

bool plasma::parser::parser::parseBreakStatement(std::any *result, error::error *result_error) const {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::BreakStatement{};
    return true;
}

bool plasma::parser::parser::parseRedoStatement(std::any *result, error::error *result_error) const {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::RedoStatement{};
    return true;
}

bool plasma::parser::parser::parsePassStatement(std::any *result, error::error *result_error) const {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::PassStatement{};
    return true;
}

/*
 * Expressions
 */