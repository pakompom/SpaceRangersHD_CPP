#pragma once
#include "types/fGov.hpp"

namespace fGov {
    static_assert(sizeof(void*) != 4 || sizeof(fGov::TfGov) == 316);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, PlanetPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, LoadPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, DialogText) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, FormattedTextLength) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, DialogRefreshTimer) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, NextChoiceTop) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, AnimationRestartRequested) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, QuestOffer) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, QuestNegotiationLevel) == 276);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, QuestRewardStep) == 280);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, QuestDurationStep) == 284);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, ScriptDialogNames) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, ScriptDialogCursor) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, PlanetBattleMapId) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, PendingTransition) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, UseHdPortrait) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, UseClassicPortrait) == 305);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, PortraitPanel) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(fGov::TfGov, SavedChoiceScroll) == 312);

} // namespace fGov
