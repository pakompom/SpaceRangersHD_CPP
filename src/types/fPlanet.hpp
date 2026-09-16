#pragma once
#include "runtime_support.hpp"
#include "types/fPanelMain.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fPanelPlanet {
    struct TfPanelPlanet;

} // namespace fPanelPlanet

namespace fPlanet {
    struct TfPlanet;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPlanet : fPanelMain::TMessageLoopGIWithMainPanel {
        PAS_CLASS_META(TfPlanet, fPanelMain::TMessageLoopGIWithMainPanel, "TfPlanet", 224)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void RefreshPlanetInfo();
        void StartTextQuest(GI_MessageLoop::TObjectGI* Sender);
        void RefreshTextQuestPrompt();
        void EndTurnClicked(GI_MessageLoop::TObjectGI* Sender);
        void MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) override;
        void SelectMusic() override;
        fPanelPlanet::TfPanelPlanet* PlanetPanel;
        fPanelLoad::TfPanelLoad* LoadPanel;
        std::uint8_t cpp_padding[4];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fPlanet
