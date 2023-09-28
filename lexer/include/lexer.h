#include <set>
#include <string>
#include <string_view>

#ifndef LEXER_H
#define LEXER_H

class Token {
public:
    enum Type {
        Number = 0,
        Identifier,
        Keyword,
        LeftParen,
        RightParen,
        LeftSquare,
        RightSquare,
        LeftCurly,
        RightCurly,
        LessThan,
        LessThanEqual,
        LeftShift,
        LeftShiftEqual,
        GreaterThan,
        GreaterThanEqual,
        RightShift,
        RightShiftEqual,
        Equal,
        EqualEqual,
        Plus,
        PlusEqual,
        PlusPlus,
        Minus,
        MinusEqual,
        MinusMinus,
        Asterisk,
        AsteriskEqual,
        Slash,
        SlashEqual,
        Percent,
        PercentEqual,
        Hash,
        Dot,
        Comma,
        Colon,
        ScopeResolution,
        Semicolon,
        StringLiteral,
        Comment,
        Pipe,
        PipeEqual,
        Or,
        Ampersand,
        AmpersandEqual,
        And,
        Not,
        NotEqual,
        Unexpected,
        Eof,
    };

    Token(Type type, const char *beg, std::size_t len) noexcept
            : type_(type), lexeme_(beg, len) {}

    bool is(Type type) const;

    std::string_view getLexeme() const;

    Type getType() const;

private:
    Type type_;
    std::string_view lexeme_;
};

class Lexer {
public:
    explicit Lexer(const std::string &source) noexcept: start_(source.c_str()) {}

    Token next() noexcept;


private:
    Token number_or_float() noexcept;

    Token identifier_or_keyword() noexcept;

    Token string_literal() noexcept;

    Token save(Token::Type type, std::size_t len, const char *beg) noexcept;

    Token start_with_plus() noexcept;

    Token start_with_minus() noexcept;

    Token start_with_asterisk() noexcept;

    Token start_with_slash() noexcept;

    Token start_with_percent() noexcept;

    Token start_with_left_angle_bracket() noexcept;

    Token start_with_right_angle_bracket() noexcept;

    Token start_with_equal() noexcept;

    Token start_with_ampersand() noexcept;

    Token start_with_pipe() noexcept;

    Token start_with_exclamation_mark() noexcept;

    Token start_with_colon() noexcept;

    char peek() const noexcept;

    char advance() noexcept;

    char peek_next() const noexcept;

    const char *start_ = nullptr;

    // handle keyword
    static const std::set<std::string_view> keywords_;
};

#endif // LEXER_H