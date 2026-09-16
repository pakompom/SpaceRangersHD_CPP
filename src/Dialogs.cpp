#include "types/Windows_group.hpp"
#include "units/Dialogs.hpp"
#include "units/WindowsImports.hpp"

// The game uses ShowMessage only for diagnostic OK dialogs. Keep its blocking
// behavior with a native host dialog; the VCL form implementation is unnecessary.
namespace Dialogs {
    void ShowMessage(const pas::AnsiString& Message) {
        std::uint8_t* cpp_arg = Message.pchar();
        std::uint32_t activeWindow = WindowsImports::GetActiveWindow();
        WindowsImports::MessageBox(activeWindow, cpp_arg, pas::literal_pointer("Rangers"), WindowsSdk::MB_OK | WindowsSdk::MB_TASKMODAL);
    }

} // namespace Dialogs
