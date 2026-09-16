#include "layout/SimpleSteamApi.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/SystemImports.hpp"
#include "units/Achievements.hpp"
#include "units/SimpleSteamApi.hpp"
#include "units/SysUtils.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"

namespace SimpleSteamApi {
    std::uint8_t SteamInitialized = false;

    SimpleSteamApi::TSteamUserId SteamUserId{};

    SimpleSteamApi::TSteamInit SteamInit{};

    SimpleSteamApi::TSteamCreateAchievements SteamCreateAchievements{};

    SimpleSteamApi::TSteamInitAchievement SteamInitAchievement{};

    SimpleSteamApi::TSteamSetLeaderboardName SteamSetLeaderboardName{};

    SimpleSteamApi::TSteamLeaderboardFound SteamLeaderboardFound{};

    SimpleSteamApi::TSteamUploadScore SteamUploadScore{};

    SimpleSteamApi::TSteamLocal SteamLocal{};

    SimpleSteamApi::TSteamRunCallbacks SteamRunCallbacks{};

    void* SteamResetAchievements{};

    SimpleSteamApi::TSteamUnlockAchievement SteamUnlockAchievement{};

    void* SteamStat{};

    SimpleSteamApi::TSteamIncreaseStat SteamIncreaseStat{};

    SimpleSteamApi::TSteamFree SteamFree{};

    void* SteamAchievementsOverlay{};

    SimpleSteamApi::TSteamAchievementsCount SteamAchievementsCount{};

    SimpleSteamApi::TSteamAchievementData SteamAchievementData{};

    void* SteamStatus{};

    void LoadSteamApi() {
        std::int32_t Error{};
        WindowsSdk::HMODULE Module = WindowsImports::LoadLibrary(pas::literal_pointer("steam_ach.dll"));
        if (Module == 0) {
            Error = WindowsImports::GetLastError();
            if (Error == 126) {
                pas::raise(pas::make_exception<pas::Exception>("cant load steam_ach.dll, missing some file"_a));
            } else if (Error == 193) {
                pas::raise(pas::make_exception<pas::Exception>("cant load steam_ach.dll, module versions mismatch"_a));
            } else {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"cant load steam_ach.dll, lasterror=", SysUtils::IntToStr(Error)})));
            }
        }
        SteamUserId = pas::callback_from_address<TSteamUserId>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamUserID")));
        SteamInit = pas::callback_from_address<TSteamInit>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamInit")));
        SteamCreateAchievements = pas::callback_from_address<TSteamCreateAchievements>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("createAchievements")));
        SteamInitAchievement = pas::callback_from_address<TSteamInitAchievement>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("initAchievement")));
        SteamSetLeaderboardName = pas::callback_from_address<TSteamSetLeaderboardName>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamSetLeaderBoardName")));
        SteamLeaderboardFound = pas::callback_from_address<TSteamLeaderboardFound>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamLeaderBoardFound")));
        SteamUploadScore = pas::callback_from_address<TSteamUploadScore>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamUploadScore")));
        SteamLocal = pas::callback_from_address<TSteamLocal>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamLocal")));
        SteamRunCallbacks = pas::callback_from_address<TSteamRunCallbacks>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamCallBacks")));
        SteamResetAchievements = WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamResetAchievements"));
        SteamUnlockAchievement = pas::callback_from_address<TSteamUnlockAchievement>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamAchievement")));
        SteamStat = WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamStat"));
        SteamIncreaseStat = pas::callback_from_address<TSteamIncreaseStat>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamStatIncrease")));
        SteamFree = pas::callback_from_address<TSteamFree>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamFree")));
        SteamAchievementsOverlay = WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamAchievementsOverlay"));
        SteamAchievementsCount = pas::callback_from_address<TSteamAchievementsCount>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamAchievementsCount")));
        SteamAchievementData = pas::callback_from_address<TSteamAchievementData>(WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamAchievementData")));
        SteamStatus = WindowsImports::GetProcAddress(Module, pas::literal_pointer("steamStatus"));
    }

    void UnloadSteamApi() {
        WindowsSdk::HMODULE Module = WindowsSdk::GetModuleHandle(pas::literal_pointer("steam_ach.dll"));
        if (Module != 0) {
            SimpleSteamApi::SteamFree();
            WindowsImports::FreeLibrary(Module);
        }
    }

    void InitializeSteamAchievements() {
        std::int32_t Index{};
        std::int32_t MaxValue{};
        std::int32_t Number{};
        EC_BlockPar::TBlockParEC* Block{};
        pas::AnsiString AchievementName{};
        pas::AnsiString StatName{};
        SimpleSteamApi::SteamCreateAchievements(82);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Achievements::AchievementDefinitions->GetBlockCount() - 1); cpp_range.next(Index); ) {
            Block = Achievements::AchievementDefinitions->GetBlockByIndex(Index);
            AchievementName = static_cast<pas::AnsiString>(Achievements::AchievementDefinitions->GetBlockNameByIndex(Index));
            Number = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Num"_wref.get())));
            MaxValue = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MaxValue"_wref.get())));
            if (MaxValue > 0) {
                StatName = pas::concat_ansi({"STAT_", AchievementName});
            } else {
                StatName = "null"_a;
            }
            AchievementName = pas::concat_ansi({"ACH_", AchievementName});
            SimpleSteamApi::SteamInitAchievement(Number, AchievementName, StatName, MaxValue);
        }
    }

} // namespace SimpleSteamApi
