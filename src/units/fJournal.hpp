#pragma once
#include "types/fJournal.hpp"

namespace GI_MessageLoop {
    struct TMessageLoopGI;

} // namespace GI_MessageLoop

namespace fJournal {
    extern pas::Array<pas::WideString, 0, 19> TelevisionClipNames;

    // The native sum loop tests index < 20; the selection loop stops before index 19.
    // Selection uses each weight plus one.
    extern pas::Array<std::int32_t, 0, 19> TelevisionClipWeights;

    std::uint8_t RunJournal(GI_MessageLoop::TMessageLoopGI* ParentLoop);

    void TfJournal_Create(TfJournal* Self);

    void TfJournal_Destroy(TfJournal* Self);

} // namespace fJournal
