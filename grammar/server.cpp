//
// Created by yn on 2023-12-08.
//

#include "grammar.h"

#include <iostream>

int main() {
    vector<string> inputs = {
            "E->E+T",
            "E->T",
            "T->T*F",
            "T->F",
            "F->(E)",
            "F->i"
    };

    Grammar g(inputs);
    g.parse("i*i+i");
}