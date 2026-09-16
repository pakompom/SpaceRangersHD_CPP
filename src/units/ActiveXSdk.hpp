#pragma once
#include "runtime_support.hpp"
#include "types/Windows_group.hpp"

namespace ActiveXSdk {
    extern const pas::WideString ole32;

    Windows::HResult PAS_STDCALL CoInitialize(void* pvReserved);

    void PAS_STDCALL CoUninitialize();

    void PAS_STDCALL CoTaskMemFree(void* pv);

} // namespace ActiveXSdk
