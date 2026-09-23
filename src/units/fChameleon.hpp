#pragma once
#include "types/aGalaxyStruct.hpp"
#include "types/fChameleon.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fChameleon {
    std::uint32_t ShowChameleonDialog(GI_MessageLoop::TMessageLoopGI* Parent, std::int32_t BlazerCharges, std::int32_t KellerCharges, std::int32_t TerronCharges, aGalaxyStruct::TKlingType VisualType, std::uint8_t Active, std::int32_t& Choice);

} // namespace fChameleon
