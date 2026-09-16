#include "layout/DirectSound.hpp"
#include "units/DirectSound.hpp"

namespace DirectSound {
    DirectSound::TDirectSoundCreate DirectSoundCreate{};

    DirectSound::TDirectSoundEnumerate DirectSoundEnumerate{};

    std::int32_t PAS_STDCALL DirectSoundEnumerateA(TDSEnumCallback Callback, void* Context) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::int32_t (PAS_STDCALL *)(void*, std::uint8_t*, std::uint8_t*, void*), void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("dsound.dll", "DirectSoundEnumerateA");
        return cpp_import(Callback.code, Context);
    }

} // namespace DirectSound
