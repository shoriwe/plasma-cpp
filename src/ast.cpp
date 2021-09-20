#include "compiler/ast.h"


bool plasma::ast::isExpression(Node *node) {
    switch (node->TypeID) {
        case SelectorID:
        case IndexID:
        case UnaryID:
        case BinaryID:
        case ReturnID:
        case HashID:
        case IdentifierID:
        case ArrayID:
        case TupleID:
        case BasicLiteralID:
        case LambdaID:
        case GeneratorID:
        case MethodInvocationID:
        case IfOneLinerID:
        case UnlessOneLinerID:
        case ParenthesesID:
            return true;
    }
    return false;

}
