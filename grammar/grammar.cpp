//
// Created by yn on 2023-12-08.
//

#include "grammar.h"

#include <stack>
#include <iostream>

Grammar::Grammar(const vector<string> &inputs) {
    for (int i = 0; i < inputs.size(); i++) {
        const auto &input = inputs[i];

        // split the input into left and right
        auto pos = input.find("->");
        if (pos == string::npos) {
            throw std::invalid_argument("invalid input grammar");
        }
        string left = input.substr(0, pos);
        string right = input.substr(pos + 2);

        productions_[left[0]].insert(right);

        if (i == 0) {
            startSymbol_ = left[0];
        }

        nonTerminals_.insert(left[0]);

        for (const auto &ch: right) {
            if (isupper(ch)) {
                nonTerminals_.insert(ch);
            } else {
                terminals_.insert(ch);
            }
        }
    }

    buildFirstSet();
    buildFollowSet();
    buildLR0DFA();
}

void Grammar::buildFirstSet() {
    // first set of terminal is itself
    // all non-capital letter on left or right is terminal
    for (const auto &item: productions_) {
        for (const auto &right: item.second) {
            for (const auto &ch: right) {
                if (isupper(ch)) {
                    continue;
                } else {
                    // symbol such as ( ) @ are terminal
                    firstSet_[ch].insert(ch);
                }
            }
        }

        //  Initialize first sets for all non-terminals
        firstSet_[item.first] = {};
    }

    // first set of non-terminal
    bool changing = true;
    while (changing) {
        changing = false;
        for (const auto &item: productions_) {
            char nonTerminal = item.first;
            for (const auto &rhs: item.second) {
                bool foundEpsilon = true;
                size_t before = firstSet_[nonTerminal].size();
                size_t after;

                for (const auto &symbol: rhs) {
                    // if the symbol is a non-terminal
                    if (isupper(symbol)) {
                        // copy all elements except epsilon
                        for (const auto &ch: firstSet_[symbol]) {
                            if (ch != epsilon) {
                                firstSet_[nonTerminal].insert(ch);
                            }
                        }
                        after = firstSet_[nonTerminal].size();

                        // If nothing is added or epsilon is not in FIRST(symbol), then stop
                        if (before == after || firstSet_[symbol].find(epsilon) == firstSet_[symbol].end()) {
                            foundEpsilon = false;
                            break;
                        }
                    } else {
                        // if the symbol is a terminal
                        firstSet_[nonTerminal].insert(symbol);
                        foundEpsilon = false;
                        break;
                    }
                }

                // If epsilon was found in all symbols of production, add epsilon to the non-terminal
                if (foundEpsilon) {
                    firstSet_[nonTerminal].insert(epsilon);
                }

                // check if the first set of non-terminal is changed
                if (before != firstSet_[nonTerminal].size()) {
                    changing = true;
                }
            }
        }
    }
}

void Grammar::buildFollowSet() {
    // Initialize follow sets for all non-terminals
    for (const auto &item: productions_) {
        followSet_[item.first] = {};
    }

    // The follow set of the start symbol contains $
    followSet_[startSymbol_].insert('$');

    bool changing = true;
    while (changing) {
        changing = false;

        for (const auto &item: productions_) {
            char left = item.first;

            for (const auto &right: item.second) {
                set<char> trailer = followSet_[left];

                for (int i = right.size() - 1; i >= 0; i--) {
                    char symbol = right[i];

                    // if the symbol is a non-terminal
                    if (isupper(symbol)) {
                        // add trailer to the follow set of the symbol
                        size_t before = followSet_[symbol].size();
                        followSet_[symbol].insert(trailer.begin(), trailer.end());
                        size_t after = followSet_[symbol].size();
                        if (before != after) {
                            changing = true;
                        }

                        // if the symbol can derive epsilon
                        if (firstSet_[symbol].find(epsilon) != firstSet_[symbol].end()) {
                            // add the first set of the symbol to the trailer
                            for (const auto &ch: firstSet_[symbol]) {
                                if (ch != epsilon) {
                                    trailer.insert(ch);
                                }
                            }
                        } else {
                            // if the symbol cannot derive epsilon
                            trailer = firstSet_[symbol];
                        }
                    } else {
                        // if the symbol is a terminal
                        trailer = {symbol};
                    }
                }
            }
        }
    }
}

set<LR0Item> Grammar::closure(const set<LR0Item> &items) {
    set<LR0Item> closure = items;

    bool changing = true;
    while (changing) {
        size_t before = closure.size();
        set<LR0Item> newItems;

        for (const auto &item: closure) {
            // if the dot is at the end of the right hand side
            if (item.dotPos == item.right.size()) {
                continue;
            }

            char symbolAfterDot = item.right[item.dotPos];

            // if the next symbol is a non-terminal
            if (isupper(symbolAfterDot)) {
                for (const auto &rhs: productions_[symbolAfterDot]) {
                    LR0Item newItem = {symbolAfterDot, rhs, 0};

                    // Add new item if it is not in the closure
                    if (closure.find(newItem) == closure.end()) {
                        newItems.insert(newItem);
                        changing = true;
                    }
                }
            }
        }

        closure.insert(newItems.begin(), newItems.end());
        size_t after = closure.size();
        if (before == after) {
            changing = false;
        }
    }

    return closure;
}

const set<char> &Grammar::getFirstSet(char c) const {
    return firstSet_.at(c);
}

const set<char> &Grammar::getFollowSet(char c) const {
    return followSet_.at(c);
}

void Grammar::buildLR0DFA() {
    // closure of the start state
    // use ' to represent the augmented expression grammar
    State startState = State{closure({{'\'', string(1, startSymbol_), 0}})};
    int id = 0;
    startState.id = id++;
    lr0DFAStates_.insert(startState);

    bool changing = true;
    while (changing) {
        size_t before = lr0DFAStates_.size();

        for (const auto &state: lr0DFAStates_) {
            for (const auto &symbol: terminals_) {
                set<LR0Item> goTo = Grammar::goTo(state.items, symbol);
                if (!goTo.empty()) {
                    State newState = State{goTo};
                    auto it = lr0DFAStates_.find(newState);
                    if (it == lr0DFAStates_.end()) {
                        newState.id = id++;
                        lr0DFAStates_.insert(newState);
                    }
                }
            }

            for (const auto &symbol: nonTerminals_) {
                set<LR0Item> goTo = Grammar::goTo(state.items, symbol);
                if (!goTo.empty()) {
                    State newState = State{goTo};
                    auto it = lr0DFAStates_.find(newState);
                    if (it == lr0DFAStates_.end()) {
                        newState.id = id++;
                        lr0DFAStates_.insert(newState);
                    }
                }
            }
        }

        size_t after = lr0DFAStates_.size();
        if (before == after) {
            changing = false;
        }
    }

    // build goto table
    for (const auto &state: lr0DFAStates_) {
        for (const auto &symbol: nonTerminals_) {
            set<LR0Item> goTo = Grammar::goTo(state.items, symbol);
            if (!goTo.empty()) {
                State newState = State{goTo};
                auto it = lr0DFAStates_.find(newState);
                if (it != lr0DFAStates_.end()) {
                    gotoTable_[{state.id, symbol}] = it->id;
                }
            }
        }
    }
}

set<LR0Item> Grammar::goTo(const set<LR0Item> &items, char symbol) {
    set<LR0Item> goTo;

    for (const auto &item: items) {
        // if the dot is at the end of the right hand side
        if (item.dotPos == item.right.size()) {
            continue;
        }

        char symbolAfterDot = item.right[item.dotPos];

        // if the next symbol is the symbol we want to go to
        if (symbolAfterDot == symbol) {
            LR0Item newItem = {item.left, item.right, item.dotPos + 1};
            goTo.insert(newItem);
        }
    }

    return closure(goTo);
}

std::optional<string> Grammar::buildSLR1Table() {
    // build action table
    for (const auto &state: lr0DFAStates_) {
        for (const auto &item: state.items) {
            // if the dot is at the end of the right hand side
            if (item.dotPos == item.right.size()) {
                // if the item is the start item
                if (item.left == '\'' && item.right == string(1, startSymbol_)) {
                    if (actionTable_.find({state.id, '$'}) != actionTable_.end()) {
                        string reason = "conflict at state " + std::to_string(state.id) + " with symbol $";
                        return reason;
                    }

                    actionTable_[{state.id, '$'}] = Action{ActionType::ACCEPT, 0};
                } else {
                    // the item is not the start item
                    for (const auto &symbol: followSet_[item.left]) {
                        if (actionTable_.find({state.id, symbol}) != actionTable_.end()) {
                            string reason =
                                    "reduce-reduce conflict at state " + std::to_string(state.id) + " with symbol " +
                                    symbol;
                            return reason;
                        }

                        int productionId = 0;
                        bool found = false;
                        for (const auto &[head, productions]: productions_) {
                            for (const auto &production: productions) {
                                if (head == item.left && production == item.right) {
                                    found = true;
                                    break;
                                }
                                productionId++;
                            }
                            if (found) {
                                break;
                            }
                        }

                        actionTable_[{state.id, symbol}] = Action{ActionType::REDUCE, productionId};
                    }
                }
            } else {
                char symbolAfterDot = item.right[item.dotPos];

                // if the next symbol is a terminal
                if (!isupper(symbolAfterDot)) {
                    set<LR0Item> goTo = Grammar::goTo(state.items, symbolAfterDot);
                    if (!goTo.empty()) {
                        State newState = State{goTo};
                        auto it = lr0DFAStates_.find(newState);
                        if (it != lr0DFAStates_.end()) {
                            if (actionTable_.find({state.id, symbolAfterDot}) != actionTable_.end()) {
                                string reason =
                                        "shift-reduce conflict at state " + std::to_string(state.id) + " with symbol " +
                                        symbolAfterDot;
                                return reason;
                            }

                            actionTable_[{state.id, symbolAfterDot}] = Action{ActionType::SHIFT, it->id};
                        }
                    }
                }
            }
        }
    }

    // success
    return std::nullopt;
}

string Grammar::parse(const string &input) {
    using std::stack, std::cout, std::endl;
    stack<pair<size_t, char>> stateStack;
    stateStack.emplace(0, '$');
    vector<char> symbolStack;

    string inputCopy = input + '$';
    size_t inputPos = 0;

    string procedure;

    while (!stateStack.empty()) {
        size_t state = stateStack.top().first;

        string symbolStackStr(symbolStack.begin(), symbolStack.end());
        procedure += "Debug: symbol stack: " + symbolStackStr + "\n";
        procedure += "Debug: input: " + inputCopy.substr(inputPos) + "\n";

        char inputSymbol = inputCopy[inputPos];
        auto it = actionTable_.find({state, inputSymbol});
        if (it == actionTable_.end()) {
            procedure += "Error: cannot find action\n\n";
            return procedure;
        }

        Action action = it->second;
        if (action.type == ActionType::ACCEPT) {
            procedure += "Info: Accept\n\n";
            return procedure;
        } else if (action.type == ActionType::REDUCE) {
            // the production must be on both sides of the state and the dotPos must be at the end

            // find the id that match the state
            auto it2 = std::find_if(lr0DFAStates_.begin(), lr0DFAStates_.end(), [state](const State &s) {
                return s.id == state;
            });
            if (it2 == lr0DFAStates_.end()) {
                procedure += "Error: cannot find state " + std::to_string(state) + "\n\n";
                return procedure;
            }

            int productionIndex = action.to;
            bool found = false;
            for (const auto &setOfProductions: productions_) {
                for (const auto &production: setOfProductions.second) {
                    if (productionIndex == 0) {
                        found = true;

                        // reduce
                        procedure += "Info: Reduce using: " + string(1, setOfProductions.first) + "->" + production + "\n\n";
                        for (int i = 0; i < production.size(); i++) {
                            symbolStack.pop_back();
                            stateStack.pop();
                        }

                        // push the non-terminal
                        symbolStack.push_back(setOfProductions.first);

                        // push the goto state
                        auto it3 = gotoTable_.find({stateStack.top().first, setOfProductions.first});
                        if (it3 == gotoTable_.end()) {
                            procedure += "Error: cannot find goto state\n\n";
                            return procedure;
                        }
                        stateStack.emplace(it3->second, setOfProductions.first);

                        break;
                    }
                    productionIndex--;
                }

                if (found) {
                    break;
                }
            }

        } else if (action.type == ActionType::SHIFT) {
            procedure += "Info: Shift to state: " + std::to_string(action.to) + "\n\n";
            stateStack.emplace(action.to, inputSymbol);
            symbolStack.push_back(inputSymbol);
            inputPos++;
        }
    }
    return procedure;
}
