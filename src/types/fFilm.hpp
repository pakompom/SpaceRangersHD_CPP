#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aEFilm.hpp"

namespace GI_GraphButton {
    struct TGraphButtonGI;

} // namespace GI_GraphButton

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace GI_ScrollBar {
    struct TScrollBarGI;

} // namespace GI_ScrollBar

namespace fFilm {
    struct TfFilmLoader;

    struct TfFilm;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfFilm : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfFilm, GI_MessageLoop::TMessageLoopGI, "TfFilm", 304)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        WindowsSdk::TPoint GetViewOffset();
        // Disables automatic camera following.
        void SetViewOffset(WindowsSdk::TPoint Offset);
        // Ignored while automatic camera following is disabled.
        void FollowViewOffset(WindowsSdk::TPoint Offset);
        void PanView(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void KeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void CopyLiveVisualStateToFilm();
        void CopyFilmVisualStateToLive();
        void ExitClicked(GI_MessageLoop::TObjectGI* Sender);
        void CenterShipClicked(GI_MessageLoop::TObjectGI* Sender);
        // Waits for the loader, swaps film buffers, resets the command cursor, then preloads the following entry. Requires a valid index and nonempty command stream.
        void SelectHistoryEntry(std::int32_t Index, std::uint8_t InitialLoad);
        static void CreateFilmSceneObjects(aEFilm::TEFilm* Film);
        static void ReleaseFilmSceneObjects(aEFilm::TEFilm* Film, std::uint8_t ReleaseTrailingReferences);
        void ReuseSceneObjectsForPreloadedFilm();
        static void AdvancePausedEffects(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SetFrameInterval(std::int32_t IntervalMs, std::uint8_t UpdateSlider);
        void SpeedSliderChanged(GI_MessageLoop::TObjectGI* Sender);
        // Backward seeking reloads the recording and executes commands forward to the requested step.
        void FrameSliderChanged(GI_MessageLoop::TObjectGI* Sender);
        void PlayStopClicked(GI_MessageLoop::TObjectGI* Sender);
        void TurnSliderChanged(GI_MessageLoop::TObjectGI* Sender);
        void StartPlayback();
        // Trailing effects continue on a separate timer.
        void PausePlayback();
        // Automatically advances to the following retained recording when this one ends.
        void AdvancePlayback(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        // Requires NextCommand <> nil.
        void AdvanceOneStep();
        void InvalidateAnimatedControls();
        void DrawFrame() override;
        void SelectMusic() override;
        GI_MessageLoop::PCallbackTimerGI PanTimer;
        std::uint8_t PanLeft;
        std::uint8_t PanRight;
        std::uint8_t PanUp;
        std::uint8_t PanDown;
        std::uint8_t Playing;
        std::uint8_t cpp_padding[3];
        GI_GraphButton::TGraphButtonGI* CenterShipButton;
        GI_Panel::TPanelGI* SpacePanel;
        GI_MessageLoop::TObjectGI* MapPanel;
        GI_ScrollBar::TScrollBarGI* FrameSlider;
        GI_ScrollBar::TScrollBarGI* SpeedSlider;
        GI_ScrollBar::TScrollBarGI* TurnSlider;
        GI_GraphButton::TGraphButtonGI* PlayButton;
        GI_GraphButton::TGraphButtonGI* StopButton;
        GI_Label::TLabelGI* DateLabel;
        TfFilmLoader* Loader;
        aEFilm::TEFilm* CurrentFilm;
        aEFilm::TEFilm* PreloadedFilm;
        std::int32_t CurrentHistoryIndex;
        std::int32_t PreloadHistoryIndex;
        EC_Struct::TPointF CameraTarget;
        std::int32_t FrameIntervalMs;
        GI_MessageLoop::PCallbackTimerGI PlaybackTimer;
        GI_MessageLoop::PCallbackTimerGI EffectsTimer;
        std::int32_t StepIndex;
        aEFilm::PEFilmCommand NextCommand;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfFilmLoader : EC_Thread::TThreadEC {
        PAS_CLASS_META(TfFilmLoader, EC_Thread::TThreadEC, "TfFilmLoader", 44)
        void virtual_TThreadEC_Execute() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fFilm
