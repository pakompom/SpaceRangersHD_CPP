#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

    struct TObjectGI;

} // namespace GI_MessageLoop

namespace fPanelRuins {
    struct TfPanelRuins;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfPanelRuins : EC_Struct::TObjectEx {
        PAS_CLASS_META(TfPanelRuins, EC_Struct::TObjectEx, "TfPanelRuins", 8)
        void p_destroy() override;
        void InitializeLayout(GI_MessageLoop::TMessageLoopGI* Screen);
        void OnOpen();
        // Native no-op lifecycle hook.
        static void OnClose();
        void Show();
        void Hide();
        void ServicesClicked(GI_MessageLoop::TObjectGI* Sender);
        void EquipmentShopClicked(GI_MessageLoop::TObjectGI* Sender);
        void GoodsShopClicked(GI_MessageLoop::TObjectGI* Sender);
        void InformationClicked(GI_MessageLoop::TObjectGI* Sender);
        void HangarClicked(GI_MessageLoop::TObjectGI* Sender);
        void TakeOffForStationTravel();
        void ProcessKeyDown(std::int32_t Key);
        GI_MessageLoop::TMessageLoopGI* Screen;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fPanelRuins
