#ifndef PLASMA_LEXER_H
#define PLASMA_LEXER_H

#include <string>
#include <regex>

#include "reader.h"
#include "plasma_error.h"

namespace plasma::lexer {
    const std::regex identifierCheck("^[a-zA-Z_]+[a-zA-Z0-9_]*$");
    const std::regex junkKindCheck("^\\00+$");
    const char CommaChar = ',';
    const char ColonChar = ':';
    const char SemiColonChar = ';';
    const char NewLineChar = '\n';
    const char OpenParenthesesChar = '(';
    const char CloseParenthesesChar = ')';
    const char OpenSquareBracketChar = '[';
    const char CloseSquareBracketChar = ']';
    const char OpenBraceChar = '{';
    const char CloseBraceChar = '}';
    const char DollarSignChar = '$';
    const char DotChar = '.';
    const char BitwiseOrChar = '|';
    const char BitwiseXorChar = '^';
    const char BitwiseAndChar = '&';
    const char AddChar = '+';
    const char SubChar = '-';
    const char StarChar = '*';
    const char DivChar = '/';
    const char ModulusChar = '%';
    const char LessThanChar = '<';
    const char GreatThanChar = '>';
    const char NegateBitsChar = '~';
    const char SignNotChar = '!';
    const char EqualsChar = '=';
    const char WhiteSpaceChar = ' ';
    const char TabChar = '\t';
    const char CommentChar = '#';
    const char BackSlashChar = '\\';
    const char PassString[] = "pass";
    const char EndString[] = "end";
    const char IfString[] = "if";
    const char UnlessString[] = "unless";
    const char ElseString[] = "else";
    const char ElifString[] = "elif";
    const char WhileString[] = "while";
    const char DoString[] = "do";
    const char ForString[] = "for";
    const char UntilString[] = "until";
    const char SwitchString[] = "switch";
    const char CaseString[] = "case";
    const char DefaultString[] = "default";
    const char ReturnString[] = "return";
    const char ContinueString[] = "continue";
    const char BreakString[] = "break";
    const char RedoString[] = "redo";
    const char DeferString[] = "defer";
    const char ModuleString[] = "module";
    const char DefString[] = "def";
    const char LambdaString[] = "lambda";
    const char InterfaceString[] = "interface";
    const char ClassString[] = "class";
    const char TryString[] = "try";
    const char ExceptString[] = "except";
    const char FinallyString[] = "finally";
    const char AndString[] = "and";
    const char OrString[] = "or";
    const char XorString[] = "xor";
    const char InString[] = "in";
    const char AwaitString[] = "await";
    const char BEGINString[] = "BEGIN";
    const char ENDString[] = "END";
    const char NotString[] = "not";
    const char TrueString[] = "True";
    const char FalseString[] = "False";
    const char NoneString[] = "None";
    const char ContextString[] = "context";
    const char RaiseString[] = "raise";
    const char AsString[] = "as";
    enum {
        NotSet, // 0
        Unknown,
        PendingEscape,
        Comment,
        Whitespace,
        Literal,
        Tab,
        IdentifierKind,
        JunkKind,
        Separator,
        Punctuation, // 10
        Assignment,
        Operator,
        SingleQuoteString,
        DoubleQuoteString,
        Integer,
        HexadecimalInteger,
        BinaryInteger,
        OctalInteger,
        Float, // 20
        AwaitKeyboard,
        ScientificFloat,
        CommandOutput,
        ByteString,
        Keyboard,
        Boolean,
        NoneType,
        EndOfFile,
        Comma,
        Colon,
        SemiColon,
        NewLine,
        Pass,
        End,
        If,
        Unless,
        As,
        Raise,
        Else,
        Elif,
        While,
        Do,
        For,
        Until,
        Switch,
        Case,
        Default,
        Return,
        Continue,
        Break,
        Redo,
        Defer,
        Module,
        Def,
        Lambda,
        Interface,
        Class,
        Try,
        Except,
        Finally,
        Await,
        BEGIN,
        END,
        Context,
        Assign,
        NegateBitsAssign,
        BitwiseOrAssign,
        BitwiseXorAssign,
        BitwiseAndAssign,
        BitwiseLeftAssign,
        BitwiseRightAssign,
        AddAssign,
        SubAssign,
        StarAssign,
        DivAssign,
        FloorDivAssign,
        ModulusAssign,
        PowerOfAssign,
        Not,
        SignNot,
        NegateBits,
        And,
        Or,
        Xor,
        In,
        Equals,
        NotEqual,
        GreaterThan,
        GreaterOrEqualThan,
        LessThan,
        LessOrEqualThan,
        BitwiseOr,
        BitwiseXor,
        BitwiseAnd,
        BitwiseLeft,
        BitwiseRight,
        Add,
        Sub,
        Star,
        Div,
        FloorDiv,
        Modulus,
        PowerOf,
        True,
        False,
        None,
        OpenParentheses,
        CloseParentheses,
        OpenSquareBracket,
        CloseSquareBracket,
        OpenBrace,
        CloseBrace,
        DollarSign,
        Dot,
    };

    struct token {
        std::string string;
        uint8_t directValue;
        uint8_t kind;
        int line;
    };

    struct lexer {
        token lastToken{"", NotSet, NotSet, -1};
        int line = 0;
        reader::reader *codeReader;
        bool complete = false;

        explicit lexer(reader::reader *codeReader);

        bool hasNext() const;

        static void guessKind(const std::string &pattern, uint8_t *kind, uint8_t *directValue);

        bool tokenizeStringLikeExpressions(char stringOpener, std::string *content, uint8_t *kind, uint8_t *directValue,
                                           error::error *result_error);

        bool tokenizeHexadecimal(char letterX, std::string *content, uint8_t *kind, uint8_t *directValue,
                                 error::error *result_error) const;

        bool tokenizeBinary(char letterB, std::string *content, uint8_t *kind, uint8_t *directValue,
                            error::error *result_error) const;

        bool tokenizeOctal(char letterO, std::string *content, uint8_t *kind, uint8_t *directValue,
                           error::error *result_error) const;

        bool tokenizeScientificFloat(std::string base, std::string *content, uint8_t *kind, uint8_t *directValue,
                                     error::error *result_error) const;

        bool tokenizeFloat(std::string base, std::string *content, uint8_t *kind, uint8_t *directValue,
                           error::error *result_error) const;

        bool tokenizeInteger(std::string base, std::string *content, uint8_t *kind, uint8_t *directValue,
                             error::error *result_error) const;

        bool tokenizeNumeric(char firstDigit, std::string *content, uint8_t *kind, uint8_t *directValue,
                             error::error *result_error) const;

        bool tokenizeChars(char startingChar, std::string *content, uint8_t *kind, uint8_t *directValue,
                           error::error *result_error);

        bool
        tokenizeComment(std::string *content, uint8_t *kind, uint8_t *directValue, error::error *result_error) const;

        bool tokenizeRepeatableOperator(char op,
                                        uint8_t directValueWhenSingle, uint8_t kindWhenSingle,
                                        uint8_t directValueWhenDouble, uint8_t kindWhenDouble,
                                        uint8_t directValueWhenSingleAssign, uint8_t kindWhenSingleAssign,
                                        uint8_t directValueWhenDoubleAssign, uint8_t kindWhenDoubleAssign,
                                        std::string *content, uint8_t *kind, uint8_t *directValue,
                                        error::error *result_error) const;

        bool tokenizeNotRepeatableOperator(char op,
                                           uint8_t directValueWhenOperator, uint8_t kindWhenOperator,
                                           uint8_t directValueWhenAssign, uint8_t kindWhenAssign,
                                           std::string *content, uint8_t *kind, uint8_t *directValue,
                                           error::error *result_error) const;

        bool _next(token *result, error::error *result_error);

        bool next(token *result, error::error *result_error);

        bool lastTokenIsSet() const;
    };
}

#endif //PLASMA_LEXER_H
