#pragma once
#include "types/aPlayer.hpp"
#include "types/fShip2.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace aShip {
    struct TShip;

} // namespace aShip

namespace fShip2 {
    extern std::int32_t StorageImageCount;

    extern pas::List* PlayerHoldEntries;

    extern aShip::TShip* PlayerHoldShip;

    extern std::uint32_t SelfSkillPointColor;

    extern std::uint32_t OtherSkillPointColor;

    extern pas::Array<pas::WideString, 0, 12> ShipEquipmentZoneNames;

    std::uint8_t RunShipEquipment(GI_MessageLoop::TMessageLoopGI* ParentLoop);

    std::int32_t CompareStoredItems(aPlayer::PStorageEntry Left, aPlayer::PStorageEntry Right, TPlayerHoldSort Sort);

    pas::WideString RankToImage(std::uint8_t Rank);

    pas::WideString RankToImageSmall(std::uint8_t Rank);

    pas::WideString PirateRankToImage(std::uint8_t Rank);

    pas::WideString PirateRankToImageSmall(std::uint8_t Rank);

    void InitializePlayerHoldView();

    void FinalizePlayerHoldView();

    void RefreshPlayerHoldView(std::uint8_t IncludeFilteredItems);

    void RemoveEmptyPlayerHoldSlot(std::int32_t Index);

    void ClearPlayerHoldEntries();

    void RemoveEmptyPlayerHoldSlots();

    std::uint8_t IsPlayerHoldSlotEmpty(std::int32_t Index);

    std::int32_t ComparePlayerHoldEntries(TPlayerHoldUnit* Left, TPlayerHoldUnit* Right, TPlayerHoldSort Sort);

    void SortPlayerHoldEntries(TPlayerHoldSort Sort);

    void RestorePlayerHoldDisplayOrder();

    std::int32_t FindPlayerHoldIndexByOrder(std::int32_t DisplayOrder);

    std::int32_t FindFreePlayerHoldOrder();

    void TfShip2_Create(TfShip2* Self);

    void TfShip2_Destroy(TfShip2* Self);

    inline void TfShip2_ProcessWindowMessage(fShip2::TfShip2* Self, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam);

} // namespace fShip2

#include "inline/fShip2.hpp"
