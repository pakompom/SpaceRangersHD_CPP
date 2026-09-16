#pragma once
#include "runtime_support.hpp"
#include "types/System.hpp"

namespace DirectSound {
    struct IDirectSoundBuffer_Tag;

    struct IDirectSoundNotify_Tag;

    struct IDirectSound_Tag;

    struct TDSPositionNotify;

    struct TSoundWaveFormat;

    struct TDSBufferDesc;

    using IDirectSoundBuffer = pas::ComPtr<IDirectSoundBuffer_Tag>;

    using IDirectSoundNotify = pas::ComPtr<IDirectSoundNotify_Tag>;

    // The engine copies 20 bytes, including the aligned wave-format tail.
    #pragma pack(push, 1)
    struct TSoundWaveFormat {
        std::uint16_t FormatTag;
        std::uint16_t Channels;
        std::uint32_t SamplesPerSecond;
        std::uint32_t AverageBytesPerSecond;
        std::uint16_t BlockAlign;
        std::uint16_t BitsPerSample;
        std::uint16_t ExtraSize;
        std::uint8_t cpp_padding[2];
    };
    #pragma pack(pop)

    using IDirectSound = pas::ComPtr<IDirectSound_Tag>;

    #pragma pack(push, 1)
    struct TDSBufferDesc {
        std::uint32_t Size;
        std::uint32_t Flags;
        std::uint32_t BufferBytes;
        std::uint32_t Reserved;
        void* WaveFormat;
        System::TGUID Algorithm;
    };
    #pragma pack(pop)

    using TDSEnumCallback = pas::StdcallProc<std::int32_t(void*, std::uint8_t*, std::uint8_t*, void*)>;

    using TDirectSoundCreate = pas::StdcallProc<std::int32_t(void*, void**, void*)>;

    using TDirectSoundEnumerate = pas::StdcallProc<std::int32_t(TDSEnumCallback, void*)>;

    #pragma pack(push, 1)
    struct TDSPositionNotify {
        std::uint32_t Offset;
        std::uint32_t EventHandle;
    };
    #pragma pack(pop)

    using PDSPositionNotify = TDSPositionNotify*;

    inline constexpr std::int32_t DS_OK = 0;

    inline constexpr std::int32_t DS_NO_VIRTUALIZATION = 142082058;

    inline constexpr std::int32_t DS_INCOMPLETE = 142082068;

    inline constexpr std::int32_t DSERR_ALLOCATED = -2005401590;

    inline constexpr std::int32_t DSERR_CONTROLUNAVAIL = -2005401570;

    inline constexpr std::int32_t DSERR_INVALIDPARAM = -2147024809;

    inline constexpr std::int32_t DSERR_INVALIDCALL = -2005401550;

    inline constexpr std::int32_t DSERR_GENERIC = -2147467259;

    inline constexpr std::int32_t DSERR_PRIOLEVELNEEDED = -2005401530;

    inline constexpr std::int32_t DSERR_OUTOFMEMORY = -2147024882;

    inline constexpr std::int32_t DSERR_BADFORMAT = -2005401500;

    inline constexpr std::int32_t DSERR_UNSUPPORTED = -2147467263;

    inline constexpr std::int32_t DSERR_NODRIVER = -2005401480;

    inline constexpr std::int32_t DSERR_ALREADYINITIALIZED = -2005401470;

    inline constexpr std::int32_t DSERR_NOAGGREGATION = -2147221232;

    inline constexpr std::int32_t DSERR_BUFFERLOST = -2005401450;

    inline constexpr std::int32_t DSERR_OTHERAPPHASPRIO = -2005401440;

    inline constexpr std::int32_t DSERR_UNINITIALIZED = -2005401430;

    inline constexpr std::int32_t DSERR_NOINTERFACE = -2147467262;

    inline constexpr std::int32_t DSERR_ACCESSDENIED = -2147024891;

    inline constexpr std::int32_t DSERR_BUFFERTOOSMALL = -2005401420;

    inline constexpr std::int32_t DSERR_DS8_REQUIRED = -2005401410;

    inline constexpr std::int32_t DSERR_SENDLOOP = -2005401400;

    inline constexpr std::int32_t DSERR_BADSENDBUFFERGUID = -2005401390;

    inline constexpr std::int32_t DSERR_OBJECTNOTFOUND = -2005397151;

    inline constexpr std::int32_t DSERR_FXUNAVAILABLE = -2005401380;

    inline constexpr std::int32_t DSBCAPS_PRIMARYBUFFER = 0x00000001;

    inline constexpr std::int32_t DSBCAPS_STATIC = 0x00000002;

    inline constexpr std::int32_t DSBCAPS_LOCSOFTWARE = 0x00000008;

    inline constexpr std::int32_t DSBCAPS_CTRLPAN = 0x00000040;

    inline constexpr std::int32_t DSBCAPS_CTRLVOLUME = 0x00000080;

    inline constexpr std::int32_t DSBCAPS_CTRLPOSITIONNOTIFY = 0x00000100;

    inline constexpr std::int32_t DSBCAPS_GETCURRENTPOSITION2 = 0x00010000;

    inline constexpr std::int32_t DSBPLAY_LOOPING = 1;

    inline constexpr std::int32_t DSBLOCK_ENTIREBUFFER = 2;

    inline constexpr std::int32_t DSBSTATUS_PLAYING = 1;

    inline constexpr std::int32_t DSSCL_PRIORITY = 2;

    inline constexpr std::int32_t DSBVOLUME_MIN = -10000;

    inline constexpr std::int32_t DSBPAN_LEFT = -10000;

    inline constexpr std::int32_t DSBPAN_RIGHT = 10000;

} // namespace DirectSound
