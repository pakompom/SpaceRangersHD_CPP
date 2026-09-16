#include "layout/GI_Cursor.hpp"
#include "types/GR_gi.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_CacheGI.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Cursor.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace GI_Cursor {
    void TCursorGI_Create(TCursorGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->ImageControl = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Self);
        Self->Active = false;
    }

    void TCursorGI_Destroy(TCursorGI* Self) {
        std::int32_t Index{};
        if (Self->AnimationTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->AnimationTimer);
            Self->AnimationTimer = nullptr;
        }
        {
            const std::int32_t cpp_last = Self->CursorHandles.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    if (Self->CursorHandles[Index] != 0) {
                        WindowsSdk::DestroyIcon(Self->CursorHandles[Index]);
                        Self->CursorHandles[Index] = 0u;
                    }
                }
            }
        }
        Self->ImagePath = pas::WideString();
        Self->CursorHandles = nullptr;
        Self->FrameIndices = nullptr;
        Self->FrameDelays = nullptr;
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TCursorGI::Clear() {
        std::int32_t Index{};
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        {
            const std::int32_t cpp_last = CursorHandles.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    if (CursorHandles[Index] != 0) {
                        WindowsSdk::DestroyIcon(CursorHandles[Index]);
                        CursorHandles[Index] = 0u;
                    }
                }
            }
        }
        ImagePath = pas::WideString();
        CursorHandles = nullptr;
        FrameIndices = nullptr;
        FrameDelays = nullptr;
        ImageControl->Clear();
    }

    void TCursorGI::SetImagePath(const pas::WideString& Path) {
        Clear();
        if (GR_Main::ShowSystemMouse) {
            if (ImagePath != Path) {
                FrameIndex = 0;
            }
            ImagePath = Path;
            RebuildSystemCursor();
        } else {
            ImageControl->SetImagePath(Path);
            SetSize(ImageControl->GetContentSize());
            ImageControl->SetSize(ClientSize);
            ImageControl->RestartPlayback();
        }
    }

    void TCursorGI::SetActive(std::uint8_t Enabled) {
        Invalidate();
        GI_MessageLoop::TObjectGI::SetActive(Enabled);
        if (GR_Main::ShowSystemMouse) {
            if (Enabled) {
                if (CursorHandles.length() - 1 >= 0) {
                    FrameIndex = 0;
                    WindowsSdk::SetCursor(CursorHandles[FrameIndex]);
                    while (WindowsSdk::ShowCursor(-1) < 0) {
                    }
                    if (FrameIndices.length() - 1 > 0) {
                        if (AnimationTimer != nullptr) {
                            MessageLoop->CancelCallbackTimer(AnimationTimer);
                            AnimationTimer = nullptr;
                        }
                        AnimationTimer = MessageLoop->ScheduleCallbackTimer(FrameDelays[FrameIndex], FrameDelays[FrameIndex], pas::bind_method<&TCursorGI::AdvanceAnimation>(this), 0);
                    }
                }
            } else {
                if (AnimationTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(AnimationTimer);
                    AnimationTimer = nullptr;
                }
                while (WindowsSdk::ShowCursor(0) >= 0) {
                }
            }
        } else {
            ImageControl->SetActive(Enabled);
            ImageControl->RestartPlayback();
        }
    }

    void TCursorGI::SetOrigin(WindowsSdk::TPoint Origin) {
        GI_MessageLoop::TObjectGI::SetOrigin(Origin);
        ImageControl->SetPosition(ClassesImports::Point(-Origin.X, -Origin.Y));
        if (GR_Main::ShowSystemMouse) {
            RebuildSystemCursor();
        }
    }

    void TCursorGI::Draw(WindowsSdk::TRect ClipRect) {
        GI_MessageLoop::TObjectGI::Draw(ClipRect);
    }

    void TCursorGI::RebuildSystemCursor() {
        EC_CacheGAI::TCGaiControlEC* GaiControl{};
        EC_CacheGI::TCGiControlEC* GiControl{};
        EC_CacheGAI::TCGaiEC* Gai{};
        EC_CacheGI::TCGiEC* Gi{};
        std::int32_t Index{};
        pas::WideString Kind{};
        pas::WideString Path{};
        GR_GraphBuf::TGraphBufGR* Buffer{};
        WindowsSdk::TIconInfo Info{};
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        {
            const std::int32_t cpp_last = CursorHandles.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    if (CursorHandles[Index] != 0) {
                        WindowsSdk::DestroyIcon(CursorHandles[Index]);
                        CursorHandles[Index] = 0u;
                    }
                }
            }
        }
        CursorHandles = nullptr;
        FrameIndices = nullptr;
        FrameDelays = nullptr;
        Path = ImagePath;
        Kind = EC_Str::ExtractNextDelimitedPartW(Path, u',');
        if (Kind == u"GAI") {
            GaiControl = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(GaiControl);
            GaiControl->SetCacheKey(Path);
            Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Gai = EC_CacheGAI::AcquireCachedGai(GaiControl);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    GaiControl->Release();
                };
                CursorHandles.set_length(Gai->GetFrameCount());
                FrameIndices.set_length(Gai->GetSequenceFrameCount(0));
                FrameDelays.set_length(Gai->GetSequenceFrameCount(0));
                Gai->FillSequenceFrameIndexTable(0, FrameIndices.data(), 4);
                Gai->FillSequenceFrameDelayTable(0, FrameDelays.data(), 4);
                {
                    const std::int32_t cpp_last_2 = CursorHandles.length() - 1;
                    if (0 <= cpp_last_2) {
                        for (Index = 0; Index <= cpp_last_2; ++Index) {
                            Gai->LoadFrameGi(Index)->DecodeToGraphBuf(Buffer, false);
                            Info.fIcon = 0;
                            {
                                std::int32_t cpp_left = Gai->LoadFrameGi(Index)->GetBoundsRect().Left;
                                std::int32_t cpp_right = cpp_left - Gai->GetBoundsRect().Left;
                                Info.xHotspot = OriginPoint.X - cpp_right;
                            }
                            {
                                std::int32_t cpp_left_2 = Gai->LoadFrameGi(Index)->GetBoundsRect().Top;
                                std::int32_t cpp_right_2 = cpp_left_2 - Gai->GetBoundsRect().Top;
                                Info.yHotspot = OriginPoint.Y - cpp_right_2;
                            }
                            Info.hbmMask = TCursorGI::CreateCursorBitmap(Buffer);
                            Info.hbmColor = Info.hbmMask;
                            CursorHandles[Index] = WindowsSdk::CreateIconIndirect(Info);
                            if (CursorHandles[Index] == 0) {
                                GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"CreateIconIndirect GetLastError=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
                            }
                            WindowsSdk::DeleteObject(Info.hbmMask);
                        }
                    }
                }
            }
            pas::free(Buffer);
            pas::free(GaiControl);
        } else if (Kind == u"GI") {
            GiControl = pas::construct_call<EC_CacheGI::TCGiControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(GiControl);
            GiControl->SetCacheKey(Path);
            Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
            Gi = EC_CacheGI::AcquireCachedGi(GiControl);
            {
                pas::ScopeExit cpp_cleanup_2 = [&]() noexcept {
                    GiControl->Release();
                };
                CursorHandles.set_length(1);
                FrameIndices.set_length(1);
                FrameDelays.set_length(1);
                FrameIndices[0] = 0;
                FrameDelays[0] = 0;
                Gi->Image->DecodeToGraphBuf(Buffer, false);
                Info.fIcon = 0;
                Info.xHotspot = OriginPoint.X;
                Info.yHotspot = OriginPoint.Y;
                Info.hbmMask = TCursorGI::CreateCursorBitmap(Buffer);
                Info.hbmColor = Info.hbmMask;
                CursorHandles[0] = WindowsSdk::CreateIconIndirect(Info);
                if (CursorHandles[0] == 0) {
                    GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"CreateIconIndirect GetLastError=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
                }
                WindowsSdk::DeleteObject(Info.hbmMask);
            }
            pas::free(Buffer);
            pas::free(GiControl);
        }
        if (FrameIndex < 0 || FrameIndices.length() - 1 < FrameIndex) {
            FrameIndex = 0;
        }
        if (Active) {
            WindowsSdk::SetCursor(CursorHandles[FrameIndices[FrameIndex]]);
            while (WindowsSdk::ShowCursor(-1) < 0) {
            }
            if (FrameIndices.length() - 1 > 0) {
                if (AnimationTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(AnimationTimer);
                    AnimationTimer = nullptr;
                }
                AnimationTimer = MessageLoop->ScheduleCallbackTimer(FrameDelays[FrameIndex], FrameDelays[FrameIndex], pas::bind_method<&TCursorGI::AdvanceAnimation>(this), 0);
            }
        }
    }

    WindowsSdk::HBITMAP PAS_STDCALL CreateDIBSection(WindowsSdk::HDC DC, pas::ConstRef<WindowsSdk::TBitmapInfo> BitmapInfo, std::uint32_t Usage, void*& Bits, WindowsImports::THandle Section, std::uint32_t Offset) {
        using CppImport = WindowsSdk::HBITMAP (PAS_STDCALL *)(WindowsSdk::HDC, const WindowsSdk::TBitmapInfo*, std::uint32_t, void**, WindowsImports::THandle, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("gdi32.dll", "CreateDIBSection");
        return cpp_import(DC, static_cast<const WindowsSdk::TBitmapInfo*>(BitmapInfo.address), Usage, &Bits, Section, Offset);
    }

    std::uint32_t TCursorGI::CreateCursorBitmap(GR_GraphBuf::TGraphBufGR* Buffer) {
        void* Bits{};
        std::uint32_t X{};
        std::uint32_t Y{};
        WindowsSdk::TBitmapV4Header Info{};
        std::int32_t Pitch = Buffer->Width * 4 & ~1;
        if ((Buffer->Width * 4 & 1) != 0) {
            Pitch += 2;
        }
        pas::fill_memory(&Info, static_cast<std::int32_t>(sizeof(WindowsSdk::TBitmapV4Header)), static_cast<std::uint8_t>(0));
        Info.bV4Size = static_cast<std::int32_t>(sizeof(WindowsSdk::TBitmapV4Header));
        Info.bV4Width = Buffer->Width;
        Info.bV4Height = -Buffer->Height;
        Info.bV4Planes = 1;
        Info.bV4BitCount = 32;
        Info.bV4V4Compression = WindowsSdk::BI_RGB;
        Info.bV4SizeImage = Buffer->Height * Pitch;
        WindowsSdk::HDC DC = WindowsSdk::GetDC(0u);
        WindowsSdk::HBITMAP Bitmap = GI_Cursor::CreateDIBSection(DC, pas::ConstRef<WindowsSdk::TBitmapInfo>(reinterpret_cast<WindowsSdk::PBitmapInfo>(&Info)), WindowsSdk::DIB_RGB_COLORS, Bits, 0u, 0u);
        if (Bitmap == 0) {
            GR_Main::RaiseWideMessage(u"DIB section"_wref.get());
        }
        std::int32_t SourceSkip = Buffer->PitchBytes - Buffer->Width * 4;
        std::int32_t DestSkip = Pitch - Buffer->Width * 4;
        void* Dest = Bits;
        void* Source = Buffer->GetPixels();
        for (auto cpp_range = pas::for_to<std::uint32_t>(0u, static_cast<std::uint32_t>(Buffer->Height - 1)); cpp_range.next(Y); ) {
            for (auto cpp_range_2 = pas::for_to<std::uint32_t>(0u, static_cast<std::uint32_t>(Buffer->Width - 1)); cpp_range_2.next(X); ) {
                pas::store_unaligned<std::uint32_t>(static_cast<System::PCardinal>(Dest), pas::load_unaligned<std::uint32_t>(static_cast<System::PCardinal>(Source)));
                Source = static_cast<std::uint8_t*>(Source) + 4;
                Dest = static_cast<std::uint8_t*>(Dest) + 4;
            }
            Source = static_cast<std::uint8_t*>(Source) + SourceSkip;
            Dest = static_cast<std::uint8_t*>(Dest) + DestSkip;
        }
        WindowsSdk::ReleaseDC(0u, DC);
        return Bitmap;
    }

    void TCursorGI::AdvanceAnimation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        ++FrameIndex;
        if (FrameIndices.length() - 1 < FrameIndex) {
            FrameIndex = 0;
        }
        WindowsSdk::SetCursor(CursorHandles[FrameIndices[FrameIndex]]);
        if (AnimationTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AnimationTimer);
            AnimationTimer = nullptr;
        }
        AnimationTimer = MessageLoop->ScheduleCallbackTimer(FrameDelays[FrameIndex], FrameDelays[FrameIndex], pas::bind_method<&TCursorGI::AdvanceAnimation>(this), 0);
    }

    void TCursorGI::p_destroy() {
        GI_Cursor::TCursorGI_Destroy(this);
    }

} // namespace GI_Cursor
