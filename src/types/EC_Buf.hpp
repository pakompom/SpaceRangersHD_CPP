#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TEncodedTableHeaderEC;

} // namespace EC_Buf

namespace EC_File {
    struct TFileEC;

} // namespace EC_File

namespace EC_Buf {
    struct TBufEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TBufEC : EC_Struct::TObjectEx {
        PAS_CLASS_META(TBufEC, EC_Struct::TObjectEx, "TBufEC", 20)
        void p_destroy() override;
        void Clear();
        // Nonpositive sizes clear the buffer; shrinking clamps Position.
        void SetSize(std::int32_t NewSize);
        std::uint8_t IsAtEnd();
        void SetPosition(std::int32_t NewPosition);
        // AddedBytes must be positive. Extends DataSize without advancing Position.
        void EnsureWriteCapacity(std::int32_t AddedBytes);
        // AddedBytes must be positive; Offset must be in 0..DataSize. Position is unchanged.
        void EnsureWriteCapacityAtOffset(std::int32_t Offset, std::int32_t AddedBytes);
        void EnsureReadable(std::int32_t Bytes);
        // Rejects nonpositive counts and ranges ending past DataSize; does not reject negative Offset.
        void EnsureReadableAtOffset(std::int32_t Offset, std::int32_t Bytes);
        void SetByteAt(std::int32_t Offset, std::uint8_t Value);
        void SetInt32At(std::int32_t Offset, std::int32_t Value);
        // Sequential operations use Position, including writes into existing data.
        void AddBytes(void* Source, std::int32_t ByteCount);
        void AddAnsiStringZ(const pas::AnsiString& Value);
        void AddWideStringZ(const pas::WideString& Value);
        void AddAnsiStringRaw(const pas::AnsiString& Value);
        void AddWideStringRaw(const pas::WideString& Value);
        void AddByte(std::uint8_t Value);
        void AddWord(std::uint16_t Value);
        void AddInt32(std::int32_t Value);
        void AddInteger(std::int32_t Value);
        void AddAnsiChar(std::uint8_t Value);
        void AddWideChar(char16_t Value);
        void AddDWord(std::uint32_t Value);
        void AddIntegerValue(std::int32_t Value);
        void AddSingle(float Value);
        void AddDouble(double Value);
        void AddBoolean(std::uint8_t Value);
        // Writes a four-byte size followed by the entire source payload, ignoring its Position.
        void AddBuffer(TBufEC* Value);
        std::uint8_t GetByteAt(std::int32_t Offset);
        std::uint32_t GetUInt32At(std::int32_t Offset);
        std::int32_t GetInt32At(std::int32_t Offset);
        // Returns Dest; requires a positive ByteCount.
        void* ReadBytes(void* Dest, std::int32_t ByteCount);
        // Consumes the terminating zero; an empty scan advances Position by two even at the buffer end.
        char16_t* ReadWideStringToBuffer(char16_t* Dest);
        char16_t GetWideChar();
        // Replaces NaN with zero.
        double GetDouble();
        // Dest.Position is preserved unless it exceeds the new size.
        void ReadLengthPrefixedBuffer(TBufEC* Dest);
        // Lengths count characters, stop at the buffer end, and leave Position unchanged.
        std::int32_t GetWideStringLength();
        std::int32_t GetWideStringLengthAt(std::int32_t Offset);
        std::int32_t GetAnsiTextLineLength();
        std::int32_t GetAnsiTextLineLengthAt(std::int32_t Offset);
        std::int32_t GetWideTextLineLength();
        std::int32_t GetWideTextLineLengthAt(std::int32_t Offset);
        // Dest must have room for the text and a terminating zero; returns Dest.
        // Line readers stop at NUL, CR or LF and consume up to two such characters.
        std::uint8_t* ReadAnsiTextLineToBuffer(std::uint8_t* Dest);
        pas::AnsiString ReadAnsiTextLine();
        char16_t* ReadWideTextLineToBuffer(char16_t* Dest);
        pas::WideString ReadWideTextLine();
        pas::WideString ReadWideString();
        // Successful transforms replace the entire payload and reset Position to zero.
        // False leaves the buffer intact, including when DataSize is less than eight.
        // FastMode is ignored in this binary.
        std::uint8_t CompressZlibPayloadInPlace(std::uint8_t FastMode);
        std::uint8_t ExpandZlibPayloadInPlace();
        // Leaves Position unchanged.
        void ApplyDatXorCipher(std::int32_t Seed);
        std::uint32_t ComputeCrc32();
        // EndOffset is exclusive; offsets are not validated.
        std::uint32_t ComputeCrc32Range(std::int32_t StartOffset, std::int32_t EndOffset);
        // CrcOffset reserves eight bytes inside the half-open range. Stores the range CRC followed by a correction word preserving the previous prefix CRC through that slot.
        void UpdateEmbeddedCrc32(std::int32_t StartOffset, std::int32_t EndOffset, std::int32_t CrcOffset);
        // Loaders replace the payload and leave Position at zero.
        // Consumes from the current file position; balances its own handle acquisition.
        void LoadFromFileChunk(EC_File::TFileEC* SourceFile, std::int32_t ByteCount);
        // Reads only the remaining file bytes; balances its own handle acquisition.
        void LoadFromFile(EC_File::TFileEC* SourceFile);
        void LoadFromWideFilePath(char16_t* FileName);
        // Writes the entire payload at the open file's current position, ignoring the buffer's Position.
        void SaveToFile(EC_File::TFileEC* DestFile);
        std::int32_t DataSize;
        std::int32_t Capacity;
        std::int32_t Position;
        void* Data;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Shared disk header in score.dat and achievements.dat, after zlib expansion.
    // The XOR stream starts at Checksum; the checksum covers the following payload.
    #pragma pack(push, 1)
    struct TEncodedTableHeaderEC {
        std::int32_t Version;
        std::uint16_t SeedHighWord;
        std::uint16_t SeedLowWord;
        std::uint32_t Checksum;
    };
    #pragma pack(pop)

    using PEncodedTableHeaderEC = TEncodedTableHeaderEC*;

    // Shared seed recurrence in DAT streams, achievements, scores and arcade objects.
    // Callers retain their different sign/modulus handling.
    inline constexpr std::int32_t SeedRngMultiplier = 16807;

    inline constexpr std::int32_t SeedRngQuotient = 127773;

    inline constexpr std::int32_t SeedRngRemainder = 2836;

    inline constexpr std::int32_t SeedRngModulus = 0x7fffffff;

    inline constexpr std::int32_t BufferGrowthSlack = 256;

    inline constexpr std::int32_t CarriageReturnCode = 13;

    inline constexpr std::int32_t LineFeedCode = 10;

} // namespace EC_Buf
