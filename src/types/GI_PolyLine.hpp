#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_Circle {
    struct TCircleGI;

} // namespace GI_Circle

namespace GI_PolyLine {
    struct TPolyLineSegmentGI;

    struct TPolyLineGI;

    using PPolyLineSegmentGI = TPolyLineSegmentGI*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPolyLineGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TPolyLineGI, GI_MessageLoop::TObjectGI, "TPolyLineGI", 320)
        void p_destroy() override;
        void Clear() override;
        PPolyLineSegmentGI AllocateSegment();
        void ClearSegments();
        void RemoveSegment(PPolyLineSegmentGI Segment);
        void AllocatePixelBuffers(PPolyLineSegmentGI Segment);
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        // Empty in native code.
        static void LoadPolyLineProperties(EC_BlockPar::TBlockParEC* Block);
        void RebuildBounds();
        PPolyLineSegmentGI AddParentLine(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t UserData);
        PPolyLineSegmentGI AddLine(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color);
        PPolyLineSegmentGI AddLocalLine(WindowsSdk::TPoint First, WindowsSdk::TPoint Last, std::uint32_t Color, std::int32_t UserData);
        void UpdateSegmentLength(PPolyLineSegmentGI Segment);
        void RetireSegment(PPolyLineSegmentGI Segment);
        void StartAnimation();
        void StopAnimation();
        void AdvanceAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void Invalidate() override;
        void ErasePreviousFrame() override;
        void PrepareFrameDraw() override;
        void DrawUpdateRects(WindowsSdk::TRect ClipRect) override;
        void Draw(WindowsSdk::TRect ClipRect) override;
        virtual void DrawSegment(PPolyLineSegmentGI Segment, WindowsSdk::TRect ClipRect);
        virtual void DrawFrameSegment(PPolyLineSegmentGI Segment, WindowsSdk::TRect ClipRect);
        // Empty in native code.
        void CommitFrameDraw() override;
        PPolyLineSegmentGI FirstSegment;
        PPolyLineSegmentGI LastSegment;
        std::uint32_t AnimationPhase;
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
        std::uint8_t FrameDrawing;
        std::uint8_t cpp_padding[3];
        // Borrowed light-mask control.
        GI_Circle::TCircleGI* ShadowCircle;
        std::uint8_t AutoRebuildBounds;
        std::uint8_t NormalizeBounds;
        std::uint8_t cpp_padding_2[2];
        std::uint32_t SegmentHeap;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPolyLineSegmentGI {
        PPolyLineSegmentGI Next;
        PPolyLineSegmentGI Prev;
        WindowsSdk::TPoint First;
        WindowsSdk::TPoint Last;
        std::int32_t UserData;
        std::uint8_t Animated;
        std::uint8_t cpp_padding[3];
        std::int32_t PixelCount;
        std::int32_t PixelCapacity;
        WindowsSdk::TPoint PixelFirst;
        WindowsSdk::TPoint PixelLast;
        void* SavedPixels;
        std::uint8_t Visible;
        WindowsSdk::TPoint PreviousFirst;
        WindowsSdk::TPoint PreviousLast;
        std::uint8_t cpp_padding_2[3];
        void* PreviousPixels;
        std::uint8_t PreviouslyVisible;
        std::uint8_t cpp_padding_3[3];
        std::uint32_t ClippedColor;
        std::uint32_t ClippedEndColor;
        std::uint32_t Color;
        std::uint32_t EndColor;
        // 0 animated RGB565, 1 alpha, 2 gradient.
        std::int32_t Kind;
    };
    #pragma pack(pop)

} // namespace GI_PolyLine
