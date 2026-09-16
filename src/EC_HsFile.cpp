#include "layout/EC_HsFile.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_HsFile.hpp"
#include "units/EC_OKGF.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsImports.hpp"

namespace EC_HsFile {
    EC_HsFile::TPackCollectionEC* PackageCollection{};

    pas::CriticalSection* PackageFileLock{};

    pas::AnsiString LooseFileRoot{};

    const pas::WideString PackSlotRangeError = u"\u041d\u043e\u043c\u0435\u0440 \u0444\u0430\u0439\u043b\u0430 \u043d\u0435 \u043c\u043e\u0436\u0435\u0442 \u0431\u044b\u0442\u044c \u0431\u043e\u043b\u0435\u0435 "_w;

    // Ignores SuffixLength; compares up to 32 trailing key bytes without checking stored length. Key is not modified.
    std::uint8_t MatchLookupKeySuffix(pas::AnsiString& Key, void* SuffixBytes, std::int32_t SuffixLength) {
        std::int32_t i{};
        std::int32_t First{};
        std::int32_t KeyLength = Key.length();
        if (KeyLength > 32) {
            First = KeyLength - 31;
        } else {
            First = 1;
        }
        std::int32_t j = 0;
        std::uint8_t Result = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(First, KeyLength); cpp_range.next(i); ) {
            if (Key.read(i) != static_cast<std::uint8_t*>(SuffixBytes)[j]) {
                Result = false;
                break;
            }
            ++j;
        }
        return Result;
    }

    // Ignores SuffixLength; copies up to 32 trailing key bytes without terminator or padding. Key is not modified.
    void CopyLookupKeySuffix(void* DestSuffixBytes, std::int32_t SuffixLength, pas::AnsiString& Key) {
        std::int32_t i{};
        std::int32_t First{};
        std::int32_t KeyLength = Key.length();
        if (KeyLength > 32) {
            First = KeyLength - 31;
        } else {
            First = 1;
        }
        std::int32_t j = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(First, KeyLength); cpp_range.next(i); ) {
            static_cast<std::uint8_t*>(DestSuffixBytes)[j] = Key.read(i);
            ++j;
        }
    }

    // Returns Text when no delimiter occurs.
    pas::AnsiString AnsiBeforeFirstDelimiter(pas::AnsiString Text, pas::AnsiString Delimiters) {
        std::int32_t j{};
        std::int32_t i = 1;
        while (i <= Text.length()) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Delimiters.length()); cpp_range.next(j); ) {
                if (Delimiters.read(j) == Text.read(i)) {
                    return pas::copy(Text, 1, i - 1);
                }
            }
            ++i;
        }
        return Text;
    }

    // Returns an empty string when no delimiter occurs.
    pas::AnsiString AnsiAfterFirstDelimiter(pas::AnsiString Text, pas::AnsiString Delimiters) {
        std::int32_t j{};
        std::int32_t i = 1;
        while (i <= Text.length()) {
            for (auto cpp_range = pas::for_to<std::int32_t>(1, Delimiters.length()); cpp_range.next(j); ) {
                if (Delimiters.read(j) == Text.read(i)) {
                    return pas::copy(Text, i + 1, Text.length() - i);
                }
            }
            ++i;
        }
        return pas::AnsiString();
    }

    void* OffsetPackPointer(void* Data, std::uint32_t ByteOffset) {
        return static_cast<std::uint8_t*>(Data) + ByteOffset;
    }

    void TPackFileEC_Create(TPackFileEC* Self) {
        std::int32_t i{};
        Self->PackageHandle = WindowsImports::INVALID_HANDLE_VALUE;
        Self->UseLooseFiles = false;
        Self->PackagePath = pas::AnsiString();
        Self->RootFolder = nullptr;
        Self->RootSubtreeOffset = 0u;
        Self->NextPack = nullptr;
        Self->PrevPack = nullptr;
        Self->CollectionIndex = -1;
        for (i = 0; i <= 15; ++i) {
            Self->OpenSlots[i].IsAvailable = true;
        }
    }

    void TPackFileEC_Destroy(TPackFileEC* Self) {
        Self->CloseAllOpenEntrySlots();
        Self->CloseForDestroy();
    }

    // Does not close an open package.
    void TPackFileEC::SetPackagePath(pas::AnsiString NewPackagePath) {
        PackagePath = NewPackagePath;
    }

    void TPackFileEC::CloseAllOpenEntrySlots() {
        std::int32_t i{};
        for (i = 0; i <= 15; ++i) {
            if (!OpenSlots[i].IsAvailable) {
                CloseEntrySlot(i);
                OpenSlots[i].IsAvailable = true;
            }
        }
    }

    // Opens the package read/write; loose-file mode creates an empty root folder.
    std::uint8_t TPackFileEC::Open() {
        std::uint32_t BytesRead{};
        if (PackageHandle != WindowsImports::INVALID_HANDLE_VALUE || RootFolder != nullptr) {
            Close();
        }
        if (UseLooseFiles) {
            RootSubtreeOffset = 0u;
            RootFolder = pas::construct_call<THsFolderEC>(THsFolderEC_Create, pas::AnsiString());
            RootFolder->InitializeEmpty();
            return true;
        }
        PackageHandle = WindowsImports::CreateFileA(PackagePath.pchar(), WindowsImports::GENERIC_READ | WindowsImports::GENERIC_WRITE, WindowsImports::FILE_SHARE_READ | WindowsImports::FILE_SHARE_WRITE, nullptr, WindowsImports::OPEN_EXISTING, WindowsImports::FILE_ATTRIBUTE_NORMAL, 0u);
        if (PackageHandle == WindowsImports::INVALID_HANDLE_VALUE) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error openning package file [READ]:", PackagePath})));
        }
        if (!WindowsImports::ReadFile(PackageHandle, &RootSubtreeOffset, static_cast<std::int32_t>(sizeof(std::uint32_t)), BytesRead, nullptr)) {
            WindowsImports::CloseHandle(PackageHandle);
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error reading package file:", PackagePath})));
        }
        RootFolder = pas::construct_call<THsFolderEC>(THsFolderEC_Create, pas::AnsiString());
        if (!RootFolder->Load(PackageHandle, RootSubtreeOffset)) {
            pas::free(RootFolder);
            RootFolder = nullptr;
            Close();
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error reading file system of the package file:", PackagePath})));
        }
        return true;
    }

    // Invalidates all open slots; returns false when already closed.
    std::uint8_t TPackFileEC::Close() {
        std::uint8_t Success{};
        std::uint8_t Result = false;
        if (PackageHandle == WindowsImports::INVALID_HANDLE_VALUE && RootFolder == nullptr) {
            return Result;
        }
        CloseAllOpenEntrySlots();
        if (RootFolder != nullptr) {
            pas::free(RootFolder);
            RootFolder = nullptr;
        }
        if (PackageHandle != WindowsImports::INVALID_HANDLE_VALUE) {
            Success = WindowsImports::CloseHandle(PackageHandle) != 0;
        } else {
            Success = true;
        }
        PackageHandle = WindowsImports::INVALID_HANDLE_VALUE;
        if (Success) {
            return true;
        }
        return Result;
    }

    std::uint8_t TPackFileEC::CloseForDestroy() {
        std::uint8_t Success{};
        std::uint8_t Result = false;
        if (PackageHandle == WindowsImports::INVALID_HANDLE_VALUE && RootFolder == nullptr) {
            return Result;
        }
        CloseAllOpenEntrySlots();
        if (RootFolder != nullptr) {
            pas::free(RootFolder);
            RootFolder = nullptr;
        }
        if (PackageHandle != WindowsImports::INVALID_HANDLE_VALUE) {
            Success = WindowsImports::CloseHandle(PackageHandle) != 0;
        } else {
            Success = true;
        }
        PackageHandle = WindowsImports::INVALID_HANDLE_VALUE;
        if (Success) {
            return true;
        }
        return Result;
    }

    // Returns -1 when all sixteen slots are occupied.
    std::int32_t TPackFileEC::FindFreeOpenSlotIndex() {
        std::int32_t i{};
        for (i = 0; i <= 15; ++i) {
            if (OpenSlots[i].IsAvailable) {
                return i;
            }
        }
        return -1;
    }

    // Returns a slot or -1; DesiredAccess applies only to loose files.
    std::int32_t TPackFileEC::OpenEntryByPath(pas::AnsiString EntryPath, std::uint32_t DesiredAccess) {
        pas::AnsiString cpp_text{};
        PPackEntryEC Entry{};
        std::uint32_t Position{};
        std::int32_t Result = -1;
        std::int32_t Slot = FindFreeOpenSlotIndex();
        if (Slot == -1) {
            return Result;
        }
        if (RootFolder == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Package not opened :", EntryPath})));
        }
        if (!UseLooseFiles) {
            Entry = RootFolder->ResolveEntryByPath(EntryPath);
            if (Entry == nullptr) {
                return Result;
            }
        } else {
            if (!SysUtilsImports::FileExists(pas::concat_ansi({LooseFileRoot, EntryPath}))) {
                return Result;
            }
            OpenSlots[Slot].FileHandle = WindowsImports::CreateFileA((cpp_text = pas::concat_ansi({LooseFileRoot, EntryPath}), cpp_text.pchar()), DesiredAccess, WindowsImports::FILE_SHARE_READ, nullptr, WindowsImports::OPEN_EXISTING, WindowsImports::FILE_ATTRIBUTE_NORMAL, 0u);
            if (OpenSlots[Slot].FileHandle == WindowsImports::INVALID_HANDLE_VALUE) {
                return Result;
            }
            OpenSlots[Slot].DataStartOffset = 0u;
            OpenSlots[Slot].CurrentDataOffset = 0u;
            OpenSlots[Slot].DataSize = WindowsImports::SetFilePointer(OpenSlots[Slot].FileHandle, 0, nullptr, WindowsImports::FILE_END);
            OpenSlots[Slot].CompressedBlockBuffer = nullptr;
            OpenSlots[Slot].DecompressedBlockBuffer = nullptr;
            OpenSlots[Slot].UsesChainedBlocks = false;
            OpenSlots[Slot].CurrentBlockIndex = -1;
            if (OpenSlots[Slot].DataSize == 0xffffffffu) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"\321\341\356\351 \342 \364\340\351\353\356\342\356\351 \361\350\361\362\345\354\345 :", EntryPath})));
            }
            Position = WindowsImports::SetFilePointer(OpenSlots[Slot].FileHandle, 0, nullptr, WindowsImports::FILE_BEGIN);
            if (Position == 0xffffffffu) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"\321\341\356\351 \342 \364\340\351\353\356\342\356\351 \361\350\361\362\345\354\345:", EntryPath})));
            }
            OpenSlots[Slot].IsAvailable = false;
            return Slot;
        }
        if (PackageHandle == WindowsImports::INVALID_HANDLE_VALUE) {
            return Result;
        }
        OpenSlots[Slot].FileHandle = PackageHandle;
        OpenSlots[Slot].DataStartOffset = Entry->TargetOffset + 4;
        OpenSlots[Slot].CurrentDataOffset = Entry->TargetOffset + 4;
        OpenSlots[Slot].DataSize = Entry->DataSize;
        OpenSlots[Slot].IsAvailable = false;
        OpenSlots[Slot].UsesChainedBlocks = Entry->Kind == 2;
        OpenSlots[Slot].CurrentBlockIndex = -1;
        if (OpenSlots[Slot].UsesChainedBlocks) {
            OpenSlots[Slot].CompressedBlockBuffer = System::AllocMem(PackCompressedBufferSize);
            OpenSlots[Slot].DecompressedBlockBuffer = System::AllocMem(PackCompressionBlockSize);
        } else {
            OpenSlots[Slot].CompressedBlockBuffer = nullptr;
            OpenSlots[Slot].DecompressedBlockBuffer = nullptr;
        }
        Position = WindowsImports::SetFilePointer(OpenSlots[Slot].FileHandle, OpenSlots[Slot].CurrentDataOffset, nullptr, WindowsImports::FILE_BEGIN);
        if (Position == 0xffffffffu) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"\321\341\356\351 \342 \357\340\352\345\362\355\356\354 \364\340\351\353\345 :", PackagePath, ":", EntryPath})));
        }
        return Slot;
    }

    // Creates or truncates a loose file for read/write access; returns a slot or -1.
    std::int32_t TPackFileEC::CreateLooseFile(pas::WideString FilePath) {
        std::int32_t Result = -1;
        std::int32_t Slot = FindFreeOpenSlotIndex();
        if (Slot == -1) {
            return Result;
        }
        OpenSlots[Slot].FileHandle = WindowsImports::CreateFileW(FilePath.pchar(), WindowsImports::GENERIC_READ | WindowsImports::GENERIC_WRITE, WindowsImports::FILE_SHARE_READ, nullptr, WindowsImports::CREATE_ALWAYS, WindowsImports::FILE_ATTRIBUTE_NORMAL, 0u);
        if (OpenSlots[Slot].FileHandle == WindowsImports::INVALID_HANDLE_VALUE) {
            return Result;
        }
        OpenSlots[Slot].DataStartOffset = 0u;
        OpenSlots[Slot].CurrentDataOffset = 0u;
        OpenSlots[Slot].DataSize = 0u;
        OpenSlots[Slot].CompressedBlockBuffer = nullptr;
        OpenSlots[Slot].DecompressedBlockBuffer = nullptr;
        OpenSlots[Slot].UsesChainedBlocks = false;
        OpenSlots[Slot].CurrentBlockIndex = -1;
        OpenSlots[Slot].IsAvailable = false;
        return Slot;
    }

    std::uint8_t TPackFileEC::CloseEntrySlot(std::uint32_t SlotIndex) {
        std::uint8_t Result = false;
        if (SlotIndex == 0xffffffffu) {
            return Result;
        }
        if (SlotIndex > 15) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({PackSlotRangeError, pas::wide_int_to_str(15), u": ", pas::wide_int64_to_str(static_cast<std::int64_t>(SlotIndex))}))));
        }
        if (OpenSlots[SlotIndex].IsAvailable) {
            return Result;
        }
        Result = true;
        if (OpenSlots[SlotIndex].FileHandle == PackageHandle) {
            if (OpenSlots[SlotIndex].UsesChainedBlocks) {
                pas::free_mem(OpenSlots[SlotIndex].CompressedBlockBuffer);
                OpenSlots[SlotIndex].CompressedBlockBuffer = nullptr;
                pas::free_mem(OpenSlots[SlotIndex].DecompressedBlockBuffer);
                OpenSlots[SlotIndex].DecompressedBlockBuffer = nullptr;
            }
            OpenSlots[SlotIndex].IsAvailable = true;
        } else {
            if (!(WindowsImports::CloseHandle(OpenSlots[SlotIndex].FileHandle) != 0)) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"\316\370\350\341\352\340 \347\340\352\360\373\362\350\377 \364\340\351\353\340 : ", SysUtils::Int64ToStr(SlotIndex)})));
            }
            if (OpenSlots[SlotIndex].UsesChainedBlocks) {
                pas::free_mem(OpenSlots[SlotIndex].CompressedBlockBuffer);
                OpenSlots[SlotIndex].CompressedBlockBuffer = nullptr;
                pas::free_mem(OpenSlots[SlotIndex].DecompressedBlockBuffer);
                OpenSlots[SlotIndex].DecompressedBlockBuffer = nullptr;
            }
            OpenSlots[SlotIndex].IsAvailable = true;
        }
        return Result;
    }

    // Leaves PackageHandle at the selected payload; I/O errors are unchecked.
    std::uint32_t TPackFileEC::GetChainedBlockStoredSizeAtIndex(std::uint32_t FirstBlockOffset, std::uint32_t BlockIndex) {
        std::uint32_t BytesRead{};
        std::uint32_t StoredSize{};
        std::uint32_t Offset = FirstBlockOffset;
        while (true) {
            WindowsImports::SetFilePointer(PackageHandle, Offset, nullptr, WindowsImports::FILE_BEGIN);
            WindowsImports::ReadFile(PackageHandle, &StoredSize, static_cast<std::int32_t>(sizeof(std::uint32_t)), BytesRead, nullptr);
            if (BlockIndex == 0) {
                break;
            }
            --BlockIndex;
            Offset = Offset + StoredSize + static_cast<std::int32_t>(sizeof(std::uint32_t));
        }
        return StoredSize;
    }

    // Compressed reads do not enforce logical EOF or report decompressor and short-block failures.
    std::uint8_t TPackFileEC::ReadEntrySlot(std::uint32_t SlotIndex, void* Buffer, std::uint32_t ByteCount) {
        std::uint32_t BytesRead{};
        std::uint32_t BlockIndex{};
        std::uint32_t BlockOffset{};
        std::uint32_t ChunkSize{};
        std::uint32_t StoredSize{};
        std::uint32_t RelativeOffset{};
        void* Decoded{};
        void* Dest{};
        std::uint8_t Result = false;
        if (SlotIndex == 0xffffffffu) {
            return Result;
        }
        if (SlotIndex > 15) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({PackSlotRangeError, pas::wide_int_to_str(15), u": ", pas::wide_int64_to_str(static_cast<std::int64_t>(SlotIndex))}))));
        }
        if (OpenSlots[SlotIndex].IsAvailable) {
            return Result;
        }
        if (OpenSlots[SlotIndex].UsesChainedBlocks) {
            Decoded = OpenSlots[SlotIndex].DecompressedBlockBuffer;
            Dest = Buffer;
            while (ByteCount != 0) {
                RelativeOffset = OpenSlots[SlotIndex].CurrentDataOffset - OpenSlots[SlotIndex].DataStartOffset;
                BlockIndex = pas::shr(RelativeOffset, PackCompressionBlockShift);
                BlockOffset = RelativeOffset - BlockIndex * PackCompressionBlockSize;
                ChunkSize = ByteCount;
                if (PackCompressionBlockSize - BlockOffset < ChunkSize) {
                    ChunkSize = PackCompressionBlockSize - BlockOffset;
                }
                if (OpenSlots[SlotIndex].CurrentBlockIndex != static_cast<std::int32_t>(BlockIndex)) {
                    StoredSize = GetChainedBlockStoredSizeAtIndex(OpenSlots[SlotIndex].DataStartOffset, BlockIndex);
                    Result = WindowsImports::ReadFile(PackageHandle, OpenSlots[SlotIndex].CompressedBlockBuffer, StoredSize, BytesRead, nullptr) != 0;
                    if (!Result) {
                        return Result;
                    }
                    EC_OKGF::OKGF_ZLib_UnCompress2(OpenSlots[SlotIndex].DecompressedBlockBuffer, PackCompressionBlockSize, OpenSlots[SlotIndex].CompressedBlockBuffer, StoredSize);
                    OpenSlots[SlotIndex].CurrentBlockIndex = BlockIndex;
                }
                pas::move_memory(EC_HsFile::OffsetPackPointer(Decoded, BlockOffset), Dest, static_cast<std::int32_t>(ChunkSize));
                Dest = EC_HsFile::OffsetPackPointer(Dest, ChunkSize);
                ByteCount -= ChunkSize;
                OpenSlots[SlotIndex].CurrentDataOffset += ChunkSize;
            }
            return true;
        }
        WindowsImports::SetFilePointer(OpenSlots[SlotIndex].FileHandle, OpenSlots[SlotIndex].CurrentDataOffset, nullptr, WindowsImports::FILE_BEGIN);
        Result = WindowsImports::ReadFile(OpenSlots[SlotIndex].FileHandle, Buffer, ByteCount, BytesRead, nullptr) != 0;
        Result = Result && ByteCount == BytesRead;
        OpenSlots[SlotIndex].CurrentDataOffset += BytesRead;
        return Result;
    }

    // Rejects compressed entries.
    std::uint8_t TPackFileEC::WriteEntrySlot(std::uint32_t SlotIndex, void* Buffer, std::uint32_t ByteCount) {
        std::uint32_t BytesWritten{};
        std::uint8_t Result = false;
        if (SlotIndex == 0xffffffffu) {
            return Result;
        }
        if (SlotIndex > 15) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({PackSlotRangeError, pas::wide_int_to_str(15), u": ", pas::wide_int64_to_str(static_cast<std::int64_t>(SlotIndex))}))));
        }
        if (OpenSlots[SlotIndex].IsAvailable) {
            return Result;
        }
        if (OpenSlots[SlotIndex].UsesChainedBlocks) {
            pas::raise(pas::make_exception<pas::Exception>("\316\370\350\341\356\367\355\340\377 \356\357\345\360\340\366\350\377 \347\340\357\350\361\350 \342 \361\346\340\362\373\351 \364\340\351\353"_a));
        }
        WindowsImports::SetFilePointer(OpenSlots[SlotIndex].FileHandle, OpenSlots[SlotIndex].CurrentDataOffset, nullptr, WindowsImports::FILE_BEGIN);
        Result = WindowsImports::WriteFile(OpenSlots[SlotIndex].FileHandle, Buffer, ByteCount, BytesWritten, nullptr) != 0;
        Result = Result && ByteCount == BytesWritten;
        OpenSlots[SlotIndex].CurrentDataOffset += BytesWritten;
        std::uint32_t Size = OpenSlots[SlotIndex].CurrentDataOffset - OpenSlots[SlotIndex].DataStartOffset;
        if (Size > OpenSlots[SlotIndex].DataSize) {
            OpenSlots[SlotIndex].DataSize = Size;
        }
        return Result;
    }

    // Origin 1 adds to the current position, 2 subtracts from size, otherwise Offset is absolute. Only compressed entries reject positions beyond DataSize.
    std::uint8_t TPackFileEC::SeekEntrySlot(std::uint32_t SlotIndex, std::uint32_t Offset, std::int32_t Origin) {
        std::uint32_t Position{};
        std::uint32_t BlockIndex{};
        std::uint8_t Result = false;
        if (SlotIndex == 0xffffffffu) {
            return Result;
        }
        if (SlotIndex > 15) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({PackSlotRangeError, pas::wide_int_to_str(15), u": ", pas::wide_int64_to_str(static_cast<std::int64_t>(SlotIndex))}))));
        }
        if (OpenSlots[SlotIndex].IsAvailable) {
            return Result;
        }
        if (Origin == WindowsImports::FILE_CURRENT) {
            Offset = OpenSlots[SlotIndex].CurrentDataOffset + Offset - OpenSlots[SlotIndex].DataStartOffset;
        } else if (Origin == WindowsImports::FILE_END) {
            Offset = OpenSlots[SlotIndex].DataSize - Offset;
        }
        if (OpenSlots[SlotIndex].UsesChainedBlocks) {
            if (Offset > OpenSlots[SlotIndex].DataSize) {
                return Result;
            }
            BlockIndex = pas::shr(Offset, PackCompressionBlockShift);
            if (OpenSlots[SlotIndex].CurrentBlockIndex != static_cast<std::int32_t>(BlockIndex)) {
                OpenSlots[SlotIndex].CurrentBlockIndex = -1;
            }
            OpenSlots[SlotIndex].CurrentDataOffset = OpenSlots[SlotIndex].DataStartOffset + Offset;
        } else {
            Position = WindowsImports::SetFilePointer(OpenSlots[SlotIndex].FileHandle, OpenSlots[SlotIndex].DataStartOffset + Offset, nullptr, WindowsImports::FILE_BEGIN);
            if (Position == 0xffffffffu) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"\316\370\350\341\352\340 \363\361\362\340\355\356\342\352\350 \363\352\340\347\340\362\345\353\377 \342 \357\340\352\345\362\355\356\354 \364\340\351\353\345 :", PackagePath})));
            }
            OpenSlots[SlotIndex].CurrentDataOffset = Position;
        }
        return true;
    }

    // Returns 0xFFFFFFFF for an unavailable slot or SlotIndex=0xFFFFFFFF.
    std::uint32_t TPackFileEC::GetEntrySlotPosition(std::uint32_t SlotIndex) {
        std::uint32_t Result = 0xffffffffu;
        if (SlotIndex == 0xffffffffu) {
            return Result;
        }
        if (SlotIndex > 15) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({PackSlotRangeError, pas::wide_int_to_str(15), u": ", pas::wide_int64_to_str(static_cast<std::int64_t>(SlotIndex))}))));
        }
        if (OpenSlots[SlotIndex].IsAvailable) {
            return Result;
        }
        return OpenSlots[SlotIndex].CurrentDataOffset - OpenSlots[SlotIndex].DataStartOffset;
    }

    // Returns 0xFFFFFFFF for an unavailable slot or SlotIndex=0xFFFFFFFF.
    std::uint32_t TPackFileEC::GetEntrySlotSize(std::uint32_t SlotIndex) {
        std::uint32_t Result = 0xffffffffu;
        if (SlotIndex == 0xffffffffu) {
            return Result;
        }
        if (SlotIndex > 15) {
            pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({PackSlotRangeError, pas::wide_int_to_str(15), u": ", pas::wide_int64_to_str(static_cast<std::int64_t>(SlotIndex))}))));
        }
        if (OpenSlots[SlotIndex].IsAvailable) {
            return Result;
        }
        return OpenSlots[SlotIndex].DataSize;
    }

    void THsFolderEC_Create(THsFolderEC* Self, pas::AnsiString FolderName) {
        Self->EntryBuffer = nullptr;
        Self->HeaderSize = 12u;
        Self->EntryCount = 0u;
        Self->EntryRecordSize = static_cast<std::int32_t>(sizeof(TPackEntryEC));
        Self->Parent = nullptr;
        Self->OriginalName = FolderName;
        Self->UpperName = SysUtilsImports::UpperCase(FolderName);
        Self->ChangedFlag = false;
        Self->InitializedEmptyFlag = false;
    }

    void THsFolderEC_CreateChild(THsFolderEC* Self, pas::AnsiString FolderName, THsFolderEC* Parent) {
        Self->EntryBuffer = nullptr;
        Self->HeaderSize = 12u;
        Self->EntryCount = 0u;
        Self->EntryRecordSize = static_cast<std::int32_t>(sizeof(TPackEntryEC));
        Self->Parent = Parent;
        Self->OriginalName = FolderName;
        Self->UpperName = SysUtilsImports::UpperCase(FolderName);
        Self->ChangedFlag = false;
        Self->InitializedEmptyFlag = false;
    }

    void THsFolderEC_Destroy(THsFolderEC* Self) {
        Self->Unload();
    }

    // Returns nil for an out-of-range index.
    PPackEntryEC THsFolderEC::GetEntry(std::uint32_t Index) {
        if (Index < EntryCount) {
            return static_cast<PPackEntryEC>(EC_HsFile::OffsetPackPointer(EntryBuffer, EntryRecordSize * Index));
        }
        return nullptr;
    }

    // Uppercases EntryName and skips entries with nonzero Flags.
    PPackEntryEC THsFolderEC::FindEntry(pas::AnsiString EntryName) {
        std::int32_t i{};
        PPackEntryEC Entry{};
        EntryName = SysUtilsImports::UpperCase(EntryName);
        PPackEntryEC Result = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(EntryCount - 1)); cpp_range.next(i); ) {
            Entry = GetEntry(i);
            if (Entry->Flags == 0) {
                if (SysUtilsImports::StrComp(Entry->UpperName.elements, EntryName.pchar()) == 0) {
                    Result = Entry;
                    break;
                }
            }
        }
        return Result;
    }

    // Requires an unloaded folder.
    void THsFolderEC::InitializeEmpty() {
        EntryCount = 0u;
        EntryRecordSize = static_cast<std::int32_t>(sizeof(TPackEntryEC));
        HeaderSize = EntryRecordSize * EntryCount + 12;
        EntryBuffer = nullptr;
        InitializedEmptyFlag = true;
        UpdateParentEntry();
    }

    // Returns false when already loaded; flagged child folders are skipped.
    std::uint8_t THsFolderEC::Load(std::uint32_t FileHandle, std::uint32_t SubtreeOffset) {
        std::uint32_t BytesRead{};
        std::int32_t i{};
        PPackEntryEC Entry{};
        THsFolderEC* Folder{};
        std::uint8_t Result = false;
        if (EntryBuffer != nullptr) {
            return Result;
        }
        InitializedEmptyFlag = false;
        ChangedFlag = false;
        WindowsImports::SetFilePointer(FileHandle, SubtreeOffset, nullptr, WindowsImports::FILE_BEGIN);
        std::uint8_t Success = WindowsImports::ReadFile(FileHandle, &HeaderSize, 12u, BytesRead, nullptr) != 0;
        if (!Success) {
            return Result;
        }
        if (BytesRead != 12) {
            return Result;
        }
        if (EntryRecordSize != static_cast<std::int32_t>(sizeof(TPackEntryEC))) {
            return Result;
        }
        EntryBuffer = static_cast<PPackEntryEC>(System::AllocMem(EntryCount * EntryRecordSize));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(EntryCount - 1)); cpp_range.next(i); ) {
            Success = WindowsImports::ReadFile(FileHandle, GetEntry(i), EntryRecordSize, BytesRead, nullptr) != 0;
            if (static_cast<std::uint8_t>(Success ^ 1) || BytesRead != EntryRecordSize) {
                Unload();
                return Result;
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(EntryCount - 1)); cpp_range_2.next(i); ) {
            Entry = GetEntry(i);
            Entry->ChildFolder = nullptr;
            Entry->KindCopy = Entry->Kind;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(EntryCount - 1)); cpp_range_3.next(i); ) {
            Entry = GetEntry(i);
            if (Entry->Kind == 3 && Entry->Flags == 0) {
                Folder = pas::construct_call<THsFolderEC>(THsFolderEC_CreateChild, pas::concat_ansi({pas::array_text<pas::AnsiString>(Entry->OriginalName.elements, 63), ""}), this);
                Entry->ChildFolder = Folder;
                Success = Folder->Load(FileHandle, Entry->TargetOffset);
                if (!Success) {
                    Unload();
                    return Result;
                }
            }
        }
        return true;
    }

    // Marks this folder and its parent changed.
    void THsFolderEC::Unload() {
        std::int32_t i{};
        PPackEntryEC Entry{};
        THsFolderEC* Folder{};
        if (EntryBuffer != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(EntryCount - 1)); cpp_range.next(i); ) {
                Entry = GetEntry(i);
                if (Entry->Kind == 3 && Entry->Flags == 0) {
                    Folder = Entry->ChildFolder;
                    if (Folder != nullptr) {
                        pas::free(Folder);
                    }
                    Entry->ChildFolder = nullptr;
                }
            }
            pas::free_mem(EntryBuffer);
            EntryBuffer = nullptr;
            EntryCount = 0u;
            HeaderSize = EntryCount * EntryRecordSize + 12;
            ChangedFlag = true;
            UpdateParentEntry();
        }
    }

    // Accepts slash and backslash separators; returns nil when absent.
    PPackEntryEC THsFolderEC::ResolveEntryByPath(pas::AnsiString EntryPath) {
        pas::AnsiString Head{};
        pas::AnsiString Tail{};
        THsFolderEC* Folder{};
        PPackEntryEC Result = nullptr;
        Head = EC_HsFile::AnsiBeforeFirstDelimiter(EntryPath, "/\\"_a);
        Tail = EC_HsFile::AnsiAfterFirstDelimiter(EntryPath, "/\\"_a);
        PPackEntryEC Entry = FindEntry(Head);
        if (Entry != nullptr) {
            if (Entry->Kind == 3) {
                if (Tail == "") {
                    return Entry;
                }
                Folder = Entry->ChildFolder;
                return Folder->ResolveEntryByPath(Tail);
            } else if (Tail == "") {
                return Entry;
            } else {
                return Result;
            }
        }
        return Result;
    }

    // Invalidates the parent's stored target offset.
    void THsFolderEC::UpdateParentEntry() {
        PPackEntryEC Entry{};
        if (Parent != nullptr) {
            Entry = Parent->FindEntry(OriginalName);
            if (Entry == nullptr) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"\321\341\356\351 \342 \364\340\351\353\356\342\356\351 \361\350\361\362\345\354\345 \357\340\352\345\362\355\356\343\356 \364\340\351\353\340 - Folder: ", UpperName})));
            }
            if (Entry->Kind != 3) {
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"\312\356\355\364\353\350\352\362 \350\354\345\355 \364\340\351\353/\344\350\360\345\352\362\356\360\350\377: ", UpperName})));
            }
            Entry->StoredSize = HeaderSize;
            Entry->TargetOffset = 0u;
            Parent->ChangedFlag = true;
        }
    }

    void TPackCollectionEC_Create(TPackCollectionEC* Self) {
        std::int32_t i{};
        Self->NameToPackIndexHash = nullptr;
        Self->UseFastNameIndex = false;
        Self->LastPack = nullptr;
        Self->FirstPack = nullptr;
        for (i = 0; i <= 127; ++i) {
            Self->PackByIndex[i] = nullptr;
        }
    }

    // Unlinks packs without freeing them.
    void TPackCollectionEC_Destroy(TPackCollectionEC* Self) {
        Self->Clear(false);
    }

    // Frees the name hash even when FreePacks is false.
    void TPackCollectionEC::Clear(std::uint8_t FreePacks) {
        std::int32_t i{};
        for (i = 0; i <= 127; ++i) {
            PackByIndex[i] = nullptr;
        }
        if (NameToPackIndexHash != nullptr) {
            pas::free(NameToPackIndexHash);
            NameToPackIndexHash = nullptr;
        }
        while (FirstPack != nullptr) {
            RemovePack(FirstPack, FreePacks);
        }
    }

    // List mutations rebuild PackByIndex and CollectionIndex without clearing the name hash.
    // The fixed array's 128-package capacity is not checked.
    void TPackCollectionEC::AddPackToFront(TPackFileEC* Pack) {
        TPackFileEC* Item{};
        std::int32_t Count{};
        std::int32_t i{};
        for (i = 0; i <= 127; ++i) {
            PackByIndex[i] = nullptr;
        }
        if (FirstPack == nullptr) {
            FirstPack = Pack;
            LastPack = Pack;
            Pack->NextPack = nullptr;
            Pack->PrevPack = nullptr;
            Item = FirstPack;
            Count = 0;
            while (Item != nullptr) {
                Item->CollectionIndex = Count;
                PackByIndex[Count] = Item;
                ++Count;
                Item = Item->NextPack;
            }
        } else {
            Pack->PrevPack = nullptr;
            Pack->NextPack = FirstPack;
            FirstPack->PrevPack = Pack;
            FirstPack = Pack;
            Item = FirstPack;
            Count = 0;
            while (Item != nullptr) {
                Item->CollectionIndex = Count;
                PackByIndex[Count] = Item;
                ++Count;
                Item = Item->NextPack;
            }
        }
    }

    void TPackCollectionEC::AddPackToBack(TPackFileEC* Pack) {
        TPackFileEC* Item{};
        std::int32_t Count{};
        std::int32_t i{};
        for (i = 0; i <= 127; ++i) {
            PackByIndex[i] = nullptr;
        }
        if (FirstPack == nullptr) {
            FirstPack = Pack;
            LastPack = Pack;
            Pack->NextPack = nullptr;
            Pack->PrevPack = nullptr;
            Item = FirstPack;
            Count = 0;
            while (Item != nullptr) {
                Item->CollectionIndex = Count;
                PackByIndex[Count] = Item;
                ++Count;
                Item = Item->NextPack;
            }
        } else {
            Pack->PrevPack = LastPack;
            Pack->NextPack = nullptr;
            LastPack->NextPack = Pack;
            LastPack = Pack;
            Item = FirstPack;
            Count = 0;
            while (Item != nullptr) {
                Item->CollectionIndex = Count;
                PackByIndex[Count] = Item;
                ++Count;
                Item = Item->NextPack;
            }
        }
    }

    // When retained, Pack keeps its old links and CollectionIndex.
    void TPackCollectionEC::RemovePack(TPackFileEC* Pack, std::uint8_t FreePack) {
        std::int32_t i{};
        for (i = 0; i <= 127; ++i) {
            PackByIndex[i] = nullptr;
        }
        if (Pack->PrevPack != nullptr) {
            Pack->PrevPack->NextPack = Pack->NextPack;
        }
        if (Pack->NextPack != nullptr) {
            Pack->NextPack->PrevPack = Pack->PrevPack;
        }
        if (FirstPack == Pack) {
            FirstPack = Pack->NextPack;
        }
        if (LastPack == Pack) {
            LastPack = Pack->PrevPack;
        }
        if (FreePack) {
            pas::free(Pack);
        }
        TPackFileEC* Item = FirstPack;
        std::int32_t Count = 0;
        while (Item != nullptr) {
            Item->CollectionIndex = Count;
            PackByIndex[Count] = Item;
            ++Count;
            Item = Item->NextPack;
        }
    }

    // A false result rolls back previously opened packages.
    std::uint8_t TPackCollectionEC::OpenAllPackages() {
        std::uint8_t Result = false;
        if (UseFastNameIndex) {
            if (NameToPackIndexHash != nullptr) {
                pas::free(NameToPackIndexHash);
            }
            NameToPackIndexHash = pas::construct_call<THashEC>(THashEC_Create);
            NameToPackIndexHash->InitializeEmptyTable(1024);
        }
        TPackFileEC* Pack = FirstPack;
        while (Pack != nullptr) {
            if (!Pack->Open()) {
                break;
            }
            Pack = Pack->NextPack;
        }
        if (Pack != nullptr) {
            Pack = Pack->PrevPack;
            while (Pack != nullptr) {
                Pack->Close();
                Pack = Pack->PrevPack;
            }
            return Result;
        }
        return true;
    }

    // Returns true regardless of individual close results.
    std::uint8_t TPackCollectionEC::CloseAllPackages() {
        TPackFileEC* Pack = FirstPack;
        while (Pack != nullptr) {
            Pack->Close();
            Pack = Pack->NextPack;
        }
        if (NameToPackIndexHash != nullptr) {
            pas::free(NameToPackIndexHash);
            NameToPackIndexHash = nullptr;
        }
        return true;
    }

    // Returns nil when out of range.
    TPackFileEC* TPackCollectionEC::GetPackByIndex(std::int32_t PackIndex) {
        TPackFileEC* Pack = FirstPack;
        while (Pack != nullptr) {
            if (PackIndex == 0) {
                break;
            }
            Pack = Pack->NextPack;
            --PackIndex;
        }
        return Pack;
    }

    // Returns package index * 16 + slot, or -1.
    std::int32_t TPackCollectionEC::OpenEntryByPathAcrossPackages(pas::AnsiString EntryPath, std::uint32_t DesiredAccess, std::uint8_t FirstPackageOnly) {
        TPackFileEC* Pack{};
        std::int32_t HashSlot{};
        std::int32_t MappedIndex{};
        std::int32_t Result = -1;
        std::int32_t Index = 0;
        std::int32_t Slot = -1;
        if (UseFastNameIndex && static_cast<std::uint8_t>(FirstPackageOnly ^ 1)) {
            HashSlot = NameToPackIndexHash->FindOrInsertKeySlot(EntryPath);
            if (HashSlot != -1) {
                MappedIndex = NameToPackIndexHash->GetSlotMappedValue(HashSlot);
                if (MappedIndex == -1) {
                    Pack = FirstPack;
                    while (Pack != nullptr) {
                        Slot = Pack->OpenEntryByPath(EntryPath, DesiredAccess);
                        if (Slot != -1) {
                            break;
                        }
                        Pack = Pack->NextPack;
                    }
                    if (Slot == -1) {
                        return Result;
                    }
                    MappedIndex = Pack->CollectionIndex;
                    NameToPackIndexHash->SetSlotMappedValue(HashSlot, MappedIndex);
                } else {
                    Pack = PackByIndex[MappedIndex];
                    Slot = Pack->OpenEntryByPath(EntryPath, DesiredAccess);
                    if (Slot == -1) {
                        NameToPackIndexHash->NoteStaleMappedValue();
                    }
                }
                if (Slot != -1) {
                    return MappedIndex * PackOpenSlotCount + Slot;
                }
            }
        }
        Pack = FirstPack;
        while (Pack != nullptr) {
            Slot = Pack->OpenEntryByPath(EntryPath, DesiredAccess);
            if (Slot != -1) {
                break;
            }
            if (FirstPackageOnly) {
                return Result;
            }
            Pack = Pack->NextPack;
            ++Index;
        }
        if (Slot != -1) {
            return Index * PackOpenSlotCount + Slot;
        }
        return Result;
    }

    // Uses the first package; truncates existing files. Returns a handle or -1.
    std::int32_t TPackCollectionEC::CreateLooseFile(pas::WideString FilePath) {
        std::int32_t Slot{};
        std::int32_t Result = -1;
        if (FirstPack != nullptr) {
            Slot = FirstPack->CreateLooseFile(FilePath);
            if (Slot != -1) {
                return Slot;
            }
        }
        return Result;
    }

    std::uint8_t TPackCollectionEC::CloseEntryHandle(std::int32_t Handle) {
        std::uint8_t Result = false;
        std::int32_t Index = pas::shr(Handle, PackOpenSlotShift);
        TPackFileEC* Pack = GetPackByIndex(Index);
        if (Pack != nullptr) {
            return Pack->CloseEntrySlot(Handle - Index * PackOpenSlotCount);
        }
        return Result;
    }

    std::uint8_t TPackCollectionEC::ReadEntryHandle(std::int32_t Handle, void* Buffer, std::uint32_t ByteCount) {
        std::uint8_t Result = false;
        std::int32_t Index = pas::shr(Handle, PackOpenSlotShift);
        TPackFileEC* Pack = GetPackByIndex(Index);
        if (Pack != nullptr) {
            return Pack->ReadEntrySlot(Handle - Index * PackOpenSlotCount, Buffer, ByteCount);
        }
        return Result;
    }

    std::uint8_t TPackCollectionEC::WriteEntryHandle(std::int32_t Handle, void* Buffer, std::uint32_t ByteCount) {
        std::uint8_t Result = false;
        std::int32_t Index = pas::shr(Handle, PackOpenSlotShift);
        TPackFileEC* Pack = GetPackByIndex(Index);
        if (Pack != nullptr) {
            return Pack->WriteEntrySlot(Handle - Index * PackOpenSlotCount, Buffer, ByteCount);
        }
        return Result;
    }

    std::uint8_t TPackCollectionEC::SeekEntryHandle(std::int32_t Handle, std::uint32_t Offset, std::int32_t Origin) {
        std::uint8_t Result = false;
        std::int32_t Index = pas::shr(Handle, PackOpenSlotShift);
        TPackFileEC* Pack = GetPackByIndex(Index);
        if (Pack != nullptr) {
            return Pack->SeekEntrySlot(Handle - Index * PackOpenSlotCount, Offset, Origin);
        }
        return Result;
    }

    // Returns 0xFFFFFFFF for an invalid handle.
    std::uint32_t TPackCollectionEC::GetEntryHandlePosition(std::int32_t Handle) {
        std::uint32_t Result = 0xffffffffu;
        std::int32_t Index = pas::shr(Handle, PackOpenSlotShift);
        TPackFileEC* Pack = GetPackByIndex(Index);
        if (Pack != nullptr) {
            return Pack->GetEntrySlotPosition(Handle - Index * PackOpenSlotCount);
        }
        return Result;
    }

    // Returns 0xFFFFFFFF for an invalid handle.
    std::uint32_t TPackCollectionEC::GetEntryHandleSize(std::int32_t Handle) {
        std::uint32_t Result = 0xffffffffu;
        std::int32_t Index = pas::shr(Handle, PackOpenSlotShift);
        TPackFileEC* Pack = GetPackByIndex(Index);
        if (Pack != nullptr) {
            return Pack->GetEntrySlotSize(Handle - Index * PackOpenSlotCount);
        }
        return Result;
    }

    void THashEC_Create(THashEC* Self) {
        Self->InitializeEmptyTable(1024);
    }

    void THashEC_Destroy(THashEC* Self) {
        THashEC::ReleaseTable();
    }

    // Key is not modified; only its trailing 32 bytes contribute to the hash.
    std::int32_t THashEC::ComputeLookupBucketAndFullHash(pas::AnsiString& Key, std::uint32_t& FullHash) {
        std::int32_t i{};
        std::int32_t First{};
        std::int32_t KeyLength = Key.length();
        if (KeyLength > 32) {
            First = KeyLength - 31;
        } else {
            First = 1;
        }
        std::uint32_t Hash = 0u;
        for (auto cpp_range = pas::for_to<std::int32_t>(First, KeyLength); cpp_range.next(i); ) {
            Hash = Key.read(i) + Hash * 2;
        }
        FullHash = Hash;
        return Hash & 1023;
    }

    // Returns -1 on failure. Native probing can reach slot 1024; promoted hits return the pre-swap index.
    std::int32_t THashEC::FindOrInsertKeySlot(pas::AnsiString Key) {
        std::uint32_t Hash{};
        std::uint32_t i{};
        THashSlotEC Temp{};
        std::uint32_t Bucket = THashEC::ComputeLookupBucketAndFullHash(Key, Hash);
        std::int32_t Found = -1;
        for (auto cpp_range = pas::for_to<std::uint32_t>(Bucket, Bucket + 5); cpp_range.next(i); ) {
            if (Slots[i].MappedValue != -1) {
                if (Slots[i].FullHash == Hash && EC_HsFile::MatchLookupKeySuffix(Key, &Slots[i].KeySuffix, 32)) {
                    Found = i;
                    ++Slots[i].HitCount;
                    ++OperationCount;
                    ++HitCount;
                    ++HitCountCopy;
                    MaybeResetStatistics();
                    if (i > Bucket && i < static_cast<std::uint32_t>(1024)) {
                        if (Slots[i].HitCount > Slots[i - 1].HitCount) {
                            Temp = Slots[i];
                            Slots[i] = Slots[i - 1];
                            Slots[i - 1] = Temp;
                        }
                    }
                    break;
                }
            } else {
                Slots[i].FullHash = Hash;
                Slots[i].HitCount = 1u;
                Slots[i].Unknown0C = 1;
                EC_HsFile::CopyLookupKeySuffix(&Slots[i].KeySuffix, 32, Key);
                Found = i;
                ++OperationCount;
                ++MissCount;
                MaybeResetStatistics();
                break;
            }
            if (i > 1023) {
                break;
            }
        }
        if (Found == -1) {
            ++OperationCount;
            ++MissCount;
            MaybeResetStatistics();
        }
        return Found;
    }

    void THashEC::SetSlotMappedValue(std::int32_t SlotIndex, std::int32_t Value) {
        Slots[SlotIndex].MappedValue = Value;
    }

    // Ignores BucketCount; the table has 1024 buckets. Always returns true.
    std::uint8_t THashEC::InitializeEmptyTable(std::int32_t BucketCount) {
        std::int32_t i{};
        OperationCount = 0u;
        HitCount = 0u;
        HitCountCopy = 0u;
        StaleValueCount = 0u;
        MissCount = 0u;
        for (i = 0; i <= 1023; ++i) {
            Slots[i].MappedValue = -1;
        }
        return true;
    }

    // Returns true without changing the table.
    std::uint8_t THashEC::ReleaseTable() {
        return true;
    }

    std::int32_t THashEC::GetSlotMappedValue(std::int32_t SlotIndex) {
        return Slots[SlotIndex].MappedValue;
    }

    void THashEC::MaybeResetStatistics() {
        if (OperationCount % 100 == 0 && OperationCount != 0) {
            OperationCount = 0u;
            HitCount = 0u;
            HitCountCopy = 0u;
            StaleValueCount = 0u;
            MissCount = 0u;
        }
    }

    void THashEC::NoteStaleMappedValue() {
        ++StaleValueCount;
    }

    void THsFolderEC::p_destroy() {
        EC_HsFile::THsFolderEC_Destroy(this);
    }

    void TPackFileEC::p_destroy() {
        EC_HsFile::TPackFileEC_Destroy(this);
    }

    void THashEC::p_destroy() {
        EC_HsFile::THashEC_Destroy(this);
    }

    void TPackCollectionEC::p_destroy() {
        EC_HsFile::TPackCollectionEC_Destroy(this);
    }

} // namespace EC_HsFile
