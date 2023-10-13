#include "codegen.h"

namespace CodeGen {
    constexpr auto &minimizedTransitionTable = DFA::minimized_transition_;

    std::string MinimizedDFA2Code(const DFA &dfa) {
        std::map<std::size_t, MinimizedDFAState> ids;
        for (std::size_t i = 0; i < DFA::Minimized_DFAStates_.size(); i++) {
            ids[i] = *std::next(DFA::Minimized_DFAStates_.begin(), i);
        }
        const auto &start = dfa.getMinimizedStart();
        const auto &ends = dfa.getMinimizedEnds();

        std::string fmt = R"(#include <string>

bool match(const std::string &input) {
    std::size_t state = %lu;

    for (auto ch: input) {
        switch (state) {)"; // end raw string

        std::string code = string_format(fmt, getID(start, ids));
        for (const auto &state: minimizedTransitionTable) {
            MinimizedDFAState key = state.first;
            code += GenerateStateCase(key, ids);
        }

        // close bracket
        code += R"(     }
        }
)";

        code += GenerateAccept(ends, ids);

        code += R"(

        return false;
})";

        return code;
    }

    template<typename... Args>
    std::string string_format(const std::string &format, Args &&... args) {
        std::size_t sz = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...) + 1;
        std::unique_ptr<char[]> buf(new char[sz]);
        std::snprintf(buf.get(), sz, format.c_str(), std::forward<Args>(args)...);
        return std::string(buf.get(), buf.get() + sz - 1);
    }

    std::size_t getID(const MinimizedDFAState &s, const std::map<std::size_t, MinimizedDFAState> &id) {
        for (auto &p: id) {
            if (p.second == s) {
                return p.first;
            }
        }
        return -1;
    }

    std::string GenerateStateCase(const MinimizedDFAState &state, const std::map<std::size_t, MinimizedDFAState> &id) {
        std::string fmt = R"(
        case %lu: switch (ch) {)"; // end raw string
        std::string code = string_format(fmt, getID(state, id));

        // get all transition from state
        for (auto &p: minimizedTransitionTable) {
            if (p.first == state) {
                for (auto &q: p.second) {
                    for (auto c: q.first) {
                        code += string_format(R"(
        case '%c': state = %lu; break;)", c, getID(q.second, id));
                    }
                }
            }
        }

        // close code
        code += R"(
        default: return false; } break;
        )"; // end raw string

        return code;
    }

    std::string
    GenerateAccept(const std::set<MinimizedDFAState> &acceptStates,
                   const std::map<std::size_t, MinimizedDFAState> &id) {
        std::string code;

        for (const auto &as: acceptStates) {
            std::string fmt = R"(
    if (state == %lu) return true;)"; // end raw string
            code += string_format(fmt, getID(as, id));
        }

        return code;
    }
}