//
// Created by yn on 14/10/2023.
//

#include "re.h"

#include <stack>
#include <map>

void Regexp::normalize_char_class(std::string_view re) {
    // assume on one char class in re
    // sanity check
    int paren_count = 0;
    int left_index = 0;
    int right_index = 0;
    std::vector<int> dash_index;

    for (int i = 0; i < re.size(); i++) {
        if (re[i] == '[') {
            paren_count++;
            left_index = i;
        } else if (re[i] == ']') {
            right_index = i;
        } else if (re[i] == '-') {
            dash_index.push_back(i);
        }
    }
    if (paren_count > 1) {
        throw std::runtime_error("More than one char class in re");
    }
    if (paren_count == 0) { // no char class, skip
        return;
    }

    std::string charClassExpanded;
    std::string result;

    // copy content before [ to result
    result.append(re.substr(0, left_index));

    // expand [0-9] to [0123456789]
    if (!dash_index.empty()) {
        for (int i: dash_index) {
            char left = re[i - 1];
            char right = re[i + 1];
            if (left > right) {
                std::swap(left, right);
            }
            for (char c = left; c <= right; c++) {
                charClassExpanded.push_back(c);
            }
        }
    } else { // no dash, just add
        charClassExpanded.append(re.substr(left_index + 1, right_index - left_index - 1));
    }

    // expand [a-z] to (a|b|c|...|z)
    result.append("(");
    for (int i = 0; i < charClassExpanded.size(); i++) {
        result.push_back(charClassExpanded[i]);
        if (i != charClassExpanded.size() - 1) {
            result.append("|");
        }
    }
    result.append(")");

    // copy content after ] to result
    result.append(re.substr(right_index + 1, re.size() - right_index));

    this->re_ = result;
}

bool Regexp::re2post() {
    std::stack<char> paren;
    std::string result;

    for (int i = 0; i < this->re_.size(); i++) {
        char c = this->re_[i];
        switch (c) {
            case '(':
                paren.push(c);
                break;
            case ')':
                while (!paren.empty() && paren.top() != '(') {
                    result.push_back(paren.top());
                    paren.pop();
                }
                if (paren.empty()) {
                    // mismatched parentheses
                    return false;
                }
                paren.pop();
                break;
            case '*':
            case '+':
            case '?':
                if (i == 0) {
                    // * + ? cannot be the first char
                    return false;
                }
                result.push_back(c);
                break;
            case '|':
                while (!paren.empty() && paren.top() != '(') {
                    result.push_back(paren.top());
                    paren.pop();
                }
                paren.push(c);
                break;
            default:
                result.push_back(c);
                break;
        }
    }

    while (!paren.empty()) {
        result.push_back(paren.top());
        paren.pop();
    }

    this->post_re_ = result;
    return true;
}
