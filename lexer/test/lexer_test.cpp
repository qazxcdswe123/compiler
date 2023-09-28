#include "gtest/gtest.h"
#include "../include/lexer.h"

TEST(Lexer, Number) {
    std::string input1 = "0X 0x 0b 0B 0 0x123";
    std::string input2 = "3.45 0.123 12.34 3.45e+3";

    std::vector<std::map<Token::Type, std::string_view>> expected = {
            {{Token::Type::Number, "0X"}},
            {{Token::Type::Number, "0x"}},
            {{Token::Type::Number, "0b"}},
            {{Token::Type::Number, "0B"}},
            {{Token::Type::Number, "0"}},
            {{Token::Type::Number, "0x123"}},
            {{Token::Type::Number, "3.45"}},
            {{Token::Type::Number, "0.123"}},
            {{Token::Type::Number, "12.34"}},
            {{Token::Type::Number, "3.45e+3"}},
    };

    std::vector<std::map<Token::Type, std::string_view>> actual;
    actual.reserve(expected.size());

    Lexer lexer1(input1);
    Lexer lexer2(input2);

    for (Token token = lexer1.next(); !token.is(Token::Type::Eof);
         token = lexer1.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    for (Token token = lexer2.next(); !token.is(Token::Type::Eof);
         token = lexer2.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    EXPECT_EQ(expected, actual);
}

TEST(Lexer, Comment) {
    std::string input1 = "/* inside\n"
                         " * multiline\n"
                         " * comment\n"
                         " */";

    std::string input2 = "// inside";

    std::vector<std::map<Token::Type, std::string_view>> expected = {
            {{Token::Type::Comment, "/* inside\n"
                                    " * multiline\n"
                                    " * comment\n"
                                    " */"}},
            {{Token::Type::Comment, "// inside"}},
    };

    std::vector<std::map<Token::Type, std::string_view>> actual;
    actual.reserve(expected.size());

    Lexer lexer(input1);

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    lexer = Lexer(input2);

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    EXPECT_EQ(expected, actual);
}

TEST(Lexer, Operator) {
    std::string input = "/= / < << <= <<= > >> >= >>= = == * *= - -= -- + ++ += ! != == || &&";

    std::vector<std::map<Token::Type, std::string_view>> expected = {
            {{Token::Type::SlashEqual,       "/="}},
            {{Token::Type::Slash,            "/"}},
            {{Token::Type::LessThan,         "<"}},
            {{Token::Type::LeftShift,        "<<"}},
            {{Token::Type::LessThanEqual,    "<="}},
            {{Token::Type::LeftShiftEqual,   "<<="}},
            {{Token::Type::GreaterThan,      ">"}},
            {{Token::Type::RightShift,       ">>"}},
            {{Token::Type::GreaterThanEqual, ">="}},
            {{Token::Type::RightShiftEqual,  ">>="}},
            {{Token::Type::Equal,            "="}},
            {{Token::Type::EqualEqual,       "=="}},
            {{Token::Type::Asterisk,         "*"}},
            {{Token::Type::AsteriskEqual,    "*="}},
            {{Token::Type::Minus,            "-"}},
            {{Token::Type::MinusEqual,       "-="}},
            {{Token::Type::MinusMinus,       "--"}},
            {{Token::Type::Plus,             "+"}},
            {{Token::Type::PlusPlus,         "++"}},
            {{Token::Type::PlusEqual,        "+="}},
            {{Token::Type::Not,              "!"}},
            {{Token::Type::NotEqual,         "!="}},
            {{Token::Type::EqualEqual,       "=="}},
            {{Token::Type::Or,               "||"}},
            {{Token::Type::And,              "&&"}},
    };

    std::vector<std::map<Token::Type, std::string_view>> actual;
    actual.reserve(expected.size());

    Lexer lexer(input);

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    EXPECT_EQ(expected, actual);
}

TEST(Lexer, Special) {
    std::string input = "   (   )   ;     !=  ==  { }  [  ]   ,";

    std::vector<std::map<Token::Type, std::string_view>> expected = {
            {{Token::Type::LeftParen,   "("}},
            {{Token::Type::RightParen,  ")"}},
            {{Token::Type::Semicolon,   ";"}},
            {{Token::Type::NotEqual,    "!="}},
            {{Token::Type::EqualEqual,  "=="}},
            {{Token::Type::LeftCurly,   "{"}},
            {{Token::Type::RightCurly,  "}"}},
            {{Token::Type::LeftSquare,  "["}},
            {{Token::Type::RightSquare, "]"}},
            {{Token::Type::Comma,       ","}},
    };

    std::vector<std::map<Token::Type, std::string_view>> actual;
    actual.reserve(expected.size());

    Lexer lexer(input);

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    EXPECT_EQ(expected, actual);
}

TEST(Lexer, Identifier) {
    std::string input = "a _a a_ _a_ a1 _a1 a1_ _a1_";

    std::vector<std::map<Token::Type, std::string_view>> expected = {
            {{Token::Type::Identifier, "a"}},
            {{Token::Type::Identifier, "_a"}},
            {{Token::Type::Identifier, "a_"}},
            {{Token::Type::Identifier, "_a_"}},
            {{Token::Type::Identifier, "a1"}},
            {{Token::Type::Identifier, "_a1"}},
            {{Token::Type::Identifier, "a1_"}},
            {{Token::Type::Identifier, "_a1_"}},
    };

    std::vector<std::map<Token::Type, std::string_view>> actual;
    actual.reserve(expected.size());

    Lexer lexer(input);

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    EXPECT_EQ(expected, actual);
}

TEST(Lexer, Keywords) {
    std::string input = "asm else new this auto enum operator throw bool explicit private true break export protected try case extern public typedef catch false register typeid char float reinterpret_cast typename class for return union const friend short unsigned const_cast goto signed using continue if sizeof virtual default inline static void delete int static_cast volatile do long struct wchat_t double mutable switch while dynamic_cast namespace template include";

    std::vector<std::map<Token::Type, std::string_view>> expected = {
            {{Token::Type::Keyword, "asm"}},
            {{Token::Type::Keyword, "else"}},
            {{Token::Type::Keyword, "new"}},
            {{Token::Type::Keyword, "this"}},
            {{Token::Type::Keyword, "auto"}},
            {{Token::Type::Keyword, "enum"}},
            {{Token::Type::Keyword, "operator"}},
            {{Token::Type::Keyword, "throw"}},
            {{Token::Type::Keyword, "bool"}},
            {{Token::Type::Keyword, "explicit"}},
            {{Token::Type::Keyword, "private"}},
            {{Token::Type::Keyword, "true"}},
            {{Token::Type::Keyword, "break"}},
            {{Token::Type::Keyword, "export"}},
            {{Token::Type::Keyword, "protected"}},
            {{Token::Type::Keyword, "try"}},
            {{Token::Type::Keyword, "case"}},
            {{Token::Type::Keyword, "extern"}},
            {{Token::Type::Keyword, "public"}},
            {{Token::Type::Keyword, "typedef"}},
            {{Token::Type::Keyword, "catch"}},
            {{Token::Type::Keyword, "false"}},
            {{Token::Type::Keyword, "register"}},
            {{Token::Type::Keyword, "typeid"}},
            {{Token::Type::Keyword, "char"}},
            {{Token::Type::Keyword, "float"}},
            {{Token::Type::Keyword, "reinterpret_cast"}},
            {{Token::Type::Keyword, "typename"}},
            {{Token::Type::Keyword, "class"}},
            {{Token::Type::Keyword, "for"}},
            {{Token::Type::Keyword, "return"}},
            {{Token::Type::Keyword, "union"}},
            {{Token::Type::Keyword, "const"}},
            {{Token::Type::Keyword, "friend"}},
            {{Token::Type::Keyword, "short"}},
            {{Token::Type::Keyword, "unsigned"}},
            {{Token::Type::Keyword, "const_cast"}},
            {{Token::Type::Keyword, "goto"}},
            {{Token::Type::Keyword, "signed"}},
            {{Token::Type::Keyword, "using"}},
            {{Token::Type::Keyword, "continue"}},
            {{Token::Type::Keyword, "if"}},
            {{Token::Type::Keyword, "sizeof"}},
            {{Token::Type::Keyword, "virtual"}},
            {{Token::Type::Keyword, "default"}},
            {{Token::Type::Keyword, "inline"}},
            {{Token::Type::Keyword, "static"}},
            {{Token::Type::Keyword, "void"}},
            {{Token::Type::Keyword, "delete"}},
            {{Token::Type::Keyword, "int"}},
            {{Token::Type::Keyword, "static_cast"}},
            {{Token::Type::Keyword, "volatile"}},
            {{Token::Type::Keyword, "do"}},
            {{Token::Type::Keyword, "long"}},
            {{Token::Type::Keyword, "struct"}},
            {{Token::Type::Keyword, "wchat_t"}},
            {{Token::Type::Keyword, "double"}},
            {{Token::Type::Keyword, "mutable"}},
            {{Token::Type::Keyword, "switch"}},
            {{Token::Type::Keyword, "while"}},
            {{Token::Type::Keyword, "dynamic_cast"}},
            {{Token::Type::Keyword, "namespace"}},
            {{Token::Type::Keyword, "template"}},
            {{Token::Type::Keyword, "include"}},
    };

    std::vector<std::map<Token::Type, std::string_view>> actual;
    actual.reserve(expected.size());

    Lexer lexer(input);

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    EXPECT_EQ(expected, actual);
}

TEST(Lexer, Program) {
    std::string input = "#include <iostream>\n"
                        "\n"
                        "int main() {\n"
                        "    std::cout << \"Hello, World!\" << std::endl;\n"
                        "    return 0;\n"
                        "}";

    std::vector<std::map<Token::Type, std::string_view>> expected = {
            {{Token::Type::Hash,            "#"}},
            {{Token::Type::Keyword,         "include"}},
            {{Token::Type::LessThan,        "<"}},
            {{Token::Type::Identifier,      "iostream"}},
            {{Token::Type::GreaterThan,     ">"}},
            {{Token::Type::Keyword,         "int"}},
            {{Token::Type::Identifier,      "main"}},
            {{Token::Type::LeftParen,       "("}},
            {{Token::Type::RightParen,      ")"}},
            {{Token::Type::LeftCurly,       "{"}},
            {{Token::Type::Identifier,      "std"}},
            {{Token::Type::ScopeResolution, "::"}},
            {{Token::Type::Identifier,      "cout"}},
            {{Token::Type::LeftShift,       "<<"}},
            {{Token::Type::StringLiteral,   "\"Hello, World!\""}},
            {{Token::Type::LeftShift,       "<<"}},
            {{Token::Type::Identifier,      "std"}},
            {{Token::Type::ScopeResolution, "::"}},
            {{Token::Type::Identifier,      "endl"}},
            {{Token::Type::Semicolon,       ";"}},
            {{Token::Type::Keyword,         "return"}},
            {{Token::Type::Number,          "0"}},
            {{Token::Type::Semicolon,       ";"}},
            {{Token::Type::RightCurly,      "}"}},
    };

    std::vector<std::map<Token::Type, std::string_view>> actual;
    actual.reserve(expected.size());

    Lexer lexer(input);

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map < Token::Type, std::string_view > token_map = {
                {token.getType(), token.getLexeme()}
        };
        actual.push_back(token_map);
    }

    EXPECT_EQ(expected, actual);
}