#pragma once
#include "types/fPanelMain.hpp"

namespace fPanelMain {
    extern std::uint32_t CurrentDateColor;

    extern std::uint32_t AdvancingDateColor;

    void TfPanelMain_Create(TfPanelMain* Self);

    void TfPanelMain_Destroy(TfPanelMain* Self);

    void TMessageLoopGIWithMainPanel_Create(TMessageLoopGIWithMainPanel* Self);

    void TMessageLoopGIWithMainPanel_Destroy(TMessageLoopGIWithMainPanel* Self);

} // namespace fPanelMain
