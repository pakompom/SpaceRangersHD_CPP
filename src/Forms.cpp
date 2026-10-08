#include "layout/Forms.hpp"
#include "units/Forms.hpp"
#include "units/SystemImports.hpp"
#include "units/WindowsImports.hpp"

// Game host surface, not a VCL implementation. GR_Main owns the actual Win32
// window/message loop. No recovered form/control layout is used by the game.
namespace Forms {
    Forms::TApplication* Application{};

    void TApplication::Initialize() {
        // Delphi Forms.Initialize invokes the optional RTL startup hook.
        if (SystemImports::InitProc != nullptr) {
            SystemImports::InitProc();
        }
    }

    std::uint8_t TApplication::GetActive() {
        bool cpp_condition = Handle != 0;
        if (cpp_condition) {
            std::uint32_t cpp_left = WindowsImports::GetActiveWindow();
            cpp_condition = cpp_left == Handle;
        }
        return cpp_condition;
    }

    void UnitInitialize() {
        Application = pas::make_object<TApplication>();
    }

} // namespace Forms
