#include "layout/aGalaxyStruct.hpp"
#include "units/aGalaxyStruct.hpp"

namespace aGalaxyStruct {
    // Loading this four-byte set from the DCU keeps it distinct from float zero.
    // The native aShip pools and are separate.
    const pas::Set<0, 255> EmptyDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkEnergy, aGalaxyStruct::dkDroidBlock}}) - pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkEnergy, aGalaxyStruct::dkDroidBlock}});

} // namespace aGalaxyStruct
