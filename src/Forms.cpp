#include "layout/Forms.hpp"
#include "units/Forms.hpp"
#include "units/SystemImports.hpp"
#include "units/WindowsImports.hpp"

namespace Forms {
    Forms::TApplication* Application{};

    void TApplication::Initialize() {
        if (SystemImports::InitProc != nullptr) {
            SystemImports::InitProc();
        }
    }

    std::uint8_t TApplication::GetActive() {
        return Handle != 0 && ([&] {
            std::uint32_t cpp_left = WindowsImports::GetActiveWindow();
            return cpp_left == Handle;
        }());
    }

    void UnitInitialize() {
        Application = pas::make_object<TApplication>();
    }

    void UnitFinalize() {
        pas::free(Application);
    }

} // namespace Forms
