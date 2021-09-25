#include <vector>
#include <string>
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "reader.h"
#include "plasma_error.h"
#include "print.h"
#include "test_parser.h"


std::string replace(std::string string, const std::string &old, const std::string &new_pattern);

/*
 * Reconstruct expressions
 */
static std::string reconstruct_binary_expression(plasma::ast::BinaryExpression *x);

static std::string reconstruct_lambda_expression(plasma::ast::LambdaExpression *x);

static std::string reconstruct_selector_expression(plasma::ast::SelectorExpression *x);

static std::string reconstruct_method_invocation_expression(plasma::ast::MethodInvocationExpression *x);

static std::string reconstruct_if_one_liner_expression(plasma::ast::IfOneLinerExpression *x);

static std::string reconstruct_unless_one_liner_expression(plasma::ast::UnlessOneLinerExpression *x);

static std::string reconstruct_unary_expression(plasma::ast::UnaryExpression *x);

static std::string reconstruct_index_expression(plasma::ast::IndexExpression *x);

static std::string reconstruct_array_expression(plasma::ast::ArrayExpression *x);

static std::string reconstruct_tuple_expression(plasma::ast::TupleExpression *x);

static std::string reconstruct_hash_expression(plasma::ast::HashExpression *x);

static std::string reconstruct_parentheses_expression(plasma::ast::ParenthesesExpression *x);

static std::string reconstruct_generator_expression(plasma::ast::GeneratorExpression *x);

/*
 * Reconstruct statements
 */
static std::string reconstruct_assign_statement(plasma::ast::AssignStatement &s);

static std::string reconstruct_return_statement(plasma::ast::ReturnStatement *x);

static std::string reconstruct_while_statement(plasma::ast::WhileStatement *x);

static std::string reconstruct_do_while_statement(plasma::ast::DoWhileStatement *x);

static std::string reconstruct_until_statement(plasma::ast::UntilStatement *x);

static std::string reconstruct_for_statement(plasma::ast::ForStatement *x);

static std::string reconstruct_switch_statement(plasma::ast::SwitchStatement *x);

static std::string reconstruct_try_statement(plasma::ast::TryStatement *x);

static std::string reconstruct_function_statement(plasma::ast::FunctionDefinitionStatement *x);

static std::string reconstruct_raise_statement(plasma::ast::RaiseStatement *x);

static std::string reconstruct_begin_statement(plasma::ast::BeginStatement *x);

static std::string reconstruct_end_statement(plasma::ast::EndStatement *x);

static std::string reconstruct_class_statement(plasma::ast::ClassStatement *x);

static std::string reconstruct_interface_statement(plasma::ast::InterfaceStatement *x);

static std::string reconstruct_module_statement(plasma::ast::ModuleStatement *x);

/*
 * Reconstruct nodes
 */
static std::string reconstruct_node(plasma::ast::Node *node);

//

std::string replace(std::string string, const std::string &old, const std::string &new_pattern) {
    size_t index = 0;
    while (true) {
        /* Locate the substring to replace. */
        index = string.find(old, index);
        if (index == std::string::npos) break;

        /* Make the replacement. */
        string.replace(index, old.length(), new_pattern);

        /* Advance index forward so the next iteration doesn't pick it up as well. */
        index += 3;
    }
    return string;
}

static std::vector<std::string> initialize_parser_tests() {
    std::vector<std::string> tests;
    tests.emplace_back("1 + 2 * 3");
    tests.emplace_back("1 * 2 + 3");
    tests.emplace_back("1 >= 2 == 3 - 4 + 5 - 6 / 7 / 8 ** 9 - 10");
    tests.emplace_back("5 - -5");
    tests.emplace_back("hello.world.carro");
    tests.emplace_back("1.4.hello.world()");
    tests.emplace_back("hello(1)");
    tests.emplace_back("'Hello world'.index(str(12345)[010])");
    tests.emplace_back("'Hello world'.index(str(12345)[(0, 10)])");
    tests.emplace_back("lambda x, y, z: print(x, y - z)");
    tests.emplace_back("lambda x: print((1 + 2) * 3)");
    tests.emplace_back("(1, 2, (3, (4, (((4 + 1))))))");
    tests.emplace_back("[1]");
    tests.emplace_back("{1: (1 * 2) / 4, 2: 282}");
    tests.emplace_back("(1 if x < 4 else 2)");
    tests.emplace_back("True");
    tests.emplace_back("not True");
    tests.emplace_back("1 if True else None");
    tests.emplace_back("!True");
    tests.emplace_back("1 unless False else None");
    tests.emplace_back("1 in (1, 2, 3)");
    tests.emplace_back("(1 for a in (3, 4))");
    tests.emplace_back("1\n2\n3\n[4, 5 + 6 != 11]");
    tests.emplace_back("a = 234");
    tests.emplace_back("a[1] ~= 234");
    tests.emplace_back("2.a += [1]");
    tests.emplace_back("a and b");
    tests.emplace_back("a xor b");
    tests.emplace_back("a or not b");
    tests.emplace_back("redo");
    tests.emplace_back("return 1");
    tests.emplace_back("return 1, 2 + 4, lambda x: x + 2, (1, 2, 3, 4)");
    tests.emplace_back("super(1)");
    tests.emplace_back("super(1, 2)");
    tests.emplace_back("super(1, 2, call((1, 2, 3, 4, 2 * (5 - 3))))");
    tests.emplace_back(
            "if a > 2\n\tcall()\nelif a < 2\n\tif a == 0\n\t\tprint(\"\\\"a\\\" is zero\")\n\telse\n\t\tprint(\"\\\"a\\\" is non zero\")\n\tend\nend");
    tests.emplace_back(
            "unless a > 2\n\tcall()\nelif a < 2\n\tif 1 if a < 2 else None\n\t\tprint(\"\\\"a\\\" is zero\")\n\telse\n\t\tprint(\"\\\"a\\\" is non zero\")\n\tend\n\tif 1 == 2\n\t\tprint(2)\n\tend\nend");
    tests.emplace_back(
            "switch Token.Kind\ncase Numeric, CommandOutput\n\tbreak\ncase String\n\tprint(\"I am a String\")\ndefault\n\tprint(\"errors\")\nend");
    tests.emplace_back("while True\n\tif a > b\n\t\tbreak\n\tend\n\ta += 1\n\tb -= 1\nend");
    tests.emplace_back("[]");
    tests.emplace_back("for a, b, c in range(10)\n\tprint(\"hello world!\")\nend");
    tests.emplace_back("until (1 + 2 * 3 / a) > 5\n\tif a > b\n\t\tbreak\n\tend\n\ta += 1\n\tb -= 1\nend");
    tests.emplace_back(
            "module something\n\tclass Hello(a.b, a, c, Hello2)\n\tend\n\tclass Hello2\n\tend\n\tclass Hello3(IHello)\n\tend\n\tinterface IHello\n\t\tdef SayHello()\n\t\t\tprint(\"Hello\")\n\t\tend\n\t\tdef SayHello()\n\t\t\tprint(\"Hello\")\n\t\tend\n\tend\n\tinterface IHello2(IHello)\n\t\tdef SayHello()\n\t\t\tprint(\"Hello\")\n\t\tend\n\t\tdef SayHello()\n\t\t\tprint(\"Hello\")\n\t\tend\n\tend\nend");
    tests.emplace_back("BEGIN\n\tNode = (1, 2)\nend");
    tests.emplace_back("END\n\tNode = (1, 2)\nend");
    tests.emplace_back(
            "try\n\tprint(variable)\nexcept UndefinedIdentifier, AnyException as errors\n\tprint(errors)\nexcept NoToStringException as errors\n\tprint(errors)\nelse\n\tprint(\"Unknown *errors\")\n\traise UnknownException()\nfinally\n\tprint(\"Done\")\nend");
    tests.emplace_back("do\n\tprint(\"Hello\")\nwhile a > b");
    tests.emplace_back(
            "def fib(n)\n\tif n == 0\n\t\treturn 0\n\tend\n\tif n == 1\n\t\treturn 1\n\tend\n\treturn fib(n - 1) + fib(n - 2)\nend\nprintln(fib(35))");
    tests.emplace_back(
            "println((1 + 2 / 3) == 1.6666666666666665)\n"
            "println(25 ** (1 / 2) == 5)\n"
            "println(\"Hello \" * 5 == \"Hello Hello Hello Hello Hello \")\n"
            "println((1, 2, 4 + 5 / 6 ** 2, 10, \"hello * 5 \" * 0) == (1, 2, 4 + 5 / 6 ** 2, 10, \"\"))\n"
            "println(1 and (1, 2, 3, 4))\n"
            "println(1 or (1, 2, 3, 4))\n"
            "println(1 xor (1, 2, 3, 4) == False)\n"
            "println(1 in (1, 2, 3, 4))\n"
            "println(1 // 2 == 0)\n"
            "println((1, 2, 3, \"Hello\") * 2 == (1, 2, 3, \"Hello\", 1, 2, 3, \"Hello\"))\n"
            "println(1 in [1, 2, 3, 4, 5])\n"
            "println(1 in (1, 2, 3, 4, 5))\n"
            "println(1 in {1: 2, 2: 3, 3: 4, 4: 5, 5: 6})\n"
            "println(((1 + 2) / 3) == 1)\n"
            "class A\n"
            "\tdef Equals(other)\n"
            "\t\treturn self.Class() == other.Class()\n"
            "\tend\n"
            "end\n"
            "println((A(), A(), A(), A()) * 2 == (A(), A(), A(), A(), A(), A(), A(), A()))\n"
            "println(1 / 2 == 0.5)\n"
            "println(1 / 2 ** 2 + 5 * 1 - 3 == 2.25)\n"
            "println(1 / 2 == 0.5)"
    );
    return tests;
}

/*
 * Reconstruct expressions
 */

static std::string reconstruct_lambda_expression(plasma::ast::LambdaExpression *x) {
    std::string result = "lambda ";
    bool first = true;
    for (plasma::ast::Identifier *argument : x->Arguments) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(argument);
    }
    result += ": ";
    bool firstRes = true;
    for (plasma::ast::Expression *res :  x->Output->Results) {
        if (firstRes) {
            firstRes = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(res);
    }
    return result;
}

static std::string reconstruct_if_one_liner_expression(plasma::ast::IfOneLinerExpression *x) {
    if (x->ElseResult != nullptr) {
        return reconstruct_node(x->Result) + " if " + reconstruct_node(x->Condition) + " else " +
               reconstruct_node(x->ElseResult);
    }
    return reconstruct_node(x->Result) + " if " + reconstruct_node(x->Condition);
}

static std::string reconstruct_unless_one_liner_expression(plasma::ast::UnlessOneLinerExpression *x) {
    if (x->ElseResult != nullptr) {
        return reconstruct_node(x->Result) + " unless " + reconstruct_node(x->Condition) + " else " +
               reconstruct_node(x->ElseResult);
    }
    return reconstruct_node(x->Result) + " unless " + reconstruct_node(x->Condition);
}

static std::string reconstruct_binary_expression(plasma::ast::BinaryExpression *x) {
    return reconstruct_node(x->LeftHandSide) + " " + x->Operator.string + " " + reconstruct_node(x->RightHandSide);
}

static std::string reconstruct_method_invocation_expression(plasma::ast::MethodInvocationExpression *x) {
    std::string result = reconstruct_node(x->Function) + "(";
    bool first = true;
    for (plasma::ast::Expression *node : x->Arguments) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result + ")";
}

static std::string reconstruct_selector_expression(plasma::ast::SelectorExpression *x) {
    std::string result;
    plasma::ast::SelectorExpression *selectorExpression = x;
    while (selectorExpression->X->TypeID == plasma::ast::SelectorID) {
        result = "." + reconstruct_node(selectorExpression->Identifier) + result;
        selectorExpression = dynamic_cast<plasma::ast::SelectorExpression *>(x->X);
    }
    result = reconstruct_node(selectorExpression->X) + "." + reconstruct_node(selectorExpression->Identifier) + result;
    return result;
}

static std::string reconstruct_unary_expression(plasma::ast::UnaryExpression *x) {
    if (x->Operator.directValue == plasma::lexer::Not) {
        return x->Operator.string + " " + reconstruct_node(x->X);
    }
    return x->Operator.string + reconstruct_node(x->X);
}

static std::string reconstruct_index_expression(plasma::ast::IndexExpression *x) {
    return reconstruct_node(x->Source) + "[" + reconstruct_node(x->Index) + "]";
}

static std::string reconstruct_array_expression(plasma::ast::ArrayExpression *x) {
    std::string result = "[";
    bool first = true;
    for (plasma::ast::Node *node : x->Values) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result + "]";
}

static std::string reconstruct_tuple_expression(plasma::ast::TupleExpression *x) {
    std::string result = "(";
    bool first = true;
    for (plasma::ast::Node *node : x->Values) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result + ")";
}

static std::string reconstruct_hash_expression(plasma::ast::HashExpression *x) {
    std::string result = "{";
    bool first = true;
    for (plasma::ast::KeyValue *keyValue : x->KeyValues) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(keyValue->Key) + ": " + reconstruct_node(keyValue->Value);
    }
    return result + "}";
}

static std::string reconstruct_parentheses_expression(plasma::ast::ParenthesesExpression *x) {
    return "(" + reconstruct_node(x->X) + ")";
}

static std::string reconstruct_generator_expression(plasma::ast::GeneratorExpression *x) {
    std::string receivers;
    bool first = true;
    for (plasma::ast::Identifier *receiver : x->Receivers) {
        if (first) {
            first = false;
        } else {
            receivers += ", ";
        }
        receivers += reconstruct_node(receiver);
    }
    return "(" + reconstruct_node(x->Operation) + " for " + receivers + " in " + reconstruct_node(x->Source) + ")";
}

/*
 * Reconstruct statements
 */
static std::string reconstruct_assign_statement(plasma::ast::AssignStatement *s) {
    return reconstruct_node(s->LeftHandSide) + " " + s->AssignOperator.string + " " +
           reconstruct_node(s->RightHandSide);
}

static std::string reconstruct_return_statement(plasma::ast::ReturnStatement *x) {
    std::string result = "return";
    bool first = true;
    for (plasma::ast::Expression *node : x->Results) {
        if (first) {
            first = false;
            result += " ";
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result;
}

static std::string reconstruct_if_statement(plasma::ast::IfStatement *x) {
    std::string result = "if " + reconstruct_node(x->Condition);
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    for (plasma::ast::ElifBlock *elif : x->ElifBlocks) {
        result += "\nelif " + replace(reconstruct_node(elif->Condition), "\n", "\n\t");
        for (plasma::ast::Node *node : elif->Body) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    if (!x->Else.empty()) {
        result += "\nelse";
        for (plasma::ast::Node *node : x->Else) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    result += "\nend";
    return result;
}

static std::string reconstruct_unless_statement(plasma::ast::UnlessStatement *x) {
    std::string result = "unless " + reconstruct_node(x->Condition);
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    for (plasma::ast::ElifBlock *elif : x->ElifBlocks) {
        result += "\nelif " + reconstruct_node(elif->Condition);
        for (plasma::ast::Node *node : elif->Body) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    if (!x->Else.empty()) {
        result += "\nelse";
        for (plasma::ast::Node *node : x->Else) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    result += "\nend";
    return result;
}

static std::string reconstruct_while_statement(plasma::ast::WhileStatement *x) {
    std::string result = "while " + reconstruct_node(x->Condition);
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_do_while_statement(plasma::ast::DoWhileStatement *x) {
    std::string result = "do";
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nwhile " + reconstruct_node(x->Condition);
}

static std::string reconstruct_until_statement(plasma::ast::UntilStatement *x) {
    std::string result = "until " + reconstruct_node(x->Condition);
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_for_statement(plasma::ast::ForStatement *x) {
    std::string result = "for ";
    bool first = true;
    for (plasma::ast::Identifier *receiver : x->Receivers) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(receiver);
    }
    result += " in " + reconstruct_node(x->Source);
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_switch_statement(plasma::ast::SwitchStatement *x) {
    std::string result = "switch " + reconstruct_node(x->Target);
    for (plasma::ast::CaseBlock *node : x->CaseBlocks) {
        result += "\ncase";
        bool first = true;
        for (plasma::ast::Node *node2 : node->Cases) {
            if (first) {
                first = false;
                result += " ";
            } else {
                result += ", ";
            }
            result += reconstruct_node(node2);
        }
        for (plasma::ast::Node *node2 : node->Body) {
            result += "\n\t" + replace(reconstruct_node(node2), "\n", "\n\t");
        }
    }
    if (!x->Default.empty()) {
        result += "\ndefault";
        for (plasma::ast::Node *node : x->Default) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    return result + "\nend";
}

static std::string reconstruct_try_statement(plasma::ast::TryStatement *x) {
    std::string result = "try";
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    for (plasma::ast::ExceptBlock *node : x->ExceptBlocks) {
        result += "\nexcept";
        bool first = true;
        for (plasma::ast::Node *node2 : node->Targets) {
            if (first) {
                first = false;
                result += " ";
            } else {
                result += ", ";
            }
            result += reconstruct_node(node2);
        }
        if (!node->CaptureName->Token.string.empty()) {
            result += " as " + reconstruct_node(node->CaptureName);
        }
        for (plasma::ast::Node *node2 : node->Body) {
            result += "\n\t" + replace(reconstruct_node(node2), "\n", "\n\t");
        }
    }
    if (!x->Else.empty()) {
        result += "\nelse";
        for (plasma::ast::Node *node : x->Else) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    if (!x->Finally.empty()) {
        result += "\nfinally";
        for (plasma::ast::Node *node : x->Finally) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    return result + "\nend";
}

static std::string reconstruct_function_statement(plasma::ast::FunctionDefinitionStatement *x) {
    std::string result = "def " + reconstruct_node(x->Name) + "(";
    bool first = true;
    for (plasma::ast::Identifier *argument : x->Arguments) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(argument);
    }
    result += ")";
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_raise_statement(plasma::ast::RaiseStatement *x) {
    std::string result = "raise";
    if (x->X != nullptr) {
        result += " " + reconstruct_node(x->X);
    }
    return result;
}

static std::string reconstruct_begin_statement(plasma::ast::BeginStatement *x) {
    std::string result = "BEGIN";
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_end_statement(plasma::ast::EndStatement *x) {
    std::string result = "END";
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_class_statement(plasma::ast::ClassStatement *x) {
    std::string result = "class " + reconstruct_node(x->Name);
    if (!x->Bases.empty()) {
        result += "(";
        bool first = true;
        for (plasma::ast::Node *base : x->Bases) {
            if (first) {
                first = false;
            } else {
                result += ", ";
            }
            result += reconstruct_node(base);
        }
        result += ")";
    }
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_interface_statement(plasma::ast::InterfaceStatement *x) {
    std::string result = "interface " + reconstruct_node(x->Name);
    if (!x->Bases.empty()) {
        result += "(";
        bool first = true;
        for (plasma::ast::Node *base : x->Bases) {
            if (first) {
                first = false;
            } else {
                result += ", ";
            }
            result += reconstruct_node(base);
        }
        result += ")";
    }
    for (plasma::ast::FunctionDefinitionStatement *node : x->MethodDefinitions) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}


static std::string reconstruct_module_statement(plasma::ast::ModuleStatement *x) {
    std::string result = "module " + reconstruct_node(x->Name);
    for (plasma::ast::Node *node : x->Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

/*
 * Reconstruct nodes
 */
static std::string reconstruct_node(plasma::ast::Node *node) {
    switch (node->TypeID) {
        case plasma::ast::BinaryID:
            return reconstruct_binary_expression(dynamic_cast<plasma::ast::BinaryExpression *>(node));
        case plasma::ast::SelectorID:
            return reconstruct_selector_expression(dynamic_cast<plasma::ast::SelectorExpression *>(node));
        case plasma::ast::UnaryID:
            return reconstruct_unary_expression(dynamic_cast<plasma::ast::UnaryExpression *>(node));
        case plasma::ast::BasicLiteralID:
            return dynamic_cast<plasma::ast::BasicLiteralExpression *>(node)->Token.string;
        case plasma::ast::IdentifierID:
            return dynamic_cast<plasma::ast::Identifier *>(node)->Token.string;
        case plasma::ast::ArrayID:
            return reconstruct_array_expression(dynamic_cast<plasma::ast::ArrayExpression *>(node));
        case plasma::ast::TupleID:
            return reconstruct_tuple_expression(dynamic_cast<plasma::ast::TupleExpression *>(node));
        case plasma::ast::HashID:
            return reconstruct_hash_expression(dynamic_cast<plasma::ast::HashExpression *>(node));
        case plasma::ast::MethodInvocationID:
            return reconstruct_method_invocation_expression(
                    dynamic_cast<plasma::ast::MethodInvocationExpression *>(node));
        case plasma::ast::IfOneLinerID:
            return reconstruct_if_one_liner_expression(dynamic_cast<plasma::ast::IfOneLinerExpression *>(node));
        case plasma::ast::UnlessOneLinerID:
            return reconstruct_unless_one_liner_expression(dynamic_cast<plasma::ast::UnlessOneLinerExpression *>(node));
        case plasma::ast::AssignID:
            return reconstruct_assign_statement(dynamic_cast<plasma::ast::AssignStatement *>(node));
        case plasma::ast::IndexID:
            return reconstruct_index_expression(dynamic_cast<plasma::ast::IndexExpression *>(node));
        case plasma::ast::LambdaID:
            return reconstruct_lambda_expression(dynamic_cast<plasma::ast::LambdaExpression *>(node));
        case plasma::ast::BreakID:
            return "break";
        case plasma::ast::RedoID:
            return "redo";
        case plasma::ast::ContinueID:
            return "continue";
        case plasma::ast::RaiseID:
            return reconstruct_raise_statement(dynamic_cast<plasma::ast::RaiseStatement *>(node));
        case plasma::ast::ParenthesesID:
            return reconstruct_parentheses_expression(dynamic_cast<plasma::ast::ParenthesesExpression *>(node));
        case plasma::ast::GeneratorID:
            return reconstruct_generator_expression(dynamic_cast<plasma::ast::GeneratorExpression *>(node));
        case plasma::ast::ReturnID:
            return reconstruct_return_statement(dynamic_cast<plasma::ast::ReturnStatement *>(node));
        case plasma::ast::IfID:
            return reconstruct_if_statement(dynamic_cast<plasma::ast::IfStatement *>(node));
        case plasma::ast::UnlessID:
            return reconstruct_unless_statement(dynamic_cast<plasma::ast::UnlessStatement *>(node));
        case plasma::ast::WhileID:
            return reconstruct_while_statement(dynamic_cast<plasma::ast::WhileStatement *>(node));
        case plasma::ast::DoWhileID:
            return reconstruct_do_while_statement(dynamic_cast<plasma::ast::DoWhileStatement *>(node));
        case plasma::ast::UntilID:
            return reconstruct_until_statement(dynamic_cast<plasma::ast::UntilStatement *>(node));
        case plasma::ast::ForID:
            return reconstruct_for_statement(dynamic_cast<plasma::ast::ForStatement *>(node));
        case plasma::ast::SwitchID:
            return reconstruct_switch_statement(dynamic_cast<plasma::ast::SwitchStatement *>(node));
        case plasma::ast::TryID:
            return reconstruct_try_statement(dynamic_cast<plasma::ast::TryStatement *>(node));
        case plasma::ast::FunctionDefinitionID:
            return reconstruct_function_statement(dynamic_cast<plasma::ast::FunctionDefinitionStatement *>(node));
        case plasma::ast::BeginID:
            return reconstruct_begin_statement(dynamic_cast<plasma::ast::BeginStatement *>(node));
        case plasma::ast::EndID:
            return reconstruct_end_statement(dynamic_cast<plasma::ast::EndStatement *>(node));
        case plasma::ast::ClassID:
            return reconstruct_class_statement(dynamic_cast<plasma::ast::ClassStatement *>(node));
        case plasma::ast::InterfaceID:
            return reconstruct_interface_statement(dynamic_cast<plasma::ast::InterfaceStatement *>(node));
        case plasma::ast::ModuleID:
            return reconstruct_module_statement(dynamic_cast<plasma::ast::ModuleStatement *>(node));
    }
    throw std::exception("error");
}

static std::string reconstruct_code(plasma::ast::Program *program) {
    std::string result;
    if (program->Begin != nullptr) {
        if (!program->Begin->Body.empty()) {
            result += reconstruct_node(program->Begin);
        }
    }
    bool first = true;
    for (plasma::ast::Node *node : program->Body) {
        if (first) {
            first = false;
        } else {
            result += "\n";
        }
        result += reconstruct_node(node);
    }
    if (program->End != nullptr) {
        if (!program->End->Body.empty()) {
            result += reconstruct_node(program->End);
        }
    }
    return result;
}

static int check_parser_result(plasma::ast::Program *result, const std::string &code) {
    std::string reconstructed = reconstruct_code(result);
    if (reconstructed == code) {
        SUCCESS(code);
        return 1;
    }
    FAIL(reconstructed + " != " + code);
    return 0;
}

void test_parser(int *number_of_tests, int *success) {
    std::vector<std::string> parsing_tests = initialize_parser_tests();
    int number_of_parsing_tests = parsing_tests.size();
    int number_of_success_parsing_tests = 0;
    for (std::string &test : parsing_tests) {
        // Setup lexer
        plasma::reader::string_reader test_reader;
        plasma::reader::string_reader_new(&test_reader, test);
        plasma::lexer::lexer test_lexer(&test_reader);
        // Setup parser
        plasma::parser::parser test_parser(&test_lexer);
        // Parse
        plasma::ast::Program *result = test_parser.parse();
        number_of_success_parsing_tests += check_parser_result(result, test);
        delete result;
    }

    TEST_FINISH("Parse samples", number_of_parsing_tests, number_of_success_parsing_tests);
    (*number_of_tests) += number_of_parsing_tests;
    (*success) += number_of_success_parsing_tests;
}