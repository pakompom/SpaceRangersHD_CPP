#pragma once
#include "types/aGalaxyStruct.hpp"

namespace aGalaxyStruct {
    // Loading this four-byte set from the DCU keeps it distinct from float zero.
    // The native aShip pools and are separate.
    extern const pas::Set<0, 255> EmptyDamageFlags;

} // namespace aGalaxyStruct
