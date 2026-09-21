#pragma once
#include "runtime_support.hpp"
#include "types/EC_Cache.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_CacheSound {
    struct TWaveFormatEx;

    struct TWaveFileHeader;

    struct TCSoundControlEC;

    struct TCSoundEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCSoundControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCSoundControlEC, EC_Cache::TCacheControlEC, "TCSoundControlEC", 24)
        void QueueLoadIfMissing(pas::List* PendingLoads) override;
        EC_Cache::TCacheDataEC* CreateData() override;
        EC_Cache::TCacheDataEC* AcquireData() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TWaveFormatEx {
        std::uint16_t FormatTag;
        std::uint16_t Channels;
        std::uint32_t SamplesPerSecond;
        std::uint32_t AverageBytesPerSecond;
        std::uint16_t BlockAlign;
        std::uint16_t BitsPerSample;
        std::uint16_t ExtraSize;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCSoundEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCSoundEC, EC_Cache::TCacheDataEC, "TCSoundEC", 60)
        void p_destroy() override;
        // Reads 44 bytes from the current position. Forces PCM without validating RIFF, WAVE or fmt identifiers. If data is absent at header offset 36, scans the whole buffer byte by byte for it. Ignores LoadOption.
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        TWaveFormatEx Format;
        std::uint8_t cpp_padding[2];
        void* SampleData;
        std::uint32_t SampleDataSize;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TWaveFileHeader {
        // Fixed PCM header view. LoadFromConfigBuffer scans for data if it is not at $24.
        // 'RIFF'.
        std::uint32_t RiffId;
        // File size minus eight.
        std::uint32_t RiffSize;
        // 'WAVE'.
        std::uint32_t WaveId;
        // 'fmt '.
        std::uint32_t FormatId;
        std::uint32_t FormatSize;
        // Reader forces PCM without consulting this field.
        std::uint16_t FormatTag;
        std::uint16_t Channels;
        std::uint32_t SamplesPerSecond;
        // Reader recomputes this from block alignment and sample rate.
        std::uint32_t AverageBytesPerSecond;
        std::uint16_t BlockAlign;
        std::uint16_t BitsPerSample;
        std::uint32_t DataId;
        std::uint32_t DataSize;
    };
    #pragma pack(pop)

    // little-endian 'data'
    inline constexpr std::int32_t WaveDataChunkId = 0x61746164;

    inline constexpr std::int32_t WaveChunkHeaderSize = 2 * static_cast<std::int32_t>(sizeof(std::uint32_t));

} // namespace EC_CacheSound
