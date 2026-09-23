#include "layout/aNormalShip.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aKling.hpp"
#include "types/aPirate.hpp"
#include "types/aTranclucator.hpp"
#include "types/aTransport.hpp"
#include "types/aWarrior.hpp"
#include "units/Achievements.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aShip.hpp"

namespace aNormalShip {
    // Processes qualifying ships after a control change; SourceShip supplies the news-variant seed.
    void ProcessSystemLiberationRewards(TNormalShip* SourceShip, aGalaxy::TStar* Star) {
        std::int32_t I{};
        std::int32_t J{};
        aShip::TShip* Ship{};
        TNormalShip* Normal{};
        aPlanet::TPlanet* Planet{};
        aPlanet::TPlanet* CeremonyPlanet{};
        pas::WideString Text{};
        // Nested helper; caller-popped link, Star at ParentFrame-4.
        auto LogPlayerEvent = [&](aShip::TShip* Ship) -> void {
            aGalaxyEvent::TGalaxyEvent* Event{};
            if (aPlayer::GetPlayer() == Ship) {
                Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerLiberatesSystem"_w, nullptr);
                Event->AddData(Star->Id);
                Event->AddData(static_cast<std::uint8_t>(Star->Status.ControlFaction));
                Event->AddData(static_cast<std::uint8_t>(Star->Status.PreviousControlFaction));
                Event->AddData(aPlayer::GetPlayer()->PendingLiberationContribution);
                Event->AddData(aPlayer::GetPlayer()->PendingLiberationCeremonyPlanet->Id);
            }
        };
        if (Star->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
            CeremonyPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Star->FindFirstInhabitedPlanet()));
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
                if (TNormalShip* normalShip = pas::class_cast_if<TNormalShip*>(Ship)) {
                    Normal = normalShip;
                    Normal->PendingLiberationCeremonyPlanet = nullptr;
                    if (Normal->CurrentSystemKills.Dominator > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfDominators || Normal->CurrentSystemKills.Pirate > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfPirates && Normal->OwnerId != aGalaxyStruct::oiPirate || aPlayer::GetPlayer() != Ship && Ship->DaysSincePlayerSeen > 1 && Normal->DominatorKillCount + Normal->PirateKillCount > Normal->LiberatedSystemCount) {
                        if (Star->Status.PreviousControlFaction == aGalaxyStruct::sfDominators) {
                            Normal->PendingLiberationContribution = Normal->CurrentSystemKills.Dominator;
                        }
                        if (Star->Status.PreviousControlFaction == aGalaxyStruct::sfPirates) {
                            Normal->PendingLiberationContribution = Normal->CurrentSystemKills.Pirate;
                        }
                        Normal->CurrentSystemKills.Dominator = 0;
                        Normal->CurrentSystemKills.Pirate = 0;
                        Normal->CurrentSystemKills.Normal = 0;
                        Normal->CurrentSystemKills.Custom = 0;
                        ++Normal->LiberatedSystemCount;
                        Normal->PendingLiberationCeremonyPlanet = CeremonyPlanet;
                        if (Normal->OwnerId != aGalaxyStruct::oiPirate) {
                            Normal->AddRankPoints(30);
                        } else {
                            Normal->AddPirateRankPoints(16u);
                        }
                        Normal->GainExperience(aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState), aGalaxyStruct::esUnscaled);
                        if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
                            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_2.next(J); ) {
                                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                                if (Planet->IsCoalitionOwned) {
                                    Planet->ChangeRelationToRanger(Ship, 100);
                                }
                            }
                        }
                        if (Ship->InNormalSpace() && (aPlayer::GetPlayer() != Ship || aRanger::PlayerAutomaticControl)) {
                            Ship->OrderLanding(CeremonyPlanet, true);
                        }
                        LogPlayerEvent(Ship);
                    }
                }
            }
            if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                if (Star->Status.PreviousControlFaction == aGalaxyStruct::sfDominators) {
                    Text = ([&] {
                        auto name = pas::borrow(Star->Name);
                        pas::WideString name_2 = Star->Constellation->GetName();
                        auto name_3 = pas::borrow(CeremonyPlanet->Name);
                        pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.NormalsTakeSystemFromKling"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                        pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag), u"<Star>"_w, name.get(), u"<Sector>"_w, std::move(name_2), u"<Planet>"_w, name_3.get());
                    }());
                    aGalaxy::Galaxy->AddPlanetNews(aGalaxyStruct::gnCoalitionTakesDominatorSystem, Text);
                } else {
                    Text = ([&] {
                        auto name_4 = pas::borrow(Star->Name);
                        pas::WideString name_5 = Star->Constellation->GetName();
                        auto name_6 = pas::borrow(CeremonyPlanet->Name);
                        pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.NormalsTakeSystemFromPirateClan"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                        pas::WideString textHighlightColorTag_2 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_2), std::move(textHighlightColorTag_2), u"<Star>"_w, name_4.get(), u"<Sector>"_w, std::move(name_5), u"<Planet>"_w, name_6.get());
                    }());
                    aGalaxy::Galaxy->AddPlanetNews(aGalaxyStruct::gnCoalitionTakesPirateSystem, Text);
                }
                {
                    Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                    if (aPlayer::GetPlayer()->CurrentStar == Star && aPlayer::GetPlayer()->InNormalSpace()) {
                        cpp_with->NotificationSoundKind = 1;
                    } else {
                        cpp_with->NotificationSoundKind = 0;
                    }
                    cpp_with->Targets[0].PlanetId = CeremonyPlanet->Id;
                }
            }
        } else if (Star->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            CeremonyPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Star->FindFirstInhabitedPlanet()));
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(I); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
                if (TNormalShip* normalShip_2 = pas::class_cast_if<TNormalShip*>(Ship)) {
                    Normal = normalShip_2;
                    Normal->PendingLiberationCeremonyPlanet = nullptr;
                    if (Normal->CurrentSystemKills.Dominator > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfDominators || Normal->CurrentSystemKills.Normal > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfCoalition && Normal->OwnerId == aGalaxyStruct::oiPirate || aPlayer::GetPlayer() != Ship && Ship->DaysSincePlayerSeen > 1 && Normal->MilitaryKillCount + Normal->DominatorKillCount > Normal->LiberatedSystemCount) {
                        if (Star->Status.PreviousControlFaction == aGalaxyStruct::sfDominators) {
                            Normal->PendingLiberationContribution = Normal->CurrentSystemKills.Dominator;
                        }
                        if (Star->Status.PreviousControlFaction == aGalaxyStruct::sfCoalition) {
                            Normal->PendingLiberationContribution = Normal->CurrentSystemKills.Normal;
                        }
                        Normal->CurrentSystemKills.Dominator = 0;
                        Normal->CurrentSystemKills.Pirate = 0;
                        Normal->CurrentSystemKills.Normal = 0;
                        Normal->CurrentSystemKills.Custom = 0;
                        ++Normal->LiberatedSystemCount;
                        Normal->PendingLiberationCeremonyPlanet = CeremonyPlanet;
                        Normal->AddPirateRankPoints(16u);
                        Normal->GainExperience(aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState), aGalaxyStruct::esUnscaled);
                        if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr) {
                            if (aPlanet::MainPiratePlanet != nullptr) {
                                aPlanet::MainPiratePlanet->ChangeRelationToRanger(Ship, 10);
                                if (aPlayer::GetPlayer() == Ship && aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
                                    aPlayer::GetPlayer()->PirateLicenseCash += System::Round(pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 1.0E+3L));
                                }
                            }
                        }
                        if (Ship->InNormalSpace() && (aPlayer::GetPlayer() != Ship || aRanger::PlayerAutomaticControl)) {
                            Ship->OrderLanding(CeremonyPlanet, true);
                        }
                        LogPlayerEvent(Ship);
                        if (aPlayer::GetPlayer() == Ship) {
                            ++aPlayer::GetPlayer()->AchievementStats->SystemsCapturedForPirates;
                            Achievements::TrySetAchievementProgress(u"PIRATE"_w, aPlayer::GetPlayer()->AchievementStats->SystemsCapturedForPirates);
                        }
                    }
                }
            }
            if (Star->Status.PreviousControlFaction == aGalaxyStruct::sfCoalition) {
                Text = ([&] {
                    auto name_7 = pas::borrow(Star->Name);
                    pas::WideString name_8 = Star->Constellation->GetName();
                    auto name_9 = pas::borrow(CeremonyPlanet->Name);
                    pas::WideString pickLocalizedTextVariant_3 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.PirateClanTakeSystemFromNormals"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                    pas::WideString textHighlightColorTag_3 = aMyFunction::TextHighlightColorTag;
                    return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_3), std::move(textHighlightColorTag_3), u"<Star>"_w, name_7.get(), u"<Sector>"_w, std::move(name_8), u"<Planet>"_w, name_9.get());
                }());
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    aGalaxy::Galaxy->AddPlanetNews(aGalaxyStruct::gnPiratesTakeCoalitionSystem, Text);
                }
            } else {
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    Text = ([&] {
                        auto name_10 = pas::borrow(Star->Name);
                        pas::WideString name_11 = Star->Constellation->GetName();
                        auto name_12 = pas::borrow(CeremonyPlanet->Name);
                        pas::WideString pickLocalizedTextVariant_4 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.PirateClanTakeSystemFromKling"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                        pas::WideString textHighlightColorTag_4 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_4), std::move(textHighlightColorTag_4), u"<Star>"_w, name_10.get(), u"<Sector>"_w, std::move(name_11), u"<Planet>"_w, name_12.get());
                    }());
                } else {
                    Text = ([&] {
                        auto name_13 = pas::borrow(Star->Name);
                        pas::WideString name_14 = Star->Constellation->GetName();
                        auto name_15 = pas::borrow(CeremonyPlanet->Name);
                        pas::WideString pickLocalizedTextVariant_5 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.PirateClanTakeSystemFromKlingAlt"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                        pas::WideString textHighlightColorTag_5 = aMyFunction::TextHighlightColorTag;
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_5), std::move(textHighlightColorTag_5), u"<Star>"_w, name_13.get(), u"<Sector>"_w, std::move(name_14), u"<Planet>"_w, name_15.get());
                    }());
                }
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    aGalaxy::Galaxy->AddPlanetNews(aGalaxyStruct::gnPiratesTakeDominatorSystem, Text);
                }
            }
            {
                Globals::TMessagePlayer* cpp_with_2 = Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                if (aPlayer::GetPlayer()->CurrentStar == Star && aPlayer::GetPlayer()->InNormalSpace()) {
                    cpp_with_2->NotificationSoundKind = 1;
                } else {
                    cpp_with_2->NotificationSoundKind = 0;
                }
                cpp_with_2->Targets[0].PlanetId = CeremonyPlanet->Id;
            }
        }
    }

    void TNormalShip_Create(TNormalShip* Self) {
        aShip::TShip_Create(Self);
        Self->TotalShipKillCount = 0;
        Self->PirateKillCount = 0;
        Self->DominatorKillCount = 0;
        Self->LiberatedSystemCount = 0;
        Self->CurrentSystemKills.Dominator = 0;
        Self->CurrentSystemKills.Pirate = 0;
        Self->CurrentSystemKills.Normal = 0;
        Self->CurrentSystemKills.Custom = 0;
        Self->PendingLiberationCeremonyPlanet = nullptr;
        Self->PendingLiberationContribution = 0;
        Self->Rank = 0;
        Self->RankPoints = 0;
        Self->LastDockedPlanet = nullptr;
        Self->LastPlayerExtortionTurn = 0;
        Self->PirateRank = 0;
        Self->PirateRankPoints = 0u;
    }

    void TNormalShip_Destroy(TNormalShip* Self) {
        aGalaxyStruct::TRangerCareer Career{};
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TRangerCareer>(aGalaxyStruct::rcTrader, aGalaxyStruct::rcWarrior); cpp_range.next(Career); ) {
            if (aGalaxy::Galaxy->EminentCareerShips[Career] == Self) {
                aGalaxy::Galaxy->EminentCareerShips[Career] = nullptr;
            }
        }
        aShip::TShip_Destroy(Self);
    }

    void TNormalShip::SaveToBuffer(EC_Buf::TBufEC* Buffer) {
        aShip::TShip::SaveToBuffer(Buffer);
        Buffer->AddIntegerValue(TotalShipKillCount);
        Buffer->AddIntegerValue(PirateKillCount);
        Buffer->AddIntegerValue(DominatorKillCount);
        Buffer->AddIntegerValue(LiberatedSystemCount);
        Buffer->AddIntegerValue(CivilianKillCount);
        Buffer->AddIntegerValue(MilitaryKillCount);
        Buffer->AddIntegerValue(RangerKillCount);
        Buffer->AddWideChar(CurrentSystemKills.Dominator);
        Buffer->AddWideChar(CurrentSystemKills.Pirate);
        Buffer->AddWideChar(CurrentSystemKills.Normal);
        Buffer->AddWideChar(CurrentSystemKills.Custom);
        if (PendingLiberationCeremonyPlanet == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(PendingLiberationCeremonyPlanet->Id);
        }
        Buffer->AddIntegerValue(PendingLiberationContribution);
        Buffer->AddAnsiChar(Rank);
        Buffer->AddWideChar(RankPoints);
        Buffer->AddAnsiChar(PirateRank);
        Buffer->AddDWord(PirateRankPoints);
        if (LastDockedPlanet == nullptr) {
            Buffer->AddDWord(0u);
        } else {
            Buffer->AddDWord(LastDockedPlanet->Id);
        }
        Buffer->AddIntegerValue(LastPlayerExtortionTurn);
    }

    void TNormalShip::LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* Galaxy) {
        aShip::TShip::LoadFromBuffer(Buffer, Galaxy);
        if (GlobalsV::LoadedSaveVersion >= 57) {
            TotalShipKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            PirateKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            DominatorKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            LiberatedSystemCount = EC_Buf::TBufEC_GetInt32(Buffer);
            CivilianKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            MilitaryKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
            RangerKillCount = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            TotalShipKillCount = EC_Buf::TBufEC_GetWord(Buffer);
            PirateKillCount = EC_Buf::TBufEC_GetWord(Buffer);
            DominatorKillCount = EC_Buf::TBufEC_GetWord(Buffer);
            LiberatedSystemCount = EC_Buf::TBufEC_GetWord(Buffer);
            CivilianKillCount = EC_Buf::TBufEC_GetWord(Buffer);
            MilitaryKillCount = EC_Buf::TBufEC_GetWord(Buffer);
            RangerKillCount = EC_Buf::TBufEC_GetWord(Buffer);
        }
        CurrentSystemKills.Dominator = EC_Buf::TBufEC_GetWord(Buffer);
        CurrentSystemKills.Pirate = EC_Buf::TBufEC_GetWord(Buffer);
        CurrentSystemKills.Normal = EC_Buf::TBufEC_GetWord(Buffer);
        if (GlobalsV::LoadedSaveVersion >= 153) {
            CurrentSystemKills.Custom = EC_Buf::TBufEC_GetWord(Buffer);
        }
        PendingLiberationCeremonyPlanet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        if (GlobalsV::LoadedSaveVersion >= 80) {
            PendingLiberationContribution = EC_Buf::TBufEC_GetInt32(Buffer);
        } else {
            PendingLiberationContribution = 0;
        }
        Rank = EC_Buf::TBufEC_GetByte(Buffer);
        RankPoints = EC_Buf::TBufEC_GetWord(Buffer);
        PirateRank = EC_Buf::TBufEC_GetByte(Buffer);
        PirateRankPoints = EC_Buf::TBufEC_GetUInt32(Buffer);
        if (GlobalsV::LoadedSaveVersion < 126 && EC_Buf::TBufEC_GetByte(Buffer) != 0) {
            OwnerId = aGalaxyStruct::oiPirate;
        }
        LastDockedPlanet = reinterpret_cast<aPlanet::TPlanet*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(EC_Buf::TBufEC_GetUInt32(Buffer))));
        LastPlayerExtortionTurn = EC_Buf::TBufEC_GetInt32(Buffer);
    }

    void TNormalShip_ResolveLoadedReferences(TNormalShip* Self, aGalaxy::TGalaxy* Galaxy) {
        aShip::TShip_ResolveLoadedReferences(Self, Galaxy);
        Self->PendingLiberationCeremonyPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->PendingLiberationCeremonyPlanet)), true)));
        Self->LastDockedPlanet = pas::checked_cast<aPlanet::TPlanet*>(static_cast<pas::Object*>(Galaxy->IdToPlanet(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Self->LastDockedPlanet)), true)));
    }

    void TNormalShip::SaveToBlock(EC_BlockPar::TBlockParEC* Block) {
        aShip::TShip::SaveToBlock(Block);
        {
            const pas::WideString& intToStr = pas::wide_int_to_str(static_cast<std::int32_t>(Rank));
            const pas::WideString& decodeTextW = EC_Str::DecodeTextW(u"Roarnuke"_w);
            Block->AddParam(decodeTextW, intToStr);
        }
        {
            const pas::WideString& intToStr_2 = pas::wide_int_to_str(static_cast<std::int32_t>(RankPoints));
            const pas::WideString& decodeTextW_2 = EC_Str::DecodeTextW(u"RearnaksProcitnotas"_w);
            Block->AddParam(decodeTextW_2, intToStr_2);
        }
        {
            const pas::WideString& intToStr_3 = pas::wide_int_to_str(static_cast<std::int32_t>(PirateRank));
            const pas::WideString& decodeTextW_3 = EC_Str::DecodeTextW(u"PlivroaktrenRiasnuk"_w);
            Block->AddParam(decodeTextW_3, intToStr_3);
        }
        {
            const pas::WideString& int64ToStr = pas::wide_int64_to_str(static_cast<std::int64_t>(PirateRankPoints));
            const pas::WideString& decodeTextW_4 = EC_Str::DecodeTextW(u"PhilroaAtrelRoasnAkoPiopionatos"_w);
            Block->AddParam(decodeTextW_4, int64ToStr);
        }
    }

    // Native editable import truncates PirateRankPoints to Word.
    void TNormalShip::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        aShip::TShip::LoadFromBlock(Block);
        Rank = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"Roarnuke"_w)))));
        RankPoints = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"RearnaksProcitnotas"_w)))));
        PirateRank = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"PlivroaktrenRiasnuk"_w)))));
        PirateRankPoints = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(pas::view(EC_Str::DecodeTextW(u"PhilroaAtrelRoasnAkoPiopionatos"_w))))));
    }

    void TNormalShip_NextDay(TNormalShip* Self) {
        pas::WideString MessageText{};
        aShip::TShip_NextDay(Self);
        std::int32_t Stage = 0;
        try {
            if (Self->InHyperspace) {
                Self->CurrentSystemKills.Dominator = 0;
                Self->CurrentSystemKills.Pirate = 0;
                Self->CurrentSystemKills.Normal = 0;
                Self->CurrentSystemKills.Custom = 0;
            }
            if (aPlayer::GetPlayer() == Self && static_cast<std::uint8_t>(aRanger::TRanger_ProcessPendingPlayerFollowTargeting(aPlayer::GetPlayer()) ^ 1)) {
                return;
            }
            if (Self->CurrentPlanet != nullptr && Self->PendingLiberationCeremonyPlanet == Self->CurrentPlanet) {
                Self->CollectLiberationRewards();
            }
            Stage = 1;
            Self->virtual_TShip_RecomputeFearState();
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == Self->CurrentStar && Self->Order != aShip::soNone && Globals::PlayerStarDayPrepared && aGalaxy::TurnsSinceLastShipMessage > 5 && static_cast<std::int32_t>(Self->Seed) * aGalaxy::Galaxy->CurrentTurn % 7 == 0 && Self->InNormalSpace() && aPlayer::GetPlayer()->InNormalSpace() && ([&] {
                pas::Extended cpp_left = aMyFunction::PointDistance(Self->Position, aPlayer::GetPlayer()->Position);
                return cpp_left < Self->GetRadarRange();
            }()) && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(aRanger::TRanger_ProcessPendingPlayerFollowTargeting(aPlayer::GetPlayer()) ^ 1) && Self->ScriptShip == nullptr && Self->LiberationGroup == nullptr) {
                MessageText = aNormalShip::TNormalShip_SelectSituationalMessage(Self, true);
                if (MessageText != u"") {
                    Self->ShowMessageToPlayer(MessageText);
                }
            }
            Stage = 2;
            aNormalShip::TNormalShip_UpdateRelationsForNearbyCombat(Self);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"Error in procedure TNormalShip.NextDay ", Self->GetFullName(u" "_wref.get()), u" label = ", pas::wide_int_to_str(Stage)}))));
            } else {
                throw;
            }
        }
    }

    // Consumes the pending ceremony, grants rewards and returns the player's presentation text.
    pas::WideString TNormalShip::CollectLiberationRewards() {
        aConst::TRewardInfo cpp_result{};
        pas::WideString Result{};
        static const pas::Set<0, 255> RewardPrograms = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::prgShipwreck, aGalaxyStruct::prgDisconnection}});
        static const pas::Set<0, 255> RewardKinds = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::atLiberation}, {aGalaxyStruct::atAccomplishment}});
        static const pas::Set<0, 255> RewardVictims = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}});
        std::int32_t I{};
        std::int32_t MinimumPriority{};
        std::int32_t RewardKind{};
        std::int32_t Quantity{};
        std::int32_t ModuleIndex{};
        std::int32_t TotalPriority{};
        std::int32_t Priority{};
        std::int32_t Roll{};
        EC_BlockPar::TBlockParEC* TextBlock{};
        pas::WideString Prefix{};
        std::uint8_t Award{};
        float AwardWeight{};
        float ProgramWeight{};
        float ArtefactWeight{};
        float ModuleWeight{};
        aItem::TItem* RewardItem{};
        aGalaxyStruct::TProgramIndex ProgramIndex{};
        aItem::TMicroModule* ModuleItem{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (AwardIds == nullptr) {
            AwardWeight = 8.0E+1f;
        } else {
            AwardWeight = aMyFunction::RemapClamped(pas::list_count(AwardIds), 0.0, 15.0, 8.0E+1, 1.0E+1);
        }
        if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(this); ranger != nullptr && ranger->HasProgram(aGalaxyStruct::prgIntercom)) {
            ProgramWeight = aMyFunction::RemapClamped(ranger->CountProgramsInFilter(static_cast<aRanger::TRangerProgramMask>(RewardPrograms)), 0.0, 1.0E+1, 8.0E+1, 1.0E+1);
        } else {
            ProgramWeight = 0.0f;
        }
        if (pas::class_cast_if<aPlayer::TPlayer*>(this) != nullptr) {
            ArtefactWeight = aMyFunction::RemapClamped(pas::list_count(Artefacts), 0.0, 6.0, 8.0E+1, 1.0E+1);
        } else {
            ArtefactWeight = 0.0f;
        }
        if (pas::class_cast_if<aPlayer::TPlayer*>(this) != nullptr) {
            ModuleWeight = aMyFunction::RandomIntRange(10, 90);
        } else {
            ModuleWeight = 0.0f;
        }
        if (AwardWeight == 0.0L && ProgramWeight == 0.0L && ArtefactWeight == 0.0L && ModuleWeight == 0.0L) {
            ModuleWeight = 1.0f;
        }
        if (AwardWeight > 0.0L) {
            AwardWeight = static_cast<long double>(AwardWeight) * aMyFunction::SeededRandomIntRange(5, 25, CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 100 + 1667);
        }
        if (ProgramWeight > 0.0L) {
            ProgramWeight = static_cast<long double>(ProgramWeight) * aMyFunction::SeededRandomIntRange(5, 25, CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 100 + 197673);
        }
        if (ArtefactWeight > 0.0L) {
            ArtefactWeight = static_cast<long double>(ArtefactWeight) * aMyFunction::SeededRandomIntRange(5, 25, CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 100 + 719671);
        }
        if (ModuleWeight > 0.0L) {
            ModuleWeight = static_cast<long double>(ModuleWeight) * aMyFunction::SeededRandomIntRange(5, 25, CurrentPlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 107 + 1967);
        }
        if (CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate && AwardWeight > 0.0L) {
            AwardWeight = 1.0f;
        }
        if (AwardWeight > 0.0L && AwardWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(ProgramWeight, ArtefactWeight))) {
            RewardKind = 1;
        } else if (ProgramWeight > 0.0L && ProgramWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(AwardWeight, ArtefactWeight))) {
            RewardKind = 2;
        } else if (ArtefactWeight > 0.0L && ArtefactWeight >= pas::real_max<float>(ModuleWeight, pas::real_max<float>(AwardWeight, ProgramWeight))) {
            RewardKind = 3;
        } else if (ModuleWeight > 0.0L && ModuleWeight >= pas::real_max<float>(ArtefactWeight, pas::real_max<float>(AwardWeight, ProgramWeight))) {
            RewardKind = 4;
        } else {
            GR_Main::RaiseWideMessage(u"CongratulationsLiberator"_wref.get());
            RewardKind = 0;
        }
        if (aPlayer::GetPlayer() == this) {
            if (CurrentPlanet->OwnerId != aGalaxyStruct::oiPirate) {
                if (CurrentPlanet->CurrentStar->Status.PreviousControlFaction == aGalaxyStruct::sfDominators) {
                    Result = aConst::LocalizedColorText(pas::concat_wide({u"PlanetCongratulations.LiberationStarNormalsFromKling.", aConst::OwnerToSys(CurrentPlanet->OwnerId), u"Text"}));
                } else {
                    Result = aConst::LocalizedColorText(pas::concat_wide({u"PlanetCongratulations.LiberationStarNormalsFromPirateClan.", aConst::OwnerToSys(CurrentPlanet->OwnerId), u"Text"}));
                }
            } else {
                if (CurrentPlanet->CurrentStar->Status.PreviousControlFaction == aGalaxyStruct::sfCoalition) {
                    Prefix = u"PlanetCongratulations.LiberationStarPirateClanFromNormals."_w;
                } else {
                    Prefix = u"PlanetCongratulations.LiberationStarPirateClanFromKling."_w;
                }
                TotalPriority = 0;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(pas::concat_wide({Prefix, u"CongratulationsCount"})))) - 1); cpp_range.next(I); ) {
                    TextBlock = GR_Main::LanguageDataConfig->FindBlockByPath(pas::concat_wide({Prefix, pas::wide_int_to_str(I)}));
                    if (TextBlock != nullptr) {
                        if (TextBlock->CountParams(u"Priority"_wref.get()) <= 0) {
                            Priority = 10;
                        } else {
                            Priority = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(pas::concat_wide({Prefix, pas::wide_int_to_str(I), u".Priority"}))));
                        }
                        TotalPriority += Priority;
                    }
                }
                Roll = aMyFunction::SeededRandomIntRange(1, TotalPriority, static_cast<std::int32_t>(Seed) * ((static_cast<std::int32_t>(Seed) + aGalaxy::Galaxy->CurrentTurn) / 20));
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(pas::concat_wide({Prefix, u"CongratulationsCount"})))) - 1); cpp_range_2.next(I); ) {
                    TextBlock = GR_Main::LanguageDataConfig->FindBlockByPath(pas::concat_wide({Prefix, pas::wide_int_to_str(I)}));
                    if (TextBlock != nullptr) {
                        if (TextBlock->CountParams(u"Priority"_wref.get()) <= 0) {
                            Priority = 10;
                        } else {
                            Priority = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(pas::concat_wide({Prefix, pas::wide_int_to_str(I), u".Priority"}))));
                        }
                        TotalPriority -= Priority;
                        if (Roll > TotalPriority) {
                            break;
                        }
                    }
                }
                Result = aConst::LocalizedColorText(pas::concat_wide({Prefix, pas::wide_int_to_str(I), u".Text"}));
            }
        } else {
            Result = pas::WideString();
        }
        if (CurrentPlanet->OwnerId != aGalaxyStruct::oiPirate) {
            Prefix = u"PlanetCongratulations.LiberationAwardNormals."_w;
        } else {
            Prefix = u"PlanetCongratulations.LiberationAwardPirateClan."_w;
        }
        switch (RewardKind) {
            case 1: {
                Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), static_cast<TAwardTypeMask>(RewardKinds), static_cast<aGalaxyStruct::TShipTypeMask>(RewardVictims));
                AddAward(Award);
                if (aPlayer::GetPlayer() == this) {
                    Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddReward"}))});
                    {
                        auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString name = (TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name;
                        aMyFunction::ReplaceTextToken(Result, u"<Reward>"_w, std::move(name), textHighlightColorTag.get());
                    }
                } else {
                    Result = pas::WideString();
                }
                break;
            }
            case 2: {
                if (aRanger::TRanger* ranger_2 = pas::class_cast_if<aRanger::TRanger*>(this)) {
                    ProgramIndex = ranger_2->SelectRandomProgramIdFromFilter(static_cast<aRanger::TRangerProgramMask>(RewardPrograms));
                    Quantity = aMyFunction::SeededRandomIntRange(1, System::Round(aMyFunction::RemapClamped(pas::checked_cast<aRanger::TRanger*>(this)->CountProgramsInFilter(static_cast<aRanger::TRangerProgramMask>(RewardPrograms)), 2.0, 1.0E+1, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].MaximumQuestProgramRewardCount, 1.0)), static_cast<std::int32_t>(ProgramIndex) + CurrentStar->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 25));
                    pas::checked_cast<aRanger::TRanger*>(this)->ProgramCounts[ProgramIndex] += Quantity;
                    if (aPlayer::GetPlayer() == this) {
                        Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddProgramms"}))});
                        {
                            auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                            pas::WideString programName = (static_cast<void>(pas::checked_cast<aRanger::TRanger*>(this)), aRanger::TRanger::GetProgramName(ProgramIndex));
                            aMyFunction::ReplaceTextToken(Result, u"<Programm>"_w, std::move(programName), textHighlightColorTag_2.get());
                        }
                        aMyFunction::ReplaceTextToken(Result, u"<Count>"_w, pas::wide_int_to_str(Quantity), aMyFunction::TextHighlightColorTag);
                    } else {
                        Result = pas::WideString();
                    }
                }
                break;
            }
            case 3: {
                RewardItem = aItem::CreateRandomLootItem(aItem::ilpReward, CurrentPlanet->OwnerId, (static_cast<std::int32_t>(CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 50 + 123424767);
                if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(RewardItem)) {
                    static_cast<aTranclucator::TTranclucator*>(artefactTranclucator->Ship)->OwnerShip = this;
                }
                if (pas::class_cast_if<aItem::TArtefact*>(RewardItem) != nullptr) {
                    pas::list_add(Artefacts, reinterpret_cast<void*>(RewardItem));
                } else {
                    pas::list_add(Inventory, reinterpret_cast<void*>(RewardItem));
                }
                if (aPlayer::GetPlayer() == this) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnGovItemReward, RewardItem, nullptr, 0);
                    Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddArtefact"})), u"\r\n", RewardItem->GetDescriptionText()});
                    {
                        auto textHighlightColorTag_3 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString displayName = RewardItem->GetDisplayName();
                        aMyFunction::ReplaceTextToken(Result, u"<Artefact>"_w, std::move(displayName), textHighlightColorTag_3.get());
                    }
                } else {
                    Result = pas::WideString();
                }
                break;
            }
            case 4: {
                I = 0;
                do {
                    MinimumPriority = System::Round(aMyFunction::RemapClamped(aGalaxy::Galaxy->TechLevel, 3.0, 8.0, 7.0E+1, 2.0E+1));
                    ModuleIndex = aGalaxy::TGalaxy::SelectMicroModule(MinimumPriority, std::min<std::int32_t>(MinimumPriority + 30, 100), aGalaxy::Galaxy->CurrentTurn / 77 + 17 * I + CurrentPlanet->Id, CurrentPlanet);
                    ++I;
                    if (I > 50) {
                        break;
                    }
                } while (!aPlayer::GetPlayer()->NeedsMicroModule(ModuleIndex + 1));
                ModuleItem = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                ModuleItem->Init(ModuleIndex);
                ModuleItem->OwnerId = CurrentPlanet->OwnerId;
                if (aPlayer::GetPlayer() == this) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnGovItemReward, ModuleItem, nullptr, 0);
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerReceivesMMAsReward"_w, nullptr);
                    Event->AddData(ModuleItem->Id);
                    Event->AddData(ModuleItem->MicroModuleIndex - 1);
                }
                pas::list_add(Inventory, reinterpret_cast<void*>(ModuleItem));
                if (aPlayer::GetPlayer() == this) {
                    Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddNod"})), u"\r\n", ModuleItem->virtual_TItem_GetInfoText(aMyFunction::TextHighlightColorTag, nullptr)});
                    aMyFunction::ReplaceTextToken(Result, u"<Nod>"_w, aConst::MicroModuleTemplates[ModuleIndex].Name, aMyFunction::TextHighlightColorTag);
                } else {
                    Result = pas::WideString();
                }
                break;
            }
        }
        {
            pas::Extended cpp_left = aMyFunction::SeededRandomIntRange(250, aGalaxy::Galaxy->ScaleIntByTechLevel(500, 1000), (static_cast<std::int32_t>(CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 100);
            I = aMyFunction::RoundAndTruncateToTens(cpp_left + System::Ln(PendingLiberationContribution * 0.2L + 1.0L) * 1.0E+3L);
        }
        GainExperience(I, aGalaxyStruct::esUnscaled);
        if (aPlayer::GetPlayer() == this) {
            Result = pas::concat_wide({Result, u"\r\n", u" ", u"\r\n", ([&] {
                pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddPoints"}));
                pas::WideString darkGreenColorTag = aMyFunction::DarkGreenColorTag;
                return aMyFunction::WrapTextInColor(pas::view(std::move(localizedColorText)), pas::view(std::move(darkGreenColorTag)));
            }())});
            aMyFunction::ReplaceTextToken(Result, u"<Points>"_w, pas::wide_int_to_str(I), pas::WideString());
        } else {
            Result = pas::WideString();
        }
        if (aPlayer::GetPlayer() == this) {
            aMyFunction::ReplaceTextToken(Result, u"<Star>"_w, CurrentPlanet->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
            aMyFunction::ReplaceTextToken(Result, u"<Planet>"_w, CurrentPlanet->Name, aMyFunction::TextHighlightColorTag);
        }
        PendingLiberationCeremonyPlanet = nullptr;
        PendingLiberationContribution = 0;
        return Result;
    }

    // Selects a merit award using the current planet, station or default human owner and returns its name.
    pas::WideString TNormalShip::AwardRandomMedal() {
        aConst::TRewardInfo cpp_result{};
        std::uint8_t Award{};
        if (CurrentPlanet != nullptr) {
            Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<TAwardTypeMask>({{aGalaxyStruct::atLiberation}, {aGalaxyStruct::atAccomplishment}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}}));
        } else if (DockedTo != nullptr) {
            Award = SelectAward(aConst::RaceToOwner(DockedTo->PilotRace), pas::constant_set<TAwardTypeMask>({{aGalaxyStruct::atLiberation}, {aGalaxyStruct::atAccomplishment}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}}));
        } else {
            Award = SelectAward(aGalaxyStruct::oiHuman, pas::constant_set<TAwardTypeMask>({{aGalaxyStruct::atLiberation}, {aGalaxyStruct::atAccomplishment}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, aGalaxyStruct::rstCustomStation}}));
        }
        AddAward(Award);
        return (TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name;
    }

    // Distributes kill rewards, career/rank changes and event counters.
    void TNormalShip_ProcessShipKill(TNormalShip* Self, aShip::TShip* Victim) {
        std::int32_t I{};
        std::int32_t SharedExperience{};
        std::int32_t ExperienceDelta{};
        std::int32_t ActivityAmount{};
        aShip::TShip* OtherShip{};
        TNormalShip* OtherNormal{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        aRanger::PQuest Quest{};
        // Nested helper; unused caller-popped static link.
        auto RecordShipKillCategory = [&](TNormalShip* Ship, aShip::TShip* Victim) -> void {
            switch (Victim->TypeId) {
                case aGalaxyStruct::stTransport: {
                    if (Victim->OwnerId != aGalaxyStruct::oiPirate) {
                        ++Ship->CivilianKillCount;
                        if (aPlayer::GetPlayer() == Ship) {
                            Achievements::TryAddAchievementProgress(u"BLACKHEAD"_w, 1);
                        }
                        Ship->CheckKillCountAwards(Victim);
                    }
                    break;
                }
                case aGalaxyStruct::stWarrior: {
                    ++Ship->MilitaryKillCount;
                    Ship->CheckKillCountAwards(Victim);
                    break;
                }
                case aGalaxyStruct::stRanger: {
                    if (pas::checked_cast<aRanger::TRanger*>(Victim)->GetDominantCareer() != aGalaxyStruct::rcPirate && Victim->OwnerId != aGalaxyStruct::oiPirate && static_cast<std::uint8_t>(static_cast<aRanger::TRanger*>(Victim)->ExcludedFromRating ^ 1)) {
                        ++Ship->RangerKillCount;
                        Ship->CheckKillCountAwards(Victim);
                    }
                    break;
                }
            }
        };
        if (Self == Victim) {
            return;
        }
        aGalaxyStruct::TExperienceSource SourceKind = aGalaxyStruct::esNormalShips;
        std::uint8_t QuestTargetKill = false;
        if (aPlayer::GetPlayer() == Self) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerKillsShip"_w, nullptr);
            Event->AddData(Victim->TypeId);
            Event->AddData(Victim->CurrentStar->Id);
            Event->AddData(Victim->Id);
            Event->AddData(Victim->OwnerId);
            Event->AddTextData(Victim->GetName());
            Event->AddData(Victim->GetFullHullRelativeStrengthPercent());
            Event->AddTextData(Victim->GetFullName(u" "_wref.get()));
            Event->AddTextData(Victim->TypeNameOverrideKey);
            if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Victim)) {
                Event->AddData(static_cast<std::uint8_t>(kling->KlingType));
            } else if (aTransport::TTransport* transport = pas::class_cast_if<aTransport::TTransport*>(Victim)) {
                Event->AddData(static_cast<std::uint8_t>(transport->TransportType));
            } else if (aWarrior::TWarrior* warrior = pas::class_cast_if<aWarrior::TWarrior*>(Victim)) {
                Event->AddData(static_cast<std::uint8_t>(warrior->WarriorType));
            } else if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Victim)) {
                Event->AddData(pirate->PirateType);
            } else {
                Event->AddData(0);
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Quests) - 1); cpp_range.next(I); ) {
                Quest = pas::list_at<aRanger::TQuest>(aPlayer::GetPlayer()->Quests, I);
                if (static_cast<std::uint8_t>(Quest->Successful ^ 1) && Quest->QuestType == aGalaxyStruct::qtKillShip && Quest->ObjectiveTarget == Victim) {
                    QuestTargetKill = true;
                }
            }
        }
        if (aPlayer::GetPlayer() == Self->PartnerShip) {
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerCompanionKillsShip"_w, nullptr);
            Event->AddData(Victim->TypeId);
            Event->AddData(Victim->CurrentStar->Id);
            Event->AddData(Victim->Id);
            Event->AddData(Victim->OwnerId);
            Event->AddTextData(Victim->GetName());
            Event->AddData(Self->TypeId);
            Event->AddData(Self->Id);
            Event->AddData(Self->OwnerId);
            Event->AddTextData(Self->GetName());
            Event->AddData(Victim->GetFullHullRelativeStrengthPercent());
            Event->AddTextData(Victim->GetFullName(u" "_wref.get()));
            Event->AddTextData(Victim->TypeNameOverrideKey);
            if (aKling::TKling* kling_2 = pas::class_cast_if<aKling::TKling*>(Victim)) {
                Event->AddData(static_cast<std::uint8_t>(kling_2->KlingType));
            } else if (aTransport::TTransport* transport_2 = pas::class_cast_if<aTransport::TTransport*>(Victim)) {
                Event->AddData(static_cast<std::uint8_t>(transport_2->TransportType));
            } else if (aWarrior::TWarrior* warrior_2 = pas::class_cast_if<aWarrior::TWarrior*>(Victim)) {
                Event->AddData(static_cast<std::uint8_t>(warrior_2->WarriorType));
            } else if (aPirate::TPirate* pirate_2 = pas::class_cast_if<aPirate::TPirate*>(Victim)) {
                Event->AddData(pirate_2->PirateType);
            } else {
                Event->AddData(0);
            }
        }
        std::int32_t Experience = 0;
        std::int32_t RankReward = 0;
        std::int32_t PirateReward = 0;
        ++Self->TotalShipKillCount;
        if (Self->CurrentStanding == aGalaxyStruct::ssCustom) {
            return;
        }
        if (Victim->CurrentStanding == aGalaxyStruct::ssCustom) {
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Custom));
            RankReward = 10;
            Experience = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
            SourceKind = aGalaxyStruct::esUnscaled;
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Self)) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 8;
                }
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    ranger->AddWarriorCareerActivity(ActivityAmount);
                }
            }
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace()) {
                static_cast<TNormalShip*>(Self->PartnerShip)->AddRankPoints(6);
            }
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                PirateReward = 8;
            }
        } else if (Victim->TypeId == aGalaxyStruct::stTransport && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
            if (!QuestTargetKill) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            }
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            {
                pas::Extended cpp_left = aMyFunction::NextRandomIntRange(100, 250, aGalaxy::Galaxy->RandomState);
                Experience = System::Round(cpp_left * (reinterpret_cast<TNormalShip*>(Victim)->Rank * 0.1L + 1.0L));
            }
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                PirateReward = 8;
                Experience = System::Round(Experience * 1.5L);
            }
            if (aPlayer::GetPlayer() == Self) {
                ActivityAmount = 4;
            } else {
                ActivityAmount = 1;
            }
            pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
        } else if (aRanger::TRanger* ranger_2 = pas::class_cast_if<aRanger::TRanger*>(Victim); ranger_2 != nullptr && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
            if (ranger_2->GetDominantCareer() == aGalaxyStruct::rcPirate) {
                SourceKind = aGalaxyStruct::esPirates;
                RankReward = 10;
                static_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(4);
                {
                    pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
                    Experience = System::Round(cpp_left_2 * (reinterpret_cast<TNormalShip*>(Victim)->PirateRank * 0.1L + 1.0L));
                }
            } else {
                if (!QuestTargetKill) {
                    aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
                    if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                        ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
                    }
                }
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 8;
                } else {
                    ActivityAmount = 2;
                }
                pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
                {
                    pas::Extended cpp_left_3 = aMyFunction::NextRandomIntRange(100, 250, aGalaxy::Galaxy->RandomState);
                    Experience = System::Round(cpp_left_3 * (reinterpret_cast<TNormalShip*>(Victim)->Rank * 0.1L + 1.0L));
                }
                if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                    PirateReward = 24;
                    Experience = System::Round(Experience * 1.5L);
                }
            }
        } else if (aRanger::TRanger* ranger_3 = pas::class_cast_if<aRanger::TRanger*>(Victim); ranger_3 != nullptr && pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr) {
            if (ranger_3->GetDominantCareer() != aGalaxyStruct::rcPirate) {
                ++Self->CurrentSystemKills.Normal;
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                    ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
                }
                PirateReward = 24;
            }
        } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
            SourceKind = aGalaxyStruct::esPirates;
            if (Victim->OwnerId == aGalaxyStruct::oiPirate && static_cast<std::uint8_t>(QuestTargetKill ^ 1)) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Pirate));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate == 0) {
                    ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate;
                }
            }
            ++Self->PirateKillCount;
            if (aPlayer::GetPlayer() == Self) {
                Achievements::TryAddAchievementProgress(u"SHIELD"_w, 1);
            }
            {
                pas::Extended cpp_left_4 = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
                Experience = System::Round(cpp_left_4 * (reinterpret_cast<TNormalShip*>(Victim)->PirateRank * 0.1L + 1.0L));
            }
            RankReward = 10;
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                Experience = Experience / 2;
            }
            if (aRanger::TRanger* ranger_4 = pas::class_cast_if<aRanger::TRanger*>(Self)) {
                ranger_4->AddWarriorCareerActivity(4);
            }
        } else if (aKling::TKling* kling_3 = pas::class_cast_if<aKling::TKling*>(Victim)) {
            ++Self->DominatorKillCount;
            if (aPlayer::GetPlayer() == Self) {
                ++aPlayer::GetPlayer()->DominatorKillsByType[kling_3->KlingType];
            }
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Dominator));
            RankReward = aConst::DominatorShipDefinitions[pas::checked_cast<aKling::TKling*>(Victim)->KlingType].RankPoints;
            Experience = System::Round(([&] {
                pas::Extended cpp_left_5 = aConst::DominatorShipDefinitions[static_cast<aKling::TKling*>(Victim)->KlingType].KillExperience;
                return cpp_left_5 * aGalaxy::Galaxy->GetDominatorKillExperienceScale();
            }()));
            SourceKind = aGalaxyStruct::esDominators;
            if (aRanger::TRanger* ranger_5 = pas::class_cast_if<aRanger::TRanger*>(Self)) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 8;
                }
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    ranger_5->AddWarriorCareerActivity(ActivityAmount);
                }
                if (aPlayer::GetPlayer() == Self) {
                    aPlayer::GetPlayer()->TryAwardDominatorPrograms(Victim);
                }
                if (aPlayer::GetPlayer() == Self && aPlayer::GetPlayer()->HasRadiationSickness()) {
                    Experience = System::Round(static_cast<long double>(aPlayer::GetPlayer()->RadiationHealth[1].Progress) * Experience);
                }
            }
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace()) {
                {
                    std::uint16_t cpp_arg = static_cast<std::int32_t>(aConst::DominatorShipDefinitions[pas::checked_cast<aKling::TKling*>(Victim)->KlingType].RankPoints) / 2 + 1;
                    TNormalShip* cpp_arg_2 = static_cast<TNormalShip*>(Self->PartnerShip);
                    cpp_arg_2->AddRankPoints(cpp_arg);
                }
                if (pas::checked_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Dominator == 0) {
                    ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Dominator;
                }
            }
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                PirateReward = aConst::DominatorShipDefinitions[pas::checked_cast<aKling::TKling*>(Victim)->KlingType].PirateRankPoints;
            }
        } else if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                {
                    pas::Extended cpp_left_6 = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
                    Experience = System::Round(cpp_left_6 * (reinterpret_cast<TNormalShip*>(Victim)->Rank * 0.1L + 1.0L));
                }
                if (pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType == aWarrior::wtFlagship) {
                    Experience *= 2;
                    PirateReward = 60;
                } else {
                    PirateReward = 16;
                }
            }
            if (aRanger::TRanger* ranger_6 = pas::class_cast_if<aRanger::TRanger*>(Self)) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 8;
                } else {
                    ActivityAmount = 2;
                }
                ranger_6->AddPirateCareerActivity(ActivityAmount);
            }
        } else if (Victim->TypeId == aGalaxyStruct::stTransport) {
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                PirateReward = 8;
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Victim->CurrentStanding == aGalaxyStruct::ssCoalitionMilitary) {
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                PirateReward = 32;
            }
            if (aRanger::TRanger* ranger_7 = pas::class_cast_if<aRanger::TRanger*>(Self)) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 8;
                } else {
                    ActivityAmount = 2;
                }
                ranger_7->AddPirateCareerActivity(ActivityAmount);
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Victim->CurrentStanding == aGalaxyStruct::ssCoalitionActive) {
            ++Self->CurrentSystemKills.Normal;
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                PirateReward = 24;
            }
            if (aRanger::TRanger* ranger_8 = pas::class_cast_if<aRanger::TRanger*>(Self)) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                ranger_8->AddPirateCareerActivity(ActivityAmount);
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::in_range(Victim->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssCoalitionPassive), static_cast<std::int32_t>(aGalaxyStruct::ssPiratePassive))) {
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                    ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
                }
            }
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Pirate));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate == 0) {
                    ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate;
                }
            }
            if (Victim->TypeId != aGalaxyStruct::rstPirateBase && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                static_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
            }
            if (Victim->TypeId == aGalaxyStruct::rstPirateBase && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                static_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(ActivityAmount);
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::in_range(Victim->CurrentStanding, static_cast<std::int32_t>(aGalaxyStruct::ssPirateActive), static_cast<std::int32_t>(aGalaxyStruct::ssPirateMilitary))) {
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Pirate));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate == 0) {
                    ++static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate;
                }
            }
            if (aRanger::TRanger* ranger_9 = pas::class_cast_if<aRanger::TRanger*>(Self)) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                ranger_9->AddWarriorCareerActivity(ActivityAmount);
            }
        }
        if (Victim->CurrentStanding != aGalaxyStruct::ssCustom) {
            if (aPlayer::GetPlayer() == Self && aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
                if (aWarrior::TWarrior* warrior_3 = pas::class_cast_if<aWarrior::TWarrior*>(Victim)) {
                    if (warrior_3->WarriorType == aWarrior::wtFlagship) {
                        aPlayer::GetPlayer()->PirateLicenseCash += System::Round(pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 2.0E+3L));
                    } else {
                        aPlayer::GetPlayer()->PirateLicenseCash += System::Round(pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 6.0E+3L));
                    }
                }
                if (Victim->TypeId == aGalaxyStruct::rstMilitaryBase) {
                    aPlayer::GetPlayer()->PirateLicenseCash += System::Round(pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 2.0E+3L));
                }
            }
            if (aPlayer::GetPlayer() == Self && Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckHaterAchievement();
            }
            RecordShipKillCategory(Self, Victim);
        }
        if (aPirate::TPirate* pirate_3 = pas::class_cast_if<aPirate::TPirate*>(Self)) {
            pirate_3->RaidPressure = 0.0f;
        }
        if (RankReward > 0 || Experience > 0 || PirateReward > 0) {
            if (RankReward > 0) {
                if (Self->OwnerId != aGalaxyStruct::oiPirate) {
                    Self->AddRankPoints(RankReward);
                }
                RankReward = RankReward / 2 + 1;
            }
            if (Experience > 0) {
                Self->GainExperience(Experience, SourceKind);
                if (Self->PartnerShip != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace()) {
                    SharedExperience = System::Round(Experience * aConst::LeadershipExperiencePercent[Self->PartnerShip->GetEffectiveSkillLevel(aGalaxyStruct::psLeadership, false)] * 0.01L);
                    if (aPlayer::GetPlayer() == Self->PartnerShip) {
                        Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerGotExpFromPartner"_w, nullptr);
                        Event->AddData(Self->Id);
                        Event->AddData(Self->PartnerShip->GetEffectiveSkillLevel(aGalaxyStruct::psLeadership, false));
                        if (SourceKind == aGalaxyStruct::esDominators) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByDominators;
                        } else if (SourceKind == aGalaxyStruct::esPirates) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByPirates;
                        } else if (SourceKind == aGalaxyStruct::esNormalShips) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByNormals;
                        } else {
                            ExperienceDelta = 0;
                        }
                        aPlayer::GetPlayer()->GainExperience(SharedExperience, SourceKind);
                        if (SourceKind == aGalaxyStruct::esDominators) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByDominators - ExperienceDelta;
                        } else if (SourceKind == aGalaxyStruct::esPirates) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByPirates - ExperienceDelta;
                        } else if (SourceKind == aGalaxyStruct::esNormalShips) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByNormals - ExperienceDelta;
                        }
                        Event->AddData(SourceKind);
                        Event->AddData(SharedExperience);
                        Event->AddData(ExperienceDelta);
                    } else {
                        pas::checked_cast<TNormalShip*>(Self->PartnerShip)->GainExperience(SharedExperience, SourceKind);
                    }
                }
                Experience = Experience / 2 + 1;
            }
            if (PirateReward > 0) {
                if (Self->OwnerId == aGalaxyStruct::oiPirate) {
                    Self->AddPirateRankPoints(PirateReward);
                }
                PirateReward = PirateReward / 2 + 1;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                OtherShip = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                if (OtherShip == Self || static_cast<std::uint8_t>(OtherShip->InNormalSpace() ^ 1) || static_cast<std::uint8_t>(OtherShip->IsAttackingShip(Victim) ^ 1)) {
                    continue;
                }
                if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(OtherShip); tranclucator != nullptr && aPlayer::GetPlayer() == tranclucator->OwnerShip) {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerTranclucatorAssistKillsShip"_w, nullptr);
                    Event->AddData(Victim->TypeId);
                    Event->AddData(Victim->CurrentStar->Id);
                    Event->AddData(Victim->Id);
                    Event->AddData(Victim->OwnerId);
                    Event->AddTextData(Victim->GetName());
                    Event->AddData(OtherShip->Id);
                    Event->AddData(OtherShip->OwnerId);
                    Event->AddTextData(OtherShip->GetName());
                    Event->AddData(Victim->GetFullHullRelativeStrengthPercent());
                    Event->AddTextData(Victim->GetFullName(u" "_wref.get()));
                    Event->AddTextData(Victim->TypeNameOverrideKey);
                    if (aKling::TKling* kling_4 = pas::class_cast_if<aKling::TKling*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(kling_4->KlingType));
                    } else if (aTransport::TTransport* transport_3 = pas::class_cast_if<aTransport::TTransport*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(transport_3->TransportType));
                    } else if (aWarrior::TWarrior* warrior_4 = pas::class_cast_if<aWarrior::TWarrior*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(warrior_4->WarriorType));
                    } else if (aPirate::TPirate* pirate_4 = pas::class_cast_if<aPirate::TPirate*>(Victim)) {
                        Event->AddData(pirate_4->PirateType);
                    } else {
                        Event->AddData(0);
                    }
                }
                if (!(pas::class_cast_if<TNormalShip*>(OtherShip) != nullptr)) {
                    continue;
                }
                OtherNormal = static_cast<TNormalShip*>(OtherShip);
                if (aPlayer::GetPlayer() == OtherShip) {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerAssistKillsShip"_w, nullptr);
                    Event->AddData(Victim->TypeId);
                    Event->AddData(Victim->CurrentStar->Id);
                    Event->AddData(Victim->Id);
                    Event->AddData(Victim->OwnerId);
                    Event->AddTextData(Victim->GetName());
                    Event->AddData(Victim->GetFullHullRelativeStrengthPercent());
                    Event->AddTextData(Victim->GetFullName(u" "_wref.get()));
                    Event->AddTextData(Victim->TypeNameOverrideKey);
                    if (aKling::TKling* kling_5 = pas::class_cast_if<aKling::TKling*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(kling_5->KlingType));
                    } else if (aTransport::TTransport* transport_4 = pas::class_cast_if<aTransport::TTransport*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(transport_4->TransportType));
                    } else if (aWarrior::TWarrior* warrior_5 = pas::class_cast_if<aWarrior::TWarrior*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(warrior_5->WarriorType));
                    } else if (aPirate::TPirate* pirate_5 = pas::class_cast_if<aPirate::TPirate*>(Victim)) {
                        Event->AddData(pirate_5->PirateType);
                    } else {
                        Event->AddData(0);
                    }
                }
                if (aPlayer::GetPlayer() == OtherShip->PartnerShip) {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerCompanionAssistKillsShip"_w, nullptr);
                    Event->AddData(Victim->TypeId);
                    Event->AddData(Victim->CurrentStar->Id);
                    Event->AddData(Victim->Id);
                    Event->AddData(Victim->OwnerId);
                    Event->AddTextData(Victim->GetName());
                    Event->AddData(OtherShip->TypeId);
                    Event->AddData(OtherShip->Id);
                    Event->AddData(OtherShip->OwnerId);
                    Event->AddTextData(OtherShip->GetName());
                    Event->AddData(Victim->GetFullHullRelativeStrengthPercent());
                    Event->AddTextData(Victim->GetFullName(u" "_wref.get()));
                    Event->AddTextData(Victim->TypeNameOverrideKey);
                    if (aKling::TKling* kling_6 = pas::class_cast_if<aKling::TKling*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(kling_6->KlingType));
                    } else if (aTransport::TTransport* transport_5 = pas::class_cast_if<aTransport::TTransport*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(transport_5->TransportType));
                    } else if (aWarrior::TWarrior* warrior_6 = pas::class_cast_if<aWarrior::TWarrior*>(Victim)) {
                        Event->AddData(static_cast<std::uint8_t>(warrior_6->WarriorType));
                    } else if (aPirate::TPirate* pirate_6 = pas::class_cast_if<aPirate::TPirate*>(Victim)) {
                        Event->AddData(pirate_6->PirateType);
                    } else {
                        Event->AddData(0);
                    }
                }
                if (aPirate::TPirate* pirate_7 = pas::class_cast_if<aPirate::TPirate*>(OtherNormal)) {
                    pirate_7->RaidPressure = 0.0f;
                }
                if (RankReward > 0 && OtherNormal->OwnerId != aGalaxyStruct::oiPirate) {
                    OtherNormal->AddRankPoints(RankReward);
                }
                if (Experience > 0) {
                    OtherNormal->GainExperience(Experience, SourceKind);
                }
                if (PirateReward > 0 && OtherNormal->OwnerId == aGalaxyStruct::oiPirate) {
                    OtherNormal->AddPirateRankPoints(PirateReward);
                }
                ++OtherNormal->TotalShipKillCount;
                if (Victim->CurrentStanding == aGalaxyStruct::ssCustom) {
                    aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&OtherNormal->CurrentSystemKills.Custom));
                } else if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
                    ++OtherNormal->DominatorKillCount;
                    aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&OtherNormal->CurrentSystemKills.Dominator));
                    if (aPlayer::GetPlayer() == OtherShip) {
                        ++aPlayer::GetPlayer()->DominatorKillsByType[pas::checked_cast<aKling::TKling*>(Victim)->KlingType];
                    }
                } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr || pas::class_cast_if<aRanger::TRanger*>(Victim) != nullptr && static_cast<aRanger::TRanger*>(Victim)->GetDominantCareer() == aGalaxyStruct::rcPirate) {
                    if (Victim->OwnerId == aGalaxyStruct::oiPirate && static_cast<std::uint8_t>(QuestTargetKill ^ 1)) {
                        aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&OtherNormal->CurrentSystemKills.Pirate));
                    }
                    ++OtherNormal->PirateKillCount;
                    if (aPlayer::GetPlayer() == OtherShip && pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
                        Achievements::TryAddAchievementProgress(u"SHIELD"_w, 1);
                    }
                } else if (pas::class_cast_if<TNormalShip*>(Victim) != nullptr && pas::contains(aConst::PlanetOwnerMasks.Coalition, Victim->OwnerId)) {
                    if (!QuestTargetKill) {
                        aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&OtherNormal->CurrentSystemKills.Normal));
                    }
                } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::contains(aConst::FactionStandingMasks[Self->CurrentStar->Status.ControlFaction], Victim->CurrentStanding)) {
                    aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(pas::byte_offset(&OtherNormal->CurrentSystemKills, Self->CurrentStar->Status.ControlFaction * sizeof(std::uint16_t))));
                }
                RecordShipKillCategory(OtherNormal, Victim);
            }
        }
    }

    void TNormalShip::CheckKillCountAwards(aShip::TShip* Victim) {
        // Caller-popped static link; ship at ParentFrame-4.
        auto Check = [&](std::int32_t InitialThreshold, std::int32_t Multiplier, std::uint16_t Count, aGalaxyStruct::TShipType VictimType) -> void {
            aConst::TRewardInfo cpp_result{};
            static const pas::Set<0, 255> BadAwards = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::atPerfidy}});
            std::int32_t Award{};
            pas::WideString Text{};
            pas::WideString ShipTypeName{};
            if (aGalaxy::Galaxy->CoalitionDefeatedTurn > 0 || Count == 65535) {
                return;
            }
            std::int32_t Threshold = InitialThreshold;
            std::int32_t I = 1;
            do {
                if (Count < Threshold) {
                    break;
                }
                if (Count == Threshold) {
                    Award = SelectAward(this->OwnerId, static_cast<TAwardTypeMask>(BadAwards), pas::make_set<aGalaxyStruct::TShipTypeMask>({{static_cast<std::int32_t>(VictimType)}})) & 0x000000ff;
                    if (Award != aGalaxyStruct::AwardNotFound) {
                        AddAward(Award);
                        if (aPlayer::GetPlayer() == this) {
                            ShipTypeName = aConst::ShipTypeNames[VictimType].Name;
                            Text = aConst::PickLocalizedTextVariant(pas::concat_wide({u"GalaxyNews.BadReward.Kill", ShipTypeName}), this->Seed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn / 10));
                            {
                                auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                                pas::WideString name = (TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name;
                                aMyFunction::ReplaceTextToken(Text, u"<Reward>"_w, std::move(name), textHighlightColorTag.get());
                            }
                            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
                        }
                    }
                    break;
                }
                Threshold = std::min<std::int32_t>(Threshold * Multiplier, 10000000);
                ++I;
            } while (!(I == 9));
        };
        switch (Victim->TypeId) {
            case aGalaxyStruct::stTransport: Check(5, 5, CivilianKillCount, aGalaxyStruct::stTransport); break;
            case aGalaxyStruct::stWarrior: Check(3, 3, MilitaryKillCount, aGalaxyStruct::stWarrior); break;
            case aGalaxyStruct::stRanger: {
                if (TypeId == aGalaxyStruct::stRanger) {
                    Check(2, 4, RangerKillCount, aGalaxyStruct::stRanger);
                }
                break;
            }
        }
    }

    // Nearby rangers attacking a friend incur a penalty; attacks on an enemy can improve relations.
    void TNormalShip_UpdateRelationsForNearbyCombat(TNormalShip* Self) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        aShip::TShip* Target{};
        aRanger::TRanger* Ranger{};
        std::uint8_t Change{};
        if (static_cast<std::uint8_t>(Self->InNormalSpace() ^ 1) || aPlayer::GetPlayer() == Self) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Ship); ranger != nullptr && Ship != Self && Ship->InNormalSpace()) {
                Ranger = ranger;
                if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Ranger->OrderTarget); ship != nullptr && Ranger->OrderTarget == Ranger->EnemyShip) {
                    Target = ship;
                    if (aShip::TShip_GetRelationLevelToShip(Self, Target) == aGalaxyStruct::rlExcellent) {
                        if (aPlayer::GetPlayer() == Ship || aMyFunction::NextRandomUnitFloat(Self->RandomState) <= 0.1L) {
                            Change = Target->OrderTarget != Ranger && aShip::TShip_GetRelationLevelToShip(Target, Ranger) == aGalaxyStruct::rlHostile;
                            if (Change) {
                                Self->ChangeRelationToRanger(Ranger, -2);
                            }
                        }
                    } else if (aShip::TShip_GetRelationLevelToShip(Self, Target) == aGalaxyStruct::rlHostile) {
                        Change = aShip::TShip_GetRelationLevelToShip(Target, Ranger) == aGalaxyStruct::rlHostile;
                        if (Change) {
                            Self->ChangeRelationToRanger(Ranger, 2);
                        }
                    }
                }
            }
        }
    }

    // Returns 255 when no award qualifies; retries duplicates twice.
    std::uint8_t TNormalShip::SelectAward(aGalaxyStruct::TOwnerId Owner, TAwardTypeMask Kinds, aGalaxyStruct::TShipTypeMask VictimTypes) {
        std::uint8_t Result{};
        std::int32_t I{};
        pas::WideString KillName{};
        pas::List* Candidates = pas::make_object<pas::List>();
        std::int32_t Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"Reward.Count"_wref.get()))) - 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count); cpp_range.next(I); ) {
            if (aConst::MatchesOwnerName(Owner, pas::view(GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Race"}))))) && pas::contains(Kinds, aConst::SysToReward(pas::view(GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Type"})))))) && ([&] {
                const pas::WideString& lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Status"})));
                aGalaxyStruct::TRangerCareer dominantCareer = GetDominantCareer();
                return aConst::MatchesCareerName(dominantCareer, pas::view(lookupLocalizedTextByKey));
            }())) {
                KillName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Kill"})));
                if (KillName.length() == 0 || pas::contains(VictimTypes, aConst::SysToShipType(pas::view(KillName)))) {
                    pas::list_add(Candidates, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(I))));
                }
            }
        }
        if (pas::list_count(Candidates) > 0) {
            Result = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Candidates, aMyFunction::SeededRandomIntRange(0, pas::list_count(Candidates) - 1, (static_cast<std::int32_t>(Seed) + aGalaxy::Galaxy->CurrentTurn) / 101))));
            if (AwardIds != nullptr && pas::list_indexof(AwardIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Result)))) >= 0) {
                Result = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Candidates, aMyFunction::SeededRandomIntRange(0, pas::list_count(Candidates) - 1, (static_cast<std::int32_t>(Seed) + 2 * aGalaxy::Galaxy->CurrentTurn) / 101))));
            }
            if (AwardIds != nullptr && pas::list_indexof(AwardIds, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Result)))) >= 0) {
                Result = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(Candidates, aMyFunction::SeededRandomIntRange(0, pas::list_count(Candidates) - 1, (static_cast<std::int32_t>(Seed) + 3 * aGalaxy::Galaxy->CurrentTurn) / 101))));
            }
        } else {
            Result = aGalaxyStruct::AwardNotFound;
        }
        pas::free(Candidates);
        return Result;
    }

    void TNormalShip::GetAwardInfo(std::uint8_t AwardId, aConst::TRewardInfo& Result) {
        Result.AwardId = AwardId;
        Result.Name = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(AwardId), ".Name"})));
        Result.Text = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(AwardId), ".Text"})));
        return;
    }

    pas::WideString TNormalShip::GetRankName() {
        return aConst::LocalizedText(pas::concat_wide({u"Rank.", aConst::CoalitionRankNames[Rank], u".Name"}));
    }

    pas::WideString TNormalShip::GetRankLongName() {
        return aConst::LocalizedText(pas::concat_wide({u"Rank.", aConst::CoalitionRankNames[Rank], u".NameBig"}));
    }

    pas::WideString TNormalShip::GetRankDescription() {
        return aConst::LocalizedColorText(pas::concat_wide({u"Rank.", aConst::CoalitionRankNames[Rank], u".Text"}));
    }

    // Does not check for maximum rank.
    pas::WideString TNormalShip::GetNextRankName() {
        return aConst::LocalizedText(pas::concat_wide({u"Rank.", aConst::CoalitionRankNames[Rank + 1], u".Name"}));
    }

    std::uint16_t TNormalShip::GetRankPointsToNextRank() {
        if (Rank < 7 && aConst::CoalitionRankPointThresholds[Rank] > RankPoints) {
            return aConst::CoalitionRankPointThresholds[Rank] - RankPoints;
        }
        return 0;
    }

    // Caps the addition at the points needed for the next rank.
    void TNormalShip::AddRankPoints(std::uint16_t Amount) {
        std::int32_t Needed = GetRankPointsToNextRank();
        RankPoints += std::min<std::int32_t>(static_cast<std::int32_t>(Amount), Needed);
        if (Needed > 0 && aPlayer::GetPlayer() == this && aPlayer::GetPlayer()->CanPromoteRank() && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            const pas::WideString& formatText1 = ([&] {
                pas::WideString nextRankName = aPlayer::GetPlayer()->GetNextRankName();
                pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.WB.NewRank"_wref.get(), Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant), std::move(textHighlightColorTag), u"<Rank>"_w, std::move(nextRankName));
            }());
            std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
            Globals::AddOrUpdatePlayerBubble(Globals::pmGalaxyNews, currentTurn, formatText1, u""_wref.get());
        }
    }

    // Maximum rank is 7; promotion resets RankPoints.
    std::uint8_t TNormalShip::TryPromoteRank() {
        if (Rank < 7 && GetRankPointsToNextRank() == 0) {
            ++Rank;
            RankPoints = 0;
            return true;
        }
        return false;
    }

    std::uint8_t TNormalShip::CanPromoteRank() {
        return Rank < 7 && GetRankPointsToNextRank() == 0;
    }

    pas::WideString TNormalShip::GetPirateRankName() {
        return aConst::LocalizedText(pas::concat_wide({u"RankPirate.", aConst::PirateRankNames[PirateRank], u".Name"}));
    }

    pas::WideString TNormalShip::GetPirateRankLongName() {
        return aConst::LocalizedText(pas::concat_wide({u"RankPirate.", aConst::PirateRankNames[PirateRank], u".NameBig"}));
    }

    pas::WideString TNormalShip::GetPirateRankDescription() {
        return aConst::LocalizedColorText(pas::concat_wide({u"RankPirate.", aConst::PirateRankNames[PirateRank], u".Text"}));
    }

    // Does not check maximum rank.
    pas::WideString TNormalShip::GetNextPirateRankName() {
        return aConst::LocalizedText(pas::concat_wide({u"RankPirate.", aConst::PirateRankNames[PirateRank + 1], u".Name"}));
    }

    std::uint16_t TNormalShip::GetPirateRankPointsToNextRank() {
        if (PirateRank < 7 && aConst::PirateRankPointThresholds[PirateRank] > PirateRankPoints) {
            return aConst::PirateRankPointThresholds[PirateRank] - PirateRankPoints;
        }
        return 0;
    }

    // Caps the addition at the points needed for the next rank.
    void TNormalShip::AddPirateRankPoints(std::uint32_t Amount) {
        std::int32_t Needed = GetPirateRankPointsToNextRank();
        PirateRankPoints += std::min<std::int64_t>(static_cast<std::int64_t>(Amount), static_cast<std::int64_t>(Needed));
    }

    // Maximum rank is 7; promotion resets PirateRankPoints.
    std::uint8_t TNormalShip::TryPromotePirateRank() {
        if (PirateRank < 7 && GetPirateRankPointsToNextRank() == 0) {
            ++PirateRank;
            if (aPlayer::GetPlayer() == this) {
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckBaronAchievement();
            }
            PirateRankPoints = 0u;
            return true;
        }
        return false;
    }

    std::uint8_t TNormalShip::CanPromotePirateRank() {
        return PirateRank < 7 && GetPirateRankPointsToNextRank() == 0;
    }

    // Automatic messages suppress object links and select the automatic-message category.
    pas::WideString TNormalShip_SelectSituationalMessage(TNormalShip* Self, std::uint8_t Automatic) {
        pas::DynArray<Globals::TShipGreetingsInfo> Definitions{};
        std::int32_t LastIndex{};
        Globals::TShipGreetingsInfo SwapA{};
        Globals::TShipGreetingsInfo SwapB{};
        pas::WideString ItemTypes{};
        pas::WideString MessageText{};
        pas::WideString BestText{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t Count{};
        std::int32_t EntryIndex{};
        std::uint8_t Good{};
        std::uint8_t Rejected{};
        aPlanet::TPlanet* Planet{};
        aItem::TItem* Item{};
        aGalaxyStruct::TShipType ShipKind{};
        aGalaxyStruct::TGreetingCountMask CountMask{};
        aShip::TShip* Other{};
        // Nested helper; caller-popped static link. Copies definitions and swaps the first half against deterministic random positions.
        auto ShuffleDefinitions = [&]() -> void {
            std::int32_t I{};
            std::int32_t OtherIndex{};
            Definitions.set_length(Globals::ShipGreetingCount);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, LastIndex); cpp_range.next(I); ) {
                Definitions[I] = Globals::ShipGreetingDefinitions[I];
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, LastIndex / 2); cpp_range_2.next(I); ) {
                OtherIndex = aMyFunction::SeededRandomIntRange(0, LastIndex, Self->Seed + 7 * I);
                SwapA = Definitions[OtherIndex];
                SwapB = Definitions[I];
                Definitions[I] = SwapA;
                Definitions[OtherIndex] = SwapB;
            }
        };
        if (aPlayer::GetPlayer() == Self->PartnerShip || aPlayer::GetPlayer()->ChameleonActive || Self->HasIndependentScriptFaction()) {
            return pas::WideString();
        }
        ItemTypes = pas::WideString();
        std::int32_t BestPriority = -1;
        std::int32_t CandidatePriority = -1;
        std::int32_t Minimum = 0;
        LastIndex = Globals::ShipGreetingCount - 1;
        ShuffleDefinitions();
        EntryIndex = aMyFunction::SeededRandomIntRange(0, LastIndex, static_cast<std::int32_t>(Self->Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) / 20);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LastIndex); cpp_range.next(I); ) {
            MessageText = pas::WideString();
            aMyFunction::IncrementWrapped(EntryIndex, Minimum, LastIndex);
            if (Self->IsFemaleHumanPilot() != (Definitions[EntryIndex].Female == 0)) {
                continue;
            }
            if (Definitions[EntryIndex].CoalitionAlreadyDefeated != Globals::gcAny && (Definitions[EntryIndex].CoalitionAlreadyDefeated == Globals::gcYes && !(aGalaxy::Galaxy->CoalitionDefeatedTurn != 0) || Definitions[EntryIndex].CoalitionAlreadyDefeated == Globals::gcNo && aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                continue;
            }
            if (Definitions[EntryIndex].DominatorsAlreadyDefeated != Globals::gcAny && (Definitions[EntryIndex].DominatorsAlreadyDefeated == Globals::gcYes && static_cast<std::uint8_t>(static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) ^ 1) ^ 1) || Definitions[EntryIndex].DominatorsAlreadyDefeated == Globals::gcNo && static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) ^ 1))) {
                continue;
            }
            if (BestPriority > 0) {
                CandidatePriority = Definitions[EntryIndex].Priority;
                {
                    std::int32_t cpp_left = CandidatePriority * aMyFunction::SeededRandomIntRange(1, 100, Self->Seed + EntryIndex * (aGalaxy::Galaxy->CurrentTurn / 20));
                    if (cpp_left < BestPriority * aMyFunction::SeededRandomIntRange(1, 100, Self->Seed + EntryIndex * (aGalaxy::Galaxy->CurrentTurn / 20) * 3)) {
                        continue;
                    }
                }
            }
            Good = aGalaxyStruct::NoGreetingGoods;
            if (Definitions[EntryIndex].Goods != aGalaxyStruct::UnspecifiedGoods) {
                Good = Definitions[EntryIndex].Goods;
            }
            if (Definitions[EntryIndex].AutoTalk != Globals::gcAny && (Automatic && Definitions[EntryIndex].AutoTalk == Globals::gcNo || static_cast<std::uint8_t>(Automatic ^ 1) && Definitions[EntryIndex].AutoTalk == Globals::gcYes)) {
                continue;
            }
            if (Definitions[EntryIndex].FlyType == Globals::gfAny) {
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
            } else if (Definitions[EntryIndex].FlyType == Globals::gfToPlanet) {
                if (!(pas::class_cast_if<aPlanet::TPlanet*>(Self->OrderTarget) != nullptr)) {
                    continue;
                }
                Planet = static_cast<aPlanet::TPlanet*>(Self->OrderTarget);
                if (!pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId)) {
                    continue;
                }
                if (Self->CurrentStar->Status.CustomFaction != u"") {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToPlanetRace, Planet->RaceId) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRelations != pas::constant_set<aGalaxyStruct::TRelationLevels>({}) && static_cast<std::uint8_t>(([&] {
                    aGalaxyStruct::TRelationLevel cpp_element = Planet->GetRelationLevelToShip(aPlayer::GetPlayer());
                    const aGalaxyStruct::TRelationLevels& cpp_set = Definitions[EntryIndex].ToPlanetRelations;
                    return pas::contains(cpp_set, cpp_element);
                }()) ^ 1)) {
                    continue;
                }
                if (Good != aGalaxyStruct::NoGreetingGoods) {
                    if (Definitions[EntryIndex].ToPlanetGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToPlanetGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(Planet->Goods[Good].Count, Good)) ^ 1)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToPlanetGoodsSale != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                        std::uint8_t cpp_element_2 = ([&] {
                            std::int32_t shopGoodsPurchasePrice = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet);
                            aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                            return galaxy->ClassifyGoodsPrice(shopGoodsPurchasePrice, Good);
                        }());
                        const Globals::TGreetingMask& cpp_set_2 = Definitions[EntryIndex].ToPlanetGoodsSale;
                        return pas::contains(cpp_set_2, cpp_element_2);
                    }()) ^ 1)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToPlanetGoodsBuy != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                        std::uint8_t cpp_element_3 = ([&] {
                            std::int32_t shopGoodsSellPrice = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet);
                            aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
                            return galaxy_2->ClassifyGoodsPrice(shopGoodsSellPrice, Good);
                        }());
                        const Globals::TGreetingMask& cpp_set_3 = Definitions[EntryIndex].ToPlanetGoodsBuy;
                        return pas::contains(cpp_set_3, cpp_element_3);
                    }()) ^ 1)) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].ToPlanetIsHomePlanet != Globals::gcAny && (Definitions[EntryIndex].ToPlanetIsHomePlanet == Globals::gcYes && !(Self->HomePlanet == Planet) || Definitions[EntryIndex].ToPlanetIsHomePlanet == Globals::gcNo && Self->HomePlanet == Planet)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRaceIsShipRace != Globals::gcAny && (Definitions[EntryIndex].ToPlanetRaceIsShipRace == Globals::gcYes && !(Planet->RaceId == Self->PilotRace) || Definitions[EntryIndex].ToPlanetRaceIsShipRace == Globals::gcNo && Planet->RaceId == Self->PilotRace)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRaceIsPlayerRace != Globals::gcAny && (Definitions[EntryIndex].ToPlanetRaceIsPlayerRace == Globals::gcYes && !(aPlayer::GetPlayer()->PilotRace == Planet->RaceId) || Definitions[EntryIndex].ToPlanetRaceIsPlayerRace == Globals::gcNo && aPlayer::GetPlayer()->PilotRace == Planet->RaceId)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetEconomy != pas::constant_set<aGalaxyStruct::TPlanetEconomies>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToPlanetEconomy, Planet->Economy) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetGovernment != pas::constant_set<aGalaxyStruct::TPlanetGovernments>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToPlanetGovernment, Planet->Government) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetIsLastPlanet != Globals::gcAny && (Definitions[EntryIndex].ToPlanetIsLastPlanet == Globals::gcYes && !(Self->LastDockedPlanet == Planet) || Definitions[EntryIndex].ToPlanetIsLastPlanet == Globals::gcNo && Self->LastDockedPlanet == Planet)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRaceIsLastPlanetRace != Globals::gcAny) {
                    if (!pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Self->LastDockedPlanet->OwnerId)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToPlanetRaceIsLastPlanetRace == Globals::gcYes && !(Planet->RaceId == Self->LastDockedPlanet->RaceId) || Definitions[EntryIndex].ToPlanetRaceIsLastPlanetRace == Globals::gcNo && Planet->RaceId == Self->LastDockedPlanet->RaceId) {
                        continue;
                    }
                }
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToPlanet>"_w, pas::concat_wide({Planet->Name, aGalaxy::GetLocalObjectLink(Planet, Automatic)}), aMyFunction::TextHighlightColorTag);
                if (Good != aGalaxyStruct::NoGreetingGoods) {
                    MessageText = ([&] {
                        auto textHighlightColorTag = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet));
                        return aMyFunction::ReplaceColoredToken(MessageText, u"<ToPlanetGoodsSale>"_w, std::move(intToStr), textHighlightColorTag.get());
                    }());
                    MessageText = ([&] {
                        auto textHighlightColorTag_2 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet));
                        return aMyFunction::ReplaceColoredToken(MessageText, u"<ToPlanetGoodsBuy>"_w, std::move(intToStr_2), textHighlightColorTag_2.get());
                    }());
                }
            } else if (Definitions[EntryIndex].FlyType == Globals::gfToStar) {
                if (!(pas::class_cast_if<aGalaxy::TStar*>(Self->OrderTarget) != nullptr)) {
                    continue;
                }
                if (Definitions[EntryIndex].HomePlanetInToStar != Globals::gcAny && (Definitions[EntryIndex].HomePlanetInToStar == Globals::gcYes && !(Self->HomePlanet->CurrentStar == Self->OrderTarget) || Definitions[EntryIndex].HomePlanetInToStar == Globals::gcNo && Self->HomePlanet->CurrentStar == Self->OrderTarget)) {
                    continue;
                }
                if (Definitions[EntryIndex].HomePlanetInCurStar != Globals::gcAny && (Definitions[EntryIndex].HomePlanetInCurStar == Globals::gcYes && !(Self->HomePlanet->CurrentStar == Self->CurrentStar) || Definitions[EntryIndex].HomePlanetInCurStar == Globals::gcNo && Self->HomePlanet->CurrentStar == Self->CurrentStar)) {
                    continue;
                }
                Rejected = false;
                for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::stWarrior); cpp_range_2.next(ShipKind); ) {
                    switch (ShipKind) {
                        case aGalaxyStruct::stKling: CountMask = Definitions[EntryIndex].KlingInToStar; break;
                        case aGalaxyStruct::stRanger: CountMask = Definitions[EntryIndex].RangerInToStar; break;
                        case aGalaxyStruct::stPirate: CountMask = Definitions[EntryIndex].PirateInToStar; break;
                        case aGalaxyStruct::stWarrior: CountMask = Definitions[EntryIndex].WarriorInToStar; break;
                        case aGalaxyStruct::stTransport: CountMask = Definitions[EntryIndex].TransportInToStar; break;
                    }
                    if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                        Count = 0;
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Ships) - 1); cpp_range_3.next(K); ) {
                            Other = pas::list_at<aShip::TShip>(static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Ships, K);
                            if (static_cast<std::uint8_t>(Other->HasScriptStateText() ^ 1) && Other->TypeNameOverrideKey == u"" && Other->TypeId == ShipKind) {
                                ++Count;
                            }
                        }
                        Count = std::min<std::int32_t>(10, Count);
                        if (!pas::contains(CountMask, static_cast<std::uint32_t>(Count))) {
                            Rejected = true;
                            break;
                        }
                    }
                }
                if (Rejected) {
                    continue;
                }
                if (Definitions[EntryIndex].ToStarControlByKling != Globals::gcAny) {
                    if (static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.CustomFaction != u"") {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToStarControlByKling == Globals::gcYes && !(static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfDominators) || Definitions[EntryIndex].ToStarControlByKling == Globals::gcNo && static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].ToStarControlByPirates != Globals::gcAny) {
                    if (static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.CustomFaction != u"") {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToStarControlByPirates == Globals::gcYes && !(static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfPirates) || Definitions[EntryIndex].ToStarControlByPirates == Globals::gcNo && static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].ToStarInBattle != Globals::gcAny && (Definitions[EntryIndex].ToStarInBattle == Globals::gcYes && !(static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.Battle != 0) || Definitions[EntryIndex].ToStarInBattle == Globals::gcNo && static_cast<aGalaxy::TStar*>(Self->OrderTarget)->Status.Battle != 0)) {
                    continue;
                }
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToStar>"_w, pas::checked_cast<aGalaxy::TStar*>(Self->OrderTarget)->Name, aMyFunction::TextHighlightColorTag);
            } else if (Definitions[EntryIndex].FlyType == Globals::gfToItem) {
                if (Self->Order != aShip::soMove || static_cast<std::uint8_t>(Self->OrderAbsolute ^ 1)) {
                    continue;
                }
                Rejected = false;
                Item = nullptr;
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Items) - 1); cpp_range_4.next(J); ) {
                    Item = pas::list_at<aItem::TItem>(Self->CurrentStar->Items, J);
                    // Native accepts either matching coordinate, rather than requiring both.
                    if (Self->GetPickupApproachPosition(Item->Position).X == Self->OrderDestination.X || Self->GetPickupApproachPosition(Item->Position).Y == Self->OrderDestination.Y) {
                        ItemTypes = Definitions[EntryIndex].ItemType;
                        if (ItemTypes == u"" || ItemTypes == u"Any" || EC_Str::FindTextPosW(Item->GetCategoryConfigName(), ItemTypes) != 0) {
                            if (Definitions[EntryIndex].ShipNeedInItem != Globals::gcAny && (Definitions[EntryIndex].ShipNeedInItem == Globals::gcYes && static_cast<std::uint8_t>(Self->ShouldPickUpItem(Item) ^ 1) || Definitions[EntryIndex].ShipNeedInItem == Globals::gcNo && Self->ShouldPickUpItem(Item))) {
                                continue;
                            }
                            Rejected = true;
                            break;
                        }
                    }
                }
                if (!Rejected) {
                    continue;
                }
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
                MessageText = ([&] {
                    auto textHighlightColorTag_3 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString cpp_arg = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Item, Automatic), Item->GetDisplayName()});
                    return aMyFunction::ReplaceColoredToken(MessageText, u"<Item>"_w, std::move(cpp_arg), textHighlightColorTag_3.get());
                }());
            } else if (Definitions[EntryIndex].FlyType == Globals::gfToShip) {
                if (!(pas::class_cast_if<aShip::TShip*>(Self->OrderTarget) != nullptr)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipType != pas::constant_set<Globals::TGreetingShipCategories>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToShipType, static_cast<aShip::TShip*>(Self->OrderTarget)->GetGreetingShipCategory()) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToShipRace, static_cast<aShip::TShip*>(Self->OrderTarget)->PilotRace) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipInPlanet != Globals::gcAny && (Definitions[EntryIndex].ToShipInPlanet == Globals::gcYes && !(static_cast<aShip::TShip*>(Self->OrderTarget)->CurrentPlanet != nullptr) || Definitions[EntryIndex].ToShipInPlanet == Globals::gcNo && static_cast<aShip::TShip*>(Self->OrderTarget)->CurrentPlanet != nullptr)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipBad != Globals::gcAny && (Definitions[EntryIndex].ToShipBad == Globals::gcYes && !(static_cast<aShip::TShip*>(Self->OrderTarget)->EnemyShip == Self) || Definitions[EntryIndex].ToShipBad == Globals::gcNo && static_cast<aShip::TShip*>(Self->OrderTarget)->EnemyShip == Self)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipRelations != pas::constant_set<aGalaxyStruct::TRelationLevels>({}) && static_cast<std::uint8_t>(([&] {
                    aGalaxyStruct::TRelationLevel cpp_element_4 = aShip::TShip_GetRelationLevelToShip(Self, pas::checked_cast<aShip::TShip*>(Self->OrderTarget));
                    const aGalaxyStruct::TRelationLevels& cpp_set_4 = Definitions[EntryIndex].ToShipRelations;
                    return pas::contains(cpp_set_4, cpp_element_4);
                }()) ^ 1)) {
                    continue;
                }
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
                MessageText = ([&] {
                    auto textHighlightColorTag_4 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString cpp_arg_2 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Self->OrderTarget, Automatic), pas::checked_cast<aShip::TShip*>(Self->OrderTarget)->GetName()});
                    return aMyFunction::ReplaceColoredToken(MessageText, u"<ToShip>"_w, std::move(cpp_arg_2), textHighlightColorTag_4.get());
                }());
                MessageText = ([&] {
                    auto textHighlightColorTag_5 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString cpp_arg_3 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Self->OrderTarget, Automatic), pas::checked_cast<aShip::TShip*>(Self->OrderTarget)->GetFullName(u" "_wref.get())});
                    return aMyFunction::ReplaceColoredToken(MessageText, u"<ToFullShip>"_w, std::move(cpp_arg_3), textHighlightColorTag_5.get());
                }());
                if (pas::checked_cast<aShip::TShip*>(Self->OrderTarget)->CurrentPlanet != nullptr) {
                    MessageText = ([&] {
                        auto textHighlightColorTag_6 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString cpp_arg_4 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(pas::checked_cast<aShip::TShip*>(Self->OrderTarget)->CurrentPlanet, Automatic), pas::checked_cast<aShip::TShip*>(Self->OrderTarget)->CurrentPlanet->GetFullName(u" "_w)});
                        return aMyFunction::ReplaceColoredToken(MessageText, u"<ToShipInPlanet>"_w, std::move(cpp_arg_4), textHighlightColorTag_6.get());
                    }());
                }
            }
            if (Definitions[EntryIndex].ShipType != pas::constant_set<Globals::TGreetingShipCategories>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipType, Self->GetGreetingShipCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].Relations != pas::constant_set<aGalaxyStruct::TRelationLevels>({}) && static_cast<std::uint8_t>(([&] {
                aGalaxyStruct::TRelationLevel cpp_element_5 = aShip::TShip_GetRelationLevelToShip(Self, aPlayer::GetPlayer());
                const aGalaxyStruct::TRelationLevels& cpp_set_5 = Definitions[EntryIndex].Relations;
                return pas::contains(cpp_set_5, cpp_element_5);
            }()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipRace, Self->PilotRace) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerRace, aPlayer::GetPlayer()->PilotRace) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipRaceIsPlayerRace != Globals::gcAny && (Definitions[EntryIndex].ShipRaceIsPlayerRace == Globals::gcYes && !(aPlayer::GetPlayer()->PilotRace == Self->PilotRace) || Definitions[EntryIndex].ShipRaceIsPlayerRace == Globals::gcNo && aPlayer::GetPlayer()->PilotRace == Self->PilotRace)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerAttackGoodShip != Globals::gcAny) {
                if (pas::class_cast_if<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                    Other = pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget);
                    Rejected = pas::class_cast_if<TNormalShip*>(Other) != nullptr && aPlayer::GetPlayer() != Other->OrderTarget && aShip::TShip_GetRelationLevelToShip(Self, Other) == aGalaxyStruct::rlExcellent && aShip::TShip_GetRelationLevelToShip(Other, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile;
                } else {
                    Rejected = false;
                }
                if (Definitions[EntryIndex].PlayerAttackGoodShip == Globals::gcNo) {
                    if (Rejected) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].PlayerAttackGoodShip == Globals::gcYes && static_cast<std::uint8_t>(Rejected ^ 1)) {
                    continue;
                }
                if (Rejected) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<FullShipGood>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(aPlayer::GetPlayer()->OrderTarget, Automatic), pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget)->GetFullName(u" "_wref.get())}), pas::WideString());
                }
            }
            if (Definitions[EntryIndex].InFear != Globals::gcAny && (Definitions[EntryIndex].InFear == Globals::gcYes && static_cast<std::uint8_t>(Self->InFear ^ 1) || Definitions[EntryIndex].InFear == Globals::gcNo && Self->InFear)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipBadFlyToShip != Globals::gcAny) {
                Rejected = Self->IsEnemyPursuingSelf();
                if (Definitions[EntryIndex].ShipBadFlyToShip == Globals::gcYes && static_cast<std::uint8_t>(Rejected ^ 1) || Definitions[EntryIndex].ShipBadFlyToShip == Globals::gcNo && Rejected) {
                    continue;
                }
            }
            if (Definitions[EntryIndex].ShipBadType != pas::constant_set<Globals::TGreetingShipCategories>({}) && Self->EnemyShip != nullptr && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipBadType, Self->EnemyShip->GetGreetingShipCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipBadRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && Self->EnemyShip != nullptr && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipBadRace, Self->EnemyShip->PilotRace) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipFlyToPlayer != Globals::gcAny && (Definitions[EntryIndex].ShipFlyToPlayer == Globals::gcYes && !(aPlayer::GetPlayer() == Self->OrderTarget) || Definitions[EntryIndex].ShipFlyToPlayer == Globals::gcNo && aPlayer::GetPlayer() == Self->OrderTarget)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerFlyToShip != Globals::gcAny && (Definitions[EntryIndex].PlayerFlyToShip == Globals::gcYes && !(aPlayer::GetPlayer()->OrderTarget == Self) || Definitions[EntryIndex].PlayerFlyToShip == Globals::gcNo && aPlayer::GetPlayer()->OrderTarget == Self)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerIsShipBad != Globals::gcAny && (Definitions[EntryIndex].PlayerIsShipBad == Globals::gcYes && !(aPlayer::GetPlayer() == Self->EnemyShip) || Definitions[EntryIndex].PlayerIsShipBad == Globals::gcNo && aPlayer::GetPlayer() == Self->EnemyShip)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipTurnBeforeEndOrder != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                Count = std::min<std::int32_t>(10, Self->EstimateOrderTravelTurns());
                if (!pas::contains(Definitions[EntryIndex].ShipTurnBeforeEndOrder, static_cast<std::uint32_t>(Count))) {
                    continue;
                }
            }
            if (Definitions[EntryIndex].PlayerTurnBeforeEndOrder != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                Count = std::min<std::int32_t>(10, aPlayer::GetPlayer()->EstimateOrderTravelTurns());
                if (!pas::contains(Definitions[EntryIndex].PlayerTurnBeforeEndOrder, static_cast<std::uint32_t>(Count))) {
                    continue;
                }
            }
            if (Self->EnemyShip != nullptr && Self->EnemyShip->CurrentStar == Self->CurrentStar && Self->EnemyShip->InNormalSpace() && Definitions[EntryIndex].ShipBadTurnBeforeEndOrder != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                Count = std::min<std::int32_t>(10, Self->EnemyShip->EstimateOrderTravelTurns());
                if (!pas::contains(Definitions[EntryIndex].ShipBadTurnBeforeEndOrder, static_cast<std::uint32_t>(Count))) {
                    continue;
                }
            }
            if (aRanger::TRanger* ranger = pas::class_cast_if<aRanger::TRanger*>(Self); ranger != nullptr && Definitions[EntryIndex].ShipStatus != pas::constant_set<aGalaxyStruct::TRangerCareerSet>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipStatus, ranger->GetDominantCareer()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerStatus != pas::constant_set<aGalaxyStruct::TRangerCareerSet>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerStatus, aPlayer::GetPlayer()->GetDominantCareer()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipStrength != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipStrength, Self->GetRelativeStrengthCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerStrength != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerStrength, aPlayer::GetPlayer()->GetRelativeStrengthCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipStructure != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipStructure, Self->GetHullConditionCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerStructure != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerStructure, aPlayer::GetPlayer()->GetHullConditionCategory()) ^ 1)) {
                continue;
            }
            if (aRanger::TRanger* ranger_2 = pas::class_cast_if<aRanger::TRanger*>(Self); ranger_2 != nullptr && static_cast<std::uint8_t>(ranger_2->ExcludedFromRating ^ 1) && Definitions[EntryIndex].ShipRating != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipRating, Self->GetRangerRatingBand()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerRating != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerRating, aPlayer::GetPlayer()->GetRangerRatingBand()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipRank, Self->Rank) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerRank, aPlayer::GetPlayer()->Rank) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerPirateRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerPirateRank, aPlayer::GetPlayer()->PirateRank) ^ 1)) {
                continue;
            }
            if (aRanger::TRanger* ranger_3 = pas::class_cast_if<aRanger::TRanger*>(Self); ranger_3 != nullptr && static_cast<std::uint8_t>(ranger_3->ExcludedFromRating ^ 1) && Definitions[EntryIndex].RatingShipWithPlayer != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                std::uint8_t cpp_element_6 = (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipRatingComparison(Self));
                const Globals::TGreetingMask& cpp_set_6 = Definitions[EntryIndex].RatingShipWithPlayer;
                return pas::contains(cpp_set_6, cpp_element_6);
            }()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].RankShipWithPlayer != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].RankShipWithPlayer, (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipRankComparison(Self))) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].RankShipWithPlayerExtra != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].RankShipWithPlayerExtra, (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipPirateRankComparison(Self))) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].StrengthShipWithPlayer != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].StrengthShipWithPlayer, (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipStrengthComparison(Self))) ^ 1)) {
                continue;
            }
            if (Good != aGalaxyStruct::NoGreetingGoods) {
                if (Definitions[EntryIndex].ShipGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(Self->CargoGoods[Good].Count, Good)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].PlayerGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(aPlayer::GetPlayer()->CargoGoods[Good].Count, Good)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ShipHaveGoods != Globals::gcAny && (Definitions[EntryIndex].ShipHaveGoods == Globals::gcYes && Self->CargoGoods[Good].Count == 0 || Definitions[EntryIndex].ShipHaveGoods == Globals::gcNo && Self->CargoGoods[Good].Count > 0)) {
                    continue;
                }
                if (Definitions[EntryIndex].PlayerHaveGoods != Globals::gcAny && (Definitions[EntryIndex].PlayerHaveGoods == Globals::gcYes && aPlayer::GetPlayer()->CargoGoods[Good].Count == 0 || Definitions[EntryIndex].PlayerHaveGoods == Globals::gcNo && aPlayer::GetPlayer()->CargoGoods[Good].Count > 0)) {
                    continue;
                }
            }
            if (Definitions[EntryIndex].ShipGoodsTypeCnt != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipGoodsTypeCnt, Self->CountCargoGoodsTypes()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerGoodsTypeCnt != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerGoodsTypeCnt, aPlayer::GetPlayer()->CountCargoGoodsTypes()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipMayScanPlayer != Globals::gcAny && (Definitions[EntryIndex].ShipMayScanPlayer == Globals::gcYes && !(Self->CanResolveObjectWithScanner(aPlayer::GetPlayer()) && Self->GetRadarRange() > 0) || Definitions[EntryIndex].ShipMayScanPlayer == Globals::gcNo && (Self->CanResolveObjectWithScanner(aPlayer::GetPlayer()) && Self->GetRadarRange() > 0))) {
                continue;
            }
            Rejected = false;
            for (auto cpp_range_5 = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::stWarrior); cpp_range_5.next(ShipKind); ) {
                switch (ShipKind) {
                    case aGalaxyStruct::stKling: CountMask = Definitions[EntryIndex].KlingInCurStar; break;
                    case aGalaxyStruct::stRanger: CountMask = Definitions[EntryIndex].RangerInCurStar; break;
                    case aGalaxyStruct::stPirate: CountMask = Definitions[EntryIndex].PirateInCurStar; break;
                    case aGalaxyStruct::stWarrior: CountMask = Definitions[EntryIndex].WarriorInCurStar; break;
                    case aGalaxyStruct::stTransport: CountMask = Definitions[EntryIndex].TransportInCurStar; break;
                }
                if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                    Count = 0;
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_6.next(K); ) {
                        Other = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, K);
                        if (static_cast<std::uint8_t>(Other->HasScriptStateText() ^ 1) && Other->TypeNameOverrideKey == u"" && Other->TypeId == ShipKind) {
                            ++Count;
                        }
                    }
                    Count = std::min<std::int32_t>(10, Count);
                    if (!pas::contains(CountMask, static_cast<std::uint32_t>(Count))) {
                        Rejected = true;
                        break;
                    }
                }
            }
            if (Rejected) {
                continue;
            }
            if (Definitions[EntryIndex].LastPlanetRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                if (Self->LastDockedPlanet == nullptr) {
                    continue;
                }
                if (!pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Self->LastDockedPlanet->OwnerId)) {
                    continue;
                }
                if (Self->LastDockedPlanet->CurrentStar->Status.CustomFaction != u"") {
                    continue;
                }
                if (!pas::contains(Definitions[EntryIndex].LastPlanetRace, Self->LastDockedPlanet->RaceId)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetRelations != pas::constant_set<aGalaxyStruct::TRelationLevels>({}) && static_cast<std::uint8_t>(([&] {
                    aGalaxyStruct::TRelationLevel cpp_element_7 = Self->LastDockedPlanet->GetRelationLevelToShip(aPlayer::GetPlayer());
                    const aGalaxyStruct::TRelationLevels& cpp_set_7 = Definitions[EntryIndex].LastPlanetRelations;
                    return pas::contains(cpp_set_7, cpp_element_7);
                }()) ^ 1)) {
                    continue;
                }
                if (Good != aGalaxyStruct::NoGreetingGoods) {
                    if (Definitions[EntryIndex].LastPlanetGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].LastPlanetGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(Self->LastDockedPlanet->Goods[Good].Count, Good)) ^ 1)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].LastPlanetGoodsSale != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                        std::uint8_t cpp_element_8 = ([&] {
                            std::int32_t shopGoodsPurchasePrice_2 = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Self->LastDockedPlanet);
                            aGalaxy::TGalaxy* galaxy_3 = aGalaxy::Galaxy;
                            return galaxy_3->ClassifyGoodsPrice(shopGoodsPurchasePrice_2, Good);
                        }());
                        const Globals::TGreetingMask& cpp_set_8 = Definitions[EntryIndex].LastPlanetGoodsSale;
                        return pas::contains(cpp_set_8, cpp_element_8);
                    }()) ^ 1)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].LastPlanetGoodsBuy != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                        std::uint8_t cpp_element_9 = ([&] {
                            std::int32_t shopGoodsSellPrice_2 = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Self->LastDockedPlanet);
                            aGalaxy::TGalaxy* galaxy_4 = aGalaxy::Galaxy;
                            return galaxy_4->ClassifyGoodsPrice(shopGoodsSellPrice_2, Good);
                        }());
                        const Globals::TGreetingMask& cpp_set_9 = Definitions[EntryIndex].LastPlanetGoodsBuy;
                        return pas::contains(cpp_set_9, cpp_element_9);
                    }()) ^ 1)) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].LastPlanetIsHomePlanet != Globals::gcAny && (Definitions[EntryIndex].LastPlanetIsHomePlanet == Globals::gcYes && !(Self->LastDockedPlanet == Self->HomePlanet) || Definitions[EntryIndex].LastPlanetIsHomePlanet == Globals::gcNo && Self->LastDockedPlanet == Self->HomePlanet)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetRaceIsShipRace != Globals::gcAny) {
                    if (!pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Self->LastDockedPlanet->OwnerId)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].LastPlanetRaceIsShipRace == Globals::gcYes && !(Self->LastDockedPlanet->RaceId == Self->PilotRace) || Definitions[EntryIndex].LastPlanetRaceIsShipRace == Globals::gcNo && Self->LastDockedPlanet->RaceId == Self->PilotRace) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].LastPlanetRaceIsPlayerRace != Globals::gcAny) {
                    if (!pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Self->LastDockedPlanet->OwnerId)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].LastPlanetRaceIsPlayerRace == Globals::gcYes && !(aPlayer::GetPlayer()->PilotRace == Self->LastDockedPlanet->RaceId) || Definitions[EntryIndex].LastPlanetRaceIsPlayerRace == Globals::gcNo && aPlayer::GetPlayer()->PilotRace == Self->LastDockedPlanet->RaceId) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].LastPlanetEconomy != pas::constant_set<aGalaxyStruct::TPlanetEconomies>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].LastPlanetEconomy, Self->LastDockedPlanet->Economy) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetGovernment != pas::constant_set<aGalaxyStruct::TPlanetGovernments>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].LastPlanetGovernment, Self->LastDockedPlanet->Government) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetInCurStar != Globals::gcAny && (Definitions[EntryIndex].LastPlanetInCurStar == Globals::gcYes && !(Self->LastDockedPlanet->CurrentStar == Self->CurrentStar) || Definitions[EntryIndex].LastPlanetInCurStar == Globals::gcNo && Self->LastDockedPlanet->CurrentStar == Self->CurrentStar)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetDistToShipInTurn != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                    Count = std::min<std::int32_t>(10, Self->EstimateTravelTurnsToPlanet(Self->LastDockedPlanet));
                    if (Count == -1) {
                        continue;
                    }
                    if (!pas::contains(Definitions[EntryIndex].LastPlanetDistToShipInTurn, static_cast<std::uint32_t>(Count))) {
                        continue;
                    }
                }
                if (Self->LastDockedPlanet->CurrentStar != Self->CurrentStar) {
                    Rejected = false;
                    for (auto cpp_range_7 = pas::for_to<aGalaxyStruct::TShipType>(aGalaxyStruct::stKling, aGalaxyStruct::stWarrior); cpp_range_7.next(ShipKind); ) {
                        switch (ShipKind) {
                            case aGalaxyStruct::stKling: {
                                CountMask = Definitions[EntryIndex].KlingInLastPlanetStar;
                                break;
                            }
                            case aGalaxyStruct::stRanger: {
                                CountMask = Definitions[EntryIndex].RangerInLastPlanetStar;
                                break;
                            }
                            case aGalaxyStruct::stPirate: {
                                CountMask = Definitions[EntryIndex].PirateInLastPlanetStar;
                                break;
                            }
                            case aGalaxyStruct::stWarrior: {
                                CountMask = Definitions[EntryIndex].WarriorInLastPlanetStar;
                                break;
                            }
                            case aGalaxyStruct::stTransport: {
                                CountMask = Definitions[EntryIndex].TransportInLastPlanetStar;
                                break;
                            }
                        }
                        if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                            Count = 0;
                            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Self->LastDockedPlanet->CurrentStar->Ships) - 1); cpp_range_8.next(K); ) {
                                Other = pas::list_at<aShip::TShip>(Self->LastDockedPlanet->CurrentStar->Ships, K);
                                if (static_cast<std::uint8_t>(Other->HasScriptStateText() ^ 1) && Other->TypeNameOverrideKey == u"" && Other->TypeId == ShipKind) {
                                    ++Count;
                                }
                            }
                            Count = std::min<std::int32_t>(10, Count);
                            if (!pas::contains(CountMask, static_cast<std::uint32_t>(Count))) {
                                Rejected = true;
                                break;
                            }
                        }
                    }
                    if (Rejected) {
                        continue;
                    }
                }
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanet>"_w, pas::concat_wide({Self->LastDockedPlanet->Name, aGalaxy::GetLocalObjectLink(Self->LastDockedPlanet, Automatic)}), aMyFunction::TextHighlightColorTag);
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanetStar>"_w, Self->LastDockedPlanet->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
                if (Good != aGalaxyStruct::NoGreetingGoods) {
                    MessageText = ([&] {
                        auto textHighlightColorTag_7 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString intToStr_3 = pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Self->LastDockedPlanet));
                        return aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanetGoodsSale>"_w, std::move(intToStr_3), textHighlightColorTag_7.get());
                    }());
                    MessageText = ([&] {
                        auto textHighlightColorTag_8 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString intToStr_4 = pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Self->LastDockedPlanet));
                        return aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanetGoodsBuy>"_w, std::move(intToStr_4), textHighlightColorTag_8.get());
                    }());
                }
            }
            if (MessageText != u"") {
                MessageText = ([&] {
                    auto textHighlightColorTag_9 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString cpp_arg_5 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Self, Automatic), Self->GetName()});
                    return aMyFunction::ReplaceColoredToken(MessageText, u"<Ship>"_w, std::move(cpp_arg_5), textHighlightColorTag_9.get());
                }());
                MessageText = ([&] {
                    auto textHighlightColorTag_10 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString cpp_arg_6 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Self, Automatic), Self->GetFullName(u" "_wref.get())});
                    return aMyFunction::ReplaceColoredToken(MessageText, u"<FullShip>"_w, std::move(cpp_arg_6), textHighlightColorTag_10.get());
                }());
                if (Self->EnemyShip != nullptr) {
                    MessageText = ([&] {
                        auto textHighlightColorTag_11 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString cpp_arg_7 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Self->EnemyShip, Automatic), Self->EnemyShip->GetName()});
                        return aMyFunction::ReplaceColoredToken(MessageText, u"<ShipBad>"_w, std::move(cpp_arg_7), textHighlightColorTag_11.get());
                    }());
                    MessageText = ([&] {
                        auto textHighlightColorTag_12 = pas::borrow(aMyFunction::TextHighlightColorTag);
                        pas::WideString cpp_arg_8 = pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Self->EnemyShip, Automatic), Self->EnemyShip->GetFullName(u" "_wref.get())});
                        return aMyFunction::ReplaceColoredToken(MessageText, u"<FullShipBad>"_w, std::move(cpp_arg_8), textHighlightColorTag_12.get());
                    }());
                }
                MessageText = ([&] {
                    auto textHighlightColorTag_13 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString rankName = Self->GetRankName();
                    return aMyFunction::ReplaceColoredToken(MessageText, u"<ShipRank>"_w, std::move(rankName), textHighlightColorTag_13.get());
                }());
                MessageText = ([&] {
                    auto textHighlightColorTag_14 = pas::borrow(aMyFunction::TextHighlightColorTag);
                    pas::WideString rankName_2 = aPlayer::GetPlayer()->GetRankName();
                    return aMyFunction::ReplaceColoredToken(MessageText, u"<PlayerRank>"_w, std::move(rankName_2), textHighlightColorTag_14.get());
                }());
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<CurStar>"_w, Self->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
                if (Self->HomePlanet != nullptr) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<HomePlanet>"_w, pas::concat_wide({Self->HomePlanet->Name, aGalaxy::GetLocalObjectLink(Self->HomePlanet, Automatic)}), aMyFunction::TextHighlightColorTag);
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<HomePlanetStar>"_w, Self->HomePlanet->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
                }
                BestText = MessageText;
                if (CandidatePriority == -1) {
                    BestPriority = Definitions[EntryIndex].Priority;
                } else {
                    BestPriority = CandidatePriority;
                }
                if (BestPriority >= 50) {
                    break;
                }
            }
        }
        return BestText;
    }

    // Enables afterburner for multi-turn orders with a serviceable engine.
    void TNormalShip::UpdateAfterburnerState() {
        AfterburnerActive = GetSlotCount(aConst::sskAfterburner) > 0 && GetEngine() != nullptr && GetEngine()->ConditionPercent > 1.0E+1L && EstimateOrderTravelTurns() > 1;
        RefreshDerivedStats(true);
    }

    void TNormalShip::TrainSkillsAutomatically() {
        float Score{};
        float BestScore{};
        aGalaxyStruct::TPilotSkill Skill{};
        aGalaxyStruct::TPilotSkill BestSkill{};
        std::uint8_t Bonus{};
        do {
            BestScore = -1.0f;
            BestSkill = aGalaxyStruct::psAccuracy;
            for (Bonus = static_cast<std::uint8_t>(22); Bonus <= static_cast<std::uint8_t>(27); ++Bonus) {
                Skill = aConst::EquipmentBonusSkills[Bonus - 22];
                if (BaseSkills[Skill] < 6) {
                    {
                        pas::Extended cpp_left = pas::sqr(static_cast<pas::Extended>(EvaluateStatBonus(static_cast<aConst::TEquipmentBonusKind>(Bonus), 1)));
                        Score = pas::real_divide(cpp_left, aConst::SkillTrainingCosts[BaseSkills[Skill] + 1][Skill]);
                    }
                    if (Score > BestScore) {
                        BestScore = Score;
                        BestSkill = Skill;
                    }
                }
            }
            if (BestScore < 0.0L || aConst::SkillTrainingCosts[BaseSkills[BestSkill] + 1][BestSkill] > FreeExperience) {
                break;
            }
        } while (TrainSkill(BestSkill));
    }

    void TNormalShip::p_destroy() {
        aNormalShip::TNormalShip_Destroy(this);
    }

    void TNormalShip::virtual_TShip_ResolveLoadedReferences(aGalaxy::TGalaxy* Galaxy) {
        aNormalShip::TNormalShip_ResolveLoadedReferences(this, Galaxy);
    }

    void TNormalShip::virtual_TShip_NextDay() {
        aNormalShip::TNormalShip_NextDay(this);
    }

} // namespace aNormalShip
