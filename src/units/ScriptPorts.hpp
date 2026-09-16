#pragma once
#include "types/ScriptPorts.hpp"

namespace ScriptPorts {
    // Explicit semantic boundary for EC_Expression's two dynamic-call asm blocks.
    inline std::uint32_t CallWords(std::uint32_t Address, const TLibraryWords& Arguments);

} // namespace ScriptPorts

#include "inline/ScriptPorts.hpp"
