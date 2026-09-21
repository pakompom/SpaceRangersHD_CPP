#include "layout/GI_GAIFile.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/WindowsImports.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_GAIFile.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_gi.hpp"

namespace GI_GAIFile {
    std::uint32_t ReadGaiFrameSize(void* Directory, std::int32_t Index) {
        return EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(Directory, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&reinterpret_cast<GR_gi::PGaiFrameEntry>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Index * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)))))->DataSize))));
    }

    void TGAIFileThreadGI_Execute(TGAIFileThreadGI* Self) {
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t SourceFrame{};
        void* Data{};
        while (!Self->IsStopRequested()) {
            Count = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Self->Owner->Header.FrameCount - 1); cpp_range.next(Index); ) {
                if (EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(Self->Owner->FrameBuffers, Index * static_cast<std::int32_t>(sizeof(void*)))) > 0) {
                    ++Count;
                }
            }
            if (Count >= Self->Owner->PreloadCount) {
                break;
            }
            pas::critical_enter(Self->Owner->FrameLock);
            Index = Self->Owner->CurrentFrame;
            pas::critical_leave(Self->Owner->FrameLock);
            Count = 0;
            while (Count < Self->Owner->GetFrameCount()) {
                if (EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(Self->Owner->FrameBuffers, Self->Owner->GetSequenceFrame(Index) * static_cast<std::int32_t>(sizeof(void*)))) == 0) {
                    break;
                }
                ++Index;
                if (Index >= Self->Owner->GetFrameCount()) {
                    Index = 0;
                }
                ++Count;
            }
            if (Count >= Self->Owner->GetFrameCount()) {
                break;
            }
            SourceFrame = Self->Owner->GetSequenceFrame(Index);
            if (Self->IsStopRequested()) {
                break;
            }
            Data = EC_Mem::AllocEC(GI_GAIFile::ReadGaiFrameSize(Self->Owner->FrameDirectory, SourceFrame));
            Self->Owner->ImageFile->SetPointer(EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(Self->Owner->FrameDirectory, SourceFrame * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)))), WindowsImports::FILE_BEGIN);
            Self->Owner->ImageFile->ReadBuffer(Data, GI_GAIFile::ReadGaiFrameSize(Self->Owner->FrameDirectory, SourceFrame));
            GR_gi::PrepareRawGiColorCache(Data);
            if (Self->IsStopRequested()) {
                EC_Mem::FreeEC(Data);
                return;
            }
            pas::critical_enter(Self->Owner->FrameLock);
            EC_Mem::WriteIntegerEC(EC_Mem::AddPointerOffset(Self->Owner->FrameBuffers, SourceFrame * static_cast<std::int32_t>(sizeof(void*))), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Data)));
            pas::critical_leave(Self->Owner->FrameLock);
        }
    }

    void TGAIFileGI_Create(TGAIFileGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->FrameLock = pas::make_critical_section<pas::CriticalSection>();
        Self->FrameImage = pas::construct_call<GR_gi::TgiGR>(GR_gi::TgiGR_Create);
        Self->ImageFile = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        Self->LoaderThread = pas::construct_call<TGAIFileThreadGI>(EC_Thread::TThreadEC_Create);
        Self->LoaderThread->Owner = Self;
        Self->LoaderThread->SetPriority(1);
        Self->ImageKindX = GI_Main::ikxCenter;
        Self->ImageKindY = GI_Main::ikyCenter;
        Self->Stopped = false;
        Self->PreloadCount = 10;
        Self->TransparentColor = 0u;
    }

    void TGAIFileGI_Destroy(TGAIFileGI* Self) {
        Self->CloseImage();
        if (Self->LoaderThread != nullptr) {
            pas::free(Self->LoaderThread);
            Self->LoaderThread = nullptr;
        }
        if (Self->FrameTimer != nullptr) {
            Self->MessageLoop->CancelCallbackTimer(Self->FrameTimer);
            Self->FrameTimer = nullptr;
        }
        if (Self->SequenceFrames != nullptr) {
            EC_Mem::FreeEC(Self->SequenceFrames);
            Self->SequenceFrames = nullptr;
        }
        if (Self->FrameDelays != nullptr) {
            EC_Mem::FreeEC(Self->FrameDelays);
            Self->FrameDelays = nullptr;
        }
        if (Self->ImageFile != nullptr) {
            pas::free(Self->ImageFile);
            Self->ImageFile = nullptr;
        }
        if (Self->FrameImage != nullptr) {
            pas::free(Self->FrameImage);
            Self->FrameImage = nullptr;
        }
        if (Self->FrameLock != nullptr) {
            pas::free(Self->FrameLock);
            Self->FrameLock = nullptr;
        }
        if (Self->PlaybackBuffer != nullptr) {
            pas::free(Self->PlaybackBuffer);
            Self->PlaybackBuffer = nullptr;
        }
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TGAIFileGI::Clear() {
        CloseImage();
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TGAIFileGI::OpenImage() {
        CloseImage();
        ImageFile->AcquireReadHandle(false);
        ImageFile->ReadBuffer(&Header, static_cast<std::int32_t>(sizeof(GR_gi::TGaiHeader)));
        FrameDirectory = EC_Mem::ReAllocREC(FrameDirectory, Header.FrameCount * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)));
        ImageFile->ReadBuffer(FrameDirectory, Header.FrameCount * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)));
        FrameBuffers = EC_Mem::AllocClearEC(Header.FrameCount * static_cast<std::int32_t>(sizeof(void*)));
        LoaderThread->Start();
    }

    void TGAIFileGI::CloseImage() {
        void* Data{};
        std::int32_t I{};
        if (LoaderThread != nullptr && LoaderThread->IsRunning()) {
            LoaderThread->RequestStop();
            LoaderThread->WaitForIdle(0xffffffffu);
        }
        if (ImageFile != nullptr && ImageFile->OpenDepth > 0) {
            ImageFile->ReleaseHandle();
        }
        if (FrameDirectory != nullptr) {
            EC_Mem::FreeEC(FrameDirectory);
            FrameDirectory = nullptr;
        }
        if (FrameBuffers != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Header.FrameCount - 1); cpp_range.next(I); ) {
                Data = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(FrameBuffers, I * static_cast<std::int32_t>(sizeof(void*)))))));
                if (Data != nullptr) {
                    EC_Mem::FreeEC(Data);
                }
            }
            EC_Mem::FreeEC(FrameBuffers);
            FrameBuffers = nullptr;
        }
    }

    void* TGAIFileGI::GetFrameData(std::int32_t FrameIndex) {
        pas::critical_enter(FrameLock);
        void* Data = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(FrameBuffers, FrameIndex * static_cast<std::int32_t>(sizeof(void*)))))));
        pas::critical_leave(FrameLock);
        if (Data != nullptr) {
            return Data;
        }
        if (LoaderThread->IsRunning()) {
            LoaderThread->RequestStop();
            LoaderThread->WaitForIdle(0xffffffffu);
        }
        Data = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(FrameBuffers, FrameIndex * static_cast<std::int32_t>(sizeof(void*)))))));
        if (Data != nullptr) {
            return Data;
        }
        Data = EC_Mem::AllocEC(GI_GAIFile::ReadGaiFrameSize(FrameDirectory, FrameIndex));
        ImageFile->SetPointer(EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(FrameDirectory, FrameIndex * static_cast<std::int32_t>(sizeof(GR_gi::TGaiFrameEntry)))), WindowsImports::FILE_BEGIN);
        ImageFile->ReadBuffer(Data, GI_GAIFile::ReadGaiFrameSize(FrameDirectory, FrameIndex));
        GR_gi::PrepareRawGiColorCache(Data);
        EC_Mem::WriteIntegerEC(EC_Mem::AddPointerOffset(FrameBuffers, FrameIndex * static_cast<std::int32_t>(sizeof(void*))), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Data)));
        LoaderThread->Start();
        return Data;
    }

    void TGAIFileGI::TrimFrameCache() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Index{};
        void* Data{};
        if (LoaderThread->IsRunning()) {
            LoaderThread->RequestStop();
            LoaderThread->WaitForIdle(0xffffffffu);
        }
        if (SequenceFrameCount > PreloadCount) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Header.FrameCount - 1); cpp_range.next(I); ) {
                Data = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(FrameBuffers, I * static_cast<std::int32_t>(sizeof(void*)))))));
                if (Data != nullptr) {
                    Index = CurrentFrame;
                    J = 0;
                    while (J < PreloadCount) {
                        if (GetSequenceFrame(Index) == I) {
                            break;
                        }
                        ++Index;
                        if (Index >= SequenceFrameCount) {
                            Index = 0;
                        }
                        ++J;
                    }
                    if (J >= PreloadCount) {
                        EC_Mem::FreeEC(Data);
                        EC_Mem::WriteIntegerEC(EC_Mem::AddPointerOffset(FrameBuffers, I * static_cast<std::int32_t>(sizeof(void*))), 0);
                    }
                }
            }
            LoaderThread->Start();
        }
    }

    std::int32_t TGAIFileGI::GetFrameCount() {
        if (ImageFile->GetFileName() == u"") {
            return 0;
        }
        if (ImageFile->OpenDepth < 1) {
            OpenImage();
        }
        return Header.FrameCount;
    }

    WindowsSdk::TPoint TGAIFileGI::GetContentSize() {
        WindowsSdk::TPoint Result{};
        if (ImageFile->GetFileName() == u"") {
            Result = ClassesImports::Point(0, 0);
        } else {
            if (ImageFile->OpenDepth < 1) {
                OpenImage();
            }
            Result = EC_Struct::SubtractPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&Header.Bounds, 8)), pas::load_unaligned<Types::TPoint>(pas::byte_offset(&Header.Bounds, 0)));
        }
        return Result;
    }

    WindowsSdk::TPoint TGAIFileGI::GetContentOrigin() {
        WindowsSdk::TPoint Result{};
        if (ImageFile->GetFileName() == u"") {
            Result = ClassesImports::Point(0, 0);
        } else {
            if (ImageFile->OpenDepth < 1) {
                OpenImage();
            }
            Result = pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&Header.Bounds, 0));
        }
        return Result;
    }

    void TGAIFileGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (ImageKindX != Value) {
            ImageKindX = Value;
            Invalidate();
        }
    }

    void TGAIFileGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (ImageKindY != Value) {
            ImageKindY = Value;
            Invalidate();
        }
    }

    void TGAIFileGI::SetSize(WindowsSdk::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
    }

    void TGAIFileGI::SetFrameSequence(pas::WideString Sequence) {
        pas::WideString Text{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t FrameCount{};
        std::int32_t Delay{};
        std::int32_t First{};
        std::int32_t Last{};
        if (LoaderThread->IsRunning()) {
            LoaderThread->RequestStop();
            LoaderThread->WaitForIdle(0xffffffffu);
        }
        if (SequenceFrames != nullptr) {
            EC_Mem::FreeEC(SequenceFrames);
            SequenceFrames = nullptr;
        }
        if (FrameDelays != nullptr) {
            EC_Mem::FreeEC(FrameDelays);
            FrameDelays = nullptr;
        }
        CurrentFrame = 0;
        SequenceFrameCount = 0;
        std::int32_t Count = (EC_Str::CountDelimitedPartsW(pas::view(Sequence), u"[]"sv) - 1) / 2;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Text = EC_Str::ExtractDelimitedPartW(pas::view(Sequence), I * 2 + 1, u"[]"sv);
            Delay = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u",-"sv)));
            First = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u",-"sv)));
            Last = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 2, u",-"sv)));
            FrameCount = pas::abs(First - Last) + 1;
            SequenceFrameCount += FrameCount;
            SequenceFrames = EC_Mem::ReAllocREC(SequenceFrames, SequenceFrameCount * static_cast<std::int32_t>(sizeof(std::int32_t)));
            FrameDelays = EC_Mem::ReAllocREC(FrameDelays, SequenceFrameCount * static_cast<std::int32_t>(sizeof(std::int32_t)));
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, FrameCount - 1); cpp_range_2.next(J); ) {
                EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(SequenceFrames, (SequenceFrameCount - FrameCount + J) * static_cast<std::int32_t>(sizeof(std::int32_t))), First);
                EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(FrameDelays, (SequenceFrameCount - FrameCount + J) * static_cast<std::int32_t>(sizeof(std::int32_t))), Delay);
                if (First < Last) {
                    ++First;
                } else {
                    --First;
                }
            }
        }
        if (!Stopped) {
            FrameTimer = MessageLoop->ScheduleCallbackTimer(GetFrameDelay(CurrentFrame), 0x00ffffff, pas::bind_method<&TGAIFileGI::AdvanceFrame>(this), 0);
        }
    }

    std::int32_t TGAIFileGI::GetSequenceFrame(std::int32_t Index) {
        return EC_Mem::ReadIntegerEC(EC_Mem::AddPointerOffset(SequenceFrames, Index * static_cast<std::int32_t>(sizeof(std::int32_t))));
    }

    std::int32_t TGAIFileGI::GetFrameDelay(std::int32_t Index) {
        return EC_Mem::ReadIntegerEC(EC_Mem::AddPointerOffset(FrameDelays, Index * static_cast<std::int32_t>(sizeof(std::int32_t))));
    }

    void TGAIFileGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        CloseImage();
        if (PlaybackBuffer != nullptr) {
            pas::free(PlaybackBuffer);
            PlaybackBuffer = nullptr;
        }
    }

    void TGAIFileGI_LoadFromConfigPath(TGAIFileGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TGAIFileGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
    }

    void TGAIFileGI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            ImageFile->SetFileName(Block->GetParam(u"Image"sv));
        }
        if (Block->CountParams(u"KindX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(pas::view(Block->GetParam(u"KindX"sv))));
        }
        if (Block->CountParams(u"KindY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(pas::view(Block->GetParam(u"KindY"sv))));
        }
        if (Block->CountParams(u"AlignX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(pas::view(Block->GetParam(u"AlignX"sv))));
        }
        if (Block->CountParams(u"AlignY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(pas::view(Block->GetParam(u"AlignY"sv))));
        }
        if (Block->CountParams(u"PBuf"_wref.get()) > 0) {
            UsePlaybackBuffer = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"PBuf"sv)));
        }
        if (Block->CountParams(u"Stop"_wref.get()) > 0) {
            Stopped = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"Stop"sv)));
        }
        if (Block->CountParams(u"Frame"_wref.get()) > 0) {
            SetFrameSequence(Block->GetParam(u"Frame"sv));
        }
        if (Block->CountParams(u"Auto"_wref.get()) > 0) {
            AutoUpdateFlags = GI_Main::ParseAutoGeometryFlagsGI(Block->GetParam(u"Auto"sv));
        }
        if (Block->CountParams(u"TransColor"_wref.get()) > 0) {
            TransparentColor = GI_Main::GetColorGI(pas::view(Block->GetParam(u"TransColor"sv)));
        }
    }

    void TGAIFileGI::UpdateAutoGeometry() {
        if ((AutoUpdateFlags & GI_Main::agfPosition) == GI_Main::agfPosition) {
            WindowsSdk::TPoint contentOrigin = GetContentOrigin();
            GI_MessageLoop::TObjectGI* parent = Parent;
            SetPosition(parent->ToLocalPoint(contentOrigin));
        }
        if ((AutoUpdateFlags & GI_Main::agfSize) == GI_Main::agfSize) {
            SetSize(GetContentSize());
        }
    }

    void TGAIFileGI::AdvanceFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        pas::critical_enter(FrameLock);
        ++CurrentFrame;
        if (CurrentFrame >= SequenceFrameCount) {
            CurrentFrame = 0;
            pas::critical_leave(FrameLock);
            if (pas::assigned(CycleCompleteCallback)) {
                CycleCompleteCallback(this);
            }
        } else {
            pas::critical_leave(FrameLock);
        }
        if (FrameTimer != nullptr) {
            MessageLoop->CancelCallbackTimer(FrameTimer);
            FrameTimer = nullptr;
        }
        if (!Stopped) {
            FrameTimer = MessageLoop->ScheduleCallbackTimer(GetFrameDelay(CurrentFrame), 0x00ffffff, pas::bind_method<&TGAIFileGI::AdvanceFrame>(this), 0);
        }
        Invalidate();
    }

    void TGAIFileGI::Draw(WindowsSdk::TRect ClipRect) {
        void* Data{};
        std::int32_t StartX{};
        std::int32_t EndX{};
        std::int32_t X{};
        std::int32_t StartY{};
        std::int32_t EndY{};
        std::int32_t Y{};
        std::int32_t Frame{};
        std::int32_t SourceFrame{};
        WindowsSdk::TRect Bounds{};
        if (CurrentFrame < 0 || CurrentFrame >= SequenceFrameCount) {
            return;
        }
        if (ImageFile->GetFileName() == u"") {
            return;
        }
        if (ImageFile->OpenDepth < 1) {
            OpenImage();
        }
        std::int32_t Width = GetContentSize().X;
        std::int32_t Height = GetContentSize().Y;
        if (ImageKindX == GI_Main::ikxLeftFill) {
            StartX = HitTestBounds.Left;
            EndX = HitTestBounds.Right;
        } else if (ImageKindX == GI_Main::ikxRightFill) {
            EndX = HitTestBounds.Right;
            StartX = EndX;
            while (StartX > ClipRect.Left) {
                StartX -= Width;
            }
        } else if (ImageKindX == GI_Main::ikxLeft) {
            StartX = HitTestBounds.Left;
            EndX = StartX + Width;
        } else if (ImageKindX == GI_Main::ikxRight) {
            EndX = HitTestBounds.Right;
            StartX = EndX - Width;
        } else if (ImageKindX == GI_Main::ikxCenter) {
            StartX = (HitTestBounds.Right - HitTestBounds.Left) / 2 + HitTestBounds.Left - Width / 2;
            EndX = StartX + Width;
        } else {
            return;
        }
        if (ImageKindY == GI_Main::ikyTopFill) {
            StartY = HitTestBounds.Top;
            EndY = HitTestBounds.Bottom;
        } else if (ImageKindY == GI_Main::ikyBottomFill) {
            EndY = HitTestBounds.Bottom;
            StartY = EndY;
            while (StartY > ClipRect.Top) {
                StartY -= Height;
            }
        } else if (ImageKindY == GI_Main::ikyTop) {
            StartY = HitTestBounds.Top;
            EndY = StartY + Height;
        } else if (ImageKindY == GI_Main::ikyBottom) {
            EndY = HitTestBounds.Bottom;
            StartY = EndY - Height;
        } else if (ImageKindY == GI_Main::ikyCenter) {
            StartY = (HitTestBounds.Bottom - HitTestBounds.Top) / 2 + HitTestBounds.Top - Height / 2;
            EndY = StartY + Height;
        } else {
            return;
        }
        Bounds = Header.Bounds;
        if (Header.Flags == 0) {
            SourceFrame = GetSequenceFrame(CurrentFrame);
            Data = GetFrameData(SourceFrame);
            FrameImage->LoadRawGiBytes(Data, GI_GAIFile::ReadGaiFrameSize(FrameDirectory, SourceFrame));
            TrimFrameCache();
            Y = StartY;
            while (Y < EndY) {
                X = StartX;
                while (X < EndX) {
                    FrameImage->DrawToGraphBuf(GR_Main::ScreenRenderBuffer, X + FrameImage->GetBoundsRect().Left - Bounds.Left, Y + FrameImage->GetBoundsRect().Top - Bounds.Top, ClipRect, 0, 255);
                    X += Width;
                }
                Y += Height;
            }
        } else if (UsePlaybackBuffer) {
            if (PlaybackBuffer == nullptr || PlaybackBuffer->Width != Width || PlaybackBuffer->Height != Height) {
                LastBufferedFrame = -1;
                if (PlaybackBuffer == nullptr) {
                    PlaybackBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                }
                PlaybackBuffer->AllocateNative(Width, Height);
            }
            if (LastBufferedFrame != CurrentFrame) {
                Frame = LastBufferedFrame + 1;
                if (Frame > CurrentFrame) {
                    Frame = 0;
                }
                if (Frame == 0) {
                    PlaybackBuffer->FillPixels16(TransparentColor);
                }
                while (Frame <= CurrentFrame) {
                    SourceFrame = GetSequenceFrame(Frame);
                    Data = GetFrameData(SourceFrame);
                    FrameImage->LoadRawGiBytes(Data, GI_GAIFile::ReadGaiFrameSize(FrameDirectory, SourceFrame));
                    FrameImage->DrawToGraphBuf(PlaybackBuffer, FrameImage->GetBoundsRect().Left - Bounds.Left, FrameImage->GetBoundsRect().Top - Bounds.Top, ClassesImports::Rect(0, 0, PlaybackBuffer->Width, PlaybackBuffer->Height), 0, 255);
                    ++Frame;
                }
                TrimFrameCache();
                LastBufferedFrame = CurrentFrame;
            }
            Y = StartY;
            while (Y < EndY) {
                X = StartX;
                while (X < EndX) {
                    {
                        GR_GraphBuf::TGraphBufGR* playbackBuffer = PlaybackBuffer;
                        std::uint16_t transparentColor = TransparentColor;
                        void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                        std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                        GR_Main::CopyTransparentGraphBuffer16Clipped(pixels, pitchBytes, X, Y, playbackBuffer, ClipRect, transparentColor);
                    }
                    X += Width;
                }
                Y += Height;
            }
        }
    }

    void TGAIFileGI::p_destroy() {
        GI_GAIFile::TGAIFileGI_Destroy(this);
    }

    void TGAIFileThreadGI::virtual_TThreadEC_Execute() {
        GI_GAIFile::TGAIFileThreadGI_Execute(this);
    }

    void TGAIFileGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_GAIFile::TGAIFileGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_GAIFile
