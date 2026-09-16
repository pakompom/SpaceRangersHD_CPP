#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"

namespace GI_Image {
    struct TImageGI;

} // namespace GI_Image

namespace GI_Label {
    struct TLabelGI;

} // namespace GI_Label

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace fPanelLoad {
    struct TfPanelLoad;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPanelLoad : EC_Struct::TObjectEx {
        PAS_CLASS_META(TfPanelLoad, EC_Struct::TObjectEx, "TfPanelLoad", 160)
        void p_destroy() override;
        void InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen);
        void OnOpen();
        void OnClose();
        static std::int32_t GetProgressSegmentCount();
        void Show();
        void Hide();
        // Accepts groups 0..3; selects a style for shutter or legacy artwork. Other values preserve the current style.
        void SelectBackgroundStyle(std::int32_t StyleGroup);
        void RefreshBackgroundImages();
        // Requires a fraction in 0..1; does not clamp the progress-segment index.
        void SetProgress(float Fraction);
        void SetShutterOpenFraction(float Fraction);
        void StartOpeningShutters();
        // Closes Screen after the animation, or immediately when shutters are disabled.
        void StartClosingShutters();
        void UpdateOpeningShutters(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void UpdateClosingShutters(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        std::uint8_t IsAnimatingShutters();
        // Returns zero without an active timer.
        std::int32_t GetShutterDirection();
        GI_MessageLoop::TMessageLoopGI* Screen;
        pas::Array<GI_Image::TImageGI*, 0, 16> ProgressSegments;
        std::uint8_t LayoutAdjusted;
        std::uint8_t cpp_padding[3];
        GI_MessageLoop::TObjectGI* BackgroundImage;
        GI_MessageLoop::TObjectGI* ShipPanelImage;
        GI_MessageLoop::TObjectGI* LoadAnimation;
        GI_MessageLoop::TObjectGI* AnimationText;
        GI_Label::TLabelGI* ProgressLabel;
        GI_MessageLoop::TObjectGI* ProgressBar;
        std::int32_t BackgroundRestTop;
        std::int32_t ShipPanelRestTop;
        std::int32_t AnimationRestTop;
        std::int32_t AnimationTextRestTop;
        std::int32_t ProgressLabelRestTop;
        std::int32_t ProgressBarRestTop;
        GI_MessageLoop::PCallbackTimerGI ShutterTimer;
        float ShutterOpenFraction;
        GI_Panel::TPanelGI* RightShutter;
        GI_Panel::TPanelGI* LeftShutter;
        GI_Panel::TPanelGI* TopShutter;
        GI_Panel::TPanelGI* BottomShutter;
        std::uint8_t HasShutters;
        std::uint8_t cpp_padding_2[3];
        // +1 opening, -1 closing.
        std::int32_t ShutterDirection;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fPanelLoad
