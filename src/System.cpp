#include "layout/System.hpp"
#include "units/System.hpp"

namespace System {
    std::uint32_t RandSeed{};

    std::uint8_t IsMultiThread{};

    System::TSystemThreadFuncProc SystemThreadFuncProc{};

    void* RaiseExceptionProc{};

    std::uint32_t HInstance{};

} // namespace System
