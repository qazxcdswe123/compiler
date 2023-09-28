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

static std::unordered_map<Token::Type, std::string_view> token_names = {
        {Token::Type::Number,           "数字"},
        {Token::Type::Identifier,       "标识符"},
        {Token::Type::Keyword,          "关键字"},
        {Token::Type::LeftParen,        "左括号"},
        {Token::Type::RightParen,       "右括号"},
        {Token::Type::LeftSquare,       "左方括号"},
        {Token::Type::RightSquare,      "右方括号"},
        {Token::Type::LeftCurly,        "左花括号"},
        {Token::Type::RightCurly,       "右花括号"},
        {Token::Type::LessThan,         "小于"},
        {Token::Type::LessThanEqual,    "小于等于"},
        {Token::Type::LeftShift,        "左移"},
        {Token::Type::LeftShiftEqual,   "左移等于"},
        {Token::Type::GreaterThan,      "大于"},
        {Token::Type::GreaterThanEqual, "大于等于"},
        {Token::Type::RightShift,       "右移"},
        {Token::Type::RightShiftEqual,  "右移等于"},
        {Token::Type::Equal,            "等于"},
        {Token::Type::EqualEqual,       "等于等于"},
        {Token::Type::Plus,             "加"},
        {Token::Type::PlusEqual,        "加等于"},
        {Token::Type::PlusPlus,         "加加"},
        {Token::Type::Minus,            "减"},
        {Token::Type::MinusEqual,       "减等于"},
        {Token::Type::MinusMinus,       "减减"},
        {Token::Type::Asterisk,         "乘"},
        {Token::Type::AsteriskEqual,    "乘等于"},
        {Token::Type::Slash,            "除"},
        {Token::Type::SlashEqual,       "除等于"},
        {Token::Type::Percent,          "取余"},
        {Token::Type::PercentEqual,     "取余等于"},
        {Token::Type::Hash,             "井号"},
        {Token::Type::Dot,              "点"},
        {Token::Type::Comma,            "逗号"},
        {Token::Type::Colon,            "冒号"},
        {Token::Type::ScopeResolution,  "域"},
        {Token::Type::Semicolon,        "分号"},
        {Token::Type::StringLiteral,    "字符串字面量"},
        {Token::Type::Comment,          "注释"},
        {Token::Type::Pipe,             "二进制或"},
        {Token::Type::PipeEqual,        "二进制或等于"},
        {Token::Type::Or,               "或"},
        {Token::Type::Ampersand,        "二进制与"},
        {Token::Type::AmpersandEqual,   "二进制与等于"},
        {Token::Type::And,              "与"},
        {Token::Type::Not,              "非"},
        {Token::Type::NotEqual,         "不等于"},
        {Token::Type::Unexpected,       "未知"},
        {Token::Type::Eof,              "文件结束"},
};


#endif // LEXER_H