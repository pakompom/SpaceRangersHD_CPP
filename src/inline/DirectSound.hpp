#pragma once
#include "types/System.hpp"
#include "units/DirectSound.hpp"

namespace DirectSound {
    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_GetCurrentPosition(pas::ComView<IDirectSoundBuffer_Tag> Self, System::PCardinal PlayCursor, System::PCardinal WriteCursor) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, System::PCardinal, System::PCardinal);
        return pas::com_method<CppMethod>(Self.get(), 4)(Self.get(), PlayCursor, WriteCursor);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_GetFormat(pas::ComView<IDirectSoundBuffer_Tag> Self, void* Format, std::uint32_t Size, System::PCardinal Written) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*, std::uint32_t, System::PCardinal);
        return pas::com_method<CppMethod>(Self.get(), 5)(Self.get(), Format, Size, Written);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_GetStatus(pas::ComView<IDirectSoundBuffer_Tag> Self, std::uint32_t& Status) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t*);
        return pas::com_method<CppMethod>(Self.get(), 9)(Self.get(), &Status);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Lock(pas::ComView<IDirectSoundBuffer_Tag> Self, std::uint32_t Offset, std::uint32_t Bytes, System::PPointer Audio1, System::PCardinal Bytes1, System::PPointer Audio2, System::PCardinal Bytes2, std::uint32_t Flags) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, System::PPointer, System::PCardinal, System::PPointer, System::PCardinal, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 11)(Self.get(), Offset, Bytes, Audio1, Bytes1, Audio2, Bytes2, Flags);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Play(pas::ComView<IDirectSoundBuffer_Tag> Self, std::uint32_t Reserved1, std::uint32_t Reserved2, std::uint32_t Flags) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 12)(Self.get(), Reserved1, Reserved2, Flags);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_SetFormat(pas::ComView<IDirectSoundBuffer_Tag> Self, void* Format) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*);
        return pas::com_method<CppMethod>(Self.get(), 14)(Self.get(), Format);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_SetVolume(pas::ComView<IDirectSoundBuffer_Tag> Self, std::int32_t Volume) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::int32_t);
        return pas::com_method<CppMethod>(Self.get(), 15)(Self.get(), Volume);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_SetPan(pas::ComView<IDirectSoundBuffer_Tag> Self, std::int32_t Pan) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::int32_t);
        return pas::com_method<CppMethod>(Self.get(), 16)(Self.get(), Pan);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Stop(pas::ComView<IDirectSoundBuffer_Tag> Self) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*);
        return pas::com_method<CppMethod>(Self.get(), 18)(Self.get());
    }

    inline std::int32_t PAS_STDCALL IDirectSoundBuffer_Unlock(pas::ComView<IDirectSoundBuffer_Tag> Self, void* Audio1, std::uint32_t Bytes1, void* Audio2, std::uint32_t Bytes2) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, void*, std::uint32_t, void*, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 19)(Self.get(), Audio1, Bytes1, Audio2, Bytes2);
    }

    inline std::int32_t PAS_STDCALL IDirectSoundNotify_SetNotificationPositions(pas::ComView<IDirectSoundNotify_Tag> Self, std::uint32_t Count, PDSPositionNotify Positions) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, PDSPositionNotify);
        return pas::com_method<CppMethod>(Self.get(), 3)(Self.get(), Count, Positions);
    }

    inline std::int32_t PAS_STDCALL IDirectSound_CreateSoundBuffer(pas::ComView<IDirectSound_Tag> Self, const TDSBufferDesc& Desc, IDirectSoundBuffer& Buffer, pas::ComView<System::IInterface_Tag> Outer) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, const TDSBufferDesc*, void**, void*);
        return pas::com_method<CppMethod>(Self.get(), 3)(Self.get(), &Desc, Buffer.address(), pas::interface_pointer(Outer));
    }

    inline std::int32_t PAS_STDCALL IDirectSound_SetCooperativeLevel(pas::ComView<IDirectSound_Tag> Self, std::uint32_t Window, std::uint32_t Level) {
        using CppMethod = std::int32_t (PAS_STDCALL *)(void*, std::uint32_t, std::uint32_t);
        return pas::com_method<CppMethod>(Self.get(), 6)(Self.get(), Window, Level);
    }

} // namespace DirectSound
