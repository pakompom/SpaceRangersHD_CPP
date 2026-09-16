#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace GI_PSWeapon {
    struct TPSWeaponGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPSWeaponGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TPSWeaponGI, GI_MessageLoop::TObjectGI, "TPSWeaponGI", 304)
        std::uint8_t IsFinished();
        virtual std::int32_t GetElapsedTicks();
        static std::uint32_t SampleGradientColor(pas::OpenArray<float> ColorValues, float Phase);
        virtual void SetTargetPoint(Types::TPoint Point) = 0;
        virtual void Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) = 0;
        Types::TPoint TargetPoint;
        std::int32_t RemainingTicks;
        std::int32_t LifetimeTicks;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_PSWeapon
