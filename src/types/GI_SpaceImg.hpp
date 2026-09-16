#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_SpaceImg {
    struct TSpaceImageGI;

    struct TSpaceImgGI;

    using PSpaceImageGI = TSpaceImageGI*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSpaceImgGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TSpaceImgGI, GI_MessageLoop::TObjectGI, "TSpaceImgGI", 312)
        void p_destroy() override;
        void ClearImages();
        PSpaceImageGI AllocateImage(float Depth);
        PSpaceImageGI AddImage(std::int32_t TemplateIndex, float X, float Y, float Depth);
        float NearestImageDistance(float X, float Y);
        static void UpdateImageOrbitAndFrame(PSpaceImageGI Image);
        void ProjectImages();
        PSpaceImageGI GetImage(std::int32_t Index);
        void AnimateImages(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SetViewPosition(EC_Struct::TPointF Position);
        void Invalidate() override;
        void OnActivate() override;
        void OnDeactivate() override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        static void LoadSpaceImageProperties(EC_BlockPar::TBlockParEC* Block);
        void UpdateAutoGeometry() override;
        void Draw(WindowsSdk::TRect ClipRect) override;
        std::int32_t ImageCount;
        PSpaceImageGI Images;
        std::uint8_t ViewDirty;
        std::uint8_t cpp_padding[3];
        EC_Struct::TPointF ViewPosition;
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TSpaceImageGI {
        std::int32_t TemplateIndex;
        std::int32_t FrameIndex;
        std::int32_t FrameTicks;
        float X;
        float Y;
        float Depth;
        float InverseDepth;
        std::uint8_t cpp_padding[4];
        EC_Struct::TVector3D OrbitCenter;
        EC_Struct::TVector3D Unknown38;
        WindowsSdk::TPoint ImageSize;
        WindowsSdk::TPoint ImageOffset;
        WindowsSdk::TPoint PixelPosition;
        double OrbitStepDegrees;
        std::int32_t Unknown70;
        std::uint8_t cpp_padding_2[4];
        double OrbitAngleRadians;
        double OrbitRadius;
    };
    #pragma pack(pop)

} // namespace GI_SpaceImg
