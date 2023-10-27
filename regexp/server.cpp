#include "../include/json.hpp"
//#include "../include/httplib.h"

#include "re.h"
#include "nfa.h"
#include "dfa.h"

#include <iostream>

using json = nlohmann::json;

// NFA to DFA test
int main() {
    Regexp re("l(l|d)*");

    auto nfa = NFA(re.get_post_re());
    auto dfa = DFA(nfa);
    dfa.minimize();
    auto res = DFA::transition_;

    const auto &start = dfa.getStart();
    auto ends = dfa.getEnds();

    json j;
    for (auto &pair: res) {
        // transition from pair.first (set<State>) to pair.second (map<char, set<State>>)
        for (auto &pair2: pair.second) {
            // transition from pair2.first (char) to pair2.second (set<State>)

            j.push_back({
                                {"is_start", pair.first == start},
                                {"is_end",   DFA::containEnd(pair.first, nfa)},
                                {"from",     pair.first},
                                {"to",       pair2.second},
                                {"char",     pair2.first}
                        });
        }
    }

    std::cout << j.dump(4) << std::endl;
}
