#include "types/Windows_group.hpp"
#include "units/ActiveXSdk.hpp"

namespace ActiveXSdk {
    const pas::WideString ole32 = u"ole32.dll"_w;

    Windows::HResult PAS_STDCALL CoInitialize(void* pvReserved) {
        using CppImport = Windows::HResult (PAS_STDCALL *)(void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("ole32.dll", "CoInitialize");
        return cpp_import(pvReserved);
    }

    void PAS_STDCALL CoUninitialize() {
        using CppImport = void (PAS_STDCALL *)();
        static const auto cpp_import = pas::win::load_import<CppImport>("ole32.dll", "CoUninitialize");
        return cpp_import();
    }

    void PAS_STDCALL CoTaskMemFree(void* pv) {
        using CppImport = void (PAS_STDCALL *)(void*);
        static const auto cpp_import = pas::win::load_import<CppImport>("ole32.dll", "CoTaskMemFree");
        return cpp_import(pv);
    }

} // namespace ActiveXSdk
