#include "layout/VorbisFile.hpp"
#include "types/DirectSound.hpp"
#include "types/EC_FileStream.hpp"
#include "types/MMSystemSdk.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/VorbisFile.hpp"
#include "units/Windows.hpp"
#include "units/WindowsImports.hpp"

namespace VorbisFile {
    std::uint8_t VorbisLoaded = false;

    VorbisFile::TVorbisFileStatus VorbisClear{};

    VorbisFile::TVorbisFOpen VorbisFOpen{};

    VorbisFile::TVorbisOpenCallbacks VorbisOpenCallbacks{};

    VorbisFile::TVorbisOpenCallbacks VorbisTestCallbacks{};

    VorbisFile::TVorbisFileStatus VorbisTestOpen{};

    VorbisFile::TVorbisLinkStatus VorbisBitrate{};

    VorbisFile::TVorbisFileStatus VorbisBitrateInstant{};

    VorbisFile::TVorbisFileStatus VorbisStreams{};

    VorbisFile::TVorbisFileStatus VorbisSeekable{};

    VorbisFile::TVorbisLinkStatus VorbisSerialNumber{};

    VorbisFile::TVorbisLinkCount VorbisRawTotal{};

    VorbisFile::TVorbisLinkCount VorbisPcmTotal{};

    VorbisFile::TVorbisLinkTime VorbisTimeTotal{};

    VorbisFile::TVorbisSeekOffset VorbisRawSeek{};

    VorbisFile::TVorbisSeekOffset VorbisPcmSeek{};

    VorbisFile::TVorbisSeekOffset VorbisPcmSeekPage{};

    VorbisFile::TVorbisSeekTime VorbisTimeSeek{};

    VorbisFile::TVorbisSeekTime VorbisTimeSeekPage{};

    VorbisFile::TVorbisTellOffset VorbisRawTell{};

    VorbisFile::TVorbisTellOffset VorbisPcmTell{};

    VorbisFile::TVorbisTellTime VorbisTimeTell{};

    VorbisFile::TVorbisLinkInfo VorbisInfo{};

    VorbisFile::TVorbisLinkInfo VorbisComment{};

    VorbisFile::TVorbisReadFloat VorbisReadFloat{};

    VorbisFile::TVorbisRead VorbisRead{};

    std::int32_t VorbisUseCount{};

    VorbisFile::TVorbisCallbacks VorbisCallbacks{};

    std::uint32_t VorbisLibrary{};

    void LoadVorbisLibrary() {
        GR_Main::AppendLogTextThreadSafe("Loading libvorbisfile.dll...."_a);
        VorbisLibrary = WindowsImports::LoadLibrary(pas::literal_pointer("libvorbisfile.dll"));
        if (VorbisLibrary != 0) {
            GR_Main::AppendLogLineThreadSafe("ok!"_a);
            VorbisLoaded = true;
            VorbisClear = pas::callback_from_address<TVorbisFileStatus>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_clear")));
            VorbisFOpen = pas::callback_from_address<TVorbisFOpen>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_fopen")));
            VorbisOpenCallbacks = pas::callback_from_address<TVorbisOpenCallbacks>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_open_callbacks")));
            VorbisTestCallbacks = pas::callback_from_address<TVorbisOpenCallbacks>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_test_callbacks")));
            VorbisTestOpen = pas::callback_from_address<TVorbisFileStatus>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_test_open")));
            VorbisBitrate = pas::callback_from_address<TVorbisLinkStatus>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_bitrate")));
            VorbisBitrateInstant = pas::callback_from_address<TVorbisFileStatus>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_bitrate_instant")));
            VorbisStreams = pas::callback_from_address<TVorbisFileStatus>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_streams")));
            VorbisSeekable = pas::callback_from_address<TVorbisFileStatus>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_seekable")));
            VorbisSerialNumber = pas::callback_from_address<TVorbisLinkStatus>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_serialnumber")));
            VorbisRawTotal = pas::callback_from_address<TVorbisLinkCount>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_raw_total")));
            VorbisPcmTotal = pas::callback_from_address<TVorbisLinkCount>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_pcm_total")));
            VorbisTimeTotal = pas::callback_from_address<TVorbisLinkTime>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_time_total")));
            VorbisRawSeek = pas::callback_from_address<TVorbisSeekOffset>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_raw_seek")));
            VorbisPcmSeek = pas::callback_from_address<TVorbisSeekOffset>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_pcm_seek")));
            VorbisPcmSeekPage = pas::callback_from_address<TVorbisSeekOffset>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_pcm_seek_page")));
            VorbisTimeSeek = pas::callback_from_address<TVorbisSeekTime>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_time_seek")));
            VorbisTimeSeekPage = pas::callback_from_address<TVorbisSeekTime>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_time_seek_page")));
            VorbisRawTell = pas::callback_from_address<TVorbisTellOffset>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_raw_tell")));
            VorbisPcmTell = pas::callback_from_address<TVorbisTellOffset>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_pcm_tell")));
            VorbisTimeTell = pas::callback_from_address<TVorbisTellTime>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_time_tell")));
            VorbisInfo = pas::callback_from_address<TVorbisLinkInfo>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_info")));
            VorbisComment = pas::callback_from_address<TVorbisLinkInfo>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_comment")));
            VorbisReadFloat = pas::callback_from_address<TVorbisReadFloat>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_read_float")));
            VorbisRead = pas::callback_from_address<TVorbisRead>(WindowsImports::GetProcAddress(VorbisLibrary, pas::literal_pointer("ov_read")));
        } else {
            GR_Main::AppendLogTextThreadSafe("FAIL"_a);
            GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({" GetLastError=", SysUtils::Int64ToStr(WindowsImports::GetLastError())}));
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error load=libvorbisfile.dll ", SysUtilsImports::SysErrorMessage(WindowsImports::GetLastError())})));
        }
        VorbisCallbacks.Read = TVorbisReadCallback(ReadVorbisSource);
        VorbisCallbacks.Seek = nullptr;
        VorbisCallbacks.Tell = nullptr;
        VorbisCallbacks.Close = nullptr;
    }

    // Returns bytes read, rather than fread's element count.
    std::uint32_t ReadVorbisSource(void* Buffer, std::uint32_t Size, std::uint32_t Count, void* Source) {
        EC_FileStream::TFileStreamEC* Stream = static_cast<EC_FileStream::TFileStreamEC*>(Source);
        return Stream->Read(Buffer, Size * Count);
    }

    // Always advertises stereo 44100-Hz signed 16-bit PCM; raises if ov_open_callbacks fails.
    std::int32_t PAS_STDCALL OpenVorbisStream(TOggWorker* Decoder, DirectSound::TSoundWaveFormat& Format, EC_FileStream::TFileStreamEC*& Stream) {
        Format.FormatTag = MMSystemSdk::WAVE_FORMAT_PCM;
        Format.Channels = VorbisOutputChannels;
        Format.BitsPerSample = VorbisOutputSampleBytes * 8;
        Format.ExtraSize = 0;
        Format.SamplesPerSecond = VorbisOutputSampleRate;
        Format.BlockAlign = VorbisOutputBlockAlign;
        Format.AverageBytesPerSecond = VorbisOutputBytesPerSecond;
        Decoder->Bitstream = 0;
        pas::critical_enter(pas::load_unaligned<pas::CriticalSection*>(Decoder->Lock));
        std::int32_t Result = VorbisFile::VorbisOpenCallbacks(Stream, &Decoder->VorbisState, nullptr, 0, VorbisCallbacks);
        pas::critical_leave(pas::load_unaligned<pas::CriticalSection*>(Decoder->Lock));
        if (Result != 0) {
            pas::raise(pas::make_exception<pas::Exception>("Error open audiofile."_a));
        }
        return 1;
    }

    std::int32_t PAS_STDCALL ReadVorbisSamples(TOggWorker* Decoder, void* Buffer, std::int32_t& ByteCount) {
        std::int32_t Count{};
        pas::DynArray<std::uint8_t> Temp{};
        Temp.set_length(VorbisScratchBytes);
        std::int32_t Total = 0;
        std::int32_t Remaining = ByteCount;
        std::uint8_t Done = false;
        pas::critical_enter(pas::load_unaligned<pas::CriticalSection*>(Decoder->Lock));
        while (!Done) {
            Count = VorbisFile::VorbisRead(&Decoder->VorbisState, Temp.data(), Remaining, VorbisLittleEndian, VorbisOutputSampleBytes, VorbisSignedSamples, Decoder->Bitstream);
            if (Count == OV_EBADLINK) {
                break;
            }
            if (Count == OV_HOLE) {
                break;
            }
            if (Count == OV_EINVAL) {
                break;
            }
            if (Count == 0) {
                break;
            }
            Windows::CopyMemory(EC_Mem::AddPointerOffset(Buffer, Total), &Temp[0], Count);
            Total += Count;
            Remaining -= Count;
            if (Remaining == 0) {
                break;
            }
        }
        pas::critical_leave(pas::load_unaligned<pas::CriticalSection*>(Decoder->Lock));
        ByteCount = Total;
        return Total;
    }

    void TOggWorker_Create(TOggWorker* Self, pas::CriticalSection*& SharedLock, std::uint8_t UseExternalLibrary) {
        pas::object_create(Self);
        Self->Lock = &SharedLock;
        if (!UseExternalLibrary) {
            pas::critical_enter(pas::load_unaligned<pas::CriticalSection*>(Self->Lock));
            if (!VorbisLoaded) {
                VorbisUseCount = 1;
                VorbisFile::LoadVorbisLibrary();
            } else {
                ++VorbisUseCount;
            }
            pas::critical_leave(pas::load_unaligned<pas::CriticalSection*>(Self->Lock));
        }
        Self->ExternalLibrary = UseExternalLibrary;
    }

    // Decrements the shared use count without unloading or clearing the decoder.
    void TOggWorker_Destroy(TOggWorker* Self) {
        if (!Self->ExternalLibrary) {
            pas::critical_enter(pas::load_unaligned<pas::CriticalSection*>(Self->Lock));
            --VorbisUseCount;
            // The native routine still compares the count, but has no unload body.
            static_cast<void>(VorbisUseCount == 0);
            pas::critical_leave(pas::load_unaligned<pas::CriticalSection*>(Self->Lock));
        }
        pas::object_destroy(Self);
    }

    void TOggWorker::p_destroy() {
        VorbisFile::TOggWorker_Destroy(this);
    }

} // namespace VorbisFile
