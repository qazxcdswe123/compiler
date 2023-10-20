#include <gtest/gtest.h>

#include "../re.h"

TEST(Regexp, CharClass) {
    std::string input_re1 = "a[bcd]e";
    std::string expected_re1 = "a(b|c|d)e";
    auto r = Regexp(input_re1);
    EXPECT_EQ(r.get_re(), expected_re1);

    std::string input_re2 = "a[0-9]e";
    std::string expected_re2 = "a(0|1|2|3|4|5|6|7|8|9)e";
    auto r2 = Regexp(input_re2);
    EXPECT_EQ(r2.get_re(), expected_re2);

    std::string input_re3 = "a[0-9a-zA-Z]e";
    std::string expected_re3 = "a(0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p"
                               "|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N"
                               "|O|P|Q|R|S|T|U|V|W|X|Y|Z)e";
    auto r3 = Regexp(input_re3);
    EXPECT_EQ(r3.get_re(), expected_re3);
}

TEST(RegexTest, RegexToPostfix) {
    std::string input_re1 = "a(b|c)*d";
    std::string expected_post_re1 = "abc|*.d.";
    auto r = Regexp(input_re1);
    EXPECT_EQ(r.get_post_re(), expected_post_re1);

    std::string input_re2 = "a+b*(c?|d)+";
    std::string expected_post_re2 = "a+b*.c?d|+.";
    auto r2 = Regexp(input_re2);
    EXPECT_EQ(r2.get_post_re(), expected_post_re2);
}
