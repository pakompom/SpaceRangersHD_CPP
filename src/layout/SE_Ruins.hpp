#pragma once
#include "types/SE_Ruins.hpp"

namespace SE_Ruins {
    static_assert(sizeof(void*) != 4 || sizeof(SE_Ruins::TRuinsSE) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, ImagePath) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, StaticImagePath) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, MinimapImagePath) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, Animation) == 88);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, StaticImage) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, MinimapImage) == 96);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, FrameIndex) == 100);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, Alpha) == 104);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, AlphaLimit) == 105);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, WeaponPortCount) == 108);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, WeaponPorts) == 112);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, HasTransitionImages) == 192);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, State) == 196);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, FadeTimer) == 200);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, KeepSize) == 204);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, PanelPartnerImage) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(SE_Ruins::TRuinsSE, HideOnStarInfo) == 212);

} // namespace SE_Ruins
