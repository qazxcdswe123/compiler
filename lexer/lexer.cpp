#include <cctype>

#include "include/lexer.h"

const std::set<std::string_view> Lexer::keywords_ = {
        "asm", "else", "new",
        "this", "auto", "enum",
        "operator", "throw", "bool",
        "explicit", "private", "true",
        "break", "export", "protected",
        "try", "case", "extern",
        "public", "typedef", "catch",
        "false", "register", "typeid",
        "char", "float", "reinterpret_cast",
        "typename", "class", "for",
        "return", "union", "const",
        "friend", "short", "unsigned",
        "const_cast", "goto", "signed",
        "using", "continue", "if",
        "sizeof", "virtual", "default",
        "inline", "static", "void",
        "delete", "int", "static_cast",
        "volatile", "do", "long",
        "struct", "wchat_t", "double",
        "mutable", "switch", "while",
        "dynamic_cast", "namespace", "template", "include"};

bool Token::is(Type type) const { return type_ == type; }

std::string_view Token::getLexeme() const { return lexeme_; }

Token::Type Token::getType() const {
    return type_;
}

Token Lexer::save(Token::Type type, std::size_t len, const char *beg = nullptr) noexcept {
    if (beg == nullptr) {
        beg = start_;
    }
    auto token = Token(type, beg, len);
//    for (auto i = 0; i < len; ++i) {
//    advance();
//    }
    return token;
}

char Lexer::peek() const noexcept { return *start_; }

char Lexer::advance() noexcept { return *start_++; }

char Lexer::peek_next() const noexcept { return *(start_ + 1); }

Token Lexer::start_with_slash() noexcept {
    // handle `//   /*   */   /=    /`
    // current peek is '/'

    const char *start = start_;
    auto c = peek_next();
    switch (c) {
        case '/': // //
            advance();
            advance();
            while (peek() != '\0' && peek() != '\n') {
                advance();
            }
            return save(Token::Type::Comment, std::distance(start, start_), start);
        case '*': // /*
            advance();
            while (peek() != '\0') {
                if (peek() == '*' && peek_next() == '/') {
                    advance();
                    advance();
                    return save(Token::Type::Comment, std::distance(start, start_), start);
                }
                advance();
            }
            // unexpected end of file
            return save(Token::Type::Unexpected, std::distance(start, start_), start);
        case '=': // /=
            advance();
            advance();
            return save(Token::Type::SlashEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Slash, 1, start);
    }
}

Token Lexer::start_with_plus() noexcept {
    // handle `++   +=   +`
    // current peek is '+'

    const char *start = start_;
    switch (peek_next()) {
        case '+': // ++
            advance();
            advance();
            return Token(Token::Type::PlusPlus, start, 2);
        case '=': // +=
            advance();
            advance();
            return Token(Token::Type::PlusEqual, start, 2);
        default:
            advance();
            return Token(Token::Type::Plus, start, 1);
    }
}

Token Lexer::start_with_minus() noexcept {
    // handle `--   -=   -`
    // current peek is '-'

    const char *start = start_;
    switch (peek_next()) {
        case '-': // --
            advance();
            advance();
            return save(Token::Type::MinusMinus, 2, start);
        case '=': // -=
            advance();
            advance();
            return save(Token::Type::MinusEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Minus, 1, start);
    }
}

Token Lexer::start_with_equal() noexcept {
    // handle `==   =`
    // current peek is '='

    const char *start = start_;
    switch (peek_next()) {
        case '=': // ==
            advance();
            advance();
            return save(Token::Type::EqualEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Equal, 1, start);
    }
}

Token Lexer::string_literal() noexcept {
    const char *start = start_;
    advance();

    while (peek() != '\0') {
        auto c = advance();
        if (c == '\'' || c == '"') {
            return save(Token::Type::StringLiteral, std::distance(start, start_), start);
        }
    }

    // unexpected end of file
    return save(Token::Type::Unexpected, std::distance(start, start_), start);
}

Token Lexer::number_or_float() noexcept {
    // handle 3.45   0.23    12.34   3.45e+3   ........
    // current peek is '0' ~ '9'

    const char *start = start_;
    auto c = peek_next();
    while (std::isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == 'x' ||
           c == 'X' || c == 'b' || c == 'B') {
        // handle 0x1234 0X1234 0b1010 0B1010 0.1234 0.1234e+3 0.1234e-3 0.1234e3

        advance();
        switch (c) {
            case 'e':
            case 'E':
                c = peek_next();
                if (c == '+' || c == '-' || std::isdigit(c)) {
                    advance(); // e
                    advance(); // + -
                } else {
                    // unexpected end of file
                    return save(Token::Type::Unexpected, std::distance(start, start_), start);
                }
                c = peek_next();
                break;
//            case 'x':
//            case 'X':
//            case 'b':
//            case 'B':
            default: // '0' ~ '9' or '.' or 'x' or 'X' or 'b' or 'B'
                // cannot fall through here since it can only apper once
                c = peek_next();
                break;
        }
    }

    advance();
    auto len = std::distance(start, start_);
    return save(Token::Type::Number, len, start);
}

Token Lexer::identifier_or_keyword() noexcept {
    // handle identifier or keyword
    // current peek is 'a' ~ 'z' or 'A' ~ 'Z' or '_'

    const char *start = start_;
    auto c = peek_next();
    while (std::isalnum(c) || c == '_') {
        advance();
        c = peek_next();
    }
    advance();

    auto len = std::distance(start, start_);
    auto lexeme = std::string_view(start, len);
    if (keywords_.find(lexeme) != keywords_.end()) {
        return save(Token::Type::Keyword, len, start);
    } else {
        return save(Token::Type::Identifier, len, start);
    }
}

Token Lexer::next() noexcept {
    while (std::isspace(peek())) {
        advance();
    }
    const char *start = start_;

    switch (peek()) {
        case '0' ... '9': // gnu extension
            return number_or_float();
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case '_':
            return identifier_or_keyword();
        case '(':
            advance();
            return save(Token::LeftParen, 1, start);
        case ')':
            advance();
            return save(Token::RightParen, 1, start);
        case '[':
            advance();
            return save(Token::Type::LeftSquare, 1, start);
        case ']':
            advance();
            return save(Token::Type::RightSquare, 1, start);
        case '{':
            advance();
            return save(Token::Type::LeftCurly, 1, start);
        case '}':
            advance();
            return save(Token::Type::RightCurly, 1, start);
        case '<':
            return start_with_left_angle_bracket();
        case '>':
            return start_with_right_angle_bracket();
        case '=':
            return start_with_equal();
        case '+':
            return start_with_plus();
        case '-':
            return start_with_minus();
        case '*':
            return start_with_asterisk();
        case '/':
            return start_with_slash();
        case '#':
            advance();
            return save(Token::Type::Hash, 1, start);
        case '.':
            advance();
            return save(Token::Type::Dot, 1, start);
        case ',':
            advance();
            return save(Token::Type::Comma, 1, start);
        case ':':
            return start_with_colon();
        case ';':
            advance();
            return save(Token::Type::Semicolon, 1, start);
        case '\'': // fall through
        case '"':
            return string_literal();
        case '|':
            return start_with_pipe();
        case '&':
            return start_with_ampersand();
        case '!':
            return start_with_exclamation_mark();
        case '%':
            return start_with_percent();
        case '\0':
            advance();
            return save(Token::Type::Eof, 1, start);
        default:
            advance();
            return save(Token::Type::Unexpected, 1, start);
    }
}

Token Lexer::start_with_asterisk() noexcept {
    // handle `*=   *`
    // current peek is '*'

    const char *start = start_;
    switch (peek_next()) {
        case '=':
            advance();
            advance();
            return save(Token::Type::AsteriskEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Asterisk, 1, start);
    }
}

Token Lexer::start_with_percent() noexcept {
    // handle `%=   %`
    // current peek is '%'

    const char *start = start_;
    switch (peek_next()) {
        case '=':
            advance();
            advance();
            return save(Token::Type::PercentEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Percent, 1, start);
    }
}

Token Lexer::start_with_left_angle_bracket() noexcept {
    // handle `<<=   <<   <=   <`
    // current peek is '<'

    const char *start = start_;
    switch (peek_next()) {
        case '<':
            advance();
            switch (peek_next()) {
                case '=':
                    advance();
                    advance();
                    return save(Token::Type::LeftShiftEqual, 3, start);
                default:
                    advance();
                    return save(Token::Type::LeftShift, 2, start);
            }
        case '=':
            advance();
            advance();
            return save(Token::Type::LessThanEqual, 2, start);
        default:
            advance();
            return save(Token::Type::LessThan, 1, start);
    }
}

Token Lexer::start_with_right_angle_bracket() noexcept {
    // handle `>>=   >>   >=   >`
    // current peek is '>'

    const char *start = start_;
    switch (peek_next()) {
        case '>':
            advance();
            switch (peek_next()) {
                case '=':
                    advance();
                    advance();
                    return save(Token::Type::RightShiftEqual, 3, start);
                default:
                    advance();
                    return save(Token::Type::RightShift, 2, start);
            }
        case '=':
            advance();
            advance();
            return save(Token::Type::GreaterThanEqual, 2, start);
        default:
            advance();
            return save(Token::Type::GreaterThan, 1, start);
    }
}

Token Lexer::start_with_ampersand() noexcept {
    // handle `&&   &=   &`
    // current peek is '&'

    const char *start = start_;
    switch (peek_next()) {
        case '&':
            advance();
            advance();
            return save(Token::Type::And, 2, start);
        case '=':
            advance();
            advance();
            return save(Token::Type::AmpersandEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Ampersand, 1, start);
    }
}

Token Lexer::start_with_pipe() noexcept {
    // handle `||   |=   |`
    // current peek is '|'

    const char *start = start_;
    switch (peek_next()) {
        case '|':
            advance();
            advance();
            return save(Token::Type::Or, 2, start);
        case '=':
            advance();
            advance();
            return save(Token::Type::PipeEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Pipe, 1, start);
    }
}

Token Lexer::start_with_exclamation_mark() noexcept {
    // handle `!=   !`
    // current peek is '!'

    const char *start = start_;
    switch (peek_next()) {
        case '=':
            advance();
            advance();
            return save(Token::Type::NotEqual, 2, start);
        default:
            advance();
            return save(Token::Type::Not, 1, start);
    }
}

Token Lexer::start_with_colon() noexcept {
    // handle `::   :`
    // current peek is ':'

    const char *start = start_;
    switch (peek_next()) {
        case ':':
            advance();
            advance();
            return save(Token::Type::ScopeResolution, 2, start);
        default:
            advance();
            return save(Token::Type::Colon, 1, start);
    }
}
