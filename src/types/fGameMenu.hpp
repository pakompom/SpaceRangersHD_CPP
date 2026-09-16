#pragma once
#include "runtime_support.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace fGameMenu {
    struct TfGameMenu;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfGameMenu : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfGameMenu, GI_MessageLoop::TMessageLoopGI, "TfGameMenu", 208)
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void ResumeClicked(GI_MessageLoop::TObjectGI* Sender);
        void SaveClicked(GI_MessageLoop::TObjectGI* Sender);
        void LoadClicked(GI_MessageLoop::TObjectGI* Sender);
        void SettingsClicked(GI_MessageLoop::TObjectGI* Sender);
        static void HelpClicked(GI_MessageLoop::TObjectGI* Sender);
        void ExitClicked(GI_MessageLoop::TObjectGI* Sender);
        void AchievementsClicked(GI_MessageLoop::TObjectGI* Sender);
        void BackgroundMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        void SelectMusic() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGameMenu
