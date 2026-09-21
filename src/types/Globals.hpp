#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Expression {
    struct TCodeEC;

} // namespace EC_Expression

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace Globals {
    struct TPlanetAdvtGroup;

} // namespace Globals

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace Globals {
    struct TScriptTemplUnit;

    struct TPlanetTempl;

    struct TPlanetSpaceTemplate;

    struct TSputnikTempl;

    struct TPlayerMessageTarget;

    struct TMessagePlayerTypeGraph;

    struct TMessagePlayer;

    struct TRobotMap;

    struct TShipGreetingsInfo;

    struct TGovGreetingsInfo;

    struct TPlanetAdvtUnit;

    struct TPlanetAdvtList;

    #pragma pack(push, 1)
    struct TPlayerMessageTarget {
        std::uint32_t ShipId;
        std::uint32_t PlanetId;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMessagePlayer : EC_Struct::TObjectEx {
        PAS_CLASS_META(TMessagePlayer, EC_Struct::TObjectEx, "TMessagePlayer", 68)
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Leaves linkage and Button untouched. ImageNameOverride is present only from save version 109 onward.
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        pas::WideString GetNormalImageName();
        pas::WideString GetActiveImageName();
        pas::WideString GetPressedImageName();
        TMessagePlayer* Prev;
        TMessagePlayer* Next;
        pas::WideString Key;
        std::uint8_t Kind;
        std::uint8_t cpp_padding[3];
        pas::WideString ImageNameOverride;
        // 0: new message; 1: system liberation; some message kinds override it.
        std::int32_t NotificationSoundKind;
        std::int32_t Turn;
        pas::WideString Text;
        pas::Array<TPlayerMessageTarget, 0, 2> Targets;
        // Borrowed UI object, rebuilt after loading.
        GI_GraphButton::TGraphButtonGI* Button;
        std::uint8_t WasRead;
        std::uint8_t NotificationSoundPlayed;
        std::uint8_t cpp_padding_2[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSputnikTempl : pas::Object {
        PAS_CLASS_META(TSputnikTempl, pas::Object, "TSputnikTempl", 12)
        std::int32_t Radius;
        pas::WideString MaskName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPlanetTempl : pas::Object {
        PAS_CLASS_META(TPlanetTempl, pas::Object, "TPlanetTempl", 24)
        std::int32_t Radius;
        pas::WideString SmallMaskName;
        pas::WideString SmallLightName;
        pas::WideString MaskName;
        pas::WideString LightName;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPlanetSpaceTemplate {
        // First SE.Planet.Style value.
        std::int32_t Style;
        // Optional second SE.Planet.Style value.
        std::int32_t StyleVariant;
        std::int32_t Radius;
        // Retained until UI shutdown.
        SE_Space::TObjectSE* SpaceObject;
    };
    #pragma pack(pop)

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptTemplUnit : EC_Struct::TObjectEx {
        PAS_CLASS_META(TScriptTemplUnit, EC_Struct::TObjectEx, "TScriptTemplUnit", 32)
        void p_destroy() override;
        // First comma-delimited Script template value; copied to TScript.ClassId on creation/restart.
        std::int32_t ClassId;
        pas::WideString Name;
        pas::WideString FileName;
        // Returned by SF_GCntRun.
        std::int32_t UseCount;
        // Returned by SF_GLastTurnRun.
        std::int32_t LastTurn;
        // -1 when no active galaxy script is bound.
        std::int32_t ActiveScriptIndex;
        EC_Expression::TCodeEC* ConditionCode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Native record RTTI.
    struct TRobotMap {
        std::int32_t Id;
        pas::WideString Name;
        // -1 when absent.
        std::int32_t Group;
        std::int32_t Access;
        // Red=1, Green=2, Blue=4.
        std::int32_t Side;
        std::int32_t Length;
        pas::WideString Map;
        // Empty mask means Any.
        aGalaxyStruct::TOwnerMask PlanetRace;
        // Empty mask means Any.
        aGalaxyStruct::TOwnerMask PlayerRace;
        // Empty mask means Any.
        aGalaxyStruct::TRangerCareerSet PlayerStatus;
        std::uint8_t cpp_padding[1];
        std::int32_t MinWins;
        std::int32_t MaxWins;
        std::int32_t Reiteration;
        std::uint8_t ReinforcementsDisabled;
        std::uint8_t Terron;
        std::uint8_t Demo;
        std::uint8_t AfterLiberation;
        pas::WideString GovTextStart;
        pas::WideString GovTextWin;
        pas::WideString GovTextLoss;
        pas::WideString RobotsStart;
        pas::WideString RobotsWin;
        pas::WideString RobotsLoss;
        pas::WideString FromAuthor;
        // Scratch rebuilt from player history by TPlayer.SelectPlanetBattleMap; not set by the definition loader.
        std::int32_t PlayerPlayCount;
    };

    // Native record RTTI.
    struct TPlanetAdvtUnit {
        pas::WideString Name;
        pas::WideString Image1;
        pas::WideString Image2;
        // Clamped to -1..1.
        std::int32_t War;
        // 0..7, or 42 when absent/unrecognized.
        std::uint8_t Goods;
        aGalaxyStruct::TOwnerMask Owner;
        std::uint8_t cpp_padding[2];
    };

    // Native RTTI names TPlanetAdvtList: a weighted advert sequence.
    // Field-only object preserves anonymous numbering; original record/object syntax is uncertain.
    struct TPlanetAdvtList {
        // Numeric PlanetAdvt.List parameter name; weighted selection in TPlanetSE.StartRandomSurfaceAnimation.
        std::int32_t Weight;
        pas::DynArray<std::int32_t> Indices;
    };

    // Native record RTTI.
    struct TPlanetAdvtGroup {
        pas::WideString Image1;
        pas::WideString Image2;
        WindowsSdk::TPoint Position;
        pas::DynArray<TPlanetAdvtUnit> Adverts;
        pas::DynArray<TPlanetAdvtList> Lists;
    };

    using TGreetingMask = pas::Set<0, 7>;

    // Byte predicates use 0=Yes, 1=No, 2=Any; omitted-field defaults vary by rule.
    // Native record RTTI.
    struct TGovGreetingsInfo {
        pas::WideString Name;
        std::int32_t Priority;
        aGalaxyStruct::TOwnerMask PlayerRace;
        aGalaxyStruct::TRangerCareerSet PlayerStatus;
        TGreetingMask PlayerRating;
        TGreetingMask PlayerRank;
        std::uint8_t Goods;
        aGalaxyStruct::TOwnerMask CurPlanetRace;
        std::uint8_t CurPlanetRaceIsPlayerRace;
        aGalaxyStruct::TRelationLevels CurPlanetRelations;
        std::uint8_t CurPlanetGoodsPermit;
        TGreetingMask CurPlanetGoodsCnt;
        TGreetingMask CurPlanetGoodsSale;
        TGreetingMask CurPlanetGoodsBuy;
        aGalaxyStruct::TPlanetEconomies CurPlanetEconomy;
        aGalaxyStruct::TPlanetGovernments CurPlanetGovernment;
        aGalaxyStruct::TGreetingCountMask RangerInCurStar;
        aGalaxyStruct::TGreetingCountMask PirateInCurStar;
        aGalaxyStruct::TGreetingCountMask KlingInCurStar;
        aGalaxyStruct::TGreetingCountMask WarriorInCurStar;
        aGalaxyStruct::TGreetingCountMask TransportInCurStar;
        std::uint8_t CurStarInBattle;
        aGalaxyStruct::TOwnerMask ToPlanetRace;
        std::uint8_t ToPlanetRaceIsPlayerRace;
        std::uint8_t ToPlanetRaceIsCurPlanetRace;
        aGalaxyStruct::TRelationLevels ToPlanetRelations;
        std::uint8_t ToPlanetGoodsPermit;
        TGreetingMask ToPlanetGoodsCnt;
        TGreetingMask ToPlanetGoodsSale;
        TGreetingMask ToPlanetGoodsBuy;
        aGalaxyStruct::TPlanetEconomies ToPlanetEconomy;
        aGalaxyStruct::TPlanetGovernments ToPlanetGovernment;
        std::uint8_t ToPlanetInCurStar;
        aGalaxyStruct::TGreetingCountMask RangerInToStar;
        aGalaxyStruct::TGreetingCountMask PirateInToStar;
        aGalaxyStruct::TGreetingCountMask KlingInToStar;
        aGalaxyStruct::TGreetingCountMask WarriorInToStar;
        aGalaxyStruct::TGreetingCountMask TransportInToStar;
        std::uint8_t ToStarControlByKling;
        std::uint8_t ToStarInBattle;
        std::uint8_t CurPlanetPirateClan;
        std::uint8_t CurStarInBattlePirates;
        aGalaxyStruct::TGreetingCountMask PirateClanInCurStar;
        aGalaxyStruct::TGreetingCountMask PirateClanInToStar;
        std::uint8_t ToStarControlByPirates;
        std::uint8_t CoalitionAlreadyDefeated;
        std::uint8_t DominatorsAlreadyDefeated;
        TGreetingMask PlayerPirateRank;
        std::uint8_t cpp_padding[2];
    };

    // Native record RTTI.
    struct TShipGreetingsInfo {
        pas::WideString Name;
        std::int32_t Priority;
        std::uint8_t AutoTalk;
        std::uint8_t FlyType;
        TGreetingMask ShipType;
        aGalaxyStruct::TRelationLevels Relations;
        aGalaxyStruct::TOwnerMask ShipRace;
        aGalaxyStruct::TOwnerMask PlayerRace;
        std::uint8_t ShipRaceIsPlayerRace;
        std::uint8_t PlayerAttackGoodShip;
        std::uint8_t InFear;
        std::uint8_t ShipBadFlyToShip;
        TGreetingMask ShipBadType;
        aGalaxyStruct::TOwnerMask ShipBadRace;
        std::uint8_t ShipFlyToPlayer;
        std::uint8_t PlayerFlyToShip;
        std::uint8_t PlayerIsShipBad;
        aGalaxyStruct::TGreetingCountMask ShipTurnBeforeEndOrder;
        aGalaxyStruct::TGreetingCountMask PlayerTurnBeforeEndOrder;
        aGalaxyStruct::TGreetingCountMask ShipBadTurnBeforeEndOrder;
        aGalaxyStruct::TRangerCareerSet ShipStatus;
        aGalaxyStruct::TRangerCareerSet PlayerStatus;
        TGreetingMask ShipStrength;
        TGreetingMask PlayerStrength;
        TGreetingMask ShipStructure;
        TGreetingMask PlayerStructure;
        TGreetingMask ShipRating;
        TGreetingMask PlayerRating;
        TGreetingMask ShipRank;
        TGreetingMask PlayerRank;
        TGreetingMask RatingShipWithPlayer;
        TGreetingMask RankShipWithPlayer;
        TGreetingMask StrengthShipWithPlayer;
        std::uint8_t Goods;
        TGreetingMask ShipGoodsCnt;
        TGreetingMask PlayerGoodsCnt;
        std::uint8_t ShipHaveGoods;
        std::uint8_t PlayerHaveGoods;
        aGalaxyStruct::TGreetingCountMask ShipGoodsTypeCnt;
        aGalaxyStruct::TGreetingCountMask PlayerGoodsTypeCnt;
        std::uint8_t ShipMayScanPlayer;
        aGalaxyStruct::TGreetingCountMask RangerInCurStar;
        aGalaxyStruct::TGreetingCountMask PirateInCurStar;
        aGalaxyStruct::TGreetingCountMask KlingInCurStar;
        aGalaxyStruct::TGreetingCountMask WarriorInCurStar;
        aGalaxyStruct::TGreetingCountMask TransportInCurStar;
        aGalaxyStruct::TOwnerMask LastPlanetRace;
        aGalaxyStruct::TRelationLevels LastPlanetRelations;
        TGreetingMask LastPlanetGoodsCnt;
        TGreetingMask LastPlanetGoodsSale;
        TGreetingMask LastPlanetGoodsBuy;
        std::uint8_t LastPlanetIsHomePlanet;
        std::uint8_t LastPlanetRaceIsShipRace;
        std::uint8_t LastPlanetRaceIsPlayerRace;
        aGalaxyStruct::TPlanetEconomies LastPlanetEconomy;
        aGalaxyStruct::TPlanetGovernments LastPlanetGovernment;
        std::uint8_t LastPlanetInCurStar;
        aGalaxyStruct::TGreetingCountMask LastPlanetDistToShipInTurn;
        aGalaxyStruct::TGreetingCountMask RangerInLastPlanetStar;
        aGalaxyStruct::TGreetingCountMask PirateInLastPlanetStar;
        aGalaxyStruct::TGreetingCountMask KlingInLastPlanetStar;
        aGalaxyStruct::TGreetingCountMask WarriorInLastPlanetStar;
        aGalaxyStruct::TGreetingCountMask TransportInLastPlanetStar;
        aGalaxyStruct::TOwnerMask ToPlanetRace;
        aGalaxyStruct::TRelationLevels ToPlanetRelations;
        TGreetingMask ToPlanetGoodsCnt;
        TGreetingMask ToPlanetGoodsSale;
        TGreetingMask ToPlanetGoodsBuy;
        std::uint8_t ToPlanetIsHomePlanet;
        std::uint8_t ToPlanetRaceIsShipRace;
        std::uint8_t ToPlanetRaceIsPlayerRace;
        aGalaxyStruct::TPlanetEconomies ToPlanetEconomy;
        aGalaxyStruct::TPlanetGovernments ToPlanetGovernment;
        std::uint8_t ToPlanetIsLastPlanet;
        std::uint8_t ToPlanetRaceIsLastPlanetRace;
        std::uint8_t HomePlanetInToStar;
        std::uint8_t HomePlanetInCurStar;
        std::uint8_t ToStarControlByKling;
        std::uint8_t ToStarInBattle;
        aGalaxyStruct::TGreetingCountMask RangerInToStar;
        aGalaxyStruct::TGreetingCountMask PirateInToStar;
        aGalaxyStruct::TGreetingCountMask KlingInToStar;
        aGalaxyStruct::TGreetingCountMask WarriorInToStar;
        aGalaxyStruct::TGreetingCountMask TransportInToStar;
        std::uint8_t cpp_padding[1];
        pas::WideString ItemType;
        std::uint8_t ShipNeedInItem;
        TGreetingMask ToShipType;
        aGalaxyStruct::TOwnerMask ToShipRace;
        std::uint8_t ToShipInPlanet;
        std::uint8_t ToShipBad;
        aGalaxyStruct::TRelationLevels ToShipRelations;
        // Second field loaded from RankShipWithPlayer; the normal-ship consumer compares PirateRank.
        TGreetingMask RankShipWithPlayerExtra;
        TGreetingMask PlayerPirateRank;
        std::uint8_t Female;
        std::uint8_t ToStarControlByPirates;
        aGalaxyStruct::TGreetingCountMask PirateClanInCurStar;
        aGalaxyStruct::TGreetingCountMask PirateClanInToStar;
        std::uint8_t CoalitionAlreadyDefeated;
        std::uint8_t DominatorsAlreadyDefeated;
    };

    // Native record RTTI.
    struct TMessagePlayerTypeGraph {
        pas::WideString NormalImage;
        pas::WideString ActiveImage;
        pas::WideString PressedImage;
        std::int32_t LifetimeTurns;
    };

    using TPlayerMessageKindSet = pas::Set<0, 15>;

    using PPlanetAdvertDefinition = TPlanetAdvtGroup*;

} // namespace Globals
