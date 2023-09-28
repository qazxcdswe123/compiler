#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <iomanip>

#include "include/lexer.h"
#include "include/json.hpp"

using json = nlohmann::json;

// map enum to name to print

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