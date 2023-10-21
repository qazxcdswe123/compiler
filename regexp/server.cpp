#include "../include/json.hpp"
//#include "../include/httplib.h"

#include "re.h"
#include "nfa.h"

#include <iostream>

using json = nlohmann::json;

int main() {
    Regexp re("l(l|d)*");
//    Regexp re("ab*");
    auto nfa = NFA(re.get_post_re());
    auto res = NFA::transition_;
    auto start = nfa.getStart();
    auto end = nfa.getEnd();

    json j;
    for (auto &pair: res) {
        for (auto &pair2: pair.second) {
            for (auto &state: pair2.second) {
                j.push_back({
                                    {"is_start", pair.first == start},
                                    {"is_end",   state == end},
                                    {"from",     pair.first},
                                    {"to",       state},
                                    {"char",     pair2.first}
                            });
            }
        }
    }

    std::cout << j.dump(4) << std::endl;
}
