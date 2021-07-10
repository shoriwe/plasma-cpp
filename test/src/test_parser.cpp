#include <vector>
#include <string>
#include "compiler/lexer.h"
#include "compiler/parser.h"
#include "reader.h"
#include "error.h"
#include "print.h"
#include "test_parser.h"

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

/*
 * Reconstruct nodes
 */
static std::string reconstruct_node(std::any node);

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
            "if a > 2\n\tcall()\nelif a < 2\n\tif a == 0\n\t\tprint(\"\\\"a\\\" is zero\")\n\telse\n\t\tprint(\"\\\"a\\\" is non zero\")");
    tests.emplace_back("\n\tend\nend");
    tests.emplace_back(
            "unless a > 2\n\tcall()\nelif a < 2\n\tif 1 if a < 2 else None\n\t\tprint(\"\\\"a\\\" is zero\")\n\telse\n\t\tprint(\"\\\"a\\\" is non zero\")\n\tend\n\tif 1 == 2\n\t\tprint(2)\n\tend\nend");
    tests.emplace_back(
            "switch Token.Kind\ncase Numeric, CommandOutput\n\tbreak\ncase String\n\tprint(\"I am a String\")\ndefault\n\tprint(\"errors\")\nend");
    tests.emplace_back("while True\n\tif a > b\n\t\tbreak\n\tend\n\ta += 1\n\tb -= 1\nend");
    tests.emplace_back("[]");
    tests.emplace_back("for a, b, c in range(10)\n\tprint(\"hello world!\")\nend");
    tests.emplace_back("until (1 + 2 * 3 / a) > 5\n\tif a > b\n\t\tbreak\n\tend\n\ta += 1\n\tb -= 1\nend");
    tests.emplace_back(
            "module something\n\tclass Hello(a.b, a, c, Hello2)\n\tend\n\tclass Hello2(IHello)\n\tend\n\tinterface IHello\n\t\tdef SayHello()\n\t\t\tprint(\"Hello\")\n\t\tend\n\t\tdef SayHello()\n\t\t\tprint(\"Hello\")\n\t\tend\n\tend\nend");
    tests.emplace_back("BEGIN\n\tNode = (1, 2)\nend");
    tests.emplace_back(
            "try\n\tprint(variable)\nexcept UndefinedIdentifier, AnyException as errors\n\tprint(errors)\nexcept NoToStringException as errors\n\tprint(errors)\nelse\n\tprint(\"Unknown *errors\")\n\traise UnknownException()\nfinally\n\tprint(\"Done\")\nend");
    tests.emplace_back("do\n\tprint(\"Hello\")\nwhile a > b");
    tests.emplace_back(
            "def fib(n)\n\tif n == 0\n\t\treturn 0\n\tend\n\tif n == 1\n\t\treturn 1\n\tend\n\treturn fib(n - 1) + fib(n - 2)\nend\nprintln(fib(35))");
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
    return result += ": " + reconstruct_node(x.Code);
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
    } else if (node.type() == typeid(plasma::ast::ParenthesesExpression)) {
        return reconstruct_parentheses_expression(std::any_cast<plasma::ast::ParenthesesExpression>(node));
    } else if (node.type() == typeid(plasma::ast::GeneratorExpression)) {
        return reconstruct_generator_expression(std::any_cast<plasma::ast::GeneratorExpression>(node));
    }
    return node.type().name();
}

static std::string reconstruct_code(plasma::ast::Program *program) {
    std::string result;
    if (!program->Begin.Body.empty()) {
        result += reconstruct_node(program->Begin);
    }
    for (const std::any &node : program->Body) {
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