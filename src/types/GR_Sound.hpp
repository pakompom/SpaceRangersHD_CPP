#pragma once
#include "runtime_support.hpp"
#include "types/DirectSound.hpp"
#include "types/EC_Struct.hpp"

namespace VorbisFile {
    struct TOggWorker;

} // namespace VorbisFile

namespace GR_Sound {
    struct TSoundBufferControl;

    struct TSoundBuffer;

    struct TSoundControl;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSoundBufferControl : EC_Struct::TObjectEx {
        PAS_CLASS_META(TSoundBufferControl, EC_Struct::TObjectEx, "TSoundBufferControl", 28)
        void p_destroy() override;
        void Clear();
        void Configure(const pas::WideString& Path, std::int32_t Group, std::uint8_t ALooping);
        void SetVolume(float Value);
        void SetPan(float Value);
        void Play();
        std::uint8_t IsPlaying();
        pas::WideString SoundPath;
        std::int32_t SoundGroup;
        std::uint8_t Looping;
        std::uint8_t cpp_padding[3];
        TSoundBuffer* Buffer;
        float Volume;
        float Pan;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSoundBuffer : EC_Struct::TObjectEx {
        PAS_CLASS_META(TSoundBuffer, EC_Struct::TObjectEx, "TSoundBuffer", 104)
        void p_destroy() override;
        void Clear();
        void Init(std::int32_t ByteCount, void* Format);
        void InitStream(std::int32_t ChunkBytes, void* Format);
        void ClearBuf();
        void Write(void* Data, std::uint32_t ByteCount, void* Format);
        std::uint8_t WriteStream(std::int32_t Chunk, VorbisFile::TOggWorker*& Decoder);
        void Play(std::uint8_t Looping);
        std::int32_t WaitForChunk();
        std::uint8_t IsPlaying();
        void SignalStop();
        void SetVolume(float Value);
        void SetVolumeScale(float Value);
        void StartVolumeRamp(std::uint32_t Interval, float Step);
        void SetPan(float Value);
        TSoundBuffer* Prev;
        TSoundBuffer* Next;
        std::uint8_t AutoRelease;
        std::uint8_t Streaming;
        std::uint8_t Started;
        std::uint8_t cpp_padding[1];
        DirectSound::IDirectSoundBuffer DirectBuffer;
        DirectSound::IDirectSoundNotify Notify;
        std::uint32_t StopEvent;
        pas::Array<std::uint32_t, 0, 2> ChunkEvents;
        std::uint32_t VolumeEvent;
        std::int32_t BufferBytes;
        DirectSound::TSoundWaveFormat WaveFormat;
        std::uint32_t VolumeTimer;
        float Volume;
        float VolumeScale;
        float VolumeStep;
        std::uint8_t FadingOut;
        std::uint8_t cpp_padding_2[3];
        std::int32_t SoundGroup;
        TSoundBufferControl* Controller;
        std::int32_t WriteOffset;
        std::uint32_t LastPlayCursor;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSoundControl : EC_Struct::TObjectEx {
        PAS_CLASS_META(TSoundControl, EC_Struct::TObjectEx, "TSoundControl", 48)
        void p_destroy() override;
        void Clear();
        void StopUncontrolledSounds();
        TSoundBuffer* AddBuffer();
        void RemoveBuffer(TSoundBuffer* Buffer);
        std::uint8_t SuppressGroup(std::int32_t Group, float Volume);
        void RemoveFinishedBuffers();
        void UpdateFades();
        void PlaySound(const pas::WideString& Path);
        TSoundBuffer* PlayEffect(const pas::WideString& Path, std::int32_t Group, float Volume, float Pan);
        TSoundBuffer* PlayLoop(const pas::WideString& Path, std::int32_t Group, float Volume, float Pan);
        void SignalStop();
        TSoundBuffer* FirstBuffer;
        TSoundBuffer* LastBuffer;
        DirectSound::IDirectSound DirectSound;
        DirectSound::IDirectSoundBuffer PrimaryBuffer;
        DirectSound::TSoundWaveFormat WaveFormat;
        pas::CriticalSection* Lock;
        std::uint32_t LastFadeTick;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    inline constexpr std::int32_t SoundStreamPrimeAll = -1;

    inline constexpr std::int32_t UnsignedPcmSilence = 0x00000080;

    inline constexpr std::int32_t SoundEventPollMs = 1000;

} // namespace GR_Sound
