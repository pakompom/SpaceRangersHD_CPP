#include "types/EC_BlockPar.hpp"
#include "types/SimpleSteamApi.hpp"
#include "types/SystemImports.hpp"
#include "types/WStringUtils.hpp"
#include "units/Achievements.hpp"
#include "units/DateUtils.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/NoSteamAchievemens.hpp"
#include "units/PopUp.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"

// Native linked unit spelling. Local achievement loading, persistence and notification.
// NotifyLocalAchievement precedes the inferred bracket; original ownership remains unresolved.
namespace NoSteamAchievemens {
    // Fills caller-owned strings/counters for a registered local achievement.
    void GetLocalAchievementData(pas::WideString Key, SimpleSteamApi::PAchievementData Data) {
        EC_BlockPar::TBlockParEC* Block{};
        Block = Achievements::AchievementDefinitions->FindBlock(Key);
        if (Block != nullptr) {
            Data->Achieved = GI_Main::ParseEnabledNameGI(Block->GetParam(u"Achieved"_wref.get()));
            *Data->Name = aConst::LocalizedColorText(pas::concat_wide({u"Achievements.", Key, u".Name"}));
            *Data->Description = aConst::LocalizedColorText(pas::concat_wide({u"Achievements.", Key, u".Description"}));
            Data->Reserved0C = 0;
            Data->MaxValue = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MaxValue"_wref.get())));
            Data->HasProgress = Data->MaxValue > 0;
            Data->Value = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Value"_wref.get())));
            *Data->IconPath = u"null"_w;
            Data->Date = SysUtils::StrToInt64(static_cast<pas::AnsiString>(Block->GetParam(u"Date"_wref.get())));
        }
    }

    // Reads achievements.dat, expands zlib, decodes its payload and verifies the additive checksum. Unknown keys do not consume their value fields in the native reader.
    void LoadLocalAchievements() {
        std::int32_t Index{};
        std::int32_t Size{};
        std::int32_t Count{};
        pas::WideString FileName{};
        EC_Buf::TBufEC* Buffer{};
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Version{};
        pas::WideString Key{};
        std::int32_t Seed{};
        std::int32_t Checksum{};
        std::uint8_t* Cursor{};
        FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"achievements.dat"});
        if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(FileName))) {
            Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
            {
                try {
                    Buffer->LoadFromWideFilePath(FileName.pchar());
                    Buffer->ExpandZlibPayloadInPlace();
                    Version = Buffer->GetInt32At(0);
                    if (Version != 0) {
                        pas::raise(pas::make_exception<pas::Abort>("Error unpacking achievements.dat"_a));
                    }
                    {
                        std::int32_t cpp_left_3 = Buffer->GetByteAt(6);
                        std::int32_t cpp_left_2 = cpp_left_3 | pas::shl(static_cast<std::int32_t>(Buffer->GetByteAt(7)), 8);
                        std::int32_t cpp_left = cpp_left_2 | pas::shl(static_cast<std::int32_t>(Buffer->GetByteAt(4)), 16);
                        Seed = cpp_left | pas::shl(static_cast<std::int32_t>(Buffer->GetByteAt(5)), 24);
                    }
                    Cursor = reinterpret_cast<std::uint8_t*>(static_cast<std::uint8_t*>(Buffer->Data) + 8);
                    Size = Buffer->DataSize;
                    for (auto cpp_range = pas::for_to<std::int32_t>(8, Size - 1); cpp_range.next(Index); ) {
                        *Cursor = *Cursor ^ static_cast<std::uint8_t>(Seed - 1);
                        Seed = 16807 * (Seed % 127773) - 2836 * (Seed / 127773);
                        if (Seed <= 0) {
                            Seed += 0x7fffffff;
                        }
                        Cursor = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Cursor) + 1);
                    }
                    Checksum = 0;
                    Cursor = reinterpret_cast<std::uint8_t*>(static_cast<std::uint8_t*>(Buffer->Data) + 12);
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(12, Size - 1); cpp_range_2.next(Index); ) {
                        Checksum += static_cast<std::uint8_t>(*Cursor ^ 0x000000ff);
                        Cursor = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Cursor) + 1);
                    }
                    if (Buffer->GetUInt32At(8) != static_cast<std::uint32_t>(Checksum)) {
                        pas::raise(pas::make_exception<pas::Abort>("Error unpacking achievements.dat"_a));
                    }
                    Buffer->SetPosition(12);
                    Count = EC_Buf::TBufEC_GetInt32(Buffer);
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
                        Key = Buffer->ReadWideString();
                        Block = Achievements::AchievementDefinitions->FindBlock(Key);
                        if (Block != nullptr) {
                            if (EC_Buf::TBufEC_GetBoolean(Buffer)) {
                                Block->SetOrAddParam(u"Achieved"_wref.get(), u"Yes"_wref.get());
                            }
                            Block->SetOrAddParam(u"Date"_wref.get(), pas::wide_int64_to_str(static_cast<std::int64_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
                            Block->SetOrAddParam(u"Value"_wref.get(), pas::wide_int_to_str(EC_Buf::TBufEC_GetInt32(Buffer)));
                        }
                    }
                } catch (...) {
                    pas::free(Buffer);
                    throw;
                }
                pas::free(Buffer);
            }
        }
    }

    // Returns true even if already unlocked; absent timestamps allow a fresh unlock.
    std::uint8_t UnlockLocalAchievement(EC_BlockPar::TBlockParEC* Block) {
        std::uint8_t Result = true;
        if (static_cast<std::uint8_t>(GI_Main::ParseEnabledNameGI(Block->GetParam(u"Achieved"_wref.get())) ^ 1) || Block->GetParam(u"Date"_wref.get()) == u"0") {
            Block->SetOrAddParam(u"Achieved"_wref.get(), u"Yes"_wref.get());
            Block->SetOrAddParam(u"Date"_wref.get(), pas::wide_int64_to_str(DateUtils::DateTimeToUnix(SysUtilsImports::Now())));
            NoSteamAchievemens::NotifyLocalAchievement(Block);
            NoSteamAchievemens::SaveLocalAchievements();
        }
        return Result;
    }

    // Positive increments only; clamps to MaxValue and saves accepted changes.
    std::uint8_t IncreaseLocalAchievementProgress(EC_BlockPar::TBlockParEC* Block, std::int32_t Amount) {
        std::int32_t NewValue{};
        std::uint8_t Result = false;
        if (Amount <= 0) {
            return Result;
        }
        if (GI_Main::ParseEnabledNameGI(Block->GetParam(u"Achieved"_wref.get())) && Block->GetParam(u"Date"_wref.get()) != u"0") {
            return Result;
        }
        std::int32_t MaxValue = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"MaxValue"_wref.get())));
        if (MaxValue == 0) {
            return Result;
        }
        std::int32_t OldValue = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Value"_wref.get())));
        if (OldValue >= MaxValue) {
            return Result;
        }
        Result = true;
        NewValue = std::min<std::int32_t>(MaxValue, OldValue + Amount);
        Block->SetOrAddParam(u"Value"_wref.get(), pas::wide_int_to_str(NewValue));
        if (NewValue >= MaxValue) {
            Block->SetOrAddParam(u"Achieved"_wref.get(), u"Yes"_wref.get());
            Block->SetOrAddParam(u"Date"_wref.get(), pas::wide_int64_to_str(DateUtils::DateTimeToUnix(SysUtilsImports::Now())));
            NoSteamAchievemens::NotifyLocalAchievement(Block);
        }
        NoSteamAchievemens::SaveLocalAchievements();
        return Result;
    }

    // Writes the native checksummed, encoded and compressed achievements.dat format.
    void SaveLocalAchievements() {
        std::int32_t Index{};
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Seed = pas::random(SystemImports::MaxInt, &System::RandSeed);
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->AddIntegerValue(0);
        Buffer->AddIntegerValue(0);
        Buffer->AddIntegerValue(0);
        Buffer->SetByteAt(6, Seed);
        Buffer->SetByteAt(7, pas::shr(Seed, 8));
        Buffer->SetByteAt(4, pas::shr(Seed, 16));
        Buffer->SetByteAt(5, pas::shr(Seed, 24));
        std::int32_t Count = Achievements::AchievementDefinitions->GetBlockCount();
        Buffer->AddIntegerValue(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Block = Achievements::AchievementDefinitions->GetBlockByIndex(Index);
            Buffer->AddWideStringZ(Achievements::AchievementDefinitions->GetBlockNameByIndex(Index));
            Buffer->AddBoolean(GI_Main::ParseEnabledNameGI(Block->GetParam(u"Achieved"_wref.get())));
            Buffer->AddDWord(SysUtils::StrToInt64(static_cast<pas::AnsiString>(Block->GetParam(u"Date"_wref.get()))));
            Buffer->AddIntegerValue(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Value"_wref.get()))));
        }
        std::int32_t Size = Buffer->DataSize;
        std::int32_t Checksum = 0;
        std::uint8_t* Cursor = reinterpret_cast<std::uint8_t*>(static_cast<std::uint8_t*>(Buffer->Data) + 12);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(12, Size - 1); cpp_range_2.next(Index); ) {
            Checksum += static_cast<std::uint8_t>(*Cursor ^ 0x000000ff);
            Cursor = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Cursor) + 1);
        }
        Buffer->SetInt32At(8, Checksum);
        Cursor = reinterpret_cast<std::uint8_t*>(static_cast<std::uint8_t*>(Buffer->Data) + 8);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(8, Size - 1); cpp_range_3.next(Index); ) {
            *Cursor = *Cursor ^ static_cast<std::uint8_t>(Seed - 1);
            Seed = 16807 * (Seed % 127773) - 2836 * (Seed / 127773);
            if (Seed <= 0) {
                Seed += 0x7fffffff;
            }
            Cursor = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Cursor) + 1);
        }
        Buffer->CompressZlibPayloadInPlace(false);
        EC_File::TFileEC* FileHandle = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        FileHandle->SetFileName(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"achievements.dat"}));
        FileHandle->CreateNew();
        FileHandle->WriteBuffer(Buffer->Data, Buffer->DataSize);
        pas::free(FileHandle);
        pas::free(Buffer);
    }

    // Queues the localized achievement toast when its controller exists.
    void NotifyLocalAchievement(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        pas::WideString ImagePath{};
        if (PopUp::PopupController != nullptr) {
            Text = aConst::LocalizedColorText(u"Achievements.AchievementReceived"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<Achievement>"_w, aConst::LocalizedColorText(pas::concat_wide({u"Achievements.", Block->GetParam(u"Id"_wref.get()), u".Name"})), u"<color=0,71,234>"_w);
            ImagePath = pas::concat_wide({u"GI,Bm.FormAchievements.Img.", Block->GetParam(u"Id"_wref.get())});
            PopUp::PopupController->QueueNotification(Text, ImagePath);
        }
    }

} // namespace NoSteamAchievemens
