#pragma once
#include "units/ScriptPorts.hpp"

namespace ScriptPorts {
    inline std::uint32_t CallWords(std::uint32_t Address, const TLibraryWords& Arguments) {
        return pas::script_call_words(Address, Arguments);
    }

} // namespace ScriptPorts
