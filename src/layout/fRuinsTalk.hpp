#pragma once
#include "types/fRuinsTalk.hpp"

namespace fRuinsTalk {
    static_assert(sizeof(void*) != 4 || sizeof(fRuinsTalk::TfRuinsTalk) == 268);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, StationPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, LoadPanel) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, DialogText) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, PresentedTextLength) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, TextPresentationTimer) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, ChoiceHeight) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, NextPortraitCycleAlternate) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, ShowArrivalVideo) == 237);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, StationOwner) == 238);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, StationType) == 239);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, ScriptVideoStartedAt) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, ScriptVideoTimer) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, ResearchItemVisited) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, ResearchItemIndexes) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, LargePortraitLayout) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, PortraitTableVisible) == 257);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, StationTransientControl) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TfRuinsTalk, SavedChoiceScroll) == 264);
    static_assert(sizeof(void*) != 4 || sizeof(fRuinsTalk::TDominionTravelQuote) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TDominionTravelQuote, Star) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TDominionTravelQuote, DrawCount) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TDominionTravelQuote, Cost) == 8);
    static_assert(sizeof(void*) != 4 || sizeof(fRuinsTalk::TConstructionEquipment) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TConstructionEquipment, Item) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TConstructionEquipment, Source) == 4);
    static_assert(sizeof(void*) != 4 || sizeof(fRuinsTalk::TResearchItemSortKey) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TResearchItemSortKey, Priority) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TResearchItemSortKey, Cost) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TResearchItemSortKey, Weight) == 8);

} // namespace fRuinsTalk
