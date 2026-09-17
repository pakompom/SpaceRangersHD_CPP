#include "layout/GI_StarField.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Panel.hpp"
#include "units/GI_StarField.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"

namespace GI_StarField {
    void TStarFieldList_Create(TStarFieldList* Self) {
        pas::object_create(Self);
    }

    void TStarFieldList_Destroy(TStarFieldList* Self) {
        Self->Clear();
        pas::object_destroy(Self);
    }

    void TStarFieldList::Clear() {
        if (Points != nullptr) {
            EC_Mem::FreeEC(Points);
            Points = nullptr;
        }
        Capacity = 0;
        Count = 0;
    }

    // Grows by 100 when incremented Count reaches Capacity.
    PStarFieldPoint TStarFieldList::AllocatePoint() {
        ++Count;
        if (Count >= Capacity) {
            Capacity += 100;
            Points = static_cast<PStarFieldPoint>(EC_Mem::ReAllocREC(Points, Capacity * static_cast<std::int32_t>(sizeof(TStarFieldPoint))));
        }
        return static_cast<PStarFieldPoint>(EC_Mem::AddPointerOffset(Points, (Count - 1) * static_cast<std::int32_t>(sizeof(TStarFieldPoint))));
    }

    // Depth must be nonzero; retains the low 16 bits of Color.
    void TStarFieldList::AddPoint(float X, float Y, float Depth, std::int32_t Color) {
        PStarFieldPoint Point = AllocatePoint();
        Point->X = X;
        Point->Y = Y;
        Point->Depth = Depth;
        Point->InverseDepth = pas::real_divide(1.0L, Depth);
        Point->Color = Color;
    }

    void TStarFieldGI_Create(TStarFieldGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_Panel::TPanelGI_Create(Self, Owner);
        Self->BackgroundCache = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->BackgroundCache);
        Self->Stars = pas::construct_call<TStarFieldList>(TStarFieldList_Create);
        Self->ViewDirty = true;
        Self->MessageLoop->RegionDrawControl = Self;
        Self->Unknown150 = 0;
        Self->BackgroundScale = 8.0f;
    }

    void TStarFieldGI_Destroy(TStarFieldGI* Self) {
        Self->MessageLoop->RegionDrawControl = nullptr;
        pas::free(Self->Stars);
        if (Self->Pixels != nullptr) {
            EC_Mem::FreeEC(Self->Pixels);
            Self->Pixels = nullptr;
        }
        Self->PixelCount = 0;
        Self->PixelCapacity = 0;
        if (Self->PreviousPixels != nullptr) {
            EC_Mem::FreeEC(Self->PreviousPixels);
            Self->PreviousPixels = nullptr;
        }
        Self->PreviousPixelCount = 0;
        pas::free(Self->BackgroundCache);
        Self->BackgroundCache = nullptr;
        GI_Panel::TPanelGI_Destroy(Self);
    }

    void TStarFieldGI::SetBackgroundImage(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::Invalidate();
        BackgroundCache->SetCacheKey(Path);
    }

    void TStarFieldGI::ClearProjectedPixels() {
        PixelCount = 0;
    }

    void TStarFieldGI::GrowPixelBuffers() {
        PixelCapacity += 64;
        Pixels = static_cast<PStarFieldPixel>(EC_Mem::ReAllocREC(Pixels, PixelCapacity * static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
        PreviousPixels = static_cast<PStarFieldPixel>(EC_Mem::ReAllocREC(PreviousPixels, PixelCapacity * static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
    }

    void TStarFieldGI::RebuildProjectedPixels() {
        PStarFieldPixel Pixel{};
        WindowsSdk::TPoint Position{};
        std::int32_t I{};
        ClearProjectedPixels();
        std::int32_t Pitch = GR_Main::ScreenRenderBuffer->PitchBytes;
        std::int32_t Left = HitTestBounds.Left;
        std::int32_t Top = HitTestBounds.Top;
        std::int32_t Right = HitTestBounds.Right;
        std::int32_t Bottom = HitTestBounds.Bottom;
        PStarFieldPoint Point = Stars->Points;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Stars->Count - 1); cpp_range.next(I); ) {
            Position.X = static_cast<std::int32_t>(System::Round((static_cast<long double>(Point->X) - ViewPosition.X) * Point->InverseDepth)) + AbsolutePosition.X;
            Position.Y = static_cast<std::int32_t>(System::Round((static_cast<long double>(Point->Y) - ViewPosition.Y) * Point->InverseDepth)) + AbsolutePosition.Y;
            if (Position.X >= Left && Position.X < Right && Position.Y >= Top && Position.Y < Bottom) {
                ++PixelCount;
                if (PixelCount > PixelCapacity) {
                    GrowPixelBuffers();
                }
                Pixel = static_cast<PStarFieldPixel>(EC_Mem::AddPointerOffset(Pixels, (PixelCount - 1) * static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
                Pixel->ByteOffset = Position.X * 2 + Position.Y * Pitch;
                pas::store_unaligned<WindowsSdk::TPoint>(&Pixel->Position, Position);
                Pixel->Color = Point->Color;
            }
            Point = static_cast<PStarFieldPoint>(EC_Mem::AddPointerOffset(Point, static_cast<std::int32_t>(sizeof(TStarFieldPoint))));
        }
    }

    void TStarFieldGI::SetViewPosition(EC_Struct::TPointF Position) {
        if (ViewPosition.X != Position.X || ViewPosition.Y != Position.Y) {
            Invalidate();
            ViewPosition = Position;
            ViewDirty = true;
            Invalidate();
        }
    }

    void TStarFieldGI::SetSize(WindowsSdk::TPoint Size) {
        if (ClientSize.X != Size.X || ClientSize.Y != Size.Y) {
            GI_MessageLoop::TObjectGI::SetSize(Size);
            ViewDirty = true;
        }
    }

    void TStarFieldGI::MarkViewDirty() {
        ViewDirty = true;
    }

    void TStarFieldGI_LoadFromConfigPath(TStarFieldGI* Self, const pas::WideString& Path) {
        GI_Panel::TPanelGI_LoadFromConfigPath(Self, Path);
        Self->LoadStarFieldProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TStarFieldGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_Panel::TPanelGI::LoadFromBlock(Block);
        LoadStarFieldProperties(Block);
    }

    void TStarFieldGI::LoadStarFieldProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            SetBackgroundImage(Block->GetParam(u"Image"_wref.get()));
        }
    }

    // Empty in native code.
    void TStarFieldGI::Invalidate() {
    }

    // Updates GlobalsV.SkipSavedPixelRestore from the background rectangle change.
    void TStarFieldGI::UpdateBackgroundBounds() {
        EC_CacheGAI::TCGaiEC* Data{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t Width{};
        std::int32_t Height{};
        WindowsSdk::TRect Bounds{};
        GlobalsV::SkipSavedPixelRestore = false;
        if (GlobalsV::BGImage) {
            if (BackgroundCache->CacheKey != u"") {
                GlobalsV::SkipSavedPixelRestore = true;
                Data = EC_CacheGAI::AcquireCachedGai(BackgroundCache);
                {
                    Bounds = Data->GetBoundsRect();
                    BackgroundCache->Release();
                }
                Width = Bounds.Right - Bounds.Left;
                Height = Bounds.Bottom - Bounds.Top;
                X = static_cast<std::int32_t>(System::Round(pas::real_divide(0.0L - ViewPosition.X, BackgroundScale))) + AbsolutePosition.X - Width / 2;
                Y = static_cast<std::int32_t>(System::Round(pas::real_divide(0.0L - ViewPosition.Y, BackgroundScale))) + AbsolutePosition.Y - Height / 2;
                Bounds.Left = X;
                Bounds.Top = Y;
                Bounds.Right = X + Width;
                Bounds.Bottom = Y + Height;
                BackgroundBounds = Bounds;
                GlobalsV::SkipSavedPixelRestore = static_cast<std::uint8_t>(SysUtils::CompareMem(&BackgroundBounds, &PreviousBackgroundBounds, static_cast<std::int32_t>(sizeof(WindowsSdk::TRect))) ^ 1);
            }
        }
    }

    void TStarFieldGI::ErasePreviousFrame() {
        PStarFieldPixel Pixel{};
        void* Buffer{};
        std::int32_t I{};
        if (ViewDirty) {
            RebuildProjectedPixels();
            ViewDirty = false;
        }
        if (!GlobalsV::HardwareRenderingEnabled) {
            Buffer = GR_Main::ScreenRenderBuffer->GetPixels();
            if (!GlobalsV::SkipSavedPixelRestore) {
                if (static_cast<std::uint8_t>(GlobalsV::BGImage ^ 1) || BackgroundCache->CacheKey == u"") {
                    Pixel = PreviousPixels;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, PreviousPixelCount - 1); cpp_range.next(I); ) {
                        EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Buffer, Pixel->ByteOffset), 0);
                        Pixel = static_cast<PStarFieldPixel>(EC_Mem::AddPointerOffset(Pixel, static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
                    }
                } else {
                    Pixel = PreviousPixels;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, PreviousPixelCount - 1); cpp_range_2.next(I); ) {
                        EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Buffer, Pixel->ByteOffset), Pixel->SavedPixel);
                        Pixel = static_cast<PStarFieldPixel>(EC_Mem::AddPointerOffset(Pixel, static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
                    }
                }
            }
        }
    }

    void TStarFieldGI::DrawBackground(WindowsSdk::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        std::int32_t I{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t Width{};
        std::int32_t Height{};
        EC_CacheGAI::TCGaiEC* Data{};
        GR_gi::TgiGR* Frame{};
        WindowsSdk::TRect Intersection{};
        WindowsSdk::TRect Bounds{};
        if (static_cast<std::uint8_t>(GlobalsV::BGImage ^ 1) || BackgroundCache->CacheKey == u"" || BackgroundBounds.Top >= ClipRect.Bottom || BackgroundBounds.Bottom <= ClipRect.Top || BackgroundBounds.Left >= ClipRect.Right || BackgroundBounds.Right <= ClipRect.Left) {
            X = ClipRect.Left;
            Y = ClipRect.Top;
            Width = ClipRect.Right - X;
            Height = ClipRect.Bottom - Y;
            if (GlobalsV::HardwareRenderingEnabled) {
                GR_DX::DrawColoredRect(X, Y, Width, Height, 0u, 255, true, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
            } else {
                std::int32_t cpp_arg = GR_Main::ScreenRenderBuffer->PitchBytes * Y + X * 2;
                void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                void* addPointerOffset = EC_Mem::AddPointerOffset(pixels, cpp_arg);
                std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                GR_Main::Ex_OKGR_Fill_WORD(addPointerOffset, pitchBytes, Width, Height, 0);
            }
        } else {
            if (BackgroundBounds.Top > ClipRect.Top) {
                X = ClipRect.Left;
                Y = ClipRect.Top;
                Width = ClipRect.Right - ClipRect.Left;
                Height = BackgroundBounds.Top - ClipRect.Top;
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawColoredRect(X, Y, Width, Height, 0u, 255, true, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                } else {
                    std::int32_t cpp_arg_2 = GR_Main::ScreenRenderBuffer->PitchBytes * Y + X * 2;
                    void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                    void* addPointerOffset_2 = EC_Mem::AddPointerOffset(pixels_2, cpp_arg_2);
                    std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_Fill_WORD(addPointerOffset_2, pitchBytes_2, Width, Height, 0);
                }
            }
            if (BackgroundBounds.Bottom < ClipRect.Bottom) {
                X = ClipRect.Left;
                Y = BackgroundBounds.Bottom;
                Width = ClipRect.Right - ClipRect.Left;
                Height = ClipRect.Bottom - BackgroundBounds.Bottom;
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawColoredRect(X, Y, Width, Height, 0u, 255, true, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                } else {
                    std::int32_t cpp_arg_3 = GR_Main::ScreenRenderBuffer->PitchBytes * Y + X * 2;
                    void* pixels_3 = GR_Main::ScreenRenderBuffer->GetPixels();
                    void* addPointerOffset_3 = EC_Mem::AddPointerOffset(pixels_3, cpp_arg_3);
                    std::int32_t pitchBytes_3 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_Fill_WORD(addPointerOffset_3, pitchBytes_3, Width, Height, 0);
                }
            }
            if (BackgroundBounds.Left > ClipRect.Left) {
                X = ClipRect.Left;
                Y = BackgroundBounds.Top;
                if (Y < ClipRect.Top) {
                    Y = ClipRect.Top;
                }
                Width = BackgroundBounds.Left - ClipRect.Left;
                Height = BackgroundBounds.Bottom;
                if (Height > ClipRect.Bottom) {
                    Height = ClipRect.Bottom;
                }
                Height -= Y;
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawColoredRect(X, Y, Width, Height, 0u, 255, true, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                } else {
                    std::int32_t cpp_arg_4 = GR_Main::ScreenRenderBuffer->PitchBytes * Y + X * 2;
                    void* pixels_4 = GR_Main::ScreenRenderBuffer->GetPixels();
                    void* addPointerOffset_4 = EC_Mem::AddPointerOffset(pixels_4, cpp_arg_4);
                    std::int32_t pitchBytes_4 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_Fill_WORD(addPointerOffset_4, pitchBytes_4, Width, Height, 0);
                }
            }
            if (BackgroundBounds.Right < ClipRect.Right) {
                X = BackgroundBounds.Right;
                Y = BackgroundBounds.Top;
                if (Y < ClipRect.Top) {
                    Y = ClipRect.Top;
                }
                Width = ClipRect.Right - BackgroundBounds.Right;
                Height = BackgroundBounds.Bottom;
                if (Height > ClipRect.Bottom) {
                    Height = ClipRect.Bottom;
                }
                Height -= Y;
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawColoredRect(X, Y, Width, Height, 0u, 255, true, reinterpret_cast<WindowsSdk::PRect>(&ClipRect));
                } else {
                    std::int32_t cpp_arg_5 = GR_Main::ScreenRenderBuffer->PitchBytes * Y + X * 2;
                    void* pixels_5 = GR_Main::ScreenRenderBuffer->GetPixels();
                    void* addPointerOffset_5 = EC_Mem::AddPointerOffset(pixels_5, cpp_arg_5);
                    std::int32_t pitchBytes_5 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_Fill_WORD(addPointerOffset_5, pitchBytes_5, Width, Height, 0);
                }
            }
            Data = EC_CacheGAI::AcquireCachedGai(BackgroundCache);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    BackgroundCache->Release();
                };
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Data->GetFrameCount() - 1); cpp_range.next(I); ) {
                    Frame = Data->LoadFrameGi(I);
                    Bounds = Frame->GetBoundsRect();
                    Bounds.Left += BackgroundBounds.Left;
                    Bounds.Top += BackgroundBounds.Top;
                    Bounds.Right += BackgroundBounds.Left;
                    Bounds.Bottom += BackgroundBounds.Top;
                    if (EC_Struct::IntersectRects(Intersection, ClipRect, Bounds)) {
                        if (GlobalsV::HardwareRenderingEnabled) {
                            pas::ComView<Direct3D9::IDirect3DTexture9_Tag> orCreateFrameSurface = (Data->GetOrCreateFrameSurface(I, cpp_result), cpp_result);
                            std::int32_t top = Bounds.Top;
                            std::int32_t left = Bounds.Left;
                            GR_DX::DrawTexture(orCreateFrameSurface, left, top, 255, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&Intersection), false, false);
                        } else {
                            Frame->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, Bounds.Left, Bounds.Top, Intersection, 0, 255);
                        }
                    }
                }
            }
        }
    }

    void TStarFieldGI::PrepareFrameDraw() {
        PStarFieldPixel Pixel{};
        std::int32_t I{};
        void* Buffer{};
        if (!GlobalsV::HardwareRenderingEnabled) {
            if (GlobalsV::BGImage) {
                if (BackgroundCache->CacheKey != u"") {
                    Buffer = GR_Main::ScreenRenderBuffer->GetPixels();
                    Pixel = Pixels;
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, PixelCount - 1); cpp_range.next(I); ) {
                        Pixel->SavedPixel = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(Buffer, Pixel->ByteOffset));
                        Pixel = static_cast<PStarFieldPixel>(EC_Mem::AddPointerOffset(Pixel, static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
                    }
                }
            }
        }
    }

    void TStarFieldGI::DrawUpdateRects(WindowsSdk::TRect ClipRect) {
        Draw(ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
    }

    // Draws all projected pixels, ignoring ClipRect.
    void TStarFieldGI::Draw(WindowsSdk::TRect ClipRect) {
        void* Buffer{};
        PStarFieldPixel Pixel = Pixels;
        std::int32_t Count = PixelCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                GR_DX::QueueDrawPoint(Pixel->Position.X, Pixel->Position.Y, GR_DX::Color565ToArgb(Pixel->Color), 255);
                Pixel = static_cast<PStarFieldPixel>(EC_Mem::AddPointerOffset(Pixel, static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
                --Count;
            }
            GR_DX::FlushDrawPoints(nullptr);
        } else {
            Buffer = GR_Main::ScreenRenderBuffer->GetPixels();
            while (Count > 0) {
                EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Buffer, Pixel->ByteOffset), Pixel->Color);
                Pixel = static_cast<PStarFieldPixel>(EC_Mem::AddPointerOffset(Pixel, static_cast<std::int32_t>(sizeof(TStarFieldPixel))));
                --Count;
            }
        }
    }

    void TStarFieldGI::CommitFrameDraw() {
        if (!GlobalsV::HardwareRenderingEnabled) {
            PreviousPixelCount = PixelCount;
            Windows::CopyMemory(PreviousPixels, Pixels, PreviousPixelCount * static_cast<std::int32_t>(sizeof(TStarFieldPixel)));
            PreviousBackgroundBounds = BackgroundBounds;
        }
    }

    void TStarFieldList::p_destroy() {
        GI_StarField::TStarFieldList_Destroy(this);
    }

    void TStarFieldGI::p_destroy() {
        GI_StarField::TStarFieldGI_Destroy(this);
    }

    void TStarFieldGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_StarField::TStarFieldGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_StarField
