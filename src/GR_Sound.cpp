#include "layout/GR_Sound.hpp"
#include "types/SystemImports.hpp"
#include "units/DirectSound.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheSound.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Sound.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/VorbisFile.hpp"
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace GR_Sound {
    std::int32_t PAS_STDCALL EnumerateSoundDevice(void* Guid, std::uint8_t* Description, std::uint8_t* Module, void* Context) {
        pas::WideString Text{};
        Text = static_cast<pas::WideString>(pas::format("- %s"_a, static_cast<pas::AnsiString>(Description)));
        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(Text));
        if (EC_Str::FindTextOffsetW(Text, u"Xonar"_wref.get(), 0) >= 0) {
            GlobalsV::XonarSoundDevice = true;
        }
        return -1;
    }

    pas::AnsiString SoundErrorText(std::int32_t Code) {
        pas::AnsiString Result{};
        switch (Code) {
            case DirectSound::DS_OK: return "DS_OK"_a;
            case DirectSound::DS_NO_VIRTUALIZATION: return "DS_NO_VIRTUALIZATION"_a;
            case DirectSound::DS_INCOMPLETE: return "DS_INCOMPLETE"_a;
            case DirectSound::DSERR_ALLOCATED: return "DSERR_ALLOCATED"_a;
            case DirectSound::DSERR_CONTROLUNAVAIL: return "DSERR_CONTROLUNAVAIL"_a;
            case DirectSound::DSERR_INVALIDPARAM: return "DSERR_INVALIDPARAM"_a;
            case DirectSound::DSERR_INVALIDCALL: return "DSERR_INVALIDCALL"_a;
            case DirectSound::DSERR_GENERIC: return "DSERR_GENERIC"_a;
            case DirectSound::DSERR_PRIOLEVELNEEDED: return "DSERR_PRIOLEVELNEEDED"_a;
            case DirectSound::DSERR_OUTOFMEMORY: return "DSERR_OUTOFMEMORY"_a;
            case DirectSound::DSERR_BADFORMAT: return "DSERR_BADFORMAT"_a;
            case DirectSound::DSERR_UNSUPPORTED: return "DSERR_UNSUPPORTED"_a;
            case DirectSound::DSERR_NODRIVER: return "DSERR_NODRIVER"_a;
            case DirectSound::DSERR_ALREADYINITIALIZED: return "DSERR_ALREADYINITIALIZED"_a;
            case DirectSound::DSERR_NOAGGREGATION: return "DSERR_NOAGGREGATION"_a;
            case DirectSound::DSERR_BUFFERLOST: return "DSERR_BUFFERLOST"_a;
            case DirectSound::DSERR_OTHERAPPHASPRIO: return "DSERR_OTHERAPPHASPRIO"_a;
            case DirectSound::DSERR_UNINITIALIZED: return "DSERR_UNINITIALIZED"_a;
            case DirectSound::DSERR_NOINTERFACE: return "DSERR_NOINTERFACE"_a;
            case DirectSound::DSERR_ACCESSDENIED: return "DSERR_ACCESSDENIED"_a;
            case DirectSound::DSERR_BUFFERTOOSMALL: return "DSERR_BUFFERTOOSMALL"_a;
            case DirectSound::DSERR_DS8_REQUIRED: return "DSERR_DS8_REQUIRED"_a;
            case DirectSound::DSERR_SENDLOOP: return "DSERR_SENDLOOP"_a;
            case DirectSound::DSERR_BADSENDBUFFERGUID: return "DSERR_BADSENDBUFFERGUID"_a;
            case DirectSound::DSERR_OBJECTNOTFOUND: return "DSERR_OBJECTNOTFOUND"_a;
            case DirectSound::DSERR_FXUNAVAILABLE: return "DSERR_FXUNAVAILABLE"_a;
            default: {
                return static_cast<pas::AnsiString>(pas::concat_wide({u"unrecognized DirectSound error ", EC_Str::CardinalToHexWideString(Code)}));
            }
        }
    }

    void TSoundBufferControl_Create(TSoundBufferControl* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TSoundBufferControl_Destroy(TSoundBufferControl* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Releases the active buffer and resets Volume/Pan only when Buffer is non-nil. Keeps path, group and looping mode.
    void TSoundBufferControl::Clear() {
        if (Buffer != nullptr) {
            Buffer->Controller = nullptr;
            Buffer->Clear();
            Buffer = nullptr;
            Volume = 0.0f;
            Pan = 0.0f;
        }
    }

    // Identical paths leave every setting unchanged, including group and looping mode.
    void TSoundBufferControl::Configure(const pas::WideString& Path, std::int32_t Group, std::uint8_t ALooping) {
        if (SoundPath != Path) {
            Clear();
            SoundGroup = Group;
            SoundPath = Path;
            Looping = ALooping;
        }
    }

    // Unchanged values do nothing. Looping sounds start lazily at nonzero volume; changing to zero clears an active loop. The controller retains the unclamped value.
    void TSoundBufferControl::SetVolume(float Value) {
        if (Volume == Value) {
            return;
        }
        if (Looping) {
            if (Value == 0.0L) {
                if (Buffer != nullptr) {
                    Clear();
                }
            } else {
                Volume = Value;
                if (Buffer == nullptr) {
                    Buffer = GR_Main::SoundManager->PlayLoop(SoundPath, SoundGroup, Volume, Pan);
                    if (Buffer != nullptr) {
                        Buffer->Controller = this;
                    }
                } else {
                    Buffer->SetVolumeScale(Volume);
                }
            }
        } else {
            Volume = Value;
            if (Buffer != nullptr) {
                Buffer->SetVolumeScale(Volume);
            }
        }
    }

    // A changed pan can start an inactive looping sound, even at zero volume.
    void TSoundBufferControl::SetPan(float Value) {
        if (Pan == Value) {
            return;
        }
        Pan = Value;
        if (Looping) {
            if (Buffer == nullptr) {
                Buffer = GR_Main::SoundManager->PlayLoop(SoundPath, SoundGroup, Volume, Pan);
                if (Buffer != nullptr) {
                    Buffer->Controller = this;
                }
            } else {
                Buffer->SetPan(Pan);
            }
        } else if (Buffer != nullptr) {
            Buffer->SetPan(Pan);
        }
    }

    // Restarts non-looping sounds; does nothing in looping mode.
    void TSoundBufferControl::Play() {
        if (Looping) {
            return;
        }
        if (Buffer != nullptr) {
            Clear();
        }
        Buffer = GR_Main::SoundManager->PlayEffect(SoundPath, SoundGroup, Volume, Pan);
        if (Buffer != nullptr) {
            Buffer->Controller = this;
        }
    }

    std::uint8_t TSoundBufferControl::IsPlaying() {
        std::uint8_t Result = false;
        if (Buffer != nullptr) {
            return Buffer->IsPlaying();
        }
        return Result;
    }

    void TSoundBuffer_Create(TSoundBuffer* Self) {
        std::int32_t i{};
        EC_Struct::TObjectEx_Create(Self);
        Self->Streaming = false;
        Self->DirectBuffer = nullptr;
        Self->Notify = nullptr;
        for (i = 0; i <= 2; ++i) {
            Self->ChunkEvents[i] = 0u;
        }
        Self->StopEvent = WindowsImports::CreateEvent(nullptr, -1, 0, nullptr);
        for (i = 0; i <= 2; ++i) {
            Self->ChunkEvents[i] = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
        }
        Self->VolumeEvent = WindowsImports::CreateEvent(nullptr, 0, 0, nullptr);
        Self->BufferBytes = 0;
        Self->Volume = 1.0f;
        Self->VolumeScale = 1.0f;
        pas::fill_memory(&Self->WaveFormat, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat)), static_cast<std::uint8_t>(0));
        if (static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(&Self->VolumeEvent) - reinterpret_cast<std::uint8_t*>(&Self->StopEvent)) != static_cast<std::int32_t>(sizeof(std::uint32_t)) + static_cast<std::int32_t>(sizeof(pas::Array<std::uint32_t, 0, 2>))) {
            GR_Main::RaiseWideMessage(u"TSoundBuffer.Create 2"_wref.get());
        }
    }

    void TSoundBuffer_Destroy(TSoundBuffer* Self) {
        std::int32_t i{};
        Self->Clear();
        if (Self->StopEvent != 0) {
            WindowsImports::CloseHandle(Self->StopEvent);
            Self->StopEvent = 0u;
        }
        for (i = 0; i <= 2; ++i) {
            if (Self->ChunkEvents[i] != 0) {
                WindowsImports::CloseHandle(Self->ChunkEvents[i]);
                Self->ChunkEvents[i] = 0u;
            }
        }
        if (Self->VolumeEvent != 0) {
            WindowsImports::CloseHandle(Self->VolumeEvent);
            Self->VolumeEvent = 0u;
        }
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TSoundBuffer::Clear() {
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            if (Controller != nullptr) {
                Controller->Buffer = nullptr;
                Controller->Volume = 0.0f;
                Controller = nullptr;
            }
            if (VolumeTimer != 0) {
                MMSystem::timeKillEvent(VolumeTimer);
                VolumeTimer = 0u;
            }
            if (DirectBuffer != nullptr) {
                DirectSound::IDirectSoundBuffer_Stop(DirectBuffer);
            }
            Notify = nullptr;
            DirectBuffer = nullptr;
            BufferBytes = 0;
            Streaming = false;
            Started = false;
            pas::fill_memory(&WaveFormat, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat)), static_cast<std::uint8_t>(0));
        }
    }

    // Copies 20 bytes from Format into internal wave-format storage.
    void TSoundBuffer::Init(std::int32_t ByteCount, void* Format) {
        std::int32_t Status{};
        DirectSound::TDSBufferDesc Desc{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            Clear();
            BufferBytes = ByteCount;
            Windows::CopyMemory(&WaveFormat, Format, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat)));
            pas::fill_memory(&Desc, static_cast<std::int32_t>(sizeof(DirectSound::TDSBufferDesc)), static_cast<std::uint8_t>(0));
            Desc.Size = static_cast<std::int32_t>(sizeof(DirectSound::TDSBufferDesc));
            Desc.Flags = DirectSound::DSBCAPS_STATIC | DirectSound::DSBCAPS_LOCSOFTWARE | DirectSound::DSBCAPS_CTRLPAN | DirectSound::DSBCAPS_CTRLVOLUME;
            Desc.BufferBytes = ByteCount;
            Desc.WaveFormat = &WaveFormat;
            pas::critical_enter(GR_Main::SoundManager->Lock);
            {
                pas::ScopeExit cpp_cleanup_2 = [&]() noexcept {
                    pas::critical_leave(GR_Main::SoundManager->Lock);
                };
                Status = ([&] {
                    DirectSound::IDirectSoundBuffer& directBuffer = pas::out_interface(DirectBuffer);
                    const DirectSound::TDSBufferDesc& desc = Desc;
                    pas::ComView<DirectSound::IDirectSound_Tag> directSound = GR_Main::SoundManager->DirectSound;
                    return DirectSound::IDirectSound_CreateSoundBuffer(directSound, desc, directBuffer, nullptr);
                }());
            }
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.Init"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
        }
    }

    // Copies 20 bytes from Format; allocates three chunks of streaming audio.
    void TSoundBuffer::InitStream(std::int32_t ChunkBytes, void* Format) {
        std::int32_t Status{};
        DirectSound::PDSPositionNotify Positions{};
        DirectSound::PDSPositionNotify Position{};
        std::int32_t i{};
        DirectSound::TDSBufferDesc Desc{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            Clear();
            WriteOffset = 0;
            LastPlayCursor = 0u;
            BufferBytes = ChunkBytes;
            Windows::CopyMemory(&WaveFormat, Format, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat)));
            pas::fill_memory(&Desc, static_cast<std::int32_t>(sizeof(DirectSound::TDSBufferDesc)), static_cast<std::uint8_t>(0));
            Desc.Size = static_cast<std::int32_t>(sizeof(DirectSound::TDSBufferDesc));
            Desc.Flags = DirectSound::DSBCAPS_LOCSOFTWARE | DirectSound::DSBCAPS_CTRLPAN | DirectSound::DSBCAPS_CTRLVOLUME | DirectSound::DSBCAPS_CTRLPOSITIONNOTIFY | DirectSound::DSBCAPS_GETCURRENTPOSITION2;
            Desc.BufferBytes = ChunkBytes * 3;
            Desc.WaveFormat = &WaveFormat;
            pas::critical_enter(GR_Main::SoundManager->Lock);
            {
                pas::ScopeExit cpp_cleanup_2 = [&]() noexcept {
                    pas::critical_leave(GR_Main::SoundManager->Lock);
                };
                Status = ([&] {
                    DirectSound::IDirectSoundBuffer& directBuffer = pas::out_interface(DirectBuffer);
                    const DirectSound::TDSBufferDesc& desc = Desc;
                    pas::ComView<DirectSound::IDirectSound_Tag> directSound = GR_Main::SoundManager->DirectSound;
                    return DirectSound::IDirectSound_CreateSoundBuffer(directSound, desc, directBuffer, nullptr);
                }());
            }
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.InitStream 1"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
            Status = ([&] {
                void* notify = pas::out_interface(Notify).address();
                pas::ComView<System::IInterface_Tag> directBuffer_2 = DirectBuffer;
                return System::IInterface_QueryInterface(directBuffer_2, System::TGUID{.D1 = 0xb0210783u, .D2 = 0x000089cd, .D3 = 0x000011d0, .D4 = pas::Array<std::uint8_t, 0, 7>{{0x000000af, 0x00000008, 0x00000000, 0x000000a0, 0x000000c9, 0x00000025, 0x000000cd, 0x00000016}}}, notify);
            }());
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.InitStream 2"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
            Positions = static_cast<DirectSound::PDSPositionNotify>(EC_Mem::AllocClearEC(3 * static_cast<std::int32_t>(sizeof(DirectSound::TDSPositionNotify))));
            Position = Positions;
            for (i = 0; i <= 2; ++i) {
                Position->EventHandle = ChunkEvents[i];
                if (i == 0) {
                    Position->Offset = ChunkBytes * 3 - 1;
                } else {
                    Position->Offset = i * ChunkBytes - 1;
                }
                Position = static_cast<DirectSound::PDSPositionNotify>(EC_Mem::AddPointerOffset(Position, static_cast<std::int32_t>(sizeof(DirectSound::TDSPositionNotify))));
            }
            Status = DirectSound::IDirectSoundNotify_SetNotificationPositions(Notify, 3u, Positions);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.InitStream 3"_a);
                EC_Mem::FreeEC(Positions);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
            EC_Mem::FreeEC(Positions);
            Status = DirectSound::IDirectSoundBuffer_GetFormat(DirectBuffer, &WaveFormat, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat)), nullptr);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.InitStream 4"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
            ClearBuf();
            SetVolume(static_cast<long double>(GlobalsV::MusicVolume) * GlobalsV::MusicVolumeScale);
            SetVolumeScale(1.0f);
            Streaming = true;
        }
    }

    // Fills the audio buffer with silence; does not release it.
    void TSoundBuffer::ClearBuf() {
        void* Data{};
        std::uint32_t Bytes{};
        std::int32_t Status{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            Data = nullptr;
            Bytes = 0u;
            if (DirectBuffer == nullptr) {
                return;
            }
            Status = DirectSound::IDirectSoundBuffer_Lock(DirectBuffer, 0u, 0u, &Data, &Bytes, nullptr, nullptr, DirectSound::DSBLOCK_ENTIREBUFFER);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.ClearBuf 1"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
            if (WaveFormat.BitsPerSample == 8) {
                Windows::FillMemory(Data, Bytes, UnsignedPcmSilence);
            } else {
                pas::fill_memory(Data, static_cast<std::int32_t>(Bytes), static_cast<std::uint8_t>(0));
            }
            Status = DirectSound::IDirectSoundBuffer_Unlock(DirectBuffer, Data, Bytes, nullptr, 0u);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.ClearBuf 2"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
        }
    }

    // Format points to 20 bytes; data and format are copied, not retained.
    void TSoundBuffer::Write(void* Data, std::uint32_t ByteCount, void* Format) {
        void* Dest{};
        std::uint32_t Bytes{};
        std::int32_t Status{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            if (static_cast<std::uint32_t>(BufferBytes) < ByteCount || static_cast<std::uint8_t>(SysUtils::CompareMem(&WaveFormat, Format, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat))) ^ 1) || Streaming) {
                Init(ByteCount, Format);
            }
            if (DirectBuffer == nullptr) {
                return;
            }
            Dest = nullptr;
            Bytes = 0u;
            Status = DirectSound::IDirectSoundBuffer_Lock(DirectBuffer, 0u, ByteCount, &Dest, &Bytes, nullptr, nullptr, DirectSound::DSBLOCK_ENTIREBUFFER);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.Write 1"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
            Windows::CopyMemory(Dest, Data, ByteCount);
            Status = DirectSound::IDirectSoundBuffer_Unlock(DirectBuffer, Dest, Bytes, nullptr, 0u);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.Write 2"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
        }
    }

    // Chunk=-1 primes the buffer; other values refill from the current playback cursor. False indicates exhaustion or an unavailable buffer.
    std::uint8_t TSoundBuffer::WriteStream(std::int32_t Chunk, VorbisFile::TOggWorker*& Decoder) {
        std::int32_t Offset{};
        void* Dest{};
        std::uint32_t LockedBytes{};
        std::int32_t Status{};
        std::int32_t ReadBytes{};
        std::int32_t WantedBytes{};
        void* Temp{};
        std::uint32_t PlayCursor{};
        std::uint32_t WriteCursor{};
        std::uint32_t PlayedChunkStart{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        std::uint8_t Result = false;
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            if (!Streaming) {
                return Result;
            }
            if (DirectBuffer == nullptr) {
                return Result;
            }
            if (Chunk == SoundStreamPrimeAll) {
                Offset = 0;
                WantedBytes = BufferBytes * 3;
            } else {
                DirectSound::IDirectSoundBuffer_GetCurrentPosition(DirectBuffer, &PlayCursor, &WriteCursor);
                PlayedChunkStart = pas::idiv(PlayCursor, static_cast<std::uint32_t>(BufferBytes)) * static_cast<std::uint32_t>(BufferBytes);
                if (LastPlayCursor == PlayedChunkStart) {
                    Result = true;
                    return Result;
                }
                LastPlayCursor = PlayedChunkStart;
                if (static_cast<std::uint32_t>(WriteOffset) >= PlayedChunkStart) {
                    if (static_cast<std::uint32_t>(WriteOffset) >= WriteCursor) {
                        WantedBytes = BufferBytes * 3 - WriteOffset;
                        Offset = WriteOffset;
                        WriteOffset = 0;
                    } else {
                        Offset = pas::idiv(static_cast<std::uint32_t>(BufferBytes) + WriteCursor - 1, static_cast<std::uint32_t>(BufferBytes)) * static_cast<std::uint32_t>(BufferBytes);
                        if (BufferBytes * 3 <= Offset) {
                            Offset = 0;
                            WantedBytes = PlayedChunkStart;
                            WriteOffset = PlayedChunkStart;
                        } else {
                            WriteOffset = 0;
                            WantedBytes = BufferBytes * 3 - Offset;
                        }
                    }
                } else {
                    Offset = WriteOffset;
                    WantedBytes = PlayedChunkStart - static_cast<std::uint32_t>(Offset);
                    if (WantedBytes <= 0) {
                        Result = true;
                        return Result;
                    }
                    WriteOffset += WantedBytes;
                }
            }
            ReadBytes = WantedBytes;
            Temp = EC_Mem::AllocEC(ReadBytes);
            if (VorbisFile::ReadVorbisSamples(Decoder, Temp, ReadBytes) == 0) {
                ReadBytes = 0;
            }
            if (ReadBytes > 0) {
                Dest = nullptr;
                LockedBytes = 0u;
                Status = DirectSound::IDirectSoundBuffer_Lock(DirectBuffer, Offset, ReadBytes, &Dest, &LockedBytes, nullptr, nullptr, 0u);
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.WriteStream 1"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
                Windows::CopyMemory(Dest, Temp, ReadBytes);
                if (BufferBytes > ReadBytes) {
                    if (WaveFormat.BitsPerSample == 8) {
                        Windows::FillMemory(static_cast<std::uint8_t*>(Dest) + ReadBytes, BufferBytes - ReadBytes, UnsignedPcmSilence);
                    } else {
                        pas::fill_memory(static_cast<void*>(static_cast<std::uint8_t*>(Dest) + ReadBytes), BufferBytes - ReadBytes, static_cast<std::uint8_t>(0));
                    }
                }
                Status = DirectSound::IDirectSoundBuffer_Unlock(DirectBuffer, Dest, LockedBytes, nullptr, 0u);
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.WriteStream 2"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
            }
            EC_Mem::FreeEC(Temp);
        }
        return ReadBytes >= WantedBytes;
    }

    // Streaming buffers always loop.
    void TSoundBuffer::Play(std::uint8_t Looping) {
        std::int32_t Status{};
        std::int32_t i{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            for (i = 0; i <= 2; ++i) {
                WindowsSdk::ResetEvent(ChunkEvents[i]);
            }
            WindowsSdk::ResetEvent(VolumeEvent);
            if (DirectBuffer == nullptr) {
                return;
            }
            if (!Streaming) {
                if (Looping) {
                    Status = DirectSound::IDirectSoundBuffer_Play(DirectBuffer, 0u, 0u, DirectSound::DSBPLAY_LOOPING);
                } else {
                    Status = DirectSound::IDirectSoundBuffer_Play(DirectBuffer, 0u, 0u, 0u);
                }
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.Play 1"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
            } else {
                Status = DirectSound::IDirectSoundBuffer_Play(DirectBuffer, 0u, 0u, DirectSound::DSBPLAY_LOOPING);
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.Play 2"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
            }
            Started = true;
        }
    }

    std::int32_t TSoundBuffer::WaitForChunk() {
        std::uint32_t WaitResult{};
        std::int32_t Result = -1;
        if (!Streaming) {
            WindowsSdk::WaitForSingleObject(ChunkEvents[0], WindowsSdk::INFINITE);
            return 0;
        } else if (VolumeTimer == 0) {
            WaitResult = WindowsSdk::WaitForMultipleObjects(3 + 1, reinterpret_cast<WindowsSdk::PWOHandleArray>(&StopEvent), 0, SoundEventPollMs);
            if (WaitResult == WindowsSdk::WAIT_TIMEOUT) {
                return 0;
            }
            return WaitResult - WindowsSdk::WAIT_OBJECT_0 - 1;
        } else {
            while (true) {
                WaitResult = WindowsSdk::WaitForMultipleObjects(3 + 2, reinterpret_cast<WindowsSdk::PWOHandleArray>(&StopEvent), 0, SoundEventPollMs);
                if (WaitResult == WindowsSdk::WAIT_TIMEOUT) {
                    Result = 0;
                    break;
                }
                if (WaitResult == WindowsSdk::WAIT_OBJECT_0) {
                    Result = -1;
                    break;
                }
                if (WaitResult != WindowsSdk::WAIT_OBJECT_0 + 3 + 1) {
                    Result = WaitResult - WindowsSdk::WAIT_OBJECT_0 - 1;
                    break;
                }
                SetVolumeScale(static_cast<long double>(VolumeScale) + VolumeStep);
                if (VolumeScale == 0.0L) {
                    Result = -1;
                    break;
                }
            }
            return Result;
        }
    }

    std::uint8_t TSoundBuffer::IsPlaying() {
        std::uint32_t Flags{};
        std::int32_t Status{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        std::uint8_t Result = false;
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            if (DirectBuffer == nullptr) {
                return Result;
            }
            Status = DirectSound::IDirectSoundBuffer_GetStatus(DirectBuffer, Flags);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.IsPlaying"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
        }
        return (Flags & DirectSound::DSBSTATUS_PLAYING) == DirectSound::DSBSTATUS_PLAYING;
    }

    void TSoundBuffer::SignalStop() {
        WindowsSdk::SetEvent(StopEvent);
    }

    // Stores the unclamped value and combines it with the buffer's secondary volume multiplier.
    void TSoundBuffer::SetVolume(float Value) {
        std::int32_t Status{};
        float Minimum{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            Volume = Value;
            if (DirectBuffer == nullptr) {
                return;
            }
            Minimum = -3.0E+3f;
            if (Value == 0.0L) {
                Status = DirectSound::IDirectSoundBuffer_SetVolume(DirectBuffer, DirectSound::DSBVOLUME_MIN);
            } else {
                Status = DirectSound::IDirectSoundBuffer_SetVolume(DirectBuffer, System::Round((0.0L - Minimum) * (static_cast<long double>(Volume) * VolumeScale) + Minimum));
            }
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.SetVolume"_a);
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"volume = ", EC_Str::FloatToWideString(Value), u"*", EC_Str::FloatToWideString(VolumeScale)})));
                if (Controller != nullptr) {
                    GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Path=", Controller->SoundPath})));
                } else {
                    GR_Main::AppendLogLineThreadSafe("control missing"_a);
                }
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
        }
    }

    void TSoundBuffer::SetVolumeScale(float Value) {
        VolumeScale = Value;
        if (VolumeScale < 0.0L) {
            VolumeScale = 0.0f;
        } else if (VolumeScale > 1.0L) {
            VolumeScale = 1.0f;
        }
        SetVolume(Volume);
    }

    void TSoundBuffer::StartVolumeRamp(std::uint32_t Interval, float Step) {
        WindowsSdk::ResetEvent(VolumeEvent);
        VolumeStep = Step;
        if (VolumeTimer != 0) {
            MMSystem::timeKillEvent(VolumeTimer);
            VolumeTimer = 0u;
        }
        VolumeTimer = MMSystem::timeSetEvent(Interval, 0u, pas::callback_from_address<MMSystem::TFNTimeCallBack>(reinterpret_cast<void*>(static_cast<std::uintptr_t>(VolumeEvent))), 0u, MMSystem::TIME_PERIODIC | MMSystem::TIME_CALLBACK_EVENT_SET);
    }

    // Clamps the DirectSound pan to -10000..10000.
    void TSoundBuffer::SetPan(float Value) {
        std::int32_t Status{};
        std::int32_t Pan{};
        pas::critical_enter(GR_Main::SoundManager->Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(GR_Main::SoundManager->Lock);
            };
            if (DirectBuffer == nullptr) {
                return;
            }
            Pan = System::Round(1.0E+4L * Value);
            if (Pan < DirectSound::DSBPAN_LEFT) {
                Pan = DirectSound::DSBPAN_LEFT;
            } else if (Pan > DirectSound::DSBPAN_RIGHT) {
                Pan = DirectSound::DSBPAN_RIGHT;
            }
            Status = DirectSound::IDirectSoundBuffer_SetPan(DirectBuffer, Pan);
            if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                GR_Main::AppendLogLineThreadSafe("Error in TSoundBuffer.SetPan"_a);
                pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
            }
        }
    }

    void TSoundControl_Create(TSoundControl* Self) {
        std::int32_t Status{};
        DirectSound::TDSBufferDesc Desc{};
        EC_Struct::TObjectEx_Create(Self);
        Self->Lock = pas::make_critical_section<pas::CriticalSection>();
        Self->FirstBuffer = nullptr;
        Self->LastBuffer = nullptr;
        Self->DirectSound = nullptr;
        Self->PrimaryBuffer = nullptr;
        if (GlobalsV::SoundEnabled || GlobalsV::MusicEnabled) {
            try {
                DirectSound::DirectSoundEnumerateA(DirectSound::TDSEnumCallback(EnumerateSoundDevice), nullptr);
                Status = DirectSound::DirectSoundCreate(nullptr, pas::out_interface(Self->DirectSound).address(), pas::interface_pointer(nullptr));
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::SuppressExceptionLogCopy = true;
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundControl.Create 1"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
                Status = DirectSound::IDirectSound_SetCooperativeLevel(Self->DirectSound, GR_Main::MainWindowHandle, DirectSound::DSSCL_PRIORITY);
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::SuppressExceptionLogCopy = true;
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundControl.Create 2"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
                pas::fill_memory(&Desc, static_cast<std::int32_t>(sizeof(DirectSound::TDSBufferDesc)), static_cast<std::uint8_t>(0));
                Desc.Size = static_cast<std::int32_t>(sizeof(DirectSound::TDSBufferDesc));
                Desc.Flags = DirectSound::DSBCAPS_PRIMARYBUFFER | DirectSound::DSBCAPS_LOCSOFTWARE;
                Status = ([&] {
                    DirectSound::IDirectSoundBuffer& primaryBuffer = pas::out_interface(Self->PrimaryBuffer);
                    const DirectSound::TDSBufferDesc& desc = Desc;
                    pas::ComView<DirectSound::IDirectSound_Tag> directSound = Self->DirectSound;
                    return DirectSound::IDirectSound_CreateSoundBuffer(directSound, desc, primaryBuffer, nullptr);
                }());
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundControl.Create 3"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
                pas::fill_memory(&Self->WaveFormat, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat)), static_cast<std::uint8_t>(0));
                Self->WaveFormat.FormatTag = 1;
                Self->WaveFormat.Channels = 2;
                Self->WaveFormat.SamplesPerSecond = 44100u;
                Self->WaveFormat.BitsPerSample = 16;
                Self->WaveFormat.BlockAlign = pas::shr(static_cast<std::int32_t>(Self->WaveFormat.BitsPerSample), 3) * Self->WaveFormat.Channels;
                Self->WaveFormat.AverageBytesPerSecond = Self->WaveFormat.SamplesPerSecond * Self->WaveFormat.BlockAlign;
                Status = DirectSound::IDirectSoundBuffer_SetFormat(Self->PrimaryBuffer, &Self->WaveFormat);
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundControl.Create 4"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
                Status = DirectSound::IDirectSoundBuffer_GetFormat(Self->PrimaryBuffer, &Self->WaveFormat, static_cast<std::int32_t>(sizeof(DirectSound::TSoundWaveFormat)), nullptr);
                if (Status != DirectSound::DS_OK && Status != DirectSound::DS_INCOMPLETE) {
                    GR_Main::AppendLogLineThreadSafe("Error in TSoundControl.Create 5"_a);
                    pas::raise(pas::make_exception<pas::Exception>(GR_Sound::SoundErrorText(Status)));
                }
                if (Self->WaveFormat.FormatTag != 1) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Sound format changed to ", SysUtils::IntToStr(Self->WaveFormat.FormatTag)}));
                }
                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"Selected interface: ", EC_Str::IntToWideString(Self->WaveFormat.Channels), u" channels at ", EC_Str::IntToWideString(Self->WaveFormat.SamplesPerSecond), u" Hz, ", EC_Str::IntToWideString(Self->WaveFormat.BitsPerSample), u" bit "})));
                if (pas::shr(static_cast<std::int32_t>(Self->WaveFormat.BitsPerSample), 3) * Self->WaveFormat.Channels != Self->WaveFormat.BlockAlign) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"BlockAlign is ", SysUtils::IntToStr(Self->WaveFormat.BlockAlign)}));
                }
                if (Self->WaveFormat.SamplesPerSecond * Self->WaveFormat.BlockAlign != Self->WaveFormat.AverageBytesPerSecond) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"AvgBytesPerSec is ", SysUtils::Int64ToStr(Self->WaveFormat.AverageBytesPerSecond)}));
                }
                GR_Main::AppendLogLineThreadSafe("Initializing sound.... ok!"_a);
            } catch (...) {
                Self->PrimaryBuffer = nullptr;
                Self->DirectSound = nullptr;
                GlobalsV::SoundEnabled = false;
                GlobalsV::MusicEnabled = false;
                GR_Main::AppendLogLineThreadSafe("Initializing sound.... failed!"_a);
            }
        }
    }

    void TSoundControl_Destroy(TSoundControl* Self) {
        Self->Clear();
        Self->PrimaryBuffer = nullptr;
        Self->DirectSound = nullptr;
        pas::free(Self->Lock);
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TSoundControl::Clear() {
        while (FirstBuffer != nullptr) {
            RemoveBuffer(FirstBuffer);
        }
    }

    void TSoundControl::StopUncontrolledSounds() {
        TSoundBuffer* Buffer{};
        pas::critical_enter(Lock);
        TSoundBuffer* NextBuffer = FirstBuffer;
        while (NextBuffer != nullptr) {
            Buffer = NextBuffer;
            NextBuffer = NextBuffer->Next;
            if (Buffer->Controller == nullptr && static_cast<std::uint8_t>(Buffer->Streaming ^ 1) && Buffer->IsPlaying()) {
                RemoveBuffer(Buffer);
            }
        }
        pas::critical_leave(Lock);
    }

    TSoundBuffer* TSoundControl::AddBuffer() {
        TSoundBuffer* Buffer{};
        pas::critical_enter(Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Lock);
            };
            Buffer = pas::construct_call<TSoundBuffer>(TSoundBuffer_Create);
            if (LastBuffer != nullptr) {
                LastBuffer->Next = Buffer;
            }
            Buffer->Prev = LastBuffer;
            Buffer->Next = nullptr;
            LastBuffer = Buffer;
            if (FirstBuffer == nullptr) {
                FirstBuffer = Buffer;
            }
        }
        return Buffer;
    }

    void TSoundControl::RemoveBuffer(TSoundBuffer* Buffer) {
        pas::critical_enter(Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Lock);
            };
            if (Buffer->Prev != nullptr) {
                Buffer->Prev->Next = Buffer->Next;
            }
            if (Buffer->Next != nullptr) {
                Buffer->Next->Prev = Buffer->Prev;
            }
            if (LastBuffer == Buffer) {
                LastBuffer = Buffer->Prev;
            }
            if (FirstBuffer == Buffer) {
                FirstBuffer = Buffer->Next;
            }
            pas::free(Buffer);
        }
    }

    std::uint8_t TSoundControl::SuppressGroup(std::int32_t Group, float Volume) {
        TSoundBuffer* Buffer{};
        std::uint8_t Result = false;
        pas::critical_enter(Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Lock);
            };
            Buffer = FirstBuffer;
            while (Buffer != nullptr) {
                if (Buffer->SoundGroup == Group) {
                    if (Buffer->VolumeScale >= Volume) {
                        Result = true;
                        return Result;
                    }
                    Buffer->FadingOut = true;
                    if (Buffer->Controller != nullptr) {
                        Buffer->Controller->Buffer = nullptr;
                        Buffer->Controller->Volume = 0.0f;
                        Buffer->Controller = nullptr;
                    }
                }
                Buffer = Buffer->Next;
            }
        }
        return Result;
    }

    void TSoundControl::RemoveFinishedBuffers() {
        TSoundBuffer* NextBuffer{};
        TSoundBuffer* Buffer{};
        pas::critical_enter(Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Lock);
            };
            NextBuffer = FirstBuffer;
            while (NextBuffer != nullptr) {
                Buffer = NextBuffer;
                NextBuffer = NextBuffer->Next;
                if (Buffer->AutoRelease && static_cast<std::uint8_t>(Buffer->IsPlaying() ^ 1)) {
                    RemoveBuffer(Buffer);
                }
            }
        }
    }

    void TSoundControl::UpdateFades() {
        TSoundBuffer* NextBuffer{};
        TSoundBuffer* Buffer{};
        float NewVolume{};
        std::uint32_t Tick = MMSystem::timeGetTime();
        if (Tick - LastFadeTick < 10) {
            return;
        }
        LastFadeTick = Tick;
        pas::critical_enter(Lock);
        {
            pas::ScopeExit cpp_cleanup = [&]() noexcept {
                pas::critical_leave(Lock);
            };
            NextBuffer = FirstBuffer;
            while (NextBuffer != nullptr) {
                Buffer = NextBuffer;
                NextBuffer = NextBuffer->Next;
                if (Buffer->FadingOut) {
                    NewVolume = pas::real_max<pas::Extended>(0.0L, Buffer->VolumeScale - 0.05L);
                    if (NewVolume > 0.05L) {
                        Buffer->SetVolumeScale(NewVolume);
                    } else {
                        RemoveBuffer(Buffer);
                    }
                }
            }
        }
    }

    void TSoundControl::PlaySound(const pas::WideString& Path) {
        EC_CacheSound::TCSoundEC* Sound{};
        TSoundBuffer* Buffer{};
        if (Path == u"") {
            return;
        }
        if (!GlobalsV::SoundEnabled) {
            return;
        }
        EC_CacheSound::TCSoundControlEC* Control = nullptr;
        RemoveFinishedBuffers();
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheSound::TCSoundControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(Path);
                Sound = EC_CacheSound::AcquireCachedSound(Control);
                Buffer = AddBuffer();
                Buffer->AutoRelease = true;
                Buffer->Write(Sound->SampleData, Sound->SampleDataSize, &Sound->Format);
                Buffer->SetVolume(GlobalsV::SoundVolume);
                Buffer->SetVolumeScale(1.0f);
                Buffer->Play(false);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Control != nullptr) {
                Control->Release();
                pas::free(Control);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
    }

    TSoundBuffer* TSoundControl::PlayEffect(const pas::WideString& Path, std::int32_t Group, float Volume, float Pan) {
        EC_CacheSound::TCSoundEC* Sound{};
        TSoundBuffer* Buffer{};
        TSoundBuffer* Result = nullptr;
        if (Path == u"") {
            return Result;
        }
        if (!GlobalsV::SoundEnabled) {
            return Result;
        }
        EC_CacheSound::TCSoundControlEC* Control = nullptr;
        RemoveFinishedBuffers();
        if (Group != 0 && SuppressGroup(Group, Volume)) {
            return Result;
        }
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheSound::TCSoundControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(Path);
                Sound = EC_CacheSound::AcquireCachedSound(Control);
                Buffer = AddBuffer();
                Buffer->SoundGroup = Group;
                Buffer->AutoRelease = true;
                Buffer->Write(Sound->SampleData, Sound->SampleDataSize, &Sound->Format);
                Buffer->SetVolume(GlobalsV::SoundVolume);
                Buffer->SetVolumeScale(Volume);
                Buffer->SetPan(Pan);
                Buffer->Play(false);
                Result = Buffer;
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Control != nullptr) {
                Control->Release();
                pas::free(Control);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        return Result;
    }

    TSoundBuffer* TSoundControl::PlayLoop(const pas::WideString& Path, std::int32_t Group, float Volume, float Pan) {
        EC_CacheSound::TCSoundEC* Sound{};
        TSoundBuffer* Buffer{};
        TSoundBuffer* Result = nullptr;
        if (Path == u"") {
            return Result;
        }
        if (!GlobalsV::SoundEnabled) {
            return Result;
        }
        EC_CacheSound::TCSoundControlEC* Control = nullptr;
        RemoveFinishedBuffers();
        if (Group != 0 && SuppressGroup(Group, Volume)) {
            return Result;
        }
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheSound::TCSoundControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(Path);
                Sound = EC_CacheSound::AcquireCachedSound(Control);
                Buffer = AddBuffer();
                Buffer->SoundGroup = Group;
                Buffer->AutoRelease = true;
                Buffer->Write(Sound->SampleData, Sound->SampleDataSize, &Sound->Format);
                Buffer->SetVolume(GlobalsV::SoundVolume);
                Buffer->SetVolumeScale(Volume);
                Buffer->SetPan(Pan);
                Buffer->Play(true);
                Result = Buffer;
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Control != nullptr) {
                Control->Release();
                pas::free(Control);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        return Result;
    }

    void TSoundControl::SignalStop() {
        pas::critical_enter(Lock);
        TSoundBuffer* Buffer = FirstBuffer;
        while (Buffer != nullptr) {
            Buffer->SignalStop();
            Buffer = Buffer->Next;
        }
        pas::critical_leave(Lock);
    }

    void TSoundBufferControl::p_destroy() {
        GR_Sound::TSoundBufferControl_Destroy(this);
    }

    void TSoundBuffer::p_destroy() {
        GR_Sound::TSoundBuffer_Destroy(this);
    }

    void TSoundControl::p_destroy() {
        GR_Sound::TSoundControl_Destroy(this);
    }

} // namespace GR_Sound
