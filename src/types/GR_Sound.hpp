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
        // Releases the active buffer and resets Volume/Pan only when Buffer is non-nil. Keeps path, group and looping mode.
        void Clear();
        // Identical paths leave every setting unchanged, including group and looping mode.
        void Configure(const pas::WideString& Path, std::int32_t Group, std::uint8_t ALooping);
        // Unchanged values do nothing. Looping sounds start lazily at nonzero volume; changing to zero clears an active loop. The controller retains the unclamped value.
        void SetVolume(float Value);
        // A changed pan can start an inactive looping sound, even at zero volume.
        void SetPan(float Value);
        // Restarts non-looping sounds; does nothing in looping mode.
        void Play();
        std::uint8_t IsPlaying();
        pas::WideString SoundPath;
        // Nonzero groups suppress quieter concurrent sounds in the same group.
        std::int32_t SoundGroup;
        std::uint8_t Looping;
        std::uint8_t cpp_padding[3];
        // Borrowed from the sound manager; the buffer holds a back-reference to this controller.
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
        // Copies 20 bytes from Format into internal wave-format storage.
        void Init(std::int32_t ByteCount, void* Format);
        // Copies 20 bytes from Format; allocates three chunks of streaming audio.
        void InitStream(std::int32_t ChunkBytes, void* Format);
        // Fills the audio buffer with silence; does not release it.
        void ClearBuf();
        // Format points to 20 bytes; data and format are copied, not retained.
        void Write(void* Data, std::uint32_t ByteCount, void* Format);
        // Chunk=-1 primes the buffer; other values refill from the current playback cursor. False indicates exhaustion or an unavailable buffer.
        std::uint8_t WriteStream(std::int32_t Chunk, VorbisFile::TOggWorker*& Decoder);
        // Streaming buffers always loop.
        void Play(std::uint8_t Looping);
        std::int32_t WaitForChunk();
        std::uint8_t IsPlaying();
        void SignalStop();
        // Stores the unclamped value and combines it with the buffer's secondary volume multiplier.
        void SetVolume(float Value);
        void SetVolumeScale(float Value);
        void StartVolumeRamp(std::uint32_t Interval, float Step);
        // Clamps the DirectSound pan to -10000..10000.
        void SetPan(float Value);
        TSoundBuffer* Prev;
        TSoundBuffer* Next;
        std::uint8_t AutoRelease;
        std::uint8_t Streaming;
        std::uint8_t Started;
        std::uint8_t cpp_padding[1];
        DirectSound::IDirectSoundBuffer DirectBuffer;
        DirectSound::IDirectSoundNotify Notify;
        // WaitForChunk passes this contiguous stop/chunk/volume event sequence to Win32.
        std::uint32_t StopEvent;
        pas::Array<std::uint32_t, 0, 2> ChunkEvents;
        std::uint32_t VolumeEvent;
        // One chunk when Streaming, otherwise the whole buffer.
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
