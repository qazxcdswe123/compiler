//
// Created by yn on 2023-12-10.
//

#include "gtest/gtest.h"
#include "../grammar.h"

#include <iostream>

TEST(GrammarTest, FIRSTANDFOLLOW) {
    vector<string> inputs = {
            "E->E+T",
            "E->T",
            "T->T*F",
            "T->F",
            "F->(E)",
            "F->i"
    };

    Grammar g(inputs);

    EXPECT_EQ(g.getFirstSet('E'), (set<char>{'(', 'i'}));
    EXPECT_EQ(g.getFirstSet('T'), (set<char>{'(', 'i'}));
    EXPECT_EQ(g.getFirstSet('F'), (set<char>{'(', 'i'}));

    EXPECT_EQ(g.getFollowSet('E'), (set<char>{'+', ')', '$'}));
    EXPECT_EQ(g.getFollowSet('T'), (set<char>{'+', '*', ')', '$'}));
    EXPECT_EQ(g.getFollowSet('F'), (set<char>{'+', '*', ')', '$'}));
}

TEST(GrammarTest, LR0) {
    vector<string> inputs = {
            "E->E+T",
            "E->T",
            "T->T*F",
            "T->F",
            "F->(E)",
            "F->i"
    };

    Grammar g(inputs);
    EXPECT_EQ(g.getLr0Items().size(), 18);
    EXPECT_EQ(g.getLr0DfaStates().size(), 12);

    // capture stdout
    testing::internal::CaptureStdout();

    using std::cout, std::endl;
    for (const auto &state: g.getLr0DfaStates()) {
        cout << "State " << state.id << endl;
        for (const auto &item: state.items) {
            cout << item.left << "->";
            for (int i = 0; i < item.right.size(); i++) {
                if (i == item.dotPos) {
                    cout << ".";
                }
                cout << item.right[i];
            }
            if (item.dotPos == item.right.size()) {
                cout << ".";
            }
            cout << endl;
        }
        cout << endl;
    }

    string output = testing::internal::GetCapturedStdout();
    string expected = R"(State 0
'->.E
E->.E+T
E->.T
F->.(E)
F->.i
T->.F
T->.T*F

State 3
'->E.
E->E.+T

State 1
E->.E+T
E->.T
F->.(E)
F->(.E)
F->.i
T->.F
T->.T*F

State 7
E->E.+T
F->(E.)

State 6
E->E+.T
F->.(E)
F->.i
T->.F
T->.T*F

State 9
E->E+T.
T->T.*F

State 5
E->T.
T->T.*F

State 10
F->.(E)
F->.i
T->T*.F

State 8
F->(E).

State 2
F->i.

State 4
T->F.

State 11
T->T*F.

)";
    EXPECT_EQ(output, expected);
}

TEST(GrammarTest, SLR1) {
    {
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

    {
        vector<string> inputs = {
                "S->L=R",
                "S->R",
                "L->*R",
                "L->i",
                "R->L"
        };

        // expect to throw
        EXPECT_THROW(Grammar g(inputs), std::invalid_argument);
    }
}


