//
// Created by yn on 2023-12-08.
//

#ifndef COMPILER_GRAMMAR_H
#define COMPILER_GRAMMAR_H

#include <map>
#include <set>
#include <string>
#include <optional>

using std::string, std::map, std::set, std::vector, std::size_t, std::pair;

const char epsilon = '@';

enum class ActionType {
    ACCEPT,
    REDUCE,
    SHIFT
};

struct Action {
    ActionType type;
    int to;
};

struct LR0Item {
    char left;
    string right;
    size_t dotPos;

    bool operator<(const LR0Item &rhs) const {
        if (left < rhs.left)
            return true;
        if (rhs.left < left)
            return false;
        if (right < rhs.right)
            return true;
        if (rhs.right < right)
            return false;
        return dotPos < rhs.dotPos;
    }
};

struct State {
    set<LR0Item> items;
    int id;

    bool operator<(const State &rhs) const {
        return items < rhs.items;
    }
};

class Grammar {
public:
    explicit Grammar(const vector<string> &inputs);

    [[nodiscard]] const set<char> &getFirstSet(char) const;

    [[nodiscard]] const set<char> &getFollowSet(char) const;

    [[nodiscard]] const set<State> &getLr0DfaStates() const {
        return lr0DFAStates_;
    }

    [[nodiscard]] const map<char, set<char>> &getFirstSet1() const {
        return firstSet_;
    }

    [[nodiscard]] const map<char, set<char>> &getFollowSet1() const {
        return followSet_;
    }

    string parse(const string &input);

    std::optional<string> buildSLR1Table();

private:
    // single capital letter for non-terminal
    // non-capital letter for terminal
    // @ for epsilon
    // the first capital letter on the left is the start symbol

    set<char> nonTerminals_;
    set<char> terminals_;
    map<char, set<string>> productions_;
    char startSymbol_;

    map<char, set<char>> firstSet_;
    map<char, set<char>> followSet_;

    set<LR0Item> lr0Items_;
    set<State> lr0DFAStates_;
    // Transition between states
    map<pair<size_t, char>, size_t> gotoTable_;
    // Action table
    map<pair<size_t, char>, Action> actionTable_;


    void buildFirstSet();
    void buildFollowSet();
    void buildLR0DFA();

    // closure of a set of LR0 items
    set<LR0Item> closure(const set<LR0Item> &items);

    // goto of a set of LR0 items
    set<LR0Item> goTo(const set<LR0Item> &items, char symbol);
};


#endif //COMPILER_GRAMMAR_H
