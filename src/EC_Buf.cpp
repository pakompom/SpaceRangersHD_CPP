#include "layout/EC_Buf.hpp"
#include "types/System.hpp"
#include "units/CrcUnit.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/Windows.hpp"

namespace EC_Buf {
    void TBufEC_Create(TBufEC* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TBufEC_Destroy(TBufEC* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TBufEC::Clear() {
        if (Data != nullptr) {
            EC_Mem::FreeEC(Data);
            Data = nullptr;
        }
        DataSize = 0;
        Capacity = 0;
        Position = 0;
    }

    // Nonpositive sizes clear the buffer; shrinking clamps Position.
    void TBufEC::SetSize(std::int32_t NewSize) {
        if (NewSize < 1) {
            Clear();
        } else {
            DataSize = NewSize;
            Capacity = NewSize + BufferGrowthSlack;
            Data = EC_Mem::ReAllocREC(Data, Capacity);
            if (Position > DataSize) {
                Position = DataSize;
            }
        }
    }

    std::uint8_t TBufEC::IsAtEnd() {
        return Position >= DataSize;
    }

    void TBufEC::SetPosition(std::int32_t NewPosition) {
        if (NewPosition < 0 || NewPosition > DataSize) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBufEC.PointerSet. zn=", SysUtils::IntToStr(NewPosition)})));
        }
        Position = NewPosition;
    }

    // AddedBytes must be positive. Extends DataSize without advancing Position.
    void TBufEC::EnsureWriteCapacity(std::int32_t AddedBytes) {
        if (AddedBytes < 1) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBufEC.TestAddLenBuf. addlen=", SysUtils::IntToStr(AddedBytes)})));
        }
        if (Position + AddedBytes > DataSize) {
            DataSize = Position + AddedBytes;
            if (DataSize > Capacity) {
                Capacity = std::max<std::int32_t>(Capacity * 2, DataSize + BufferGrowthSlack);
                Data = EC_Mem::ReAllocREC(Data, Capacity);
            }
        }
    }

    // AddedBytes must be positive; Offset must be in 0..DataSize. Position is unchanged.
    void TBufEC::EnsureWriteCapacityAtOffset(std::int32_t Offset, std::int32_t AddedBytes) {
        if (AddedBytes < 1 || Offset < 0 || Offset > DataSize) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBufEC.TestAddLenBuf. sme=", SysUtils::IntToStr(Offset), " addlen=", SysUtils::IntToStr(AddedBytes)})));
        }
        if (Offset + AddedBytes > DataSize) {
            DataSize = Offset + AddedBytes;
            if (DataSize > Capacity) {
                Capacity = std::max<std::int32_t>(Capacity * 2, DataSize + BufferGrowthSlack);
                Data = EC_Mem::ReAllocREC(Data, Capacity);
            }
        }
    }

    void TBufEC::EnsureReadable(std::int32_t Bytes) {
        if (Bytes < 1 || Position + Bytes > DataSize) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBufEC.TestGet. len=", SysUtils::IntToStr(Bytes)})));
        }
    }

    // Rejects nonpositive counts and ranges ending past DataSize; does not reject negative Offset.
    void TBufEC::EnsureReadableAtOffset(std::int32_t Offset, std::int32_t Bytes) {
        if (Bytes < 1 || Offset + Bytes > DataSize) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TBufEC.TestGet. sme=", SysUtils::IntToStr(Offset), " len=", SysUtils::IntToStr(Bytes)})));
        }
    }

    void TBufEC::SetByteAt(std::int32_t Offset, std::uint8_t Value) {
        EnsureWriteCapacityAtOffset(Offset, static_cast<std::int32_t>(sizeof(std::uint8_t)));
        EC_Mem::WriteByteEC(static_cast<std::uint8_t*>(Data) + Offset, Value);
    }

    void TBufEC::SetInt32At(std::int32_t Offset, std::int32_t Value) {
        EnsureWriteCapacityAtOffset(Offset, static_cast<std::int32_t>(sizeof(std::int32_t)));
        EC_Mem::WriteIntegerEC(static_cast<std::uint8_t*>(Data) + Offset, Value);
    }

    // Sequential operations use Position, including writes into existing data.
    void TBufEC::AddBytes(void* Source, std::int32_t ByteCount) {
        EnsureWriteCapacity(ByteCount);
        Windows::CopyMemory(EC_Mem::AddPointerOffset(Data, Position), Source, ByteCount);
        Position += ByteCount;
    }

    void TBufEC::AddAnsiStringZ(const pas::AnsiString& Value) {
        std::int32_t ByteCount = Value.length();
        EnsureWriteCapacity(ByteCount + 1);
        Windows::CopyMemory(EC_Mem::AddPointerOffset(Data, Position), Value.pchar(), ByteCount + 1);
        Position = Position + ByteCount + 1;
    }

    void TBufEC::AddWideStringZ(const pas::WideString& Value) {
        std::int32_t ByteCount = Value.length() * static_cast<std::int32_t>(sizeof(char16_t));
        EnsureWriteCapacity(ByteCount + static_cast<std::int32_t>(sizeof(char16_t)));
        Windows::CopyMemory(EC_Mem::AddPointerOffset(Data, Position), Value.pchar(), ByteCount + static_cast<std::int32_t>(sizeof(char16_t)));
        Position = Position + ByteCount + static_cast<std::int32_t>(sizeof(char16_t));
    }

    void TBufEC::AddAnsiStringRaw(const pas::AnsiString& Value) {
        std::int32_t ByteCount = Value.length();
        if (ByteCount > 0) {
            EnsureWriteCapacity(ByteCount);
            Windows::CopyMemory(EC_Mem::AddPointerOffset(Data, Position), Value.pchar(), ByteCount);
            Position += ByteCount;
        }
    }

    void TBufEC::AddWideStringRaw(const pas::WideString& Value) {
        std::int32_t ByteCount = Value.length() * static_cast<std::int32_t>(sizeof(char16_t));
        if (ByteCount > 0) {
            EnsureWriteCapacity(ByteCount);
            Windows::CopyMemory(EC_Mem::AddPointerOffset(Data, Position), Value.pchar(), ByteCount);
            Position += ByteCount;
        }
    }

    void TBufEC::AddByte(std::uint8_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::uint8_t)));
        EC_Mem::WriteByteEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::uint8_t));
    }

    void TBufEC::AddWord(std::uint16_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::uint16_t)));
        EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::uint16_t));
    }

    void TBufEC::AddInt32(std::int32_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::int32_t)));
        EC_Mem::WriteIntegerEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::int32_t));
    }

    void TBufEC::AddInteger(std::int32_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::int32_t)));
        EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::int32_t));
    }

    void TBufEC::AddAnsiChar(std::uint8_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::uint8_t)));
        EC_Mem::WriteByteEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::uint8_t));
    }

    void TBufEC::AddWideChar(char16_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(char16_t)));
        EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(char16_t));
    }

    void TBufEC::AddDWord(std::uint32_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::uint32_t)));
        EC_Mem::WriteIntegerEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::uint32_t));
    }

    void TBufEC::AddIntegerValue(std::int32_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::int32_t)));
        EC_Mem::WriteInt32EC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::int32_t));
    }

    void TBufEC::AddSingle(float Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(float)));
        EC_Mem::WriteSingleEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(float));
    }

    void TBufEC::AddDouble(double Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(double)));
        EC_Mem::WriteDoubleEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(double));
    }

    void TBufEC::AddBoolean(std::uint8_t Value) {
        EnsureWriteCapacity(static_cast<std::int32_t>(sizeof(std::uint8_t)));
        EC_Mem::WriteByteEC(EC_Mem::AddPointerOffset(Data, Position), Value);
        Position += static_cast<std::int32_t>(sizeof(std::uint8_t));
    }

    // Writes a four-byte size followed by the entire source payload, ignoring its Position.
    void TBufEC::AddBuffer(TBufEC* Value) {
        AddDWord(Value->DataSize);
        if (Value->DataSize > 0) {
            AddBytes(Value->Data, Value->DataSize);
        }
    }

    std::uint8_t TBufEC::GetByteAt(std::int32_t Offset) {
        EnsureReadableAtOffset(Offset, static_cast<std::int32_t>(sizeof(std::uint8_t)));
        return EC_Mem::ReadByteEC(static_cast<std::uint8_t*>(Data) + Offset);
    }

    std::uint32_t TBufEC::GetUInt32At(std::int32_t Offset) {
        EnsureReadableAtOffset(Offset, static_cast<std::int32_t>(sizeof(std::uint32_t)));
        return EC_Mem::ReadDWordEC(static_cast<std::uint8_t*>(Data) + Offset);
    }

    std::int32_t TBufEC::GetInt32At(std::int32_t Offset) {
        EnsureReadableAtOffset(Offset, static_cast<std::int32_t>(sizeof(std::int32_t)));
        return EC_Mem::ReadIntegerEC(static_cast<std::uint8_t*>(Data) + Offset);
    }

    // Returns Dest; requires a positive ByteCount.
    void* TBufEC::ReadBytes(void* Dest, std::int32_t ByteCount) {
        EnsureReadable(ByteCount);
        Windows::CopyMemory(Dest, EC_Mem::AddPointerOffset(Data, Position), ByteCount);
        Position += ByteCount;
        return Dest;
    }

    // Consumes the terminating zero; an empty scan advances Position by two even at the buffer end.
    char16_t* TBufEC::ReadWideStringToBuffer(char16_t* Dest) {
        std::int32_t Count = GetWideStringLengthAt(Position);
        if (Count > 0) {
            EnsureReadable(Count * static_cast<std::int32_t>(sizeof(char16_t)) + static_cast<std::int32_t>(sizeof(char16_t)));
            Windows::CopyMemory(Dest, static_cast<std::uint8_t*>(Data) + Position, Count * static_cast<std::int32_t>(sizeof(char16_t)) + static_cast<std::int32_t>(sizeof(char16_t)));
            Position = Position + Count * static_cast<std::int32_t>(sizeof(char16_t)) + static_cast<std::int32_t>(sizeof(char16_t));
        } else {
            Position += static_cast<std::int32_t>(sizeof(char16_t));
            pas::store_unaligned<char16_t>(Dest, u'\000');
        }
        return Dest;
    }

    char16_t TBufEC::GetWideChar() {
        EnsureReadable(static_cast<std::int32_t>(sizeof(char16_t)));
        char16_t Result = EC_Mem::ReadWideCharEC(static_cast<std::uint8_t*>(Data) + Position);
        Position += 2;
        return Result;
    }

    // Replaces NaN with zero.
    double TBufEC::GetDouble() {
        EnsureReadable(static_cast<std::int32_t>(sizeof(double)));
        double Result = EC_Mem::ReadDoubleEC(static_cast<std::uint8_t*>(Data) + Position);
        if (pas::is_nan(Result)) {
            Result = 0.0;
            GR_Main::AppendLogLineThreadSafe("Warning! NaN encountered, replaced with zero."_a);
        }
        Position += static_cast<std::int32_t>(sizeof(double));
        return Result;
    }

    // Dest.Position is preserved unless it exceeds the new size.
    void TBufEC::ReadLengthPrefixedBuffer(TBufEC* Dest) {
        std::int32_t ByteCount = EC_Buf::TBufEC_GetUInt32(this);
        Dest->SetSize(ByteCount);
        if (ByteCount > 0) {
            ReadBytes(Dest->Data, ByteCount);
        }
    }

    // Lengths count characters, stop at the buffer end, and leave Position unchanged.
    std::int32_t TBufEC::GetWideStringLength() {
        return GetWideStringLengthAt(Position);
    }

    std::int32_t TBufEC::GetWideStringLengthAt(std::int32_t Offset) {
        std::int32_t Cursor = Offset;
        std::int32_t Count = 0;
        while (Cursor + 1 < DataSize) {
            if (EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(Data, Cursor)) == 0) {
                return Count;
            }
            ++Count;
            Cursor += static_cast<std::int32_t>(sizeof(char16_t));
        }
        return Count;
    }

    std::int32_t TBufEC::GetAnsiTextLineLength() {
        return GetAnsiTextLineLengthAt(Position);
    }

    std::int32_t TBufEC::GetAnsiTextLineLengthAt(std::int32_t Offset) {
        std::uint8_t Ch{};
        std::int32_t Cursor = Offset;
        std::int32_t Count = 0;
        while (Cursor < DataSize) {
            Ch = EC_Mem::ReadByteEC(EC_Mem::AddPointerOffset(Data, Cursor));
            if (Ch == 0 || Ch == CarriageReturnCode || Ch == LineFeedCode) {
                return Count;
            }
            ++Count;
            Cursor += 1;
        }
        return Count;
    }

    std::int32_t TBufEC::GetWideTextLineLength() {
        return GetWideTextLineLengthAt(Position);
    }

    std::int32_t TBufEC::GetWideTextLineLengthAt(std::int32_t Offset) {
        std::uint16_t Ch{};
        std::int32_t Cursor = Offset;
        std::int32_t Count = 0;
        while (Cursor + 1 < DataSize) {
            Ch = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(Data, Cursor));
            if (Ch == 0 || Ch == CarriageReturnCode || Ch == LineFeedCode) {
                return Count;
            }
            ++Count;
            Cursor += static_cast<std::int32_t>(sizeof(char16_t));
        }
        return Count;
    }

    // Dest must have room for the text and a terminating zero; returns Dest.
    // Line readers stop at NUL, CR or LF and consume up to two such characters.
    std::uint8_t* TBufEC::ReadAnsiTextLineToBuffer(std::uint8_t* Dest) {
        std::uint8_t Ch{};
        std::int32_t Count = GetAnsiTextLineLength();
        if (Count > 0) {
            Windows::CopyMemory(Dest, static_cast<std::uint8_t*>(Data) + Position, Count);
            Dest[Count] = '\000';
            Position += Count;
        } else {
            *Dest = '\000';
        }
        if (Position < DataSize) {
            Ch = EC_Mem::ReadByteEC(static_cast<std::uint8_t*>(Data) + Position);
            if (Ch == 0 || Ch == CarriageReturnCode || Ch == LineFeedCode) {
                Position += 1;
            }
            if (Position < DataSize) {
                Ch = EC_Mem::ReadByteEC(static_cast<std::uint8_t*>(Data) + Position);
                if (Ch == 0 || Ch == CarriageReturnCode || Ch == LineFeedCode) {
                    Position += 1;
                }
            }
        }
        return Dest;
    }

    pas::AnsiString TBufEC::ReadAnsiTextLine() {
        pas::AnsiString Result{};
        std::int32_t Count = GetAnsiTextLineLength();
        if (Count > 0) {
            Result.set_length(Count);
            ReadAnsiTextLineToBuffer(Result.pchar());
            return Result;
        }
        Result.set_length(2);
        ReadAnsiTextLineToBuffer(Result.pchar());
        return pas::AnsiString();
    }

    char16_t* TBufEC::ReadWideTextLineToBuffer(char16_t* Dest) {
        std::uint16_t Ch{};
        std::int32_t Count = GetWideTextLineLength();
        if (Count > 0) {
            Windows::CopyMemory(Dest, static_cast<std::uint8_t*>(Data) + Position, Count * static_cast<std::int32_t>(sizeof(char16_t)));
            pas::store_unaligned<char16_t>(Dest + Count, u'\000');
            Position += Count * static_cast<std::int32_t>(sizeof(char16_t));
        } else {
            pas::store_unaligned<char16_t>(Dest, u'\000');
        }
        if (Position + 1 < DataSize) {
            Ch = EC_Mem::ReadWordEC(static_cast<std::uint8_t*>(Data) + Position);
            if (Ch == 0 || Ch == CarriageReturnCode || Ch == LineFeedCode) {
                Position += static_cast<std::int32_t>(sizeof(char16_t));
            }
            if (Position + 1 < DataSize) {
                Ch = EC_Mem::ReadWordEC(static_cast<std::uint8_t*>(Data) + Position);
                if (Ch == 0 || Ch == CarriageReturnCode || Ch == LineFeedCode) {
                    Position += static_cast<std::int32_t>(sizeof(char16_t));
                }
            }
        }
        return Dest;
    }

    pas::WideString TBufEC::ReadWideTextLine() {
        pas::WideString Result{};
        std::int32_t Count = GetWideTextLineLength();
        if (Count > 0) {
            Result.set_length(Count);
            ReadWideTextLineToBuffer(Result.pchar());
            return Result;
        }
        Result.set_length(2);
        ReadWideTextLineToBuffer(Result.pchar());
        return pas::WideString();
    }

    pas::WideString TBufEC::ReadWideString() {
        pas::WideString Result{};
        std::int32_t Count{};
        Count = GetWideStringLength();
        if (Count > 0) {
            Result.set_length(Count);
            ReadWideStringToBuffer(Result.pchar());
            Result.set_length(Count);
            return Result;
        }
        Result.set_length(1);
        ReadWideStringToBuffer(reinterpret_cast<char16_t*>(&Count));
        Result.set_length(0);
        return pas::WideString();
    }

    // Successful transforms replace the entire payload and reset Position to zero.
    // False leaves the buffer intact, including when DataSize is less than eight.
    // FastMode is ignored in this binary.
    std::uint8_t TBufEC::CompressZlibPayloadInPlace(std::uint8_t FastMode) {
        std::int32_t Mode = 0;
        if (FastMode == true) {
            Mode = 0;
        }
        if (DataSize < 8) {
            return false;
        }
        void* Buffer = EC_Mem::AllocEC(DataSize);
        std::int32_t ByteCount = GR_Main::OKGF_ZLib_Compress(Buffer, Data, DataSize, Mode);
        if (ByteCount == 0) {
            EC_Mem::FreeEC(Buffer);
            return false;
        }
        EC_Mem::FreeEC(Data);
        Data = Buffer;
        DataSize = ByteCount;
        Capacity = ByteCount;
        Position = 0;
        return true;
    }

    std::uint8_t TBufEC::ExpandZlibPayloadInPlace() {
        if (DataSize < 8) {
            return false;
        }
        std::int32_t ByteCount = GR_Main::OKGF_ZLib_UnCompress(nullptr, 0, Data, DataSize);
        if (ByteCount == 0) {
            return false;
        }
        void* Buffer = EC_Mem::AllocEC(ByteCount);
        ByteCount = GR_Main::OKGF_ZLib_UnCompress(Buffer, ByteCount, Data, DataSize);
        if (ByteCount == 0) {
            EC_Mem::FreeEC(Buffer);
            return false;
        }
        EC_Mem::FreeEC(Data);
        Data = Buffer;
        DataSize = ByteCount;
        Capacity = ByteCount;
        Position = 0;
        return true;
    }

    // Leaves Position unchanged.
    void TBufEC::ApplyDatXorCipher(std::int32_t Seed) {
        std::int32_t State{};
        std::int32_t i{};
        // Nested helper of TBufEC.ApplyDatXorCipher; requires its parent stack frame.
        auto StepDatXorSeedState = [&]() -> std::int32_t {
            State = 16807 * (State % 127773) - 2836 * (State / 127773);
            if (State <= 0) {
                State += 0x7fffffff;
            }
            return State - 1;
        };
        State = Seed;
        std::uint8_t* Cursor = static_cast<std::uint8_t*>(Data);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, DataSize - 1); cpp_range.next(i); ) {
            *Cursor = *Cursor ^ static_cast<std::uint8_t>(StepDatXorSeedState());
            Cursor = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Cursor) + 1);
        }
    }

    std::uint32_t TBufEC::ComputeCrc32() {
        return CrcUnit::ComputeCrc32(Data, DataSize);
    }

    // EndOffset is exclusive; offsets are not validated.
    std::uint32_t TBufEC::ComputeCrc32Range(std::int32_t StartOffset, std::int32_t EndOffset) {
        return CrcUnit::ComputeCrc32(static_cast<std::uint8_t*>(Data) + StartOffset, EndOffset - StartOffset);
    }

    // CrcOffset reserves eight bytes inside the half-open range. Stores the range CRC followed by a correction word preserving the previous prefix CRC through that slot.
    void TBufEC::UpdateEmbeddedCrc32(std::int32_t StartOffset, std::int32_t EndOffset, std::int32_t CrcOffset) {
        if (CrcOffset < StartOffset) {
            pas::raise(pas::make_exception<pas::Exception>("CRC update error"_a));
        }
        if (CrcOffset + 8 > EndOffset) {
            pas::raise(pas::make_exception<pas::Exception>("CRC update error"_a));
        }
        std::uint32_t WholeCrc = CrcUnit::ExtendCrc32(0u, static_cast<std::uint8_t*>(Data) + StartOffset, EndOffset - StartOffset);
        std::uint32_t PreviousPrefixCrc = CrcUnit::ExtendCrc32(0u, static_cast<std::uint8_t*>(Data) + StartOffset, CrcOffset - StartOffset + 8);
        pas::store_unaligned<std::uint32_t>(reinterpret_cast<System::PCardinal>(static_cast<std::uint8_t*>(Data) + CrcOffset), WholeCrc);
        std::uint32_t NewPrefixCrc = CrcUnit::ExtendCrc32(0u, static_cast<std::uint8_t*>(Data) + StartOffset, CrcOffset - StartOffset + 4);
        CrcUnit::WriteCrc32Correction(NewPrefixCrc, PreviousPrefixCrc, CrcOffset + 4 + static_cast<std::uint8_t*>(Data));
    }

    // Loaders replace the payload and leave Position at zero.
    // Consumes from the current file position; balances its own handle acquisition.
    void TBufEC::LoadFromFileChunk(EC_File::TFileEC* SourceFile, std::int32_t ByteCount) {
        std::int32_t ChunkSize{};
        void* Cursor{};
        std::int32_t NextChunkSize{};
        Clear();
        SourceFile->AcquireReadWriteHandle();
        try {
            SetSize(ByteCount);
            Cursor = Data;
            if (ByteCount > 0) {
                do {
                    if (ByteCount > 262144) {
                        NextChunkSize = 262144;
                    } else {
                        NextChunkSize = ByteCount;
                    }
                    ChunkSize = NextChunkSize;
                    SourceFile->ReadBuffer(Cursor, ChunkSize);
                    Cursor = EC_Mem::AddPointerOffset(Cursor, ChunkSize);
                    ByteCount -= ChunkSize;
                    if (ByteCount > 0) {
                        SysUtilsImports::Sleep(1u);
                    }
                } while (!(ByteCount <= 0));
            }
        } catch (...) {
            Clear();
        }
        SourceFile->ReleaseHandle();
        return;
    }

    // Reads only the remaining file bytes; balances its own handle acquisition.
    void TBufEC::LoadFromFile(EC_File::TFileEC* SourceFile) {
        std::int32_t ByteCount{};
        Clear();
        SourceFile->AcquireReadWriteHandle();
        try {
            {
                std::uint32_t cpp_left = SourceFile->GetSize();
                ByteCount = cpp_left - SourceFile->GetPointer();
            }
            SetSize(ByteCount);
            SourceFile->ReadBuffer(Data, ByteCount);
        } catch (...) {
            Clear();
        }
        SourceFile->ReleaseHandle();
    }

    void TBufEC::LoadFromWideFilePath(char16_t* FileName) {
        EC_File::TFileEC* SourceFile = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        {
            try {
                SourceFile->SetFileName(static_cast<pas::WideString>(FileName));
                SourceFile->AcquireReadHandle(false);
                LoadFromFile(SourceFile);
            } catch (...) {
                pas::free(SourceFile);
                throw;
            }
            pas::free(SourceFile);
        }
    }

    // Writes the entire payload at the open file's current position, ignoring the buffer's Position.
    void TBufEC::SaveToFile(EC_File::TFileEC* DestFile) {
        DestFile->WriteBuffer(Data, DataSize);
    }

    void TBufEC::p_destroy() {
        EC_Buf::TBufEC_Destroy(this);
    }

} // namespace EC_Buf
