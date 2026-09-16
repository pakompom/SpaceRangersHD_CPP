#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_Cursor {
    struct TCursorGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCursorGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TCursorGI, GI_MessageLoop::TObjectGI, "TCursorGI", 316)
        void p_destroy() override;
        // Clears cursor resources and the image child, retaining the child object.
        void Clear() override;
        void SetImagePath(const pas::WideString& Path);
        void SetActive(std::uint8_t Enabled) override;
        void SetOrigin(WindowsSdk::TPoint Origin) override;
        void Draw(WindowsSdk::TRect ClipRect) override;
        // Builds Windows cursor handles from GI/GAI resources and schedules animation when active.
        void RebuildSystemCursor();
        // Copies a 32-bit image to a top-down Windows DIB; caller owns the bitmap.
        static std::uint32_t CreateCursorBitmap(GR_GraphBuf::TGraphBufGR* Buffer);
        // Timer and UserData are unused; replaces AnimationTimer after advancing the sequence.
        void AdvanceAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        GI_Image::TImageGI* ImageControl;
        pas::WideString ImagePath;
        pas::DynArray<std::uint32_t> CursorHandles;
        pas::DynArray<std::int32_t> FrameIndices;
        pas::DynArray<std::int32_t> FrameDelays;
        std::int32_t FrameIndex;
        GI_MessageLoop::PCallbackTimerGI AnimationTimer;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_Cursor
