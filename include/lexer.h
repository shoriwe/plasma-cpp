#ifndef PLASMA_LEXER_H
#define PLASMA_LEXER_H

#include <string>
#include <regex>

#include "reader.h"
#include "error.h"

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
    const char BitWiseAndChar = '&';
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
    const std::string PassString = "pass";
    const std::string SuperString = "super";
    const std::string EndString = "end";
    const std::string IfString = "if";
    const std::string UnlessString = "unless";
    const std::string ElseString = "else";
    const std::string ElifString = "elif";
    const std::string WhileString = "while";
    const std::string DoString = "do";
    const std::string ForString = "for";
    const std::string UntilString = "until";
    const std::string SwitchString = "switch";
    const std::string CaseString = "case";
    const std::string DefaultString = "default";
    const std::string YieldString = "yield";
    const std::string ReturnString = "return";
    const std::string ContinueString = "continue";
    const std::string BreakString = "break";
    const std::string RedoString = "redo";
    const std::string DeferString = "defer";
    const std::string ModuleString = "module";
    const std::string DefString = "def";
    const std::string LambdaString = "lambda";
    const std::string InterfaceString = "interface";
    const std::string ClassString = "class";
    const std::string TryString = "try";
    const std::string ExceptString = "except";
    const std::string FinallyString = "finally";
    const std::string AndString = "and";
    const std::string OrString = "or";
    const std::string XorString = "xor";
    const std::string InString = "in";
    const std::string IsInstanceOfString = "isinstanceof";
    const std::string AwaitString = "await";
    const std::string BEGINString = "BEGIN";
    const std::string ENDString = "END";
    const std::string NotString = "not";
    const std::string TrueString = "True";
    const std::string FalseString = "False";
    const std::string NoneString = "None";
    const std::string ContextString = "context";
    const std::string RaiseString = "raise";
    const std::string AsString = "as";
    enum {
        NotSet,
        Unknown,
        PendingEscape,
        Comment,
        Whitespace,
        Literal,
        Tab,
        IdentifierKind,
        JunkKind,
        Separator,
        Punctuation,
        Assignment,
        Comparator,
        Operator,
        SingleQuoteString,
        DoubleQuoteString,
        Integer,
        HexadecimalInteger,
        BinaryInteger,
        OctalInteger,
        Float,
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
        Super,
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
        Yield,
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
        BitWiseAndAssign,
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
        BitWiseAnd,
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
        int index;
    };

    struct lexer {
        token lastToken{"", NotSet, NotSet, -1, -1};
        int line = 0;
        reader::reader *codeReader;
        bool complete = false;

        lexer(reader::reader *codeReader);

        bool hasNext() const;

        void guessKind(std::string pattern, uint8_t *kind, uint8_t *directValue);

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
