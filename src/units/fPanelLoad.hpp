#pragma once
#include "types/fPanelLoad.hpp"

namespace fPanelLoad {
    extern fPanelLoad::TfPanelLoad* ActiveLoadPanel;

    void TfPanelLoad_Create(TfPanelLoad* Self);

    void TfPanelLoad_Destroy(TfPanelLoad* Self);

} // namespace fPanelLoad
