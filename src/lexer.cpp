#include "compiler/lexer.h"


plasma::lexer::lexer::lexer(plasma::reader::reader *codeReader) {
    this->codeReader = codeReader;
}

bool plasma::lexer::lexer::hasNext() const {
    return !this->complete;
}

void plasma::lexer::lexer::guessKind(const std::string &pattern, uint8_t *kind, uint8_t *directValue) {

    (*kind) = Keyboard;
    if (pattern == PassString) {
        (*directValue) = Pass;
    } else if (pattern == SuperString) {
        (*directValue) = Super;
    } else if (pattern == EndString) {
        (*directValue) = End;
    } else if (pattern == IfString) {
        (*directValue) = If;
    } else if (pattern == UnlessString) {
        (*directValue) = Unless;
    } else if (pattern == ElseString) {
        (*directValue) = Else;
    } else if (pattern == ElifString) {
        (*directValue) = Elif;
    } else if (pattern == WhileString) {
        (*directValue) = While;
    } else if (pattern == DoString) {
        (*directValue) = Do;
    } else if (pattern == ForString) {
        (*directValue) = For;
    } else if (pattern == UntilString) {
        (*directValue) = Until;
    } else if (pattern == SwitchString) {
        (*directValue) = Switch;
    } else if (pattern == CaseString) {
        (*directValue) = Case;
    } else if (pattern == DefaultString) {
        (*directValue) = Default;
    } else if (pattern == YieldString) {
        (*directValue) = Yield;
    } else if (pattern == ReturnString) {
        (*directValue) = Return;
    } else if (pattern == ContinueString) {
        (*directValue) = Continue;
    } else if (pattern == BreakString) {
        (*directValue) = Break;
    } else if (pattern == RedoString) {
        (*directValue) = Redo;
    } else if (pattern == ModuleString) {
        (*directValue) = Module;
    } else if (pattern == DefString) {
        (*directValue) = Def;
    } else if (pattern == LambdaString) {
        (*directValue) = Lambda;
    } else if (pattern == InterfaceString) {
        (*directValue) = Interface;
    } else if (pattern == ClassString) {
        (*directValue) = Class;
    } else if (pattern == TryString) {
        (*directValue) = Try;
    } else if (pattern == ExceptString) {
        (*directValue) = Except;
    } else if (pattern == FinallyString) {
        (*directValue) = Finally;
    } else if (pattern == AndString) {
        (*kind) = Comparator;
        (*directValue) = And;
    } else if (pattern == OrString) {
        (*kind) = Comparator;
        (*directValue) = Or;
    } else if (pattern == XorString) {
        (*kind) = Comparator;
        (*directValue) = Xor;
    } else if (pattern == InString) {
        (*kind) = Comparator;
        (*directValue) = In;
    } else if (pattern == AsString) {
        (*directValue) = As;
    } else if (pattern == RaiseString) {
        (*directValue) = Raise;
    } else if (pattern == AwaitString) {
        (*kind) = AwaitKeyboard;
        (*directValue) = Await;
    } else if (pattern == BEGINString) {
        (*directValue) = BEGIN;
    } else if (pattern == ENDString) {
        (*directValue) = END;
    } else if (pattern == NotString) {
        (*kind) = Operator;
        (*directValue) = Not;
    } else if (pattern == TrueString) {
        (*kind) = Boolean;
        (*directValue) = True;
    } else if (pattern == FalseString) {
        (*kind) = Boolean;
        (*directValue) = False;
    } else if (pattern == NoneString) {
        (*kind) = NoneType;
        (*directValue) = None;
    } else if (pattern == DeferString) {
        (*directValue) = Defer;
    } else if (pattern == ContextString) {
        (*directValue) = Context;
    } else if (std::regex_match(pattern, identifierCheck)) {
        (*kind) = IdentifierKind;
        (*directValue) = Unknown;
    } else if (std::regex_match(pattern, junkKindCheck)) {
        (*kind) = JunkKind;
        (*directValue) = Unknown;
    } else {
        (*kind) = Unknown;
        (*directValue) = Unknown;
    }
}

bool plasma::lexer::lexer::tokenizeStringLikeExpressions(char stringOpener, std::string *content, uint8_t *kind,
                                                         uint8_t *directValue, error::error *result_error) {
    content->push_back(stringOpener);
    uint8_t target;
    switch (stringOpener) {
        case '\'':
            target = SingleQuoteString;
            break;
        case '"':
            target = DoubleQuoteString;
            break;
        case '`':
            target = CommandOutput;
            break;
        default:
            break;
    }
    (*kind) = Literal;
    (*directValue) = Unknown;
    bool escaped = false;
    bool finish = false;
    for (; this->codeReader->hasNext() && !finish; this->codeReader->next()) {
        char currentChar = this->codeReader->currentChar();
        if (escaped) {
            switch (currentChar) {
                case '\\':
                case '\'':
                case '"':
                case '`':
                case 'a':
                case 'b':
                case 'e':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case '?':
                case 'u':
                case 'x':
                    escaped = false;
                    break;
                default:
                    (*result_error) = error::error(error::LexingError, "invalid escape sequence", this->line);
                    return false;
            }
        } else if (currentChar == '\n') {
            this->line++;
        } else if (currentChar == stringOpener) {
            (*directValue) = target;
            finish = true;
        } else if (currentChar == '\\') {
            escaped = true;
        }
        content->push_back(currentChar);
    }
    if ((*directValue) != target) {
        (*result_error) = error::error(error::LexingError, "string never closed", this->line);
        return false;
    }
    return true;
}

bool plasma::lexer::lexer::tokenizeHexadecimal(char letterX, std::string *content, uint8_t *kind, uint8_t *directValue,
                                               error::error *result_error) const {
    content->push_back('0');
    content->push_back(letterX);
    (*kind) = Literal;
    (*directValue) = Unknown;
    if (!this->codeReader->hasNext()) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    char nextDigit = this->codeReader->currentChar();
    if (!(('0' <= nextDigit && nextDigit <= '9') ||
          ('a' <= nextDigit && nextDigit <= 'f') ||
          ('A' <= nextDigit && nextDigit <= 'F'))) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    this->codeReader->next();
    content->push_back(nextDigit);
    (*directValue) = HexadecimalInteger;
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        nextDigit = this->codeReader->currentChar();
        if (!(('0' <= nextDigit && nextDigit <= '9') ||
              ('a' <= nextDigit && nextDigit <= 'f') ||
              ('A' <= nextDigit && nextDigit <= 'F')) &&
            nextDigit != '_') {
            return true;
        }
        content->push_back(nextDigit);
    }
    return true;
}

bool plasma::lexer::lexer::tokenizeBinary(char letterB, std::string *content, uint8_t *kind, uint8_t *directValue,
                                          error::error *result_error) const {
    content->push_back('0');
    content->push_back(letterB);
    (*kind) = Literal;
    (*directValue) = Unknown;
    if (!this->codeReader->hasNext()) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    char nextDigit = this->codeReader->currentChar();
    if (!(nextDigit == '0' || nextDigit == '1')) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    this->codeReader->next();
    content->push_back(nextDigit);
    (*directValue) = BinaryInteger;
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        nextDigit = this->codeReader->currentChar();
        if (!(nextDigit == '0' || nextDigit == '1') && nextDigit != '_') {
            return true;
        }
        content->push_back(nextDigit);
    }
    return true;
}

bool plasma::lexer::lexer::tokenizeOctal(char letterO, std::string *content, uint8_t *kind, uint8_t *directValue,
                                         error::error *result_error) const {
    content->push_back('0');
    content->push_back(letterO);
    (*kind) = Literal;
    (*directValue) = Unknown;
    if (!this->codeReader->hasNext()) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    char nextDigit = this->codeReader->currentChar();
    if (!('0' <= nextDigit && nextDigit <= '7')) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    this->codeReader->next();
    content->push_back(nextDigit);
    (*directValue) = OctalInteger;
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        nextDigit = this->codeReader->currentChar();
        if (!('0' <= nextDigit && nextDigit <= '7') && nextDigit != '_') {
            return true;
        }
        content->push_back(nextDigit);
    }
    return true;
}

bool plasma::lexer::lexer::tokenizeScientificFloat(std::string base, std::string *content, uint8_t *kind,
                                                   uint8_t *directValue, error::error *result_error) const {
    std::string result = std::move(base);
    (*kind) = Literal;
    (*directValue) = Unknown;
    if (!this->codeReader->hasNext()) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    char direction = this->codeReader->currentChar();
    if ((direction != '-') && (direction != '+')) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    this->codeReader->next();
    if (!this->codeReader->hasNext()) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    result.push_back(direction);
    char nextDigit = this->codeReader->currentChar();
    if (!('0' <= nextDigit && nextDigit <= '9')) {
        error::new_unknown_token_lind(result_error, this->line);
        return false;
    }
    result.push_back(nextDigit);
    this->codeReader->next();
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        nextDigit = this->codeReader->currentChar();
        if (!('0' <= nextDigit && nextDigit <= '9') && nextDigit != '_') {
            (*directValue) = ScientificFloat;
            (*content) = result;
            return true;
        }
        result.push_back(nextDigit);
    }
    (*directValue) = ScientificFloat;
    (*content) = result;
    return true;
}

bool plasma::lexer::lexer::tokenizeFloat(
        std::string base,
        std::string *content,
        uint8_t *kind,
        uint8_t *directValue,
        error::error *result_error) const {
    (*kind) = Literal;
    (*directValue) = Integer;
    if (!this->codeReader->hasNext()) {
        this->codeReader->redo();
        base.pop_back();
        (*content) = base;
        return true;
    }
    char nextDigit = this->codeReader->currentChar();
    if (!('0' <= nextDigit && nextDigit <= '9')) {
        this->codeReader->redo();
        base.pop_back();
        (*content) = base;
        return true;
    }
    this->codeReader->next();
    base.push_back(nextDigit);
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        nextDigit = this->codeReader->currentChar();
        if (('0' <= nextDigit && nextDigit <= '9') || nextDigit == '_') {
            base.push_back(nextDigit);
        } else if ((nextDigit == 'e') || (nextDigit == 'E')) {
            this->codeReader->next();
            base.push_back(nextDigit);
            return this->tokenizeScientificFloat(base, content, kind, directValue, result_error);
        } else {
            break;
        }
    }
    (*directValue) = Float;
    (*content) = base;
    return true;
}

bool plasma::lexer::lexer::tokenizeInteger(std::string base, std::string *content, uint8_t *kind, uint8_t *directValue,
                                           error::error *result_error) const {
    (*kind) = Literal;
    (*directValue) = Unknown;
    if (!this->codeReader->hasNext()) {
        (*content) = std::move(base);
        (*directValue) = Integer;
        return true;
    }
    char nextDigit = this->codeReader->currentChar();
    if (nextDigit == '.') {
        this->codeReader->next();
        base.push_back(nextDigit);
        return this->tokenizeFloat(base, content, kind, directValue, result_error);
    } else if (nextDigit == 'e' || nextDigit == 'E') {
        this->codeReader->next();
        base.push_back(nextDigit);
        return this->tokenizeScientificFloat(base, content, kind, directValue, result_error);
    } else if (!('0' <= nextDigit && nextDigit <= '9')) {
        (*content) = base;
        (*directValue) = Integer;
        return true;
    }
    this->codeReader->next();
    base.push_back(nextDigit);
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        nextDigit = this->codeReader->currentChar();
        if (nextDigit == 'e' || nextDigit == 'E') {
            this->codeReader->next();
            base.push_back(nextDigit);
            return this->tokenizeScientificFloat(base, content, kind, directValue, result_error);
        } else if (nextDigit == '.') {
            this->codeReader->next();
            base.push_back(nextDigit);
            return this->tokenizeFloat(base, content, kind, directValue, result_error);
        } else if (('0' <= nextDigit && nextDigit <= '9') || nextDigit == '_') {
            base.push_back(nextDigit);
        } else {
            break;
        }
    }
    (*content) = base;
    (*directValue) = Integer;
    return true;
}

bool
plasma::lexer::lexer::tokenizeNumeric(char firstDigit, std::string *content, uint8_t *kind, uint8_t *direct_value,
                                      error::error *result_error) const {
    (*kind) = Literal;
    (*direct_value) = Integer;
    if (!this->codeReader->hasNext()) {
        (*content) = std::string(1, firstDigit);
        return true;
    }
    char nextChar = this->codeReader->currentChar();
    this->codeReader->next();
    std::string base;
    base.push_back(firstDigit);
    base.push_back(nextChar);
    if (firstDigit == '0') {
        switch (nextChar) {
            case 'x':
            case 'X': // Hexadecimal
                return this->tokenizeHexadecimal(nextChar, content, kind, direct_value, result_error);
            case 'b':
            case 'B': // Binary
                return this->tokenizeBinary(nextChar, content, kind, direct_value, result_error);
            case 'o':
            case 'O': // Octal
                return this->tokenizeOctal(nextChar, content, kind, direct_value, result_error);
            case 'e':
            case 'E': // Scientific float
                return this->tokenizeScientificFloat(base, content, kind, direct_value, result_error);
            case '.': // Maybe a float
                return this->tokenizeFloat(base, content, kind, direct_value,
                                           result_error); // Integer, Float Or Scientific Float
            default:
                if (('0' <= nextChar && nextChar <= '9') || nextChar == '_') {
                    return this->tokenizeInteger(base, content, kind, direct_value,
                                                 result_error); // Integer, Float or Scientific Float
                }
                break;
        }
    } else {
        switch (nextChar) {
            case 'e':
            case 'E': // Scientific float
                return this->tokenizeScientificFloat(base, content, kind, direct_value,
                                                     result_error);
            case '.': // Maybe a float
                return this->tokenizeFloat(base, content, kind, direct_value,
                                           result_error); // Integer, Float Or Scientific Float
            default:
                if (('0' <= nextChar && nextChar <= '9') || nextChar == '_') {
                    return this->tokenizeInteger(base, content, kind, direct_value,
                                                 result_error); // Integer, Float or Scientific Float
                }
        }
    }
    this->codeReader->redo();
    (*content) = std::string(1, firstDigit);
    (*kind) = Literal;
    (*direct_value) = Integer;
    return true;
}

bool plasma::lexer::lexer::tokenizeChars(char startingChar, std::string *content, uint8_t *kind, uint8_t *direct_value,
                                         error::error *result_error) {
    std::string result = std::string(1, startingChar);
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        char currentChar = this->codeReader->currentChar();
        if (('a' <= currentChar && currentChar <= 'z') || ('A' <= currentChar && currentChar <= 'Z') ||
            ('0' <= currentChar && currentChar <= '9') || (currentChar == '_')) {
            result.push_back(currentChar);
        } else {
            break;
        }
    }
    this->guessKind(result, kind, direct_value);
    (*content) = result;
    return true;
}

bool plasma::lexer::lexer::tokenizeComment(std::string *content, uint8_t *kind, uint8_t *directValue,
                                           error::error *result_error) const {
    std::string result;
    for (; this->codeReader->hasNext(); this->codeReader->next()) {
        char currentChar = this->codeReader->currentChar();
        if (currentChar == '\n') {
            break;
        }
        result.push_back(currentChar);
    }
    (*kind) = Comment;
    (*directValue) = Comment;
    (*content) = result;
    return true;
}

bool plasma::lexer::lexer::tokenizeRepeatableOperator(char op, uint8_t directValueWhenSingle, uint8_t kindWhenSingle,
                                                      uint8_t directValueWhenDouble, uint8_t kindWhenDouble,
                                                      uint8_t directValueWhenSingleAssign, uint8_t kindWhenSingleAssign,
                                                      uint8_t directValueWhenDoubleAssign, uint8_t kindWhenDoubleAssign,
                                                      std::string *content, uint8_t *kind, uint8_t *directValue,
                                                      error::error *result_error) const {
    std::string result(1, op);
    (*kind) = kindWhenSingle;
    (*directValue) = directValueWhenSingle;
    if (this->codeReader->hasNext()) {
        char nextChar = this->codeReader->currentChar();
        if (nextChar == op) {
            result.push_back(op);
            this->codeReader->next();
            (*kind) = kindWhenDouble;
            (*directValue) = directValueWhenDouble;

            if (this->codeReader->hasNext()) {
                char nextNextChar = this->codeReader->currentChar();
                if (nextNextChar == '=') {
                    result.push_back('=');
                    (*kind) = kindWhenDoubleAssign;
                    this->codeReader->next();
                    (*directValue) = directValueWhenDoubleAssign;
                }
            }
        } else if (nextChar == '=') {
            (*kind) = kindWhenSingleAssign;
            (*directValue) = directValueWhenSingleAssign;
            result.push_back('=');

            this->codeReader->next();
        }
    }
    (*content) = result;
    return true;
}

bool plasma::lexer::lexer::tokenizeNotRepeatableOperator(char op, uint8_t directValueWhenOperator,
                                                         uint8_t kindWhenOperator, uint8_t directValueWhenAssign,
                                                         uint8_t kindWhenAssign, std::string *content, uint8_t *kind,
                                                         uint8_t *directValue, error::error *result_error) const {
    std::string result(1, op);
    (*kind) = kindWhenOperator;
    (*directValue) = directValueWhenOperator;
    if (this->codeReader->hasNext()) {
        char nextChar = this->codeReader->currentChar();
        if (nextChar == '=') {
            (*kind) = kindWhenAssign;
            (*directValue) = directValueWhenAssign;
            result.push_back('=');
            this->codeReader->next();
        }
    }
    (*content) = result;
    return true;
}

bool plasma::lexer::lexer::_next(token *result, error::error *result_error) {
    if (!this->codeReader->hasNext()) {
        this->complete = true;
        (*result) = token{"EOF", EndOfFile, EndOfFile, this->line};

        return true;
    }
    uint8_t kind = Unknown;
    uint8_t directValue = Unknown;
    std::string content;

    int currentLine = this->line;
    int index = this->codeReader->index();
    char currentChar = this->codeReader->currentChar();
    std::string base(1, currentChar);

    this->codeReader->next();

    bool success = true;
    switch (currentChar) {
        case NewLineChar:
            this->line++;
            content = base;
            directValue = NewLine;
            kind = Separator;
            break;
        case SemiColonChar:
            content = base;
            directValue = SemiColon;
            kind = Separator;
            break;
        case ColonChar:
            content = base;
            directValue = Colon;
            kind = Punctuation;
            break;
        case CommaChar:
            content = base;
            directValue = Comma;
            kind = Punctuation;
            break;
        case OpenParenthesesChar:
            content = base;
            directValue = OpenParentheses;
            kind = Punctuation;
            break;
        case CloseParenthesesChar:
            content = base;
            directValue = CloseParentheses;
            kind = Punctuation;
            break;
        case OpenSquareBracketChar:
            content = base;
            directValue = OpenSquareBracket;
            kind = Punctuation;
            break;
        case CloseSquareBracketChar:
            content = base;
            directValue = CloseSquareBracket;
            kind = Punctuation;
            break;
        case OpenBraceChar:
            content = base;
            directValue = OpenBrace;
            kind = Punctuation;
            break;
        case CloseBraceChar:
            content = base;
            directValue = CloseBrace;
            kind = Punctuation;
            break;
        case DollarSignChar:
            content = base;
            directValue = DollarSign;
            kind = Punctuation;
            break;
        case DotChar:
            content = base;
            directValue = Dot;
            kind = Punctuation;
            break;
        case WhiteSpaceChar:
            content = base;
            directValue = Whitespace;
            kind = Whitespace;
            break;
        case TabChar:
            content = base;
            directValue = Tab;
            kind = Whitespace;
            break;
        case CommentChar:
            success = this->tokenizeComment(&content, &kind, &directValue, result_error);
            content = "+" + content;
            break;
        case '`':
        case '\'':
        case '"': // String1
            success = this->tokenizeStringLikeExpressions(currentChar, &content, &kind, &directValue, result_error);
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            success = this->tokenizeNumeric(currentChar, &content, &kind, &directValue, result_error);
            break;
        case StarChar:
            success = tokenizeRepeatableOperator(currentChar, Star, Operator, PowerOf, Operator,
                                                 StarAssign, Assignment, PowerOfAssign,
                                                 Assignment, &content, &kind, &directValue, result_error);
            break;
        case DivChar:
            success = this->tokenizeRepeatableOperator(currentChar, Div, Operator, FloorDiv, Operator,
                                                       DivAssign, Assignment, FloorDivAssign,
                                                       Assignment, &content, &kind, &directValue, result_error);
            break;
        case LessThanChar:
            success = this->tokenizeRepeatableOperator(currentChar, LessThan, Comparator, BitwiseLeft,
                                                       Operator, LessOrEqualThan, Comparator,
                                                       BitwiseLeftAssign, Assignment, &content, &kind, &directValue,
                                                       result_error);
            break;
        case GreatThanChar:
            success = this->tokenizeRepeatableOperator(currentChar, GreaterThan, Comparator, BitwiseRight,
                                                       Operator, GreaterOrEqualThan, Comparator,
                                                       BitwiseRightAssign, Assignment, &content, &kind,
                                                       &directValue, result_error);
            break;
        case AddChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, Add, Operator, AddAssign, Assignment, &content,
                                                          &kind, &directValue, result_error);
            break;
        case SubChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, Sub, Operator, SubAssign, Assignment, &content,
                                                          &kind, &directValue, result_error);

            break;
        case ModulusChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, Modulus, Operator, ModulusAssign,
                                                          Assignment, &content, &kind, &directValue, result_error);
            break;
        case BitwiseXorChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, BitwiseXor, Operator,
                                                          BitwiseXorAssign, Assignment, &content, &kind, &directValue,
                                                          result_error);
            break;
        case BitWiseAndChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, BitWiseAnd, Operator,
                                                          BitWiseAndAssign, Assignment, &content, &kind, &directValue,
                                                          result_error);
            break;
        case BitwiseOrChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, BitwiseOr, Operator, BitwiseOrAssign,
                                                          Assignment, &content, &kind, &directValue, result_error);
            break;
        case SignNotChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, SignNot, Operator, NotEqual,
                                                          Comparator, &content, &kind, &directValue, result_error);
            break;
        case NegateBitsChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, NegateBits, Operator,
                                                          NegateBitsAssign, Assignment, &content, &kind, &directValue,
                                                          result_error);
            break;
        case EqualsChar:
            success = this->tokenizeNotRepeatableOperator(currentChar, Assign, Assignment, Equals,
                                                          Comparator, &content, &kind, &directValue, result_error);
            break;
        case BackSlashChar:
            if (this->codeReader->hasNext()) {
                char nextChar = this->codeReader->currentChar();
                if (nextChar != '\n') {
                    (*result_error) = error::error(error::LexingError, "line escape not followed by a new line",
                                                   this->line);
                    return false;
                }
                base.push_back('\n');
                content = base;
                this->codeReader->next();
            }
            kind = PendingEscape;
            break;
        default:
            if (currentChar == 'b') {
                if (this->codeReader->hasNext()) {
                    char nextChar = this->codeReader->currentChar();
                    if (nextChar == '\'' || nextChar == '"') {
                        this->codeReader->next();
                        success = this->tokenizeStringLikeExpressions(nextChar, &content, &kind, &directValue,
                                                                      result_error);
                        if (success) {
                            content = "b" + content;
                        }
                        if (directValue != Unknown) {
                            directValue = ByteString;
                        }
                        break;
                    }
                }
            }
            success = this->tokenizeChars(currentChar, &content, &kind, &directValue, result_error);
            break;
    }
    if (!success) {
        return false;
    }
    (*result) = token{content, directValue, kind, currentLine};
    return true;
}

bool plasma::lexer::lexer::next(token *result, error::error *result_error) {
    if (!this->_next(result, result_error)) {
        return false;
    }
    if (result->kind == JunkKind) {
        return this->next(result, result_error);
    }
    if (result->kind == Comment) {
        return this->next(result, result_error);
    }
    if (result->kind == Whitespace) {
        return this->next(result, result_error);
    }
    this->lastToken = *result;
    return true;
}

bool plasma::lexer::lexer::lastTokenIsSet() const {
    return this->lastToken.directValue == NotSet && this->lastToken.kind == NotSet && this->lastToken.line == -1 &&
           this->lastToken.string.length() == 0;
}
