//
// Created by yn on 14/10/2023.
//

#include "dfa.h"

#include <algorithm>
#include <queue>
#include <stack>

std::map<std::set<State>, std::map<char, std::set<State>>> DFA::transition_ = {};

std::set<std::set<State>> DFA::DFAStates_ = {};

std::set<State> DFA::getEpsilonClosure(const std::set<State> &from) {
    std::set<State> res;
    std::stack<State> st;
    for (auto state: from) {
        st.push(state);
        res.insert(state);
    }

    while (!st.empty()) {
        auto state = st.top();
        st.pop();

        // find first, or it will automatically create a new entry
        if (NFA::transition_.find(state) != NFA::transition_.end()) { // has transition
            if (NFA::transition_[state].find('\0') != NFA::transition_[state].end()) { // has epsilon transition
                for (auto next: NFA::transition_[state]['\0']) {
                    if (res.find(next) == res.end()) { // not in res
                        st.push(next);
                        res.insert(next);
                    }
                }
            }
        }
    }

    return res;
}

std::set<State> DFA::move(const std::set<State> &from, char c) {
    std::set<State> res;
    for (auto state: from) {
        if (NFA::transition_.find(state) != NFA::transition_.end()) { // has transition
            if (NFA::transition_[state].find(c) != NFA::transition_[state].end()) { // has c transition
                for (auto next: NFA::transition_[state][c]) {
                    res.insert(next);
                }
            }
        }
    }
    return res;
}


bool DFA::containEnd(const std::set<State> &states, const NFA &nfa) {
    return std::any_of(states.begin(), states.end(), [&nfa](State state) {
        return state == nfa.getEnd();
    });
}

DFA::DFA(const NFA &nfa) {
    DFAState start = {nfa.getStart()};
    start_ = getEpsilonClosure(start);
    std::queue<DFAState> q;
    q.push(start_);
    DFAStates_.insert(start_);
    std::set<DFAState> visited;
    visited.insert(start_);
    std::set<char> alphabet = getAlphabet();

    while (!q.empty()) {
        auto states = q.front();
        q.pop();
        for (auto c: alphabet) {
            auto next = move(states, c);
            if (next.empty()) {
                continue;
            }

            auto epsilonClosure = getEpsilonClosure(next);
            if (visited.find(epsilonClosure) == visited.end()) {
                visited.insert(epsilonClosure);
                q.push(epsilonClosure);
            }
            transition_[states][c] = epsilonClosure;
            DFAStates_.insert(epsilonClosure);
        }
    }

    for (const auto &states: visited) {
        if (containEnd(states, nfa)) {
            ends_.insert(states);
        }
    }
}

std::set<char> DFA::getAlphabet() {
    std::set<char> alphabet;
    for (auto &transition: NFA::transition_) {
        for (auto &c: transition.second) {
            if (c.first != '\0') {
                alphabet.insert(c.first);
            }
        }
    }
    return alphabet;
}

const std::set<State> &DFA::getStart() const {
    return start_;
}

const std::set<std::set<State>> &DFA::getEnds() const {
    return ends_;
}

void DFA::reset() {
    transition_.clear();
    DFAStates_.clear();
    minimized_transition_.clear();
    Minimized_DFAStates_.clear();
}

/*
 * Return true if states contains state
 */
template<typename T>
bool contain(const std::set<T> &states, const T &state) {
    return states.find(state) != states.end();
}

std::map<MinimizedDFAState, std::map<std::set<char>, MinimizedDFAState>> DFA::minimized_transition_ = {};

std::set<MinimizedDFAState> DFA::Minimized_DFAStates_ = {};

void DFA::minimize() {
    std::vector<std::set<DFAState>> partition = {ends_};
    std::set<DFAState> non_accepting;
    for (auto &state: DFAStates_) {
        if (!contain(ends_, state)) {
            non_accepting.insert(state);
        }
    }
    partition.push_back(non_accepting);
    auto worklist = partition;

    auto alphabet = getAlphabet();

    while (!worklist.empty()) {
        // select a set s from Worklist and remove it
        auto s = worklist.back();
        worklist.pop_back();

        // for each char in alphabet
        for (auto c: alphabet) {
            // image is the set whose member can goto s via c

            std::set<DFAState> image;
            for (auto &state: DFAStates_) {
                if (transition_.find(state) == transition_.end()) {
                    continue;
                }
                if (transition_[state].find(c) == transition_[state].end()) {
                    continue;
                }
                if (contain(s, transition_[state][c])) {
                    image.insert(state);
                }
            }

            // for each set q in Partition that has a state in Image do
            for (auto &q: partition) {
                bool q_has_state_in_image = false;
                for (auto &state: q) {
                    if (contain(image, state)) {
                        q_has_state_in_image = true;
                        break;
                    }
                }
                if (!q_has_state_in_image) {
                    continue;
                }

                // q1 is the intersection of q and Image
                std::set<DFAState> q1;
                std::set_intersection(q.begin(), q.end(), image.begin(), image.end(),
                                      std::inserter(q1, q1.begin()));

                // q2 is the difference of q and Image
                std::set<DFAState> q2;
                std::set_difference(q.begin(), q.end(), image.begin(), image.end(),
                                    std::inserter(q2, q2.begin()));

                // if q2 is not empty, split q around s and c
                // remove q from partition, add q1 and q2
                if (q2.empty()) {
                    continue;
                }
                partition.erase(std::find(partition.begin(), partition.end(), q));
                partition.push_back(q1);
                partition.push_back(q2);

                // if q in worklist, remove q and add q1 q2
                if (std::find(worklist.begin(), worklist.end(), q) != worklist.end()) {
                    worklist.erase(std::find(worklist.begin(), worklist.end(), q));
                    worklist.push_back(q1);
                    worklist.push_back(q2);
                } else if (q1.size() <= q2.size()) {
                    worklist.push_back(q1);
                } else {
                    worklist.push_back(q2);
                }

                if (s == q) {
                    break;
                }
            }
        }
    }

    // construct minimized DFA states
    for (auto &p: partition) {
        // FIXME:
        Minimized_DFAStates_.insert(p);
        // mark start and ends
        if (contain(p, start_)) {
            minimized_start_ = p;
            continue;
        }

        for (auto &e: ends_) {
            if (contain(p, e)) {
                minimized_ends_.insert(p);
                break;
            }
        }
    }

    // construct minimized transition table
    for (auto &fromMinimizedDFAState: Minimized_DFAStates_) {
        std::set<char> possible_alphabet;
        MinimizedDFAState next_minimized_state;
        bool next_found = false;
        for (auto &dfaState: fromMinimizedDFAState) {
            // find the transition the dfa state has
            if (transition_.find(dfaState) != transition_.end()) {
                for (auto &c: alphabet) {
                    if (transition_.at(dfaState).find(c) != transition_.at(dfaState).end()) {
                        // find the set of dfa state that the transition leads to
                        possible_alphabet.insert(c);
                        if (next_found) {
                            continue;
                        }

                        auto next = transition_[dfaState][c];
                        for (auto &toMinimizedDFAState: Minimized_DFAStates_) {
                            if (contain(toMinimizedDFAState, next)) {
                                next_minimized_state = toMinimizedDFAState;
                                next_found = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (next_found) {
            minimized_transition_[fromMinimizedDFAState][possible_alphabet] = next_minimized_state;
        }
    }
}

const MinimizedDFAState &DFA::getMinimizedStart() const {
    return minimized_start_;
}

const std::set<MinimizedDFAState> &DFA::getMinimizedEnds() const {
    return minimized_ends_;
}
