#pragma once
#include "runtime_support.hpp"

namespace aPacket {
    std::uint8_t InitializePackageCollection();

    std::uint8_t LoadConfiguredPackages();

    void FinalizePackageCollection();

} // namespace aPacket
