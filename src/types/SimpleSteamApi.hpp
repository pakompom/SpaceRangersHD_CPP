#pragma once
#include "runtime_support.hpp"
#include "types/WStringUtils.hpp"

namespace SimpleSteamApi {
    struct TAchievementData;

    using TSteamInit = pas::Proc<std::uint8_t(pas::WideString&, pas::WideString&)>;

    using TSteamSetLeaderboardName = pas::Proc<void(const pas::AnsiString&)>;

    using TSteamLocal = pas::Proc<std::uint8_t(std::int32_t)>;

    using TSteamFree = pas::Proc<void()>;

    using TSteamUnlockAchievement = pas::Proc<std::uint8_t(std::int32_t)>;

    using TSteamAchievementsCount = pas::Proc<std::int32_t()>;

    using PAchievementData = TAchievementData*;

    // Caller-owned buffers used by both Steam and local achievement queries.
    #pragma pack(push, 1)
    struct TAchievementData {
        WStringUtils::PStartupWideString Name;
        WStringUtils::PStartupWideString Description;
        std::uint8_t Achieved;
        std::uint8_t HasProgress;
        std::uint8_t cpp_padding[2];
        // Native local backend and allocator set zero; meaning unresolved.
        std::int32_t Reserved0C;
        std::int32_t MaxValue;
        std::int32_t Value;
        WStringUtils::PStartupWideString IconPath;
        std::uint8_t cpp_padding_2[4];
        std::int64_t Date;
    };
    #pragma pack(pop)

    using TSteamIncreaseStat = pas::Proc<std::uint8_t(std::int32_t, std::int32_t)>;

    using TSteamAchievementData = pas::Proc<void(std::int32_t, PAchievementData)>;

    using TSteamLeaderboardFound = pas::Proc<std::uint8_t()>;

    using TSteamUploadScore = pas::Proc<void(std::int32_t)>;

    using TSteamCreateAchievements = pas::Proc<void(std::int32_t)>;

    using TSteamInitAchievement = pas::Proc<void(std::int32_t, const pas::AnsiString&, const pas::AnsiString&, std::int32_t)>;

    using TSteamUserId = pas::Proc<std::int64_t()>;

    using TSteamRunCallbacks = pas::Proc<void()>;

} // namespace SimpleSteamApi
