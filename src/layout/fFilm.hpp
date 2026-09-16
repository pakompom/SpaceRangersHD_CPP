#pragma once
#include "types/fFilm.hpp"

namespace fFilm {
    static_assert(sizeof(void*) != 4 || sizeof(fFilm::TfFilm) == 304);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PanTimer) == 208);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PanLeft) == 212);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PanRight) == 213);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PanUp) == 214);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PanDown) == 215);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, Playing) == 216);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, CenterShipButton) == 220);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, SpacePanel) == 224);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, MapPanel) == 228);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, FrameSlider) == 232);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, SpeedSlider) == 236);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, TurnSlider) == 240);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PlayButton) == 244);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, StopButton) == 248);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, DateLabel) == 252);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, Loader) == 256);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, CurrentFilm) == 260);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PreloadedFilm) == 264);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, CurrentHistoryIndex) == 268);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PreloadHistoryIndex) == 272);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, CameraTarget) == 276);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, FrameIntervalMs) == 284);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, PlaybackTimer) == 288);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, EffectsTimer) == 292);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, StepIndex) == 296);
    static_assert(sizeof(void*) != 4 || offsetof(fFilm::TfFilm, NextCommand) == 300);
    static_assert(sizeof(void*) != 4 || sizeof(fFilm::TfFilmLoader) == 44);

} // namespace fFilm
