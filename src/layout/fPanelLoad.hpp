#pragma once
#include "types/fPanelLoad.hpp"

namespace fPanelLoad {
    static_assert(sizeof(void*) != 4 || sizeof(fPanelLoad::TfPanelLoad) == 160);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, Screen) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ProgressSegments) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, LayoutAdjusted) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, BackgroundImage) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ShipPanelImage) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, LoadAnimation) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, AnimationText) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ProgressLabel) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ProgressBar) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, BackgroundRestTop) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ShipPanelRestTop) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, AnimationRestTop) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, AnimationTextRestTop) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ProgressLabelRestTop) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ProgressBarRestTop) == 124);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ShutterTimer) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ShutterOpenFraction) == 132);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, RightShutter) == 136);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, LeftShutter) == 140);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, TopShutter) == 144);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, BottomShutter) == 148);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, HasShutters) == 152);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelLoad::TfPanelLoad, ShutterDirection) == 156);

} // namespace fPanelLoad
