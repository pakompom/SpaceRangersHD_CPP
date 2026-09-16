#pragma once
#include "types/System.hpp"
#include "types/VFW.hpp"

namespace VFW {
    void PAS_STDCALL AVIFileInit();

    void PAS_STDCALL AVIFileExit();

    std::int32_t PAS_STDCALL AVIFileOpenA(IAVIFile& FileHandle, std::uint8_t* FileName, std::uint32_t Mode, void* Handler);

    std::int32_t PAS_STDCALL AVIFileGetStream(pas::ComView<IAVIFile_Tag> FileHandle, IAVIStream& Stream, std::uint32_t StreamType, std::int32_t Index);

    std::int32_t PAS_STDCALL AVIStreamInfoA(pas::ComView<IAVIStream_Tag> Stream, TAVIStreamInfoA& Info, std::int32_t Size);

    std::int32_t PAS_STDCALL AVIStreamReadFormat(pas::ComView<IAVIStream_Tag> Stream, std::int32_t Position, void* Format, std::int32_t& FormatSize);

    std::int32_t PAS_STDCALL AVIStreamRead(pas::ComView<IAVIStream_Tag> Stream, std::int32_t Start, std::int32_t Samples, void* Buffer, std::int32_t BufferSize, System::PCardinal BytesRead, System::PCardinal SamplesRead);

    std::int32_t PAS_STDCALL AVIStreamLength(pas::ComView<IAVIStream_Tag> Stream);

} // namespace VFW
