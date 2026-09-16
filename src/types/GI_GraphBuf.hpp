#pragma once
#include "runtime_support.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_GraphBuf {
    struct TGraphBufGI;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TGraphBufGI : GI_MessageLoop::TObjectGI {
        PAS_CLASS_META(TGraphBufGI, GI_MessageLoop::TObjectGI, "TGraphBufGI", 300)
        void p_destroy() override;
        void SetImageKindX(GI_Main::TImageKindXGI Value);
        void SetImageKindY(GI_Main::TImageKindYGI Value);
        // Detaches borrowed buffers without freeing them.
        void Clear() override;
        void SetHalfAlpha(std::uint8_t Value);
        // Does not modify a previously borrowed buffer; the resulting buffer is owned.
        void AllocateBuffer(std::int32_t Width, std::int32_t Height, std::uint8_t UseTexture);
        void ClearOwnedBuffer();
        // Requires equal nonempty extents, in-bounds rectangles and a buffer without per-pixel alpha.
        void CopyScreenRectToBuffer(WindowsSdk::TRect ScreenRect, WindowsSdk::TRect BufferRect);
        void LoadBitmapPathAsRgba(const pas::WideString& BitmapPath);
        void LoadBitmapPathAsRgb(const pas::WideString& BitmapPath);
        // Black pixels do not count as hits.
        std::uint8_t HitTestPixel(WindowsSdk::TPoint Point);
        // Uses nonzero pixels. CenterFill is unsupported and can leave bounds changed and temporary storage leaked.
        WindowsSdk::TPoint GetVisualCenter();
        void LoadFromConfigPath(const pas::WideString& Path) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadImageProperties(EC_BlockPar::TBlockParEC* Block);
        // Fits the image inside the control size while preserving its aspect ratio.
        void LoadScaledBitmapPathAsRgba(const pas::WideString& BitmapPath);
        void LoadScaledGiPath(const pas::WideString& GiPath);
        void Draw(WindowsSdk::TRect ClipRect) override;
        // Buffer is borrowed; alpha flags are unchanged.
        void BindExternalGraphBuf(GR_GraphBuf::TGraphBufGR* Buffer);
        GR_GraphBuf::TGraphBufGR* GraphBuf;
        GI_Main::TImageKindXGI ImageKindX;
        GI_Main::TImageKindYGI ImageKindY;
        std::uint8_t HalfAlpha;
        std::uint8_t SourceHasPerPixelAlpha;
        std::uint8_t UsesExternalGraphBuf;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GI_GraphBuf
