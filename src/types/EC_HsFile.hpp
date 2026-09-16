#pragma once
#include "runtime_support.hpp"

namespace EC_HsFile {
    struct TPackEntryEC;

    struct THsFolderEC;

    struct THashSlotEC;

    struct TPackOpenSlotEC;

    struct TPackFileEC;

    struct THashEC;

    struct TPackCollectionEC;

    using TPackFileArray = pas::Array<TPackFileEC*, 0, 127>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPackCollectionEC : pas::Object {
        PAS_CLASS_META(TPackCollectionEC, pas::Object, "TPackCollectionEC", 532)
        void p_destroy() override;
        // Frees the name hash even when FreePacks is false.
        void Clear(std::uint8_t FreePacks);
        // List mutations rebuild PackByIndex and CollectionIndex without clearing the name hash.
        // The fixed array's 128-package capacity is not checked.
        void AddPackToFront(TPackFileEC* Pack);
        void AddPackToBack(TPackFileEC* Pack);
        // When retained, Pack keeps its old links and CollectionIndex.
        void RemovePack(TPackFileEC* Pack, std::uint8_t FreePack);
        // A false result rolls back previously opened packages.
        std::uint8_t OpenAllPackages();
        // Returns true regardless of individual close results.
        std::uint8_t CloseAllPackages();
        // Returns nil when out of range.
        TPackFileEC* GetPackByIndex(std::int32_t PackIndex);
        // Returns package index * 16 + slot, or -1.
        std::int32_t OpenEntryByPathAcrossPackages(pas::AnsiString EntryPath, std::uint32_t DesiredAccess, std::uint8_t FirstPackageOnly);
        // Uses the first package; truncates existing files. Returns a handle or -1.
        std::int32_t CreateLooseFile(pas::WideString FilePath);
        std::uint8_t CloseEntryHandle(std::int32_t Handle);
        std::uint8_t ReadEntryHandle(std::int32_t Handle, void* Buffer, std::uint32_t ByteCount);
        std::uint8_t WriteEntryHandle(std::int32_t Handle, void* Buffer, std::uint32_t ByteCount);
        std::uint8_t SeekEntryHandle(std::int32_t Handle, std::uint32_t Offset, std::int32_t Origin);
        // Returns 0xFFFFFFFF for an invalid handle.
        std::uint32_t GetEntryHandlePosition(std::int32_t Handle);
        // Returns 0xFFFFFFFF for an invalid handle.
        std::uint32_t GetEntryHandleSize(std::int32_t Handle);
        TPackFileEC* FirstPack;
        TPackFileEC* LastPack;
        THashEC* NameToPackIndexHash;
        std::uint8_t UseFastNameIndex;
        std::uint8_t cpp_padding[3];
        TPackFileArray PackByIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPackOpenSlotEC {
        std::uint32_t FileHandle;
        std::uint8_t IsAvailable;
        std::uint32_t DataStartOffset;
        std::uint32_t CurrentDataOffset;
        std::uint32_t DataSize;
        void* CompressedBlockBuffer;
        void* DecompressedBlockBuffer;
        std::uint8_t UsesChainedBlocks;
        std::int32_t CurrentBlockIndex;
    };
    #pragma pack(pop)

    using TPackOpenSlotArray = pas::Array<TPackOpenSlotEC, 0, 15>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPackFileEC : pas::Object {
        PAS_CLASS_META(TPackFileEC, pas::Object, "TPackFileEC", 528)
        void p_destroy() override;
        // Does not close an open package.
        void SetPackagePath(pas::AnsiString NewPackagePath);
        void CloseAllOpenEntrySlots();
        // Opens the package read/write; loose-file mode creates an empty root folder.
        std::uint8_t Open();
        // Invalidates all open slots; returns false when already closed.
        std::uint8_t Close();
        std::uint8_t CloseForDestroy();
        // Returns -1 when all sixteen slots are occupied.
        std::int32_t FindFreeOpenSlotIndex();
        // Returns a slot or -1; DesiredAccess applies only to loose files.
        std::int32_t OpenEntryByPath(pas::AnsiString EntryPath, std::uint32_t DesiredAccess);
        // Creates or truncates a loose file for read/write access; returns a slot or -1.
        std::int32_t CreateLooseFile(pas::WideString FilePath);
        std::uint8_t CloseEntrySlot(std::uint32_t SlotIndex);
        // Leaves PackageHandle at the selected payload; I/O errors are unchecked.
        std::uint32_t GetChainedBlockStoredSizeAtIndex(std::uint32_t FirstBlockOffset, std::uint32_t BlockIndex);
        // Compressed reads do not enforce logical EOF or report decompressor and short-block failures.
        std::uint8_t ReadEntrySlot(std::uint32_t SlotIndex, void* Buffer, std::uint32_t ByteCount);
        // Rejects compressed entries.
        std::uint8_t WriteEntrySlot(std::uint32_t SlotIndex, void* Buffer, std::uint32_t ByteCount);
        // Origin 1 adds to the current position, 2 subtracts from size, otherwise Offset is absolute. Only compressed entries reject positions beyond DataSize.
        std::uint8_t SeekEntrySlot(std::uint32_t SlotIndex, std::uint32_t Offset, std::int32_t Origin);
        // Returns 0xFFFFFFFF for an unavailable slot or SlotIndex=0xFFFFFFFF.
        std::uint32_t GetEntrySlotPosition(std::uint32_t SlotIndex);
        // Returns 0xFFFFFFFF for an unavailable slot or SlotIndex=0xFFFFFFFF.
        std::uint32_t GetEntrySlotSize(std::uint32_t SlotIndex);
        TPackFileEC* NextPack;
        TPackFileEC* PrevPack;
        std::uint8_t UseLooseFiles;
        std::uint8_t cpp_padding[3];
        std::uint32_t PackageHandle;
        pas::AnsiString PackagePath;
        THsFolderEC* RootFolder;
        TPackOpenSlotArray OpenSlots;
        std::uint32_t RootSubtreeOffset;
        std::uint8_t cpp_padding_2[12];
        std::int32_t CollectionIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PPackEntryEC = TPackEntryEC*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct THsFolderEC : pas::Object {
        PAS_CLASS_META(THsFolderEC, pas::Object, "THsFolderEC", 36)
        void p_destroy() override;
        // Returns nil for an out-of-range index.
        PPackEntryEC GetEntry(std::uint32_t Index);
        // Uppercases EntryName and skips entries with nonzero Flags.
        PPackEntryEC FindEntry(pas::AnsiString EntryName);
        // Requires an unloaded folder.
        void InitializeEmpty();
        // Returns false when already loaded; flagged child folders are skipped.
        std::uint8_t Load(std::uint32_t FileHandle, std::uint32_t SubtreeOffset);
        // Marks this folder and its parent changed.
        void Unload();
        // Accepts slash and backslash separators; returns nil when absent.
        PPackEntryEC ResolveEntryByPath(pas::AnsiString EntryPath);
        // Invalidates the parent's stored target offset.
        void UpdateParentEntry();
        pas::AnsiString UpperName;
        pas::AnsiString OriginalName;
        std::uint32_t HeaderSize;
        std::uint32_t EntryCount;
        std::uint32_t EntryRecordSize;
        THsFolderEC* Parent;
        PPackEntryEC EntryBuffer;
        std::uint8_t ChangedFlag;
        std::uint8_t InitializedEmptyFlag;
        std::uint8_t cpp_padding[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPackEntryEC {
        std::uint32_t StoredSize;
        std::uint32_t DataSize;
        pas::Array<std::uint8_t, 0, 62> UpperName;
        pas::Array<std::uint8_t, 0, 62> OriginalName;
        // Kind 2 is compressed data; kind 3 is a child folder.
        std::int32_t Kind;
        std::int32_t KindCopy;
        std::uint32_t Flags;
        std::uint8_t cpp_padding[4];
        // The four bytes remain unresolved.
        std::uint32_t TargetOffset;
        THsFolderEC* ChildFolder;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct THashSlotEC {
        std::uint32_t FullHash;
        std::int32_t MappedValue;
        std::uint32_t HitCount;
        std::int32_t Unknown0C;
        pas::Array<std::uint8_t, 0, 31> KeySuffix;
        std::uint8_t cpp_padding[4];
    };
    #pragma pack(pop)

    using THashSlotArray = pas::Array<THashSlotEC, 0, 1023>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct THashEC : pas::Object {
        PAS_CLASS_META(THashEC, pas::Object, "THashEC", 53276)
        void p_destroy() override;
        // Key is not modified; only its trailing 32 bytes contribute to the hash.
        static std::int32_t ComputeLookupBucketAndFullHash(pas::AnsiString& Key, std::uint32_t& FullHash);
        // Returns -1 on failure. Native probing can reach slot 1024; promoted hits return the pre-swap index.
        std::int32_t FindOrInsertKeySlot(pas::AnsiString Key);
        void SetSlotMappedValue(std::int32_t SlotIndex, std::int32_t Value);
        // Ignores BucketCount; the table has 1024 buckets. Always returns true.
        std::uint8_t InitializeEmptyTable(std::int32_t BucketCount);
        // Returns true without changing the table.
        static std::uint8_t ReleaseTable();
        std::int32_t GetSlotMappedValue(std::int32_t SlotIndex);
        void MaybeResetStatistics();
        void NoteStaleMappedValue();
        std::uint32_t OperationCount;
        std::uint32_t HitCount;
        // Incremented on every hit alongside HitCount; no distinct use recovered.
        std::uint32_t HitCountCopy;
        std::uint32_t StaleValueCount;
        std::uint32_t MissCount;
        // Native THashEC cleanup owns this otherwise unused field.
        pas::AnsiString ReservedText;
        THashSlotArray Slots;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // A collection handle combines the package index and its four-bit open slot.
    inline constexpr std::int32_t PackOpenSlotShift = 4;

    inline constexpr std::int32_t PackOpenSlotCount = 1 << EC_HsFile::PackOpenSlotShift;

    inline constexpr std::int32_t PackCompressionBlockShift = 16;

    inline constexpr std::int32_t PackCompressionBlockSize = 1 << EC_HsFile::PackCompressionBlockShift;

    inline constexpr std::int32_t PackCompressedBufferSize = 72112;

} // namespace EC_HsFile
