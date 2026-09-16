#pragma once
#include "types/GI_MessageLoop.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/fTalk.hpp"

namespace aItem {
    struct TItem;

} // namespace aItem

namespace fTalk {
    // Shared disabled-choice callback.
    extern GI_MessageLoop::TDialogChoiceEventGI ScriptDialogBlockCallback;

    extern std::int32_t TruceOfferAmount;

    extern std::int32_t ExtortionDemandAmount;

    extern std::int32_t PartnerOfferAmount;

    extern std::int32_t PartnerGiftAmount;

    // Set by native conversation setup; cleared by cleanup. Suppresses recursive SF_Dialog dispatch.
    extern std::uint8_t TalkDialogActive;

    extern pas::Array<float, 0, 15> TalkSlideCurve;

    std::uint8_t IsMilitaryProtectedQuestItem(aItem::TItem* Item);

    void DonateMilitaryResearchMaterial(aGalaxyStruct::TDominatorSeries Series, std::int32_t Amount);

    std::int32_t GetMilitaryHullRepairCost();

    std::int32_t GetMilitaryEquipmentRepairCost();

    // Native modal conversation wrapper.
    std::uint8_t RunTalk(GI_MessageLoop::TMessageLoopGI* ParentLoop);

    void PayPartnerGiftMoney();

    void PayPiratePartnerGiftMoney();

    void TfTalkA_Create(TfTalkA* Self);

    void TfTalkA_Destroy(TfTalkA* Self);

} // namespace fTalk
