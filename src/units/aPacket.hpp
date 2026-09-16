#pragma once
#include "runtime_support.hpp"

namespace aPacket {
    // Creates the loose-file package and returns true regardless of OpenAllPackages' result.
    std::uint8_t InitializePackageCollection();

    // Appends packages in language-mod, language, mod, then base order; existing entries are retained.
    std::uint8_t LoadConfiguredPackages();

    // Requires an initialized package collection.
    void FinalizePackageCollection();

} // namespace aPacket
