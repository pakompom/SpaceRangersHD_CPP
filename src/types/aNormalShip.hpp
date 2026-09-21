#pragma once
#include "runtime_support.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aConst {
    struct TRewardInfo;

} // namespace aConst

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace aNormalShip {
    struct TSystemKillCounts;

    struct TNormalShip;

    #pragma pack(push, 1)
    struct TSystemKillCounts {
        std::uint16_t Normal;
        std::uint16_t Dominator;
        std::uint16_t Pirate;
        std::uint16_t Custom;
    };
    #pragma pack(pop)

    using TAwardTypeMask = pas::Set<0, 7>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TNormalShip : aShip::TShip {
        PAS_CLASS_META(TNormalShip, aShip::TShip, "TNormalShip", 1296)
        void p_destroy() override;
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) override;
        void virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        // Native editable import truncates PirateRankPoints to Word.
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void virtual_TShip_NextDay() override;
        // Consumes the pending ceremony, grants rewards and returns the player's presentation text.
        pas::WideString CollectLiberationRewards();
        // Selects a merit award using the current planet, station or default human owner and returns its name.
        pas::WideString AwardRandomMedal();
        void CheckKillCountAwards(aShip::TShip* Victim);
        // Returns 255 when no award qualifies; retries duplicates twice.
        std::uint8_t SelectAward(aGalaxyStruct::TOwnerId Owner, TAwardTypeMask Kinds, aGalaxyStruct::TShipTypeMask VictimTypes);
        static void GetAwardInfo(std::uint8_t AwardId, aConst::TRewardInfo& Result);
        pas::WideString GetRankName();
        pas::WideString GetRankLongName();
        pas::WideString GetRankDescription();
        // Does not check for maximum rank.
        pas::WideString GetNextRankName();
        std::uint16_t GetRankPointsToNextRank();
        // Caps the addition at the points needed for the next rank.
        void AddRankPoints(std::uint16_t Amount);
        // Maximum rank is 7; promotion resets RankPoints.
        std::uint8_t TryPromoteRank();
        std::uint8_t CanPromoteRank();
        pas::WideString GetPirateRankName();
        pas::WideString GetPirateRankLongName();
        pas::WideString GetPirateRankDescription();
        // Does not check maximum rank.
        pas::WideString GetNextPirateRankName();
        std::uint16_t GetPirateRankPointsToNextRank();
        // Caps the addition at the points needed for the next rank.
        void AddPirateRankPoints(std::uint32_t Amount);
        // Maximum rank is 7; promotion resets PirateRankPoints.
        std::uint8_t TryPromotePirateRank();
        std::uint8_t CanPromotePirateRank();
        // Enables afterburner for multi-turn orders with a serviceable engine.
        void UpdateAfterburnerState() override;
        void TrainSkillsAutomatically();
        aPlanet::TPlanet* LastDockedPlanet;
        std::int32_t TotalShipKillCount;
        std::int32_t PirateKillCount;
        std::int32_t DominatorKillCount;
        // Also counts qualifying captures by the pirate side.
        std::int32_t LiberatedSystemCount;
        // Excludes Pirate Clan civilians.
        std::int32_t CivilianKillCount;
        std::int32_t MilitaryKillCount;
        // Excludes rangers omitted from the rating.
        std::int32_t RangerKillCount;
        TSystemKillCounts CurrentSystemKills;
        aPlanet::TPlanet* PendingLiberationCeremonyPlanet;
        std::int32_t PendingLiberationContribution;
        std::uint8_t Rank;
        std::uint8_t cpp_padding[1];
        std::uint16_t RankPoints;
        // Shared 30-turn cooldown for money, cargo and paid-truce demands.
        std::int32_t LastPlayerExtortionTurn;
        std::uint8_t PirateRank;
        std::uint8_t cpp_padding_2[3];
        std::uint32_t PirateRankPoints;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TSystemKillCountArray = pas::Array<std::uint16_t, 0, 3>;

} // namespace aNormalShip
