#pragma once
#include "types/VorbisFile.hpp"

namespace DirectSound {
    struct TSoundWaveFormat;

} // namespace DirectSound

namespace EC_FileStream {
    struct TFileStreamEC;

} // namespace EC_FileStream

namespace VorbisFile {
    extern std::uint8_t VorbisLoaded;

    extern VorbisFile::TVorbisFileStatus VorbisClear;

    extern VorbisFile::TVorbisFOpen VorbisFOpen;

    extern VorbisFile::TVorbisOpenCallbacks VorbisOpenCallbacks;

    extern VorbisFile::TVorbisOpenCallbacks VorbisTestCallbacks;

    extern VorbisFile::TVorbisFileStatus VorbisTestOpen;

    extern VorbisFile::TVorbisLinkStatus VorbisBitrate;

    extern VorbisFile::TVorbisFileStatus VorbisBitrateInstant;

    extern VorbisFile::TVorbisFileStatus VorbisStreams;

    extern VorbisFile::TVorbisFileStatus VorbisSeekable;

    extern VorbisFile::TVorbisLinkStatus VorbisSerialNumber;

    extern VorbisFile::TVorbisLinkCount VorbisRawTotal;

    extern VorbisFile::TVorbisLinkCount VorbisPcmTotal;

    extern VorbisFile::TVorbisLinkTime VorbisTimeTotal;

    extern VorbisFile::TVorbisSeekOffset VorbisRawSeek;

    extern VorbisFile::TVorbisSeekOffset VorbisPcmSeek;

    extern VorbisFile::TVorbisSeekOffset VorbisPcmSeekPage;

    extern VorbisFile::TVorbisSeekTime VorbisTimeSeek;

    extern VorbisFile::TVorbisSeekTime VorbisTimeSeekPage;

    extern VorbisFile::TVorbisTellOffset VorbisRawTell;

    extern VorbisFile::TVorbisTellOffset VorbisPcmTell;

    extern VorbisFile::TVorbisTellTime VorbisTimeTell;

    extern VorbisFile::TVorbisLinkInfo VorbisInfo;

    extern VorbisFile::TVorbisLinkInfo VorbisComment;

    extern VorbisFile::TVorbisReadFloat VorbisReadFloat;

    extern VorbisFile::TVorbisRead VorbisRead;

    extern std::int32_t VorbisUseCount;

    extern VorbisFile::TVorbisCallbacks VorbisCallbacks;

    extern std::uint32_t VorbisLibrary;

    void LoadVorbisLibrary();

    std::uint32_t ReadVorbisSource(void* Buffer, std::uint32_t Size, std::uint32_t Count, void* Source);

    std::int32_t PAS_STDCALL OpenVorbisStream(TOggWorker* Decoder, DirectSound::TSoundWaveFormat& Format, EC_FileStream::TFileStreamEC*& Stream);

    std::int32_t PAS_STDCALL ReadVorbisSamples(TOggWorker* Decoder, void* Buffer, std::int32_t& ByteCount);

    void TOggWorker_Create(TOggWorker* Self, PCriticalSection SharedLock, std::uint8_t UseExternalLibrary);

    void TOggWorker_Destroy(TOggWorker* Self);

} // namespace VorbisFile
