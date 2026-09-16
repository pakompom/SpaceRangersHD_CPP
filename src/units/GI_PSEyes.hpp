#pragma once
#include "types/GI_PSEyes.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_PSEyes {
    extern pas::DynArray<GI_PSEyes::TEyesPalette> EyesPalettes;

    extern pas::DynArray<std::int32_t> EyesWidths;

    extern pas::DynArray<std::int32_t> EyesSegmentLengths;

    extern pas::DynArray<std::int32_t> EyesDispersions;

    extern pas::DynArray<std::int32_t> EyesStartingAlphas;

    void LoadEyesPalettes();

    void TPSEyesGI_Create(TPSEyesGI* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t PaletteIndex);

    void TPSEyesGI_Destroy(TPSEyesGI* Self);

} // namespace GI_PSEyes
