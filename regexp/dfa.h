//
// Created by yn on 14/10/2023.
//

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H

#include "nfa.h"

class DFA{
public:
    static std::map<State, std::map<char, State>> transition_;

    // TODO
    void reset();

    explicit DFA(const NFA &nfa);

private:
    State start_;
    std::set<State> end_;
};


#endif //COMPILER_DFA_H
