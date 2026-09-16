#pragma once
#include "types/ScriptPorts.hpp"

namespace ScriptPorts {
    inline std::uint32_t CallWords(std::uint32_t Address, const TLibraryWords& Arguments);

} // namespace ScriptPorts

#include "inline/ScriptPorts.hpp"
