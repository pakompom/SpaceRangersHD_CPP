#pragma once
#include "types/Rangers.hpp"
#include "types/WStringUtils.hpp"

namespace Windows {
    struct TSystemTime;

} // namespace Windows

namespace Rangers {
    extern Rangers::TSteamCallbacksThread* SteamCallbackThread;

    extern Rangers::TAD* ApplicationEvents;

    extern Windows::TSystemTime StartupTime;

    extern std::int32_t ArgumentIndex;

    extern pas::AnsiString ExecutableFileName;

    extern std::uint8_t HadProtectedStatus;

    extern std::uint32_t WineModule;

    extern void* WineGetVersion;

    extern Rangers::TWineGetHostVersion WineGetHostVersion;

    extern void* WineNtToUnixFileName;

    extern Rangers::TWineGetBuildId WineGetBuildId;

    extern std::uint8_t* WineHostOS;

    extern std::uint8_t* WineHostVersion;

    extern WStringUtils::PStartupWideString LanguageBuffer;

    extern pas::AnsiString LanguageFileName;

    extern pas::AnsiString LanguageLine;

    extern pas::WideString StartupText;

    extern pas::TextFile LanguageFile;

    void TSteamCallbacksThread_Execute(TSteamCallbacksThread* Self);

    // Changes the process working directory and does not restore it; paths pass through the ANSI filesystem API.
    void ClearReadOnlyAttributesRecursive(pas::WideString DirectoryPath);

    void HandleApplicationActivated();

    void HandleApplicationDeactivated();

    // Checks background work, but this build performs no idle action. Assigned to GR_Main.OnMessageIdle. Removing the empty tests changes native behavior and bytes.
    void HandleMessageIdle();

    // Empty conditional callback assigned to GR_Main.OnMessageResume.
    void HandleMessageResume();

    // Nonrecursive; restores the previous working directory.
    void PurgeCacheDirectoryFiles();

    // Comma-separated lowercase names from INSTALL_*.txt in the current directory.
    pas::WideString CollectInstallLanguageCodes();

    void ProgramMain();

} // namespace Rangers
