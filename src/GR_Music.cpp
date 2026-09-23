#include "layout/GR_Music.hpp"
#include "types/DirectSound.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_File.hpp"
#include "types/GR_Sound.hpp"
#include "types/SystemImports.hpp"
#include "units/EC_FileStream.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Thread.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Music.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/VorbisFile.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace GR_Music {
    pas::WideString ChooseMusicFile(const pas::WideString& Category, const pas::WideString& CurrentFile) {
        pas::WideString Result{};
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Count{};
        std::int32_t Index{};
        std::int32_t Weight{};
        try {
            Block = GR_Main::MainDataConfig->GetBlockByPath(pas::concat_wide({u"Music.", Category}));
            Count = Block->GetParamCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                if (Block->GetParamValue(Index) != u"" && ([&] {
                    pas::WideString cpp_string = EC_Str::TrimWideString(EC_Str::LowerCaseWideString(Block->GetParamValue(Index)));
                    return cpp_string == CurrentFile;
                }())) {
                    return pas::WideString();
                }
            }
            Weight = 0;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
                if (Block->GetParamValue(Index) != u"") {
                    Weight += EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParamName(Index)));
                }
            }
            Weight = pas::random(Weight, &System::RandSeed);
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
                if (Block->GetParamValue(Index) != u"") {
                    Weight -= EC_Str::ExtractDigitsToIntW(pas::view(Block->GetParamName(Index)));
                    if (Weight < 0) {
                        Result = EC_Str::TrimWideString(EC_Str::LowerCaseWideString(Block->GetParamValue(Index)));
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Result));
                        return Result;
                    }
                }
            }
        } catch (...) {
            Result = pas::WideString();
        }
        return Result;
    }

    void TMusicUnit_Create(TMusicUnit* Self, char16_t* LibraryName) {
        EC_Thread::TThreadEC_Create(Self);
        Self->Buffer = nullptr;
        Self->DecodeLock = nullptr;
        Self->BuiltinVorbis = false;
        Self->DecodeLock = pas::make_critical_section<pas::CriticalSection>();
        if (static_cast<pas::AnsiString>(LibraryName) == "vorbisfile.dll") {
            Self->Decoder = pas::construct_call<VorbisFile::TOggWorker>(VorbisFile::TOggWorker_Create, Self->DecodeLock, false);
            Self->BuiltinVorbis = true;
            Self->Buffer = GR_Main::SoundManager->AddBuffer();
        } else {
            Self->Decoder = pas::construct_call<VorbisFile::TOggWorker>(VorbisFile::TOggWorker_Create, Self->DecodeLock, true);
            if (!Self->BuiltinVorbis) {
                GR_Main::AppendLogTextThreadSafe(pas::concat_ansi({"Load ", static_cast<pas::AnsiString>(LibraryName), " .... "}));
                try {
                    Self->DecoderLibrary = WindowsSdk::LoadLibraryW(LibraryName);
                } catch (...) {
                    GR_Main::AppendLogLineThreadSafe("fail"_a);
                    throw;
                }
                if (Self->DecoderLibrary == 0) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"fail GetLastError=", EC_Str::IntToWideString(WindowsImports::GetLastError())})));
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide_reverse({EC_Str::IntToWideString(WindowsImports::GetLastError()), static_cast<pas::WideString>(pas::concat_ansi({"Error load=", static_cast<pas::AnsiString>(LibraryName), "  GetLastError="}))}))));
                }
                GR_Main::AppendLogLineThreadSafe("ok"_a);
            }
            Self->Buffer = GR_Main::SoundManager->AddBuffer();
        }
    }

    void TMusicUnit_Destroy(TMusicUnit* Self) {
        if (Self->IsRunning()) {
            Self->RequestStop();
            if (Self->StartPlaybackEvent != 0) {
                WindowsSdk::SetEvent(Self->StartPlaybackEvent);
            }
            Self->WaitForIdle(WindowsSdk::INFINITE);
        }
        Self->Clear();
        if (Self->Buffer != nullptr) {
            GR_Main::SoundManager->RemoveBuffer(Self->Buffer);
            Self->Buffer = nullptr;
        }
        if (Self->DecodeLock != nullptr) {
            pas::free(Self->DecodeLock);
            Self->DecodeLock = nullptr;
        }
        // The native destructor leaves Decoder and DecoderLibrary allocated.
        EC_Thread::TThreadEC_Destroy(Self);
    }

    void TMusicUnit::Clear() {
        ImmediateStop = false;
        Buffer->Clear();
        if (StartPlaybackEvent != 0) {
            WindowsImports::CloseHandle(StartPlaybackEvent);
            StartPlaybackEvent = 0u;
        }
        pas::critical_enter(DecodeLock);
        if (Stream != nullptr) {
            pas::free(Stream);
            Stream = nullptr;
        }
        pas::critical_leave(DecodeLock);
    }

    void TMusicUnit::LoadFile(const pas::WideString& FileName, std::uint8_t Deferred) {
        if (([&] {
            pas::WideString cpp_string = GetFileName();
            return cpp_string != FileName;
        }())) {
            RequestedFileName = FileName;
            if (IsRunning()) {
                RequestStop();
                if (StartPlaybackEvent != 0) {
                    WindowsSdk::SetEvent(StartPlaybackEvent);
                }
                WaitForIdle(WindowsSdk::INFINITE);
            }
            Clear();
            Stream = pas::construct_call<EC_FileStream::TFileStreamEC>(EC_FileStream::TFileStreamEC_Create, 0x000400ff, FileName);
            if (Deferred) {
                SetPriority(EC_Thread::ThreadPriorityLowest);
                StartPlaybackEvent = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
                if (StartPlaybackEvent == 0) {
                    pas::raise(pas::make_exception<pas::Exception>("CreateEvent"_a));
                }
            } else {
                SetPriority(EC_Thread::ThreadPriorityAboveNormal);
            }
            Start();
        }
    }

    pas::WideString TMusicUnit::GetFileName() {
        pas::WideString Result{};
        if (!IsRunning()) {
            Result = pas::WideString();
        }
        pas::critical_enter(DecodeLock);
        if (Stream == nullptr) {
            Result = pas::WideString();
        } else {
            Result = Stream->SourceFile->GetFileName();
        }
        pas::critical_leave(DecodeLock);
        return Result;
    }

    std::uint8_t TMusicUnit::IsIntroTrack() {
        return GetFileName() == u"music\\1c.dat" || GetFileName() == u"music\\logo.dat" || GetFileName() == u"music\\intro.dat";
    }

    void TMusicUnit_Execute(TMusicUnit* Self) {
        std::uint8_t Ended{};
        DirectSound::TSoundWaveFormat Format{};
        std::int32_t Chunk{};
        if (VorbisFile::OpenVorbisStream(Self->Decoder, Format, Self->Stream) == 0) {
            Self->Clear();
            WindowsSdk::SetEvent(Self->CompletionEvent);
            return;
        }
        try {
            Self->Buffer->InitStream(MusicChunkBytes, &Format);
            if (!Self->Buffer->WriteStream(GR_Sound::SoundStreamPrimeAll, Self->Decoder)) {
                Self->Clear();
                WindowsSdk::SetEvent(Self->CompletionEvent);
                return;
            }
            if (Self->StartPlaybackEvent != 0) {
                WindowsSdk::WaitForSingleObject(Self->StartPlaybackEvent, WindowsSdk::INFINITE);
                if (Self->IsStopRequested()) {
                    Self->Clear();
                    WindowsSdk::SetEvent(Self->CompletionEvent);
                    return;
                }
                Self->SetPriority(EC_Thread::ThreadPriorityAboveNormal);
                SysUtilsImports::Sleep(100u);
                SysUtilsImports::Sleep(100u);
            }
            Ended = false;
            if (Self->IsIntroTrack()) {
                Self->Buffer->SetVolumeScale(1.0f);
            } else {
                Self->Buffer->SetVolumeScale(0.0f);
            }
            if (!Self->IsIntroTrack()) {
                Self->Buffer->StartVolumeRamp(100u, 0.1f);
            }
            Self->Buffer->Play(false);
            do {
                Chunk = Self->Buffer->WaitForChunk();
            } while (!(Chunk != 0));
            while (!Ended) {
                if (Self->ImmediateStop) {
                    break;
                }
                if (Self->IsStopRequested()) {
                    Self->SetStopRequested(false);
                    if (!Self->IsIntroTrack()) {
                        Self->Buffer->StartVolumeRamp(100u, -0.1f);
                    }
                }
                Ended = static_cast<std::uint8_t>(Self->Buffer->WriteStream(Chunk, Self->Decoder) ^ 1);
                if (Self->Stream->EndOfFile) {
                    if (Self->Stream->FillAvailable + Self->Stream->ReadAvailable <= MusicEndFadeThresholdBytes) {
                        Self->RequestStop();
                    }
                }
                Chunk = Self->Buffer->WaitForChunk();
                if (Chunk < 0) {
                    break;
                }
            }
        } catch (...) {
        }
        Self->Clear();
        WindowsSdk::SetEvent(Self->CompletionEvent);
    }

    void TMusicControl_Create(TMusicControl* Self) {
        EC_Thread::TThreadEC_Create(Self);
        Self->Current = nullptr;
        Self->Queued = nullptr;
        Self->ControlLock = pas::make_critical_section<pas::CriticalSection>();
        if (GlobalsV::MusicEnabled) {
            if (GlobalsV::MusicEnabled) {
                Self->Current = pas::construct_call<TMusicUnit>(TMusicUnit_Create, pas::literal_pointer(u"vorbisfile.dll"));
                Self->Queued = pas::construct_call<TMusicUnit>(TMusicUnit_Create, pas::literal_pointer(u"vorbisfile.dll"));
                // Native order: Current receives the still-zero handle before creation.
                Self->Current->CompletionEvent = Self->CompletionEvent;
                Self->Queued->CompletionEvent = 0u;
            }
            Self->SetPriority(EC_Thread::ThreadPriorityAboveNormal);
            Self->CompletionEvent = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
            if (Self->CompletionEvent == 0) {
                pas::raise(pas::make_exception<pas::Exception>("CreateEvent"_a));
            }
            if (GlobalsV::MusicEnabled) {
                Self->Start();
            }
            GR_Main::AppendLogLineThreadSafe("Pre-fetching music.... ok!"_a);
        }
    }

    void TMusicControl_Destroy(TMusicControl* Self) {
        Self->RequestStop();
        if (Self->CompletionEvent != 0) {
            WindowsSdk::SetEvent(Self->CompletionEvent);
        }
        if (Self->IsRunning()) {
            Self->WaitForIdle(WindowsSdk::INFINITE);
        }
        Self->Clear();
        if (Self->CompletionEvent != 0) {
            WindowsImports::CloseHandle(Self->CompletionEvent);
            Self->CompletionEvent = 0u;
        }
        if (Self->ControlLock != nullptr) {
            pas::free(Self->ControlLock);
            Self->ControlLock = nullptr;
        }
        EC_Thread::TThreadEC_Destroy(Self);
    }

    void TMusicControl::Clear() {
        if (Current != nullptr) {
            Current->RequestStop();
            WindowsSdk::SetEvent(Current->StartPlaybackEvent);
        }
        if (Queued != nullptr) {
            Queued->RequestStop();
            WindowsSdk::SetEvent(Queued->StartPlaybackEvent);
        }
        if (Current != nullptr) {
            if (Current->IsRunning()) {
                Current->WaitForIdle(WindowsSdk::INFINITE);
            }
        }
        if (Queued != nullptr) {
            if (Queued->IsRunning()) {
                Queued->WaitForIdle(WindowsSdk::INFINITE);
            }
        }
        if (Current != nullptr) {
            pas::free(Current);
            Current = nullptr;
        }
        if (Queued != nullptr) {
            pas::free(Queued);
            Queued = nullptr;
        }
    }

    void TMusicControl_Execute(TMusicControl* Self) {
        TMusicUnit* Previous{};
        while (!Self->IsStopRequested()) {
            WindowsSdk::WaitForSingleObject(Self->CompletionEvent, WindowsSdk::INFINITE);
            if (Self->IsStopRequested()) {
                break;
            }
            SysUtilsImports::Sleep(10u);
            pas::critical_enter(Self->ControlLock);
            if (Self->Queued->IsRunning()) {
                Previous = Self->Current;
                Self->Current = Self->Queued;
                Self->Queued = Previous;
                Self->CurrentFileName = Self->Current->GetFileName();
                Self->Current->CompletionEvent = Self->CompletionEvent;
                Self->Queued->CompletionEvent = 0u;
                if (Self->Current->StartPlaybackEvent != 0) {
                    WindowsSdk::SetEvent(Self->Current->StartPlaybackEvent);
                }
            }
            pas::critical_leave(Self->ControlLock);
        }
    }

    void TMusicControl::PlayFile(const pas::WideString& FileName) {
        if (!GlobalsV::MusicEnabled) {
            return;
        }
        if (FileName == u"") {
            return;
        }
        pas::critical_enter(ControlLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(ControlLock);
            };
            if (Queued->IsRunning()) {
                Queued->RequestStop();
                WindowsSdk::SetEvent(Queued->StartPlaybackEvent);
                Queued->WaitForIdle(WindowsSdk::INFINITE);
            }
            Queued->LoadFile(FileName, true);
            if (Current->IsRunning()) {
                Current->RequestStop();
            } else {
                WindowsSdk::SetEvent(CompletionEvent);
            }
        }
    }

    void TMusicControl::PlayCategory(const pas::WideString& Category) {
        std::int32_t Attempts{};
        pas::WideString Chosen{};
        if (GlobalsV::MusicEnabled) {
            pas::critical_enter(ControlLock);
            {
                pas::ScopeExit cpp_cleanup = [&]() noexcept {
                    pas::critical_leave(ControlLock);
                };
                if (CategoryOverride == u"") {
                    Chosen = ([&] {
                        const pas::WideString& fileName = Current->GetFileName();
                        const pas::WideString& category = Category;
                        return GR_Music::ChooseMusicFile(category, fileName);
                    }());
                    if (Chosen != u"" && Chosen == CurrentFileName) {
                        Chosen = GR_Music::ChooseMusicFile(u"All"_wref.get(), Current->GetFileName());
                    }
                } else {
                    Attempts = 0;
                    do {
                        Chosen = ([&] {
                            const pas::WideString& fileName_2 = Current->GetFileName();
                            const pas::WideString& categoryOverride = CategoryOverride;
                            return GR_Music::ChooseMusicFile(categoryOverride, fileName_2);
                        }());
                        ++Attempts;
                        if (Attempts > 20) {
                            break;
                        }
                    } while (!(Chosen == u"" || Chosen != CurrentFileName));
                }
                if (Chosen != u"") {
                    PlayFile(Chosen);
                }
            }
        }
    }

    void TMusicControl::RequestFadeOut() {
        if (GlobalsV::MusicEnabled && Current->IsRunning()) {
            Current->RequestStop();
        }
    }

    void TMusicControl::StopImmediately() {
        if (GlobalsV::MusicEnabled && Current->IsRunning()) {
            Current->RequestStop();
            Current->ImmediateStop = true;
        }
    }

    std::uint8_t TMusicControl::HasSelectedMusic() {
        std::uint8_t Result{};
        pas::critical_enter(ControlLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(ControlLock);
            };
            Result = Current->GetFileName() != u"" || Queued->GetFileName() != u"";
        }
        return Result;
    }

    std::uint8_t TMusicControl::IsPlaying() {
        std::uint8_t Result{};
        pas::critical_enter(ControlLock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(ControlLock);
            };
            Result = Current != nullptr && Current->Buffer != nullptr && Current->Buffer->IsPlaying() || Queued != nullptr && Queued->Buffer != nullptr && Queued->Buffer->IsPlaying();
        }
        return Result;
    }

    void TMusicUnit::p_destroy() {
        GR_Music::TMusicUnit_Destroy(this);
    }

    void TMusicControl::p_destroy() {
        GR_Music::TMusicControl_Destroy(this);
    }

    void TMusicUnit::virtual_TThreadEC_Execute() {
        GR_Music::TMusicUnit_Execute(this);
    }

    void TMusicControl::virtual_TThreadEC_Execute() {
        GR_Music::TMusicControl_Execute(this);
    }

} // namespace GR_Music
