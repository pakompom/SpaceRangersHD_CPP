#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace SE_Ruins {
    struct TRuinsSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRuinsSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TRuinsSE, SE_Space::TObjectSE, "TRuinsSE", 216)
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetState(std::int32_t Value);
        void AnimationCycleComplete(GI_MessageLoop::TObjectGI* Sender);
        void AdvanceFade(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SetPosition(EC_Struct::TPointF APosition) override;
        void SetDepth(float Value) override;
        float GetDepth() override;
        std::uint8_t GetAlpha() override;
        void SetAlpha(std::uint8_t Value) override;
        void DrawMap() override;
        EC_Struct::TPointF GetWeaponPortPoint(std::uint32_t Seed);
        std::uint8_t HitTestCursor() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        pas::WideString StaticImagePath;
        pas::WideString MinimapImagePath;
        GI_GAI::TgaiGI* Animation;
        GI_Image::TImageGI* StaticImage;
        GI_Image::TImageGI* MinimapImage;
        std::int32_t FrameIndex;
        std::uint8_t Alpha;
        std::uint8_t AlphaLimit;
        std::uint8_t cpp_padding[2];
        std::uint32_t WeaponPortCount;
        pas::Array<EC_Struct::TPointF, 1, 10> WeaponPorts;
        std::uint8_t HasTransitionImages;
        std::uint8_t cpp_padding_2[3];
        // 2 disappears, 3 appears, 4 finishes the To animation.
        std::int32_t State;
        GI_MessageLoop::PCallbackTimerGI FadeTimer;
        std::uint8_t KeepSize;
        std::uint8_t cpp_padding_3[3];
        pas::WideString PanelPartnerImage;
        std::uint8_t HideOnStarInfo;
        std::uint8_t cpp_padding_4[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Ruins
