#pragma once
#include "types/fMods.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fMods {
    extern fMods::TfModsManager* ModsManagerScreen;

    extern std::uint32_t ModTabColor;

    extern std::uint32_t ModTabDownColor;

    extern std::uint32_t ModSelectedColor;

    extern std::uint32_t ModWarningColor;

    extern std::uint32_t ModErrorColor;

    std::int32_t ShowModsManager(GI_MessageLoop::TMessageLoopGI* Parent);

    void TfModsManager_Destroy(TfModsManager* Self);

} // namespace fMods
