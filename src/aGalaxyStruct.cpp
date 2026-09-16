#include "layout/aGalaxyStruct.hpp"
#include "units/aGalaxyStruct.hpp"

namespace aGalaxyStruct {
    const pas::Set<0, 255> EmptyDamageFlags = pas::constant_set<pas::Set<0, 255>>({{0, 19}}) - pas::constant_set<pas::Set<0, 255>>({{0, 19}});

} // namespace aGalaxyStruct
