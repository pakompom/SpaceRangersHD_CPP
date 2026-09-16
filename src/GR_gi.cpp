#include "layout/GR_gi.hpp"
#include "types/EC_Buf.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "units/BitmapPorts.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_gi.hpp"
#include "units/GlobalsV.hpp"
#include "units/Windows.hpp"

namespace GR_gi {
    void SwapGiSourceRedBlue(GR_GraphBuf::TGraphBufGR*& GraphBuf);

    void SwapGiSourceRedBlue(GR_GraphBuf::TGraphBufGR*& GraphBuf) {
        void* Pixels = GraphBuf->GetPixels();
        std::int32_t Count = GraphBuf->Width * GraphBuf->Height;
        BitmapPorts::SwapRedBlue32(Pixels, Count);
    }

    void PrepareRawGiColorCache(void* Data) {
        TgiGR* Image = pas::construct_call<TgiGR>(TgiGR_Create);
        Image->LoadRawGiBytes(Data, 1);
        Image->BuildPalettedFormat4ColorCache();
        pas::free(Image);
    }

    void TgiGR_Create(TgiGR* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TgiGR_Destroy(TgiGR* Self) {
        Self->ClearData();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TgiGR::ClearData() {
        if (Data != nullptr && static_cast<std::uint8_t>(UsesExternalData ^ 1)) {
            EC_Mem::FreeEC(Data);
        }
        Data = nullptr;
        DataSize = 0;
        UsesExternalData = false;
        Header = nullptr;
    }

    std::uint8_t TgiGR::IsEmpty() {
        return Header == nullptr;
    }

    void TgiGR::LoadRawGiBytes(void* BufferPtr, std::int32_t ByteCount) {
        ClearData();
        Data = BufferPtr;
        DataSize = ByteCount;
        UsesExternalData = true;
        Header = static_cast<PgiHeaderGR>(Data);
    }

    void TgiGR::LoadRawGiFromBuffer(EC_Buf::TBufEC* SourceBuffer) {
        ClearData();
        DataSize = SourceBuffer->DataSize;
        Data = EC_Mem::AllocEC(DataSize);
        Windows::CopyMemory(Data, SourceBuffer->Data, DataSize);
        UsesExternalData = false;
        Header = static_cast<PgiHeaderGR>(Data);
    }

    void TgiGR::LoadCompressedGiBytes(void* BufferPtr, std::int32_t ByteCount) {
        ClearData();
        if (ByteCount < 8) {
            return;
        }
        DataSize = GR_Main::OKGF_ZLib_UnCompress(nullptr, 0, BufferPtr, ByteCount);
        if (DataSize == 0) {
            return;
        }
        Data = EC_Mem::AllocEC(DataSize);
        DataSize = GR_Main::OKGF_ZLib_UnCompress(Data, DataSize, BufferPtr, ByteCount);
        if (DataSize == 0) {
            EC_Mem::FreeEC(Data);
            Data = nullptr;
        } else {
            Header = static_cast<PgiHeaderGR>(Data);
            UsesExternalData = false;
        }
    }

    WindowsSdk::TRect TgiGR::GetBoundsRect() {
        WindowsSdk::TRect Result{};
        Result = Header->Bounds;
        return Result;
    }

    WindowsSdk::TPoint TgiGR::GetContentSize() {
        WindowsSdk::TPoint Result{};
        Result.X = Header->Bounds.Right - Header->Bounds.Left;
        Result.Y = Header->Bounds.Bottom - Header->Bounds.Top;
        return Result;
    }

    WindowsSdk::TPoint TgiGR::GetTopLeft() {
        WindowsSdk::TPoint Result{};
        Result.X = Header->Bounds.Left;
        Result.Y = Header->Bounds.Top;
        return Result;
    }

    std::int32_t TgiGR::GetFormat() {
        return Header->Format;
    }

    PgiPlaneGR TgiGR::GetPlane(std::int32_t PlaneIndex) {
        return static_cast<PgiPlaneGR>(static_cast<void*>(PlaneIndex * static_cast<std::int32_t>(sizeof(TgiPlaneGR)) + static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + static_cast<std::uint8_t*>(Data)));
    }

    std::int32_t TgiGR::GetClipRectCount() {
        return Header->ClipRectCount;
    }

    WindowsSdk::TRect TgiGR::GetClipRect(std::int32_t RectIndex) {
        WindowsSdk::TRect Result{};
        PgiClipRectDiskGR Rect = static_cast<PgiClipRectDiskGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + Header->ClipRectTableOffset + 1 + RectIndex * static_cast<std::int32_t>(sizeof(TgiClipRectDiskGR))));
        Result.Left = Rect->Left;
        Result.Top = Rect->Top;
        Result.Right = Rect->Right;
        Result.Bottom = Rect->Bottom;
        return Result;
    }

    void TgiGR::BuildPalettedFormat4ColorCache() {
        std::int32_t Index{};
        std::uint32_t Color{};
        if (GlobalsV::HardwareRenderingEnabled || Header == nullptr || Header->Format != 4) {
            return;
        }
        PgiPlaneGR Plane = GetPlane(1);
        void* Source = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
        void* Dest = Source;
        std::int32_t Count = pas::idiv(Plane->DataSize, static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Color = EC_Mem::ReadDWordEC(Source);
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(Color & 0x000000ff, Color >> 8 & 0x000000ff, Color >> 16 & 0x000000ff);
            EC_Mem::WriteWordEC(Dest, Color);
            Source = EC_Mem::AddPointerOffset(Source, static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
            Dest = EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(std::uint16_t)));
        }
    }

    void TgiGR::DrawToGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::int32_t X, std::int32_t Y, WindowsSdk::TRect DrawRect, std::uint8_t BlendMode, std::uint8_t Alpha) {
        PgiPlaneGR Plane{};
        PgiPlaneGR PalettePlane{};
        WindowsSdk::TRect InclusiveClip{};
        switch (Header->Format) {
            case 0: {
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                if (Plane->DataOffset != 0) {
                    if (Header->RedMask == 0x00ff0000 && Header->GreenMask == 0x0000ff00 && Header->BlueMask == 0x000000ff && Header->AlphaMask == 0xff000000u) {
                        std::int32_t cpp_arg = Y + Plane->Bounds.Top - Header->Bounds.Top;
                        void* cpp_arg_2 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                        std::int32_t cpp_arg_3 = (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(std::uint16_t));
                        std::int32_t cpp_arg_4 = Header->Bounds.Right - Header->Bounds.Left;
                        std::int32_t cpp_arg_5 = Header->Bounds.Bottom - Header->Bounds.Top;
                        void* pixels = GraphBuf->GetPixels();
                        std::int32_t cpp_arg_6 = X + Plane->Bounds.Left - Header->Bounds.Left;
                        std::int32_t pitchBytes = GraphBuf->PitchBytes;
                        GR_Main::DrawAlphaBuffer16Clipped(pixels, pitchBytes, cpp_arg_6, cpp_arg, cpp_arg_2, cpp_arg_3, cpp_arg_4, cpp_arg_5, DrawRect);
                    } else {
                        std::int32_t cpp_arg_7 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                        void* cpp_arg_8 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                        std::int32_t cpp_arg_9 = (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(std::uint16_t));
                        std::int32_t cpp_arg_10 = Header->Bounds.Right - Header->Bounds.Left;
                        std::int32_t cpp_arg_11 = Header->Bounds.Bottom - Header->Bounds.Top;
                        void* pixels_2 = GraphBuf->GetPixels();
                        std::int32_t cpp_arg_12 = X + Plane->Bounds.Left - Header->Bounds.Left;
                        std::int32_t pitchBytes_2 = GraphBuf->PitchBytes;
                        GR_Main::CopyBuffer16Clipped(pixels_2, pitchBytes_2, cpp_arg_12, cpp_arg_7, cpp_arg_8, cpp_arg_9, cpp_arg_10, cpp_arg_11, DrawRect, BlendMode);
                    }
                }
                break;
            }
            case 1: {
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                if (Plane->DataOffset != 0) {
                    std::int32_t cpp_arg_13 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                    void* cpp_arg_14 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                    void* pixels_3 = GraphBuf->GetPixels();
                    std::int32_t cpp_arg_15 = X + Plane->Bounds.Left - Header->Bounds.Left;
                    std::int32_t pitchBytes_3 = GraphBuf->PitchBytes;
                    GR_Main::DrawTransparentBuffer16(pixels_3, pitchBytes_3, cpp_arg_15, cpp_arg_13, cpp_arg_14, DrawRect, false);
                }
                break;
            }
            case 2: {
                InclusiveClip.Left = DrawRect.Left;
                InclusiveClip.Top = DrawRect.Top;
                InclusiveClip.Right = DrawRect.Right - 1;
                InclusiveClip.Bottom = DrawRect.Bottom - 1;
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + (static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + 2 * static_cast<std::int32_t>(sizeof(TgiPlaneGR)))));
                if (Plane->DataOffset != 0) {
                    std::int32_t cpp_arg_16 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                    void* cpp_arg_17 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                    const WindowsSdk::TRect& inclusiveClip = InclusiveClip;
                    void* pixels_4 = GraphBuf->GetPixels();
                    std::int32_t cpp_arg_18 = X + Plane->Bounds.Left - Header->Bounds.Left;
                    std::int32_t pitchBytes_4 = GraphBuf->PitchBytes;
                    GR_Main::Ex_OKGR_AlphaBuf_DrawClip_16(pixels_4, pitchBytes_4, cpp_arg_18, cpp_arg_16, cpp_arg_17, inclusiveClip);
                }
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + (static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + static_cast<std::int32_t>(sizeof(TgiPlaneGR)))));
                if (Plane->DataOffset != 0) {
                    std::int32_t cpp_arg_19 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                    void* cpp_arg_20 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                    const WindowsSdk::TRect& inclusiveClip_2 = InclusiveClip;
                    void* pixels_5 = GraphBuf->GetPixels();
                    std::int32_t cpp_arg_21 = X + Plane->Bounds.Left - Header->Bounds.Left;
                    std::int32_t pitchBytes_5 = GraphBuf->PitchBytes;
                    GR_Main::Ex_OKGR_TransAlphaBuf_DrawClip_WORD(pixels_5, pitchBytes_5, cpp_arg_21, cpp_arg_19, cpp_arg_20, inclusiveClip_2);
                }
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                if (Plane->DataOffset != 0) {
                    std::int32_t cpp_arg_22 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                    void* cpp_arg_23 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                    const WindowsSdk::TRect& inclusiveClip_3 = InclusiveClip;
                    void* pixels_6 = GraphBuf->GetPixels();
                    std::int32_t cpp_arg_24 = X + Plane->Bounds.Left - Header->Bounds.Left;
                    std::int32_t pitchBytes_6 = GraphBuf->PitchBytes;
                    GR_Main::Ex_OKGR_TransBuf_DrawClip_WORD(pixels_6, pitchBytes_6, cpp_arg_24, cpp_arg_22, cpp_arg_23, inclusiveClip_3);
                }
                break;
            }
            case 3: {
                InclusiveClip.Left = DrawRect.Left;
                InclusiveClip.Top = DrawRect.Top;
                InclusiveClip.Right = DrawRect.Right - 1;
                InclusiveClip.Bottom = DrawRect.Bottom - 1;
                if (Alpha == 255) {
                    Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                    if (Plane->DataOffset != 0) {
                        std::int32_t cpp_arg_25 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                        void* cpp_arg_26 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                        const WindowsSdk::TRect& inclusiveClip_4 = InclusiveClip;
                        void* pixels_7 = GraphBuf->GetPixels();
                        std::int32_t cpp_arg_27 = X + Plane->Bounds.Left - Header->Bounds.Left;
                        std::int32_t pitchBytes_7 = GraphBuf->PitchBytes;
                        GR_Main::Ex_OKGR_AlphaIndexed_CopyDrawClip_WORD(pixels_7, pitchBytes_7, cpp_arg_27, cpp_arg_25, cpp_arg_26, inclusiveClip_4);
                    }
                    Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + (static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + static_cast<std::int32_t>(sizeof(TgiPlaneGR)))));
                    if (Plane->DataOffset != 0) {
                        std::int32_t cpp_arg_28 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                        void* cpp_arg_29 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                        const WindowsSdk::TRect& inclusiveClip_5 = InclusiveClip;
                        void* pixels_8 = GraphBuf->GetPixels();
                        std::int32_t cpp_arg_30 = X + Plane->Bounds.Left - Header->Bounds.Left;
                        std::int32_t pitchBytes_8 = GraphBuf->PitchBytes;
                        GR_Main::Ex_OKGR_AlphaIndexed_AlphaDrawClip_16(pixels_8, pitchBytes_8, cpp_arg_30, cpp_arg_28, cpp_arg_29, inclusiveClip_5);
                    }
                } else if (Alpha >= 4) {
                    Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                    if (Plane->DataOffset != 0) {
                        std::int32_t cpp_arg_31 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                        void* cpp_arg_32 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                        const WindowsSdk::TRect& inclusiveClip_6 = InclusiveClip;
                        void* pixels_9 = GraphBuf->GetPixels();
                        std::int32_t cpp_arg_33 = X + Plane->Bounds.Left - Header->Bounds.Left;
                        std::int32_t pitchBytes_9 = GraphBuf->PitchBytes;
                        GR_Main::Ex_OKGR_AlphaIndexed_CopyDrawClip_Alpha_16(pixels_9, pitchBytes_9, cpp_arg_33, cpp_arg_31, cpp_arg_32, inclusiveClip_6, Alpha);
                    }
                    Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + (static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + static_cast<std::int32_t>(sizeof(TgiPlaneGR)))));
                    if (Plane->DataOffset != 0) {
                        std::int32_t cpp_arg_34 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                        void* cpp_arg_35 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                        const WindowsSdk::TRect& inclusiveClip_7 = InclusiveClip;
                        void* pixels_10 = GraphBuf->GetPixels();
                        std::int32_t cpp_arg_36 = X + Plane->Bounds.Left - Header->Bounds.Left;
                        std::int32_t pitchBytes_10 = GraphBuf->PitchBytes;
                        GR_Main::Ex_OKGR_AlphaIndexed_AlphaDrawClip_Alpha_16(pixels_10, pitchBytes_10, cpp_arg_36, cpp_arg_34, cpp_arg_35, inclusiveClip_7, Alpha);
                    }
                }
                break;
            }
            case 4: {
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                PalettePlane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + (static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + static_cast<std::int32_t>(sizeof(TgiPlaneGR)))));
                {
                    std::int32_t cpp_arg_37 = Y + Plane->Bounds.Top - Header->Bounds.Top;
                    void* cpp_arg_38 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                    void* cpp_arg_39 = PalettePlane->DataOffset + static_cast<std::uint8_t*>(Data);
                    std::int32_t cpp_arg_40 = Header->Bounds.Right - Header->Bounds.Left;
                    std::int32_t cpp_arg_41 = Header->Bounds.Right - Header->Bounds.Left;
                    std::int32_t cpp_arg_42 = Header->Bounds.Bottom - Header->Bounds.Top;
                    void* pixels_11 = GraphBuf->GetPixels();
                    std::int32_t cpp_arg_43 = X + Plane->Bounds.Left - Header->Bounds.Left;
                    std::int32_t pitchBytes_11 = GraphBuf->PitchBytes;
                    GR_Main::CopyPalettedBuffer16Clipped(pixels_11, pitchBytes_11, cpp_arg_43, cpp_arg_37, cpp_arg_38, cpp_arg_39, cpp_arg_40, cpp_arg_41, cpp_arg_42, DrawRect);
                }
                break;
            }
            case 5: {
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                {
                    std::uint8_t* cpp_left = static_cast<std::uint8_t*>(GraphBuf->GetPixels());
                    void* cpp_arg_44 = cpp_left + (X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + GraphBuf->PitchBytes * Y);
                    void* cpp_arg_45 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                    std::int32_t pitchBytes_12 = GraphBuf->PitchBytes;
                    GR_Main::Ex_OKGR_F5_DrawRGBA(cpp_arg_44, pitchBytes_12, cpp_arg_45);
                }
                break;
            }
            case 6: {
                Plane = static_cast<PgiPlaneGR>(static_cast<void*>(static_cast<std::uint8_t*>(Data) + static_cast<std::int32_t>(sizeof(TgiHeaderGR))));
                {
                    std::uint8_t* cpp_left_2 = static_cast<std::uint8_t*>(GraphBuf->GetPixels());
                    void* cpp_arg_46 = cpp_left_2 + (X * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + GraphBuf->PitchBytes * Y);
                    void* cpp_arg_47 = static_cast<std::uint8_t*>(Data) + Plane->DataOffset;
                    std::int32_t pitchBytes_13 = GraphBuf->PitchBytes;
                    GR_Main::Ex_OKGR_F6_DrawRGBA(cpp_arg_46, pitchBytes_13, cpp_arg_47);
                }
                break;
            }
        }
    }

    void TgiGR::DecodeToGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::uint8_t Keep16BitPixels) {
        PgiPlaneGR Plane{};
        PgiPlaneGR PalettePlane{};
        std::uint32_t Y{};
        if (Header->Format == 0) {
            GraphBuf->AllocateRgba(Header->Bounds.Right - Header->Bounds.Left, Header->Bounds.Bottom - Header->Bounds.Top, (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                if (Header->AlphaMask == 0) {
                    if (Keep16BitPixels) {
                        for (auto cpp_range = pas::for_to<std::uint32_t>(0u, static_cast<std::uint32_t>(GraphBuf->Height) - 1); cpp_range.next(Y); ) {
                            void* addPointerOffset = EC_Mem::AddPointerOffset(Data, Plane->DataOffset + Y * GraphBuf->Width * static_cast<std::int32_t>(sizeof(std::uint16_t)));
                            std::int32_t cpp_arg = GraphBuf->PitchBytes * Y;
                            void* pixels = GraphBuf->GetPixels();
                            void* addPointerOffset_2 = EC_Mem::AddPointerOffset(pixels, cpp_arg);
                            std::uint32_t cpp_arg_2 = GraphBuf->Width * static_cast<std::int32_t>(sizeof(std::uint16_t));
                            Windows::CopyMemory(addPointerOffset_2, addPointerOffset, cpp_arg_2);
                        }
                    } else {
                        std::int32_t pitchBytes = GraphBuf->PitchBytes;
                        std::int32_t width = GraphBuf->Width;
                        std::int32_t height = GraphBuf->Height;
                        void* pixels_2 = GraphBuf->GetPixels();
                        void* addPointerOffset_3 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                        std::int32_t cpp_arg_3 = GraphBuf->Width * static_cast<std::int32_t>(sizeof(std::uint16_t));
                        GR_Main::Ex_OKGF_Convert565toBGRA(addPointerOffset_3, cpp_arg_3, pixels_2, pitchBytes, width, height);
                    }
                } else {
                    void* addPointerOffset_4 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                    void* pixels_3 = GraphBuf->GetPixels();
                    std::uint32_t cpp_arg_4 = GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) * GraphBuf->Height;
                    Windows::CopyMemory(pixels_3, addPointerOffset_4, cpp_arg_4);
                }
            }
        } else if (Header->Format == 1) {
            GraphBuf->AllocateRgba(Header->Bounds.Right - Header->Bounds.Left, Header->Bounds.Bottom - Header->Bounds.Top, (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
            GraphBuf->ClearPixels();
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                std::int32_t cpp_arg_5 = (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                void* pixels_4 = GraphBuf->GetPixels();
                void* addPointerOffset_5 = EC_Mem::AddPointerOffset(pixels_4, cpp_arg_5);
                void* addPointerOffset_6 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                std::int32_t pitchBytes_2 = GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_TransBuf_Draw_RGBA(addPointerOffset_5, pitchBytes_2, addPointerOffset_6);
            }
        } else if (Header->Format == 2) {
            GraphBuf->AllocateRgba(Header->Bounds.Right - Header->Bounds.Left, Header->Bounds.Bottom - Header->Bounds.Top, (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
            GraphBuf->ClearPixels();
            Plane = GetPlane(2);
            if (Plane->DataOffset != 0) {
                std::int32_t cpp_arg_6 = (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                void* pixels_5 = GraphBuf->GetPixels();
                void* addPointerOffset_7 = EC_Mem::AddPointerOffset(pixels_5, cpp_arg_6);
                void* addPointerOffset_8 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                std::int32_t pitchBytes_3 = GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_AlphaBuf_Draw_RGBA(addPointerOffset_7, pitchBytes_3, addPointerOffset_8);
            }
            Plane = GetPlane(1);
            if (Plane->DataOffset != 0) {
                std::int32_t cpp_arg_7 = (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                void* pixels_6 = GraphBuf->GetPixels();
                void* addPointerOffset_9 = EC_Mem::AddPointerOffset(pixels_6, cpp_arg_7);
                void* addPointerOffset_10 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                std::int32_t pitchBytes_4 = GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_TransAlphaBuf_Draw_RGBA(addPointerOffset_9, pitchBytes_4, addPointerOffset_10);
            }
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                std::int32_t cpp_arg_8 = (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                void* pixels_7 = GraphBuf->GetPixels();
                void* addPointerOffset_11 = EC_Mem::AddPointerOffset(pixels_7, cpp_arg_8);
                void* addPointerOffset_12 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                std::int32_t pitchBytes_5 = GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_TransBuf_Draw_RGBA(addPointerOffset_11, pitchBytes_5, addPointerOffset_12);
            }
        } else if (Header->Format == 3) {
            GraphBuf->AllocateRgba(Header->Bounds.Right - Header->Bounds.Left, Header->Bounds.Bottom - Header->Bounds.Top, (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
            GraphBuf->ClearPixels();
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                std::int32_t cpp_arg_9 = (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                void* pixels_8 = GraphBuf->GetPixels();
                void* addPointerOffset_13 = EC_Mem::AddPointerOffset(pixels_8, cpp_arg_9);
                void* addPointerOffset_14 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                std::int32_t pitchBytes_6 = GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_AlphaIndexed_Draw_RGBA(addPointerOffset_13, pitchBytes_6, addPointerOffset_14);
            }
            Plane = GetPlane(1);
            if (Plane->DataOffset != 0) {
                std::int32_t cpp_arg_10 = (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * GraphBuf->Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                void* pixels_9 = GraphBuf->GetPixels();
                void* addPointerOffset_15 = EC_Mem::AddPointerOffset(pixels_9, cpp_arg_10);
                void* addPointerOffset_16 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                std::int32_t pitchBytes_7 = GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_AlphaIndexed_AlphaDraw_RGBA(addPointerOffset_15, pitchBytes_7, addPointerOffset_16);
            }
        } else if (Header->Format == 4) {
            GraphBuf->AllocateRgba(Header->Bounds.Right - Header->Bounds.Left, Header->Bounds.Bottom - Header->Bounds.Top, (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
            GraphBuf->ClearPixels();
            Plane = GetPlane(0);
            PalettePlane = GetPlane(1);
            {
                std::uint32_t height_2 = GraphBuf->Height;
                void* addPointerOffset_17 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                std::int32_t width_2 = GraphBuf->Width;
                void* addPointerOffset_18 = EC_Mem::AddPointerOffset(Data, PalettePlane->DataOffset);
                void* pixels_10 = GraphBuf->GetPixels();
                std::uint32_t width_3 = GraphBuf->Width;
                std::int32_t pitchBytes_8 = GraphBuf->PitchBytes;
                GR_Main::ExpandPaletteToBgra(pixels_10, pitchBytes_8, width_3, height_2, addPointerOffset_17, width_2, addPointerOffset_18);
            }
        } else if (Header->Format == 5) {
            if (GraphBuf != nullptr && Header->Bounds.Right - Header->Bounds.Left <= GraphBuf->Width && Header->Bounds.Bottom - Header->Bounds.Top <= GraphBuf->Height) {
                Plane = GetPlane(0);
                {
                    void* addPointerOffset_19 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                    void* pixels_11 = GraphBuf->GetPixels();
                    std::int32_t pitchBytes_9 = GraphBuf->PitchBytes;
                    GR_Main::Ex_OKGR_F5_DrawRGBA(pixels_11, pitchBytes_9, addPointerOffset_19);
                }
            }
        } else if (Header->Format == 6) {
            if (GraphBuf != nullptr && Header->Bounds.Right - Header->Bounds.Left <= GraphBuf->Width && Header->Bounds.Bottom - Header->Bounds.Top <= GraphBuf->Height) {
                Plane = GetPlane(0);
                {
                    void* addPointerOffset_20 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                    void* pixels_12 = GraphBuf->GetPixels();
                    std::int32_t pitchBytes_10 = GraphBuf->PitchBytes;
                    GR_Main::Ex_OKGR_F6_DrawRGBA(pixels_12, pitchBytes_10, addPointerOffset_20);
                }
            }
        }
    }

    void TgiGR::DecodeRawRegion(void* Destination, std::int32_t PitchBytes, std::int32_t SourceX, std::int32_t SourceY, std::int32_t Width, std::int32_t Height, std::uint8_t Keep16BitPixels) {
        PgiPlaneGR Plane{};
        std::int32_t Y{};
        std::int32_t SourcePitch{};
        if (Header->Format == 0) {
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                if (Header->AlphaMask == 0) {
                    SourcePitch = (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(std::uint16_t));
                    if (Keep16BitPixels) {
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
                            Windows::CopyMemory(EC_Mem::AddPointerOffset(Destination, PitchBytes * Y), EC_Mem::AddPointerOffset(Data, (SourceY + Y) * SourcePitch + SourceX * static_cast<std::int32_t>(sizeof(std::uint16_t)) + Plane->DataOffset), Width * static_cast<std::int32_t>(sizeof(std::uint16_t)));
                        }
                    } else {
                        GR_Main::Ex_OKGF_Convert565toBGRA(EC_Mem::AddPointerOffset(Data, SourceY * SourcePitch + SourceX * static_cast<std::int32_t>(sizeof(std::uint16_t)) + Plane->DataOffset), SourcePitch, Destination, PitchBytes, Width, Height);
                    }
                } else {
                    SourcePitch = (Header->Bounds.Right - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Height - 1); cpp_range_2.next(Y); ) {
                        Windows::CopyMemory(EC_Mem::AddPointerOffset(Destination, PitchBytes * Y), EC_Mem::AddPointerOffset(Data, (Y + SourceY) * SourcePitch + SourceX * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + Plane->DataOffset), Width * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)));
                    }
                }
            }
        } else {
            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in TgiGR.DrawRGBA()::FZag.format=", EC_Str::IntToWideString(Header->Format)})));
        }
    }

    void TgiGR::DecodeToPixels(void* Destination, std::int32_t PitchBytes, std::int32_t Width, std::int32_t Height, std::uint8_t Keep16BitPixels) {
        PgiPlaneGR Plane{};
        PgiPlaneGR PalettePlane{};
        std::int32_t Y{};
        if (Header->Format == 0) {
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                if (Header->AlphaMask == 0) {
                    if (Keep16BitPixels) {
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, Height - 1); cpp_range.next(Y); ) {
                            Windows::CopyMemory(EC_Mem::AddPointerOffset(Destination, PitchBytes * Y), EC_Mem::AddPointerOffset(Data, Y * Width * static_cast<std::int32_t>(sizeof(std::uint16_t)) + Plane->DataOffset), Width * static_cast<std::int32_t>(sizeof(std::uint16_t)));
                        }
                    } else {
                        GR_Main::Ex_OKGF_Convert565toBGRA(EC_Mem::AddPointerOffset(Data, Plane->DataOffset), Width * static_cast<std::int32_t>(sizeof(std::uint16_t)), Destination, PitchBytes, Width, Height);
                    }
                } else {
                    Windows::CopyMemory(Destination, EC_Mem::AddPointerOffset(Data, Plane->DataOffset), PitchBytes * Height);
                }
            }
        } else if (Header->Format == 1) {
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                GR_Main::Ex_OKGR_TransBuf_Draw_RGBA(EC_Mem::AddPointerOffset(Destination, (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * PitchBytes), PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
            }
        } else if (Header->Format == 2) {
            Plane = GetPlane(2);
            if (Plane->DataOffset != 0) {
                GR_Main::Ex_OKGR_AlphaBuf_Draw_RGBA(EC_Mem::AddPointerOffset(Destination, (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * PitchBytes), PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
            }
            Plane = GetPlane(1);
            if (Plane->DataOffset != 0) {
                GR_Main::Ex_OKGR_TransAlphaBuf_Draw_RGBA(EC_Mem::AddPointerOffset(Destination, (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * PitchBytes), PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
            }
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                GR_Main::Ex_OKGR_TransBuf_Draw_RGBA(EC_Mem::AddPointerOffset(Destination, (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * PitchBytes), PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
            }
        } else if (Header->Format == 3) {
            Plane = GetPlane(0);
            if (Plane->DataOffset != 0) {
                GR_Main::Ex_OKGR_AlphaIndexed_Draw_RGBA(EC_Mem::AddPointerOffset(Destination, (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * PitchBytes), PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
            }
            Plane = GetPlane(1);
            if (Plane->DataOffset != 0) {
                GR_Main::Ex_OKGR_AlphaIndexed_AlphaDraw_RGBA(EC_Mem::AddPointerOffset(Destination, (Plane->Bounds.Left - Header->Bounds.Left) * static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA)) + (Plane->Bounds.Top - Header->Bounds.Top) * PitchBytes), PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
            }
        } else if (Header->Format == 4) {
            Plane = GetPlane(0);
            PalettePlane = GetPlane(1);
            GR_Main::ExpandPaletteToBgra(Destination, PitchBytes, Width, Height, EC_Mem::AddPointerOffset(Data, Plane->DataOffset), Width, EC_Mem::AddPointerOffset(Data, PalettePlane->DataOffset));
        } else if (Header->Format == 5) {
            Plane = GetPlane(0);
            GR_Main::Ex_OKGR_F5_DrawRGBA(Destination, PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
        } else if (Header->Format == 6) {
            Plane = GetPlane(0);
            GR_Main::Ex_OKGR_F6_DrawRGBA(Destination, PitchBytes, EC_Mem::AddPointerOffset(Data, Plane->DataOffset));
        }
    }

    void TgiGR::CreateFromGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, std::int32_t StorageMode) {
        ClearData();
        std::uint32_t ByteCount = GraphBuf->Width * GraphBuf->Height;
        if (StorageMode == 2) {
            ByteCount *= static_cast<std::int32_t>(sizeof(GR_GraphBuf::TColorRGBA));
        } else {
            ByteCount *= static_cast<std::int32_t>(sizeof(std::uint16_t));
        }
        DataSize = ByteCount + (static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + static_cast<std::int32_t>(sizeof(TgiPlaneGR)));
        Data = EC_Mem::AllocEC(DataSize);
        Header = static_cast<PgiHeaderGR>(Data);
        pas::fill_memory(Header, static_cast<std::int32_t>(sizeof(TgiHeaderGR)), static_cast<std::uint8_t>(0));
        pas::store_unaligned<std::uint8_t>(pas::byte_offset(&Header->Magic, 0 * sizeof(std::uint8_t)), 'g');
        pas::store_unaligned<std::uint8_t>(pas::byte_offset(&Header->Magic, 1 * sizeof(std::uint8_t)), 'i');
        Header->Version = 1;
        pas::store_unaligned<WindowsSdk::TRect>(&Header->Bounds, ClassesImports::Rect(0, 0, GraphBuf->Width, GraphBuf->Height));
        Header->PlaneCount = 1;
        PgiPlaneGR Plane = GetPlane(0);
        Plane->DataOffset = static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + static_cast<std::int32_t>(sizeof(TgiPlaneGR));
        Plane->DataSize = ByteCount;
        pas::store_unaligned<WindowsSdk::TRect>(&Plane->Bounds, Header->Bounds);
        switch (StorageMode) {
            case 0: {
                Header->RedMask = 0x00ff0000u;
                Header->GreenMask = 0x0000ff00u;
                Header->BlueMask = 0x000000ffu;
                Header->AlphaMask = 0xff000000u;
                {
                    void* pixels = GraphBuf->GetPixels();
                    void* addPointerOffset = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                    Windows::CopyMemory(addPointerOffset, pixels, ByteCount);
                }
                break;
            }
            case 1: {
                GR_gi::SwapGiSourceRedBlue(GraphBuf);
                Header->RedMask = 0x0000f800u;
                Header->GreenMask = 0x000007e0u;
                Header->BlueMask = 0x0000001fu;
                {
                    void* pixels_2 = GraphBuf->GetPixels();
                    std::int32_t pitchBytes = GraphBuf->PitchBytes;
                    std::int32_t width = GraphBuf->Width;
                    std::int32_t height = GraphBuf->Height;
                    void* addPointerOffset_2 = EC_Mem::AddPointerOffset(Data, Plane->DataOffset);
                    std::int32_t cpp_arg = GraphBuf->Width * static_cast<std::int32_t>(sizeof(std::uint16_t));
                    GR_Main::Ex_OKGF_Convert_8888to565(addPointerOffset_2, cpp_arg, 0, 0, pixels_2, pitchBytes, 0, 0, width, height);
                }
                GR_gi::SwapGiSourceRedBlue(GraphBuf);
                break;
            }
        }
    }

    void TgiGR::CreateFormat2FromGraphBuf(GR_GraphBuf::TGraphBufGR* GraphBuf, WindowsSdk::TPoint TopLeft) {
        ClearData();
        std::int32_t height = GraphBuf->Height;
        void* pixels = GraphBuf->GetPixels();
        std::int32_t width = GraphBuf->Width;
        std::int32_t pitchBytes = GraphBuf->PitchBytes;
        std::int32_t ByteCount = GR_Main::Ex_OKGR_TransBuf_BuildFromRGBA_16(pixels, pitchBytes, width, height, nullptr);
        ByteCount += ([&] {
            std::int32_t height_2 = GraphBuf->Height;
            void* pixels_2 = GraphBuf->GetPixels();
            std::int32_t width_2 = GraphBuf->Width;
            std::int32_t pitchBytes_2 = GraphBuf->PitchBytes;
            return GR_Main::Ex_OKGR_TransAlphaBuf_BuildFromRGBA_16(pixels_2, pitchBytes_2, width_2, height_2, nullptr);
        }());
        ByteCount += ([&] {
            std::int32_t height_3 = GraphBuf->Height;
            void* pixels_3 = GraphBuf->GetPixels();
            std::int32_t width_3 = GraphBuf->Width;
            std::int32_t pitchBytes_3 = GraphBuf->PitchBytes;
            return GR_Main::Ex_OKGR_AlphaBuf_BuildFromRGBA(pixels_3, pitchBytes_3, width_3, height_3, nullptr);
        }());
        std::int32_t Offset = static_cast<std::int32_t>(sizeof(TgiHeaderGR)) + 3 * static_cast<std::int32_t>(sizeof(TgiPlaneGR));
        DataSize = Offset + ByteCount;
        Data = EC_Mem::AllocEC(DataSize);
        Header = static_cast<PgiHeaderGR>(Data);
        pas::fill_memory(Header, static_cast<std::int32_t>(sizeof(TgiHeaderGR)), static_cast<std::uint8_t>(0));
        pas::store_unaligned<std::uint8_t>(pas::byte_offset(&Header->Magic, 0 * sizeof(std::uint8_t)), 'g');
        pas::store_unaligned<std::uint8_t>(pas::byte_offset(&Header->Magic, 1 * sizeof(std::uint8_t)), 'i');
        Header->Version = 1;
        pas::store_unaligned<WindowsSdk::TRect>(&Header->Bounds, ClassesImports::Rect(TopLeft.X, TopLeft.Y, TopLeft.X + GraphBuf->Width, TopLeft.Y + GraphBuf->Height));
        Header->Format = 2;
        Header->PlaneCount = 3;
        Header->RedMask = 0x0000f800u;
        Header->GreenMask = 0x000007e0u;
        Header->BlueMask = 0x0000001fu;
        PgiPlaneGR Plane = GetPlane(0);
        Plane->DataOffset = Offset;
        {
            std::int32_t height_4 = GraphBuf->Height;
            void* addPointerOffset = EC_Mem::AddPointerOffset(Data, Offset);
            void* pixels_4 = GraphBuf->GetPixels();
            std::int32_t width_4 = GraphBuf->Width;
            std::int32_t pitchBytes_4 = GraphBuf->PitchBytes;
            Plane->DataSize = GR_Main::Ex_OKGR_TransBuf_BuildFromRGBA_16(pixels_4, pitchBytes_4, width_4, height_4, addPointerOffset);
        }
        pas::store_unaligned<WindowsSdk::TRect>(&Plane->Bounds, Header->Bounds);
        Offset += Plane->DataSize;
        Plane = GetPlane(1);
        Plane->DataOffset = Offset;
        {
            std::int32_t height_5 = GraphBuf->Height;
            void* addPointerOffset_2 = EC_Mem::AddPointerOffset(Data, Offset);
            void* pixels_5 = GraphBuf->GetPixels();
            std::int32_t width_5 = GraphBuf->Width;
            std::int32_t pitchBytes_5 = GraphBuf->PitchBytes;
            Plane->DataSize = GR_Main::Ex_OKGR_TransAlphaBuf_BuildFromRGBA_16(pixels_5, pitchBytes_5, width_5, height_5, addPointerOffset_2);
        }
        pas::store_unaligned<WindowsSdk::TRect>(&Plane->Bounds, Header->Bounds);
        Offset += Plane->DataSize;
        Plane = GetPlane(2);
        Plane->DataOffset = Offset;
        {
            std::int32_t height_6 = GraphBuf->Height;
            void* addPointerOffset_3 = EC_Mem::AddPointerOffset(Data, Offset);
            void* pixels_6 = GraphBuf->GetPixels();
            std::int32_t width_6 = GraphBuf->Width;
            std::int32_t pitchBytes_6 = GraphBuf->PitchBytes;
            Plane->DataSize = GR_Main::Ex_OKGR_AlphaBuf_BuildFromRGBA(pixels_6, pitchBytes_6, width_6, height_6, addPointerOffset_3);
        }
        pas::store_unaligned<WindowsSdk::TRect>(&Plane->Bounds, Header->Bounds);
    }

    void TgiGR::p_destroy() {
        GR_gi::TgiGR_Destroy(this);
    }

} // namespace GR_gi
