#pragma once
#include "types/aGalaxyStruct.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"

namespace aConst {
    // Only Coalition manufacturers are eligible.
    inline aGalaxyStruct::TOwnerId PickRandomEquipmentOwner(std::uint32_t RandomValue) {
        return static_cast<aGalaxyStruct::TOwnerId>(aMyFunction::SeededRandomIntRange(0, 4, RandomValue));
    }

} // namespace aConst
