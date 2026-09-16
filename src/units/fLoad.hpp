#pragma once
#include "types/fLoad.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace fLoad {
    extern std::uint8_t IntroFinished;

    extern std::uint8_t IntroPlaying;

    void QueueCommonLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);

    void QueueSpaceLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);

    void QueueHyperspaceLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);

    void QueueArcadeLoadingAssets(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);

    void RemoveDuplicateCacheLoads(pas::List* PendingLoads);

    void QueueConfiguredLoadingAssets(pas::List* PendingLoads, pas::WideString Path);

    void LoadPendingAssets(pas::List* PendingLoads);

    void TCacheLoader_Execute(TCacheLoader* Self);

    void TfLoad_Create(TfLoad* Self);

    void TfLoad_Destroy(TfLoad* Self);

} // namespace fLoad
