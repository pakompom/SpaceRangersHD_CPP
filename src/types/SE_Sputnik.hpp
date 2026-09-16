#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/SE_Space.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GI_Planet {
    struct TPlanetGI;

} // namespace GI_Planet

namespace SE_Sputnik {
    struct TSputnikSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSputnikSE : SE_Space::TObjectSE {
        PAS_CLASS_META(TSputnikSE, SE_Space::TObjectSE, "TSputnikSE", 180)
        // Does nothing when satellite graphics are disabled.
        void AttachToSpace(SE_Space::TSpaceSE* ASpace) override;
        void DetachFromSpace() override;
        void SetOrbitCenter(EC_Struct::TPointF Center) override;
        EC_Struct::TPointF GetOrbitCenter() override;
        // Returns a new buffer owned by the caller; excludes OrbitAngle.
        EC_Buf::TBufEC* BuildStateBuffer() override;
        // Rewinds Buffer to zero and rebuilds the orbit transform and display position.
        void LoadStateBuffer(EC_Buf::TBufEC* Buffer) override;
        void RebuildOrbitTransform();
        // Requires a nonzero depth range when attached; updates position, apparent radius and drawing depth.
        void UpdateOrbitDisplay();
        void AdvanceOrbitTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void AdvanceRotationTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void LoadTemplate(EC_BlockPar::TBlockParEC* Block) override;
        void ApplyConfig(EC_BlockPar::TBlockParEC* Block) override;
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        pas::WideString ImagePath;
        // Serialized as a byte; separates overlapping satellites in front of/behind the planet.
        std::int32_t DepthOrder;
        EC_Struct::TPointF OrbitCenter;
        // Degrees.
        float OrbitInclination;
        // Degrees in the display plane.
        float OrbitRotation;
        float OrbitAngleStep;
        std::uint32_t OrbitTimerInterval;
        float OrbitRadius;
        std::int32_t MinDisplayRadius;
        std::int32_t MaxDisplayRadius;
        std::uint32_t RotationTimerInterval;
        std::int32_t SurfaceMapStep;
        // Saved separately by TSputnik.SaveToBuffer.
        float OrbitAngle;
        std::int32_t SurfaceMapOffset;
        std::int32_t DisplayRadius;
        // A full turn has 256 steps.
        std::uint8_t LightAngle;
        std::uint8_t cpp_padding[3];
        float InclinationCos;
        float InclinationSin;
        float RotationCos;
        float RotationSin;
        float MinOrbitDepth;
        float MaxOrbitDepth;
        // Owned while attached.
        GI_Planet::TPlanetGI* PlanetControl;
        GI_MessageLoop::PCallbackTimerGI OrbitTimer;
        GI_MessageLoop::PCallbackTimerGI RotationTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Sputnik
