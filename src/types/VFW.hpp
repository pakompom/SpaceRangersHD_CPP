#pragma once
#include "runtime_support.hpp"
#include "types/Types.hpp"

namespace VFW {
    struct IAVIFile_Tag;

    struct IAVIStream_Tag;

    struct TAVIStreamInfoA;

    using IAVIFile = pas::ComPtr<IAVIFile_Tag>;

    using IAVIStream = pas::ComPtr<IAVIStream_Tag>;

    #pragma pack(push, 1)
    struct TAVIStreamInfoA {
        std::uint32_t StreamType;
        std::uint32_t Handler;
        std::uint32_t Flags;
        std::uint32_t Caps;
        std::uint16_t Priority;
        std::uint16_t Language;
        std::uint32_t Scale;
        std::uint32_t Rate;
        std::uint32_t Start;
        std::uint32_t Length;
        std::uint32_t InitialFrames;
        std::uint32_t SuggestedBufferSize;
        std::uint32_t Quality;
        std::uint32_t SampleSize;
        Types::TRect Frame;
        std::uint32_t EditCount;
        std::uint32_t FormatChangeCount;
        pas::Array<std::uint8_t, 0, 63> Name;
    };
    #pragma pack(pop)

} // namespace VFW
