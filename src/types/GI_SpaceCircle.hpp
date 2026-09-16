#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_SpaceCircle {
    struct TSpaceCircleSavedLineGI;

    struct TSpaceCircleSegmentGI;

    struct TSpaceCircleGI;

    using PSpaceCircleSegmentGI = TSpaceCircleSegmentGI*;

    using PSpaceCircleSavedLineGI = TSpaceCircleSavedLineGI*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSpaceCircleGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TSpaceCircleGI, GI_MessageLoop::TObjectGI, "TSpaceCircleGI", 340)
        void p_destroy() override;
        void SetRadius(std::int32_t Value);
        void SetCenter(Types::TPoint Value);
        void ClearSegments();
        void ClearPreviousLines();
        void RebuildSegments();
        void ProjectAndClipSegments();
        void RotateSegments(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        // Deactivation is deferred until CommitFrameDraw.
        void SetActive(std::uint8_t Enabled) override;
        void OnActivate() override;
        void OnDeactivate() override;
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        // Empty in native code.
        static void LoadSpaceCircleProperties(EC_BlockPar::TBlockParEC* Block);
        // Empty in native code.
        void Invalidate() override;
        void ErasePreviousFrame() override;
        void PrepareFrameDraw() override;
        void DrawUpdateRects(Types::TRect ClipRect) override;
        void Draw(Types::TRect ClipRect) override;
        void CommitFrameDraw() override;
        std::int32_t SegmentCount;
        PSpaceCircleSegmentGI Segments;
        std::int32_t PreviousLineCount;
        PSpaceCircleSavedLineGI PreviousLines;
        Types::TPoint Center;
        std::int32_t Radius;
        std::uint32_t Color;
        std::uint8_t GeometryDirty;
        std::uint8_t cpp_padding[3];
        std::int32_t DrawnSegmentCount;
        std::uint8_t DeactivateAfterFrame;
        std::uint8_t cpp_padding_2[3];
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
        void* SavedPixels;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TSpaceCircleSegmentGI {
        EC_Struct::TPointF First;
        EC_Struct::TPointF Last;
        std::int32_t ClipResult;
        Types::TPoint PixelFirst;
        Types::TPoint PixelLast;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TSpaceCircleSavedLineGI {
        Types::TPoint First;
        Types::TPoint Last;
    };
    #pragma pack(pop)

} // namespace GI_SpaceCircle
