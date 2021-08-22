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
static std::string reconstruct_binary_expression(const plasma::ast::BinaryExpression &x);

static std::string reconstruct_lambda_expression(const plasma::ast::LambdaExpression &x);

static std::string reconstruct_selector_expression(const plasma::ast::SelectorExpression &x);

static std::string reconstruct_method_invocation_expression(const plasma::ast::MethodInvocationExpression &x);

static std::string reconstruct_if_one_liner_expression(const plasma::ast::IfOneLinerExpression &x);

static std::string reconstruct_unless_one_liner_expression(const plasma::ast::UnlessOneLinerExpression &x);

static std::string reconstruct_unary_expression(const plasma::ast::UnaryExpression &x);

static std::string reconstruct_index_expression(const plasma::ast::IndexExpression &x);

static std::string reconstruct_array_expression(const plasma::ast::ArrayExpression &x);

static std::string reconstruct_tuple_expression(const plasma::ast::TupleExpression &x);

static std::string reconstruct_hash_expression(const plasma::ast::HashExpression &x);

static std::string reconstruct_parentheses_expression(const plasma::ast::ParenthesesExpression &x);

static std::string reconstruct_generator_expression(const plasma::ast::GeneratorExpression &x);

/*
 * Reconstruct statements
 */
static std::string reconstruct_assign_statement(const plasma::ast::AssignStatement &s);

static std::string reconstruct_super_statement(const plasma::ast::SuperInvocationStatement &x);

static std::string reconstruct_yield_statement(const plasma::ast::YieldStatement &x);

static std::string reconstruct_return_statement(const plasma::ast::ReturnStatement &x);

static std::string reconstruct_while_statement(const plasma::ast::WhileStatement &x);

static std::string reconstruct_do_while_statement(const plasma::ast::DoWhileStatement &x);

static std::string reconstruct_until_statement(const plasma::ast::UntilStatement &x);

static std::string reconstruct_for_statement(const plasma::ast::ForStatement &x);

static std::string reconstruct_switch_statement(const plasma::ast::SwitchStatement &x);

static std::string reconstruct_try_statement(const plasma::ast::TryStatement &x);

static std::string reconstruct_function_statement(const plasma::ast::FunctionDefinitionStatement &x);

static std::string reconstruct_raise_statement(const plasma::ast::RaiseStatement &x);

static std::string reconstruct_begin_statement(const plasma::ast::BeginStatement &x);

static std::string reconstruct_end_statement(const plasma::ast::EndStatement &x);

static std::string reconstruct_class_statement(const plasma::ast::ClassStatement &x);

static std::string reconstruct_interface_statement(const plasma::ast::InterfaceStatement &x);

static std::string reconstruct_module_statement(const plasma::ast::ModuleStatement &x);

/*
 * Reconstruct nodes
 */
static std::string reconstruct_node(std::any node);

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
    tests.emplace_back("1 if True");
    tests.emplace_back("!True");
    tests.emplace_back("1 unless False");
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
    tests.emplace_back("yield 1");
    tests.emplace_back("yield 1, 2 + 4, lambda x: x + 2, (1, 2, 3, 4)");
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

static std::string reconstruct_lambda_expression(const plasma::ast::LambdaExpression &x) {
    std::string result = "lambda ";
    bool first = true;
    for (const plasma::ast::Identifier &argument : x.Arguments) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(argument);
    }
    result += ": ";
    bool firstRes = true;
    for (const auto &res :  x.Output.Results) {
        if (firstRes) {
            firstRes = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(res);
    }
    return result;
}

static std::string reconstruct_if_one_liner_expression(const plasma::ast::IfOneLinerExpression &x) {
    if (x.ElseResult.has_value()) {
        return reconstruct_node(x.Result) + " if " + reconstruct_node(x.Condition) + " else " +
               reconstruct_node(x.ElseResult);
    }
    return reconstruct_node(x.Result) + " if " + reconstruct_node(x.Condition);
}

static std::string reconstruct_unless_one_liner_expression(const plasma::ast::UnlessOneLinerExpression &x) {
    if (x.ElseResult.has_value()) {
        return reconstruct_node(x.Result) + " unless " + reconstruct_node(x.Condition) + " else " +
               reconstruct_node(x.ElseResult);
    }
    return reconstruct_node(x.Result) + " unless " + reconstruct_node(x.Condition);
}

static std::string reconstruct_binary_expression(const plasma::ast::BinaryExpression &x) {
    return reconstruct_node(x.LeftHandSide) + " " + x.Operator.string + " " + reconstruct_node(x.RightHandSide);
}

static std::string reconstruct_method_invocation_expression(const plasma::ast::MethodInvocationExpression &x) {
    std::string result = reconstruct_node(x.Function) + "(";
    bool first = true;
    for (const std::any &node : x.Arguments) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result + ")";
}

static std::string reconstruct_selector_expression(const plasma::ast::SelectorExpression &x) {
    std::string result;
    plasma::ast::SelectorExpression selectorExpression = x;
    while (selectorExpression.X.type() == typeid(plasma::ast::SelectorExpression)) {
        result = "." + reconstruct_node(selectorExpression.Identifier) + result;
        selectorExpression = std::any_cast<plasma::ast::SelectorExpression>(x.X);
    }
    result = reconstruct_node(selectorExpression.X) + "." + reconstruct_node(selectorExpression.Identifier) + result;
    return result;
}

static std::string reconstruct_unary_expression(const plasma::ast::UnaryExpression &x) {
    if (x.Operator.directValue == plasma::lexer::Not) {
        return x.Operator.string + " " + reconstruct_node(x.X);
    }
    return x.Operator.string + reconstruct_node(x.X);
}

static std::string reconstruct_index_expression(const plasma::ast::IndexExpression &x) {
    return reconstruct_node(x.Source) + "[" + reconstruct_node(x.Index) + "]";
}

static std::string reconstruct_array_expression(const plasma::ast::ArrayExpression &x) {
    std::string result = "[";
    bool first = true;
    for (const std::any &node : x.Values) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result + "]";
}

static std::string reconstruct_tuple_expression(const plasma::ast::TupleExpression &x) {
    std::string result = "(";
    bool first = true;
    for (const std::any &node : x.Values) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result + ")";
}

static std::string reconstruct_hash_expression(const plasma::ast::HashExpression &x) {
    std::string result = "{";
    bool first = true;
    for (const plasma::ast::KeyValue &keyValue : x.Values) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(keyValue.Key) + ": " + reconstruct_node(keyValue.Value);
    }
    return result + "}";
}

static std::string reconstruct_parentheses_expression(const plasma::ast::ParenthesesExpression &x) {
    return "(" + reconstruct_node(x.X) + ")";
}

static std::string reconstruct_generator_expression(const plasma::ast::GeneratorExpression &x) {
    std::string receivers;
    bool first = true;
    for (const plasma::ast::Identifier &receiver : x.Receivers) {
        if (first) {
            first = false;
        } else {
            receivers += ", ";
        }
        receivers += reconstruct_node(receiver);
    }
    return "(" + reconstruct_node(x.Operation) + " for " + receivers + " in " + reconstruct_node(x.Source) + ")";
}

/*
 * Reconstruct statements
 */
static std::string reconstruct_assign_statement(const plasma::ast::AssignStatement &s) {
    return reconstruct_node(s.LeftHandSide) + " " + s.AssignOperator.string + " " + reconstruct_node(s.RightHandSide);
}

static std::string reconstruct_super_statement(const plasma::ast::SuperInvocationStatement &x) {
    std::string result = "super(";
    bool first = true;
    for (const std::any &node : x.Arguments) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(node);
    }
    return result + ")";
}

static std::string reconstruct_yield_statement(const plasma::ast::YieldStatement &x) {
    std::string result = "yield";
    bool first = true;
    for (const std::any &node : x.Results) {
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

static std::string reconstruct_return_statement(const plasma::ast::ReturnStatement &x) {
    std::string result = "return";
    bool first = true;
    for (const std::any &node : x.Results) {
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

static std::string reconstruct_if_statement(const plasma::ast::IfStatement &x) {
    std::string result = "if " + reconstruct_node(x.Condition);
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    for (const plasma::ast::ElifBlock &elif : x.ElifBlocks) {
        result += "\nelif " + replace(reconstruct_node(elif.Condition), "\n", "\n\t");
        for (const std::any &node : elif.Body) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    if (!x.Else.empty()) {
        result += "\nelse";
        for (const std::any &node : x.Else) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    result += "\nend";
    return result;
}

static std::string reconstruct_unless_statement(const plasma::ast::UnlessStatement &x) {
    std::string result = "unless " + reconstruct_node(x.Condition);
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    for (const plasma::ast::ElifBlock &elif : x.ElifBlocks) {
        result += "\nelif " + reconstruct_node(elif.Condition);
        for (const std::any &node : elif.Body) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    if (!x.Else.empty()) {
        result += "\nelse";
        for (const std::any &node : x.Else) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    result += "\nend";
    return result;
}

static std::string reconstruct_while_statement(const plasma::ast::WhileStatement &x) {
    std::string result = "while " + reconstruct_node(x.Condition);
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_do_while_statement(const plasma::ast::DoWhileStatement &x) {
    std::string result = "do";
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nwhile " + reconstruct_node(x.Condition);
}

static std::string reconstruct_until_statement(const plasma::ast::UntilStatement &x) {
    std::string result = "until " + reconstruct_node(x.Condition);
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_for_statement(const plasma::ast::ForStatement &x) {
    std::string result = "for ";
    bool first = true;
    for (const plasma::ast::Identifier &receiver : x.Receivers) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(receiver);
    }
    result += " in " + reconstruct_node(x.Source);
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_switch_statement(const plasma::ast::SwitchStatement &x) {
    std::string result = "switch " + reconstruct_node(x.Target);
    for (const plasma::ast::CaseBlock &node : x.CaseBlocks) {
        result += "\ncase";
        bool first = true;
        for (const std::any &node2 : node.Cases) {
            if (first) {
                first = false;
                result += " ";
            } else {
                result += ", ";
            }
            result += reconstruct_node(node2);
        }
        for (const std::any &node2 : node.Body) {
            result += "\n\t" + replace(reconstruct_node(node2), "\n", "\n\t");
        }
    }
    if (!x.Default.empty()) {
        result += "\ndefault";
        for (const std::any &node : x.Default) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    return result + "\nend";
}

static std::string reconstruct_try_statement(const plasma::ast::TryStatement &x) {
    std::string result = "try";
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    for (const plasma::ast::ExceptBlock &node : x.ExceptBlocks) {
        result += "\nexcept";
        bool first = true;
        for (const std::any &node2 : node.Targets) {
            if (first) {
                first = false;
                result += " ";
            } else {
                result += ", ";
            }
            result += reconstruct_node(node2);
        }
        if (!node.CaptureName.Token.string.empty()) {
            result += " as " + reconstruct_node(node.CaptureName);
        }
        for (const std::any &node2 : node.Body) {
            result += "\n\t" + replace(reconstruct_node(node2), "\n", "\n\t");
        }
    }
    if (!x.Else.empty()) {
        result += "\nelse";
        for (const std::any &node : x.Else) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    if (!x.Finally.empty()) {
        result += "\nfinally";
        for (const std::any &node : x.Finally) {
            result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
        }
    }
    return result + "\nend";
}

static std::string reconstruct_function_statement(const plasma::ast::FunctionDefinitionStatement &x) {
    std::string result = "def " + reconstruct_node(x.Name) + "(";
    bool first = true;
    for (const plasma::ast::Identifier &argument : x.Arguments) {
        if (first) {
            first = false;
        } else {
            result += ", ";
        }
        result += reconstruct_node(argument);
    }
    result += ")";
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_raise_statement(const plasma::ast::RaiseStatement &x) {
    std::string result = "raise";
    if (x.X.has_value()) {
        result += " " + reconstruct_node(x.X);
    }
    return result;
}

static std::string reconstruct_begin_statement(const plasma::ast::BeginStatement &x) {
    std::string result = "BEGIN";
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_end_statement(const plasma::ast::EndStatement &x) {
    std::string result = "END";
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_class_statement(const plasma::ast::ClassStatement &x) {
    std::string result = "class " + reconstruct_node(x.Name);
    if (!x.Bases.empty()) {
        result += "(";
        bool first = true;
        for (const std::any &base : x.Bases) {
            if (first) {
                first = false;
            } else {
                result += ", ";
            }
            result += reconstruct_node(base);
        }
        result += ")";
    }
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

static std::string reconstruct_interface_statement(const plasma::ast::InterfaceStatement &x) {
    std::string result = "interface " + reconstruct_node(x.Name);
    if (!x.Bases.empty()) {
        result += "(";
        bool first = true;
        for (const std::any &base : x.Bases) {
            if (first) {
                first = false;
            } else {
                result += ", ";
            }
            result += reconstruct_node(base);
        }
        result += ")";
    }
    for (const plasma::ast::FunctionDefinitionStatement &node : x.MethodDefinitions) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}


static std::string reconstruct_module_statement(const plasma::ast::ModuleStatement &x) {
    std::string result = "module " + reconstruct_node(x.Name);
    for (const std::any &node : x.Body) {
        result += "\n\t" + replace(reconstruct_node(node), "\n", "\n\t");
    }
    return result + "\nend";
}

/*
 * Reconstruct nodes
 */
static std::string reconstruct_node(std::any node) {
    if (node.type() == typeid(plasma::ast::BinaryExpression)) {
        return reconstruct_binary_expression(std::any_cast<plasma::ast::BinaryExpression>(node));
    } else if (node.type() == typeid(plasma::ast::SelectorExpression)) {
        return reconstruct_selector_expression(std::any_cast<plasma::ast::SelectorExpression>(node));
    } else if (node.type() == typeid(plasma::ast::UnaryExpression)) {
        return reconstruct_unary_expression(std::any_cast<plasma::ast::UnaryExpression>(node));
    } else if (node.type() == typeid(plasma::ast::BasicLiteralExpression)) {
        return std::any_cast<plasma::ast::BasicLiteralExpression>(node).Token.string;
    } else if (node.type() == typeid(plasma::ast::Identifier)) {
        return std::any_cast<plasma::ast::Identifier>(node).Token.string;
    } else if (node.type() == typeid(plasma::ast::ArrayExpression)) {
        return reconstruct_array_expression(std::any_cast<plasma::ast::ArrayExpression>(node));
    } else if (node.type() == typeid(plasma::ast::TupleExpression)) {
        return reconstruct_tuple_expression(std::any_cast<plasma::ast::TupleExpression>(node));
    } else if (node.type() == typeid(plasma::ast::HashExpression)) {
        return reconstruct_hash_expression(std::any_cast<plasma::ast::HashExpression>(node));
    } else if (node.type() == typeid(plasma::ast::MethodInvocationExpression)) {
        return reconstruct_method_invocation_expression(std::any_cast<plasma::ast::MethodInvocationExpression>(node));
    } else if (node.type() == typeid(plasma::ast::IfOneLinerExpression)) {
        return reconstruct_if_one_liner_expression(std::any_cast<plasma::ast::IfOneLinerExpression>(node));
    } else if (node.type() == typeid(plasma::ast::UnlessOneLinerExpression)) {
        return reconstruct_unless_one_liner_expression(std::any_cast<plasma::ast::UnlessOneLinerExpression>(node));
    } else if (node.type() == typeid(plasma::ast::AssignStatement)) {
        return reconstruct_assign_statement(std::any_cast<plasma::ast::AssignStatement>(node));
    } else if (node.type() == typeid(plasma::ast::IndexExpression)) {
        return reconstruct_index_expression(std::any_cast<plasma::ast::IndexExpression>(node));
    } else if (node.type() == typeid(plasma::ast::LambdaExpression)) {
        return reconstruct_lambda_expression(std::any_cast<plasma::ast::LambdaExpression>(node));
    } else if (node.type() == typeid(plasma::ast::SuperInvocationStatement)) {
        return reconstruct_super_statement(std::any_cast<plasma::ast::SuperInvocationStatement>(node));
    } else if (node.type() == typeid(plasma::ast::BreakStatement)) {
        return "break";
    } else if (node.type() == typeid(plasma::ast::RedoStatement)) {
        return "redo";
    } else if (node.type() == typeid(plasma::ast::ContinueStatement)) {
        return "continue";
    } else if (node.type() == typeid(plasma::ast::RaiseStatement)) {
        return reconstruct_raise_statement(std::any_cast<plasma::ast::RaiseStatement>(node));
    } else if (node.type() == typeid(plasma::ast::ParenthesesExpression)) {
        return reconstruct_parentheses_expression(std::any_cast<plasma::ast::ParenthesesExpression>(node));
    } else if (node.type() == typeid(plasma::ast::GeneratorExpression)) {
        return reconstruct_generator_expression(std::any_cast<plasma::ast::GeneratorExpression>(node));
    } else if (node.type() == typeid(plasma::ast::YieldStatement)) {
        return reconstruct_yield_statement(std::any_cast<plasma::ast::YieldStatement>(node));
    } else if (node.type() == typeid(plasma::ast::ReturnStatement)) {
        return reconstruct_return_statement(std::any_cast<plasma::ast::ReturnStatement>(node));
    } else if (node.type() == typeid(plasma::ast::IfStatement)) {
        return reconstruct_if_statement(std::any_cast<plasma::ast::IfStatement>(node));
    } else if (node.type() == typeid(plasma::ast::UnlessStatement)) {
        return reconstruct_unless_statement(std::any_cast<plasma::ast::UnlessStatement>(node));
    } else if (node.type() == typeid(plasma::ast::WhileStatement)) {
        return reconstruct_while_statement(std::any_cast<plasma::ast::WhileStatement>(node));
    } else if (node.type() == typeid(plasma::ast::DoWhileStatement)) {
        return reconstruct_do_while_statement(std::any_cast<plasma::ast::DoWhileStatement>(node));
    } else if (node.type() == typeid(plasma::ast::UntilStatement)) {
        return reconstruct_until_statement(std::any_cast<plasma::ast::UntilStatement>(node));
    } else if (node.type() == typeid(plasma::ast::ForStatement)) {
        return reconstruct_for_statement(std::any_cast<plasma::ast::ForStatement>(node));
    } else if (node.type() == typeid(plasma::ast::SwitchStatement)) {
        return reconstruct_switch_statement(std::any_cast<plasma::ast::SwitchStatement>(node));
    } else if (node.type() == typeid(plasma::ast::TryStatement)) {
        return reconstruct_try_statement(std::any_cast<plasma::ast::TryStatement>(node));
    } else if (node.type() == typeid(plasma::ast::FunctionDefinitionStatement)) {
        return reconstruct_function_statement(std::any_cast<plasma::ast::FunctionDefinitionStatement>(node));
    } else if (node.type() == typeid(plasma::ast::BeginStatement)) {
        return reconstruct_begin_statement(std::any_cast<plasma::ast::BeginStatement>(node));
    } else if (node.type() == typeid(plasma::ast::EndStatement)) {
        return reconstruct_end_statement(std::any_cast<plasma::ast::EndStatement>(node));
    } else if (node.type() == typeid(plasma::ast::ClassStatement)) {
        return reconstruct_class_statement(std::any_cast<plasma::ast::ClassStatement>(node));
    } else if (node.type() == typeid(plasma::ast::InterfaceStatement)) {
        return reconstruct_interface_statement(std::any_cast<plasma::ast::InterfaceStatement>(node));
    } else if (node.type() == typeid(plasma::ast::ModuleStatement)) {
        return reconstruct_module_statement(std::any_cast<plasma::ast::ModuleStatement>(node));
    }
    return node.type().name();
}

static std::string reconstruct_code(plasma::ast::Program *program) {
    std::string result;
    if (!program->Begin.Body.empty()) {
        result += reconstruct_node(program->Begin);
    }
    bool first = true;
    for (const std::any &node : program->Body) {
        if (first) {
            first = false;
        } else {
            result += "\n";
        }
        result += reconstruct_node(node);
    }
    if (!program->End.Body.empty()) {
        result += reconstruct_node(program->End);
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
    for (const std::string &test : parsing_tests) {
        // Setup lexer
        plasma::reader::string_reader test_reader;
        plasma::reader::string_reader_new(&test_reader, test);
        plasma::lexer::lexer test_lexer(&test_reader);
        // Setup parser
        plasma::parser::parser test_parser(&test_lexer);
        // Parse
        plasma::ast::Program result;
        plasma::error::error result_error;
        if (test_parser.parse(&result, &result_error)) {
            number_of_success_parsing_tests += check_parser_result(&result, test);
        } else {
            FAIL(result_error.string() + " -> " + test);
        }
    }

    TEST_FINISH("Parse samples", number_of_parsing_tests, number_of_success_parsing_tests);
    (*number_of_tests) += number_of_parsing_tests;
    (*success) += number_of_success_parsing_tests;
}