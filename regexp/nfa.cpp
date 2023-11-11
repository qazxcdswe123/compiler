//
// Created by yn on 14/10/2023.
//

#include "nfa.h"

#include <stack>

std::size_t NFA::state_count_ = 0;
std::map<State, std::map<char, std::set<State>>> NFA::transition_ = {};

NFA::NFA(const char &input) {
    this->start_ = state_count_++;
    this->end_ = state_count_++;
    transition_[this->start_][input].insert(this->end_);
}

NFA::NFA(std::string_view post_re) {
    std::stack<NFA> NFAStack;

    for (auto c: post_re) {
        switch (c) {
            case '*': {
                // zero or more
                auto nfa = NFAStack.top();
                NFAStack.pop();
                epsilon_transition(nfa.getEnd(), nfa.getStart());
                epsilon_transition(nfa.getStart(), nfa.getEnd());
                NFAStack.emplace(nfa);
                break;
            }
            case '?': {
                // zero or one
                auto nfa = NFAStack.top();
                NFAStack.pop();
                epsilon_transition(nfa.getStart(), nfa.getEnd());
                NFAStack.emplace(nfa);
                break;
            }
            case '+': {
                // one or more
                auto nfa = NFAStack.top();
                NFAStack.pop();
                epsilon_transition(nfa.getEnd(), nfa.getStart());
                NFAStack.emplace(nfa);
                break;
            }
            default: {
                // alnum or other special char
                // |
                // '\'
                if (isNormalChar(c)) {
                    NFAStack.emplace(c);
                } else {
                    // |
                    // '\'
                    auto nfa1 = NFAStack.top();
                    NFAStack.pop();
                    auto nfa2 = NFAStack.top();
                    NFAStack.pop();
                    NFAStack.emplace(nfa2, nfa1, c);
                }
                break;
            }
        }
    }

    auto nfa = NFAStack.top();
    this->start_ = nfa.getStart();
    this->end_ = nfa.getEnd();
}

NFA::NFA(NFA &first, NFA &second, const char &input) {
    switch (input) {
        case '\\': {
            // concat
            epsilon_transition(first.getEnd(), second.getStart());
            this->start_ = first.getStart();
            this->end_ = second.getEnd();
            break;
        }
        case '|': {
            // or
            this->start_ = state_count_++;
            this->end_ = state_count_++;
            epsilon_transition(this->start_, first.getStart());
            epsilon_transition(this->start_, second.getStart());
            epsilon_transition(first.getEnd(), this->end_);
            epsilon_transition(second.getEnd(), this->end_);
            break;
        }
        default: {
            throw std::runtime_error(&"invalid operator"[input]);
        }
    }
}

void NFA::epsilon_transition(State from, State to) {
    transition_[from][Epsilon].insert(to);
}

State NFA::getStart() const {
    return start_;
}

State NFA::getEnd() const {
    return end_;
}

void NFA::reset() {
    state_count_ = 0;
    transition_.clear();
}

bool NFA::isNormalChar(char c) {
    return (c != '*') && (c != '?') && (c != '+') && (c != '|') && (c != '\\');
}

