//
// Created by yn on 2023-12-08.
//

#include "grammar.h"

#include "../include/json.hpp"
#include "../include/httplib.h"

using json = nlohmann::json;

int main() {
//    vector<std::string> inputs = {
//E->E+T
//E->T
//T->T*F
//T->F
//F->(E)
//F->i
//    };
//
//    Grammar g(inputs);
//    g.parse("i*i+i");
    httplib::Server svr;

    svr.Post("/grammar", [](const httplib::Request &req, httplib::Response &res) {
        // handle form-data, key1: grammar, key2: sentence
        auto rules = req.get_param_value("grammar");
        std::cout << "Grammar: " << rules << std::endl;
        auto sentence = req.get_param_value("sentence");
        std::cout << "Sentence: " << sentence << std::endl;

        // parse grammar, split by \n
        vector<string> inputs;
        string tmp;
        for (auto c: rules) {
            if (c == '\n') {
                inputs.push_back(tmp);
                tmp.clear();
            } else {
                tmp.push_back(c);
            }
        }
        if (!tmp.empty()) {
            inputs.push_back(tmp);
        }

        json j;
        // parse grammar
        Grammar g(inputs);
        auto firstSet = g.getFirstSet1();
        j["firstSet"] = firstSet;

        auto followSet = g.getFollowSet1();
        j["followSet"] = followSet;

        auto DFAStates = g.getLr0DfaStates();
        for (auto &state: DFAStates) {
            for (auto &item: state.items) {
                j["DFAStates"][state.id].push_back({
                                                           {"left",   item.left},
                                                           {"right",  item.right},
                                                           {"dotPos", item.dotPos}
                                                   });
            }
        }

        auto notSLR1Reason = g.buildSLR1Table();
        // if it is nullopt, then it is SLR1
        if (notSLR1Reason.has_value()) {
            j["isSLR1"] = false;
            j["notSLR1Reason"] = notSLR1Reason.value();
        } else {
            j["isSLR1"] = true;
        }

        j["nonTerminals"] = g.getNonTerminals();
        // terminals include $, which is the end of the sentence
        j["terminals"] = g.getTerminals();
        j["terminals"].push_back('$');
        auto actionTable = g.getActionTable();
        for (auto &action: actionTable) {
            using std::to_string;
            j["actionTable"].push_back({
                                               {"state",  to_string(action.first.first)},
                                               {"symbol", string(1, action.first.second)},
                                               // lambda for action to json
                                               {"action", [&action, &g]() -> string {
                                                   switch (action.second.type) {
                                                       case ActionType::SHIFT:
                                                           return "s" + to_string(action.second.to);
                                                       case ActionType::REDUCE:
                                                           return "r" + to_string(action.second.to);
                                                       case ActionType::ACCEPT:
                                                           return "acc";
                                                       default:
                                                           return "error";
                                                   }
                                               }()}
                                       });
        }

        auto gotoTable = g.getGotoTable();
        for (auto &go: gotoTable) {
            j["gotoTable"].push_back({
                                             {"state",  go.first.first},
                                             {"symbol", string(1, go.first.second)},
                                             {"goto",   go.second}
                                     });
        }

        auto procedure = g.parse(sentence);
        j["procedure"] = procedure;

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(j.dump(), "application/json");
    });

    std::cout << "Starting server on port 8080..." << std::endl;
    std::cout << "Waiting for requests..." << std::endl;
    svr.listen("0.0.0.0", 8080);
}