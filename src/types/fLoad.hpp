#pragma once
#include "runtime_support.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace fPanelLoad {
    struct TfPanelLoad;

} // namespace fPanelLoad

namespace fLoad {
    struct TCacheLoader;

    struct TfLoad;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCacheLoader : EC_Thread::TThreadEC {
        PAS_CLASS_META(TCacheLoader, EC_Thread::TThreadEC, "TCacheLoader", 56)
        void virtual_TThreadEC_Execute() override;
        void SetPendingLoads(pas::List* Loads, std::uint8_t StartImmediately);
        pas::List* PendingLoads;
        std::int32_t TotalLoadCount;
        std::int32_t CompletedLoadCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TfLoad : GI_MessageLoop::TMessageLoopGI {
        PAS_CLASS_META(TfLoad, GI_MessageLoop::TMessageLoopGI, "TfLoad", 264)
        void p_destroy() override;
        void InitializeLayout() override;
        void OnOpen() override;
        void OnClose() override;
        void UpdateLoadingProgress(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        void IntroMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point);
        void IntroKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key);
        void StartIntroItem(std::int32_t Index);
        void UpdateIntro(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData);
        GI_MessageLoop::PCallbackTimerGI ProgressTimer;
        float LoadProgress;
        float DisplayedProgress;
        std::uint8_t LoadingFinished;
        std::uint8_t cpp_padding[3];
        std::int32_t IntroSkipRequest;
        GI_MessageLoop::PCallbackTimerGI IntroTimer;
        std::uint32_t IntroStartedAt;
        EC_BlockPar::TBlockParEC* IntroConfig;
        std::int32_t IntroItemIndex;
        std::int32_t IntroVideoFrameCount;
        std::int32_t IntroDurationMs;
        std::int32_t IntroImageKind;
        fPanelLoad::TfPanelLoad* LoadPanel;
        std::int32_t BackgroundStyle;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace fLoad
