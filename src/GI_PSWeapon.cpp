#include "layout/GI_PSWeapon.hpp"
#include "types/GR_GraphBuf.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GR_Main.hpp"
#include "units/System.hpp"

namespace GI_PSWeapon {
    void TPSWeaponGI_Create(TPSWeaponGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->LifetimeTicks = 65;
        Self->RemainingTicks = Self->LifetimeTicks;
    }

    std::uint8_t TPSWeaponGI::IsFinished() {
        return RemainingTicks <= 0;
    }

    // Returns LifetimeTicks minus RemainingTicks without clamping.
    std::int32_t TPSWeaponGI::GetElapsedTicks() {
        return LifetimeTicks - RemainingTicks;
    }

    // Cyclic interpolation of normalized RGB triples in the current pixel format. Requires at least one triple and nonnegative Phase; trailing incomplete triples are ignored.
    std::uint32_t TPSWeaponGI::SampleGradientColor(pas::OpenArray<float> ColorValues, float Phase) {
        std::int32_t Count = ColorValues.length() / 3;
        std::int32_t Index = System::Trunc(Phase);
        float Fraction = static_cast<long double>(Phase) - Index;
        Index = pas::imod(Index, Count);
        std::int32_t NextIndex = Index + 1;
        if (NextIndex >= Count) {
            NextIndex = 0;
        }
        return GR_Main::CurrentPixelFormat->PackNormalizedRgb((static_cast<long double>(ColorValues[3 * NextIndex]) - ColorValues[3 * Index]) * Fraction + ColorValues[3 * Index], (static_cast<long double>(ColorValues[3 * NextIndex + 1]) - ColorValues[3 * Index + 1]) * Fraction + ColorValues[3 * Index + 1], (static_cast<long double>(ColorValues[3 * NextIndex + 2]) - ColorValues[3 * Index + 2]) * Fraction + ColorValues[3 * Index + 2]);
    }

} // namespace GI_PSWeapon
