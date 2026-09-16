#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

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
        void SetSize(std::int32_t NewSize);
        std::uint8_t IsAtEnd();
        void SetPosition(std::int32_t NewPosition);
        void EnsureWriteCapacity(std::int32_t AddedBytes);
        void EnsureWriteCapacityAtOffset(std::int32_t Offset, std::int32_t AddedBytes);
        void EnsureReadable(std::int32_t Bytes);
        void EnsureReadableAtOffset(std::int32_t Offset, std::int32_t Bytes);
        void SetByteAt(std::int32_t Offset, std::uint8_t Value);
        void SetInt32At(std::int32_t Offset, std::int32_t Value);
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
        void AddBuffer(TBufEC* Value);
        std::uint8_t GetByteAt(std::int32_t Offset);
        std::uint32_t GetUInt32At(std::int32_t Offset);
        std::int32_t GetInt32At(std::int32_t Offset);
        void* ReadBytes(void* Dest, std::int32_t ByteCount);
        char16_t* ReadWideStringToBuffer(char16_t* Dest);
        char16_t GetWideChar();
        double GetDouble();
        void ReadLengthPrefixedBuffer(TBufEC* Dest);
        std::int32_t GetWideStringLength();
        std::int32_t GetWideStringLengthAt(std::int32_t Offset);
        std::int32_t GetAnsiTextLineLength();
        std::int32_t GetAnsiTextLineLengthAt(std::int32_t Offset);
        std::int32_t GetWideTextLineLength();
        std::int32_t GetWideTextLineLengthAt(std::int32_t Offset);
        std::uint8_t* ReadAnsiTextLineToBuffer(std::uint8_t* Dest);
        pas::AnsiString ReadAnsiTextLine();
        char16_t* ReadWideTextLineToBuffer(char16_t* Dest);
        pas::WideString ReadWideTextLine();
        pas::WideString ReadWideString();
        std::uint8_t CompressZlibPayloadInPlace(std::uint8_t FastMode);
        std::uint8_t ExpandZlibPayloadInPlace();
        void ApplyDatXorCipher(std::int32_t Seed);
        std::uint32_t ComputeCrc32();
        std::uint32_t ComputeCrc32Range(std::int32_t StartOffset, std::int32_t EndOffset);
        void UpdateEmbeddedCrc32(std::int32_t StartOffset, std::int32_t EndOffset, std::int32_t CrcOffset);
        void LoadFromFileChunk(EC_File::TFileEC* SourceFile, std::int32_t ByteCount);
        void LoadFromFile(EC_File::TFileEC* SourceFile);
        void LoadFromWideFilePath(char16_t* FileName);
        void SaveToFile(EC_File::TFileEC* DestFile);
        std::int32_t DataSize;
        std::int32_t Capacity;
        std::int32_t Position;
        void* Data;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    inline constexpr std::int32_t BufferGrowthSlack = 256;

    inline constexpr std::int32_t CarriageReturnCode = 13;

    inline constexpr std::int32_t LineFeedCode = 10;

} // namespace EC_Buf
