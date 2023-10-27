#pragma once

#include "dfa.h"

namespace CodeGen {
    std::string MinimizedDFA2Code(const DFA &dfa);

    std::string GenerateStateCase(const MinimizedDFAState &state, const std::map<std::size_t, MinimizedDFAState> &id);

    std::string
    GenerateAccept(const std::set<MinimizedDFAState> &acceptStates, const std::map<std::size_t, MinimizedDFAState> &id);

    std::size_t getID(const MinimizedDFAState &, const std::map<std::size_t, MinimizedDFAState> &id);

    /*
     * Usage: string_format("Hello, %s!", "world");
     */
    template<typename... Args>
    std::string string_format(const std::string &format, Args &&... args);


} // namespace CodeGen