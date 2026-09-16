#include "layout/VFW.hpp"
#include "types/System.hpp"
#include "units/VFW.hpp"

namespace VFW {
    void PAS_STDCALL AVIFileInit() {
        using CppImport = void (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIFileInit");
        return cpp_import();
    }

    void PAS_STDCALL AVIFileExit() {
        using CppImport = void (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIFileExit");
        return cpp_import();
    }

    std::int32_t PAS_STDCALL AVIFileOpenA(IAVIFile& FileHandle, std::uint8_t* FileName, std::uint32_t Mode, void* Handler) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void**, std::uint8_t*, std::uint32_t, void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIFileOpenA");
        return cpp_import(FileHandle.address(), FileName, Mode, Handler);
    }

    std::int32_t PAS_STDCALL AVIFileGetStream(pas::ComView<IAVIFile_Tag> FileHandle, IAVIStream& Stream, std::uint32_t StreamType, std::int32_t Index) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*, void**, std::uint32_t, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIFileGetStream");
        return cpp_import(pas::interface_pointer(FileHandle), Stream.address(), StreamType, Index);
    }

    std::int32_t PAS_STDCALL AVIStreamInfoA(pas::ComView<IAVIStream_Tag> Stream, TAVIStreamInfoA& Info, std::int32_t Size) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*, TAVIStreamInfoA*, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIStreamInfoA");
        return cpp_import(pas::interface_pointer(Stream), &Info, Size);
    }

    std::int32_t PAS_STDCALL AVIStreamReadFormat(pas::ComView<IAVIStream_Tag> Stream, std::int32_t Position, void* Format, std::int32_t& FormatSize) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*, std::int32_t, void*, std::int32_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIStreamReadFormat");
        return cpp_import(pas::interface_pointer(Stream), Position, Format, &FormatSize);
    }

    std::int32_t PAS_STDCALL AVIStreamRead(pas::ComView<IAVIStream_Tag> Stream, std::int32_t Start, std::int32_t Samples, void* Buffer, std::int32_t BufferSize, System::PCardinal BytesRead, System::PCardinal SamplesRead) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*, std::int32_t, std::int32_t, void*, std::int32_t, System::PCardinal, System::PCardinal);
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIStreamRead");
        return cpp_import(pas::interface_pointer(Stream), Start, Samples, Buffer, BufferSize, BytesRead, SamplesRead);
    }

    std::int32_t PAS_STDCALL AVIStreamLength(pas::ComView<IAVIStream_Tag> Stream) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("AVIFIL32.DLL", "AVIStreamLength");
        return cpp_import(pas::interface_pointer(Stream));
    }

} // namespace VFW
