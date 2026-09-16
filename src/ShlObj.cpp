#include "units/ShlObj.hpp"

// Delphi 2007 Win32 shell declarations used by GetGameUserDirectory.
namespace ShlObj {
    std::int32_t PAS_STDCALL SHGetSpecialFolderLocation(std::uint32_t Window, std::int32_t Folder, PItemIDList& ItemIdList) {
        using CppImport = std::int32_t (PAS_STDCALL *)(std::uint32_t, std::int32_t, PItemIDList*);
        static const auto cpp_import = pas::win::load_import<CppImport>("shell32.dll", "SHGetSpecialFolderLocation");
        return cpp_import(Window, Folder, &ItemIdList);
    }

    std::int32_t PAS_STDCALL SHGetPathFromIDListA(PItemIDList ItemIdList, std::uint8_t* Path) {
        using CppImport = std::int32_t (PAS_STDCALL *)(PItemIDList, std::uint8_t*);
        static const auto cpp_import = pas::win::load_import<CppImport>("shell32.dll", "SHGetPathFromIDListA");
        return cpp_import(ItemIdList, Path);
    }

} // namespace ShlObj
