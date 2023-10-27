//
// Created by yn on 25/10/2023.
//

#ifndef COMPILER_MINIMIZED_DFA_H
#define COMPILER_MINIMIZED_DFA_H

#include "dfa.h"

namespace minimized_dfa {
    extern State start_state;
    extern std::set<State> end_states;

    extern std::map<State, std::map<char, State>> transitions;

    void reset();

    void DFA2MDFA(const DFA &dfa);

} // minimized_dfa

#endif //COMPILER_MINIMIZED_DFA_H
