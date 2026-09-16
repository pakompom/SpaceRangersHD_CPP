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
        void ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) override;
        void SaveToBlock(EC_BlockPar::TBlockParEC* Block) override;
        void LoadFromBlock(EC_BlockPar::TBlockParEC* Block) override;
        void NextDay() override;
        pas::WideString CollectLiberationRewards();
        pas::WideString AwardRandomMedal();
        void ProcessShipKill(aShip::TShip* Victim);
        void CheckKillCountAwards(aShip::TShip* Victim);
        void UpdateRelationsForNearbyCombat();
        std::uint8_t SelectAward(std::uint8_t Owner, TAwardTypeMask Kinds, aGalaxyStruct::TShipTypeMask VictimTypes);
        static void GetAwardInfo(std::uint8_t AwardId, aConst::TRewardInfo& Result);
        pas::WideString GetRankName();
        pas::WideString GetRankLongName();
        pas::WideString GetRankDescription();
        pas::WideString GetNextRankName();
        std::uint16_t GetRankPointsToNextRank();
        void AddRankPoints(std::uint16_t Amount);
        std::uint8_t TryPromoteRank();
        std::uint8_t CanPromoteRank();
        pas::WideString GetPirateRankName();
        pas::WideString GetPirateRankLongName();
        pas::WideString GetPirateRankDescription();
        pas::WideString GetNextPirateRankName();
        std::uint16_t GetPirateRankPointsToNextRank();
        void AddPirateRankPoints(std::uint32_t Amount);
        std::uint8_t TryPromotePirateRank();
        std::uint8_t CanPromotePirateRank();
        pas::WideString SelectSituationalMessage(std::uint8_t Automatic);
        void UpdateAfterburnerState() override;
        void TrainSkillsAutomatically();
        aPlanet::TPlanet* LastDockedPlanet;
        std::int32_t TotalShipKillCount;
        std::int32_t PirateKillCount;
        std::int32_t DominatorKillCount;
        std::int32_t LiberatedSystemCount;
        std::int32_t CivilianKillCount;
        std::int32_t MilitaryKillCount;
        std::int32_t RangerKillCount;
        TSystemKillCounts CurrentSystemKills;
        aPlanet::TPlanet* PendingLiberationCeremonyPlanet;
        std::int32_t PendingLiberationContribution;
        std::uint8_t Rank;
        std::uint8_t cpp_padding[1];
        std::uint16_t RankPoints;
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
