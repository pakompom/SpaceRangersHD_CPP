#pragma once
#include "types/GI_GAI.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GR_GraphBuf {
    struct TGraphBufGR;

} // namespace GR_GraphBuf

namespace GI_GAI {
    // Frame zero with a 50 ms delay.
    extern const pas::WideString SingleFrameAnimationSpec;

    extern std::uint32_t GaiFrameHeap;

    void LoadGaiFrameToGraphBuf(const pas::WideString& Path, GR_GraphBuf::TGraphBufGR* GraphBuf, std::uint32_t Seed);

    void TgaiGI_Create(TgaiGI* Self, GI_MessageLoop::TObjectGI* Owner);

    void TgaiGI_Destroy(TgaiGI* Self);

    void TgaiGI_LoadFromConfigPath(TgaiGI* Self, const pas::WideString& Path);

} // namespace GI_GAI
