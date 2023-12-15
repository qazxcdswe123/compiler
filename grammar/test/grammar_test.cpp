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
        auto notSLR1Reason = g.buildSLR1Table();
        EXPECT_FALSE(notSLR1Reason.has_value());

        auto procedure = g.parse("i*i+i");
        EXPECT_EQ(procedure,
                  "Debug: symbol stack: \nDebug: input: i*i+i$\nInfo: Shift to state: 2\n\nDebug: symbol stack: i\nDebug: input: *i+i$\nInfo: Reduce using: F->i\n\nDebug: symbol stack: F\nDebug: input: *i+i$\nInfo: Reduce using: T->F\n\nDebug: symbol stack: T\nDebug: input: *i+i$\nInfo: Shift to state: 10\n\nDebug: symbol stack: T*\nDebug: input: i+i$\nInfo: Shift to state: 2\n\nDebug: symbol stack: T*i\nDebug: input: +i$\nInfo: Reduce using: F->i\n\nDebug: symbol stack: T*F\nDebug: input: +i$\nInfo: Reduce using: T->T*F\n\nDebug: symbol stack: T\nDebug: input: +i$\nInfo: Reduce using: E->T\n\nDebug: symbol stack: E\nDebug: input: +i$\nInfo: Shift to state: 6\n\nDebug: symbol stack: E+\nDebug: input: i$\nInfo: Shift to state: 2\n\nDebug: symbol stack: E+i\nDebug: input: $\nInfo: Reduce using: F->i\n\nDebug: symbol stack: E+F\nDebug: input: $\nInfo: Reduce using: T->F\n\nDebug: symbol stack: E+T\nDebug: input: $\nInfo: Reduce using: E->E+T\n\nDebug: symbol stack: E\nDebug: input: $\nInfo: Accept\n\n");
    }

    {
        vector<string> inputs = {
                "S->L=R",
                "S->R",
                "L->*R",
                "L->i",
                "R->L"
        };

        // expect to say it's not SLR1 (have string in the optional type)
        Grammar g(inputs);
        auto notSLR1Reason = g.buildSLR1Table();
        EXPECT_TRUE(notSLR1Reason.has_value());
        EXPECT_EQ(notSLR1Reason.value(), "shift-reduce conflict at state 3 with symbol =");
    }
}


