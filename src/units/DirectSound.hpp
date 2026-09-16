#pragma once
#include "types/DirectSound.hpp"
#include "types/System.hpp"

namespace DirectSound {
    extern DirectSound::TDirectSoundCreate DirectSoundCreate;

    extern DirectSound::TDirectSoundEnumerate DirectSoundEnumerate;

    std::int32_t PAS_STDCALL DirectSoundEnumerateA(TDSEnumCallback Callback, void* Context);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_GetCurrentPosition(pas::ComView<IDirectSoundBuffer_Tag> Self, System::PCardinal PlayCursor, System::PCardinal WriteCursor);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_GetFormat(pas::ComView<IDirectSoundBuffer_Tag> Self, void* Format, std::uint32_t Size, System::PCardinal Written);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_GetStatus(pas::ComView<IDirectSoundBuffer_Tag> Self, std::uint32_t& Status);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Lock(pas::ComView<IDirectSoundBuffer_Tag> Self, std::uint32_t Offset, std::uint32_t Bytes, System::PPointer Audio1, System::PCardinal Bytes1, System::PPointer Audio2, System::PCardinal Bytes2, std::uint32_t Flags);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Play(pas::ComView<IDirectSoundBuffer_Tag> Self, std::uint32_t Reserved1, std::uint32_t Reserved2, std::uint32_t Flags);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_SetFormat(pas::ComView<IDirectSoundBuffer_Tag> Self, void* Format);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_SetVolume(pas::ComView<IDirectSoundBuffer_Tag> Self, std::int32_t Volume);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_SetPan(pas::ComView<IDirectSoundBuffer_Tag> Self, std::int32_t Pan);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Stop(pas::ComView<IDirectSoundBuffer_Tag> Self);

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Unlock(pas::ComView<IDirectSoundBuffer_Tag> Self, void* Audio1, std::uint32_t Bytes1, void* Audio2, std::uint32_t Bytes2);

    inline std::int32_t PAS_STDCALL IDirectSoundNotify_SetNotificationPositions(pas::ComView<IDirectSoundNotify_Tag> Self, std::uint32_t Count, PDSPositionNotify Positions);

    inline std::int32_t PAS_STDCALL IDirectSound_CreateSoundBuffer(pas::ComView<IDirectSound_Tag> Self, const TDSBufferDesc& Desc, IDirectSoundBuffer& Buffer, pas::ComView<System::IInterface_Tag> Outer);

    inline std::int32_t PAS_STDCALL IDirectSound_SetCooperativeLevel(pas::ComView<IDirectSound_Tag> Self, std::uint32_t Window, std::uint32_t Level);

} // namespace DirectSound

#include "inline/DirectSound.hpp"
