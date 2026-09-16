#include "units/MMSystem.hpp"

namespace MMSystem {
    std::uint32_t PAS_STDCALL timeBeginPeriod(std::uint32_t Period) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("winmm.dll", "timeBeginPeriod");
        return cpp_import(Period);
    }

    std::uint32_t PAS_STDCALL timeEndPeriod(std::uint32_t Period) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("winmm.dll", "timeEndPeriod");
        return cpp_import(Period);
    }

    std::uint32_t PAS_STDCALL timeGetTime() {
        using CppImport = std::uint32_t (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("winmm.dll", "timeGetTime");
        return cpp_import();
    }

    std::uint32_t PAS_STDCALL timeKillEvent(std::uint32_t TimerId) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("winmm.dll", "timeKillEvent");
        return cpp_import(TimerId);
    }

    std::uint32_t PAS_STDCALL timeSetEvent(std::uint32_t Delay, std::uint32_t Resolution, TFNTimeCallBack Callback, std::uint32_t User, std::uint32_t Flags) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, std::uint32_t, void (PAS_STDCALL *)(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t), std::uint32_t, std::uint32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("winmm.dll", "timeSetEvent");
        return cpp_import(Delay, Resolution, Callback.code, User, Flags);
    }

} // namespace MMSystem
