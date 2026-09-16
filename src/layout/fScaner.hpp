#pragma once
#include "types/fScaner.hpp"

namespace fScaner {
    static_assert(sizeof(void*) != 4 || sizeof(fScaner::TfScaner) == 636);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, BackgroundBuffer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ExitButton) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ShipToInspect) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemInfoWindow) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemImage) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemNameLabel) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemDescriptionLabel) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemSizeLabel) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemPriceLabel) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemRaceImage) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, SkillsPanel) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, FreeSkillPointsLabel) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, SkillImages) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, SkillPositiveImages) == 280);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, SkillNegativeImages) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, SkillPanels) == 328);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, SkillImageRestTop) == 352);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, SkillButtons) == 376);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, RewardsBuffer) == 400);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, RewardWindow) == 404);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, HoveredItemAnimation) == 408);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, VisibleCargoCount) == 412);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, CargoOffset) == 416);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, CargoEntryCount) == 420);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, PanelSlideTimer) == 424);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, PanelSlideStep) == 428);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, PanelSlideStartX) == 432);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, PanelSlideEndX) == 436);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ItemHoverTimer) == 440);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, HideItemTimer) == 444);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, PropertyHintTimer) == 448);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ShipImageCenter) == 452);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, ArtefactZones) == 460);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, EquipmentAnimations) == 464);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, HoveredItem) == 624);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, HoveredRewardId) == 628);
    static_assert(sizeof(void*) != 4 || offsetof(fScaner::TfScaner, CompactHullInfo) == 632);

} // namespace fScaner
