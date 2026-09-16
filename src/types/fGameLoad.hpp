#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_MessageLoop.hpp"

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fGameLoad {
    struct TThreadGameLoad;

    struct TfGameLoad;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfGameLoad : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfGameLoad, GI_MessageLoop::TMessageLoopGI, "TfGameLoad", 236)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        std::uint8_t IsLoading();
        void UpdateLoadingProgress(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void SelectMusic() override;
        TThreadGameLoad* LoadThread;
        GI_MessageLoop::PCallbackTimerGI ProgressTimer;
        std::uint8_t AssetPreloadStarted;
        std::uint8_t cpp_padding[3];
        float TargetProgress;
        float DisplayedProgress;
        std::uint8_t LoadingComplete;
        std::uint8_t cpp_padding_2[3];
        fPanelLoad::TfPanelLoad* LoadPanel;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TThreadGameLoad : EC_Thread::TThreadEC {
        PAS_CLASS_META(TThreadGameLoad, EC_Thread::TThreadEC, "TThreadGameLoad", 48)
        void virtual_TThreadEC_Execute() override;
        std::uint8_t Succeeded;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fGameLoad
