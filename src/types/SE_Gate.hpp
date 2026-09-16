#pragma once
#include "runtime_support.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_RotateImageGAI {
    struct TRotateImageGaiGI;

} // namespace GI_RotateImageGAI

namespace SE_Gate {
    struct TGateSE;

    struct TGateEffectSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGateSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TGateSE, SE_Space::TObjectSE, "TGateSE", 116)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetSize(Types::TPoint Value) override;
        std::uint8_t GetAngle() override;
        void SetAngle(std::uint8_t Value) override;
        pas::WideString GetText() override;
        void SetText(const pas::WideString& Value) override;
        // Changes idle state 0 to opening state 1.
        void Open();
        // Changes open state 2 to closing state 3.
        void Close();
        // Resets StateStep and rebuilds attached graphics.
        void SetState(std::int32_t Value);
        void RebuildStateGraphics();
        // Both native callers pass nil, 0; timer payload is unused.
        void AdvanceAnimation(void* UnusedTimer, std::int32_t UnusedData);
        void Advance() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        std::uint8_t Angle;
        std::uint8_t cpp_padding[3];
        std::int32_t State;
        std::int32_t StateStep;
        pas::WideString LabelText;
        GI_RotateImageGAI::TRotateImageGaiGI* Image;
        GI_Label::TLabelGI* TextLabel;
        float TextRed;
        float TextGreen;
        float TextBlue;
        std::int32_t TickCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGateEffectSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TGateEffectSE, SE_Space::TObjectSE, "TGateEffectSE", 92)
        void p_destroy() override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetSize(Types::TPoint Value) override;
        std::uint8_t GetAngle() override;
        void SetAngle(std::uint8_t Value) override;
        void RebuildStateGraphics();
        // Both native callers pass nil, 0; timer payload is unused.
        void AdvanceAnimation(void* UnusedTimer, std::int32_t UnusedData);
        void Advance() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        std::uint8_t Angle;
        std::uint8_t cpp_padding[3];
        std::int32_t StateStep;
        GI_RotateImageGAI::TRotateImageGaiGI* Image;
        std::int32_t TickCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Gate
