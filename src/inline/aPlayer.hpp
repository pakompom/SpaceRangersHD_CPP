#pragma once
#include "units/aPlayer.hpp"

namespace aPlayer {
    inline TPlayer* GetPlayer() {
        return reinterpret_cast<TPlayer*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EncodedPlayer ^
            0xb1cd15d3u)));
    }

} // namespace aPlayer
