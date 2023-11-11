//
// Created by yn on 14/10/2023.
//

#include "re.h"

#include <stack>
#include <vector>

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

/*
 * Convert infix regexp re to postfix notation.
 * Insert . as explicit concatenation operator.
 * Return true if success, false otherwise.
 */
bool Regexp::re2post() {
    struct paren {
        int n_alt;
        int n_char;
    };
    int n_alt = 0, n_char = 0;
    std::stack<paren> paren_stack;
    std::string result;

    for (char c : this->re_) {
        switch (c) {
            case '(': {
                if (n_char > 1) {
                    --n_char;
                    result.push_back('\\');
                }
                if (paren_stack.size() > 100) {
                    return false;
                }
                paren p{};
                p.n_alt = n_alt;
                p.n_char = n_char;
                paren_stack.push(p);
                n_alt = 0;
                n_char = 0;
                break;
            }
            case '|': {
                if (n_char == 0) {
                    return false;
                }
                while (--n_char > 0) {
                    result.push_back('\\');
                }
                n_alt++;
                break;
            }
            case ')': {
                if (paren_stack.empty()) {
                    return false;
                }
                if (n_char == 0) {
                    return false;
                }
                while (--n_char > 0) {
                    result.push_back('\\');
                }
                for (; n_alt > 0; n_alt--) {
                    result.push_back('|');
                }
                n_alt = paren_stack.top().n_alt;
                n_char = paren_stack.top().n_char;
                paren_stack.pop();
                n_char++;
                break;
            }
            case '*':
            case '+':
            case '?': {
                if (n_char == 0) {
                    return false;
                }
                result.push_back(c);
                break;
            }
            default: {
                if (n_char > 1) {
                    --n_char;
                    result.push_back('\\');
                }
                result.push_back(c);
                n_char++;
                break;
            }
        }
    }

    if (!paren_stack.empty()) {
        return false;
    }
    while (--n_char > 0) {
        result.push_back('\\');
    }
    for (; n_alt > 0; n_alt--) {
        result.push_back('|');
    }
    this->post_re_ = result;
    return true;
}
