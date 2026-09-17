#pragma once
#include "types/aPlanet.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace aGalaxy {
    struct TGalaxy;

} // namespace aGalaxy

namespace aItem {
    struct TEquipment;

} // namespace aItem

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace aPlanet {
    // Compared against an inclusive 0..100 roll for each economic event.
    extern std::int32_t EconomicEventChance;

    // Five 32-byte rows, eight Integer weights in TKlingType order; SpawnWeightedDominatorShip selects row 1..5 from faction control.
    extern aPlanet::TDominatorSpawnWeightTable DominatorSpawnWeights;

    // Script.PlanetPirateClan; borrowed reference, retained after PirateWin(3).
    extern aPlanet::TPlanet* MainPiratePlanet;

    void TSputnik_Create(TSputnik* Self);

    void TSputnik_Destroy(TSputnik* Self);

    void TPlanet_Create(TPlanet* Self);

    void TPlanet_Destroy(TPlanet* Self);

    void TPlanet_LoadFromBuffer(TPlanet* Self, EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy);

    void TPlanet_NextDay(TPlanet* Self);

    // Queues planet dialogue to the UI thread and waits for its event; requires normal-space player state.
    std::uint8_t TPlanet_RequestDialog(TPlanet* Self);

    void TPlanet_UpdateMarketState(TPlanet* Self);

    aItem::TEquipment* TPlanet_GenerateEquipmentOffer(TPlanet* Self, void* Ship, std::uint8_t ItemType);

    // Returns a new owning list of generated equipment, using the race quota table. Caller forwards ForceGeneratedOffers in CL; this routine saves but never reads it.
    aMyFunction::TObjectList* TPlanet_BuildEquipmentOfferBatch(TPlanet* Self, void* Ship, std::uint8_t UnusedForceGeneratedOffers);

} // namespace aPlanet
