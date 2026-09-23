#pragma once
#include "types/fGalaxy2.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fGalaxy2 {
    extern const pas::Array<std::uint8_t, 1, 7> GalaxyMapFriendlyShipOrder;

    extern const pas::WideString GalaxySummaryWhiteColorTag;

    void CaptureGalaxyPreview(GI_MessageLoop::TMessageLoopGI* ParentLoop);

    std::uint8_t RunGalaxyMap(GI_MessageLoop::TMessageLoopGI* ParentLoop);

    void TfGalaxy2_Create(TfGalaxy2* Self);

    void TfGalaxy2_Destroy(TfGalaxy2* Self);

} // namespace fGalaxy2
