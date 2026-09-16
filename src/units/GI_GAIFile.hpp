#pragma once
#include "types/GI_GAIFile.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_GAIFile {
    void TGAIFileThreadGI_Execute(TGAIFileThreadGI* Self);

    void TGAIFileGI_Create(TGAIFileGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TGAIFileGI_Destroy(TGAIFileGI* Self);

} // namespace GI_GAIFile
