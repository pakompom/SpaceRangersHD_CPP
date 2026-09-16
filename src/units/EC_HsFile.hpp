#pragma once
#include "types/EC_HsFile.hpp"

namespace EC_HsFile {
    extern EC_HsFile::TPackCollectionEC* PackageCollection;

    extern pas::CriticalSection* PackageFileLock;

    extern pas::AnsiString LooseFileRoot;

    extern const pas::WideString PackSlotRangeError;

    // Ignores SuffixLength; compares up to 32 trailing key bytes without checking stored length. Key is not modified.
    std::uint8_t MatchLookupKeySuffix(pas::AnsiString& Key, void* SuffixBytes, std::int32_t SuffixLength);

    // Ignores SuffixLength; copies up to 32 trailing key bytes without terminator or padding. Key is not modified.
    void CopyLookupKeySuffix(void* DestSuffixBytes, std::int32_t SuffixLength, pas::AnsiString& Key);

    // Returns Text when no delimiter occurs.
    pas::AnsiString AnsiBeforeFirstDelimiter(pas::AnsiString Text, pas::AnsiString Delimiters);

    // Returns an empty string when no delimiter occurs.
    pas::AnsiString AnsiAfterFirstDelimiter(pas::AnsiString Text, pas::AnsiString Delimiters);

    void* OffsetPackPointer(void* Data, std::uint32_t ByteOffset);

    void TPackFileEC_Create(TPackFileEC* Self);

    void TPackFileEC_Destroy(TPackFileEC* Self);

    void THsFolderEC_Create(THsFolderEC* Self, pas::AnsiString FolderName);

    void THsFolderEC_CreateChild(THsFolderEC* Self, pas::AnsiString FolderName, THsFolderEC* Parent);

    void THsFolderEC_Destroy(THsFolderEC* Self);

    void TPackCollectionEC_Create(TPackCollectionEC* Self);

    // Unlinks packs without freeing them.
    void TPackCollectionEC_Destroy(TPackCollectionEC* Self);

    void THashEC_Create(THashEC* Self);

    void THashEC_Destroy(THashEC* Self);

} // namespace EC_HsFile
