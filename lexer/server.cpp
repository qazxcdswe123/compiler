//
// Created by yn on 21/9/2023.
//
#include "include/json.hpp"
#include "include/httplib.h"
#include "include/lexer.h"

#include <string>

using json = nlohmann::json;

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

int main() {
    httplib::Server svr;

    svr.Post("/lex", [](const httplib::Request &req, httplib::Response &res) {
                 std::string code = req.body;
                 std::cout << "Code Input: " << code << std::endl;
                 std::vector<std::map<Token::Type, std::string_view>> tokens;
                 Lexer lexer(code);

                 for (Token token = lexer.next(); !token.is(Token::Type::Eof);
                      token = lexer.next()) {
                     std::map < Token::Type, std::string_view > token_map = {
                             {token.getType(), token.getLexeme()}
                     };
                     tokens.push_back(token_map);
                 }

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

                 res.set_header("Access-Control-Allow-Origin", "*");
                 res.set_content(j.dump(), "application/json");
             }
    );

    svr.Post("/replace", [](const httplib::Request &req, httplib::Response &res) {
                 std::string code = req.body;
                 std::cout << "Code Input: " << code << std::endl;
                 std::vector<std::map<Token::Type, std::string_view>> tokens;
                 Lexer lexer(code);

                 for (Token token = lexer.next(); !token.is(Token::Type::Eof);
                      token = lexer.next()) {
                     std::map < Token::Type, std::string_view > token_map = {
                             {token.getType(), token.getLexeme()}
                     };
                     tokens.push_back(token_map);
                 }

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

                 res.set_header("Access-Control-Allow-Origin", "*");
                 res.set_content(j.dump(), "application/json");
             }
    );

    svr.listen("0.0.0.0", 8080);
}
