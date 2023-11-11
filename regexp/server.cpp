#include "../include/json.hpp"
#include "../include/httplib.h"

#include "re.h"
#include "codegen.h"

using json = nlohmann::json;
using namespace httplib;

void allReset() {
    NFA::reset();
    DFA::reset();
}

void handleRE2NFA(const Request &req, Response &res) {
    allReset();

    std::string re = req.body;
    if (re.empty()) {
        return;
    }
    // substitute '\n' with |
    for (auto &c: re) {
        if (c == '\n') {
            c = '|';
        }
    }
    std::cout << "re: " << re << std::endl;
    Regexp regexp(re);
    std::cout << "post_re: " << regexp.get_post_re() << std::endl;
    NFA nfa(regexp.get_post_re());
    std::cout << "NFA state count: " << NFA::state_count_ << std::endl;

    // to json
    auto transition = NFA::transition_;
    auto start = nfa.getStart();
    auto end = nfa.getEnd();
    json j;

    for (auto &pair: transition) {
        for (auto &pair2: pair.second) {
            for (auto &state: pair2.second) {
                j.push_back({
                                    {"is_start", pair.first == start},
                                    {"is_end",   state == end},
                                    {"from",     pair.first},
                                    {"to",       state},
                                    {"char",     pair2.first}
                            });
            }
        }
    }

    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(j.dump(), "application/json");
}

void handleRE2DFA(const Request &req, Response &res) {
    allReset();

    std::string re = req.body;
    if (re.empty()) {
        return;
    }
    // substitute '\n' with |
    for (auto &c: re) {
        if (c == '\n') {
            c = '|';
        }
    }
    std::cout << "re: " << re << std::endl;
    Regexp regexp(re);
    std::cout << "post_re: " << regexp.get_post_re() << std::endl;
    NFA nfa(regexp.get_post_re());
    std::cout << "NFA state count: " << NFA::state_count_ << std::endl;
    DFA dfa(nfa);
    std::cout << "DFA state count: " << DFA::DFAStates_.size() << std::endl;

    // to json
    auto transition = DFA::transition_;
    const auto &start = dfa.getStart();
    auto ends = dfa.getEnds();
    json j;

    for (auto &pair: transition) {
        for (auto &pair2: pair.second) {
            j.push_back({
                                {"is_start", pair.first == start},
                                {"is_end",   ends.find(pair2.second) != ends.end()},
                                {"from",     pair.first},
                                {"to",       pair2.second},
                                {"char",     pair2.first}
                        });
        }
    }

    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(j.dump(), "application/json");
}

void handleRE2MiniDFA(const Request &req, Response &res) {
    allReset();

    std::string re = req.body;
    if (re.empty()) {
        return;
    }
    // substitute '\n' with |
    for (auto &c: re) {
        if (c == '\n') {
            c = '|';
        }
    }
    std::cout << "re: " << re << std::endl;
    Regexp regexp(re);
    std::cout << "post_re: " << regexp.get_post_re() << std::endl;
    NFA nfa(regexp.get_post_re());
    std::cout << "NFA state count: " << NFA::state_count_ << std::endl;
    DFA dfa(nfa);
    std::cout << "DFA state count: " << DFA::DFAStates_.size() << std::endl;
    dfa.minimize();
    std::cout << "MiniDFA state count: " << DFA::DFAStates_.size() << std::endl;

    // to json
    auto transition = DFA::minimized_transition_;
    const auto &start = dfa.getMinimizedStart();
    auto ends = dfa.getMinimizedEnds();
    json j;

    for (auto &pair: transition) {
        for (auto &pair2: pair.second) {
            j.push_back({
                                {"is_start", pair.first == start},
                                {"is_end",   ends.find(pair2.second) != ends.end()},
                                {"from",     pair.first},
                                {"to",       pair2.second},
                                {"char",     pair2.first}
                        });
        }
    }

    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(j.dump(), "application/json");
}

void handleRE2Code(const Request &req, Response &res) {
    allReset();

    std::string re = req.body;
    if (re.empty()) {
        return;
    }
    // substitute '\n' with |
    for (auto &c: re) {
        if (c == '\n') {
            c = '|';
        }
    }
    std::cout << "re: " << re << std::endl;
    Regexp regexp(re);
    std::cout << "post_re: " << regexp.get_post_re() << std::endl;
    NFA nfa(regexp.get_post_re());
    std::cout << "NFA state count: " << NFA::state_count_ << std::endl;
    DFA dfa(nfa);
    std::cout << "DFA state count: " << DFA::DFAStates_.size() << std::endl;
    dfa.minimize();
    std::cout << "MiniDFA state count: " << DFA::DFAStates_.size() << std::endl;
    std::string result = CodeGen::MinimizedDFA2Code(dfa);

    json j;
    j["code"] = result;

    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(j.dump(), "application/json");
}

// NFA to DFA test
int main() {
    httplib::Server svr;

    svr.Post("/re2nfa", handleRE2NFA);
    svr.Post("/re2dfa", handleRE2DFA);
    svr.Post("/re2minidfa", handleRE2MiniDFA);
    svr.Post("/re2code", handleRE2Code);

    std::cout << "Starting api server on http://localhost:8080..." << std::endl;
    std::cout << "Waiting for requests..." << std::endl;
    svr.listen("0.0.0.0", 8080);
}
