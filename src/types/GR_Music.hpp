#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"
#include "types/VorbisFile.hpp"

namespace EC_FileStream {
    struct TFileStreamEC;

} // namespace EC_FileStream

namespace GR_Sound {
    struct TSoundBuffer;

} // namespace GR_Sound

namespace GR_Music {
    struct TMusicUnit;

    struct TMusicControl;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMusicControl : EC_Thread::TThreadEC {
        PAS_CLASS_META(TMusicControl, EC_Thread::TThreadEC, "TMusicControl", 72)
        void p_destroy() override;
        void Clear();
        void virtual_TThreadEC_Execute() override;
        void PlayFile(const pas::WideString& FileName);
        void PlayCategory(const pas::WideString& Category);
        void RequestFadeOut();
        void StopImmediately();
        std::uint8_t HasSelectedMusic();
        std::uint8_t IsPlaying();
        std::uint32_t CompletionEvent;
        pas::CriticalSection* ControlLock;
        TMusicUnit* Current;
        TMusicUnit* Queued;
        std::uint8_t cpp_padding[4];
        pas::WideString CurrentFileName;
        pas::WideString CategoryOverride;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMusicUnit : EC_Thread::TThreadEC {
        PAS_CLASS_META(TMusicUnit, EC_Thread::TThreadEC, "TMusicUnit", 84)
        void p_destroy() override;
        void Clear();
        void LoadFile(const pas::WideString& FileName, std::uint8_t Deferred);
        pas::WideString GetFileName();
        std::uint8_t IsIntroTrack();
        void virtual_TThreadEC_Execute() override;
        VorbisFile::TOggWorker* Decoder;
        std::uint8_t BuiltinVorbis;
        std::uint8_t cpp_padding[3];
        pas::WideString RequestedFileName;
        std::uint8_t ImmediateStop;
        std::uint8_t cpp_padding_2[3];
        GR_Sound::TSoundBuffer* Buffer;
        pas::CriticalSection* DecodeLock;
        std::uint32_t DecoderLibrary;
        EC_FileStream::TFileStreamEC* Stream;
        std::uint32_t StartPlaybackEvent;
        std::uint32_t CompletionEvent;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    inline constexpr std::int32_t MusicChunkBytes = 2 * VorbisFile::VorbisOutputBytesPerSecond;

    inline constexpr std::int32_t MusicEndFadeThresholdBytes = 0x0000c800;

} // namespace GR_Music
