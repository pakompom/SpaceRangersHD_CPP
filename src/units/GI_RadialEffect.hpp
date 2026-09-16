#pragma once
#include "types/EC_Struct.hpp"
#include "types/GI_RadialEffect.hpp"
#include "types/Types.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RadialEffect {
    extern pas::DynArray<pas::WideString> AuraAnimationPaths;

    extern pas::DynArray<std::int32_t> RadiationRadii;

    extern pas::DynArray<GI_RadialEffect::TRadiationPalette> RadiationPalettes;

    extern pas::Array<Types::TPoint, 0, 23> RadiationEdgeStarts;

    extern pas::Array<Types::TPoint, 0, 23> RadiationEdgeEnds;

    extern pas::Array<EC_Struct::TPointF, 0, 5> RadiationHexagon;

    void LoadRadiationPalettes();

    void TPSRadEffectGI_Create(TPSRadEffectGI* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex);

    void TPSRadEffectGI_Destroy(TPSRadEffectGI* Self);

} // namespace GI_RadialEffect
