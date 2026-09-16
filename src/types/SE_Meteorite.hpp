#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace SE_Meteorite {
    struct TMeteoriteSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMeteoriteSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TMeteoriteSE, SE_Space::TObjectSE, "TMeteoriteSE", 100)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        static std::uint8_t IsNearView(EC_Struct::TPointF Point);
        void PlaceRandomly();
        void RestartOutsideView();
        void AdvanceMotion(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString ImagePath;
        std::int32_t TimerInterval;
        float Speed;
        // Radians, zero points upward.
        float Angle;
        GI_GAI::TgaiGI* Animation;
        SE_Space::PSpaceTimerSE MoveTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Meteorite
