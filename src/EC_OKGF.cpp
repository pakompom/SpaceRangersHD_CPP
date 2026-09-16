#include "layout/EC_OKGF.hpp"
#include "units/EC_OKGF.hpp"

namespace EC_OKGF {
    // Accepts a ZL02 header followed by a zlib stream. Returns the decompressed byte count or zero on invalid input/decompression failure. Native DLL export uses four stack arguments and RET 16.
    std::int32_t PAS_STDCALL OKGF_ZLib_UnCompress2(void* Dest, std::int32_t DestCapacity, void* Source, std::int32_t SourceSize) {
        using CppImport = std::int32_t (PAS_STDCALL *)(void*, std::int32_t, void*, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("ZLib.dll", "OKGF_ZLib_UnCompress2");
        return cpp_import(Dest, DestCapacity, Source, SourceSize);
    }

} // namespace EC_OKGF
