#pragma once
#include "runtime_support.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace GR_Main {
    struct TCCSnapshot;

    struct TMemoryStatusEx;

    struct TDisplayModeGR;

    struct TCursorUnit;

    struct TCCInterface;

    using PCCSnapshot = TCCSnapshot*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCCInterface : pas::Object {
        PAS_CLASS_META(TCCInterface, pas::Object, "TCCInterface", 16)
        void p_destroy() override;
        void Reset();
        PCCSnapshot GetSnapshot();
        static PCCSnapshot CreateDecoy();
        static PCCSnapshot CreateEmptySnapshot();
        static PCCSnapshot CopySnapshot(PCCSnapshot Source);
        void CommitSnapshot(PCCSnapshot Snapshot);
        void ClearSnapshots();
        std::uint8_t GetResourceChecksumFailed();
        void SetResourceChecksumFailed(std::uint8_t Value);
        std::uint8_t GetTamperDetected();
        void SetTamperDetected(std::uint8_t Value);
        std::uint8_t GetFlag0A();
        void SetFlag0A(std::uint8_t Value);
        std::uint8_t GetEditableStateApplied();
        void SetEditableStateApplied(std::uint8_t Value);
        std::int32_t GetProtectedStateXorSeed();
        void SetProtectedStateXorSeed(std::int32_t Value);
        std::int32_t GetValue10();
        void SetValue10(std::int32_t Value);
        std::int32_t GetIntegrityStatus();
        void SetIntegrityStatus(std::int32_t Value);
        std::int32_t GetIntegrityError();
        void SetIntegrityError(std::int32_t Value);
        std::uint32_t GetIntegrityChecksum();
        void SetIntegrityChecksum(std::uint32_t Value);
        std::uint32_t GetIntegrityChecksum1();
        void SetIntegrityChecksum1(std::uint32_t Value);
        std::uint32_t GetIntegrityChecksum2();
        void SetIntegrityChecksum2(std::uint32_t Value);
        std::int32_t GetEncodedCheatPoints();
        void SetEncodedCheatPoints(std::int32_t Value);
        EC_Buf::TBufEC* Buffer;
        PCCSnapshot SnapshotHead;
        pas::CriticalSection* Lock;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TCCSnapshot {
        PCCSnapshot Prev;
        PCCSnapshot Next;
        std::uint8_t ResourceChecksumFailed;
        std::uint8_t TamperDetected;
        std::uint8_t Flag0A;
        std::uint8_t cpp_padding[1];
        std::int32_t ProtectedStateXorSeed;
        std::int32_t Value10;
        std::int32_t IntegrityStatus;
        std::int32_t IntegrityError;
        std::uint32_t IntegrityChecksum;
        std::uint32_t IntegrityChecksum1;
        std::uint32_t IntegrityChecksum2;
        std::int32_t EncodedCheatPoints;
        std::uint8_t EditableStateApplied;
        std::uint8_t cpp_padding_2[3];
    };
    #pragma pack(pop)

    using TDebugKeyCallbackGR = pas::Proc<void(std::uint16_t)>;

    using TRuntimeCallbackGR = pas::Proc<void()>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCursorUnit : pas::Object {
        PAS_CLASS_META(TCursorUnit, pas::Object, "TCursorUnit", 28)
        TCursorUnit* Prev;
        TCursorUnit* Next;
        pas::WideString Name;
        pas::WideString ImagePath;
        WindowsSdk::TPoint HotSpot;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TWindowMessageCallbackGR = pas::Method<void(std::uint32_t, std::uint32_t, std::int32_t)>;

    #pragma pack(push, 1)
    struct TMemoryStatusEx {
        std::uint32_t Length;
        std::uint32_t MemoryLoad;
        std::uint64_t TotalPhys;
        std::uint64_t AvailPhys;
        std::uint64_t TotalPageFile;
        std::uint64_t AvailPageFile;
        std::uint64_t TotalVirtual;
        std::uint64_t AvailVirtual;
        std::uint64_t AvailExtendedVirtual;
    };
    #pragma pack(pop)

    using TBlendPixel16 = pas::Proc<void(void*, std::uint16_t, std::uint8_t)>;

    #pragma pack(push, 1)
    struct TDisplayModeGR {
        std::uint32_t Width;
        std::uint32_t Height;
        std::uint32_t RefreshRate;
        std::uint32_t Format;
    };
    #pragma pack(pop)

    using TTriangleRasterizer16 = pas::Proc<void(void*, std::int32_t, std::int32_t, std::int32_t, std::uint32_t, std::int32_t, std::int32_t, std::uint32_t, std::int32_t, std::int32_t, std::uint32_t, WindowsSdk::PRect)>;

    using TLineRasterizer16 = pas::Proc<void(void*, std::int32_t, std::int32_t, std::int32_t, std::uint32_t, std::int32_t, std::int32_t, std::uint32_t)>;

} // namespace GR_Main
