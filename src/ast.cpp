#include "compiler/ast.h"


bool plasma::ast::isExpression(std::any *node) {
    if (node->type() == typeid(ArrayExpression)) {
        return true;
    } else if (node->type() == typeid(TupleExpression)) {
        return true;
    } else if (node->type() == typeid(HashExpression)) {
        return true;
    } else if (node->type() == typeid(Identifier)) {
        return true;
    } else if (node->type() == typeid(BasicLiteralExpression)) {
        return true;
    } else if (node->type() == typeid(BinaryExpression)) {
        return true;
    } else if (node->type() == typeid(UnaryExpression)) {
        return true;
    } else if (node->type() == typeid(LambdaExpression)) {
        return true;
    } else if (node->type() == typeid(GeneratorExpression)) {
        return true;
    } else if (node->type() == typeid(SelectorExpression)) {
        return true;
    } else if (node->type() == typeid(MethodInvocationExpression)) {
        return true;
    } else if (node->type() == typeid(IndexExpression)) {
        return true;
    } else if (node->type() == typeid(IfOneLinerExpression)) {
        return true;
    } else if (node->type() == typeid(UnlessOneLinerExpression)) {
        return true;
    } else if (node->type() == typeid(ParenthesesExpression)) {
        return true;
    }
    return false;

}