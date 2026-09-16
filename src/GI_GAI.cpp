#include "layout/GI_GAI.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_Sound.hpp"
#include "types/GR_gi.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheGAI.hpp"
#include "units/EC_CacheGI.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aMyFunction.hpp"

namespace GI_GAI {
    std::uint32_t GaiFrameHeap = 0u;

    void LoadGaiFrameToGraphBuf(const pas::WideString& Path, GR_GraphBuf::TGraphBufGR* GraphBuf, std::uint32_t Seed) {
        std::int32_t FrameIndex{};
        EC_Cache::TCacheControlEC* Control = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Control);
        Control->SetCacheKey(Path);
        EC_CacheGAI::TCGaiEC* Gai = EC_CacheGAI::AcquireCachedGai(Control);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                Control->Release();
            };
            FrameIndex = 0;
            if (Seed != 0) {
                FrameIndex = aMyFunction::SeededRandomIntRange(0, Gai->GetFrameCount() - 1, Seed);
            }
            Gai->LoadFrameGi(FrameIndex)->DecodeToGraphBuf(GraphBuf, false);
        }
        pas::free(Control);
    }

    void TgaiGI_Create(TgaiGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        if (GaiFrameHeap == 0) {
            GaiFrameHeap = WindowsSdk::HeapCreate(0u, 0x00008000u, 0u);
            if (GaiFrameHeap == 0) {
                pas::raise(pas::make_exception<pas::Exception>("TgaiGI.HeapCreate"_a));
            }
        }
        Self->Alpha = 255;
        Self->MainImageCache = pas::construct_call<EC_CacheGAI::TCGaiControlEC>(EC_Cache::TCacheControlEC_Create);
        EC_Cache::TCacheEC::ResetControl(Self->MainImageCache);
        Self->ImageKindX = GI_Main::ikxCenter;
        Self->ImageKindY = GI_Main::ikyCenter;
        Self->StopPlaybackRequested = false;
        Self->StopAfterOneCycle = false;
        Self->SequenceIndex = -1;
        Self->TransparentColor = 0u;
        Self->SkipImageUpdateRect = false;
    }

    void TgaiGI_Destroy(TgaiGI* Self) {
        if (Self->CachedPlaybackGraphBuf != nullptr) {
            pas::free(Self->CachedPlaybackGraphBuf);
            Self->CachedPlaybackGraphBuf = nullptr;
        }
        if (Self->AutoFrameTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->AutoFrameTimer);
            Self->AutoFrameTimer = nullptr;
        }
        pas::free(Self->MainImageCache);
        Self->MainImageCache = nullptr;
        if (Self->FirstFrameImageCache != nullptr) {
            pas::free(Self->FirstFrameImageCache);
            Self->FirstFrameImageCache = nullptr;
        }
        Self->ClearFrameSequence();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TgaiGI::Clear() {
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TgaiGI::SetImagePath(const pas::WideString& ImagePath) {
        SequenceFrame = 0;
        if (CachedPlaybackGraphBuf != nullptr) {
            pas::free(CachedPlaybackGraphBuf);
            CachedPlaybackGraphBuf = nullptr;
        }
        if (MainImageCache->CacheKey != ImagePath) {
            Invalidate();
            MainImageCache->SetCacheKey(ImagePath);
        }
    }

    pas::WideString TgaiGI::GetImagePath() {
        return MainImageCache->CacheKey;
    }

    void TgaiGI::SetFirstFrameImagePath(const pas::WideString& ImagePath) {
        SequenceFrame = 0;
        if (CachedPlaybackGraphBuf != nullptr) {
            pas::free(CachedPlaybackGraphBuf);
            CachedPlaybackGraphBuf = nullptr;
        }
        if (FirstFrameImageCache == nullptr) {
            FirstFrameImageCache = pas::construct_call<EC_CacheGI::TCGiControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(FirstFrameImageCache);
        }
        if (FirstFrameImageCache->CacheKey != ImagePath) {
            Invalidate();
            FirstFrameImageCache->SetCacheKey(ImagePath);
        }
    }

    pas::WideString TgaiGI::GetFirstFrameImagePath() {
        if (FirstFrameImageCache == nullptr) {
            return pas::WideString();
        }
        return FirstFrameImageCache->CacheKey;
    }

    void TgaiGI::SetSequenceFrame(std::int32_t FrameInSequence) {
        SequenceFrame = FrameInSequence;
        if (CachedPlaybackGraphBuf != nullptr) {
            pas::free(CachedPlaybackGraphBuf);
            CachedPlaybackGraphBuf = nullptr;
        }
        Invalidate();
    }

    void TgaiGI::SetFramePosition(std::int32_t FrameInSequence, std::uint8_t ForwardOnly) {
        if (SequenceFrame == FrameInSequence) {
            return;
        }
        if (FrameInSequence <= SequenceFrame && ForwardOnly) {
            return;
        }
        SequenceFrame = FrameInSequence;
        if (SequenceFrame < 0 || SequenceFrame >= SequenceFrameCount) {
            SequenceFrame = 0;
        }
        Invalidate();
    }

    std::int32_t TgaiGI::GetMainImageFrameCount() {
        std::int32_t Result{};
        if (FirstFrameOnly) {
            return 0;
        }
        EC_CacheGAI::TCGaiEC* Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
        {
            Result = Image->GetFrameCount();
            MainImageCache->Release();
        }
        return Result;
    }

    void TgaiGI::StopAutoPlayback() {
        StopPlaybackRequested = true;
        if (AutoFrameTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AutoFrameTimer);
            AutoFrameTimer = nullptr;
        }
    }

    void TgaiGI::RestartPlayback() {
        std::int32_t Delay{};
        StopPlaybackRequested = false;
        if (AutoFrameTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AutoFrameTimer);
            AutoFrameTimer = nullptr;
        }
        if (SequenceFrameCount > 1) {
            Delay = GetFrameDelay(SequenceFrame);
            AutoFrameTimer = MessageLoop->ScheduleCallbackTimer(Delay, Delay, pas::bind_method<&TgaiGI::AdvanceAutoFrame>(this), 0);
        }
        if (StartSoundName != u"" && SequenceFrame == 0) {
            GR_Main::SoundManager->PlaySound(StartSoundName);
        }
    }

    WindowsSdk::TPoint TgaiGI::GetContentSize() {
        WindowsSdk::TPoint Result{};
        EC_CacheGAI::TCGaiEC* Image{};
        EC_CacheGI::TCGiEC* First{};
        if (MainImageCache->CacheKey == u"") {
            Result = ClassesImports::Point(0, 0);
            return Result;
        }
        if (!FirstFrameOnly) {
            Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
            {
                Result = Image->GetCanvasSize();
                MainImageCache->Release();
            }
        } else if (FirstFrameImageCache != nullptr) {
            First = EC_CacheGI::AcquireCachedGi(FirstFrameImageCache);
            {
                Result = First->Image->GetContentSize();
                FirstFrameImageCache->Release();
            }
        } else {
            Result = ClassesImports::Point(0, 0);
        }
        return Result;
    }

    WindowsSdk::TPoint TgaiGI::GetContentOrigin() {
        WindowsSdk::TPoint Result{};
        EC_CacheGAI::TCGaiEC* Image{};
        EC_CacheGI::TCGiEC* First{};
        WindowsSdk::TRect Bounds{};
        if (MainImageCache->CacheKey == u"") {
            Result = ClassesImports::Point(0, 0);
            return Result;
        }
        if (!FirstFrameOnly) {
            Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
            {
                Bounds = Image->GetBoundsRect();
                Result = pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Bounds, 0));
                MainImageCache->Release();
            }
        } else if (FirstFrameImageCache != nullptr) {
            First = EC_CacheGI::AcquireCachedGi(FirstFrameImageCache);
            {
                Bounds = First->Image->GetBoundsRect();
                Result = pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Bounds, 0));
                FirstFrameImageCache->Release();
            }
        } else {
            Result = ClassesImports::Point(0, 0);
        }
        return Result;
    }

    void TgaiGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            Invalidate();
        }
    }

    void TgaiGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            Invalidate();
        }
    }

    void TgaiGI::SetAlpha(std::uint8_t Value) {
        if (Alpha != Value) {
            Alpha = Value;
            Invalidate();
        }
    }

    void TgaiGI::SetSize(WindowsSdk::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
    }

    void TgaiGI::ClearFrameSequence() {
        if (SequenceFrameIndexTable != nullptr) {
            EC_Mem::FreeFromHeapEC(GaiFrameHeap, SequenceFrameIndexTable);
            SequenceFrameIndexTable = nullptr;
        }
        if (SequenceFrameDelayTable != nullptr) {
            EC_Mem::FreeFromHeapEC(GaiFrameHeap, SequenceFrameDelayTable);
            SequenceFrameDelayTable = nullptr;
        }
        SequenceFrame = 0;
        SequenceFrameCount = 0;
    }

    void TgaiGI::LoadFrameSequenceFromText(const pas::WideString& FrameSpec) {
        pas::WideString Part{};
        std::int32_t Index{};
        std::int32_t Offset{};
        std::int32_t RangeCount{};
        std::int32_t Delay{};
        std::int32_t First{};
        std::int32_t Last{};
        std::int32_t TimerDelay{};
        ClearFrameSequence();
        std::int32_t Count = (EC_Str::CountDelimitedPartsW(FrameSpec, u"[]"_wref.get()) - 1) / 2;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Part = EC_Str::ExtractDelimitedPartW(FrameSpec, Index * 2 + 1, u"[]"_wref.get());
            Delay = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Part, 0, u",-"_wref.get()));
            First = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Part, 1, u",-"_wref.get()));
            Last = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Part, 2, u",-"_wref.get()));
            RangeCount = pas::abs(First - Last) + 1;
            SequenceFrameCount += RangeCount;
            SequenceFrameIndexTable = static_cast<std::int32_t*>(EC_Mem::ReAllocFromHeapREC(GaiFrameHeap, SequenceFrameIndexTable, SequenceFrameCount * static_cast<std::int32_t>(sizeof(std::int32_t))));
            SequenceFrameDelayTable = static_cast<std::int32_t*>(EC_Mem::ReAllocFromHeapREC(GaiFrameHeap, SequenceFrameDelayTable, SequenceFrameCount * static_cast<std::int32_t>(sizeof(std::int32_t))));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, RangeCount - 1); cpp_range_2.next(Offset); ) {
                EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(SequenceFrameIndexTable, (SequenceFrameCount - RangeCount + Offset) * static_cast<std::int32_t>(sizeof(std::int32_t))), First);
                EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(SequenceFrameDelayTable, (SequenceFrameCount - RangeCount + Offset) * static_cast<std::int32_t>(sizeof(std::int32_t))), Delay);
                if (First < Last) {
                    ++First;
                } else {
                    --First;
                }
            }
        }
        if (!StopPlaybackRequested) {
            TimerDelay = GetFrameDelay(SequenceFrame);
            if (AutoFrameTimer != nullptr) {
                MessageLoop->CancelCallbackTimer(AutoFrameTimer);
                AutoFrameTimer = nullptr;
            }
            AutoFrameTimer = MessageLoop->ScheduleCallbackTimer(TimerDelay, TimerDelay, pas::bind_method<&TgaiGI::AdvanceAutoFrame>(this), 0);
        }
    }

    std::int32_t TgaiGI::GetSequenceCount() {
        std::int32_t Result{};
        if (FirstFrameOnly) {
            return 0;
        }
        EC_CacheGAI::TCGaiEC* Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
        {
            Result = Image->GetSequenceCount();
            MainImageCache->Release();
        }
        return Result;
    }

    std::int32_t TgaiGI::GetSequenceFrameSourceIndex(std::int32_t FrameInSequence) {
        return EC_Mem::ReadIntegerEC(EC_Mem::AddPointerOffset(SequenceFrameIndexTable, FrameInSequence * static_cast<std::int32_t>(sizeof(std::int32_t))));
    }

    void TgaiGI::SetFrameDelay(std::int32_t FrameInSequence, std::int32_t DelayMs) {
        EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(SequenceFrameDelayTable, FrameInSequence * static_cast<std::int32_t>(sizeof(std::int32_t))), DelayMs);
    }

    std::int32_t TgaiGI::GetFrameDelay(std::int32_t FrameInSequence) {
        return EC_Mem::ReadIntegerEC(EC_Mem::AddPointerOffset(SequenceFrameDelayTable, FrameInSequence * static_cast<std::int32_t>(sizeof(std::int32_t))));
    }

    std::uint8_t TgaiGI::HitTestPixel(WindowsSdk::TPoint Point) {
        EC_CacheGAI::TCGaiEC* Image{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        std::uint32_t Pixel{};
        void* Pixels{};
        GR_GraphBuf::TGraphBufGR* Buffer{};
        GR_gi::TgiGR* Frame{};
        WindowsSdk::TRect Clip{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect FirstBounds{};
        std::uint8_t Result = false;
        Pixel = 0u;
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&Clip, 0), Point);
        Clip.Right = Point.X + 1;
        Clip.Bottom = Point.Y + 1;
        Image = nullptr;
        EC_CacheGI::TCGiEC* First = nullptr;
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                if (!FirstFrameOnly) {
                    Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
                }
                if (FirstFrameImageCache != nullptr) {
                    First = EC_CacheGI::AcquireCachedGi(FirstFrameImageCache);
                }
                if (Image != nullptr) {
                    Width = Image->GetCanvasSize().X;
                    Height = Image->GetCanvasSize().Y;
                    if (First != nullptr) {
                        FirstBounds = First->Image->GetBoundsRect();
                        WindowsSdk::UnionRect(Bounds, Image->GetBoundsRect(), FirstBounds);
                        if (!SysUtils::CompareMem(&Bounds, &FirstBounds, static_cast<std::int32_t>(sizeof(WindowsSdk::TRect)))) {
                            GR_Main::RaiseWideMessage(u"TgaiGI.Draw Pos-Size"_wref.get());
                        }
                        Width = FirstBounds.Right - FirstBounds.Left;
                        Height = FirstBounds.Bottom - FirstBounds.Top;
                        if (First->Image->GetFormat() != 0) {
                            GR_Main::RaiseWideMessage(u"TgaiGI.Draw Format gi not 0"_wref.get());
                        }
                    }
                } else if (First != nullptr) {
                    FirstBounds = First->Image->GetBoundsRect();
                    Width = FirstBounds.Right - FirstBounds.Left;
                    Height = FirstBounds.Bottom - FirstBounds.Top;
                    if (First->Image->GetFormat() != 0) {
                        GR_Main::RaiseWideMessage(u"TgaiGI.Draw Format gi not 0"_wref.get());
                    }
                } else {
                    Width = 0;
                    Height = 0;
                }
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
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
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
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                Pixels = EC_Mem::AddPointerOffset(&Pixel, -(GR_Main::ScreenRenderBuffer->PitchBytes * Point.Y + Point.X * static_cast<std::int32_t>(sizeof(std::uint16_t))));
                Buffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                Buffer->AttachPixels(1, 1, GR_Main::ScreenRenderBuffer->PitchBytes, Pixels);
                if (Image != nullptr) {
                    Bounds = Image->GetBoundsRect();
                }
                if (Image != nullptr && static_cast<std::uint8_t>(Image->HasPlaybackFlags() ^ 1)) {
                    Y = Top;
                    while (Y < Bottom) {
                        X = Left;
                        while (X < Right) {
                            Frame = Image->LoadFrameGi(GetSequenceFrameSourceIndex(SequenceFrame));
                            Frame->DrawToGraphBuf(Buffer, X + Frame->GetBoundsRect().Left - Bounds.Left, Y + Frame->GetBoundsRect().Top - Bounds.Top, Clip, 0, 255);
                            X += Width;
                        }
                        Y += Height;
                    }
                } else if (FirstFrameImageCache != nullptr) {
                    if (CachedPlaybackGraphBuf != nullptr) {
                        if (CachedPlaybackGraphBuf->Width == Width && CachedPlaybackGraphBuf->Height == Height) {
                            Y = Top;
                            while (Y < Bottom) {
                                X = Left;
                                while (X < Right) {
                                    {
                                        GR_GraphBuf::TGraphBufGR* cachedPlaybackGraphBuf = CachedPlaybackGraphBuf;
                                        void* pixels = Buffer->GetPixels();
                                        std::int32_t pitchBytes = Buffer->PitchBytes;
                                        GR_Main::DrawAlphaGraphBuffer16Clipped(pixels, pitchBytes, X, Y, cachedPlaybackGraphBuf, Clip);
                                    }
                                    X += Width;
                                }
                                Y += Height;
                            }
                        }
                    }
                }
                pas::free(Buffer);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            if (Image != nullptr) {
                MainImageCache->Release();
            }
            if (First != nullptr) {
                FirstFrameImageCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return Result;
            }
        }
        return Pixel != 0;
    }

    void TgaiGI::SetActive(std::uint8_t Value) {
        if (Active != Value) {
            GI_MessageLoop::TObjectGI::SetActive(Value);
            if (!Value) {
                if (AutoFrameTimer != nullptr) {
                    MessageLoop->CancelCallbackTimer(AutoFrameTimer);
                    AutoFrameTimer = nullptr;
                }
                if (CachedPlaybackGraphBuf != nullptr) {
                    pas::free(CachedPlaybackGraphBuf);
                    CachedPlaybackGraphBuf = nullptr;
                }
                Active = true;
                GI_MessageLoop::TObjectGI::Invalidate();
                Active = false;
            } else {
                if (!StopPlaybackRequested) {
                    RestartPlayback();
                }
                GI_MessageLoop::TObjectGI::Invalidate();
            }
        }
    }

    void TgaiGI::OnDeactivate() {
        if (AutoFrameTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(AutoFrameTimer);
            AutoFrameTimer = nullptr;
        }
        if (CachedPlaybackGraphBuf != nullptr) {
            pas::free(CachedPlaybackGraphBuf);
            CachedPlaybackGraphBuf = nullptr;
        }
        GI_MessageLoop::TObjectGI::OnDeactivate();
    }

    void TgaiGI::LoadFromConfigPath(const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI::LoadFromConfigPath(Path);
        LoadAnimationProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TgaiGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadAnimationProperties(Block);
    }

    void TgaiGI::LoadAnimationProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            MainImageCache->SetCacheKey(Block->GetParam(u"Image"_wref.get()));
        }
        if (Block->CountParams(u"ImageFirst"_wref.get()) > 0) {
            SetFirstFrameImagePath(Block->GetParam(u"ImageFirst"_wref.get()));
        }
        if (Block->CountParams(u"KindX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"KindX"_wref.get())));
        }
        if (Block->CountParams(u"KindY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"KindY"_wref.get())));
        }
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(Block->GetParam(u"AlignX"_wref.get())));
        }
        if (Block->CountParams(u"AlignY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(Block->GetParam(u"AlignY"_wref.get())));
        }
        if (Block->CountParams(u"PBuf"_wref.get()) > 0) {
            UsesPlaybackBuffer = GI_Main::ParseEnabledNameGI(Block->GetParam(u"PBuf"_wref.get()));
        }
        if (Block->CountParams(u"Stop"_wref.get()) > 0) {
            StopPlaybackRequested = GI_Main::ParseEnabledNameGI(Block->GetParam(u"Stop"_wref.get()));
        }
        if (Block->CountParams(u"Frame"_wref.get()) > 0) {
            LoadFrameSequenceFromText(Block->GetParam(u"Frame"_wref.get()));
        }
        if (Block->CountParams(u"FrameLoad"_wref.get()) > 0) {
            SequenceIndex = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"FrameLoad"_wref.get())));
        }
        if (Block->CountParams(u"Auto"_wref.get()) > 0) {
            AutoUpdateFlags = GI_Main::ParseAutoGeometryFlagsGI(Block->GetParam(u"Auto"_wref.get()));
        }
        if (Block->CountParams(u"TransColor"_wref.get()) > 0) {
            TransparentColor = GI_Main::GetColorGI(Block->GetParam(u"TransColor"_wref.get()));
        }
        if (Block->CountParams(u"SkipImageUpdateRect"_wref.get()) > 0) {
            SkipImageUpdateRect = GI_Main::ParseEnabledNameGI(Block->GetParam(u"SkipImageUpdateRect"_wref.get()));
        }
        if (Block->CountParams(u"StopAfterOneCycle"_wref.get()) > 0) {
            StopAfterOneCycle = GI_Main::ParseEnabledNameGI(Block->GetParam(u"StopAfterOneCycle"_wref.get()));
        }
        if (Block->CountParams(u"SoundStart"_wref.get()) > 0) {
            StartSoundName = Block->GetParam(u"SoundStart"_wref.get());
            if (AutoFrameTimer != nullptr) {
                MessageLoop->CancelCallbackTimer(AutoFrameTimer);
                AutoFrameTimer = nullptr;
            }
        }
    }

    void TgaiGI::UpdateAutoGeometry() {
        EC_CacheGAI::TCGaiEC* Image{};
        GI_MessageLoop::TObjectGI::UpdateAutoGeometry();
        if ((AutoUpdateFlags & GI_Main::agfPosition) == GI_Main::agfPosition) {
            WindowsSdk::TPoint contentOrigin = GetContentOrigin();
            GI_MessageLoop::TObjectGI* parent = Parent;
            SetPosition(parent->ToLocalPoint(contentOrigin));
        }
        if ((AutoUpdateFlags & GI_Main::agfSize) == GI_Main::agfSize) {
            SetSize(GetContentSize());
        }
        if (SequenceIndex >= 0) {
            ClearFrameSequence();
            if (static_cast<std::uint8_t>(FirstFrameOnly ^ 1) && MainImageCache != nullptr && MainImageCache->CacheKey != u"") {
                Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
                {
                    pas::ScopeExit cpp_cleanup = [&]() noexcept {
                        MainImageCache->Release();
                    };
                    if (SequenceIndex < 0 || SequenceIndex >= Image->GetSequenceCount()) {
                        MainImageCache->Release();
                        SequenceFrameCount = 0;
                        GR_Main::AppendLogLineThreadSafe("TgaiGI.AfterLoad. Anim not found."_a);
                        return;
                    }
                    SequenceFrameCount = Image->GetSequenceFrameCount(SequenceIndex);
                    SequenceFrameIndexTable = static_cast<std::int32_t*>(EC_Mem::ReAllocFromHeapREC(GaiFrameHeap, SequenceFrameIndexTable, SequenceFrameCount * static_cast<std::int32_t>(sizeof(std::int32_t))));
                    SequenceFrameDelayTable = static_cast<std::int32_t*>(EC_Mem::ReAllocFromHeapREC(GaiFrameHeap, SequenceFrameDelayTable, SequenceFrameCount * static_cast<std::int32_t>(sizeof(std::int32_t))));
                    Image->FillSequenceFrameIndexTable(SequenceIndex, SequenceFrameIndexTable, static_cast<std::int32_t>(sizeof(std::int32_t)));
                    Image->FillSequenceFrameDelayTable(SequenceIndex, SequenceFrameDelayTable, static_cast<std::int32_t>(sizeof(std::int32_t)));
                }
            }
        }
    }

    void TgaiGI::SetOneCycleDuration(std::int32_t DurationMs) {
        std::int32_t Index{};
        std::int32_t Delay{};
        StopAfterOneCycle = true;
        if (SequenceFrameDelayTable != nullptr) {
            Delay = System::Round(pas::real_divide(DurationMs, SequenceFrameCount));
            if (Delay < 1) {
                Delay = 1;
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, SequenceFrameCount - 1); cpp_range.next(Index); ) {
                EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(SequenceFrameDelayTable, Index * static_cast<std::int32_t>(sizeof(std::int32_t))), Delay);
            }
        }
    }

    void TgaiGI::AdvanceAutoFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Delay{};
        std::uint8_t Wrapped = false;
        ++SequenceFrame;
        if (pas::assigned(FrameAdvancedCallback)) {
            FrameAdvancedCallback(this);
        }
        if (SequenceFrame >= SequenceFrameCount) {
            SequenceFrame = 0;
            if (StartSoundName != u"") {
                GR_Main::SoundManager->PlaySound(StartSoundName);
            }
            Wrapped = true;
        }
        if (StopPlaybackRequested) {
            if (AutoFrameTimer != nullptr) {
                MessageLoop->CancelCallbackTimer(AutoFrameTimer);
                AutoFrameTimer = nullptr;
            }
        } else if (AutoFrameTimer == nullptr) {
            Delay = GetFrameDelay(SequenceFrame);
            AutoFrameTimer = MessageLoop->ScheduleCallbackTimer(Delay, Delay, pas::bind_method<&TgaiGI::AdvanceAutoFrame>(this), 0);
        } else {
            Delay = GetFrameDelay(SequenceFrame);
            MessageLoop->UpdateCallbackTimer(AutoFrameTimer, Delay, Delay);
        }
        Invalidate();
        if (Wrapped) {
            if (StopAfterOneCycle) {
                StopAutoPlayback();
                SetActive(false);
            }
            if (pas::assigned(CycleCompleteCallback)) {
                CycleCompleteCallback(this);
            }
        }
    }

    void TgaiGI::Invalidate() {
        EC_CacheGAI::TCGaiEC* Image{};
        EC_CacheGI::TCGiEC* First{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        std::int32_t FrameIndex{};
        std::int32_t RectIndex{};
        std::int32_t RectCount{};
        GR_gi::TgiGR* Frame{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect FirstBounds{};
        WindowsSdk::TRect Rect{};
        WindowsSdk::TRect Clip{};
        WindowsSdk::TRect FrameBounds{};
        if (static_cast<std::uint8_t>(Active ^ 1) || GlobalsV::HardwareRenderingEnabled) {
            return;
        }
        if (static_cast<std::uint8_t>(SkipImageUpdateRect ^ 1) && FirstFrameImageCache != nullptr && static_cast<std::uint8_t>(FirstFrameOnly ^ 1) && UsesPlaybackBuffer) {
            if (CachedPlaybackGraphBuf == nullptr || LastCachedFrameIndex < 0 || SequenceFrame < LastCachedFrameIndex) {
                GI_MessageLoop::TObjectGI::Invalidate();
                return;
            }
            if (SequenceFrame <= LastCachedFrameIndex) {
                return;
            }
            Image = nullptr;
            First = nullptr;
            Clip = HitTestBounds;
            {
                pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
                std::exception_ptr cpp_error{};
                try {
                    Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
                    First = EC_CacheGI::AcquireCachedGi(FirstFrameImageCache);
                    FirstBounds = First->Image->GetBoundsRect();
                    WindowsSdk::UnionRect(Bounds, Image->GetBoundsRect(), FirstBounds);
                    if (!SysUtils::CompareMem(&Bounds, &FirstBounds, static_cast<std::int32_t>(sizeof(WindowsSdk::TRect)))) {
                        GR_Main::RaiseWideMessage(u"TgaiGI.Update Pos-Size"_wref.get());
                    }
                    Width = FirstBounds.Right - FirstBounds.Left;
                    Height = FirstBounds.Bottom - FirstBounds.Top;
                    if (First->Image->GetFormat() != 0) {
                        GR_Main::RaiseWideMessage(u"TgaiGI.Update Format gi not 0"_wref.get());
                    }
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
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
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
                        cpp_flow = pas::FinallyFlow::Return;
                        goto cpp_cleanup;
                    }
                    Bounds = Image->GetBoundsRect();
                    Y = Top;
                    while (Y < Bottom) {
                        X = Left;
                        while (X < Right) {
                            FrameIndex = LastCachedFrameIndex + 1;
                            if (FrameIndex > SequenceFrame) {
                                FrameIndex = 0;
                            }
                            while (FrameIndex <= SequenceFrame) {
                                Frame = Image->LoadFrameGi(GetSequenceFrameSourceIndex(FrameIndex));
                                if (Frame != nullptr) {
                                    FrameBounds = Frame->GetBoundsRect();
                                    RectCount = Frame->GetClipRectCount();
                                    if (RectCount < 1) {
                                        GI_MessageLoop::TObjectGI::Invalidate();
                                        cpp_flow = pas::FinallyFlow::Return;
                                        goto cpp_cleanup;
                                    }
                                    for (auto cpp_range = pas::for_to<std::int32_t>(0, RectCount - 1); cpp_range.next(RectIndex); ) {
                                        Rect = Frame->GetClipRect(RectIndex);
                                        Rect.Left = X + Rect.Left + (FrameBounds.Left - Bounds.Left);
                                        Rect.Top = Y + Rect.Top + (FrameBounds.Top - Bounds.Top);
                                        Rect.Right = X + Rect.Right + (FrameBounds.Left - Bounds.Left);
                                        Rect.Bottom = Y + Rect.Bottom + (FrameBounds.Top - Bounds.Top);
                                        MessageLoop->QueueUpdateRect(Rect);
                                    }
                                }
                                ++FrameIndex;
                            }
                            X += Width;
                        }
                        Y += Height;
                    }
                } catch (...) {
                    cpp_error = std::current_exception();
                }
                cpp_cleanup:;
                if (Image != nullptr) {
                    MainImageCache->Release();
                }
                if (First != nullptr) {
                    FirstFrameImageCache->Release();
                }
                if (cpp_error) {
                    std::rethrow_exception(cpp_error);
                }
                if (cpp_flow == pas::FinallyFlow::Return) {
                    return;
                }
            }
        } else {
            GI_MessageLoop::TObjectGI::Invalidate();
        }
    }

    void TgaiGI::SetHardwareMirrorHorizontal(std::uint8_t Value) {
        HardwareMirrorHorizontal = Value;
    }

    void TgaiGI::Draw(WindowsSdk::TRect ClipRect) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        Direct3D9::IDirect3DTexture9 cpp_result_2{};
        Direct3D9::IDirect3DTexture9 cpp_result_3{};
        Direct3D9::IDirect3DTexture9 cpp_result_4{};
        EC_CacheGAI::TCGaiEC* Image{};
        std::int32_t Width{};
        std::int32_t Height{};
        std::int32_t Left{};
        std::int32_t Right{};
        std::int32_t X{};
        std::int32_t Top{};
        std::int32_t Bottom{};
        std::int32_t Y{};
        std::int32_t FrameIndex{};
        std::int32_t FrameCount{};
        GR_gi::TgiGR* Frame{};
        Direct3D9::IDirect3DTexture9 Texture{};
        WindowsSdk::TPoint FrameOrigin{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect FirstBounds{};
        if (SequenceFrame < 0) {
            return;
        }
        if (SequenceFrame >= SequenceFrameCount && static_cast<std::uint8_t>(FirstFrameOnly ^ 1)) {
            return;
        }
        if (AutoFrameTimer == nullptr && static_cast<std::uint8_t>(StopPlaybackRequested ^ 1)) {
            RestartPlayback();
        }
        if (MainImageCache->CacheKey == u"" && (FirstFrameImageCache == nullptr || FirstFrameImageCache->CacheKey == u"" || static_cast<std::uint8_t>(FirstFrameOnly ^ 1))) {
            return;
        }
        Image = nullptr;
        EC_CacheGI::TCGiEC* First = nullptr;
        {
            pas::FinallyFlow cpp_flow = pas::FinallyFlow::Normal;
            std::exception_ptr cpp_error{};
            try {
                if (!FirstFrameOnly) {
                    Image = EC_CacheGAI::AcquireCachedGai(MainImageCache);
                }
                if (FirstFrameImageCache != nullptr) {
                    First = EC_CacheGI::AcquireCachedGi(FirstFrameImageCache);
                }
                if (Image != nullptr) {
                    Width = Image->GetCanvasSize().X;
                    Height = Image->GetCanvasSize().Y;
                    if (First != nullptr) {
                        FirstBounds = First->Image->GetBoundsRect();
                        WindowsSdk::UnionRect(Bounds, Image->GetBoundsRect(), FirstBounds);
                        if (!SysUtils::CompareMem(&Bounds, &FirstBounds, static_cast<std::int32_t>(sizeof(WindowsSdk::TRect)))) {
                            GR_Main::RaiseWideMessage(u"TgaiGI.Draw Pos-Size"_wref.get());
                        }
                        Width = FirstBounds.Right - FirstBounds.Left;
                        Height = FirstBounds.Bottom - FirstBounds.Top;
                        if (First->Image->GetFormat() != 0) {
                            GR_Main::RaiseWideMessage(u"TgaiGI.Draw Format gi not 0"_wref.get());
                        }
                    }
                } else if (First != nullptr) {
                    FirstBounds = First->Image->GetBoundsRect();
                    Width = FirstBounds.Right - FirstBounds.Left;
                    Height = FirstBounds.Bottom - FirstBounds.Top;
                    if (First->Image->GetFormat() != 0) {
                        GR_Main::RaiseWideMessage(u"TgaiGI.Draw Format gi not 0"_wref.get());
                    }
                } else {
                    Width = 0;
                    Height = 0;
                }
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
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
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
                    cpp_flow = pas::FinallyFlow::Return;
                    goto cpp_cleanup;
                }
                if (Image != nullptr) {
                    Bounds = Image->GetBoundsRect();
                } else if (First != nullptr) {
                    Bounds = First->Image->GetBoundsRect();
                }
                if (Image != nullptr && static_cast<std::uint8_t>(Image->HasPlaybackFlags() ^ 1)) {
                    if (GlobalsV::HardwareRenderingEnabled) {
                        Texture = (Image->GetOrCreateFrameSurface(GetSequenceFrameSourceIndex(SequenceFrame), cpp_result), cpp_result);
                        FrameOrigin = Image->GetFrameOrigin(GetSequenceFrameSourceIndex(SequenceFrame));
                        if (Texture != nullptr) {
                            Y = Top;
                            while (Y < Bottom) {
                                X = Left;
                                while (X < Right) {
                                    GR_DX::DrawTexture(Texture, FrameOrigin.X + X, FrameOrigin.Y + Y, Alpha, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, HardwareMirrorHorizontal);
                                    X += Width;
                                }
                                Y += Height;
                            }
                        }
                    } else {
                        Frame = Image->LoadFrameGi(GetSequenceFrameSourceIndex(SequenceFrame));
                        Y = Top;
                        while (Y < Bottom) {
                            X = Left;
                            while (X < Right) {
                                if (Frame != nullptr) {
                                    Frame->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, X + Frame->GetBoundsRect().Left - Bounds.Left, Y + Frame->GetBoundsRect().Top - Bounds.Top, ClipRect, 0, Alpha);
                                }
                                X += Width;
                            }
                            Y += Height;
                        }
                    }
                } else if (Image != nullptr && static_cast<std::uint8_t>(UsesPlaybackBuffer ^ 1)) {
                    Y = Top;
                    if (GlobalsV::HardwareRenderingEnabled) {
                        while (Y < Bottom) {
                            X = Left;
                            while (X < Right) {
                                FrameCount = GetSequenceFrameSourceIndex(SequenceFrame);
                                for (auto cpp_range = pas::for_to<std::int32_t>(0, FrameCount - 1); cpp_range.next(FrameIndex); ) {
                                    Texture = (Image->GetOrCreateFrameSurface(GetSequenceFrameSourceIndex(SequenceFrame), cpp_result_2), cpp_result_2);
                                    FrameOrigin = Image->GetFrameOrigin(GetSequenceFrameSourceIndex(SequenceFrame));
                                    GR_DX::DrawTexture(Texture, FrameOrigin.X + X, FrameOrigin.Y + Y, Alpha, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, HardwareMirrorHorizontal);
                                }
                                X += Width;
                            }
                            Y += Height;
                        }
                    } else {
                        while (Y < Bottom) {
                            X = Left;
                            while (X < Right) {
                                FrameCount = GetSequenceFrameSourceIndex(SequenceFrame);
                                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, FrameCount - 1); cpp_range_2.next(FrameIndex); ) {
                                    Frame = Image->LoadFrameGi(GetSequenceFrameSourceIndex(FrameIndex));
                                    if (Frame != nullptr) {
                                        Frame->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, X + Frame->GetBoundsRect().Left - Bounds.Left, Y + Frame->GetBoundsRect().Top - Bounds.Top, ClipRect, 0, Alpha);
                                    }
                                }
                                X += Width;
                            }
                            Y += Height;
                        }
                    }
                } else if (FirstFrameImageCache != nullptr) {
                    if (CachedPlaybackGraphBuf == nullptr || CachedPlaybackGraphBuf->Width != Width || CachedPlaybackGraphBuf->Height != Height) {
                        LastCachedFrameIndex = -1;
                        if (CachedPlaybackGraphBuf == nullptr) {
                            CachedPlaybackGraphBuf = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, true);
                        }
                        CachedPlaybackGraphBuf->AllocateRgba(Width, Height, Width * 4);
                    }
                    if (LastCachedFrameIndex != SequenceFrame) {
                        FrameIndex = LastCachedFrameIndex + 1;
                        if (FrameIndex > SequenceFrame) {
                            FrameIndex = 0;
                        }
                        if (FrameIndex == 0) {
                            void* addPointerOffset = EC_Mem::AddPointerOffset(First->Image->Data, First->Image->GetPlane(0)->DataOffset);
                            void* pixels = CachedPlaybackGraphBuf->GetPixels();
                            std::uint32_t cpp_arg = CachedPlaybackGraphBuf->PitchBytes * CachedPlaybackGraphBuf->Height;
                            Windows::CopyMemory(pixels, addPointerOffset, cpp_arg);
                        }
                        if (Image != nullptr) {
                            while (FrameIndex <= SequenceFrame) {
                                Frame = Image->LoadFrameGi(GetSequenceFrameSourceIndex(FrameIndex));
                                if (Frame != nullptr && Frame->GetContentSize().X > 0 && Frame->GetContentSize().Y > 0) {
                                    Frame->DrawToGraphBuf(CachedPlaybackGraphBuf, Frame->GetBoundsRect().Left - Bounds.Left, Frame->GetBoundsRect().Top - Bounds.Top, ClassesImports::Rect(0, 0, CachedPlaybackGraphBuf->Width, CachedPlaybackGraphBuf->Height), 0, Alpha);
                                }
                                ++FrameIndex;
                            }
                        }
                        LastCachedFrameIndex = SequenceFrame;
                    }
                    Y = Top;
                    if (GlobalsV::HardwareRenderingEnabled) {
                        Texture = (CachedPlaybackGraphBuf->GetTexture(cpp_result_3), cpp_result_3);
                        while (Y < Bottom) {
                            X = Left;
                            while (X < Right) {
                                GR_DX::DrawTexture(Texture, X, Y, Alpha, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, HardwareMirrorHorizontal);
                                X += Width;
                            }
                            Y += Height;
                        }
                    } else {
                        while (Y < Bottom) {
                            X = Left;
                            while (X < Right) {
                                {
                                    GR_GraphBuf::TGraphBufGR* cachedPlaybackGraphBuf = CachedPlaybackGraphBuf;
                                    WindowsSdk::TRect clipRect = ClipRect;
                                    void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                                    std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                                    GR_Main::DrawAlphaGraphBuffer16Clipped(pixels_2, pitchBytes, X, Y, cachedPlaybackGraphBuf, clipRect);
                                }
                                X += Width;
                            }
                            Y += Height;
                        }
                    }
                } else if (Image != nullptr) {
                    if (CachedPlaybackGraphBuf == nullptr || CachedPlaybackGraphBuf->Width != Width || CachedPlaybackGraphBuf->Height != Height) {
                        LastCachedFrameIndex = -1;
                        if (CachedPlaybackGraphBuf == nullptr) {
                            CachedPlaybackGraphBuf = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, true);
                        }
                        if (GlobalsV::HardwareRenderingEnabled) {
                            CachedPlaybackGraphBuf->AllocateRgba(Width, Height, Width * 4);
                        } else {
                            CachedPlaybackGraphBuf->AllocateNative(Width, Height);
                        }
                    }
                    if (LastCachedFrameIndex != SequenceFrame) {
                        FrameIndex = LastCachedFrameIndex + 1;
                        if (FrameIndex > SequenceFrame) {
                            FrameIndex = 0;
                        }
                        if (FrameIndex == 0 && static_cast<std::uint8_t>(GlobalsV::HardwareRenderingEnabled ^ 1)) {
                            CachedPlaybackGraphBuf->FillPixels16(TransparentColor);
                        }
                        while (FrameIndex <= SequenceFrame) {
                            Frame = Image->LoadFrameGi(GetSequenceFrameSourceIndex(FrameIndex));
                            if (Frame != nullptr) {
                                if (GlobalsV::HardwareRenderingEnabled) {
                                    std::int32_t width = CachedPlaybackGraphBuf->Width;
                                    std::int32_t height = CachedPlaybackGraphBuf->Height;
                                    std::int32_t cpp_arg_2 = (Frame->GetBoundsRect().Top - Bounds.Top) * CachedPlaybackGraphBuf->PitchBytes + (Frame->GetBoundsRect().Left - Bounds.Left) * 4;
                                    void* pixels_3 = CachedPlaybackGraphBuf->GetPixels();
                                    void* addPointerOffset_2 = EC_Mem::AddPointerOffset(pixels_3, cpp_arg_2);
                                    std::int32_t pitchBytes_2 = CachedPlaybackGraphBuf->PitchBytes;
                                    Frame->DecodeToPixels(addPointerOffset_2, pitchBytes_2, width, height, false);
                                } else {
                                    Frame->DrawToGraphBuf(CachedPlaybackGraphBuf, Frame->GetBoundsRect().Left - Bounds.Left, Frame->GetBoundsRect().Top - Bounds.Top, ClassesImports::Rect(0, 0, CachedPlaybackGraphBuf->Width, CachedPlaybackGraphBuf->Height), 0, Alpha);
                                }
                            }
                            ++FrameIndex;
                        }
                        LastCachedFrameIndex = SequenceFrame;
                    }
                    Y = Top;
                    if (GlobalsV::HardwareRenderingEnabled) {
                        Texture = (CachedPlaybackGraphBuf->GetTexture(cpp_result_4), cpp_result_4);
                        while (Y < Bottom) {
                            X = Left;
                            while (X < Right) {
                                GR_DX::DrawTexture(Texture, X, Y, Alpha, 0x00ffffffu, reinterpret_cast<WindowsSdk::PRect>(&ClipRect), false, HardwareMirrorHorizontal);
                                X += Width;
                            }
                            Y += Height;
                        }
                    } else {
                        while (Y < Bottom) {
                            X = Left;
                            while (X < Right) {
                                {
                                    GR_GraphBuf::TGraphBufGR* cachedPlaybackGraphBuf_2 = CachedPlaybackGraphBuf;
                                    WindowsSdk::TRect clipRect_2 = ClipRect;
                                    std::uint16_t transparentColor = TransparentColor;
                                    void* pixels_4 = GR_Main::ScreenRenderBuffer->GetPixels();
                                    std::int32_t pitchBytes_3 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                    GR_Main::CopyTransparentGraphBuffer16Clipped(pixels_4, pitchBytes_3, X, Y, cachedPlaybackGraphBuf_2, clipRect_2, transparentColor);
                                }
                                X += Width;
                            }
                            Y += Height;
                        }
                    }
                }
            } catch (...) {
                cpp_error = std::current_exception();
            }
            cpp_cleanup:;
            if (Image != nullptr) {
                MainImageCache->Release();
            }
            if (First != nullptr) {
                FirstFrameImageCache->Release();
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
            if (cpp_flow == pas::FinallyFlow::Return) {
                return;
            }
        }
    }

    void TgaiGI::PrimeImageCaches() {
        if (MainImageCache != nullptr && static_cast<std::uint8_t>(FirstFrameOnly ^ 1) && MainImageCache->CacheKey != u"") {
            EC_CacheGAI::AcquireCachedGai(MainImageCache);
            MainImageCache->Release();
        }
        if (FirstFrameImageCache != nullptr && FirstFrameImageCache->CacheKey != u"") {
            EC_CacheGI::AcquireCachedGi(FirstFrameImageCache);
            FirstFrameImageCache->Release();
        }
    }

    void TgaiGI::QueueImageLoad(pas::List* PendingLoads) {
        MainImageCache->QueueLoadIfMissing(PendingLoads);
    }

    void TgaiGI::p_destroy() {
        GI_GAI::TgaiGI_Destroy(this);
    }

} // namespace GI_GAI
