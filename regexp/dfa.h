//
// Created by yn on 14/10/2023.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H

#include "nfa.h"

typedef std::set<NFAState> DFAState;

typedef std::set<DFAState> MinimizedDFAState;

class DFA {
public:
    static std::set<NFAState> getEpsilonClosure(const std::set<NFAState> &from);

    static std::set<NFAState> move(const std::set<NFAState> &from, char c);

    static bool containEnd(const std::set<NFAState> &states, const NFA &nfa);

    static std::set<DFAState> DFAStates_;

    static std::set<std::set<DFAState>> Minimized_DFAStates_;

    // transition table for DFA
    static std::map<std::set<NFAState>, std::map<char, std::set<NFAState>>> transition_;

    // transition table for Minimzed DFA
    static std::map<MinimizedDFAState, std::map<std::set<char>, MinimizedDFAState>> minimized_transition_;

    static void reset();

    /*
     * Return a pair of start and end states
     * Will modify transition_
     */

    explicit DFA(const NFA &nfa);

    [[nodiscard]] const std::set<NFAState> &getStart() const;

    [[nodiscard]] const std::set<std::set<NFAState>> &getEnds() const;

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
