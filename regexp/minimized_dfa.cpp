//
// Created by yn on 25/10/2023.
//

#include "minimized_dfa.h"

namespace minimized_dfa {
    State start_state;
    std::set<State> end_states;

    std::map<State, std::map<char, State>> transitions;

    void reset() {
        start_state = 0;
        end_states.clear();
        transitions.clear();
    }

    void DFA2MDFA(const DFA &dfa) {
H
    }
} // minimized_dfa