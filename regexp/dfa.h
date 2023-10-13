//
// Created by yn on 14/10/2023.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H

#include "nfa.h"

typedef std::set<State> DFAState;

typedef std::set<DFAState> MinimizedDFAState;

class DFA {
public:
    static std::set<State> getEpsilonClosure(const std::set<State> &from);

    static std::set<State> move(const std::set<State> &from, char c);

    static bool containEnd(const std::set<State> &states, const NFA &nfa);

    static std::set<std::set<State>> DFAStates_;

    static std::set<std::set<DFAState>> Minimized_DFAStates_;

    // transition table for DFA
    static std::map<std::set<State>, std::map<char, std::set<State>>> transition_;

    static std::map<MinimizedDFAState, std::map<std::set<char>, std::set<DFAState>>> minimized_transition_;

    static void reset();

    /*
     * Return a pair of start and end states
     * Will modify transition_
     */

    explicit DFA(const NFA &nfa);

    [[nodiscard]] const std::set<State> &getStart() const;

    [[nodiscard]] const std::set<std::set<State>> &getEnds() const;

    void minimize();

private:
    DFAState start_;
    std::set<DFAState> ends_;
    MinimizedDFAState minimized_start_;
    std::set<MinimizedDFAState> minimized_ends_;

    [[nodiscard]] static std::set<char> getAlphabet();

public:
    [[nodiscard]] const MinimizedDFAState &getMinimizedStart() const;

    [[nodiscard]] const std::set<MinimizedDFAState> &getMinimizedEnds() const;
};

#endif //COMPILER_DFA_H
