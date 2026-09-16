#pragma once
#include "runtime_support.hpp"

namespace VorbisFile {
    struct TOggWorker;

    struct TVorbisCallbacks;

    using PCriticalSection = pas::CriticalSection**;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TOggWorker : pas::Object {
        PAS_CLASS_META(TOggWorker, pas::Object, "TOggWorker", 740)
        void p_destroy() override;
        std::uint8_t cpp_padding[4];
        pas::Array<std::uint8_t, 0, 719> VorbisState;
        PCriticalSection Lock;
        std::int32_t Bitstream;
        std::uint8_t ExternalLibrary;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TVorbisFileStatus = pas::Proc<std::int32_t(void*)>;

    using TVorbisFOpen = pas::Proc<std::int32_t(std::uint8_t*, void*)>;

    using TVorbisReadCallback = pas::Proc<std::uint32_t(void*, std::uint32_t, std::uint32_t, void*)>;

    using TVorbisSeekCallback = pas::Proc<std::int32_t(void*, std::int64_t, std::int32_t)>;

    using TVorbisCloseCallback = pas::Proc<std::int32_t(void*)>;

    using TVorbisTellCallback = pas::Proc<std::int32_t(void*)>;

    #pragma pack(push, 1)
    struct TVorbisCallbacks {
        TVorbisReadCallback Read;
        TVorbisSeekCallback Seek;
        TVorbisCloseCallback Close;
        TVorbisTellCallback Tell;
    };
    #pragma pack(pop)

    using TVorbisOpenCallbacks = pas::Proc<std::int32_t(void*, void*, std::uint8_t*, std::int32_t, TVorbisCallbacks)>;

    using TVorbisLinkStatus = pas::Proc<std::int32_t(void*, std::int32_t)>;

    using TVorbisLinkCount = pas::Proc<std::int64_t(void*, std::int32_t)>;

    using TVorbisLinkTime = pas::Proc<double(void*, std::int32_t)>;

    using TVorbisSeekOffset = pas::Proc<std::int32_t(void*, std::int64_t)>;

    using TVorbisSeekTime = pas::Proc<std::int32_t(void*, double)>;

    using TVorbisTellOffset = pas::Proc<std::int64_t(void*)>;

    using TVorbisTellTime = pas::Proc<double(void*)>;

    using TVorbisLinkInfo = pas::Proc<void*(void*, std::int32_t)>;

    using TVorbisReadFloat = pas::Proc<std::int32_t(void*, void*&, std::int32_t, std::int32_t&)>;

    using TVorbisRead = pas::Proc<std::int32_t(void*, void*, std::int32_t, std::int32_t, std::int32_t, std::int32_t, std::int32_t&)>;

    inline constexpr std::int32_t VorbisOutputSampleRate = 44100;

    inline constexpr std::int32_t VorbisOutputChannels = 2;

    inline constexpr std::int32_t VorbisOutputSampleBytes = static_cast<std::int32_t>(sizeof(std::int16_t));

    inline constexpr std::int32_t VorbisOutputBlockAlign = VorbisFile::VorbisOutputChannels * VorbisFile::VorbisOutputSampleBytes;

    inline constexpr std::int32_t VorbisOutputBytesPerSecond = VorbisFile::VorbisOutputSampleRate * VorbisFile::VorbisOutputBlockAlign;

    inline constexpr std::int32_t OV_HOLE = -3;

    inline constexpr std::int32_t OV_EINVAL = -131;

    inline constexpr std::int32_t OV_EBADLINK = -137;

    inline constexpr std::int32_t VorbisLittleEndian = 0;

    inline constexpr std::int32_t VorbisSignedSamples = 1;

    inline constexpr std::int32_t VorbisScratchBytes = 4096;

} // namespace VorbisFile
