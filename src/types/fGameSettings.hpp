#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"
#include "types/aGalaxyStruct.hpp"

namespace fGameSettings {
    struct TThreadCreateNewGame;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TThreadCreateNewGame : EC_Thread::TThreadEC {
        PAS_CLASS_META(TThreadCreateNewGame, EC_Thread::TThreadEC, "TThreadCreateNewGame", 76)
        void virtual_TThreadEC_Execute() override;
        std::uint8_t PlayerRace;
        aGalaxyStruct::TGalaxyDifficultyLevels DifficultyLevels;
        std::uint8_t cpp_padding[3];
        std::int32_t CaptainPortraitIndex;
        pas::WideString PlayerName;
        std::int32_t CharacterPreset;
        pas::Array<std::uint8_t, 0, 1> StartingItemTypes;
        pas::Array<std::uint8_t, 0, 1> StartingSkills;
        std::uint8_t IronWill;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGameSettings
