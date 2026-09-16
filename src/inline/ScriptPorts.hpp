#pragma once
#include "units/ScriptPorts.hpp"

namespace ScriptPorts {
    // Explicit semantic boundary for EC_Expression's two dynamic-call asm blocks.
    inline std::uint32_t CallWords(std::uint32_t Address, const TLibraryWords& Arguments) {
        return pas::script_call_words(Address, Arguments);
    }

} // namespace ScriptPorts
