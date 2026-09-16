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

    void ClearReadOnlyAttributesRecursive(pas::WideString DirectoryPath);

    void HandleApplicationActivated();

    void HandleApplicationDeactivated();

    void HandleMessageIdle();

    void HandleMessageResume();

    void PurgeCacheDirectoryFiles();

    pas::WideString CollectInstallLanguageCodes();

    void ProgramMain();

} // namespace Rangers
