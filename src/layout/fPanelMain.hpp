#pragma once
#include "types/fPanelMain.hpp"

namespace fPanelMain {
    static_assert(sizeof(void*) != 4 || sizeof(fPanelMain::TMessageLoopGIWithMainPanel) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TMessageLoopGIWithMainPanel, MainPanel) == 208);
    static_assert(sizeof(void*) != 4 || sizeof(fPanelMain::TfPanelMain) == 136);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, Screen) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, StatusTimer) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MessagePulseTimer) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MessageSlideTimer) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, HelpLabel) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DisplayedShipId) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DisplayedPlanetId) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MessagePanel) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, BackgroundImage) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, ShipButton) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, GalaxyButton) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, QuestButton) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, EndTurnButton) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MenuButton) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DateLabel) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DateNextImage) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, NextDateLabel) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, NavigationLocked) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MessagePulseStep) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MessageSlideDirection) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MessagePanelRestTop) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DisplayedTurn) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DateSlideProgress) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, TargetTurn) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DateTimer) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, DateTimerIntervalMs) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, AuxiliaryItems) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MoneyWarningActive) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MoneyWarningTicks) == 116);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, MoneyWarningTimer) == 120);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, CargoWarningActive) == 124);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, CargoWarningTicks) == 128);
    static_assert(sizeof(void*) != 4 || offsetof(fPanelMain::TfPanelMain, CargoWarningTimer) == 132);

} // namespace fPanelMain
