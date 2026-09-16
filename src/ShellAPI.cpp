#include "units/ShellAPI.hpp"

// Win32 SDK surface used by the game's help-menu action.
namespace ShellAPI {
    std::uint32_t PAS_STDCALL ShellExecuteA(std::uint32_t Window, std::uint8_t* Operation, std::uint8_t* FileName, std::uint8_t* Parameters, std::uint8_t* Directory, std::int32_t ShowCommand) {
        using CppImport = std::uint32_t (PAS_STDCALL *)(std::uint32_t, std::uint8_t*, std::uint8_t*, std::uint8_t*, std::uint8_t*, std::int32_t);
        static const auto cpp_import = pas::win::load_import<CppImport>("shell32.dll", "ShellExecuteA");
        return cpp_import(Window, Operation, FileName, Parameters, Directory, ShowCommand);
    }

} // namespace ShellAPI
