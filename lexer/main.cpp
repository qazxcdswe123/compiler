#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <map>
#include <iomanip>

#include "include/lexer.h"
#include "include/json.hpp"
#include "include/httplib.h"

using json = nlohmann::json;

// map enum to name to print
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
        {Token::Type::Asterisk,        "乘"},
        {Token::Type::AsteriskEqual,   "乘等于"},
        {Token::Type::Slash,           "除"},
        {Token::Type::SlashEqual,      "除等于"},
        {Token::Type::Percent,         "取余"},
        {Token::Type::PercentEqual,    "取余等于"},
        {Token::Type::Hash,            "井号"},
        {Token::Type::Dot,             "点"},
        {Token::Type::Comma,           "逗号"},
        {Token::Type::Colon,           "冒号"},
        {Token::Type::ScopeResolution, "域"},
        {Token::Type::Semicolon,       "分号"},
        {Token::Type::StringLiteral,   "字符串字面量"},
        {Token::Type::Comment,         "注释"},
        {Token::Type::Pipe,            "二进制或"},
        {Token::Type::PipeEqual,       "二进制或等于"},
        {Token::Type::Or,              "或"},
        {Token::Type::Ampersand,       "二进制与"},
        {Token::Type::AmpersandEqual,  "二进制与等于"},
        {Token::Type::And,             "与"},
        {Token::Type::Not,             "非"},
        {Token::Type::NotEqual,         "不等于"},
        {Token::Type::Unexpected,       "未知"},
        {Token::Type::Eof,              "文件结束"},
};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source file>" << std::endl;
        return 1;
    }

    char *filename = argv[1];

    // read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return 1;
    }

    std::string source;
    std::string line;
    while (std::getline(file, line)) {
        source += line + '\n';
    }

    // lex
    Lexer lexer(source);
    std::vector<std::map<Token::Type, std::string_view>> tokens;

    for (Token token = lexer.next(); !token.is(Token::Type::Eof);
         token = lexer.next()) {
        std::map<Token::Type, std::string_view> token_map = {
                {token.getType(), token.getLexeme()}
        };
        tokens.push_back(token_map);
    }



    // print with style
//    for (auto &token: tokens) {
//        for (auto &pair: token) {
//            std::cout << std::setw(20) << std::left << token_names[pair.first]
//                      << pair.second << std::endl;
//        }
//    }

    // write to json
    json j;
    for (auto &token: tokens) {
        for (auto &pair: token) {
            // append to end
            j.push_back({
                                {"type",   token_names[pair.first]},
                                {"lexeme", pair.second}
                        });
        }
    }
}