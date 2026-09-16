#pragma once
#include "runtime_support.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace SE_GAIEffect {
    struct TGAIEffectSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGAIEffectSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TGAIEffectSE, SE_Space::TObjectSE, "TGAIEffectSE", 108)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetImagePosition(Types::TPoint Value);
        void SetDurationScale(float Value);
        void Advance() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        pas::WideString SoundPath;
        Types::TPoint ImagePosition;
        float DurationScale;
        pas::WideString ImagePath;
        GI_GAI::TgaiGI* Animation;
        std::int32_t StepsPerFrame;
        std::int32_t StepIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_GAIEffect
