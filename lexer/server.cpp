#include "include/json.hpp"
#include "include/httplib.h"
#include "include/lexer.h"

#include <string>

using json = nlohmann::json;

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
                     auto lexeme = token.getLexeme();
                     if (lexeme == "include") {
                         lexeme = "import";
                     }
                     std::map < Token::Type, std::string_view > token_map = {
                             {token.getType(), lexeme}
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
    std::cout << "Starting server on port 8080..." << std::endl;
    std::cout << "Waiting for requests..." << std::endl;
}
