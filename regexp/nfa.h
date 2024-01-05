//
// Created by yn on 14/10/2023.
//

#ifndef COMPILER_NFA_H
#define COMPILER_NFA_H

#include <map>
#include <set>
#include <string>
#include <stdexcept>

typedef std::size_t NFAState;

constexpr char Epsilon = '\0';

class NFA {
private:
    NFAState start_;
    NFAState end_;

public:
    static std::map<NFAState, std::map<char, std::set<NFAState>>> transition_;
    static std::size_t state_count_;

    static void reset();

    explicit NFA(const char &input); // construct NFA for single char

    explicit NFA(std::string_view post_re); // construct NFA from postfix regex

    NFA(NFA &first, NFA &second, const char &input); // NFA for concatenation

    static void epsilon_transition(NFAState from, NFAState to);

    [[nodiscard]] NFAState getStart() const;

    [[nodiscard]] NFAState getEnd() const;

    static bool isNormalChar(char c);
};

#endif // COMPILER_NFA_H