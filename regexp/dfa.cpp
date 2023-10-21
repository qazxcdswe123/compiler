//
// Created by yn on 14/10/2023.
//

#include "dfa.h"

std::map<State, std::map<char, State>> DFA::transition_ = {};

std::set<State> getEpsilonClosure(const std::set<State>& from) {

}

DFA::DFA(const NFA &nfa) {
    // subset construction
    std::set<State> from = {nfa.getStart()};
    std::set<State> to = getEpsilonClosure(from);
}
