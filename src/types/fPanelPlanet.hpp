#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

    struct TObjectGI;

} // namespace GI_MessageLoop

namespace fPanelPlanet {
    struct TfPanelPlanet;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPanelPlanet : EC_Struct::TObjectEx {
        PAS_CLASS_META(TfPanelPlanet, EC_Struct::TObjectEx, "TfPanelPlanet", 8)
        void p_destroy() override;
        void InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen);
        static void OnOpen();
        static void OnClose();
        void Show();
        void Hide();
        void HangarClicked(GI_MessageLoop::TObjectGI* Sender);
        void EquipmentShopClicked(GI_MessageLoop::TObjectGI* Sender);
        void GoodsShopClicked(GI_MessageLoop::TObjectGI* Sender);
        void GovernmentClicked(GI_MessageLoop::TObjectGI* Sender);
        void InformationClicked(GI_MessageLoop::TObjectGI* Sender);
        void PlanetClicked(GI_MessageLoop::TObjectGI* Sender);
        void ProcessKeyDown(std::int32_t Key);
        GI_MessageLoop::TMessageLoopGI* Screen;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fPanelPlanet
