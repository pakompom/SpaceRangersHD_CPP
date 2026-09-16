#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fMainForm {
    struct TfMainForm;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfMainForm : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfMainForm, GI_MessageLoop::TMessageLoopGI, "TfMainForm", 244)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void QuitClicked(GI_MessageLoop::TObjectGI* Sender);
        void NewGameClicked(GI_MessageLoop::TObjectGI* Sender);
        void AchievementsClicked(GI_MessageLoop::TObjectGI* Sender);
        void LoadGameClicked(GI_MessageLoop::TObjectGI* Sender);
        void SettingsClicked(GI_MessageLoop::TObjectGI* Sender);
        void ScoresClicked(GI_MessageLoop::TObjectGI* Sender);
        void AboutClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void MainPanelMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MenuShipAnimationFinished(GI_MessageLoop::TObjectGI* Sender);
        void GaalShipAnimationFinished(GI_MessageLoop::TObjectGI* Sender);
        void ScrollBackground(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void ClosePopup();
        void RobotBattleClicked(GI_MessageLoop::TObjectGI* Sender);
        void TextQuestClicked(GI_MessageLoop::TObjectGI* Sender);
        void ArcadeBattleClicked(GI_MessageLoop::TObjectGI* Sender);
        void ModsClicked(GI_MessageLoop::TObjectGI* Sender);
        void SelectMusic() override;
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        GI_MessageLoop::PCallbackTimerGI BackgroundTimer;
        std::int32_t BackgroundScrollOffset;
        std::int32_t LastMenuShipAnimation;
        std::int32_t LastGaalShipAnimation;
        // Cleared on open; other use remains unresolved.
        pas::WideString MenuTextState;
        std::uint8_t cpp_padding[8];
        // Cleared when closing PanelAB.
        std::int32_t PopupState;
        fPanelLoad::TfPanelLoad* LoadPanel;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fMainForm
