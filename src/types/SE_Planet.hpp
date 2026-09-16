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
        // Destination must be a TPlanetSE. Copies configuration, not attached controls/timers.
        void CopyTo(SE_Space::TObjectSE* Destination) override;
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void RebuildRings();
        void RebuildSurfaceAnimation();
        // Requires an allocated image control and a mask with positive total animation weight.
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
        // Does nothing for ruins; rebuilds rings when attached to space.
        void SetRingKind(std::uint8_t Kind);
        // Does nothing for ruins; rebuilds the animation when attached to space.
        void SetSurfaceAnimationMask(std::int32_t Mask);
        void UpdateLightAngleFromStar();
        void SurfaceAnimationFinished(GI_MessageLoop::TObjectGI* Sender);
        void AdvanceRotationTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        // UserData selects cloud 1..3.
        void AdvanceCloudTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        std::uint8_t HitTestCursor() override;
        void DrawMap() override;
        // SmallPreview affects detached normal planets only. Attached planets reuse their current surface renderer; ruins use their static image.
        void RenderToBuffer(GI_MessageLoop::TMessageLoopGI* Screen, GR_GraphBuf::TGraphBufGR* Buffer, std::uint8_t SmallPreview);
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        WindowsSdk::TPoint ImageOrigin;
        std::int32_t SurfaceMapOffset;
        // A full turn has 256 steps.
        std::uint8_t LightAngle;
        std::uint8_t cpp_padding[3];
        // Milliseconds before conversion to space ticks; saved as a Word by TPlanet.
        std::uint32_t RotationTimerInterval;
        std::int32_t SurfaceMapStep;
        pas::WideString MinimapImagePath;
        WindowsSdk::TPoint MinimapImageOrigin;
        std::uint8_t cpp_padding_2[4];
        double OrbitalVelocity;
        std::int32_t Radius;
        // Native VMT cleanup lists the three image strings individually.
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
        // 0x00BBGGRR.
        std::uint32_t AtmosphereColor;
        // Water, land and hill exploration tile counts copied by planet generation.
        pas::Array<std::int32_t, 0, 2> SpaceConfigValues;
        pas::WideString BackgroundGraph;
        // Template's Quest parameter.
        std::uint8_t QuestEnabled;
        // PlanetRing resource selector; 0 disables rings. Kinds 1/4/5 also select animation families.
        std::uint8_t RingKind;
        // Film playback sets this from MinimapOwner <> 6.
        std::uint8_t Civilized;
        std::uint8_t cpp_padding_3[1];
        std::int32_t SurfaceAnimationMask;
        std::int32_t SurfaceAnimationIndex;
        GI_Planet::TPlanetGI* PlanetControl;
        GI_Image::TImageGI* RingControl1;
        GI_Image::TImageGI* RingControl2;
        GI_AlphaImage::TAlphaImageGI* MinimapControl;
        SE_Space::PSpaceTimerSE RotationTimer;
        // No creation/assignment path found in this build; only positioned, freed and cleared. Concrete descendant cannot be recovered.
        GI_MessageLoop::TObjectGI* LegacySurfaceControl;
        GI_Image::TImageGI* SurfaceImageControl;
        std::int32_t SurfaceAnimationFrame;
        WindowsSdk::TPoint SurfaceAnimationOffset;
        std::int32_t MapOrbitPointCount;
        // Owned raw allocation.
        PPlanetMapOrbitPoint MapOrbitPoints;
        PPlanetCollisionCircle CollisionCircle;
        // 0..7 use owner names; higher values select numbered icons.
        std::uint8_t MinimapOwner;
        std::uint8_t cpp_padding_4[3];
        pas::WideString RuinsAnimationPath;
        pas::WideString RuinsImagePath;
        pas::WideString RuinsMinimapPath;
        GI_GAI::TgaiGI* RuinsAnimationControl;
        GI_Image::TImageGI* RuinsImageControl;
        GI_Image::TImageGI* RuinsMinimapControl;
        std::int32_t RuinsAnimationFrame;
        // GraphKey starts with Ruins.
        std::uint8_t IsRuins;
        std::uint8_t cpp_padding_5[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPlanetMapOrbitPoint {
        WindowsSdk::TPoint Position;
        // Byte offset in the minimap's 16-bit pixel buffer.
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
