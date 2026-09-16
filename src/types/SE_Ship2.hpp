#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_AlphaImage {
    struct TAlphaImageGI;

} // namespace GI_AlphaImage

namespace GI_RotateImage5 {
    struct TRotateImage5GI;

} // namespace GI_RotateImage5

namespace GI_Tail {
    struct TTailGI;

} // namespace GI_Tail

namespace SE_Ship2 {
    struct TShip2AnimSE;

    struct TShip2SE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TShip2SE : SE_Space::TObjectSE {
        PAS_CLASS_META(TShip2SE, SE_Space::TObjectSE, "TShip2SE", 440)
        void p_destroy() override;
        void CopyTo(SE_Space::TObjectSE* Destination) override;
        void CopyDataFromMirrorImage(SE_Space::TObjectSE* Destination);
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetTailMode(std::int32_t Value);
        pas::WideString GetImagePath();
        void SetSize(Types::TPoint Value) override;
        void SetPosition(EC_Struct::TPointF APosition) override;
        void SetTailDepth(float Value);
        void SetDepth(float Value) override;
        float GetDepth() override;
        std::uint8_t GetAngle() override;
        void SetAngle(std::uint8_t Value) override;
        void OffsetTailsAlongHeading(float Distance);
        void OffsetTails(EC_Struct::TPointF Delta);
        void SetTailsEmitting(std::uint8_t Value);
        std::uint8_t GetAlpha() override;
        void SetAlpha(std::uint8_t Value) override;
        EC_Struct::TPointF GetOrbitCenter() override;
        EC_Struct::TPointF ScaleImagePoint(EC_Struct::TPointF Point);
        EC_Struct::TPointF ImagePointToWorld(EC_Struct::TPointF Point);
        EC_Struct::TPointF GetTargetPoint(std::uint8_t Heading, std::int32_t Seed);
        EC_Struct::TPointF GetWeaponPortPoint(std::uint8_t Heading, std::uint32_t Seed);
        std::uint8_t HitTestCursor() override;
        TShip2AnimSE* AddAnimation();
        void DeleteAnimation(TShip2AnimSE* Animation);
        TShip2AnimSE* AddReducedAnimation();
        void DeleteReducedAnimation(TShip2AnimSE* Animation);
        void StartAnimationTimer();
        void StopAnimationTimer();
        void StartStateTimer();
        void StopStateTimer();
        void AdvanceAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SelectNextAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void DrawMap() override;
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        Types::TPoint ImageSize;
        EC_Struct::TPointF ImageScale;
        Types::TPoint ImageOrigin;
        EC_Struct::TPointF ImageCenter;
        std::uint8_t Angle;
        std::uint8_t Alpha;
        std::uint8_t AlphaLimit;
        std::uint8_t cpp_padding[1];
        pas::WideString MinimapImagePath;
        pas::WideString AlternateImagePath;
        Types::TPoint MinimapImageOrigin;
        std::int32_t StateIntervalMs;
        pas::WideString ImagePath;
        pas::WideString ReducedImagePath;
        pas::Array<EC_Struct::TPointF, 1, 10> TailOrigins;
        std::uint32_t TailEmitIntervalMs;
        GI_RotateImage5::TRotateImage5GI* Image;
        GI_AlphaImage::TAlphaImageGI* MinimapImage;
        pas::Array<GI_Tail::TTailGI*, 1, 10> Tails;
        pas::WideString TailPrefix;
        std::uint32_t WeaponPortCount;
        pas::Array<EC_Struct::TPointF, 1, 10> WeaponPorts;
        std::uint8_t SharedAnimations;
        std::uint8_t cpp_padding_2[3];
        TShip2AnimSE* FirstAnimation;
        TShip2AnimSE* LastAnimation;
        TShip2AnimSE* FirstReducedAnimation;
        TShip2AnimSE* LastReducedAnimation;
        TShip2AnimSE* CurrentAnimation;
        TShip2AnimSE* NextAnimation;
        TShip2AnimSE* DefaultAnimation;
        TShip2AnimSE* DefaultReducedAnimation;
        std::int32_t CurrentFrameIndex;
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
        GI_MessageLoop::PCallbackTimerGI StateTimer;
        std::int32_t TotalAnimationWeight;
        std::int32_t TotalReducedAnimationWeight;
        std::int32_t TailMode;
        pas::WideString PanelPartnerImage;
        std::int32_t SmallSize;
        std::int32_t LargeSize;
        float TargetSizeScale;
        std::int32_t AngleOverride;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TShip2AnimSE : pas::Object {
        PAS_CLASS_META(TShip2AnimSE, pas::Object, "TShip2AnimSE", 28)
        void p_destroy() override;
        void Clear();
        void Load(pas::WideString Specification);
        TShip2AnimSE* Prev;
        TShip2AnimSE* Next;
        std::int32_t Weight;
        std::int32_t FrameCount;
        pas::DynArray<std::uint16_t> Frames;
        pas::DynArray<std::uint16_t> Delays;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Ship2
