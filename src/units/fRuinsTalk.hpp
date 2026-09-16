#pragma once
#include "types/aItem.hpp"
#include "types/fRuinsTalk.hpp"

namespace aGalaxy {
    struct TStar;

} // namespace aGalaxy

namespace aPlanet {
    struct TPlanet;

} // namespace aPlanet

namespace fRuinsTalk {
    extern std::int32_t BusinessQuoteSmallAmount;

    extern std::int32_t BusinessQuoteMediumAmount;

    extern std::int32_t BusinessQuoteLargeAmount;

    extern std::int32_t BusinessQuoteLargeDueTurn;

    extern std::int32_t BusinessQuoteMediumDueTurn;

    extern std::int32_t BusinessQuoteSmallDueTurn;

    extern float BusinessDepositQuoteInterestRate;

    extern std::int32_t NodeExchangeHighPriorityModule;

    extern std::int32_t NodeExchangeMediumPriorityModule;

    extern std::int32_t NodeExchangeLowPriorityModule;

    extern std::int32_t NodeExchangeHighPriorityCost;

    extern std::int32_t NodeExchangeMediumPriorityCost;

    extern std::int32_t NodeExchangeLowPriorityCost;

    // Shared by allegiance changes and station service quotes.
    extern std::int32_t StationServiceQuoteCost;

    // Rounded distance to the military base destination.
    extern std::int32_t MilitaryTravelDistance;

    extern aGalaxy::TStar* InvestmentRangerCenterStar;

    extern aGalaxy::TStar* InvestmentPirateBaseStar;

    extern aGalaxy::TStar* InvestmentMilitaryBaseStar;

    extern aGalaxy::TStar* InvestmentScienceBaseStar;

    extern aGalaxy::TStar* InvestmentBusinessCenterStar;

    extern aGalaxy::TStar* InvestmentMedicalBaseStar;

    extern aPlanet::TPlanet* InvestmentDefensePlanet;

    extern pas::Array<std::int32_t, 0, 11> InvestmentQuoteCosts;

    extern std::uint8_t SelectedResearchSeries;

    extern std::int32_t NearbyTradeAdviceCost;

    extern std::int32_t DistantTradeAdviceCost;

    extern pas::Array<std::int32_t, 0, 11> PirateProgramQuoteCosts;

    extern pas::Array<std::int32_t, 0, 2> PirateChameleonQuoteCosts;

    // Shared quote amounts are replaced when opening either banking dialog.
    extern aItem::TEquipment* StationImprovementItem;

    extern aItem::TImprovementKind StationImprovementKind;

    extern std::int32_t StationImprovementDetail;

    // 0: station services; 1: hull bridge; higher values: custom bridge.
    extern std::uint8_t StationBridgeMode;

    extern pas::Array<fRuinsTalk::TConstructionEquipment, 42, 49> ConstructionEquipment;

    extern pas::Array<fRuinsTalk::TConstructionEquipment, 1, 5> ConstructionWeapons;

    extern pas::Array<fRuinsTalk::TDominionTravelQuote, 1, 4> DominionTravelQuotes;

    std::int32_t GetDominionRelocationCost(aGalaxy::TStar* Star);

    std::int32_t ApplyRecentDominionOrderSurcharge(float Cost);

    std::int32_t GetConstructionShopCost();

    std::int32_t GetConstructionFreeSpace();

    void SelectConstructionItem(aItem::TEquipment* Item, std::uint8_t Source);

    pas::WideString GetStationBackgroundPath();

    void ResetStationImprovement();

    void PayDepositMoney();

    // Native M_Main reserves four unreferenced bytes after its inline scalar cells.
    // Keep that gap without moving the named locals or emitting an instruction.
    void ReserveGreetingFrame();

    // Preserve the native clamp cells before the captured money receiver.
    void PayStationModernization(std::uint32_t QuotedCost);

    void PayNationalityMoney();

    void PayChameleonMoney(std::int32_t Cost);

    // Native reads the active count before evaluating the rank clamps.
    void ComputeStimulantOfferLimit(std::uint8_t Rank, std::int32_t Bonus, std::int32_t& Limit);

    void PayConstructionMoney(std::int32_t Price);

    void TfRuinsTalk_Create(TfRuinsTalk* Self);

    void TfRuinsTalk_Destroy(TfRuinsTalk* Self);

    inline void TfRuinsTalk_ProcessWindowMessage(fRuinsTalk::TfRuinsTalk* Self, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam);

} // namespace fRuinsTalk

#include "inline/fRuinsTalk.hpp"
