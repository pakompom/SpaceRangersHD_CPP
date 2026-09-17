#pragma once
#include "types/aNormalShip.hpp"

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aNormalShip {
    // Processes qualifying ships after a control change; SourceShip supplies the news-variant seed.
    void ProcessSystemLiberationRewards(TNormalShip* SourceShip, aGalaxy::TStar* Star);

    void TNormalShip_Create(TNormalShip* Self);

    void TNormalShip_Destroy(TNormalShip* Self);

    void TNormalShip_ResolveLoadedReferences(TNormalShip* Self, aGalaxy::TGalaxy* Galaxy);

    void TNormalShip_NextDay(TNormalShip* Self);

    // Distributes kill rewards, career/rank changes and event counters.
    void TNormalShip_ProcessShipKill(TNormalShip* Self, aShip::TShip* Victim);

    // Nearby rangers attacking a friend incur a penalty; attacks on an enemy can improve relations.
    void TNormalShip_UpdateRelationsForNearbyCombat(TNormalShip* Self);

    // Automatic messages suppress object links and select the automatic-message category.
    pas::WideString TNormalShip_SelectSituationalMessage(TNormalShip* Self, std::uint8_t Automatic);

} // namespace aNormalShip
