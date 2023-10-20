#include "../include/json.hpp"
//#include "../include/httplib.h"

#include "re.h"
#include "nfa.h"

#include <iostream>

using json = nlohmann::json;

int main() {
    Regexp re("l(l|d)*");
    auto nfa = NFA(re.get_post_re());
    auto res = NFA::transition_;

    json j;
    for (auto &pair: res) {
        for (auto &pair2: pair.second) {
            for (auto &state: pair2.second) {
                j.push_back({
                                    {"is_start", pair.first == nfa.getStart()},
                                    {"is_end",   state == nfa.getEnd()},
                                    {"from",     pair.first},
                                    {"to",       state},
                                    {"char",     pair2.first}
                            });
            }
        }
    }

    std::cout << j.dump(4) << std::endl;
}
