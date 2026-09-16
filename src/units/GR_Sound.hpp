#pragma once
#include "types/GR_Sound.hpp"

namespace GR_Sound {
    std::int32_t PAS_STDCALL EnumerateSoundDevice(void* Guid, std::uint8_t* Description, std::uint8_t* Module, void* Context);

    pas::AnsiString SoundErrorText(std::int32_t Code);

    void TSoundBufferControl_Create(TSoundBufferControl* Self);

    void TSoundBufferControl_Destroy(TSoundBufferControl* Self);

    void TSoundBuffer_Create(TSoundBuffer* Self);

    void TSoundBuffer_Destroy(TSoundBuffer* Self);

    void TSoundControl_Create(TSoundControl* Self);

    void TSoundControl_Destroy(TSoundControl* Self);

} // namespace GR_Sound
