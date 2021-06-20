#include <string>

#include "reader.h"
#include "error.h"
#include "compiler/lexer.h"

#include "test_lexer.h"
#include "print.h"

static void tokenize_strings(int *number_of_tests, int *success) {
    {
        std::string title = "Double quoted string";
        (*number_of_tests)++;

        std::string testString = "\"Hello World\"";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "\"Hello World\"" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::DoubleQuoteString) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Single quoted string";
        (*number_of_tests)++;

        std::string testString = "'Hello World'";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "'Hello World'" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::SingleQuoteString) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Command output string";
        (*number_of_tests)++;

        std::string testString = "`Hello World`";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "`Hello World`" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::CommandOutput) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
}

static void tokenize_integers(int *number_of_tests, int *success) {
    {
        std::string title = "Basic integer";
        (*number_of_tests)++;

        std::string testString = "10";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "10" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::Integer) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Pretty integer";
        (*number_of_tests)++;

        std::string testString = "1_000_000";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "1_000_000" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::Integer) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
}

static void tokenize_hex_integers(int *number_of_tests, int *success) {
    {
        std::string title = "Basic hexadecimal integer";
        (*number_of_tests)++;

        std::string testString = "0xffff";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "0xffff" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::HexadecimalInteger) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Pretty hexadecimal integer";
        (*number_of_tests)++;

        std::string testString = "0xF_F_F_F";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "0xF_F_F_F" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::HexadecimalInteger) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
}

static void tokenize_bin_integers(int *number_of_tests, int *success) {
    {
        std::string title = "Basic binary integer";
        (*number_of_tests)++;

        std::string testString = "0b01001";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "0b01001" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::BinaryInteger) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Pretty binary integer";
        (*number_of_tests)++;

        std::string testString = "0b0_1_0_0_1";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "0b0_1_0_0_1" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::BinaryInteger) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
}

static void tokenize_oct_integers(int *number_of_tests, int *success) {
    {
        std::string title = "Basic octal integer";
        (*number_of_tests)++;

        std::string testString = "0o01237";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "0o01237" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::OctalInteger) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Pretty octal integer";
        (*number_of_tests)++;

        std::string testString = "0o0_1_2_3_7";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "0o0_1_2_3_7" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::OctalInteger) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
}

static void tokenize_floats(int *number_of_tests, int *success) {
    {
        std::string title = "Basic float";
        (*number_of_tests)++;

        std::string testString = "0.105";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "0.105" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::Float) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Pretty float";
        (*number_of_tests)++;

        std::string testString = "100_000.000_001";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "100_000.000_001" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::Float) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
}

static void tokenize_scientific_floats(int *number_of_tests, int *success) {
    {
        std::string title = "Basic scientific float from integer base";
        (*number_of_tests)++;

        std::string testString = "1e+10";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "1e+10" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::ScientificFloat) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Basic scientific float from float base";
        (*number_of_tests)++;

        std::string testString = "1.5e+10";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "1.5e+10" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::ScientificFloat) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Pretty scientific float from integer base";
        (*number_of_tests)++;

        std::string testString = "1_000e+10";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "1_000e+10" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::ScientificFloat) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
    {
        std::string title = "Pretty scientific float from float base";
        (*number_of_tests)++;

        std::string testString = "1.5_001e+10";
        plasma::reader::string_reader reader;
        plasma::reader::string_reader_new(&reader, testString);
        plasma::lexer::lexer lexer(&reader);

        plasma::error::error error;
        plasma::lexer::token token;

        bool tokenizationSuccess = lexer.next(&token, &error);
        if (tokenizationSuccess && token.string == "1.5_001e+10" && token.kind == plasma::lexer::Literal &&
            token.directValue == plasma::lexer::ScientificFloat) {
            (*success)++;
            SUCCESS(title);
        } else {
            FAIL(title);
        }
    }
}

static void tokenize_literals(int *number_of_tests, int *success) {
    LOG("Tokenize Literals");
    tokenize_strings(number_of_tests, success);
    tokenize_integers(number_of_tests, success);
    tokenize_hex_integers(number_of_tests, success);
    tokenize_bin_integers(number_of_tests, success);
    tokenize_oct_integers(number_of_tests, success);
    tokenize_floats(number_of_tests, success);
    tokenize_scientific_floats(number_of_tests, success);
    TEST_FINISH("Tokenize Literals", *number_of_tests, *success);
}

void test_lexer(int *number_of_tests, int *success) {
    tokenize_literals(number_of_tests, success);
}