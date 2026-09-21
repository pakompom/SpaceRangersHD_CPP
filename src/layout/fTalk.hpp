#pragma once
#include "types/fTalk.hpp"

namespace fTalk {
    static_assert(sizeof(void*) != 4 || sizeof(fTalk::TfTalk) == 308);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, MainPanel) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, DialogPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, DialogText) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, PresentedTextLength) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, TextPresentationTimer) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, ChoiceHeight) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, SkipShipScriptAdvance) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, ChoiceMousePressed) == 241);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, RequestedMapCenter) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, CurrentMapCenter) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, MapSelectionTimer) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, RequestedMapHover) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, CurrentFilmObjectId) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, CurrentMapHover) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, MinimapRefreshTimer) == 268);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, MinimapEnabled) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, MapDragging) == 273);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, MapDragPoint) == 274);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, SlideTimer) == 284);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, SlideProgress) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, DialogPanelLeft) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, ModalTransition) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, ReturnedFromTrade) == 300);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalk, SavedChoiceScroll) == 304);
    static_assert(sizeof(void*) != 4 || sizeof(fTalk::TfTalkA) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalkA, Reserved04) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalkA, Callback) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalkA, FallbackCallback) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalkA, Value) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalkA, ExtraValue) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(fTalk::TfTalkA, FallbackText) == 32);

} // namespace fTalk
