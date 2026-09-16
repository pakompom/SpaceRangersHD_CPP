#include "layout/Rangers.hpp"
#include "types/Classes.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GR_Sound.hpp"
#include "types/fGameLoad.hpp"
#include "types/fGameSettings.hpp"
#include "units/Achievements.hpp"
#include "units/CheatCode.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/ExceptionInfo.hpp"
#include "units/Forms.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/NoSteamAchievemens.hpp"
#include "units/Rangers.hpp"
#include "units/Robot.hpp"
#include "units/SE_SoundRnd.hpp"
#include "units/SimpleSteamApi.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/SystemImports.hpp"
#include "units/ThreadCalc.hpp"
#include "units/WStringUtils.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aGalaxy.hpp"
#include "units/aModsInfo.hpp"
#include "units/aPacket.hpp"
#include "units/aSaveLoad.hpp"

namespace Rangers {
    Rangers::TSteamCallbacksThread* SteamCallbackThread = nullptr;

    Rangers::TAD* ApplicationEvents{};

    Windows::TSystemTime StartupTime{};

    std::int32_t ArgumentIndex{};

    pas::AnsiString ExecutableFileName{};

    std::uint8_t HadProtectedStatus{};

    std::uint32_t WineModule{};

    void* WineGetVersion{};

    Rangers::TWineGetHostVersion WineGetHostVersion{};

    void* WineNtToUnixFileName{};

    Rangers::TWineGetBuildId WineGetBuildId{};

    std::uint8_t* WineHostOS{};

    std::uint8_t* WineHostVersion{};

    WStringUtils::PStartupWideString LanguageBuffer{};

    pas::AnsiString LanguageFileName{};

    pas::AnsiString LanguageLine{};

    pas::WideString StartupText{};

    pas::TextFile LanguageFile{};

    void TSteamCallbacksThread_Execute(TSteamCallbacksThread* Self) {
        std::uint32_t CurrentTick{};
        std::uint32_t LastCallbackTick = MMSystem::timeGetTime();
        while (!Self->IsStopRequested()) {
            CurrentTick = MMSystem::timeGetTime();
            if (CurrentTick - LastCallbackTick > 200) {
                SimpleSteamApi::SteamRunCallbacks();
                LastCallbackTick = CurrentTick;
            }
            SysUtilsImports::Sleep(100u);
        }
    }

    // Changes the process working directory and does not restore it; paths pass through the ANSI filesystem API.
    void ClearReadOnlyAttributesRecursive(pas::WideString DirectoryPath) {
        Windows::TWin32FindDataA FindData{};
        SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(DirectoryPath));
        WindowsImports::THandle SearchHandle = WindowsSdk::FindFirstFile(pas::literal_pointer("*.*"), FindData);
        if (SearchHandle != WindowsImports::INVALID_HANDLE_VALUE) {
            do {
                if ((FindData.dwFileAttributes & WindowsSdk::FILE_ATTRIBUTE_READONLY) != 0) {
                    static_cast<void>(WindowsSdk::SetFileAttributesA(FindData.cFileName.elements, WindowsImports::FILE_ATTRIBUTE_NORMAL));
                }
            } while (WindowsSdk::FindNextFile(SearchHandle, FindData));
            WindowsImports::FindClose(SearchHandle);
        }
        SearchHandle = WindowsSdk::FindFirstFile(pas::literal_pointer("*.*"), FindData);
        if (SearchHandle != WindowsImports::INVALID_HANDLE_VALUE) {
            do {
                if ((FindData.dwFileAttributes & WindowsImports::FILE_ATTRIBUTE_DIRECTORY) != 0) {
                    if (pas::array_text<pas::AnsiString>(FindData.cFileName.elements, 260) != "." && pas::array_text<pas::AnsiString>(FindData.cFileName.elements, 260) != "..") {
                        Rangers::ClearReadOnlyAttributesRecursive(pas::concat_wide({DirectoryPath, u"\\", static_cast<pas::WideString>(pas::array_text<pas::AnsiString>(FindData.cFileName.elements, 260))}));
                    }
                }
            } while (WindowsSdk::FindNextFile(SearchHandle, FindData));
            WindowsImports::FindClose(SearchHandle);
        }
    }

    void HandleApplicationActivated() {
        GR_Sound::TSoundBuffer* Buffer{};
        GR_Main::RuntimeActive = true;
        if (GR_Main::SoundManager != nullptr) {
            Buffer = GR_Main::SoundManager->FirstBuffer;
            while (Buffer != nullptr) {
                if (Buffer->Streaming) {
                    Buffer->SetVolume(static_cast<long double>(GlobalsV::MusicVolume) * GlobalsV::MusicVolumeScale);
                } else {
                    Buffer->SetVolume(GlobalsV::SoundVolume);
                }
                Buffer = Buffer->Next;
            }
        }
        if (GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId] != nullptr) {
            GR_Main::FullFrameRedrawRequested = true;
            pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->InvalidateViewport();
        }
        if (GlobalsV::MemorySnapshotActive) {
            aSaveLoad::RestoreGameFromMemorySnapshot();
        }
    }

    void TAD::ApplicationActivated(pas::Object* Sender) {
        Rangers::HandleApplicationActivated();
    }

    void HandleApplicationDeactivated() {
        if (GR_Main::WindowedModeRequested) {
            return;
        }
        GR_Main::RuntimeActive = false;
        return;
    }

    void TAD::ApplicationDeactivated(pas::Object* Sender) {
        Rangers::HandleApplicationDeactivated();
    }

    // Checks background work, but this build performs no idle action. Assigned to GR_Main.OnMessageIdle. Removing the empty tests changes native behavior and bytes.
    void HandleMessageIdle() {
        if (GlobalsV::MemorySnapshotActive) {
            return;
        }
        if (aGalaxy::Galaxy == nullptr) {
            return;
        }
        if (Globals::NewGameGenerationThread != nullptr && Globals::NewGameGenerationThread->IsRunning()) {
            return;
        }
        if (Globals::GameLoadScreen != nullptr && Globals::GameLoadScreen->IsLoading()) {
            return;
        }
        if (ThreadCalc::IsTurnCalculationRunning()) {
            return;
        }
    }

    // Empty conditional callback assigned to GR_Main.OnMessageResume.
    void HandleMessageResume() {
        if (GlobalsV::MemorySnapshotActive) {
            return;
        }
        if (aGalaxy::Galaxy == nullptr) {
            return;
        }
    }

    // Nonrecursive; restores the previous working directory.
    void PurgeCacheDirectoryFiles() {
        pas::AnsiString FileName{};
        pas::AnsiString OldDirectory{};
        SysUtils::TSearchRec Search{};
        OldDirectory = SysUtilsImports::GetCurrentDir();
        SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Cache\\"})));
        if (SysUtilsImports::FindFirst("*.*"_a, SysUtilsImports::faAnyFile, Search) == 0) {
            do {
                FileName = Search.Name;
                if (FileName != "." && FileName != "..") {
                    SysUtilsImports::DeleteFile(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Cache\\", static_cast<pas::WideString>(FileName)})));
                }
            } while (!(SysUtilsImports::FindNext(Search) != 0));
            SysUtilsImports::FindClose(Search);
        }
        SysUtilsImports::SetCurrentDir(OldDirectory);
    }

    // Comma-separated lowercase names from INSTALL_*.txt in the current directory.
    pas::WideString CollectInstallLanguageCodes() {
        pas::WideString Result{};
        pas::WideString FileName{};
        pas::WideString LanguageCode{};
        pas::AnsiString LowerCode{};
        std::int32_t NameLength{};
        SysUtils::TSearchRec Search{};
        if (SysUtilsImports::FindFirst("INSTALL_*.txt"_a, SysUtilsImports::faAnyFile, Search) == 0) {
            do {
                FileName = static_cast<pas::WideString>(Search.Name);
                NameLength = FileName.length();
                LanguageCode = pas::copy(FileName, 9, NameLength - 12);
                LowerCode = SysUtilsImports::AnsiLowerCase(static_cast<pas::AnsiString>(LanguageCode));
                if (Result == u"") {
                    Result = static_cast<pas::WideString>(LowerCode);
                } else {
                    Result = pas::concat_wide({Result, u",", static_cast<pas::WideString>(LowerCode)});
                }
            } while (!(SysUtilsImports::FindNext(Search) != 0));
            SysUtilsImports::FindClose(Search);
        }
        return Result;
    }

    void ProgramMain() {
        Forms::UnitInitialize();
        {
            try {
                // Native initializer saves and replaces the RTL raise hook.
                ExceptionInfo::UnitInitialize();
                // Compiler unit entry registers the native command order.
                CheatCode::UnitInitialize();
                // Compiler unit entry calls the virtual destructor directly.
                {
                    try {
                        SysUtils::DecimalSeparator = '.';
                        GR_Main::MainRuntimeThreadId = WindowsImports::GetCurrentThreadId();
                        GR_Main::CCInterface = pas::construct_call<GR_Main::TCCInterface>(GR_Main::TCCInterface_Create);
                        GR_Main::RuntimeExitCheckCallback1 = pas::Proc<void()>(Globals::HandleRuntimeExitCheck1);
                        GR_Main::RuntimeExitCheckCallback2 = pas::Proc<void()>(Globals::HandleRuntimeExitCheck2);
                        GR_Main::DebugKeyCallback = GR_Main::TDebugKeyCallbackGR(CheatCode::HandleDebugKey);
                        GR_Main::RuntimeStartupTick = MMSystem::timeGetTime();
                        GR_Main::OnMessageIdle = GR_Main::TRuntimeCallbackGR(HandleMessageIdle);
                        GR_Main::OnMessageResume = GR_Main::TRuntimeCallbackGR(HandleMessageResume);
                        Forms::TApplication::Initialize();
                        ApplicationEvents = pas::make_object<TAD>();
                        Forms::Application->OnActivate = pas::bind_static_method<&TAD::ApplicationActivated>(ApplicationEvents);
                        Forms::Application->OnDeactivate = pas::bind_static_method<&TAD::ApplicationDeactivated>(ApplicationEvents);
                        if (WindowsImports::OpenEvent(WindowsImports::EVENT_MODIFY_STATE, 0, pas::literal_pointer("EG_SpaceRangers_Run")) != 0) {
                            WindowsImports::MessageBox(0u, pas::literal_pointer("Please terminate already running instance of the game!"), pas::literal_pointer("Space Rangers"), WindowsImports::MB_ICONERROR);
                        } else {
                            WindowsImports::CreateEvent(nullptr, -1, -1, pas::literal_pointer("EG_SpaceRangers_Run"));
                            for (auto cpp_range = pas::for_to<std::int32_t>(1, SystemImports::ParamCount()); cpp_range.next(ArgumentIndex); ) {
                                if (SysUtilsImports::LowerCase(SystemImports::ParamStr(ArgumentIndex)) == "savemergedcfg") {
                                    GlobalsV::DumpLoadedConfig = true;
                                }
                            }
                            ExecutableFileName.set_length(WindowsImports::MAX_PATH);
                            if (WindowsImports::GetModuleFileNameA(0u, ExecutableFileName.pchar(), WindowsImports::MAX_PATH) != 0) {
                                ExecutableFileName.set_length(static_cast<std::int32_t>(SysUtilsImports::StrLen(ExecutableFileName.pchar())));
                                Rangers::ClearReadOnlyAttributesRecursive(EC_Str::ExtractFileDirW(static_cast<pas::WideString>(ExecutableFileName)));
                                SysUtilsImports::SetCurrentDir(static_cast<pas::AnsiString>(EC_Str::ExtractFileDirW(static_cast<pas::WideString>(ExecutableFileName))));
                            }
                            EC_Str::WriteRegistryStringLegacy(WindowsImports::HKEY_LOCAL_MACHINE, u"SOFTWARE\\CLASSES\\avifile\\Extensions\\VDO"_w, pas::WideString(), u"{00020000-0000-0000-C000-000000000046}"_w);
                            SystemImports::Randomize();
                            do {
                                try {
                                    try {
                                        GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
                                        GR_Main::CreateStartupLogFile();
                                        GR_Main::SelectedLanguage = pas::WideString();
                                        GR_Main::AvailableLanguageCodes = pas::WideString();
                                        GR_Main::RequestedLanguage = pas::WideString();
                                        Achievements::InitializeAchievementDefinitions();
                                        if (!GR_Main::SkipModsOnReload) {
                                            GR_Main::InitializePlatformRuntimeAndMainWindow();
                                        }
                                        SimpleSteamApi::SteamInitialized = false;
                                        if (GR_Main::InstallConfig->CountParamsByPath(u"GameDistributor"_wref.get()) > 0) {
                                            StartupText = static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(GR_Main::InstallConfig->GetParamByPathOrMarker(u"GameDistributor"_wref.get()))));
                                            if (StartupText == u"steam") {
                                                GR_Main::AppendLogLineThreadSafe("GameDistributor=Steam"_a);
                                                SimpleSteamApi::LoadSteamApi();
                                                GR_Main::SelectedLanguage.set_length(255);
                                                GR_Main::AvailableLanguageCodes.set_length(255);
                                                SimpleSteamApi::SteamInitialized = SimpleSteamApi::SteamInit(GR_Main::SelectedLanguage, GR_Main::AvailableLanguageCodes);
                                                if (!SimpleSteamApi::SteamInitialized) {
                                                    GR_Main::AppendLogLineThreadSafe("Steam not initialized"_a);
                                                }
                                            } else if (StartupText == u"gog") {
                                                GR_Main::AppendLogLineThreadSafe("GameDistributor=GOG"_a);
                                            } else {
                                                GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"GameDistributor=Unknown(", GR_Main::InstallConfig->GetParamByPathOrMarker(u"GameDistributor"_wref.get()), u")"})));
                                            }
                                        } else {
                                            GR_Main::AppendLogLineThreadSafe("GameDistributor=None"_a);
                                        }
                                        if (SimpleSteamApi::SteamInitialized) {
                                            LanguageBuffer = &GR_Main::SelectedLanguage;
                                            WStringUtils::TruncateStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&LanguageBuffer));
                                            LanguageBuffer = &GR_Main::AvailableLanguageCodes;
                                            WStringUtils::TruncateStartupWideString(pas::Var<WStringUtils::PStartupWideString>(&LanguageBuffer));
                                        } else {
                                            GR_Main::SelectedLanguage = u"english"_w;
                                            GR_Main::AvailableLanguageCodes = Rangers::CollectInstallLanguageCodes();
                                            LanguageFileName = static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"Lang.txt"}));
                                            if (SysUtilsImports::FileExists(LanguageFileName)) {
                                                pas::text_assign(LanguageFile, LanguageFileName, false);
                                                pas::text_open(LanguageFile, 2, false);
                                                while (!pas::text_eof(LanguageFile, false)) {
                                                    pas::text_readln(LanguageFile, LanguageLine, false);
                                                    StartupText = static_cast<pas::WideString>(LanguageLine);
                                                    if (EC_Str::CountDelimitedPartsW(StartupText, u"="_wref.get()) > 1) {
                                                        if (EC_Str::ExtractDelimitedPartW(StartupText, 0, u"="_wref.get()) == u"Lang") {
                                                            GR_Main::SelectedLanguage = EC_Str::TrimWideString(EC_Str::ExtractDelimitedPartW(StartupText, 1, u"="_wref.get()));
                                                        }
                                                    }
                                                }
                                                pas::text_close(LanguageFile, false);
                                            }
                                        }
                                        GR_Main::LoadLanguageAndPackages();
                                        if (SimpleSteamApi::SteamInitialized) {
                                            SimpleSteamApi::SteamSetLeaderboardName("Scores"_a);
                                            SimpleSteamApi::InitializeSteamAchievements();
                                            SimpleSteamApi::SteamInitialized = SimpleSteamApi::SteamLocal(214730);
                                        }
                                        Globals::InitializeScriptHostRuntime();
                                        NoSteamAchievemens::LoadLocalAchievements();
                                        GlobalsV::RunningUnderWine = false;
                                        WineModule = WindowsImports::LoadLibrary(pas::literal_pointer("ntdll.dll"));
                                        if (WineModule > 32) {
                                            WineGetVersion = WindowsImports::GetProcAddress(WineModule, pas::literal_pointer("wine_get_version"));
                                            WineGetHostVersion = pas::callback_from_address<TWineGetHostVersion>(WindowsImports::GetProcAddress(WineModule, pas::literal_pointer("wine_get_host_version")));
                                            WineNtToUnixFileName = WindowsImports::GetProcAddress(WineModule, pas::literal_pointer("wine_nt_to_unix_file_name"));
                                            WineGetBuildId = pas::callback_from_address<TWineGetBuildId>(WindowsImports::GetProcAddress(WineModule, pas::literal_pointer("wine_get_build_id")));
                                            GlobalsV::RunningUnderWine = WineGetVersion != nullptr || WineGetHostVersion != nullptr || WineNtToUnixFileName != nullptr || WineGetBuildId != nullptr;
                                            if (GlobalsV::RunningUnderWine) {
                                                GR_Main::AppendLogLineThreadSafe("----------------------------------"_a);
                                                GR_Main::AppendLogLineThreadSafe("NOTICE: Game is launched under Wine or Proton!"_a);
                                                GR_Main::AppendLogLineThreadSafe("NOTICE: Game may work as it does on Windows - or be funky, bug out and crash."_a);
                                                GR_Main::AppendLogLineThreadSafe("NOTICE: Please refer to https://www.protondb.com/app/214730 if you having any issues"_a);
                                                GR_Main::AppendLogLineThreadSafe("NOTICE: MacOS users may try to ask for help on https://www.reddit.com/r/macgaming/ or https://www.reddit.com/r/wine_gaming/"_a);
                                                GR_Main::AppendLogLineThreadSafe("----------------------------------"_a);
                                                if (WineGetHostVersion != nullptr) {
                                                    Rangers::WineGetHostVersion(WineHostOS, WineHostVersion);
                                                    GR_Main::AppendLogTextThreadSafe("Host OS="_a);
                                                    // Native startup reports Darwin as Linux; retain that behavior.
                                                    if (static_cast<pas::AnsiString>(WineHostOS) == "Darwin") {
                                                        GR_Main::AppendLogTextThreadSafe("Linux"_a);
                                                    } else {
                                                        GR_Main::AppendLogTextThreadSafe(static_cast<pas::AnsiString>(WineHostOS));
                                                    }
                                                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({" ", static_cast<pas::AnsiString>(static_cast<pas::ShortString>(WineHostVersion))}));
                                                } else {
                                                    GR_Main::AppendLogLineThreadSafe("Can't detect host OS, wine_get_host_version() not found"_a);
                                                }
                                                if (WineGetBuildId != nullptr) {
                                                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Wine=", static_cast<pas::AnsiString>(Rangers::WineGetBuildId())}));
                                                } else {
                                                    GR_Main::AppendLogLineThreadSafe("Can't detect Wine build version, wine_get_build_id() not found"_a);
                                                }
                                            }
                                            WindowsImports::FreeLibrary(WineModule);
                                        }
                                        HadProtectedStatus = false;
                                        WindowsImports::GetSystemTime(StartupTime);
                                        GR_Main::AppendOptionalDebugLogLine(pas::format("=== Start %d-%.2d-%.2d %.2d.%.2d.%.2d.%.3d"_a, static_cast<std::int32_t>(StartupTime.wYear), static_cast<std::int32_t>(StartupTime.wMonth), static_cast<std::int32_t>(StartupTime.wDay), static_cast<std::int32_t>(StartupTime.wHour), static_cast<std::int32_t>(StartupTime.wMinute), static_cast<std::int32_t>(StartupTime.wSecond), static_cast<std::int32_t>(StartupTime.wMilliseconds)));
                                        while (true) {
                                            if (Globals::ReloadScriptTemplates || Globals::ReloadModsRequested) {
                                                GR_Main::LoadDatConfigAndModOverrides();
                                            }
                                            if (Globals::ReloadModsRequested) {
                                                GR_Main::LoadSelectedModInstallBlocks();
                                                if (!aPacket::LoadConfiguredPackages()) {
                                                    pas::raise(pas::make_exception<pas::Exception>("Error while openning package files"_a));
                                                }
                                                Globals::ReloadModsRequested = false;
                                            }
                                            GR_Main::InitializeRuntimeAndSettings();
                                            Globals::InitializeGlobalUiRuntime();
                                            Robot::InitializeRobotRuntime();
                                            if (SteamCallbackThread == nullptr) {
                                                SteamCallbackThread = pas::construct_call<TSteamCallbacksThread>(EC_Thread::TThreadEC_Create);
                                            }
                                            if (static_cast<std::uint8_t>(SteamCallbackThread->IsRunning() ^ 1) && SimpleSteamApi::SteamInitialized) {
                                                SteamCallbackThread->Start();
                                            }
                                            MMSystem::timeBeginPeriod(1u);
                                            if (aGalaxy::Galaxy != nullptr) {
                                                aGalaxy::Galaxy->RefreshAllShipDerivedState();
                                                aGalaxy::Galaxy->ReapplyInterfaceOverrides();
                                                aGalaxy::Galaxy->BindScriptImports();
                                                static_cast<void>(HadProtectedStatus);
                                            }
                                            if (GR_Main::BuildVersionMismatch) {
                                                break;
                                            }
                                            GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
                                            Globals::ScreenLoadMode = 0;
                                            if (GlobalsV::PostLoadScreenId == GlobalsV::screenArcadeBattle) {
                                                GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
                                            }
                                            if (GlobalsV::ScreenUsesCompositeLoadAssets(GlobalsV::PostLoadScreenId)) {
                                                Globals::ScreenLoadMode = 3;
                                            }
                                            Globals::RunMainScreenStateLoop();
                                            MMSystem::timeEndPeriod(1u);
                                            HadProtectedStatus = false;
                                            if (aGalaxy::Galaxy != nullptr) {
                                                HadProtectedStatus = GR_Main::CCInterface->GetProtectedStateXorSeed() != 0;
                                                aGalaxy::Galaxy->RestoreProtectedState();
                                                aGalaxy::TGalaxy::ClearIntegrityStatus();
                                            }
                                            Robot::FinalizeRobotRuntime();
                                            GR_Main::AppendLogLineThreadSafe("Closing game screens..."_a);
                                            Globals::FinalizeGlobalUiRuntime();
                                            GR_Main::AppendLogLineThreadSafe("Closing rendering and audio..."_a);
                                            GR_Main::FinalizeRuntimeAndSettings();
                                            GR_Main::AppendLogLineThreadSafe("Rendering and audio closed"_a);
                                            if (GlobalsV::RequestedScreenId == GlobalsV::screenNone && GlobalsV::PostLoadScreenId == GlobalsV::screenNone || GR_Main::ExitScreenLoop) {
                                                GR_Main::FreeDatConfigRoots();
                                                break;
                                            }
                                            if (Globals::ReloadModsRequested) {
                                                Globals::ResetScriptHostRuntimeState();
                                                GR_Main::ResetInstalledPackageState();
                                                GR_Main::FreeDatConfigRoots();
                                                aModsInfo::ClearModInfoState();
                                                GR_Main::SkipModsOnReload = false;
                                            }
                                        }
                                        GR_Main::SkipModsOnReload = false;
                                        if (aGalaxy::Galaxy != nullptr) {
                                            if (ThreadCalc::IsTurnCalculationRunning()) {
                                                ThreadCalc::WaitForTurnCalculation();
                                            }
                                            pas::free(aGalaxy::Galaxy);
                                            aGalaxy::Galaxy = nullptr;
                                            if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
                                                pas::free(aSaveLoad::MemorySnapshotBuffer);
                                            }
                                            aSaveLoad::MemorySnapshotBuffer = nullptr;
                                            GlobalsV::MemorySnapshotActive = false;
                                        }
                                        SE_SoundRnd::FreeAllRandomSounds();
                                        SteamCallbackThread->RequestStop();
                                        SimpleSteamApi::UnloadSteamApi();
                                        Globals::FinalizeScriptHostRuntime();
                                        GR_Main::AppendLogLineThreadSafe("Closing platform runtime..."_a);
                                        GR_Main::FinalizePlatformRuntime();
                                        if (EC_Struct::StartupCleanupObject != nullptr) {
                                            pas::free(EC_Struct::StartupCleanupObject);
                                            EC_Struct::StartupCleanupObject = nullptr;
                                        }
                                        Rangers::PurgeCacheDirectoryFiles();
                                        GR_Main::AppendLogLineThreadSafe("Game shutdown complete"_a);
                                    } catch (...) {
                                        if (static_cast<std::uint8_t>(GR_Main::SuppressModRetryPrompt ^ 1) && GR_Main::SelectedMods != u"" && static_cast<std::uint8_t>(GR_Main::SkipModsOnReload ^ 1)) {
                                            if (WindowsImports::MessageBox(GR_Main::MainWindowHandle, pas::literal_pointer("Failed to launch, do you want to try restarting without mods?"), pas::literal_pointer("Exception:"), WindowsImports::MB_OKCANCEL | WindowsImports::MB_ICONERROR) == WindowsImports::IDOK) {
                                                Globals::ResetScriptHostRuntimeState();
                                                GR_Main::ResetInstalledPackageState();
                                                aModsInfo::ClearModInfoState();
                                                GR_Main::SkipModsOnReload = true;
                                                throw;
                                            }
                                        }
                                        GR_Main::SkipModsOnReload = false;
                                        if (aGalaxy::Galaxy != nullptr) {
                                            // Native passes the event field, not the thread handle.
                                            if (ThreadCalc::IsTurnCalculationRunning()) {
                                                WindowsImports::TerminateThread(Globals::TurnCalculationThread->IdleEvent, 0u);
                                            }
                                            pas::free(aGalaxy::Galaxy);
                                            aGalaxy::Galaxy = nullptr;
                                        }
                                        if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
                                            pas::free(aSaveLoad::MemorySnapshotBuffer);
                                        }
                                        aSaveLoad::MemorySnapshotBuffer = nullptr;
                                        GlobalsV::MemorySnapshotActive = false;
                                        Globals::FinalizeGlobalUiRuntime();
                                        GR_Main::FinalizeRuntimeAndSettings();
                                        SE_SoundRnd::FreeAllRandomSounds();
                                        Globals::FinalizeScriptHostRuntime();
                                        GR_Main::FinalizePlatformRuntime();
                                        if (EC_Struct::StartupCleanupObject != nullptr) {
                                            pas::free(EC_Struct::StartupCleanupObject);
                                            EC_Struct::StartupCleanupObject = nullptr;
                                        }
                                        throw;
                                    }
                                } catch (...) {
                                    auto cpp_exception_2 = pas::caught_object();
                                    if (pas::Exception* StartupException = pas::class_cast_if<pas::Exception*>(cpp_exception_2)) {
                                        GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Exception ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(StartupException))), " with message ", StartupException->message}));
                                    } else {
                                        throw;
                                    }
                                }
                            } while (GR_Main::SkipModsOnReload);
                        }
                    } catch (...) {
                        CheatCode::UnitFinalize();
                        throw;
                    }
                    CheatCode::UnitFinalize();
                }
            } catch (...) {
                Forms::UnitFinalize();
                throw;
            }
            Forms::UnitFinalize();
        }
    }

    void TSteamCallbacksThread::virtual_TThreadEC_Execute() {
        Rangers::TSteamCallbacksThread_Execute(this);
    }

} // namespace Rangers
