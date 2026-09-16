#pragma once
#include "types/fHangar.hpp"

namespace fHangar {
    extern const pas::Array<pas::Array<float, 0, 7>, 0, 2> HangarDominatorPortraitScales;

    void TfHangar_Create(TfHangar* Self);

    void TfHangar_Destroy(TfHangar* Self);

} // namespace fHangar
