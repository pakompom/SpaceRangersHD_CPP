#pragma once
#include "types/GI_MessageLoop.hpp"
#include "types/Windows_group.hpp"

namespace Types {
    struct TPoint;

} // namespace Types

namespace GI_MessageLoop {
    extern pas::List* MessageLoopStack;

    extern std::uint8_t IgnoreWarpMouseMove;

    extern WindowsSdk::TPoint LastMousePosition;

    void PushMessageLoop(TMessageLoopGI* Loop);

    void PopMessageLoop(TMessageLoopGI* Loop);

    void TObjectGI_Create(TObjectGI* Self, TObjectGI* Owner);

    void TObjectGI_Destroy(TObjectGI* Self);

    void TFormSoundGroup_Create(TFormSoundGroup* Self);

    void TFormSoundGroup_Destroy(TFormSoundGroup* Self);

    void TMessageLoopGI_Create(TMessageLoopGI* Self);

    void TMessageLoopGI_Destroy(TMessageLoopGI* Self);

    void TMessageLoopGI_RestoreSavedPixels16(TMessageLoopGI* Self);

    inline void TMessageLoopGI_ProcessWindowMessage(GI_MessageLoop::TMessageLoopGI* Self, std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam);

} // namespace GI_MessageLoop

#include "inline/GI_MessageLoop.hpp"
