#include "layout/GI_GraphBuf.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_CacheGI.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"

namespace GI_GraphBuf {
    void TGraphBufGI_Create(TGraphBufGI* Self, GI_MessageLoop::TObjectGI* Owner, std::uint8_t UseTexture) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->GraphBuf = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, UseTexture);
        Self->ImageKindX = GI_Main::ikxCenter;
        Self->ImageKindY = GI_Main::ikyCenter;
        Self->HalfAlpha = false;
        Self->UsesExternalGraphBuf = false;
    }

    // Frees GraphBuf only when it is owned.
    void TGraphBufGI_Destroy(TGraphBufGI* Self) {
        if (!Self->UsesExternalGraphBuf) {
            pas::free(Self->GraphBuf);
        }
        Self->GraphBuf = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TGraphBufGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            Invalidate();
        }
    }

    void TGraphBufGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            Invalidate();
        }
    }

    // Detaches borrowed buffers without freeing them.
    void TGraphBufGI::Clear() {
        ImageKindX = GI_Main::ikxCenter;
        ImageKindY = GI_Main::ikyCenter;
        HalfAlpha = false;
        if (UsesExternalGraphBuf) {
            GraphBuf = nullptr;
            UsesExternalGraphBuf = false;
        }
        if (GraphBuf != nullptr) {
            GraphBuf->Clear();
        }
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TGraphBufGI::SetHalfAlpha(std::uint8_t Value) {
        if (HalfAlpha != Value) {
            HalfAlpha = Value;
            Invalidate();
        }
    }

    // Does not modify a previously borrowed buffer; the resulting buffer is owned.
    void TGraphBufGI::AllocateBuffer(std::int32_t Width, std::int32_t Height, std::uint8_t UseTexture) {
        if (UsesExternalGraphBuf) {
            GraphBuf = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, UseTexture);
        }
        GraphBuf->AllocateNative(Width, Height);
        SourceHasPerPixelAlpha = false;
        UsesExternalGraphBuf = false;
    }

    void TGraphBufGI::ClearOwnedBuffer() {
        if (!UsesExternalGraphBuf) {
            GraphBuf->Clear();
        }
    }

    // Requires equal nonempty extents, in-bounds rectangles and a buffer without per-pixel alpha.
    void TGraphBufGI::CopyScreenRectToBuffer(WindowsSdk::TRect ScreenRect, WindowsSdk::TRect BufferRect) {
        if (SourceHasPerPixelAlpha) {
            return;
        }
        if (GraphBuf->GetPixels() == nullptr) {
            return;
        }
        if (ScreenRect.Right - ScreenRect.Left != BufferRect.Right - BufferRect.Left) {
            return;
        }
        if (ScreenRect.Bottom - ScreenRect.Top != BufferRect.Bottom - BufferRect.Top) {
            return;
        }
        if (ScreenRect.Right == ScreenRect.Left) {
            return;
        }
        if (ScreenRect.Bottom == ScreenRect.Top) {
            return;
        }
        if (ScreenRect.Left < 0) {
            return;
        }
        if (ScreenRect.Top < 0) {
            return;
        }
        if (GR_Main::GameScreenWidth < ScreenRect.Right) {
            return;
        }
        if (GR_Main::GameScreenHeight < ScreenRect.Bottom) {
            return;
        }
        if (BufferRect.Left < 0) {
            return;
        }
        if (BufferRect.Top < 0) {
            return;
        }
        if (GraphBuf->Width < BufferRect.Right) {
            return;
        }
        if (GraphBuf->Height < BufferRect.Bottom) {
            return;
        }
        if (GlobalsV::HardwareRenderingEnabled) {
            GraphBuf->LoadFromScreen(0);
        } else {
            void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
            std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
            void* pixels_2 = GraphBuf->GetPixels();
            std::int32_t pitchBytes_2 = GraphBuf->PitchBytes;
            GR_Main::Ex_OKGR_Copy_XY_XY_WORD(pixels_2, pitchBytes_2, BufferRect.Left, BufferRect.Top, pixels, pitchBytes, ScreenRect.Left, ScreenRect.Top, BufferRect.Right - BufferRect.Left, BufferRect.Bottom - BufferRect.Top);
        }
    }

    void TGraphBufGI::LoadBitmapPathAsRgba(const pas::WideString& BitmapPath) {
        EC_Cache::TCacheControlEC* Control = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Control);
        Control->SetCacheKey(BitmapPath);
        EC_CacheBitmap::TCBitmapEC* Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(Control);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Control->Release();
            };
            GraphBuf->AllocateRgba(Bitmap->Bitmap->Width, Bitmap->Bitmap->Height, Bitmap->Bitmap->PitchBytes);
            {
                void* pixels = Bitmap->Bitmap->GetPixels();
                void* pixels_2 = GraphBuf->GetPixels();
                std::uint32_t cpp_arg = GraphBuf->PitchBytes * GraphBuf->Height;
                Windows::CopyMemory(pixels_2, pixels, cpp_arg);
            }
        }
        pas::free(Control);
        SourceHasPerPixelAlpha = true;
    }

    void TGraphBufGI::LoadBitmapPathAsRgb(const pas::WideString& BitmapPath) {
        EC_Cache::TCacheControlEC* Control = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Control);
        Control->SetCacheKey(BitmapPath);
        EC_CacheBitmap::TCBitmapEC* Bitmap = EC_CacheBitmap::AcquireOrCreateBitmap(Control);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Control->Release();
            };
            GraphBuf->AllocateRgb(Bitmap->Bitmap->Width, Bitmap->Bitmap->Height, Bitmap->Bitmap->PitchBytes);
            {
                void* pixels = Bitmap->Bitmap->GetPixels();
                void* pixels_2 = GraphBuf->GetPixels();
                std::uint32_t cpp_arg = GraphBuf->PitchBytes * GraphBuf->Height;
                Windows::CopyMemory(pixels_2, pixels, cpp_arg);
            }
        }
        pas::free(Control);
        SourceHasPerPixelAlpha = false;
    }

    // Black pixels do not count as hits.
    std::uint8_t TGraphBufGI::HitTestPixel(WindowsSdk::TPoint Point) {
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        std::uint32_t Pixel{};
        WindowsSdk::TRect Clip{};
        std::uint8_t Result = false;
        if (GraphBuf->GetPixels() == nullptr) {
            return Result;
        }
        Pixel = 0u;
        Clip.Left = Point.X;
        Clip.Top = Point.Y;
        Clip.Right = Point.X + 1;
        Clip.Bottom = Point.Y + 1;
        std::int32_t Width = GraphBuf->Width;
        std::int32_t Height = GraphBuf->Height;
        if (ImageKindX == GI_Main::ikxLeftFill) {
            Left = HitTestBounds.Left;
            Right = HitTestBounds.Right;
        } else if (ImageKindX == GI_Main::ikxRightFill) {
            Right = HitTestBounds.Right;
            Left = Right;
            while (Left > Clip.Left) {
                Left -= Width;
            }
        } else if (ImageKindX == GI_Main::ikxLeft) {
            Left = HitTestBounds.Left;
            Right = Left + Width;
        } else if (ImageKindX == GI_Main::ikxRight) {
            Right = HitTestBounds.Right;
            Left = Right - Width;
        } else if (ImageKindX == GI_Main::ikxCenter) {
            Left = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - Width / 2;
            Right = Left + Width;
        } else {
            return false;
        }
        if (ImageKindY == GI_Main::ikyTopFill) {
            Top = HitTestBounds.Top;
            Bottom = HitTestBounds.Bottom;
        } else if (ImageKindY == GI_Main::ikyBottomFill) {
            Bottom = HitTestBounds.Bottom;
            Top = Bottom;
            while (Top > Clip.Top) {
                Top -= Height;
            }
        } else if (ImageKindY == GI_Main::ikyTop) {
            Top = HitTestBounds.Top;
            Bottom = Top + Height;
        } else if (ImageKindY == GI_Main::ikyBottom) {
            Bottom = HitTestBounds.Bottom;
            Top = Bottom - Height;
        } else if (ImageKindY == GI_Main::ikyCenter) {
            Top = (HitTestBounds.Bottom - HitTestBounds.Top) / 2 + HitTestBounds.Top - Height / 2;
            Bottom = Top + Height;
        } else {
            return false;
        }
        void* Pixels = EC_Mem::AddPointerOffset(&Pixel, -(GR_Main::ScreenRenderBuffer->PitchBytes * Point.Y + Point.X * static_cast<std::int32_t>(sizeof(std::uint16_t))));
        GR_GraphBuf::TGraphBufGR* Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        Buffer->AttachPixels(1, 1, GR_Main::ScreenRenderBuffer->PitchBytes, Pixels);
        if (SourceHasPerPixelAlpha) {
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        GR_GraphBuf::TGraphBufGR* graphBuf = GraphBuf;
                        void* pixels = Buffer->GetPixels();
                        std::int32_t pitchBytes = Buffer->PitchBytes;
                        GR_Main::DrawAlphaGraphBuffer16Clipped(pixels, pitchBytes, X, Y, graphBuf, Clip);
                    }
                    X += Width;
                }
                Y += Height;
            }
        } else {
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        GR_GraphBuf::TGraphBufGR* graphBuf_2 = GraphBuf;
                        std::uint8_t halfAlpha = HalfAlpha;
                        void* pixels_2 = Buffer->GetPixels();
                        std::int32_t pitchBytes_2 = Buffer->PitchBytes;
                        GR_Main::CopyGraphBuffer16Clipped(pixels_2, pitchBytes_2, X, Y, graphBuf_2, Clip, halfAlpha, false);
                    }
                    X += Width;
                }
                Y += Height;
            }
        }
        pas::free(Buffer);
        return Pixel != 0;
    }

    // Uses nonzero pixels. CenterFill is unsupported and can leave bounds changed and temporary storage leaked.
    WindowsSdk::TPoint TGraphBufGI::GetVisualCenter() {
        WindowsSdk::TPoint Result{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        WindowsSdk::TRect SavedBounds{};
        WindowsSdk::TRect Clip{};
        SavedBounds = HitTestBounds;
        HitTestBounds.Right -= HitTestBounds.Left;
        HitTestBounds.Bottom -= HitTestBounds.Top;
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        Clip = HitTestBounds;
        GR_GraphBuf::TGraphBufGR* Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
        Buffer->AllocateNative(HitTestBounds.Right, HitTestBounds.Bottom);
        std::int32_t Width = GraphBuf->Width;
        std::int32_t Height = GraphBuf->Height;
        if (ImageKindX == GI_Main::ikxLeftFill) {
            Left = HitTestBounds.Left;
            Right = HitTestBounds.Right;
        } else if (ImageKindX == GI_Main::ikxRightFill) {
            Right = HitTestBounds.Right;
            Left = Right;
            while (Left > Clip.Left) {
                Left -= Width;
            }
        } else if (ImageKindX == GI_Main::ikxLeft) {
            Left = HitTestBounds.Left;
            Right = Left + Width;
        } else if (ImageKindX == GI_Main::ikxRight) {
            Right = HitTestBounds.Right;
            Left = Right - Width;
        } else if (ImageKindX == GI_Main::ikxCenter) {
            Left = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - Width / 2;
            Right = Left + Width;
        } else {
            return Result;
        }
        if (ImageKindY == GI_Main::ikyTopFill) {
            Top = HitTestBounds.Top;
            Bottom = HitTestBounds.Bottom;
        } else if (ImageKindY == GI_Main::ikyBottomFill) {
            Bottom = HitTestBounds.Bottom;
            Top = Bottom;
            while (Top > Clip.Top) {
                Top -= Height;
            }
        } else if (ImageKindY == GI_Main::ikyTop) {
            Top = HitTestBounds.Top;
            Bottom = Top + Height;
        } else if (ImageKindY == GI_Main::ikyBottom) {
            Bottom = HitTestBounds.Bottom;
            Top = Bottom - Height;
        } else if (ImageKindY == GI_Main::ikyCenter) {
            Top = (HitTestBounds.Bottom - HitTestBounds.Top) / 2 + HitTestBounds.Top - Height / 2;
            Bottom = Top + Height;
        } else {
            return Result;
        }
        Buffer->ClearPixels();
        if (SourceHasPerPixelAlpha) {
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        GR_GraphBuf::TGraphBufGR* graphBuf = GraphBuf;
                        void* pixels = Buffer->GetPixels();
                        std::int32_t pitchBytes = Buffer->PitchBytes;
                        GR_Main::DrawAlphaGraphBuffer16Clipped(pixels, pitchBytes, X, Y, graphBuf, Clip);
                    }
                    X += Width;
                }
                Y += Height;
            }
        } else {
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        GR_GraphBuf::TGraphBufGR* graphBuf_2 = GraphBuf;
                        std::uint8_t halfAlpha = HalfAlpha;
                        void* pixels_2 = Buffer->GetPixels();
                        std::int32_t pitchBytes_2 = Buffer->PitchBytes;
                        GR_Main::CopyGraphBuffer16Clipped(pixels_2, pitchBytes_2, X, Y, graphBuf_2, Clip, halfAlpha, false);
                    }
                    X += Width;
                }
                Y += Height;
            }
        }
        Result.X = 0;
        Result.Y = 0;
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Buffer->Height - 1); cpp_range.next(Y); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Buffer->Width - 1); cpp_range_2.next(X); ) {
                if (Buffer->GetPixel16(X, Y) != 0) {
                    Result.X += X;
                    Result.Y += Y;
                    ++Count;
                }
            }
        }
        pas::free(Buffer);
        HitTestBounds = SavedBounds;
        if (Count < 1) {
            Result = ClassesImports::Point(0, 0);
        } else {
            std::int32_t cpp_arg = pas::idiv(Result.Y, Count);
            std::int32_t cpp_arg_2 = pas::idiv(Result.X, Count);
            Result = ClassesImports::Point(cpp_arg_2, cpp_arg);
        }
        return Result;
    }

    void TGraphBufGI_LoadFromConfigPath(TGraphBufGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TGraphBufGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
    }

    void TGraphBufGI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"HalfAlpha"_wref.get()) > 0) {
            SetHalfAlpha(GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"HalfAlpha"sv))));
        }
        if (Block->CountParams(u"CacheRGBA"_wref.get()) > 0) {
            LoadScaledBitmapPathAsRgba(Block->GetParam(u"CacheRGBA"sv));
        } else if (Block->CountParams(u"CacheGI"_wref.get()) > 0) {
            LoadScaledGiPath(Block->GetParam(u"CacheGI"sv));
        }
    }

    // Fits the image inside the control size while preserving its aspect ratio.
    void TGraphBufGI::LoadScaledBitmapPathAsRgba(const pas::WideString& BitmapPath) {
        LoadBitmapPathAsRgba(BitmapPath);
        if (GraphBuf->Width * ClientSize.Y >= GraphBuf->Height * ClientSize.X) {
            GraphBuf->RescaleBilinearRgba(ClientSize.X, System::Round(pas::real_divide(ClientSize.X, static_cast<std::uint32_t>(GraphBuf->Width)) * static_cast<std::uint32_t>(GraphBuf->Height)));
        } else {
            GraphBuf->RescaleBilinearRgba(System::Round(pas::real_divide(ClientSize.Y, static_cast<std::uint32_t>(GraphBuf->Height)) * static_cast<std::uint32_t>(GraphBuf->Width)), ClientSize.Y);
        }
    }

    void TGraphBufGI::LoadScaledGiPath(const pas::WideString& GiPath) {
        SourceHasPerPixelAlpha = true;
        EC_Cache::TCacheControlEC* Control = pas::construct_call<EC_CacheGI::TCGiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Control);
        Control->SetCacheKey(GiPath);
        EC_CacheGI::TCGiEC* Image = EC_CacheGI::AcquireCachedGi(Control);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Control->Release();
            };
            Image->Image->DecodeToGraphBuf(GraphBuf, false);
        }
        pas::free(Control);
        if (GraphBuf->Width * ClientSize.Y >= GraphBuf->Height * ClientSize.X) {
            GraphBuf->RescaleRgba(ClientSize.X, System::Round(pas::real_divide(ClientSize.X, static_cast<std::uint32_t>(GraphBuf->Width)) * static_cast<std::uint32_t>(GraphBuf->Height)), 5);
        } else {
            GraphBuf->RescaleRgba(System::Round(pas::real_divide(ClientSize.Y, static_cast<std::uint32_t>(GraphBuf->Height)) * static_cast<std::uint32_t>(GraphBuf->Width)), ClientSize.Y, 5);
        }
    }

    void TGraphBufGI::Draw(WindowsSdk::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        if (GraphBuf->GetPixels() == nullptr) {
            return;
        }
        std::int32_t Width = GraphBuf->Width;
        std::int32_t Height = GraphBuf->Height;
        if (ImageKindX == GI_Main::ikxLeftFill) {
            Left = HitTestBounds.Left;
            Right = HitTestBounds.Right;
        } else if (ImageKindX == GI_Main::ikxRightFill) {
            Right = HitTestBounds.Right;
            Left = Right;
            while (Left > ClipRect.Left) {
                Left -= Width;
            }
        } else if (ImageKindX == GI_Main::ikxLeft) {
            Left = HitTestBounds.Left;
            Right = Left + Width;
        } else if (ImageKindX == GI_Main::ikxRight) {
            Right = HitTestBounds.Right;
            Left = Right - Width;
        } else if (ImageKindX == GI_Main::ikxCenter) {
            Left = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - Width / 2;
            Right = Left + Width;
        } else {
            return;
        }
        if (ImageKindY == GI_Main::ikyTopFill) {
            Top = HitTestBounds.Top;
            Bottom = HitTestBounds.Bottom;
        } else if (ImageKindY == GI_Main::ikyBottomFill) {
            Bottom = HitTestBounds.Bottom;
            Top = Bottom;
            while (Top > ClipRect.Top) {
                Top -= Height;
            }
        } else if (ImageKindY == GI_Main::ikyTop) {
            Top = HitTestBounds.Top;
            Bottom = Top + Height;
        } else if (ImageKindY == GI_Main::ikyBottom) {
            Bottom = HitTestBounds.Bottom;
            Top = Bottom - Height;
        } else if (ImageKindY == GI_Main::ikyCenter) {
            Top = (HitTestBounds.Bottom - HitTestBounds.Top) / 2 + HitTestBounds.Top - Height / 2;
            Bottom = Top + Height;
        } else {
            return;
        }
        if (GlobalsV::HardwareRenderingEnabled) {
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        std::int32_t cpp_arg = 255 - pas::shl(static_cast<std::int32_t>(HalfAlpha), 7);
                        pas::ComView<Direct3D9::IDirect3DTexture9_Tag> texture = (GraphBuf->GetTexture(cpp_result), cpp_result);
                        GR_DX::DrawTexture(texture, X, Y, cpp_arg, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, false);
                    }
                    X += Width;
                }
                Y += Height;
            }
        } else if (SourceHasPerPixelAlpha) {
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        GR_GraphBuf::TGraphBufGR* graphBuf = GraphBuf;
                        WindowsSdk::TRect clipRect = ClipRect;
                        void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                        std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::DrawAlphaGraphBuffer16Clipped(pixels, pitchBytes, X, Y, graphBuf, clipRect);
                    }
                    X += Width;
                }
                Y += Height;
            }
        } else {
            Y = Top;
            while (Y < Bottom) {
                X = Left;
                while (X < Right) {
                    {
                        GR_GraphBuf::TGraphBufGR* graphBuf_2 = GraphBuf;
                        WindowsSdk::TRect clipRect_2 = ClipRect;
                        std::uint8_t halfAlpha = HalfAlpha;
                        void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                        std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::CopyGraphBuffer16Clipped(pixels_2, pitchBytes_2, X, Y, graphBuf_2, clipRect_2, halfAlpha, false);
                    }
                    X += Width;
                }
                Y += Height;
            }
        }
    }

    // Buffer is borrowed; alpha flags are unchanged.
    void TGraphBufGI::BindExternalGraphBuf(GR_GraphBuf::TGraphBufGR* Buffer) {
        if (GraphBuf != nullptr && static_cast<std::uint8_t>(UsesExternalGraphBuf ^ 1)) {
            pas::free(GraphBuf);
            GraphBuf = nullptr;
        }
        GraphBuf = Buffer;
        UsesExternalGraphBuf = true;
    }

    void TGraphBufGI::p_destroy() {
        GI_GraphBuf::TGraphBufGI_Destroy(this);
    }

    void TGraphBufGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_GraphBuf::TGraphBufGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_GraphBuf
