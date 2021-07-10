#include <iostream>
#include <utility>
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

bool plasma::parser::parser::next(error::error *result_error) {
    lexer::token result;
    if (!this->Lexer->next(&result, result_error)) {
        return false;
    }
    this->currentToken = result;
    return true;
}

bool plasma::parser::parser::directValueMatch(uint8_t directValue) const {
    return this->currentToken.directValue == directValue;
}

bool plasma::parser::parser::kindMatch(uint8_t kind) const {
    return this->currentToken.kind == kind;
}

bool plasma::parser::parser::removeNewLines(error::error *result_error) {
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
plasma::parser::parser::parseAssignStatement(std::any leftHandSide, std::any *result, error::error *result_error) {
    lexer::token assignmentToken = this->currentToken;
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
    (*result) = ast::AssignStatement{
            .LeftHandSide= leftHandSide,
            .AssignOperator = assignmentToken,
            .RightHandSide = rightHandSide,
    };
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
        receivers.push_back(ast::Identifier{
                .Token = this->currentToken
        });
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
    ast::Identifier name{
            .Token = this->currentToken
    };
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
    ast::Identifier name{
            .Token = this->currentToken
    };
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
        arguments.push_back(ast::Identifier{
                .Token = this->currentToken
        });
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
            if (!this->parseBinaryExpression(0, &target, result_error)) {
                return false;
            }
            if (!ast::isExpression(&target)) {
                newSyntaxError(this->currentLine(), ExceptBlock, result_error);
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
                newSyntaxError(this->currentLine(), ExceptBlock, result_error);
                return false;
            }
            captureName.Token = this->currentToken;
            if (!this->next(result_error)) {
                return false;
            }
        }
        if (!this->directValueMatch(lexer::NewLine)) {
            newSyntaxError(this->currentLine(), ExceptBlock, result_error);
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
            newSyntaxError(this->currentLine(), FinallyBlock, result_error);
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
    ast::Identifier name{
            .Token = this->currentToken
    };
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
    ast::Identifier name{
            .Token = this->currentToken
    };
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
                    if (this->directValueMatch(lexer::Elif) || this->directValueMatch(lexer::Else) ||
                        this->directValueMatch(lexer::End)) {
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
                if (!this->next(result_error)) {
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
                    if (this->directValueMatch(lexer::Elif) || this->directValueMatch(lexer::Else) ||
                        this->directValueMatch(lexer::End)) {
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
                if (!this->next(result_error)) {
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
                    if (!this->next(result_error)) {
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
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::SuperInvocationStatement{arguments};
    return true;
}

bool plasma::parser::parser::parseContinueStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::ContinueStatement{};
    return true;
}

bool plasma::parser::parser::parseBreakStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::BreakStatement{};
    return true;
}

bool plasma::parser::parser::parseRedoStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::RedoStatement{};
    return true;
}

bool plasma::parser::parser::parsePassStatement(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::PassStatement{};
    return true;
}

/*
 * Expressions
 */

bool plasma::parser::parser::parseBinaryExpression(uint8_t precedence, std::any *result, error::error *result_error) {
    std::any leftHandSide;
    std::any rightHandSide;
    if (!this->parseUnaryExpression(&leftHandSide, result_error)) {
        return false;
    }
    if (!ast::isExpression(&leftHandSide)) {
        (*result) = leftHandSide;
        return true;
    }
    while (this->hasNext()) {
        if (!this->kindMatch(lexer::Operator) && !this->kindMatch(lexer::Comparator)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        lexer::token operator_ = this->currentToken;
        uint8_t operatorPrecedence = this->currentToken.directValue;
        if (operatorPrecedence < precedence) {
            (*result) = leftHandSide;
            return true;
        }
        if (!this->next(result_error)) {
            return false;
        }
        int line = this->currentLine();
        if (!this->parseBinaryExpression(operatorPrecedence + 1, &rightHandSide, result_error)) {
            return false;
        }
        if (!ast::isExpression(&rightHandSide)) {
            newNonExpressionReceivedError(line, BinaryExpression, result_error);
            return false;
        }
        leftHandSide = ast::BinaryExpression{
                .LeftHandSide = leftHandSide,
                .Operator = operator_,
                .RightHandSide =  rightHandSide,
        };
    }
    (*result) = leftHandSide;
    return true;
}

bool plasma::parser::parser::parseUnaryExpression(std::any *result, error::error *result_error) {
    if (this->kindMatch(lexer::Operator)) {
        lexer::token operator_;
        int line;
        std::any x;
        switch (this->currentToken.directValue) {
            case lexer::Sub:
            case lexer::Add:
            case lexer::NegateBits:
            case lexer::SignNot:
            case lexer::Not:
                operator_ = this->currentToken;
                if (!this->next(result_error)) {
                    return false;
                }
                line = this->currentLine();
                if (!this->parseUnaryExpression(&x, result_error)) {
                    return false;
                }
                if (!ast::isExpression(&x)) {
                    newNonExpressionReceivedError(line, PointerExpression, result_error);
                    return false;
                }
                (*result) = ast::UnaryExpression{
                        .Operator = operator_,
                        .X = x
                };
                return true;
            default:
                break;
        }
    }
    return this->parsePrimaryExpression(result, result_error);
}

bool plasma::parser::parser::parsePrimaryExpression(std::any *result, error::error *result_error) {
    std::any parsedNode;
    if (!this->parseOperand(&parsedNode, result_error)) {
        return false;
    }
    // FixMe
    while (true) {
        std::any parsedNodeBackup = parsedNode;
        switch (this->currentToken.directValue) {
            case lexer::Dot:
                if (!this->parseSelectorExpression(parsedNodeBackup, &parsedNode, result_error)) {
                    return false;
                }
                break;
            case lexer::OpenParentheses:
                if (!this->parseMethodInvocationExpression(parsedNodeBackup, &parsedNode, result_error)) {
                    return false;
                }
                break;
            case lexer::OpenSquareBracket:
                if (!this->parseIndexExpression(parsedNodeBackup, &parsedNode, result_error)) {
                    return false;
                }
                break;
            case lexer::If:
                if (!this->parseIfOneLinerExpression(parsedNodeBackup, &parsedNode, result_error)) {
                    return false;
                }
                break;
            case lexer::Unless:
                if (!this->parseUnlessOneLinerExpression(parsedNodeBackup, &parsedNode, result_error)) {
                    return false;
                }
                break;
            default:
                if (this->kindMatch(lexer::Assignment)) {
                    if (!this->parseAssignStatement(parsedNodeBackup, &parsedNode, result_error)) {
                        return false;
                    }
                }
                goto breakLoop;
        }
    }
    breakLoop:
    (*result) = parsedNode;
    return true;
}

bool plasma::parser::parser::parseLiteral(std::any *result, error::error *result_error) {
    if (!this->kindMatch(lexer::Literal) && !this->kindMatch(lexer::Boolean) && !this->kindMatch(lexer::NoneType)) {
        newInvalidKindOfTokenError(this->currentLine(), result_error);
        return false;
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
            if (!this->next(result_error)) {
                return false;
            }
            (*result) = ast::BasicLiteralExpression{
                    .Token = tok,
                    .Kind = tok.kind,
                    .DirectValue = tok.directValue
            };
            return true;
        default:
            break;
    }
    newInvalidKindOfTokenError(this->currentLine(), result_error);
    return false;
}

bool plasma::parser::parser::parseOperand(std::any *result, error::error *result_error) {
    lexer::token identifier;
    switch (currentToken.kind) {
        case lexer::Literal:
        case lexer::Boolean:
        case lexer::NoneType:
            return this->parseLiteral(result, result_error);
        case lexer::IdentifierKind:
            identifier = this->currentToken;
            if (!this->next(result_error)) {
                return false;
            }
            (*result) = ast::Identifier{
                    .Token = identifier,
            };
            return true;
        case lexer::Keyboard:
            switch (this->currentToken.directValue) {
                case lexer::Lambda:
                    return this->parseLambdaExpression(result, result_error);
                case lexer::While:
                    return this->parseWhileStatement(result, result_error);
                case lexer::For:
                    return this->parseForStatement(result, result_error);
                case lexer::Until:
                    return this->parseUntilStatement(result, result_error);
                case lexer::If:
                    return this->parseIfStatement(result, result_error);
                case lexer::Unless:
                    return this->parseUnlessStatement(result, result_error);
                case lexer::Switch:
                    return this->parseSwitchStatement(result, result_error);
                case lexer::Module:
                    return this->parseModuleStatement(result, result_error);
                case lexer::Def:
                    return this->parseFunctionDefinitionStatement(result, result_error);
                case lexer::Interface:
                    return this->parseInterfaceStatement(result, result_error);
                case lexer::Class:
                    return this->parseClassStatement(result, result_error);
                case lexer::Raise:
                    return this->parseRaiseStatement(result, result_error);
                case lexer::Try:
                    return this->parseTryStatement(result, result_error);
                case lexer::Return:
                    return this->parseReturnStatement(result, result_error);
                case lexer::Yield:
                    return this->parseYieldStatement(result, result_error);
                case lexer::Super:
                    return this->parseSuperStatement(result, result_error);
                case lexer::Continue:
                    return this->parseContinueStatement(result, result_error);
                case lexer::Break:
                    return this->parseBreakStatement(result, result_error);
                case lexer::Redo:
                    return this->parseRedoStatement(result, result_error);
                case lexer::Pass:
                    return this->parsePassStatement(result, result_error);
                case lexer::Do:
                    return this->parseDoWhileStatement(result, result_error);
                default:
                    break;
            }
        case lexer::Punctuation:
            switch (this->currentToken.directValue) {
                case lexer::OpenParentheses:
                    return this->parseParenthesesExpression(result, result_error);
                case lexer::OpenSquareBracket:
                    return this->parseArrayExpression(result, result_error);
                case lexer::OpenBrace:
                    return this->parseHashExpression(result, result_error);
                default:
                    break;
            }
        default:
            break;
    }
    // FixMe: Why is this a step behind?
    std::cout << this->currentToken.string << std::endl;
    (*result_error) = error::error(error::LexingError, "Unknown Token", this->currentLine());
    return false;
}

bool plasma::parser::parser::parseLambdaExpression(std::any *result, error::error *result_error) {
    std::vector<ast::Identifier> arguments;
    if (!this->next(result_error)) {
        return false;
    }
    int line;
    std::any identifier;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::Colon)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        line = this->currentLine();
        if (!this->parseBinaryExpression(0, &identifier, result_error)) {
            return false;
        }
        if (identifier.type() != typeid(ast::Identifier)) {
            newNonIdentifierReceivedError(line, LambdaExpression, result_error);
            return false;
        }
        arguments.push_back(std::any_cast<ast::Identifier>(identifier));
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        } else if (!this->directValueMatch(lexer::Colon)) {
            newSyntaxError(line, LambdaExpression, result_error);
            return false;
        }
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::Colon)) {
        newSyntaxError(this->currentLine(), LambdaExpression, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    line = this->currentLine();
    std::any code;
    if (!this->parseBinaryExpression(0, &code, result_error)) {
        return false;
    }
    if (!ast::isExpression(&code)) {
        newNonExpressionReceivedError(line, LambdaExpression, result_error);
        return false;
    }
    (*result) = ast::LambdaExpression{
            .Arguments = arguments,
            .Code = code
    };
    return true;
}

bool plasma::parser::parser::parseParenthesesExpression(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (this->directValueMatch(lexer::CloseParentheses)) {
        newSyntaxError(this->currentLine(), ParenthesesExpression, result_error);
        return false;
    }
    int line = this->currentLine();
    std::any firstExpression;
    if (!this->parseBinaryExpression(0, &firstExpression, result_error)) {
        return false;
    }
    if (!ast::isExpression(&firstExpression)) {
        newNonExpressionReceivedError(line, ParenthesesExpression, result_error);
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (this->directValueMatch(lexer::For)) {
        return this->parseGeneratorExpression(firstExpression, result, result_error);
    }
    if (this->directValueMatch(lexer::CloseParentheses)) {
        if (!this->next(result_error)) {
            return false;
        }
        (*result) = ast::ParenthesesExpression{
                .X = firstExpression
        };
        return true;
    } else if (!this->directValueMatch(lexer::Comma)) {
        newSyntaxError(this->currentLine(), ParenthesesExpression, result_error);
        return false;
    }
    std::vector<std::any> values;
    values.push_back(firstExpression);
    if (!this->next(result_error)) {
        return false;
    }
    std::any nextValue;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        line = currentLine();
        if (!this->parseBinaryExpression(0, &nextValue, result_error)) {
            return false;
        }
        if (!ast::isExpression(&nextValue)) {
            newNonExpressionReceivedError(line, TupleExpression, result_error);
            return false;
        }
        values.push_back(nextValue);
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        } else if (!this->directValueMatch(lexer::CloseParentheses)) {
            newSyntaxError(this->currentLine(), TupleExpression, result_error);
            return false;
        }
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        newExpressionNeverClosed(this->currentLine(), TupleExpression, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::TupleExpression{
            .Values = values
    };
    return true;
}

bool plasma::parser::parser::parseArrayExpression(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::any value;
    std::vector<std::any> values;
    int line;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseSquareBracket)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        line = this->currentLine();
        if (!this->parseBinaryExpression(0, &value, result_error)) {
            return false;
        }
        values.push_back(value);
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        } else if (!this->directValueMatch(lexer::CloseSquareBracket)) {
            newSyntaxError(this->currentLine(), ArrayExpression, result_error);
            return false;
        }
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::ArrayExpression{
            .Values = values
    };
    return true;
}

bool plasma::parser::parser::parseHashExpression(std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line;
    std::vector<ast::KeyValue> values;
    std::any leftHandSide;
    std::any rightHandSide;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseBrace)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        line = this->currentLine();
        if (!this->parseBinaryExpression(0, &leftHandSide, result_error)) {
            return false;
        }
        if (!ast::isExpression(&leftHandSide)) {
            newNonExpressionReceivedError(line, HashExpression, result_error);
            return false;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (!this->directValueMatch(lexer::Colon)) {
            newSyntaxError(this->currentLine(), HashExpression, result_error);
            return false;
        }
        if (!this->next(result_error)) {
            return false;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        line = this->currentLine();
        if (!this->parseBinaryExpression(0, &rightHandSide, result_error)) {
            return false;
        }
        if (!ast::isExpression(&rightHandSide)) {
            newNonExpressionReceivedError(this->currentLine(), HashExpression, result_error);
            return false;
        }
        values.push_back(ast::KeyValue{
                .Key = leftHandSide,
                .Value = rightHandSide
        });
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
    }
    if (!this->directValueMatch(lexer::CloseBrace)) {
        newSyntaxError(this->currentLine(), HashExpression, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::HashExpression{
            .Values = values
    };
    return true;
}

bool
plasma::parser::parser::parseSelectorExpression(std::any expression, std::any *result, error::error *result_error) {
    std::any selector = expression;
    lexer::token identifier;
    while (this->hasNext()) {
        if (!this->directValueMatch(lexer::Dot)) {
            break;
        }
        if (!this->next(result_error)) {
            return false;
        }
        identifier = this->currentToken;
        if (identifier.kind != lexer::IdentifierKind) {
            newSyntaxError(this->currentLine(), SelectorExpression, result_error);
            return false;
        }
        selector = ast::SelectorExpression{
                .X = selector,
                .Identifier = ast::Identifier{
                        .Token = identifier
                }
        };
        if (!this->next(result_error)) {
            return false;
        }
    }
    (*result) = selector;
    return true;
}

bool plasma::parser::parser::parseMethodInvocationExpression(std::any expression, std::any *result,
                                                             error::error *result_error) {
    std::vector<std::any> arguments;
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line;
    std::any argument;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::CloseParentheses)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        line = this->currentLine();
        if (!this->parseBinaryExpression(0, &argument, result_error)) {
            return false;
        }
        if (!ast::isExpression(&argument)) {
            newNonExpressionReceivedError(line, MethodInvocationExpression, result_error);
            return false;
        }
        arguments.push_back(argument);
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (this->directValueMatch(lexer::Comma)) {
            if (!this->next(result_error)) {
                return false;
            }
        }
    }
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        newSyntaxError(this->currentLine(), MethodInvocationExpression, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::MethodInvocationExpression{
            .Function = expression,
            .Arguments = arguments
    };
    return true;
}

bool plasma::parser::parser::parseIndexExpression(std::any expression, std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line = this->currentLine();
    std::any index;
    if (!this->parseBinaryExpression(0, &index, result_error)) {
        return false;
    }
    if (!ast::isExpression(&index)) {
        newNonExpressionReceivedError(line, IndexExpression, result_error);
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::CloseSquareBracket)) {
        newSyntaxError(this->currentLine(), IndexExpression, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::IndexExpression{
            .Source = expression,
            .Index = index
    };
    return true;
}

bool
plasma::parser::parser::parseIfOneLinerExpression(std::any expression, std::any *result, error::error *result_error) {
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
        newNonExpressionReceivedError(line, IfOneLinerExpression, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::Else)) {
        (*result) = ast::IfOneLinerExpression{
                .Result = expression,
                .Condition = condition,
        };
        return true;
    }
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    line = this->currentLine();
    std::any elseResult;
    if (!this->parseBinaryExpression(0, &elseResult, result_error)) {
        return false;
    }
    if (!ast::isExpression(&elseResult)) {
        newNonExpressionReceivedError(line, OneLineElseBlock, result_error);
        return false;
    }
    (*result) = ast::IfOneLinerExpression{
            .Result = expression,
            .Condition = condition,
            .ElseResult = elseResult,
    };
    return true;
}

bool plasma::parser::parser::parseUnlessOneLinerExpression(std::any expression, std::any *result,
                                                           error::error *result_error) {
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
        newNonExpressionReceivedError(line, UnlessOneLinerExpression, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::Else)) {
        (*result) = ast::UnlessOneLinerExpression{
                .Result = expression,
                .Condition = condition,
        };
        return true;
    }
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    line = this->currentLine();
    std::any elseResult;
    if (!this->parseBinaryExpression(0, &elseResult, result_error)) {
        return false;
    }
    if (!ast::isExpression(&elseResult)) {
        newNonExpressionReceivedError(line, OneLineElseBlock, result_error);
        return false;
    }
    (*result) = ast::UnlessOneLinerExpression{
            .Result = expression,
            .Condition = condition,
            .ElseResult = elseResult,
    };
    return true;
}

bool
plasma::parser::parser::parseGeneratorExpression(std::any expression, std::any *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    std::vector<ast::Identifier> variables;
    int numberOfVariables = 0;
    while (this->hasNext()) {
        if (this->directValueMatch(lexer::In)) {
            break;
        }
        if (!this->removeNewLines(result_error)) {
            return false;
        }
        if (!this->kindMatch(lexer::IdentifierKind)) {
            newNonIdentifierReceivedError(this->currentLine(), GeneratorExpression, result_error);
            return false;
        }
        variables.push_back(ast::Identifier{
                .Token = this->currentToken
        });
        numberOfVariables++;
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
        }
    }
    if (numberOfVariables == 0) {
        newSyntaxError(this->currentLine(), GeneratorExpression, result_error);
        return false;
    }
    if (!this->directValueMatch(lexer::In)) {
        newSyntaxError(this->currentLine(), GeneratorExpression, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    int line = this->currentLine();
    std::any source;
    if (!this->parseBinaryExpression(0, &source, result_error)) {
        return false;
    }
    if (!ast::isExpression(&source)) {
        newNonExpressionReceivedError(line, GeneratorExpression, result_error);
        return false;
    }
    if (!this->removeNewLines(result_error)) {
        return false;
    }
    if (!this->directValueMatch(lexer::CloseParentheses)) {
        newSyntaxError(line, GeneratorExpression, result_error);
        return false;
    }
    if (!this->next(result_error)) {
        return false;
    }
    (*result) = ast::GeneratorExpression{
            .Operation = expression,
            .Receivers = variables,
            .Source = source
    };
    return true;
}

bool plasma::parser::parser::parse(ast::Program *result, error::error *result_error) {
    if (!this->next(result_error)) {
        return false;
    }
    std::any parsedNode;
    while (this->hasNext()) {
        if (this->kindMatch(lexer::Separator)) {
            if (!this->next(result_error)) {
                return false;
            }
            continue;
        }
        if (this->directValueMatch(lexer::BEGIN)) {
            if (!this->parseBeginStatement(&parsedNode, result_error)) {
                return false;
            }
            result->Begin = std::any_cast<ast::BeginStatement>(parsedNode);
        } else if (this->directValueMatch(lexer::END)) {
            if (!this->parseEndStatement(&parsedNode, result_error)) {
                return false;
            }
            result->End = std::any_cast<ast::EndStatement>(parsedNode);
        } else {
            if (!this->parseBinaryExpression(0, &parsedNode, result_error)) {
                return false;
            }
            result->Body.push_back(parsedNode);
        }
    }
    return true;
}