#include "layout/fScore.hpp"
#include "types/Achievements.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_Main.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aShip.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_File.hpp"
#include "units/EC_Str.hpp"
#include "units/ExceptionInfo.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SimpleSteamApi.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aSaveLoad.hpp"
#include "units/fScore.hpp"

namespace fScore {
    using TScoreKillCounters = pas::Array<std::int32_t, 0, 6>;

    #pragma pack(push, 1)
    struct TScoreCounterView {
        pas::Array<std::uint8_t, 0, 39> Prefix;
        TScoreKillCounters Counters;
    };
    #pragma pack(pop)

    using PScoreCounterView = TScoreCounterView*;

    #pragma pack(push, 1)
    struct TShipCounterView {
        pas::Array<std::uint8_t, 0, 1235> Prefix;
        TScoreKillCounters Counters;
    };
    #pragma pack(pop)

    using PShipCounterView = TShipCounterView*;

    std::uint8_t ShouldSwapScoreEntries(TfScoreUnit*& Candidate, TfScoreUnit*& Current);

    static_assert(sizeof(void*) != 4 || sizeof(fScore::TScoreCounterView) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TScoreCounterView, Prefix) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TScoreCounterView, Counters) == 40);
    static_assert(sizeof(void*) != 4 || sizeof(fScore::TShipCounterView) == 1264);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TShipCounterView, Prefix) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fScore::TShipCounterView, Counters) == 1236);

    void SetElapsedScoreTurns(TfScore* Screen, TfScoreUnit* Entry) {
        std::int32_t Turns{};
        pas::WideString Template{};
        Template = aConst::LocalizedColorText(u"FormScore.TurnWin"_wref.get());
        if (Entry->FinishedTurn - 300 < 0) {
            Turns = 0;
        } else {
            Turns = Entry->FinishedTurn - 300;
        }
        {
            const pas::WideString& formatText1 = aMyFunction::FormatText1(Template, u"<color=255,222,0>"_w, u"<Date>"_w, pas::wide_int_to_str(Turns));
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(Screen->GetByName(u"ITurn"_wref.get()));
            cpp_arg->SetText(formatText1);
        }
    }

    void TfScoreUnit_Create(TfScoreUnit* Self) {
        pas::object_create(Self);
        Self->ScoreTags = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
    }

    void TfScoreUnit_Destroy(TfScoreUnit* Self) {
        pas::free(Self->ScoreTags);
        pas::object_destroy(Self);
    }

    void TfScoreUnit::CapturePlayer(std::uint8_t Victory) {
        aShip::TPilotSkill Skill{};
        std::int32_t I{};
        aRanger::PPlayerOldQuest Quest{};
        std::uint8_t Difficulty{};
        std::uint8_t Award{};
        PlayerName = aPlayer::GetPlayer()->Name;
        PortraitFaceId = aPlayer::GetPlayer()->PortraitFaceId;
        PilotRace = aPlayer::GetPlayer()->PilotRace;
        for (Difficulty = static_cast<std::uint8_t>(0); Difficulty <= static_cast<std::uint8_t>(7); ++Difficulty) {
            DifficultyLevels[Difficulty] = aGalaxy::Galaxy->DifficultyLevels[Difficulty];
        }
        Disqualified = GR_Main::CCInterface->GetTamperDetected() || GR_Main::CCInterface->GetFlag0A() || GR_Main::CCInterface->GetEditableStateApplied() || aGalaxy::Galaxy->CustomRules.Enabled || GR_Main::CCInterface->GetIntegrityError() != 0 || aGalaxy::Galaxy->GetCheatPoints() != 0;
        FinishedTurn = aGalaxy::Galaxy->CurrentTurn;
        Rank = aPlayer::GetPlayer()->Rank;
        PirateRank = aPlayer::GetPlayer()->PirateRank;
        pas::store_unaligned<TScoreKillCounters>(&reinterpret_cast<PScoreCounterView>(this)->Counters, pas::load_unaligned<TScoreKillCounters>(&reinterpret_cast<PShipCounterView>(aPlayer::GetPlayer())->Counters));
        OtherShipKillCount = OtherShipKillCount - PirateKillCount - DominatorKillCount;
        ArcadeKillCount = aPlayer::GetPlayer()->HyperspaceKillCount + aPlayer::GetPlayer()->BlackHoleKillCount;
        if (aPlayer::GetPlayer()->AwardIds == nullptr) {
            AwardCount = 0;
        } else {
            AwardCount = pas::list_count(aPlayer::GetPlayer()->AwardIds);
            AwardIds.set_length(AwardCount);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, AwardCount - 1); cpp_range.next(I); ) {
                Award = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aPlayer::GetPlayer()->AwardIds, I)));
                AwardIds[I] = Award;
            }
        }
        TotalExperience = aPlayer::GetPlayer()->TotalExperience;
        for (auto cpp_range_2 = pas::for_to<aShip::TPilotSkill>(aShip::psAccuracy, aShip::psLeadership); cpp_range_2.next(Skill); ) {
            SkillLevels[Skill] = aPlayer::GetPlayer()->GetBaseSkillLevel(Skill);
        }
        ScoreTags->Clear();
        if (GR_Main::CCInterface->Buffer->DataSize > 0) {
            ScoreTags->AddBytes(GR_Main::CCInterface->Buffer->Data, GR_Main::CCInterface->Buffer->DataSize);
        }
        GenerationSeed = aGalaxy::Galaxy->GenerationSeed;
        QuestResults.set_length(pas::list_count(aRanger::PlayerOldQuests));
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aRanger::PlayerOldQuests) - 1); cpp_range_3.next(I); ) {
            Quest = pas::list_at<aRanger::TPlayerOldQuest>(aRanger::PlayerOldQuests, I);
            QuestResults[I].Successful = Quest->Successful;
            QuestResults[I].QuestType = Quest->QuestType;
            QuestResults[I].QuestNumber = Quest->QuestNumber;
        }
        PlanetBattles = aPlayer::GetPlayer()->PlanetBattles;
        PlanetBattleHistory.set_length(aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1 + 1);
        {
            const std::int32_t cpp_last = aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    PlanetBattleHistory[I] = aPlayer::GetPlayer()->PlanetBattleHistory[I];
                }
            }
        }
        if (aGalaxy::Galaxy->BlazerSeriesResolvedTurn == 0) {
            BlazerEndingState = 0;
        } else if (aGalaxy::Galaxy->BlazerLandingPlanetId != 0 && aKling::BlazerShip != nullptr) {
            BlazerEndingState = 3;
        } else if (aGalaxy::Galaxy->BlazerSelfDestructTurn != 0) {
            BlazerEndingState = 2;
        } else {
            BlazerEndingState = 1;
        }
        if (GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle) {
            if (aGalaxy::Galaxy->KellerLeaveTurn != 0) {
                KellerEndingState = 2;
            } else if (aKling::KellerShip == nullptr) {
                KellerEndingState = 1;
            } else {
                KellerEndingState = 0;
            }
        } else if (aGalaxy::Galaxy->KellerSeriesResolvedTurn == 0) {
            KellerEndingState = 0;
        } else if (aGalaxy::Galaxy->KellerLeaveTurn != 0) {
            KellerEndingState = 2;
        } else if (aGalaxy::Galaxy->KellerResearchTargetStarId != 0) {
            KellerEndingState = 3;
        } else {
            KellerEndingState = 1;
        }
        if (aGalaxy::Galaxy->TerronSeriesResolvedTurn == 0) {
            TerronEndingState = 0;
        } else if (aGalaxy::Galaxy->TerronToStarTurn != 0) {
            TerronEndingState = 2;
        } else if (aGalaxy::Galaxy->TerronLandingLockTurn != 0) {
            TerronEndingState = 3;
        } else {
            TerronEndingState = 1;
        }
        if (aGalaxy::Galaxy->PirateWinTurn == 0) {
            PirateEndingState = 0;
        } else {
            PirateEndingState = aGalaxy::Galaxy->PirateWinType;
        }
        VictoryAchieved = Victory;
        RecalculateTotalScore();
        if (VictoryAchieved && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1) && static_cast<std::uint8_t>(GR_Main::CCInterface->GetFlag0A() ^ 1) && static_cast<std::uint8_t>(GR_Main::CCInterface->GetEditableStateApplied() ^ 1) && GR_Main::CCInterface->GetIntegrityError() == 0 && aGalaxy::Galaxy->GetCheatPoints() == 0) {
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckNoQuestVictoryAchievement();
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckChampionVictoryAchievement(TotalScore);
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckLongGameVictoryAchievement(TotalScore, FinishedTurn);
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckPacifistVictoryAchievement();
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckNoLoadVictoryAchievement();
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckFastVictoryAchievement();
            if (SimpleSteamApi::SteamInitialized && SimpleSteamApi::SteamLeaderboardFound()) {
                SimpleSteamApi::SteamUploadScore(TotalScore);
            }
        }
    }

    void TfScoreUnit::RecalculateDifficultyPercent() {
        std::uint8_t I{};
        DifficultyPercent = 0;
        for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(7); ++I) {
            DifficultyPercent = DifficultyPercent + 50 * DifficultyLevels[I] + 50;
        }
        DifficultyPercent = DifficultyPercent / 8;
    }

    void TfScoreUnit::RecalculateTotalScore() {
        pas::Extended Experience{};
        pas::Extended Difficulty{};
        std::uint8_t DominatorsResolved{};
        std::uint8_t PirateResolved{};
        std::uint8_t PirateDefeat{};
        RecalculateDifficultyPercent();
        Experience = TotalExperience;
        Difficulty = DifficultyPercent;
        if (VictoryAchieved) {
            {
                pas::Extended cpp_right = Math::Power(pas::real_max<pas::Extended>(7.0L, pas::real_divide(FinishedTurn - 300, 365.0L)), 1.3L);
                TotalScore = System::Round(pas::real_divide(pas::real_divide(Experience * Difficulty, 1.0E+2L), cpp_right));
            }
            DominatorsResolved = TerronEndingState != 0 && KellerEndingState != 0 && BlazerEndingState != 0;
            PirateResolved = PirateRank >= 7 || PirateEndingState == 3;
            PirateDefeat = PirateEndingState == 4;
            if (DominatorsResolved == false || PirateDefeat != false) {
                if (PirateResolved || DominatorsResolved) {
                    TotalScore = MathImports::Floor(TotalScore * 0.75L);
                } else {
                    TotalScore = TotalScore / 2;
                }
            }
        } else {
            TotalScore = 0;
        }
    }

    void TfScoreUnit::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        std::uint8_t Skill{};
        std::int32_t I{};
        std::uint8_t Difficulty{};
        Buffer->AddIntegerValue(205);
        Buffer->AddBoolean(VictoryAchieved);
        for (Difficulty = static_cast<std::uint8_t>(0); Difficulty <= static_cast<std::uint8_t>(7); ++Difficulty) {
            Buffer->AddAnsiChar(DifficultyLevels[Difficulty]);
        }
        Buffer->AddWideStringZ(PlayerName);
        Buffer->AddAnsiChar(PortraitFaceId);
        Buffer->AddAnsiChar(PilotRace);
        Buffer->AddIntegerValue(FinishedTurn);
        Buffer->AddAnsiChar(Rank);
        Buffer->AddAnsiChar(PirateRank);
        Buffer->AddIntegerValue(OtherShipKillCount);
        Buffer->AddIntegerValue(PirateKillCount);
        Buffer->AddIntegerValue(DominatorKillCount);
        Buffer->AddIntegerValue(LiberatedSystemCount);
        Buffer->AddIntegerValue(ArcadeKillCount);
        Buffer->AddIntegerValue(AwardCount);
        Buffer->AddIntegerValue(AwardIds.length() - 1 + 1);
        {
            const std::int32_t cpp_last = AwardIds.length() - 1;
            if (0 <= cpp_last) {
                for (I = 0; I <= cpp_last; ++I) {
                    Buffer->AddAnsiChar(AwardIds[I]);
                }
            }
        }
        Buffer->AddIntegerValue(TotalExperience);
        for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(5); ++Skill) {
            Buffer->AddAnsiChar(SkillLevels[Skill]);
        }
        Buffer->AddBoolean(Disqualified);
        Buffer->AddBuffer(ScoreTags);
        Buffer->AddIntegerValue(GenerationSeed);
        Buffer->AddWideChar(QuestResults.length() - 1 + 1);
        {
            const std::int32_t cpp_last_2 = QuestResults.length() - 1;
            if (0 <= cpp_last_2) {
                for (I = 0; I <= cpp_last_2; ++I) {
                    Buffer->AddBoolean(QuestResults[I].Successful);
                    Buffer->AddAnsiChar(static_cast<std::uint8_t>(QuestResults[I].QuestType));
                    Buffer->AddAnsiChar(QuestResults[I].QuestNumber);
                }
            }
        }
        Buffer->AddIntegerValue(PlanetBattles);
        Buffer->AddAnsiChar(BlazerEndingState);
        Buffer->AddAnsiChar(KellerEndingState);
        Buffer->AddAnsiChar(TerronEndingState);
        Buffer->AddAnsiChar(PirateEndingState);
        Buffer->AddWideChar(PlanetBattleHistory.length() - 1 + 1);
        {
            const std::int32_t cpp_last_3 = PlanetBattleHistory.length() - 1;
            if (0 <= cpp_last_3) {
                for (I = 0; I <= cpp_last_3; ++I) {
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].MapId);
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].Statistics[0]);
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].Statistics[1]);
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].Statistics[2]);
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].Statistics[3]);
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].Statistics[4]);
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].Statistics[5]);
                    Buffer->AddAnsiChar(PlanetBattleHistory[I].ResultCode);
                    Buffer->AddAnsiChar(PlanetBattleHistory[I].CompletionMode);
                    Buffer->AddIntegerValue(PlanetBattleHistory[I].DateTurn);
                }
            }
        }
    }

    void TfScoreUnit::LoadFromBuffer(EC_Buf::TBufEC* Buffer, std::int32_t FileVersion) {
        std::uint8_t Skill{};
        std::int32_t I{};
        std::int32_t Count{};
        std::uint8_t Difficulty{};
        std::int32_t Marker = EC_Buf::TBufEC_GetInt32(Buffer);
        if (Marker < 205 || Marker > 205) {
            Globals::ScoreScreen->CreateDefaultTable();
        } else {
            VictoryAchieved = EC_Buf::TBufEC_GetBoolean(Buffer);
            for (Difficulty = static_cast<std::uint8_t>(0); Difficulty <= static_cast<std::uint8_t>(7); ++Difficulty) {
                DifficultyLevels[Difficulty] = EC_Buf::TBufEC_GetByte(Buffer);
            }
            PlayerName = Buffer->ReadWideString();
            PortraitFaceId = EC_Buf::TBufEC_GetByte(Buffer);
            PilotRace = EC_Buf::TBufEC_GetByte(Buffer);
            FinishedTurn = EC_Buf::TBufEC_GetInt32(Buffer);
            Rank = EC_Buf::TBufEC_GetByte(Buffer);
            PirateRank = EC_Buf::TBufEC_GetByte(Buffer);
            OtherShipKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            PirateKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            DominatorKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            LiberatedSystemCount = EC_Buf::TBufEC_GetInt32(Buffer);
            ArcadeKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            AwardCount = EC_Buf::TBufEC_GetInt32(Buffer);
            Count = EC_Buf::TBufEC_GetInt32(Buffer);
            AwardIds.set_length(Count);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
                AwardIds[I] = EC_Buf::TBufEC_GetByte(Buffer);
            }
            TotalExperience = EC_Buf::TBufEC_GetInt32(Buffer);
            for (Skill = static_cast<std::uint8_t>(0); Skill <= static_cast<std::uint8_t>(5); ++Skill) {
                SkillLevels[Skill] = EC_Buf::TBufEC_GetByte(Buffer);
            }
            if (FileVersion < 1) {
                Disqualified = false;
                ScoreTags->Clear();
                GenerationSeed = 0;
                QuestResults = nullptr;
            } else {
                Disqualified = EC_Buf::TBufEC_GetBoolean(Buffer);
                Buffer->ReadLengthPrefixedBuffer(ScoreTags);
                GenerationSeed = EC_Buf::TBufEC_GetInt32(Buffer);
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                QuestResults.set_length(Count);
                {
                    const std::int32_t cpp_last = QuestResults.length() - 1;
                    if (0 <= cpp_last) {
                        for (I = 0; I <= cpp_last; ++I) {
                            QuestResults[I].Successful = EC_Buf::TBufEC_GetBoolean(Buffer);
                            QuestResults[I].QuestType = static_cast<aGalaxyStruct::TQuestType>(EC_Buf::TBufEC_GetByte(Buffer));
                            QuestResults[I].QuestNumber = EC_Buf::TBufEC_GetByte(Buffer);
                        }
                    }
                }
            }
            PlanetBattles = EC_Buf::TBufEC_GetInt32(Buffer);
            BlazerEndingState = EC_Buf::TBufEC_GetByte(Buffer);
            KellerEndingState = EC_Buf::TBufEC_GetByte(Buffer);
            TerronEndingState = EC_Buf::TBufEC_GetByte(Buffer);
            PirateEndingState = EC_Buf::TBufEC_GetByte(Buffer);
            PlanetBattleHistory = nullptr;
            if (Marker >= 2) {
                Count = EC_Buf::TBufEC_GetWord(Buffer);
                PlanetBattleHistory.set_length(Count);
                {
                    const std::int32_t cpp_last_2 = PlanetBattleHistory.length() - 1;
                    if (0 <= cpp_last_2) {
                        for (I = 0; I <= cpp_last_2; ++I) {
                            PlanetBattleHistory[I].MapId = EC_Buf::TBufEC_GetInt32(Buffer);
                            PlanetBattleHistory[I].Statistics[0] = EC_Buf::TBufEC_GetInt32(Buffer);
                            PlanetBattleHistory[I].Statistics[1] = EC_Buf::TBufEC_GetInt32(Buffer);
                            PlanetBattleHistory[I].Statistics[2] = EC_Buf::TBufEC_GetInt32(Buffer);
                            PlanetBattleHistory[I].Statistics[3] = EC_Buf::TBufEC_GetInt32(Buffer);
                            PlanetBattleHistory[I].Statistics[4] = EC_Buf::TBufEC_GetInt32(Buffer);
                            PlanetBattleHistory[I].Statistics[5] = EC_Buf::TBufEC_GetInt32(Buffer);
                            PlanetBattleHistory[I].ResultCode = EC_Buf::TBufEC_GetByte(Buffer);
                            PlanetBattleHistory[I].CompletionMode = EC_Buf::TBufEC_GetByte(Buffer);
                            PlanetBattleHistory[I].DateTurn = EC_Buf::TBufEC_GetInt32(Buffer);
                        }
                    }
                }
            }
            RecalculateTotalScore();
        }
    }

    void TfScoreUnit::ExportToFile(pas::WideString FileName) {
        pas::WideString Text{};
        pas::AnsiString AnsiText{};
        std::uint8_t* Data{};
        std::int32_t I{};
        Text = pas::concat_wide({u"// Score for Space Rangers 2", u"\r\n"});
        Text = pas::concat_wide({Text, u"Name=", PlayerName, u"\r\n"});
        Text = pas::concat_wide({Text, u"EMail=", u"\r\n"});
        Text = pas::concat_wide({Text, u"Race=", aConst::OwnerInfo[aConst::RaceToOwner(PilotRace) & 0x0000007f].DisplayName, u"\r\n"});
        Text = pas::concat_wide({Text, u"Score=", pas::wide_int_to_str(TotalScore), u"\r\n"});
        Text = pas::concat_wide({Text, u"Level=", pas::wide_int_to_str(DifficultyPercent), u"\r\n"});
        Text = pas::concat_wide({Text, u"Date=", aGalaxy::FormatGameTurnDate(FinishedTurn), u"\r\n"});
        Text = pas::concat_wide({Text, u"Rank=", aConst::LocalizedText(pas::concat_wide({u"Rank.", aConst::CoalitionRankNames[Rank], u".Name"})), u"\r\n"});
        Text = pas::concat_wide({Text, u"LiberationSystem=", pas::wide_int_to_str(LiberatedSystemCount), u"\r\n"});
        Text = pas::concat_wide({Text, u"Rewards=", pas::wide_int_to_str(AwardCount), u"\r\n"});
        Text = pas::concat_wide({Text, u"SkillAccuracy=", pas::wide_int_to_str(static_cast<std::int32_t>(SkillLevels[0])), u"\r\n"});
        Text = pas::concat_wide({Text, u"SkillMobility=", pas::wide_int_to_str(static_cast<std::int32_t>(SkillLevels[1])), u"\r\n"});
        Text = pas::concat_wide({Text, u"SkillTechnical=", pas::wide_int_to_str(static_cast<std::int32_t>(SkillLevels[2])), u"\r\n"});
        Text = pas::concat_wide({Text, u"SkillTrader=", pas::wide_int_to_str(static_cast<std::int32_t>(SkillLevels[3])), u"\r\n"});
        Text = pas::concat_wide({Text, u"SkillCharm=", pas::wide_int_to_str(static_cast<std::int32_t>(SkillLevels[4])), u"\r\n"});
        Text = pas::concat_wide({Text, u"SkillLeadership=", pas::wide_int_to_str(static_cast<std::int32_t>(SkillLevels[5])), u"\r\n", u"\r\n", u"\r\n"});
        Text = pas::concat_wide({Text, u"*************** Protect database ****************", u"\r\n", u"\r\n"});
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        EC_Buf::TBufEC* Encoded = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        std::int32_t Seed = aMyFunction::RandomIntRange(0, 2000000000);
        SaveToBuffer(Buffer);
        if (SimpleSteamApi::SteamInitialized) {
            Buffer->AddAnsiStringZ(SysUtils::Int64ToStr(SimpleSteamApi::SteamUserId()));
        } else {
            Buffer->AddAnsiStringZ(SysUtils::IntToStr(0));
        }
        Buffer->CompressZlibPayloadInPlace(false);
        Buffer->ApplyDatXorCipher(Seed);
        Encoded->AddIntegerValue(3);
        Encoded->AddDWord(Seed ^ 0x140f3f9b);
        Encoded->AddDWord(0u);
        Encoded->AddDWord(0u);
        Encoded->AddBytes(Buffer->Data, Buffer->DataSize);
        Encoded->UpdateEmbeddedCrc32(0, Encoded->DataSize, 8);
        Buffer->Clear();
        Data = static_cast<std::uint8_t*>(Encoded->Data);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Encoded->DataSize - 1); cpp_range.next(I); ) {
            Buffer->AddAnsiStringRaw(static_cast<pas::AnsiString>(pas::concat_wide({u" ", ExceptionInfo::ByteToHexText(*Data)})));
            if ((I & 0x0000000f) == 0x0000000f) {
                Buffer->AddAnsiStringRaw("\r\n"_a);
            }
            Data = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Data) + 1);
        }
        AnsiText = static_cast<pas::AnsiString>(Text);
        EC_File::TFileEC* FileObject = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        try {
            FileObject->SetFileName(FileName);
            FileObject->CreateNew();
            FileObject->WriteBuffer(AnsiText.pchar(), AnsiText.length());
            FileObject->WriteBuffer(Buffer->Data, Buffer->DataSize);
        } catch (...) {
        }
        pas::free(FileObject);
        pas::free(Buffer);
        pas::free(Encoded);
    }

    void TfScore::SortAndTrimEntries() {
        TfScoreUnit* Candidate{};
        TfScoreUnit* Current{};
        pas::Object* Last{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Count = pas::list_count(Entries);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 2); cpp_range.next(I); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(I + 1, Count - 1); cpp_range_2.next(J); ) {
                Current = pas::list_at<TfScoreUnit>(Entries, I);
                Candidate = pas::list_at<TfScoreUnit>(Entries, J);
                if (fScore::ShouldSwapScoreEntries(Candidate, Current)) {
                    pas::list_put(Entries, I, reinterpret_cast<void*>(Candidate));
                    pas::list_put(Entries, J, reinterpret_cast<void*>(Current));
                }
            }
        }
        while (pas::list_count(Entries) > 11) {
            Last = pas::list_at<pas::Object>(Entries, pas::list_count(Entries) - 1);
            pas::free(Last);
            pas::list_delete(Entries, pas::list_count(Entries) - 1);
        }
    }

    std::uint8_t ShouldSwapScoreEntries(TfScoreUnit*& Candidate, TfScoreUnit*& Current) {
        if (Candidate->TotalScore < Current->TotalScore) {
            return false;
        }
        if (Candidate->TotalScore > Current->TotalScore) {
            return true;
        }
        if (Candidate->DifficultyPercent < Current->DifficultyPercent) {
            return false;
        }
        if (Candidate->DifficultyPercent < Current->DifficultyPercent) {
            return true;
        }
        if (Candidate->FinishedTurn > Current->FinishedTurn) {
            return false;
        }
        if (Candidate->FinishedTurn < Current->FinishedTurn) {
            return true;
        }
        return false;
    }

    void TfScore::InitializeDefaultEntry(std::int32_t Index, TfScoreUnit*& Entry) {
        std::int32_t I{};
        aGalaxyStruct::TQuestType Kind{};
        pas::Array<std::int32_t, 0, 4> QuestCounts{};
        switch (Index) {
            case 0: {
                Entry->PortraitFaceId = 0;
                Entry->DifficultyLevels[0] = 3;
                Entry->DifficultyLevels[1] = 3;
                Entry->DifficultyLevels[2] = 3;
                Entry->DifficultyLevels[3] = 3;
                Entry->DifficultyLevels[4] = 3;
                Entry->DifficultyLevels[5] = 3;
                Entry->DifficultyLevels[6] = 3;
                Entry->DifficultyLevels[7] = 3;
                Entry->FinishedTurn = 5600;
                Entry->OtherShipKillCount = 34;
                Entry->PirateKillCount = 50;
                Entry->DominatorKillCount = 170;
                Entry->ArcadeKillCount = 27;
                Entry->LiberatedSystemCount = 15;
                Entry->AwardCount = 14;
                Entry->TotalExperience = 100000;
                Entry->SkillLevels[0] = 4;
                Entry->SkillLevels[1] = 5;
                Entry->SkillLevels[2] = 4;
                Entry->SkillLevels[3] = 5;
                Entry->SkillLevels[4] = 5;
                Entry->SkillLevels[5] = 5;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 20;
                QuestCounts[aGalaxyStruct::qtKillShip] = 5;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 30;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 8;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 12;
                Entry->PlanetBattles = 7;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 3;
                Entry->KellerEndingState = 2;
                Entry->TerronEndingState = 2;
                break;
            }
            case 1: {
                Entry->PortraitFaceId = 9;
                Entry->DifficultyLevels[0] = 3;
                Entry->DifficultyLevels[1] = 3;
                Entry->DifficultyLevels[2] = 2;
                Entry->DifficultyLevels[3] = 2;
                Entry->DifficultyLevels[4] = 2;
                Entry->DifficultyLevels[5] = 2;
                Entry->DifficultyLevels[6] = 2;
                Entry->DifficultyLevels[7] = 2;
                Entry->FinishedTurn = 6000;
                Entry->OtherShipKillCount = 12;
                Entry->PirateKillCount = 95;
                Entry->DominatorKillCount = 280;
                Entry->ArcadeKillCount = 22;
                Entry->LiberatedSystemCount = 13;
                Entry->AwardCount = 11;
                Entry->TotalExperience = 90000;
                Entry->SkillLevels[0] = 5;
                Entry->SkillLevels[1] = 5;
                Entry->SkillLevels[2] = 4;
                Entry->SkillLevels[3] = 5;
                Entry->SkillLevels[4] = 3;
                Entry->SkillLevels[5] = 4;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 18;
                QuestCounts[aGalaxyStruct::qtKillShip] = 2;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 28;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 8;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 2;
                Entry->PlanetBattles = 6;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 2;
                Entry->KellerEndingState = 2;
                Entry->TerronEndingState = 2;
                break;
            }
            case 2: {
                Entry->PortraitFaceId = 11;
                Entry->DifficultyLevels[0] = 2;
                Entry->DifficultyLevels[1] = 2;
                Entry->DifficultyLevels[2] = 2;
                Entry->DifficultyLevels[3] = 2;
                Entry->DifficultyLevels[4] = 2;
                Entry->DifficultyLevels[5] = 2;
                Entry->DifficultyLevels[6] = 2;
                Entry->DifficultyLevels[7] = 2;
                Entry->FinishedTurn = 6500;
                Entry->OtherShipKillCount = 50;
                Entry->PirateKillCount = 46;
                Entry->DominatorKillCount = 308;
                Entry->ArcadeKillCount = 66;
                Entry->LiberatedSystemCount = 8;
                Entry->AwardCount = 7;
                Entry->TotalExperience = 85000;
                Entry->SkillLevels[0] = 5;
                Entry->SkillLevels[1] = 5;
                Entry->SkillLevels[2] = 4;
                Entry->SkillLevels[3] = 3;
                Entry->SkillLevels[4] = 3;
                Entry->SkillLevels[5] = 5;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 14;
                QuestCounts[aGalaxyStruct::qtKillShip] = 18;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 26;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 11;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 14;
                Entry->PlanetBattles = 11;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 1;
                Entry->KellerEndingState = 1;
                Entry->TerronEndingState = 1;
                break;
            }
            case 3: {
                Entry->PortraitFaceId = 13;
                Entry->DifficultyLevels[0] = 2;
                Entry->DifficultyLevels[1] = 2;
                Entry->DifficultyLevels[2] = 2;
                Entry->DifficultyLevels[3] = 2;
                Entry->DifficultyLevels[4] = 2;
                Entry->DifficultyLevels[5] = 2;
                Entry->DifficultyLevels[6] = 1;
                Entry->DifficultyLevels[7] = 1;
                Entry->FinishedTurn = 7000;
                Entry->OtherShipKillCount = 102;
                Entry->PirateKillCount = 2;
                Entry->DominatorKillCount = 135;
                Entry->ArcadeKillCount = 16;
                Entry->LiberatedSystemCount = 6;
                Entry->AwardCount = 12;
                Entry->TotalExperience = 80000;
                Entry->SkillLevels[0] = 3;
                Entry->SkillLevels[1] = 5;
                Entry->SkillLevels[2] = 4;
                Entry->SkillLevels[3] = 5;
                Entry->SkillLevels[4] = 5;
                Entry->SkillLevels[5] = 2;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 8;
                QuestCounts[aGalaxyStruct::qtKillShip] = 19;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 5;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 1;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 0;
                Entry->PlanetBattles = 9;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 2;
                Entry->KellerEndingState = 1;
                Entry->TerronEndingState = 3;
                break;
            }
            case 4: {
                Entry->PortraitFaceId = 3;
                Entry->DifficultyLevels[0] = 2;
                Entry->DifficultyLevels[1] = 2;
                Entry->DifficultyLevels[2] = 2;
                Entry->DifficultyLevels[3] = 2;
                Entry->DifficultyLevels[4] = 1;
                Entry->DifficultyLevels[5] = 1;
                Entry->DifficultyLevels[6] = 1;
                Entry->DifficultyLevels[7] = 1;
                Entry->FinishedTurn = 7500;
                Entry->OtherShipKillCount = 28;
                Entry->PirateKillCount = 110;
                Entry->DominatorKillCount = 282;
                Entry->ArcadeKillCount = 44;
                Entry->LiberatedSystemCount = 7;
                Entry->AwardCount = 11;
                Entry->TotalExperience = 76000;
                Entry->SkillLevels[0] = 5;
                Entry->SkillLevels[1] = 4;
                Entry->SkillLevels[2] = 3;
                Entry->SkillLevels[3] = 1;
                Entry->SkillLevels[4] = 2;
                Entry->SkillLevels[5] = 5;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 7;
                QuestCounts[aGalaxyStruct::qtKillShip] = 13;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 10;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 15;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 4;
                Entry->PlanetBattles = 3;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 1;
                Entry->KellerEndingState = 1;
                Entry->TerronEndingState = 1;
                break;
            }
            case 5: {
                Entry->PortraitFaceId = 10;
                Entry->DifficultyLevels[0] = 2;
                Entry->DifficultyLevels[1] = 2;
                Entry->DifficultyLevels[2] = 1;
                Entry->DifficultyLevels[3] = 1;
                Entry->DifficultyLevels[4] = 1;
                Entry->DifficultyLevels[5] = 1;
                Entry->DifficultyLevels[6] = 1;
                Entry->DifficultyLevels[7] = 1;
                Entry->FinishedTurn = 8000;
                Entry->OtherShipKillCount = 34;
                Entry->PirateKillCount = 50;
                Entry->DominatorKillCount = 148;
                Entry->ArcadeKillCount = 30;
                Entry->LiberatedSystemCount = 11;
                Entry->AwardCount = 10;
                Entry->TotalExperience = 63000;
                Entry->SkillLevels[0] = 3;
                Entry->SkillLevels[1] = 4;
                Entry->SkillLevels[2] = 4;
                Entry->SkillLevels[3] = 2;
                Entry->SkillLevels[4] = 5;
                Entry->SkillLevels[5] = 1;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 17;
                QuestCounts[aGalaxyStruct::qtKillShip] = 10;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 3;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 9;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 18;
                Entry->PlanetBattles = 5;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 3;
                Entry->KellerEndingState = 2;
                Entry->TerronEndingState = 1;
                break;
            }
            case 6: {
                Entry->PortraitFaceId = 10;
                Entry->DifficultyLevels[0] = 1;
                Entry->DifficultyLevels[1] = 1;
                Entry->DifficultyLevels[2] = 1;
                Entry->DifficultyLevels[3] = 1;
                Entry->DifficultyLevels[4] = 1;
                Entry->DifficultyLevels[5] = 1;
                Entry->DifficultyLevels[6] = 1;
                Entry->DifficultyLevels[7] = 1;
                Entry->FinishedTurn = 8500;
                Entry->OtherShipKillCount = 40;
                Entry->PirateKillCount = 30;
                Entry->DominatorKillCount = 180;
                Entry->ArcadeKillCount = 52;
                Entry->LiberatedSystemCount = 8;
                Entry->AwardCount = 7;
                Entry->TotalExperience = 52000;
                Entry->SkillLevels[0] = 2;
                Entry->SkillLevels[1] = 5;
                Entry->SkillLevels[2] = 2;
                Entry->SkillLevels[3] = 4;
                Entry->SkillLevels[4] = 2;
                Entry->SkillLevels[5] = 3;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 5;
                QuestCounts[aGalaxyStruct::qtKillShip] = 20;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 6;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 18;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 4;
                Entry->PlanetBattles = 2;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 2;
                Entry->KellerEndingState = 1;
                Entry->TerronEndingState = 2;
                break;
            }
            case 7: {
                Entry->PortraitFaceId = 11;
                Entry->DifficultyLevels[0] = 1;
                Entry->DifficultyLevels[1] = 1;
                Entry->DifficultyLevels[2] = 1;
                Entry->DifficultyLevels[3] = 1;
                Entry->DifficultyLevels[4] = 1;
                Entry->DifficultyLevels[5] = 1;
                Entry->DifficultyLevels[6] = 0;
                Entry->DifficultyLevels[7] = 0;
                Entry->FinishedTurn = 9000;
                Entry->OtherShipKillCount = 12;
                Entry->PirateKillCount = 25;
                Entry->DominatorKillCount = 92;
                Entry->ArcadeKillCount = 1;
                Entry->LiberatedSystemCount = 6;
                Entry->AwardCount = 9;
                Entry->TotalExperience = 45000;
                Entry->SkillLevels[0] = 4;
                Entry->SkillLevels[1] = 2;
                Entry->SkillLevels[2] = 5;
                Entry->SkillLevels[3] = 1;
                Entry->SkillLevels[4] = 1;
                Entry->SkillLevels[5] = 0;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 10;
                QuestCounts[aGalaxyStruct::qtKillShip] = 18;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 15;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 10;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 0;
                Entry->PlanetBattles = 1;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 2;
                Entry->KellerEndingState = 1;
                Entry->TerronEndingState = 1;
                break;
            }
            case 8: {
                Entry->PortraitFaceId = 7;
                Entry->DifficultyLevels[0] = 1;
                Entry->DifficultyLevels[1] = 1;
                Entry->DifficultyLevels[2] = 1;
                Entry->DifficultyLevels[3] = 1;
                Entry->DifficultyLevels[4] = 0;
                Entry->DifficultyLevels[5] = 0;
                Entry->DifficultyLevels[6] = 0;
                Entry->DifficultyLevels[7] = 0;
                Entry->FinishedTurn = 9300;
                Entry->OtherShipKillCount = 40;
                Entry->PirateKillCount = 2;
                Entry->DominatorKillCount = 74;
                Entry->ArcadeKillCount = 10;
                Entry->LiberatedSystemCount = 5;
                Entry->AwardCount = 7;
                Entry->TotalExperience = 38500;
                Entry->SkillLevels[0] = 2;
                Entry->SkillLevels[1] = 3;
                Entry->SkillLevels[2] = 4;
                Entry->SkillLevels[3] = 4;
                Entry->SkillLevels[4] = 0;
                Entry->SkillLevels[5] = 2;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 16;
                QuestCounts[aGalaxyStruct::qtKillShip] = 3;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 6;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 5;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 8;
                Entry->PlanetBattles = 2;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 2;
                Entry->KellerEndingState = 2;
                Entry->TerronEndingState = 2;
                break;
            }
            case 9: {
                Entry->PortraitFaceId = 28;
                Entry->DifficultyLevels[0] = 1;
                Entry->DifficultyLevels[1] = 1;
                Entry->DifficultyLevels[2] = 0;
                Entry->DifficultyLevels[3] = 0;
                Entry->DifficultyLevels[4] = 0;
                Entry->DifficultyLevels[5] = 0;
                Entry->DifficultyLevels[6] = 0;
                Entry->DifficultyLevels[7] = 0;
                Entry->FinishedTurn = 9500;
                Entry->OtherShipKillCount = 8;
                Entry->PirateKillCount = 9;
                Entry->DominatorKillCount = 45;
                Entry->ArcadeKillCount = 2;
                Entry->LiberatedSystemCount = 3;
                Entry->AwardCount = 5;
                Entry->TotalExperience = 33000;
                Entry->SkillLevels[0] = 0;
                Entry->SkillLevels[1] = 3;
                Entry->SkillLevels[2] = 0;
                Entry->SkillLevels[3] = 3;
                Entry->SkillLevels[4] = 5;
                Entry->SkillLevels[5] = 3;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 5;
                QuestCounts[aGalaxyStruct::qtKillShip] = 13;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 8;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 0;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 0;
                Entry->PlanetBattles = 3;
                Entry->VictoryAchieved = true;
                Entry->BlazerEndingState = 1;
                Entry->KellerEndingState = 1;
                Entry->TerronEndingState = 3;
                break;
            }
            case 10: {
                Entry->PortraitFaceId = 12;
                Entry->DifficultyLevels[0] = 0;
                Entry->DifficultyLevels[1] = 0;
                Entry->DifficultyLevels[2] = 0;
                Entry->DifficultyLevels[3] = 0;
                Entry->DifficultyLevels[4] = 0;
                Entry->DifficultyLevels[5] = 0;
                Entry->DifficultyLevels[6] = 0;
                Entry->DifficultyLevels[7] = 0;
                Entry->FinishedTurn = 3000;
                Entry->OtherShipKillCount = 63;
                Entry->PirateKillCount = 7;
                Entry->DominatorKillCount = 40;
                Entry->ArcadeKillCount = 28;
                Entry->LiberatedSystemCount = 1;
                Entry->AwardCount = 1;
                Entry->TotalExperience = 12000;
                Entry->SkillLevels[0] = 2;
                Entry->SkillLevels[1] = 3;
                Entry->SkillLevels[2] = 1;
                Entry->SkillLevels[3] = 1;
                Entry->SkillLevels[4] = 2;
                Entry->SkillLevels[5] = 1;
                QuestCounts[aGalaxyStruct::qtSendLetter] = 10;
                QuestCounts[aGalaxyStruct::qtKillShip] = 0;
                QuestCounts[aGalaxyStruct::qtPlanetQuest] = 6;
                QuestCounts[aGalaxyStruct::qtDefendSystem] = 0;
                QuestCounts[aGalaxyStruct::qtDefendShip] = 4;
                Entry->PlanetBattles = 0;
                Entry->VictoryAchieved = false;
                Entry->BlazerEndingState = 1;
                Entry->KellerEndingState = 0;
                Entry->TerronEndingState = 0;
                break;
            }
        }
        Entry->PlayerName = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"FormScore.Winners.", SysUtils::IntToStr(Index), ".Name"})));
        Entry->PilotRace = aConst::OwnerToRace(aConst::OwnerFromInternalName(GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"FormScore.Winners.", SysUtils::IntToStr(Index), ".Race"})))));
        Entry->Rank = System::Round(aMyFunction::RemapClamped(Index, 0.0, 1.0E+1, 6.0, 3.0));
        std::int32_t Count = QuestCounts[aGalaxyStruct::qtSendLetter] + QuestCounts[aGalaxyStruct::qtKillShip] + QuestCounts[aGalaxyStruct::qtPlanetQuest] + QuestCounts[aGalaxyStruct::qtDefendSystem] + QuestCounts[aGalaxyStruct::qtDefendShip];
        Entry->QuestResults.set_length(Count);
        Count = 0;
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TQuestType>(aGalaxyStruct::qtSendLetter, aGalaxyStruct::qtDefendShip); cpp_range.next(Kind); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, QuestCounts[Kind] - 1); cpp_range_2.next(I); ) {
                Entry->QuestResults[Count].Successful = true;
                Entry->QuestResults[Count].QuestType = Kind;
                Entry->QuestResults[Count].QuestNumber = Count;
                ++Count;
            }
        }
        Entry->RecalculateTotalScore();
    }

    void TfScore::CreateDefaultTable() {
        TfScoreUnit* Entry{};
        std::int32_t I{};
        ClearEntries();
        for (I = 0; I <= 10; ++I) {
            Entry = pas::construct_call<TfScoreUnit>(TfScoreUnit_Create);
            pas::list_add(Entries, reinterpret_cast<void*>(Entry));
            TfScore::InitializeDefaultEntry(I, Entry);
        }
        SortAndTrimEntries();
    }

    void TfScore::RecordPlayerResult(std::uint8_t Victory) {
        std::int32_t I{};
        std::int32_t J{};
        TfScoreUnit* Other{};
        if (SimpleSteamApi::SteamInitialized && static_cast<std::uint8_t>(SimpleSteamApi::SteamLeaderboardFound() ^ 1)) {
            SimpleSteamApi::SteamSetLeaderboardName("Scores"_a);
        }
        ReloadTable();
        std::int32_t Count = pas::list_count(Entries);
        if (Count != 11) {
            GR_Main::RaiseWideMessage(u"Score sort"_wref.get());
        }
        aGalaxy::TGalaxy::AppendIntegritySnapshot();
        TfScoreUnit* Entry = pas::construct_call<TfScoreUnit>(TfScoreUnit_Create);
        Entry->CapturePlayer(Victory);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Entries) - 1); cpp_range.next(I); ) {
            Other = pas::list_at<TfScoreUnit>(Entries, I);
            if (Entry->TotalScore == Other->TotalScore && Entry->GenerationSeed == Other->GenerationSeed) {
                Other->CapturePlayer(Victory);
                SaveTableToDisk();
                SelectedIndex = I;
                pas::free(Entry);
                return;
            }
        }
        pas::free(Entry);
        pas::list_at<TfScoreUnit>(Entries, pas::list_count(Entries) - 1)->CapturePlayer(Victory);
        SelectedIndex = Count - 1;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 2); cpp_range_2.next(I); ) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(I + 1, Count - 1); cpp_range_3.next(J); ) {
                Entry = pas::list_at<TfScoreUnit>(Entries, I);
                Other = pas::list_at<TfScoreUnit>(Entries, J);
                if (Entry->TotalScore < Other->TotalScore) {
                    pas::list_put(Entries, I, reinterpret_cast<void*>(Other));
                    pas::list_put(Entries, J, reinterpret_cast<void*>(Entry));
                    if (SelectedIndex == I) {
                        SelectedIndex = J;
                    } else if (SelectedIndex == J) {
                        SelectedIndex = I;
                    }
                }
            }
        }
        SaveTableToDisk();
    }

    void TfScore::RemoveSelectedEntryAndRefill() {
        std::int32_t I{};
        TfScoreUnit* Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Entries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TfScoreUnit>(Entries, I);
            if (Entry->ScoreTags->DataSize <= 0 || I == SelectedIndex) {
                pas::free(Entry);
                pas::list_put(Entries, I, nullptr);
            }
        }
        I = 0;
        while (I < pas::list_count(Entries)) {
            if (pas::list_get(Entries, I) == nullptr) {
                pas::list_delete(Entries, I);
            } else {
                ++I;
            }
        }
        for (I = 0; I <= 10; ++I) {
            Entry = pas::construct_call<TfScoreUnit>(TfScoreUnit_Create);
            pas::list_add(Entries, reinterpret_cast<void*>(Entry));
            TfScore::InitializeDefaultEntry(I, Entry);
        }
        SortAndTrimEntries();
    }

    void TfScore::ClearEntries() {
        std::int32_t I{};
        pas::Object* Entry{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Entries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<pas::Object>(Entries, I);
            pas::free(Entry);
        }
        pas::list_clear(Entries);
    }

    void TfScore::LoadTableFromDisk() {
        pas::WideString cpp_text{};
        std::uint8_t* Data{};
        std::int32_t I{};
        std::int32_t Size{};
        std::int32_t Seed{};
        std::uint32_t Checksum{};
        TfScoreUnit* Entry{};
        std::int32_t Version{};
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        {
            try {
                try {
                    Buffer->LoadFromWideFilePath((cpp_text = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"score.dat"}), cpp_text.pchar()));
                    Buffer->ExpandZlibPayloadInPlace();
                    Version = Buffer->GetInt32At(0);
                    if (Version != 2) {
                        pas::raise(pas::make_exception<pas::Abort>("Error unpacking score.dat"_a));
                    }
                    {
                        std::int32_t cpp_left_3 = Buffer->GetByteAt(6);
                        std::int32_t cpp_left_2 = cpp_left_3 | pas::shl(static_cast<std::int32_t>(Buffer->GetByteAt(7)), 8);
                        std::int32_t cpp_left = cpp_left_2 | pas::shl(static_cast<std::int32_t>(Buffer->GetByteAt(4)), 16);
                        Seed = cpp_left | pas::shl(static_cast<std::int32_t>(Buffer->GetByteAt(5)), 24);
                    }
                    Data = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Buffer->Data)) + 8)));
                    Size = Buffer->DataSize;
                    for (auto cpp_range = pas::for_to<std::int32_t>(8, Size - 1); cpp_range.next(I); ) {
                        *Data = *Data ^ static_cast<std::uint8_t>(Seed - 1);
                        Seed = 16807 * (Seed % 127773) - 2836 * (Seed / 127773);
                        if (Seed <= 0) {
                            Seed += SystemImports::MaxInt;
                        }
                        Data = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Data) + 1);
                    }
                    Checksum = 0u;
                    Data = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Buffer->Data)) + 12)));
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(12, Size - 1); cpp_range_2.next(I); ) {
                        Checksum += static_cast<std::uint8_t>(*Data ^ 0x000000ff);
                        Data = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Data) + 1);
                    }
                    if (Buffer->GetUInt32At(8) != Checksum) {
                        pas::raise(pas::make_exception<pas::Abort>("Error unpacking score.dat"_a));
                    }
                    Buffer->SetPosition(12);
                    for (I = 0; I <= 10; ++I) {
                        Entry = pas::construct_call<TfScoreUnit>(TfScoreUnit_Create);
                        pas::list_add(Entries, reinterpret_cast<void*>(Entry));
                        Entry->LoadFromBuffer(Buffer, Version);
                    }
                } catch (...) {
                    CreateDefaultTable();
                }
            } catch (...) {
                pas::free(Buffer);
                throw;
            }
            pas::free(Buffer);
        }
    }

    void TfScore::ReloadTable() {
        ClearEntries();
        if (!SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"score.dat"})))) {
            CreateDefaultTable();
        } else {
            LoadTableFromDisk();
            SortAndTrimEntries();
        }
    }

    void TfScore::SaveTableToDisk() {
        std::int32_t I{};
        TfScoreUnit* Entry{};
        if (pas::list_count(Entries) != 11) {
            CreateDefaultTable();
        }
        std::int32_t Seed = pas::random(SystemImports::MaxInt, &System::RandSeed);
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        Buffer->AddIntegerValue(2);
        Buffer->AddIntegerValue(0);
        Buffer->AddIntegerValue(0);
        Buffer->SetByteAt(6, Seed);
        Buffer->SetByteAt(7, pas::shr(Seed, 8));
        Buffer->SetByteAt(4, pas::shr(Seed, 16));
        Buffer->SetByteAt(5, pas::shr(Seed, 24));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Entries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TfScoreUnit>(Entries, I);
            Entry->SaveToBuffer(Buffer);
        }
        std::int32_t Size = Buffer->DataSize;
        std::int32_t Checksum = 0;
        std::uint8_t* Data = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Buffer->Data)) + 12)));
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(12, Size - 1); cpp_range_2.next(I); ) {
            Checksum += static_cast<std::uint8_t>(*Data ^ 0x000000ff);
            Data = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Data) + 1);
        }
        Buffer->SetInt32At(8, Checksum);
        Data = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Buffer->Data)) + 8)));
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(8, Size - 1); cpp_range_3.next(I); ) {
            *Data = *Data ^ static_cast<std::uint8_t>(Seed - 1);
            Seed = 16807 * (Seed % 127773) - 2836 * (Seed / 127773);
            if (Seed <= 0) {
                Seed += SystemImports::MaxInt;
            }
            Data = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uint8_t*>(Data) + 1);
        }
        Buffer->CompressZlibPayloadInPlace(false);
        EC_File::TFileEC* FileObject = pas::construct_call<EC_File::TFileEC>(EC_File::TFileEC_Create);
        FileObject->SetFileName(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"score.dat"}));
        FileObject->CreateNew();
        FileObject->WriteBuffer(Buffer->Data, Buffer->DataSize);
        pas::free(FileObject);
        pas::free(Buffer);
    }

    void TfScore_Create(TfScore* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->Entries = pas::make_object<pas::List>();
    }

    void TfScore_Destroy(TfScore* Self) {
        Self->ClearEntries();
        pas::free(Self->Entries);
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfScore::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("fScore... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FirstChild->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* PanelToServer = MainPanel->FindByNameRecursive(u"PanelToServer"_wref.get());
                PanelToServer->SetPosition(ClassesImports::Point(PanelToServer->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelToServer->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* PanelWin = MainPanel->FindByNameRecursive(u"PanelWin"_wref.get());
                PanelWin->SetPosition(ClassesImports::Point(PanelWin->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelWin->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        SetHelpCallback(pas::bind_method<&TfScore::ShowControlHelp>(this));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClear"_wref.get()))->UpCallback = pas::bind_method<&TfScore::DeleteEntryClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExit"_wref.get()))->UpCallback = pas::bind_method<&TfScore::CloseClicked>(this);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfScore::KeyDown>(this);
        SelectedIndex = 0;
    }

    void TfScore::OnOpen() {
        std::int32_t I{};
        GI_Panel::TPanelGI* Row{};
        GI_Panel::TPanelGI* SendPanel{};
        if (SimpleSteamApi::SteamInitialized && static_cast<std::uint8_t>(SimpleSteamApi::SteamLeaderboardFound() ^ 1)) {
            SimpleSteamApi::SteamSetLeaderboardName("Scores"_a);
        }
        if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
            pas::free(aSaveLoad::MemorySnapshotBuffer);
        }
        aSaveLoad::MemorySnapshotBuffer = nullptr;
        GlobalsV::MemorySnapshotActive = false;
        if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
            pas::free(aGalaxy::Galaxy);
        }
        aGalaxy::Galaxy = nullptr;
        GetByName(u"LabelHelp"_wref.get())->SetActive(false);
        GI_Panel::TPanelGI* Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelSlot"_wref.get()));
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 10); cpp_range.next(I); ) {
            Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
            if (GR_Main::GiResourceVariant() == 1) {
                Row->SetPosition(ClassesImports::Point(0, I * 40));
                Row->SetSize(ClassesImports::Point(Panel->ClientSize.X, 40));
            } else {
                Row->SetPosition(ClassesImports::Point(0, I * 51));
                Row->SetSize(ClassesImports::Point(Panel->ClientSize.X, 51));
            }
            Row->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I)})));
            Row->LeftButtonDownCallback = pas::bind_method<&TfScore::EntryMouseDown>(this);
            Row->MouseEnterCallback = pas::bind_method<&TfScore::EntryMouseEnter>(this);
            Row->MouseLeaveCallback = pas::bind_method<&TfScore::EntryMouseLeave>(this);
            Row->UserValue = I;
            {
                GI_Image::TImageGI* cpp_with = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
                cpp_with->SetDepthByName(u"99"_wref.get());
                cpp_with->SetPosition(ClassesImports::Point(0, 0));
                if (GR_Main::GiResourceVariant() == 1) {
                    cpp_with->SetSize(ClassesImports::Point(454, 40));
                } else {
                    cpp_with->SetSize(ClassesImports::Point(577, 51));
                }
                cpp_with->SetActive(true);
                cpp_with->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Active"})));
            }
            {
                GI_Label::TLabelGI* cpp_with_2 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
                if (GR_Main::GiResourceVariant() == 1) {
                    cpp_with_2->SetPosition(ClassesImports::Point(6, 4));
                    cpp_with_2->SetSize(ClassesImports::Point(45, 32));
                } else {
                    cpp_with_2->SetPosition(ClassesImports::Point(5, 4));
                    cpp_with_2->SetSize(ClassesImports::Point(59, 43));
                }
                cpp_with_2->SetDepthByName(u"98"_wref.get());
                cpp_with_2->SetFontName(GlobalsV::NormalFontName);
                cpp_with_2->SetTextAlignX(GI_Main::taxCenter);
                cpp_with_2->SetTextAlignY(GI_Main::tayCenterEx);
                cpp_with_2->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Nom"})));
                cpp_with_2->SetText(pas::wide_int_to_str(I + 1));
            }
            {
                GI_Label::TLabelGI* cpp_with_3 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
                if (GR_Main::GiResourceVariant() == 1) {
                    cpp_with_3->SetPosition(ClassesImports::Point(370, 4));
                    cpp_with_3->SetSize(ClassesImports::Point(80, 28));
                } else {
                    cpp_with_3->SetPosition(ClassesImports::Point(471, 4));
                    cpp_with_3->SetSize(ClassesImports::Point(101, 43));
                }
                cpp_with_3->SetDepthByName(u"98"_wref.get());
                cpp_with_3->SetFontName(GlobalsV::NormalBoldFontName);
                cpp_with_3->SetTextAlignX(GI_Main::taxCenter);
                cpp_with_3->SetTextAlignY(GI_Main::tayCenterEx);
                cpp_with_3->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Score"})));
            }
            {
                GI_Label::TLabelGI* cpp_with_4 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
                if (GR_Main::GiResourceVariant() == 1) {
                    cpp_with_4->SetPosition(ClassesImports::Point(117, 4));
                    cpp_with_4->SetSize(ClassesImports::Point(173, 32));
                } else {
                    cpp_with_4->SetPosition(ClassesImports::Point(148, 4));
                    cpp_with_4->SetSize(ClassesImports::Point(221, 43));
                }
                cpp_with_4->SetDepthByName(u"98"_wref.get());
                cpp_with_4->SetFontName(GlobalsV::NormalBoldFontName);
                cpp_with_4->SetTextAlignX(GI_Main::taxCenter);
                cpp_with_4->SetTextAlignY(GI_Main::tayCenterEx);
                cpp_with_4->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Name"})));
            }
            {
                GI_Label::TLabelGI* cpp_with_5 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
                if (GR_Main::GiResourceVariant() == 1) {
                    cpp_with_5->SetPosition(ClassesImports::Point(291, 4));
                    cpp_with_5->SetSize(ClassesImports::Point(78, 32));
                } else {
                    cpp_with_5->SetPosition(ClassesImports::Point(371, 4));
                    cpp_with_5->SetSize(ClassesImports::Point(99, 43));
                }
                cpp_with_5->SetDepthByName(u"98"_wref.get());
                cpp_with_5->SetFontName(GlobalsV::NormalFontName);
                cpp_with_5->SetTextAlignX(GI_Main::taxCenter);
                cpp_with_5->SetTextAlignY(GI_Main::tayCenterEx);
                cpp_with_5->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Code"})));
            }
            SendPanel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelToServer"_wref.get())));
            SendPanel->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "ToServer"})));
            SendPanel->SetPosition(ClassesImports::Point(0, I * GR_Main::GiScalePixelsEx(51, 40)));
            SendPanel->SetSize(ClassesImports::Point(GR_Main::GiScalePixelsEx(56, 45), GR_Main::GiScalePixelsEx(55, 44)));
            {
                GI_Image::TImageGI* cpp_with_6 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, SendPanel);
                cpp_with_6->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"SendPanel"}));
                cpp_with_6->SetPosition(ClassesImports::Point(0, 0));
                cpp_with_6->SetSize(cpp_with_6->GetContentSize());
                cpp_with_6->SetDepth(9.0);
            }
            {
                GI_Image::TImageGI* cpp_with_7 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, SendPanel);
                cpp_with_7->SetName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "ToServerLight"})));
                cpp_with_7->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"SendLight"}));
                cpp_with_7->SetPosition(ClassesImports::Point(GR_Main::GiScalePixelsEx(20, 16), GR_Main::GiScalePixelsEx(4, 5)));
                cpp_with_7->SetSize(cpp_with_7->GetContentSize());
                cpp_with_7->SetDepth(8.0);
                cpp_with_7->SetActive(true);
            }
            {
                GI_GraphButton::TGraphButtonGI* cpp_with_8 = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, SendPanel);
                cpp_with_8->UpCallback = pas::bind_method<&TfScore::ExportEntryClicked>(this);
                cpp_with_8->SetPosition(ClassesImports::Point(GR_Main::GiScalePixelsEx(12, 10), GR_Main::GiScalePixelsEx(9, 7)));
                cpp_with_8->SetDepth(7.0);
                cpp_with_8->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"SendN"}));
                cpp_with_8->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"SendA"}));
                cpp_with_8->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"SendD"}));
                cpp_with_8->EnterSound = u"Sound.ButtonEnter"_w;
                cpp_with_8->LeaveSound = u"Sound.ButtonLeave"_w;
                cpp_with_8->ClickSound = u"Sound.ButtonClick"_w;
                cpp_with_8->HitKind = GI_GraphButton::gbhGraph;
                cpp_with_8->MouseBlocking = true;
                cpp_with_8->SetSize(cpp_with_8->GetMaxStateImageSize());
                cpp_with_8->UserValue = I;
                cpp_with_8->HelpText = GR_Main::LookupLocalizedTextByKey(u"FormScore.HelpToServer"_wref.get());
                cpp_with_8->HelpCallback = pas::bind_method<&TfScore::ShowControlHelp>(this);
            }
        }
        ReloadTable();
        RefreshDetails();
    }

    void TfScore::OnClose() {
        {
            GI_Panel::TPanelGI* PanelSlot = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelSlot"_wref.get()));
            PanelSlot->FreeOwnedChildren();
        }
        {
            GI_Panel::TPanelGI* PanelToServer = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelToServer"_wref.get()));
            PanelToServer->FreeOwnedChildren();
        }
    }

    void TfScore::EntryMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender->UserValue != SelectedIndex) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonEnter"_wref.get());
            {
                GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(Sender->UserValue), "Active"}))));
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(pas::list_at<TfScoreUnit>(Entries, Sender->UserValue)->PilotRace) & 0x0000007f].InternalName, u"A"}));
            }
        }
    }

    void TfScore::EntryMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (Sender->UserValue != SelectedIndex) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonLeave"_wref.get());
            {
                GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(Sender->UserValue), "Active"}))));
                cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(pas::list_at<TfScoreUnit>(Entries, Sender->UserValue)->PilotRace) & 0x0000007f].InternalName, u"N"}));
            }
        }
    }

    void TfScore::DeleteEntryClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        TfScoreUnit* Entry = pas::list_at<TfScoreUnit>(Entries, SelectedIndex);
        Text = ([&] {
            auto playerName = pas::borrow(Entry->PlayerName);
            pas::WideString intToStr = pas::wide_int_to_str(Entry->TotalScore);
            pas::WideString paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormScore.QueryDelete"_wref.get());
            return aMyFunction::FormatText2(std::move(paramByPathOrMarker), u"<color=255,240,100>"_w, u"<Name>"_w, playerName.get(), u"<Score>"_w, std::move(intToStr));
        }());
        if (GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
            GR_Main::PostMouseMoveMessage();
        } else {
            GR_Main::PostMouseMoveMessage();
            RemoveSelectedEntryAndRefill();
            SaveTableToDisk();
            RefreshDetails();
        }
    }

    void TfScore::ClearTableClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormScore.QueryClear"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) != GI_MessageBox::mbgResultOK) {
            GR_Main::PostMouseMoveMessage();
        } else {
            GR_Main::PostMouseMoveMessage();
            SysUtilsImports::DeleteFile(static_cast<pas::AnsiString>(pas::concat_wide({GR_Main::GetGameUserDirectory(), u"score.dat"})));
            ReloadTable();
            SaveTableToDisk();
            RefreshDetails();
        }
    }

    void TfScore::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        Globals::ScreenLoadMode = 4;
        GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
        GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
        RequestClose(1);
    }

    void TfScore::KeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Key == WindowsSdk::VK_ESCAPE || Key == WindowsSdk::VK_RETURN) {
            CloseClicked(nullptr);
        } else if (Key == 'C') {
            ClearTableClicked(nullptr);
        } else if (Key == WindowsSdk::VK_HOME || Key == WindowsSdk::VK_PRIOR) {
            SelectedIndex = 0;
            RefreshDetails();
        } else if (Key == WindowsSdk::VK_END || Key == WindowsSdk::VK_NEXT) {
            SelectedIndex = pas::list_count(Entries) - 1;
            RefreshDetails();
        } else if (Key == WindowsSdk::VK_UP || Key == WindowsSdk::VK_LEFT) {
            SelectedIndex = std::max<std::int32_t>(0, SelectedIndex - 1);
            RefreshDetails();
        } else if (Key == WindowsSdk::VK_DOWN || Key == WindowsSdk::VK_RIGHT) {
            SelectedIndex = std::min<std::int32_t>(pas::list_count(Entries) - 1, SelectedIndex + 1);
            RefreshDetails();
        }
    }

    void TfScore::RefreshDetails() {
        std::int32_t I{};
        std::int32_t X{};
        TfScoreUnit* Entry{};
        std::uint8_t Selected{};
        std::uint16_t LetterQuests{};
        std::uint16_t ShipKillQuests{};
        std::uint16_t PlanetQuests{};
        std::uint16_t SystemDefenseQuests{};
        std::uint16_t ShipDefenseQuests{};
        GI_MessageLoop::TObjectGI* Control{};
        pas::WideString Text{};
        pas::WideString Separator{};
        pas::WideString ResolvedColor{};
        pas::WideString UnresolvedColor{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Entries) - 1); cpp_range.next(I); ) {
            Entry = pas::list_at<TfScoreUnit>(Entries, I);
            Selected = I == SelectedIndex;
            {
                GI_Image::TImageGI* cpp_with = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Active"}))));
                if (Selected) {
                    cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(Entry->PilotRace) & 0x0000007f].InternalName, u"D"}));
                } else {
                    cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(Entry->PilotRace) & 0x0000007f].InternalName, u"N"}));
                }
            }
            {
                GI_Label::TLabelGI* cpp_with_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Nom"}))));
                if (Selected) {
                    cpp_with_2->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 222, 0));
                } else {
                    cpp_with_2->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
                }
            }
            {
                GI_Label::TLabelGI* cpp_with_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Score"}))));
                if (Selected) {
                    cpp_with_3->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 222, 0));
                } else {
                    cpp_with_3->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
                }
                cpp_with_3->SetText(pas::wide_int_to_str(Entry->TotalScore));
            }
            {
                GI_Label::TLabelGI* cpp_with_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Name"}))));
                cpp_with_4->SetText(Entry->PlayerName);
                if (Selected) {
                    cpp_with_4->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 222, 0));
                } else {
                    cpp_with_4->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
                }
            }
            {
                GI_Label::TLabelGI* cpp_with_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "Code"}))));
                cpp_with_5->SetText(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Entry->DifficultyPercent), "%"})));
                if (Selected) {
                    cpp_with_5->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 222, 0));
                } else {
                    cpp_with_5->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
                }
            }
            {
                GI_Panel::TPanelGI* cpp_with_6 = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "ToServer"}))));
                cpp_with_6->SetActive(false);
            }
            {
                GI_MessageLoop::TObjectGI* cpp_with_7 = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Slot", SysUtils::IntToStr(I), "ToServerLight"})));
                cpp_with_7->SetActive(pas::list_at<TfScoreUnit>(Entries, I)->Exported);
            }
        }
        Entry = pas::list_at<TfScoreUnit>(Entries, SelectedIndex);
        {
            GI_GraphButton::TGraphButtonGI* ButClear = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClear"_wref.get()));
            ButClear->SetDisabled(Entry->ScoreTags->DataSize <= 0);
        }
        {
            GI_Image::TImageGI* CaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"_wref.get()));
            CaptainI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(Entry->PilotRace) & 0x0000007f].InternalName, pas::wide_int_to_str(Entry->PortraitFaceId), u"i"}));
            CaptainI->SetImageKindX(GI_Main::ikxCenter);
            CaptainI->SetImageKindY(GI_Main::ikyCenter);
            CaptainI->SetActive(true);
        }
        {
            GI_GAI::TgaiGI* CaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"_wref.get()));
            CaptainA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
            CaptainA->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), aConst::OwnerInfo[aConst::RaceToOwner(Entry->PilotRace) & 0x0000007f].InternalName, pas::wide_int_to_str(Entry->PortraitFaceId), u"a"}));
            CaptainA->SequenceIndex = 0;
            CaptainA->UpdateAutoGeometry();
            CaptainA->SetImageKindX(GI_Main::ikxCenter);
            CaptainA->SetImageKindY(GI_Main::ikyCenter);
            CaptainA->SetActive(true);
            CaptainA->RestartPlayback();
        }
        {
            GI_Image::TImageGI* IRankImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"IRankImage"_wref.get()));
            if (Entry->Rank == 0) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank0"}));
            } else if (Entry->Rank == 1) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank1"}));
            } else if (Entry->Rank == 2) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank2"}));
            } else if (Entry->Rank == 3) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank3"}));
            } else if (Entry->Rank == 4) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank4"}));
            } else if (Entry->Rank == 5) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank5"}));
            } else if (Entry->Rank == 6) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank6"}));
            } else if (Entry->Rank == 7) {
                IRankImage->SetImagePath(pas::concat_wide({u"GI,Bm.FormShip.", GR_Main::GiResourceSuffix(), u"Rank7"}));
            }
        }
        {
            GI_Image::TImageGI* Skill0 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Skill0"_wref.get()));
            Skill0->SetActive(Entry->SkillLevels[0] > 0);
            if (Skill0->Active) {
                Skill0->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(Entry->SkillLevels[0] - 1)}));
            }
        }
        {
            GI_Image::TImageGI* Skill1 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Skill1"_wref.get()));
            Skill1->SetActive(Entry->SkillLevels[1] > 0);
            if (Skill1->Active) {
                Skill1->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(Entry->SkillLevels[1] - 1)}));
            }
        }
        {
            GI_Image::TImageGI* Skill2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Skill2"_wref.get()));
            Skill2->SetActive(Entry->SkillLevels[2] > 0);
            if (Skill2->Active) {
                Skill2->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(Entry->SkillLevels[2] - 1)}));
            }
        }
        {
            GI_Image::TImageGI* Skill3 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Skill3"_wref.get()));
            Skill3->SetActive(Entry->SkillLevels[3] > 0);
            if (Skill3->Active) {
                Skill3->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(Entry->SkillLevels[3] - 1)}));
            }
        }
        {
            GI_Image::TImageGI* Skill4 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Skill4"_wref.get()));
            Skill4->SetActive(Entry->SkillLevels[4] > 0);
            if (Skill4->Active) {
                Skill4->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(Entry->SkillLevels[4] - 1)}));
            }
        }
        {
            GI_Image::TImageGI* Skill5 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Skill5"_wref.get()));
            Skill5->SetActive(Entry->SkillLevels[5] > 0);
            if (Skill5->Active) {
                Skill5->SetImagePath(pas::concat_wide({u"GI,Bm.FormScore2.", GR_Main::GiResourceSuffix(), u"Skill", pas::wide_int_to_str(Entry->SkillLevels[5] - 1)}));
            }
        }
        {
            const pas::WideString& formatText1 = ([&] {
                pas::WideString formatGameTurnDate = aGalaxy::FormatGameTurnDate(Entry->FinishedTurn);
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormScore.DateWin"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,222,0>"_w, u"<Date>"_w, std::move(formatGameTurnDate));
            }());
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IDate"_wref.get()));
            cpp_arg->SetText(formatText1);
        }
        fScore::SetElapsedScoreTurns(this, Entry);
        {
            const pas::WideString& formatText1_2 = ([&] {
                pas::WideString localizedText = aConst::LocalizedText(pas::concat_wide({u"Rank.", aConst::CoalitionRankNames[Entry->Rank], u".Name"}));
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormScore.Rank"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Rank>"_w, std::move(localizedText));
            }());
            GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IRank"_wref.get()));
            cpp_arg_2->SetText(formatText1_2);
        }
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(Entry->DominatorKillCount);
            GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IKillDominator"_wref.get()));
            cpp_arg_3->SetText(intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Entry->PirateKillCount);
            GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IKillPirate"_wref.get()));
            cpp_arg_4->SetText(intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(Entry->OtherShipKillCount);
            GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IKillNormal"_wref.get()));
            cpp_arg_5->SetText(intToStr_3);
        }
        {
            const pas::WideString& intToStr_4 = pas::wide_int_to_str(Entry->ArcadeKillCount);
            GI_Label::TLabelGI* cpp_arg_6 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IKillHyper"_wref.get()));
            cpp_arg_6->SetText(intToStr_4);
        }
        {
            const pas::WideString& formatText1_3 = ([&] {
                pas::WideString intToStr_5 = pas::wide_int_to_str(Entry->LiberatedSystemCount);
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormScore.LiberationSystem"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,222,0>"_w, u"<LiberationSystem>"_w, std::move(intToStr_5));
            }());
            GI_Label::TLabelGI* cpp_arg_7 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ILiberationSystem"_wref.get()));
            cpp_arg_7->SetText(formatText1_3);
        }
        {
            const pas::WideString& formatText1_4 = ([&] {
                pas::WideString intToStr_6 = pas::wide_int_to_str(Entry->AwardCount);
                pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormScore.Rewards"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Rewards>"_w, std::move(intToStr_6));
            }());
            GI_Label::TLabelGI* cpp_arg_8 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IRewards"_wref.get()));
            cpp_arg_8->SetText(formatText1_4);
        }
        LetterQuests = 0;
        ShipKillQuests = 0;
        PlanetQuests = 0;
        SystemDefenseQuests = 0;
        ShipDefenseQuests = 0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Entry->QuestResults.length() - 1); cpp_range_2.next(I); ) {
            if (Entry->QuestResults[I].Successful) {
                if (Entry->QuestResults[I].QuestType == aGalaxyStruct::qtSendLetter) {
                    ++LetterQuests;
                } else if (Entry->QuestResults[I].QuestType == aGalaxyStruct::qtKillShip) {
                    ++ShipKillQuests;
                } else if (Entry->QuestResults[I].QuestType == aGalaxyStruct::qtPlanetQuest) {
                    ++PlanetQuests;
                } else if (Entry->QuestResults[I].QuestType == aGalaxyStruct::qtDefendSystem) {
                    ++SystemDefenseQuests;
                } else if (Entry->QuestResults[I].QuestType == aGalaxyStruct::qtDefendShip) {
                    ++ShipDefenseQuests;
                }
            }
        }
        {
            const pas::WideString& formatText1_5 = ([&] {
                pas::WideString intToStr_7 = pas::wide_int_to_str(LetterQuests + ShipKillQuests + PlanetQuests + SystemDefenseQuests + ShipDefenseQuests);
                pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormScore.Quests"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_5), u"<color=255,240,100>"_w, u"<Quests>"_w, std::move(intToStr_7));
            }());
            GI_Label::TLabelGI* cpp_arg_9 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IQuests"_wref.get()));
            cpp_arg_9->SetText(formatText1_5);
        }
        if (GR_Main::GiResourceVariant() == 2) {
            Separator = u"+"_w;
        } else {
            Separator = u":"_w;
        }
        {
            GI_MessageLoop::TObjectGI* IQuests = GetByName(u"IQuests"_wref.get());
            X = IQuests->LocalPosition.X + IQuests->ClientSize.X + 5;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 10); cpp_range_3.next(I); ) {
                Control = FindControlByPath(static_cast<pas::WideString>(pas::concat_ansi({"QI", SysUtils::IntToStr(I)})));
                if (Control == nullptr) {
                    Control = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, GetByName(u"PanelWin"_wref.get()));
                    Control->SetName(static_cast<pas::WideString>(pas::concat_ansi({"QI", SysUtils::IntToStr(I)})));
                }
                Control->SetPosition(ClassesImports::Point(X, IQuests->LocalPosition.Y + 2));
                Control->SetSize(ClassesImports::Point(1, IQuests->ClientSize.Y - 2));
                {
                    GI_Label::TLabelGI* cpp_with_19 = pas::checked_cast<GI_Label::TLabelGI*>(Control);
                    if (GR_Main::GiResourceVariant() == 2) {
                        cpp_with_19->SetFontName(GlobalsV::SmallFontName);
                    } else {
                        cpp_with_19->SetFontName(GlobalsV::MiniFontName);
                    }
                    cpp_with_19->SetTextAlignX(GI_Main::taxAuto);
                    cpp_with_19->SetTextAlignY(GI_Main::tayCenterEx);
                    cpp_with_19->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(199, 135, 0));
                    if (I == 0) {
                        cpp_with_19->SetText(u"("_wref.get());
                    } else if (I == 1) {
                        cpp_with_19->SetText(pas::wide_int_to_str(static_cast<std::int32_t>(LetterQuests)));
                    } else if (I == 2) {
                        cpp_with_19->SetText(Separator);
                    } else if (I == 3) {
                        cpp_with_19->SetText(pas::wide_int_to_str(static_cast<std::int32_t>(ShipKillQuests)));
                    } else if (I == 4) {
                        cpp_with_19->SetText(Separator);
                    } else if (I == 5) {
                        cpp_with_19->SetText(pas::wide_int_to_str(static_cast<std::int32_t>(PlanetQuests)));
                    } else if (I == 6) {
                        cpp_with_19->SetText(Separator);
                    } else if (I == 7) {
                        cpp_with_19->SetText(pas::wide_int_to_str(static_cast<std::int32_t>(SystemDefenseQuests)));
                    } else if (I == 8) {
                        cpp_with_19->SetText(Separator);
                    } else if (I == 9) {
                        cpp_with_19->SetText(pas::wide_int_to_str(static_cast<std::int32_t>(ShipDefenseQuests)));
                    } else if (I == 10) {
                        cpp_with_19->SetText(u")"_wref.get());
                    }
                    if (pas::in_set<1, 1, 3, 3, 5, 5, 7, 7, 9, 9>(I)) {
                        cpp_with_19->HelpText = ([&] {
                            pas::WideString text = cpp_with_19->GetText();
                            pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"FormScore.Quests", SysUtils::IntToStr((I - 1) / 2 + 1)})));
                            return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey), u"<color=255,240,100>"_w, u"<N>"_w, std::move(text));
                        }());
                        cpp_with_19->MouseEnterCallback = pas::bind_method<&TfScore::QuestHelpMouseEnter>(this);
                        cpp_with_19->MouseLeaveCallback = pas::bind_method<&TfScore::QuestHelpMouseLeave>(this);
                    }
                    cpp_with_19->SetTextAlignX(GI_Main::taxCenter);
                    if (GR_Main::GiResourceVariant() == 1) {
                        cpp_with_19->SetSize(ClassesImports::Point(cpp_with_19->ClientSize.X - 3, cpp_with_19->ClientSize.Y));
                    } else {
                        cpp_with_19->SetSize(ClassesImports::Point(cpp_with_19->ClientSize.X - 4, cpp_with_19->ClientSize.Y));
                    }
                    X += cpp_with_19->ClientSize.X;
                }
            }
        }
        {
            const pas::WideString& formatText1_6 = ([&] {
                pas::WideString intToStr_8 = pas::wide_int_to_str(Entry->PlanetBattles);
                pas::WideString localizedColorText_6 = aConst::LocalizedColorText(u"FormScore.PlanetBattles"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_6), u"<color=255,240,100>"_w, u"<PlanetBattles>"_w, std::move(intToStr_8));
            }());
            GI_Label::TLabelGI* cpp_arg_10 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPlanetBattles"_wref.get()));
            cpp_arg_10->SetText(formatText1_6);
        }
        {
            const pas::WideString& formatText1_7 = ([&] {
                pas::WideString intToStr_9 = pas::wide_int_to_str(Entry->TotalExperience);
                pas::WideString localizedColorText_7 = aConst::LocalizedColorText(u"FormScore.Exp"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_7), u"<color=255,222,0>"_w, u"<Exp>"_w, std::move(intToStr_9));
            }());
            GI_Label::TLabelGI* cpp_arg_11 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IExp"_wref.get()));
            cpp_arg_11->SetText(formatText1_7);
        }
        ResolvedColor = u"<color=255,100,50>"_w;
        UnresolvedColor = u"<color=30,252,30>"_w;
        switch (Entry->BlazerEndingState) {
            case 0: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.BlazerLeave"_wref.get()), UnresolvedColor);
                break;
            }
            case 1: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.BlazerDead"_wref.get()), ResolvedColor);
                break;
            }
            case 2: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.BlazerSuicide"_wref.get()), ResolvedColor);
                break;
            }
            default: {
                if (Entry->PirateEndingState == 5) {
                    Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.BlazerChangeSideAlt"_wref.get()), ResolvedColor);
                } else {
                    Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.BlazerChangeSide"_wref.get()), ResolvedColor);
                }
                break;
            }
        }
        {
            const pas::WideString& formatText1_8 = ([&] {
                pas::WideString lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"ShipType.Dominator.Blazer.0"_wref.get());
                pas::WideString text_2 = Text;
                return aMyFunction::FormatText1(std::move(text_2), pas::WideString(), u"<Blazer>"_w, std::move(lookupLocalizedTextByKey_2));
            }());
            GI_Label::TLabelGI* cpp_arg_12 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IBlazer"_wref.get()));
            cpp_arg_12->SetText(formatText1_8);
        }
        switch (Entry->KellerEndingState) {
            case 0: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.KellerLeave"_wref.get()), UnresolvedColor);
                break;
            }
            case 1: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.KellerDead"_wref.get()), ResolvedColor);
                break;
            }
            case 2: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.KellerFly"_wref.get()), ResolvedColor);
                break;
            }
            default: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.KellerNewResearch"_wref.get()), ResolvedColor);
                break;
            }
        }
        {
            const pas::WideString& formatText1_9 = ([&] {
                pas::WideString lookupLocalizedTextByKey_3 = GR_Main::LookupLocalizedTextByKey(u"ShipType.Dominator.Keller.0"_wref.get());
                pas::WideString text_3 = Text;
                return aMyFunction::FormatText1(std::move(text_3), pas::WideString(), u"<Keller>"_w, std::move(lookupLocalizedTextByKey_3));
            }());
            GI_Label::TLabelGI* cpp_arg_13 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IKeller"_wref.get()));
            cpp_arg_13->SetText(formatText1_9);
        }
        switch (Entry->TerronEndingState) {
            case 0: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.TerronLeave"_wref.get()), UnresolvedColor);
                break;
            }
            case 1: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.TerronDead"_wref.get()), ResolvedColor);
                break;
            }
            case 2: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.TerronStar"_wref.get()), ResolvedColor);
                break;
            }
            default: {
                Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.TerronBattle"_wref.get()), ResolvedColor);
                break;
            }
        }
        {
            const pas::WideString& formatText1_10 = ([&] {
                pas::WideString lookupLocalizedTextByKey_4 = GR_Main::LookupLocalizedTextByKey(u"ShipType.Dominator.Terron.0"_wref.get());
                pas::WideString text_4 = Text;
                return aMyFunction::FormatText1(std::move(text_4), pas::WideString(), u"<Terron>"_w, std::move(lookupLocalizedTextByKey_4));
            }());
            GI_Label::TLabelGI* cpp_arg_14 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ITerron"_wref.get()));
            cpp_arg_14->SetText(formatText1_10);
        }
        if (Entry->PirateEndingState > 0) {
            Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"FormScore.PirateWin", SysUtils::IntToStr(Entry->PirateEndingState)}))), ResolvedColor);
        } else {
            Text = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormScore.PirateWin0"_wref.get()), UnresolvedColor);
        }
        {
            GI_Label::TLabelGI* IPirate = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPirate"_wref.get()));
            IPirate->SetActive(true);
            IPirate->SetText(Text);
        }
        if (Entry->VictoryAchieved) {
            const pas::WideString& formatText1_11 = ([&] {
                pas::WideString intToStr_10 = pas::wide_int_to_str(Entry->TotalScore);
                pas::WideString localizedColorText_8 = aConst::LocalizedColorText(u"FormScore.TotalWin"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_8), u"<color=255,222,0>"_w, u"<Total>"_w, std::move(intToStr_10));
            }());
            GI_Label::TLabelGI* cpp_arg_15 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ITotal"_wref.get()));
            cpp_arg_15->SetText(formatText1_11);
        } else {
            const pas::WideString& localizedColorText_9 = aConst::LocalizedColorText(u"FormScore.TotalLoss"_wref.get());
            GI_Label::TLabelGI* cpp_arg_16 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ITotal"_wref.get()));
            cpp_arg_16->SetText(localizedColorText_9);
        }
        {
            const pas::WideString& localizedColorText_10 = aConst::LocalizedColorText(u"FormScore.Note"_wref.get());
            GI_Label::TLabelGI* cpp_arg_17 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"INote"_wref.get()));
            cpp_arg_17->SetText(localizedColorText_10);
        }
    }

    void TfScore::EntryMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (!Sender->IsOccludedAtPoint(Point)) {
            GR_Main::SoundManager->PlaySound(u"Sound.ButtonClick"_wref.get());
            SelectedIndex = EC_Str::ExtractDigitsToIntW(Sender->ControlName);
            RefreshDetails();
        }
    }

    void TfScore::ExportEntryClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString FileName{};
        pas::WideString Text{};
        std::int32_t Index = Sender->UserValue;
        TfScoreUnit* Entry = pas::list_at<TfScoreUnit>(Entries, Index);
        if (SimpleSteamApi::SteamInitialized && SimpleSteamApi::SteamLeaderboardFound() && static_cast<std::uint8_t>(Entry->Disqualified ^ 1)) {
            SimpleSteamApi::SteamUploadScore(Entry->TotalScore);
        }
        if (Index + 1 < 10) {
            FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"ToServer0", pas::wide_int_to_str(Index + 1), u".txt"});
        } else {
            FileName = pas::concat_wide({GR_Main::GetGameUserDirectory(), u"ToServer", pas::wide_int_to_str(Index + 1), u".txt"});
        }
        Entry->ExportToFile(FileName);
        Text = aConst::LocalizedColorText(u"FormScore.ToServer"_wref.get());
        Text = aMyFunction::ReplaceColoredToken(Text, u"<Player>"_w, Entry->PlayerName, u"<color=255,240,100>"_w);
        Text = aMyFunction::ReplaceColoredToken(Text, u"<File>"_w, EC_Str::ReplaceAllWideString(FileName, u"\\"_wref.get(), u" \\ "_wref.get()), u"<color=255,240,100>"_w);
        Text = ([&] {
            pas::WideString formatGameTurnDate = aGalaxy::FormatGameTurnDate(Entry->FinishedTurn);
            pas::WideString text = Text;
            return aMyFunction::ReplaceColoredToken(std::move(text), u"<WinGameDate>"_w, std::move(formatGameTurnDate), u"<color=255,240,100>"_w);
        }());
        Entry->Exported = true;
        GI_MessageBox::ShowMessageBoxGI(this, Text, GI_MessageBox::mbgOK | GI_MessageBox::mbgUnused04 | GI_MessageBox::mbgLeftAlign, 0, 0, 0);
        RefreshDetails();
    }

    void TfScore::QuestHelpMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        ShowControlHelp(Sender, true);
    }

    void TfScore::QuestHelpMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        ShowControlHelp(Sender, false);
    }

    void TfScore::ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible) {
        GI_Label::TLabelGI* LabelHelp = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LabelHelp"_wref.get()));
        if (Sender->HelpText == u"") {
            Visible = false;
        }
        LabelHelp->SetActive(Visible);
        LabelHelp->SetText(Sender->HelpText);
    }

    void TfScore::SelectMusic() {
        GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
    }

    void TfScoreUnit::p_destroy() {
        fScore::TfScoreUnit_Destroy(this);
    }

    void TfScore::p_destroy() {
        fScore::TfScore_Destroy(this);
    }

} // namespace fScore
