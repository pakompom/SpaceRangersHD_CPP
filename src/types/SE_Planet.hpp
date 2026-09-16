#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_AlphaImage {
    struct TAlphaImageGI;

} // namespace GI_AlphaImage

namespace GI_GAI {
    struct TgaiGI;

} // namespace GI_GAI

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_MessageLoop {
    struct TMessageLoopGI;

    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Planet {
    struct TPlanetGI;

} // namespace GI_Planet

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace SE_Planet {
    struct TPlanetCollisionCircle;

    struct TPlanetMapOrbitPoint;

    struct TPlanetSE;

    using PPlanetMapOrbitPoint = TPlanetMapOrbitPoint*;

    using PPlanetCollisionCircle = TPlanetCollisionCircle*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlanetSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TPlanetSE, SE_Space::TObjectSE, "TPlanetSE", 312)
        void CopyTo(SE_Space::TObjectSE* Destination) override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void RebuildRings();
        void RebuildSurfaceAnimation();
        void StartRandomSurfaceAnimation();
        void SetMinimapOwner(std::uint8_t Owner);
        void SetPosition(EC_Struct::TPointF APosition) override;
        void SetSurfaceMapOffset(std::int32_t Value);
        void SetCloud1MapOffset(std::int32_t Value);
        void SetCloud2MapOffset(std::int32_t Value);
        void SetCloud3MapOffset(std::int32_t Value);
        void SetLightAngle(std::uint8_t Value);
        void SetRotationTimerInterval(std::uint32_t Value);
        void SetSurfaceMapStep(std::int32_t Value);
        void SetRingKind(std::uint8_t Kind);
        void SetSurfaceAnimationMask(std::int32_t Mask);
        void UpdateLightAngleFromStar();
        void SurfaceAnimationFinished(GI_MessageLoop::TObjectGI* Sender);
        void AdvanceRotationTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        void AdvanceCloudTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        std::uint8_t HitTestCursor() override;
        void DrawMap() override;
        void RenderToBuffer(GI_MessageLoop::TMessageLoopGI* Screen, GR_GraphBuf::TGraphBufGR* Buffer, std::uint8_t SmallPreview);
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        WindowsSdk::TPoint ImageOrigin;
        std::int32_t SurfaceMapOffset;
        std::uint8_t LightAngle;
        std::uint8_t cpp_padding[3];
        std::uint32_t RotationTimerInterval;
        std::int32_t SurfaceMapStep;
        pas::WideString MinimapImagePath;
        WindowsSdk::TPoint MinimapImageOrigin;
        std::uint8_t cpp_padding_2[4];
        double OrbitalVelocity;
        std::int32_t Radius;
        pas::WideString Cloud1ImagePath;
        float Cloud1RelativeRotationSpeed;
        std::int32_t Cloud1MapStep;
        SE_Space::PSpaceTimerSE Cloud1Timer;
        std::int32_t Cloud1MapOffset;
        pas::WideString Cloud2ImagePath;
        float Cloud2RelativeRotationSpeed;
        std::int32_t Cloud2MapStep;
        SE_Space::PSpaceTimerSE Cloud2Timer;
        std::int32_t Cloud2MapOffset;
        pas::WideString Cloud3ImagePath;
        float Cloud3RelativeRotationSpeed;
        std::int32_t Cloud3MapStep;
        SE_Space::PSpaceTimerSE Cloud3Timer;
        std::int32_t Cloud3MapOffset;
        std::uint32_t AtmosphereColor;
        pas::Array<std::int32_t, 0, 2> SpaceConfigValues;
        pas::WideString BackgroundGraph;
        std::uint8_t QuestEnabled;
        std::uint8_t RingKind;
        std::uint8_t Civilized;
        std::uint8_t cpp_padding_3[1];
        std::int32_t SurfaceAnimationMask;
        std::int32_t SurfaceAnimationIndex;
        GI_Planet::TPlanetGI* PlanetControl;
        GI_Image::TImageGI* RingControl1;
        GI_Image::TImageGI* RingControl2;
        GI_AlphaImage::TAlphaImageGI* MinimapControl;
        SE_Space::PSpaceTimerSE RotationTimer;
        GI_MessageLoop::TObjectGI* LegacySurfaceControl;
        GI_Image::TImageGI* SurfaceImageControl;
        std::int32_t SurfaceAnimationFrame;
        WindowsSdk::TPoint SurfaceAnimationOffset;
        std::int32_t MapOrbitPointCount;
        PPlanetMapOrbitPoint MapOrbitPoints;
        PPlanetCollisionCircle CollisionCircle;
        std::uint8_t MinimapOwner;
        std::uint8_t cpp_padding_4[3];
        pas::WideString RuinsAnimationPath;
        pas::WideString RuinsImagePath;
        pas::WideString RuinsMinimapPath;
        GI_GAI::TgaiGI* RuinsAnimationControl;
        GI_Image::TImageGI* RuinsImageControl;
        GI_Image::TImageGI* RuinsMinimapControl;
        std::int32_t RuinsAnimationFrame;
        std::uint8_t IsRuins;
        std::uint8_t cpp_padding_5[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPlanetMapOrbitPoint {
        WindowsSdk::TPoint Position;
        std::int32_t PixelOffset;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TPlanetCollisionCircle {
        PPlanetCollisionCircle Next;
        PPlanetCollisionCircle Prev;
        EC_Struct::TPointF Position;
        float Radius;
        float RadiusSquared;
    };
    #pragma pack(pop)

} // namespace SE_Planet
