#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_File {
    struct TFileEC;

} // namespace EC_File

namespace EC_Data {
    struct TDataEC;

    struct TDataFileEC;

    struct TDataElEC;

    struct tclist;

    using PDataFileEC = TDataFileEC**;

    enum TDataEntryKind : std::uint32_t {
        dekFile = 1,
        dekSubtree = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TDataEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TDataEC, EC_Struct::TObjectEx, "TDataEC", 44)
        void p_destroy() override;
        std::uint8_t IsEmpty();
        void Clear();
        TDataElEC* AddEntry(TDataEntryKind EntryKind);
        TDataElEC* FindIndexedEntry(const pas::WideString& Name);
        std::int32_t FindInsertionIndex(TDataElEC* Entry);
        void InsertIntoIndex(TDataElEC* Entry);
        void RebuildIndex();
        TDataFileEC* InternFileName(const pas::WideString& FileName);
        TDataElEC* FindEntry(const pas::WideString& Name);
        TDataElEC* FindEntryByPath(const pas::WideString& Path);
        void ReadEntryBuffer(TDataElEC* Entry, EC_Buf::TBufEC* Dest);
        TDataEC* GetData(const pas::WideString& Name);
        void ReadBufferByPath(const pas::WideString& Path, EC_Buf::TBufEC* Dest);
        std::uint8_t FileExistsByPath(const pas::WideString& Path);
        void AddMissingFromBlock(EC_BlockPar::TBlockParEC* Block);
        void WriteToBlock(EC_BlockPar::TBlockParEC* Block);
        void MergeFrom(TDataEC* Source);
        void LoadFromDecodedBuffer(EC_Buf::TBufEC* Buf);
        void LoadFromEncryptedDatFile(const pas::WideString& FileName);
        pas::CriticalSection* FileLock;
        std::uint8_t SharesInternedFileList;
        std::uint8_t cpp_padding[3];
        PDataFileEC InternedFileListHeadRef;
        PDataFileEC InternedFileListTailRef;
        TDataFileEC* OwnedInternedFileListHead;
        TDataFileEC* OwnedInternedFileListTail;
        TDataElEC* FirstEntry;
        TDataElEC* LastEntry;
        pas::DynArray<TDataElEC*> IndexedEntries;
        std::int32_t IndexedEntryCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TDataFileEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TDataFileEC, EC_Struct::TObjectEx, "TDataFileEC", 16)
        void p_destroy() override;
        static void Clear();
        TDataFileEC* Prev;
        TDataFileEC* Next;
        EC_File::TFileEC* FileRef;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TDataElEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TDataElEC, EC_Struct::TObjectEx, "TDataElEC", 36)
        void p_destroy() override;
        void ClearChildData();
        TDataElEC* Prev;
        TDataElEC* Next;
        pas::WideString Name;
        TDataEntryKind Kind;
        TDataEC* ChildData;
        TDataFileEC* SharedFileRef;
        std::uint32_t FileOffset;
        std::int32_t ByteCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    struct tclist {
        std::uint32_t NameCrc;
        std::uint32_t FileCrc;
        pas::WideString EncodedName;
    };

    using TResourceChecksumTable = pas::Array<tclist, 0, 1023>;

} // namespace EC_Data
