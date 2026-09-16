#pragma once
#include "units/EC_Mem.hpp"

namespace EC_Mem {
    // These stack-ABI accessors are handwritten assembly in the native unit.
    inline void* AddPointerOffset(void* Data, std::int32_t ByteOffset) {
        return pas::byte_offset(Data, ByteOffset);
    }

    inline void WriteByteEC(void* Dest, std::uint8_t Value) {
        pas::store_unaligned(Dest, Value);
    }

    inline void WriteWordEC(void* Dest, std::uint16_t Value) {
        pas::store_unaligned(Dest, Value);
    }

    inline void WriteIntegerEC(void* Dest, std::int32_t Value) {
        pas::store_unaligned(Dest, Value);
    }

    inline void WriteInt32EC(void* Dest, std::int32_t Value) {
        pas::store_unaligned(Dest, Value);
    }

    inline void WriteSingleEC(void* Dest, float Value) {
        pas::store_unaligned(Dest, Value);
    }

    inline void WriteDoubleEC(void* Dest, double Value) {
        pas::store_unaligned(Dest, Value);
    }

    inline std::uint8_t ReadByteEC(void* Source) {
        return pas::load_unaligned<std::uint8_t>(Source);
    }

    inline char16_t ReadWideCharEC(void* Source) {
        return pas::load_unaligned<char16_t>(Source);
    }

    inline std::uint16_t ReadWordEC(void* Source) {
        return pas::load_unaligned<std::uint16_t>(Source);
    }

    inline std::uint32_t ReadDWordEC(void* Source) {
        return pas::load_unaligned<std::uint32_t>(Source);
    }

    inline std::int32_t ReadIntegerEC(void* Source) {
        return pas::load_unaligned<std::int32_t>(Source);
    }

    inline float ReadSingleEC(void* Source) {
        return pas::load_unaligned<float>(Source);
    }

    inline double ReadDoubleEC(void* Source) {
        return pas::load_unaligned<double>(Source);
    }

} // namespace EC_Mem
