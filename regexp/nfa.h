//
// Created by yn on 14/10/2023.
//

#ifndef COMPILER_NFA_H
#define COMPILER_NFA_H

#include <map>
#include <set>
#include <string>

typedef std::size_t State;

constexpr char Epsilon = '\0';

class NFA {
private:
    State start_;
    State end_;

public:
    static std::map<State, std::map<char, std::set<State>>> transition_;
    static std::size_t state_count_;

    explicit NFA(const char &input);

    explicit NFA(std::string_view post_re); // construct NFA from postfix regex

    NFA(NFA &first, NFA &second, const char &input); // NFA for concatenation

    static void epsilon_transition(State from, State to);

    [[nodiscard]] State getStart() const;

    [[nodiscard]] State getEnd() const;
};

#endif // COMPILER_NFA_H