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
                if (pas::class_cast_if<TNormalShip*>(Ship) != nullptr) {
                    Normal = pas::checked_cast<TNormalShip*>(Ship);
                    Normal->PendingLiberationCeremonyPlanet = nullptr;
                    if (Normal->CurrentSystemKills.Dominator > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfDominators || Normal->CurrentSystemKills.Pirate > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfPirates && Normal->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || aPlayer::GetPlayer() != Ship && Ship->DaysSincePlayerSeen > 1 && Normal->DominatorKillCount + Normal->PirateKillCount > Normal->LiberatedSystemCount) {
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
                        if (Normal->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                            Normal->AddRankPoints(30);
                        } else {
                            Normal->AddPirateRankPoints(16u);
                        }
                        Normal->GainExperience(aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState), 0);
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
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Star>"_w, name.get(), u"<Sector>"_w, std::move(name_2), u"<Planet>"_w, name_3.get());
                    }());
                    aGalaxy::Galaxy->AddPlanetNews(29, Text);
                } else {
                    Text = ([&] {
                        auto name_4 = pas::borrow(Star->Name);
                        pas::WideString name_5 = Star->Constellation->GetName();
                        auto name_6 = pas::borrow(CeremonyPlanet->Name);
                        pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.NormalsTakeSystemFromPirateClan"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_2), u"<color=255,240,100>"_w, u"<Star>"_w, name_4.get(), u"<Sector>"_w, std::move(name_5), u"<Planet>"_w, name_6.get());
                    }());
                    aGalaxy::Galaxy->AddPlanetNews(30, Text);
                }
                {
                    Globals::TMessagePlayer* cpp_with = Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
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
                if (pas::class_cast_if<TNormalShip*>(Ship) != nullptr) {
                    Normal = pas::checked_cast<TNormalShip*>(Ship);
                    Normal->PendingLiberationCeremonyPlanet = nullptr;
                    if (Normal->CurrentSystemKills.Dominator > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfDominators || Normal->CurrentSystemKills.Normal > 0 && Star->Status.PreviousControlFaction == aGalaxyStruct::sfCoalition && Normal->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) || aPlayer::GetPlayer() != Ship && Ship->DaysSincePlayerSeen > 1 && Normal->MilitaryKillCount + Normal->DominatorKillCount > Normal->LiberatedSystemCount) {
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
                        Normal->GainExperience(aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState), 0);
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
                    return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_3), u"<color=255,240,100>"_w, u"<Star>"_w, name_7.get(), u"<Sector>"_w, std::move(name_8), u"<Planet>"_w, name_9.get());
                }());
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    aGalaxy::Galaxy->AddPlanetNews(32, Text);
                }
            } else {
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    Text = ([&] {
                        auto name_10 = pas::borrow(Star->Name);
                        pas::WideString name_11 = Star->Constellation->GetName();
                        auto name_12 = pas::borrow(CeremonyPlanet->Name);
                        pas::WideString pickLocalizedTextVariant_4 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.PirateClanTakeSystemFromKling"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_4), u"<color=255,240,100>"_w, u"<Star>"_w, name_10.get(), u"<Sector>"_w, std::move(name_11), u"<Planet>"_w, name_12.get());
                    }());
                } else {
                    Text = ([&] {
                        auto name_13 = pas::borrow(Star->Name);
                        pas::WideString name_14 = Star->Constellation->GetName();
                        auto name_15 = pas::borrow(CeremonyPlanet->Name);
                        pas::WideString pickLocalizedTextVariant_5 = aConst::PickLocalizedTextVariant(u"GalaxyNews.Globals.PirateClanTakeSystemFromKlingAlt"_wref.get(), SourceShip->Seed * (aGalaxy::Galaxy->CurrentTurn / 10));
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_5), u"<color=255,240,100>"_w, u"<Star>"_w, name_13.get(), u"<Sector>"_w, std::move(name_14), u"<Planet>"_w, name_15.get());
                    }());
                }
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    aGalaxy::Galaxy->AddPlanetNews(31, Text);
                }
            }
            {
                Globals::TMessagePlayer* cpp_with_2 = Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
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
        std::uint8_t Career{};
        for (Career = static_cast<std::uint8_t>(0); Career <= static_cast<std::uint8_t>(2); ++Career) {
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
            OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiPirate);
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
        Rank = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"Roarnuke"_w))));
        RankPoints = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"RearnaksProcitnotas"_w))));
        PirateRank = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"PlivroaktrenRiasnuk"_w))));
        PirateRankPoints = static_cast<std::uint16_t>(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(EC_Str::DecodeTextW(u"PhilroaAtrelRoasnAkoPiopionatos"_w)))));
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
                return cpp_left < aShip::TShip_GetRadarRange(Self);
            }()) && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && static_cast<std::uint8_t>(aRanger::TRanger_ProcessPendingPlayerFollowTargeting(aPlayer::GetPlayer()) ^ 1) && Self->ScriptShip == nullptr && Self->LiberationGroup == nullptr) {
                MessageText = Self->SelectSituationalMessage(true);
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
        static const pas::Set<0, 255> RewardVictims = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::stKling, 13}});
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
        std::uint8_t ProgramIndex{};
        aItem::TMicroModule* ModuleItem{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (AwardIds == nullptr) {
            AwardWeight = 8.0E+1f;
        } else {
            AwardWeight = aMyFunction::RemapClamped(pas::list_count(AwardIds), 0.0, 15.0, 8.0E+1, 1.0E+1);
        }
        if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr && static_cast<aRanger::TRanger*>(this)->HasProgram(aGalaxyStruct::prgIntercom)) {
            ProgramWeight = aMyFunction::RemapClamped(pas::checked_cast<aRanger::TRanger*>(this)->CountProgramsInFilter(static_cast<aRanger::TRangerProgramMask>(RewardPrograms)), 0.0, 1.0E+1, 8.0E+1, 1.0E+1);
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
        if (CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && AwardWeight > 0.0L) {
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
            if (CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                if (static_cast<std::uint8_t>(CurrentPlanet->CurrentStar->Status.PreviousControlFaction) == 1) {
                    Result = aConst::LocalizedColorText(pas::concat_wide({u"PlanetCongratulations.LiberationStarNormalsFromKling.", aConst::OwnerToSys(CurrentPlanet->OwnerId), u"Text"}));
                } else {
                    Result = aConst::LocalizedColorText(pas::concat_wide({u"PlanetCongratulations.LiberationStarNormalsFromPirateClan.", aConst::OwnerToSys(CurrentPlanet->OwnerId), u"Text"}));
                }
            } else {
                if (static_cast<std::uint8_t>(CurrentPlanet->CurrentStar->Status.PreviousControlFaction) == 0) {
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
        if (CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
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
                    aMyFunction::ReplaceTextToken(Result, u"<Reward>"_w, (TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name, u"<color=255,240,100>"_w);
                } else {
                    Result = pas::WideString();
                }
                break;
            }
            case 2: {
                if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr) {
                    ProgramIndex = pas::checked_cast<aRanger::TRanger*>(this)->SelectRandomProgramIdFromFilter(static_cast<aRanger::TRangerProgramMask>(RewardPrograms));
                    Quantity = aMyFunction::SeededRandomIntRange(1, System::Round(aMyFunction::RemapClamped(pas::checked_cast<aRanger::TRanger*>(this)->CountProgramsInFilter(static_cast<aRanger::TRangerProgramMask>(RewardPrograms)), 2.0, 1.0E+1, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].MaximumQuestProgramRewardCount, 1.0)), ProgramIndex + CurrentStar->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 25));
                    pas::checked_cast<aRanger::TRanger*>(this)->ProgramCounts[ProgramIndex] += Quantity;
                    if (aPlayer::GetPlayer() == this) {
                        Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddProgramms"}))});
                        aMyFunction::ReplaceTextToken(Result, u"<Programm>"_w, (static_cast<void>(pas::checked_cast<aRanger::TRanger*>(this)), aRanger::TRanger::GetProgramName(ProgramIndex)), u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Result, u"<Count>"_w, pas::wide_int_to_str(Quantity), u"<color=255,240,100>"_w);
                    } else {
                        Result = pas::WideString();
                    }
                }
                break;
            }
            case 3: {
                RewardItem = aItem::CreateRandomLootItem(aItem::ilpReward, CurrentPlanet->OwnerId, (static_cast<std::int32_t>(CurrentPlanet->GenerationSeed) + aGalaxy::Galaxy->CurrentTurn) / 50 + 123424767);
                if (pas::class_cast_if<aItem::TArtefactTranclucator*>(RewardItem) != nullptr) {
                    static_cast<aTranclucator::TTranclucator*>(reinterpret_cast<aItem::TArtefactTranclucator*>(RewardItem)->Ship)->OwnerShip = this;
                }
                if (pas::class_cast_if<aItem::TArtefact*>(RewardItem) != nullptr) {
                    pas::list_add(Artefacts, reinterpret_cast<void*>(RewardItem));
                } else {
                    pas::list_add(Inventory, reinterpret_cast<void*>(RewardItem));
                }
                if (aPlayer::GetPlayer() == this) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnGovItemReward, RewardItem, nullptr, 0);
                    Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddArtefact"})), u"\r\n", RewardItem->GetDescriptionText()});
                    aMyFunction::ReplaceTextToken(Result, u"<Artefact>"_w, RewardItem->GetDisplayName(), u"<color=255,240,100>"_w);
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
                    aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnGovItemReward, ModuleItem, nullptr, 0);
                    Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerReceivesMMAsReward"_w, nullptr);
                    Event->AddData(ModuleItem->Id);
                    Event->AddData(ModuleItem->MicroModuleIndex - 1);
                }
                pas::list_add(Inventory, reinterpret_cast<void*>(ModuleItem));
                if (aPlayer::GetPlayer() == this) {
                    Result = pas::concat_wide({Result, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddNod"})), u"\r\n", ModuleItem->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, nullptr)});
                    aMyFunction::ReplaceTextToken(Result, u"<Nod>"_w, aConst::MicroModuleTemplates[ModuleIndex].Name, u"<color=255,240,100>"_w);
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
        GainExperience(I, 0);
        if (aPlayer::GetPlayer() == this) {
            Result = pas::concat_wide({Result, u"\r\n", u" ", u"\r\n", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(pas::concat_wide({Prefix, u"AddPoints"})), u"<color=45,105,45>"_w)});
            aMyFunction::ReplaceTextToken(Result, u"<Points>"_w, pas::wide_int_to_str(I), pas::WideString());
        } else {
            Result = pas::WideString();
        }
        if (aPlayer::GetPlayer() == this) {
            aMyFunction::ReplaceTextToken(Result, u"<Star>"_w, CurrentPlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Result, u"<Planet>"_w, CurrentPlanet->Name, u"<color=255,240,100>"_w);
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
            Award = SelectAward(aConst::RaceToOwner(CurrentPlanet->RaceId), pas::constant_set<TAwardTypeMask>({{aGalaxyStruct::atLiberation}, {aGalaxyStruct::atAccomplishment}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
        } else if (DockedTo != nullptr) {
            Award = SelectAward(aConst::RaceToOwner(DockedTo->PilotRace), pas::constant_set<TAwardTypeMask>({{aGalaxyStruct::atLiberation}, {aGalaxyStruct::atAccomplishment}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
        } else {
            Award = SelectAward(aGalaxyStruct::oiHuman, pas::constant_set<TAwardTypeMask>({{aGalaxyStruct::atLiberation}, {aGalaxyStruct::atAccomplishment}}), pas::constant_set<aGalaxyStruct::TShipTypeMask>({{aGalaxyStruct::stKling, 13}}));
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
                    if (Victim->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
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
                    if (pas::checked_cast<aRanger::TRanger*>(Victim)->GetDominantCareer() != aGalaxyStruct::rcPirate && Victim->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && static_cast<std::uint8_t>(pas::checked_cast<aRanger::TRanger*>(Victim)->ExcludedFromRating ^ 1)) {
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
        std::uint8_t SourceKind = 3;
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
            if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
                Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Victim)->KlingType));
            } else if (pas::class_cast_if<aTransport::TTransport*>(Victim) != nullptr) {
                Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aTransport::TTransport*>(Victim)->TransportType));
            } else if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
                Event->AddData(pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType);
            } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
                Event->AddData(pas::checked_cast<aPirate::TPirate*>(Victim)->PirateType);
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
            if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
                Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Victim)->KlingType));
            } else if (pas::class_cast_if<aTransport::TTransport*>(Victim) != nullptr) {
                Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aTransport::TTransport*>(Victim)->TransportType));
            } else if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
                Event->AddData(pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType);
            } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
                Event->AddData(pas::checked_cast<aPirate::TPirate*>(Victim)->PirateType);
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
            SourceKind = 0;
            if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 8;
                }
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::checked_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(ActivityAmount);
                }
            }
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace()) {
                pas::checked_cast<TNormalShip*>(Self->PartnerShip)->AddRankPoints(6);
            }
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PirateReward = 8;
            }
        } else if (Victim->TypeId == aGalaxyStruct::stTransport && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
            if (!QuestTargetKill) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            }
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            {
                pas::Extended cpp_left = aMyFunction::NextRandomIntRange(100, 250, aGalaxy::Galaxy->RandomState);
                Experience = System::Round(cpp_left * (static_cast<std::int8_t>(reinterpret_cast<TNormalShip*>(Victim)->Rank + static_cast<std::uint8_t>(0)) * 0.1L + 1.0L));
            }
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PirateReward = 8;
                Experience = System::Round(Experience * 1.5L);
            }
            if (aPlayer::GetPlayer() == Self) {
                ActivityAmount = 4;
            } else {
                ActivityAmount = 1;
            }
            pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
        } else if (pas::class_cast_if<aRanger::TRanger*>(Victim) != nullptr && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
            if (pas::checked_cast<aRanger::TRanger*>(Victim)->GetDominantCareer() == aGalaxyStruct::rcPirate) {
                SourceKind = 2;
                RankReward = 10;
                pas::checked_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(4);
                {
                    pas::Extended cpp_left_2 = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
                    Experience = System::Round(cpp_left_2 * (static_cast<std::int8_t>(reinterpret_cast<TNormalShip*>(Victim)->PirateRank + static_cast<std::uint8_t>(0)) * 0.1L + 1.0L));
                }
            } else {
                if (!QuestTargetKill) {
                    aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
                    if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                        ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
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
                    Experience = System::Round(cpp_left_3 * (static_cast<std::int8_t>(reinterpret_cast<TNormalShip*>(Victim)->Rank + static_cast<std::uint8_t>(0)) * 0.1L + 1.0L));
                }
                if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    PirateReward = 24;
                    Experience = System::Round(Experience * 1.5L);
                }
            }
        } else if (pas::class_cast_if<aRanger::TRanger*>(Victim) != nullptr && pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr) {
            if (reinterpret_cast<aRanger::TRanger*>(Victim)->GetDominantCareer() != aGalaxyStruct::rcPirate) {
                ++Self->CurrentSystemKills.Normal;
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                    ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
                }
                PirateReward = 24;
            }
        } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
            SourceKind = 2;
            if (Victim->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && static_cast<std::uint8_t>(QuestTargetKill ^ 1)) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Pirate));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate == 0) {
                    ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate;
                }
            }
            ++Self->PirateKillCount;
            if (aPlayer::GetPlayer() == Self) {
                Achievements::TryAddAchievementProgress(u"SHIELD"_w, 1);
            }
            {
                pas::Extended cpp_left_4 = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
                Experience = System::Round(cpp_left_4 * (static_cast<std::int8_t>(reinterpret_cast<TNormalShip*>(Victim)->PirateRank + static_cast<std::uint8_t>(0)) * 0.1L + 1.0L));
            }
            RankReward = 10;
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                Experience = Experience / 2;
            }
            if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                pas::checked_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(4);
            }
        } else if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
            ++Self->DominatorKillCount;
            if (aPlayer::GetPlayer() == Self) {
                ++aPlayer::GetPlayer()->DominatorKillsByType[pas::checked_cast<aKling::TKling*>(Victim)->KlingType];
            }
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Dominator));
            RankReward = aConst::DominatorShipDefinitions[pas::checked_cast<aKling::TKling*>(Victim)->KlingType].RankPoints;
            Experience = System::Round(([&] {
                pas::Extended cpp_left_5 = aConst::DominatorShipDefinitions[pas::checked_cast<aKling::TKling*>(Victim)->KlingType].KillExperience;
                return cpp_left_5 * aGalaxy::Galaxy->GetDominatorKillExperienceScale();
            }()));
            SourceKind = 1;
            if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 8;
                }
                if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                    pas::checked_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(ActivityAmount);
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
                    TNormalShip* cpp_arg_2 = pas::checked_cast<TNormalShip*>(Self->PartnerShip);
                    cpp_arg_2->AddRankPoints(cpp_arg);
                }
                if (pas::checked_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Dominator == 0) {
                    ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Dominator;
                }
            }
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PirateReward = aConst::DominatorShipDefinitions[pas::checked_cast<aKling::TKling*>(Victim)->KlingType].PirateRankPoints;
            }
        } else if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                {
                    pas::Extended cpp_left_6 = aMyFunction::NextRandomIntRange(250, 500, aGalaxy::Galaxy->RandomState);
                    Experience = System::Round(cpp_left_6 * (static_cast<std::int8_t>(reinterpret_cast<TNormalShip*>(Victim)->Rank + static_cast<std::uint8_t>(0)) * 0.1L + 1.0L));
                }
                if (pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType == aWarrior::wtFlagship) {
                    Experience *= 2;
                    PirateReward = 60;
                } else {
                    PirateReward = 16;
                }
            }
            if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 8;
                } else {
                    ActivityAmount = 2;
                }
                pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
            }
        } else if (Victim->TypeId == aGalaxyStruct::stTransport) {
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PirateReward = 8;
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Victim->CurrentStanding == aGalaxyStruct::ssCoalitionMilitary) {
            aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PirateReward = 32;
            }
            if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 8;
                } else {
                    ActivityAmount = 2;
                }
                pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && Victim->CurrentStanding == aGalaxyStruct::ssCoalitionActive) {
            ++Self->CurrentSystemKills.Normal;
            if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
            }
            if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                PirateReward = 24;
            }
            if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::in_range(Victim->CurrentStanding, aGalaxyStruct::ssCoalitionPassive, aGalaxyStruct::ssPiratePassive)) {
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Normal));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal == 0) {
                    ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Normal;
                }
            }
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Pirate));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate == 0) {
                    ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate;
                }
            }
            if (Victim->TypeId != static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase) && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                pas::checked_cast<aRanger::TRanger*>(Self)->AddPirateCareerActivity(ActivityAmount);
            }
            if (Victim->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase) && pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                pas::checked_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(ActivityAmount);
            }
        } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::in_range(Victim->CurrentStanding, aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary)) {
            if (Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&Self->CurrentSystemKills.Pirate));
                if (Self->PartnerShip != nullptr && pas::class_cast_if<TNormalShip*>(Self->PartnerShip) != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace() && static_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate == 0) {
                    ++reinterpret_cast<TNormalShip*>(Self->PartnerShip)->CurrentSystemKills.Pirate;
                }
            }
            if (pas::class_cast_if<aRanger::TRanger*>(Self) != nullptr) {
                if (aPlayer::GetPlayer() == Self) {
                    ActivityAmount = 4;
                } else {
                    ActivityAmount = 1;
                }
                pas::checked_cast<aRanger::TRanger*>(Self)->AddWarriorCareerActivity(ActivityAmount);
            }
        }
        if (Victim->CurrentStanding != aGalaxyStruct::ssCustom) {
            if (aPlayer::GetPlayer() == Self && aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
                if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
                    if (pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType == aWarrior::wtFlagship) {
                        aPlayer::GetPlayer()->PirateLicenseCash += System::Round(pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 2.0E+3L));
                    } else {
                        aPlayer::GetPlayer()->PirateLicenseCash += System::Round(pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 6.0E+3L));
                    }
                }
                if (Victim->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase)) {
                    aPlayer::GetPlayer()->PirateLicenseCash += System::Round(pas::real_divide(aGalaxy::Galaxy->AverageRangerCapital, 2.0E+3L));
                }
            }
            if (aPlayer::GetPlayer() == Self && Self->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfCoalition) {
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckHaterAchievement();
            }
            RecordShipKillCategory(Self, Victim);
        }
        if (pas::class_cast_if<aPirate::TPirate*>(Self) != nullptr) {
            pas::checked_cast<aPirate::TPirate*>(Self)->RaidPressure = 0.0f;
        }
        if (RankReward > 0 || Experience > 0 || PirateReward > 0) {
            if (RankReward > 0) {
                if (Self->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    Self->AddRankPoints(RankReward);
                }
                RankReward = RankReward / 2 + 1;
            }
            if (Experience > 0) {
                Self->GainExperience(Experience, SourceKind);
                if (Self->PartnerShip != nullptr && Self->PartnerShip->CurrentStar == Self->CurrentStar && Self->PartnerShip->InNormalSpace()) {
                    SharedExperience = System::Round(Experience * aConst::LeadershipExperiencePercent[Self->PartnerShip->GetEffectiveSkillLevel(aShip::psLeadership, false) & 0x0000007f] * 0.01L);
                    if (aPlayer::GetPlayer() == Self->PartnerShip) {
                        Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerGotExpFromPartner"_w, nullptr);
                        Event->AddData(Self->Id);
                        Event->AddData(Self->PartnerShip->GetEffectiveSkillLevel(aShip::psLeadership, false));
                        if (SourceKind == 1) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByDominators;
                        } else if (SourceKind == 2) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByPirates;
                        } else if (SourceKind == 3) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByNormals;
                        } else {
                            ExperienceDelta = 0;
                        }
                        aPlayer::GetPlayer()->GainExperience(SharedExperience, SourceKind);
                        if (SourceKind == 1) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByDominators - ExperienceDelta;
                        } else if (SourceKind == 2) {
                            ExperienceDelta = aPlayer::GetPlayer()->ExperienceByPirates - ExperienceDelta;
                        } else if (SourceKind == 3) {
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
                if (Self->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    Self->AddPirateRankPoints(PirateReward);
                }
                PirateReward = PirateReward / 2 + 1;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Self->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                OtherShip = pas::list_at<aShip::TShip>(Self->CurrentStar->Ships, I);
                if (OtherShip == Self || static_cast<std::uint8_t>(OtherShip->InNormalSpace() ^ 1) || static_cast<std::uint8_t>(OtherShip->IsAttackingShip(Victim) ^ 1)) {
                    continue;
                }
                if (pas::class_cast_if<aTranclucator::TTranclucator*>(OtherShip) != nullptr && aPlayer::GetPlayer() == reinterpret_cast<aTranclucator::TTranclucator*>(OtherShip)->OwnerShip) {
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
                    if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
                        Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Victim)->KlingType));
                    } else if (pas::class_cast_if<aTransport::TTransport*>(Victim) != nullptr) {
                        Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aTransport::TTransport*>(Victim)->TransportType));
                    } else if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
                        Event->AddData(pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType);
                    } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
                        Event->AddData(pas::checked_cast<aPirate::TPirate*>(Victim)->PirateType);
                    } else {
                        Event->AddData(0);
                    }
                }
                if (!(pas::class_cast_if<TNormalShip*>(OtherShip) != nullptr)) {
                    continue;
                }
                OtherNormal = reinterpret_cast<TNormalShip*>(OtherShip);
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
                    if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
                        Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Victim)->KlingType));
                    } else if (pas::class_cast_if<aTransport::TTransport*>(Victim) != nullptr) {
                        Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aTransport::TTransport*>(Victim)->TransportType));
                    } else if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
                        Event->AddData(pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType);
                    } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
                        Event->AddData(pas::checked_cast<aPirate::TPirate*>(Victim)->PirateType);
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
                    if (pas::class_cast_if<aKling::TKling*>(Victim) != nullptr) {
                        Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aKling::TKling*>(Victim)->KlingType));
                    } else if (pas::class_cast_if<aTransport::TTransport*>(Victim) != nullptr) {
                        Event->AddData(static_cast<std::uint8_t>(pas::checked_cast<aTransport::TTransport*>(Victim)->TransportType));
                    } else if (pas::class_cast_if<aWarrior::TWarrior*>(Victim) != nullptr) {
                        Event->AddData(pas::checked_cast<aWarrior::TWarrior*>(Victim)->WarriorType);
                    } else if (pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
                        Event->AddData(pas::checked_cast<aPirate::TPirate*>(Victim)->PirateType);
                    } else {
                        Event->AddData(0);
                    }
                }
                if (pas::class_cast_if<aPirate::TPirate*>(OtherNormal) != nullptr) {
                    pas::checked_cast<aPirate::TPirate*>(OtherNormal)->RaidPressure = 0.0f;
                }
                if (RankReward > 0 && OtherNormal->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                    OtherNormal->AddRankPoints(RankReward);
                }
                if (Experience > 0) {
                    OtherNormal->GainExperience(Experience, SourceKind);
                }
                if (PirateReward > 0 && OtherNormal->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
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
                    if (Victim->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate) && static_cast<std::uint8_t>(QuestTargetKill ^ 1)) {
                        aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&OtherNormal->CurrentSystemKills.Pirate));
                    }
                    ++OtherNormal->PirateKillCount;
                    if (aPlayer::GetPlayer() == OtherShip && pas::class_cast_if<aPirate::TPirate*>(Victim) != nullptr) {
                        Achievements::TryAddAchievementProgress(u"SHIELD"_w, 1);
                    }
                } else if (pas::class_cast_if<TNormalShip*>(Victim) != nullptr && pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), Victim->OwnerId)) {
                    if (!QuestTargetKill) {
                        aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(&OtherNormal->CurrentSystemKills.Normal));
                    }
                } else if (pas::in_range(Victim->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation)) && pas::contains(pas::load_unaligned<aShip::TStationStandingMask>(&aConst::FactionStandingMasks[Self->CurrentStar->Status.ControlFaction]), Victim->CurrentStanding)) {
                    aConst::IncrementWordSaturating(pas::Var<std::uint16_t>(pas::byte_offset(&OtherNormal->CurrentSystemKills, Self->CurrentStar->Status.ControlFaction * sizeof(std::uint16_t))));
                }
                RecordShipKillCategory(OtherNormal, Victim);
            }
        }
    }

    void TNormalShip::CheckKillCountAwards(aShip::TShip* Victim) {
        // Caller-popped static link; ship at ParentFrame-4.
        auto Check = [&](std::int32_t InitialThreshold, std::int32_t Multiplier, std::uint16_t Count, std::uint8_t VictimType) -> void {
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
                            aMyFunction::ReplaceTextToken(Text, u"<Reward>"_w, (TNormalShip::GetAwardInfo(Award, cpp_result), cpp_result).Name, u"<color=255,240,100>"_w);
                            Globals::AddOrUpdatePlayerBubble(0, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
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
            if (pas::class_cast_if<aRanger::TRanger*>(Ship) != nullptr && Ship != Self && Ship->InNormalSpace()) {
                Ranger = pas::checked_cast<aRanger::TRanger*>(Ship);
                if (pas::class_cast_if<aShip::TShip*>(Ranger->OrderTarget) != nullptr && Ranger->OrderTarget == Ranger->EnemyShip) {
                    Target = pas::checked_cast<aShip::TShip*>(Ranger->OrderTarget);
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
    std::uint8_t TNormalShip::SelectAward(std::uint8_t Owner, TAwardTypeMask Kinds, aGalaxyStruct::TShipTypeMask VictimTypes) {
        std::uint8_t Result{};
        std::int32_t I{};
        pas::WideString KillName{};
        pas::List* Candidates = pas::make_object<pas::List>();
        std::int32_t Count = SysUtils::StrToInt(static_cast<pas::AnsiString>(GR_Main::LookupLocalizedTextByKey(u"Reward.Count"_wref.get()))) - 1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count); cpp_range.next(I); ) {
            if (aConst::MatchesOwnerName(Owner, GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Race"})))) && pas::contains(Kinds, aConst::SysToReward(GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Type"}))))) && ([&] {
                const pas::WideString& lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Status"})));
                std::uint8_t dominantCareer = static_cast<std::uint8_t>(GetDominantCareer());
                return aConst::MatchesCareerName(dominantCareer, lookupLocalizedTextByKey);
            }())) {
                KillName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Reward.", SysUtils::IntToStr(I), ".Kill"})));
                if (KillName.length() == 0 || pas::contains(VictimTypes, aConst::SysToShipType(KillName))) {
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
                return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Rank>"_w, std::move(nextRankName));
            }());
            std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
            Globals::AddOrUpdatePlayerBubble(0, currentTurn, formatText1, u""_wref.get());
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
    pas::WideString TNormalShip::SelectSituationalMessage(std::uint8_t Automatic) {
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
        std::uint8_t ShipKind{};
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
                OtherIndex = aMyFunction::SeededRandomIntRange(0, LastIndex, this->Seed + 7 * I);
                SwapA = Definitions[OtherIndex];
                SwapB = Definitions[I];
                Definitions[I] = SwapA;
                Definitions[OtherIndex] = SwapB;
            }
        };
        if (aPlayer::GetPlayer() == PartnerShip || aPlayer::GetPlayer()->ChameleonActive || HasIndependentScriptFaction()) {
            return pas::WideString();
        }
        ItemTypes = pas::WideString();
        std::int32_t BestPriority = -1;
        std::int32_t CandidatePriority = -1;
        std::int32_t Minimum = 0;
        LastIndex = Globals::ShipGreetingCount - 1;
        ShuffleDefinitions();
        EntryIndex = aMyFunction::SeededRandomIntRange(0, LastIndex, static_cast<std::int32_t>(Seed * static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn)) / 20);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, LastIndex); cpp_range.next(I); ) {
            MessageText = pas::WideString();
            aMyFunction::IncrementWrapped(EntryIndex, Minimum, LastIndex);
            if (IsFemaleHumanPilot() != (Definitions[EntryIndex].Female == 0)) {
                continue;
            }
            if (Definitions[EntryIndex].CoalitionAlreadyDefeated != 2 && (Definitions[EntryIndex].CoalitionAlreadyDefeated == 0 && !(aGalaxy::Galaxy->CoalitionDefeatedTurn != 0) || Definitions[EntryIndex].CoalitionAlreadyDefeated == 1 && aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                continue;
            }
            if (Definitions[EntryIndex].DominatorsAlreadyDefeated != 2 && (Definitions[EntryIndex].DominatorsAlreadyDefeated == 0 && static_cast<std::uint8_t>(static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) ^ 1) ^ 1) || Definitions[EntryIndex].DominatorsAlreadyDefeated == 1 && static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) ^ 1))) {
                continue;
            }
            if (BestPriority > 0) {
                CandidatePriority = Definitions[EntryIndex].Priority;
                {
                    std::int32_t cpp_left = CandidatePriority * aMyFunction::SeededRandomIntRange(1, 100, Seed + EntryIndex * (aGalaxy::Galaxy->CurrentTurn / 20));
                    if (cpp_left < BestPriority * aMyFunction::SeededRandomIntRange(1, 100, Seed + EntryIndex * (aGalaxy::Galaxy->CurrentTurn / 20) * 3)) {
                        continue;
                    }
                }
            }
            Good = 50;
            if (Definitions[EntryIndex].Goods != 42) {
                Good = Definitions[EntryIndex].Goods;
            }
            if (Definitions[EntryIndex].AutoTalk != 2 && (Automatic && Definitions[EntryIndex].AutoTalk == 1 || static_cast<std::uint8_t>(Automatic ^ 1) && Definitions[EntryIndex].AutoTalk == 0)) {
                continue;
            }
            if (Definitions[EntryIndex].FlyType == 0) {
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
            } else if (Definitions[EntryIndex].FlyType == 1) {
                if (!(pas::class_cast_if<aPlanet::TPlanet*>(OrderTarget) != nullptr)) {
                    continue;
                }
                Planet = pas::checked_cast<aPlanet::TPlanet*>(OrderTarget);
                if (!pas::in_set<0, 4, 7, 7>(Planet->OwnerId)) {
                    continue;
                }
                if (CurrentStar->Status.CustomFaction != u"") {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToPlanetRace, Planet->RaceId) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRelations != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                    std::uint8_t cpp_element = static_cast<std::uint8_t>(Planet->GetRelationLevelToShip(aPlayer::GetPlayer()));
                    const Globals::TGreetingMask& cpp_set = Definitions[EntryIndex].ToPlanetRelations;
                    return pas::contains(cpp_set, cpp_element);
                }()) ^ 1)) {
                    continue;
                }
                if (Good != 50) {
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
                if (Definitions[EntryIndex].ToPlanetIsHomePlanet != 2 && (Definitions[EntryIndex].ToPlanetIsHomePlanet == 0 && !(HomePlanet == Planet) || Definitions[EntryIndex].ToPlanetIsHomePlanet == 1 && HomePlanet == Planet)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRaceIsShipRace != 2 && (Definitions[EntryIndex].ToPlanetRaceIsShipRace == 0 && !(Planet->RaceId == PilotRace) || Definitions[EntryIndex].ToPlanetRaceIsShipRace == 1 && Planet->RaceId == PilotRace)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRaceIsPlayerRace != 2 && (Definitions[EntryIndex].ToPlanetRaceIsPlayerRace == 0 && !(aPlayer::GetPlayer()->PilotRace == Planet->RaceId) || Definitions[EntryIndex].ToPlanetRaceIsPlayerRace == 1 && aPlayer::GetPlayer()->PilotRace == Planet->RaceId)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetEconomy != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToPlanetEconomy, static_cast<std::uint8_t>(Planet->Economy)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetGovernment != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToPlanetGovernment, static_cast<std::uint8_t>(Planet->Government)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetIsLastPlanet != 2 && (Definitions[EntryIndex].ToPlanetIsLastPlanet == 0 && !(LastDockedPlanet == Planet) || Definitions[EntryIndex].ToPlanetIsLastPlanet == 1 && LastDockedPlanet == Planet)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToPlanetRaceIsLastPlanetRace != 2) {
                    if (!pas::in_set<0, 4, 7, 7>(LastDockedPlanet->OwnerId)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToPlanetRaceIsLastPlanetRace == 0 && !(Planet->RaceId == LastDockedPlanet->RaceId) || Definitions[EntryIndex].ToPlanetRaceIsLastPlanetRace == 1 && Planet->RaceId == LastDockedPlanet->RaceId) {
                        continue;
                    }
                }
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToPlanet>"_w, pas::concat_wide({Planet->Name, aGalaxy::GetLocalObjectLink(Planet, Automatic)}), u"<color=255,240,100>"_w);
                if (Good != 50) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToPlanetGoodsSale>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet)), u"<color=255,240,100>"_w);
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToPlanetGoodsBuy>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet)), u"<color=255,240,100>"_w);
                }
            } else if (Definitions[EntryIndex].FlyType == 2) {
                if (!(pas::class_cast_if<aGalaxy::TStar*>(OrderTarget) != nullptr)) {
                    continue;
                }
                if (Definitions[EntryIndex].HomePlanetInToStar != 2 && (Definitions[EntryIndex].HomePlanetInToStar == 0 && !(HomePlanet->CurrentStar == OrderTarget) || Definitions[EntryIndex].HomePlanetInToStar == 1 && HomePlanet->CurrentStar == OrderTarget)) {
                    continue;
                }
                if (Definitions[EntryIndex].HomePlanetInCurStar != 2 && (Definitions[EntryIndex].HomePlanetInCurStar == 0 && !(HomePlanet->CurrentStar == CurrentStar) || Definitions[EntryIndex].HomePlanetInCurStar == 1 && HomePlanet->CurrentStar == CurrentStar)) {
                    continue;
                }
                Rejected = false;
                for (ShipKind = static_cast<std::uint8_t>(0); ShipKind <= static_cast<std::uint8_t>(4); ++ShipKind) {
                    switch (ShipKind) {
                        case 0: CountMask = Definitions[EntryIndex].KlingInToStar; break;
                        case 1: CountMask = Definitions[EntryIndex].RangerInToStar; break;
                        case 3: CountMask = Definitions[EntryIndex].PirateInToStar; break;
                        case 4: CountMask = Definitions[EntryIndex].WarriorInToStar; break;
                        case 2: CountMask = Definitions[EntryIndex].TransportInToStar; break;
                    }
                    if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                        Count = 0;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Ships) - 1); cpp_range_2.next(K); ) {
                            Other = pas::list_at<aShip::TShip>(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Ships, K);
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
                if (Definitions[EntryIndex].ToStarControlByKling != 2) {
                    if (pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.CustomFaction != u"") {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToStarControlByKling == 0 && !(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfDominators) || Definitions[EntryIndex].ToStarControlByKling == 1 && pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].ToStarControlByPirates != 2) {
                    if (pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.CustomFaction != u"") {
                        continue;
                    }
                    if (Definitions[EntryIndex].ToStarControlByPirates == 0 && !(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfPirates) || Definitions[EntryIndex].ToStarControlByPirates == 1 && pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].ToStarInBattle != 2 && (Definitions[EntryIndex].ToStarInBattle == 0 && !(pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.Battle != 0) || Definitions[EntryIndex].ToStarInBattle == 1 && pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Status.Battle != 0)) {
                    continue;
                }
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToStar>"_w, pas::checked_cast<aGalaxy::TStar*>(OrderTarget)->Name, u"<color=255,240,100>"_w);
            } else if (Definitions[EntryIndex].FlyType == 3) {
                if (Order != aShip::soMove || static_cast<std::uint8_t>(OrderAbsolute ^ 1)) {
                    continue;
                }
                Rejected = false;
                Item = nullptr;
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Items) - 1); cpp_range_3.next(J); ) {
                    Item = pas::list_at<aItem::TItem>(CurrentStar->Items, J);
                    // Native accepts either matching coordinate, rather than requiring both.
                    if (GetPickupApproachPosition(Item->Position).X == OrderDestination.X || GetPickupApproachPosition(Item->Position).Y == OrderDestination.Y) {
                        ItemTypes = Definitions[EntryIndex].ItemType;
                        if (ItemTypes == u"" || ItemTypes == u"Any" || EC_Str::FindTextPosW(Item->GetCategoryConfigName(), ItemTypes) != 0) {
                            if (Definitions[EntryIndex].ShipNeedInItem != 2 && (Definitions[EntryIndex].ShipNeedInItem == 0 && static_cast<std::uint8_t>(ShouldPickUpItem(Item) ^ 1) || Definitions[EntryIndex].ShipNeedInItem == 1 && ShouldPickUpItem(Item))) {
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
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<Item>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(Item, Automatic), Item->GetDisplayName()}), u"<color=255,240,100>"_w);
            } else if (Definitions[EntryIndex].FlyType == 4) {
                if (!(pas::class_cast_if<aShip::TShip*>(OrderTarget) != nullptr)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipType != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToShipType, pas::checked_cast<aShip::TShip*>(OrderTarget)->GetGreetingShipCategory()) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ToShipRace, pas::checked_cast<aShip::TShip*>(OrderTarget)->PilotRace) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipInPlanet != 2 && (Definitions[EntryIndex].ToShipInPlanet == 0 && !(pas::checked_cast<aShip::TShip*>(OrderTarget)->CurrentPlanet != nullptr) || Definitions[EntryIndex].ToShipInPlanet == 1 && pas::checked_cast<aShip::TShip*>(OrderTarget)->CurrentPlanet != nullptr)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipBad != 2 && (Definitions[EntryIndex].ToShipBad == 0 && !(pas::checked_cast<aShip::TShip*>(OrderTarget)->EnemyShip == this) || Definitions[EntryIndex].ToShipBad == 1 && pas::checked_cast<aShip::TShip*>(OrderTarget)->EnemyShip == this)) {
                    continue;
                }
                if (Definitions[EntryIndex].ToShipRelations != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                    std::uint8_t cpp_element_4 = static_cast<std::uint8_t>(aShip::TShip_GetRelationLevelToShip(this, pas::checked_cast<aShip::TShip*>(OrderTarget)));
                    const Globals::TGreetingMask& cpp_set_4 = Definitions[EntryIndex].ToShipRelations;
                    return pas::contains(cpp_set_4, cpp_element_4);
                }()) ^ 1)) {
                    continue;
                }
                MessageText = aConst::LocalizedColorText(pas::concat_wide({u"ShipGreetings.", Definitions[EntryIndex].Name, u".Text"}));
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToShip>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(OrderTarget, Automatic), pas::checked_cast<aShip::TShip*>(OrderTarget)->GetName()}), u"<color=255,240,100>"_w);
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToFullShip>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(OrderTarget, Automatic), pas::checked_cast<aShip::TShip*>(OrderTarget)->GetFullName(u" "_wref.get())}), u"<color=255,240,100>"_w);
                if (pas::checked_cast<aShip::TShip*>(OrderTarget)->CurrentPlanet != nullptr) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ToShipInPlanet>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(pas::checked_cast<aShip::TShip*>(OrderTarget)->CurrentPlanet, Automatic), pas::checked_cast<aShip::TShip*>(OrderTarget)->CurrentPlanet->GetFullName(u" "_w)}), u"<color=255,240,100>"_w);
                }
            }
            if (Definitions[EntryIndex].ShipType != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipType, GetGreetingShipCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].Relations != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                std::uint8_t cpp_element_5 = static_cast<std::uint8_t>(aShip::TShip_GetRelationLevelToShip(this, aPlayer::GetPlayer()));
                const Globals::TGreetingMask& cpp_set_5 = Definitions[EntryIndex].Relations;
                return pas::contains(cpp_set_5, cpp_element_5);
            }()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipRace, PilotRace) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerRace, aPlayer::GetPlayer()->PilotRace) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipRaceIsPlayerRace != 2 && (Definitions[EntryIndex].ShipRaceIsPlayerRace == 0 && !(aPlayer::GetPlayer()->PilotRace == PilotRace) || Definitions[EntryIndex].ShipRaceIsPlayerRace == 1 && aPlayer::GetPlayer()->PilotRace == PilotRace)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerAttackGoodShip != 2) {
                if (pas::class_cast_if<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                    Other = pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget);
                    Rejected = pas::class_cast_if<TNormalShip*>(Other) != nullptr && aPlayer::GetPlayer() != Other->OrderTarget && aShip::TShip_GetRelationLevelToShip(this, Other) == aGalaxyStruct::rlExcellent && aShip::TShip_GetRelationLevelToShip(Other, aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile;
                } else {
                    Rejected = false;
                }
                if (Definitions[EntryIndex].PlayerAttackGoodShip == 1) {
                    if (Rejected) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].PlayerAttackGoodShip == 0 && static_cast<std::uint8_t>(Rejected ^ 1)) {
                    continue;
                }
                if (Rejected) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<FullShipGood>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(aPlayer::GetPlayer()->OrderTarget, Automatic), pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget)->GetFullName(u" "_wref.get())}), pas::WideString());
                }
            }
            if (Definitions[EntryIndex].InFear != 2 && (Definitions[EntryIndex].InFear == 0 && static_cast<std::uint8_t>(InFear ^ 1) || Definitions[EntryIndex].InFear == 1 && InFear)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipBadFlyToShip != 2) {
                Rejected = IsEnemyPursuingSelf();
                if (Definitions[EntryIndex].ShipBadFlyToShip == 0 && static_cast<std::uint8_t>(Rejected ^ 1) || Definitions[EntryIndex].ShipBadFlyToShip == 1 && Rejected) {
                    continue;
                }
            }
            if (Definitions[EntryIndex].ShipBadType != pas::constant_set<Globals::TGreetingMask>({}) && EnemyShip != nullptr && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipBadType, EnemyShip->GetGreetingShipCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipBadRace != pas::constant_set<aGalaxyStruct::TOwnerMask>({}) && EnemyShip != nullptr && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipBadRace, EnemyShip->PilotRace) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipFlyToPlayer != 2 && (Definitions[EntryIndex].ShipFlyToPlayer == 0 && !(aPlayer::GetPlayer() == OrderTarget) || Definitions[EntryIndex].ShipFlyToPlayer == 1 && aPlayer::GetPlayer() == OrderTarget)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerFlyToShip != 2 && (Definitions[EntryIndex].PlayerFlyToShip == 0 && !(aPlayer::GetPlayer()->OrderTarget == this) || Definitions[EntryIndex].PlayerFlyToShip == 1 && aPlayer::GetPlayer()->OrderTarget == this)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerIsShipBad != 2 && (Definitions[EntryIndex].PlayerIsShipBad == 0 && !(aPlayer::GetPlayer() == EnemyShip) || Definitions[EntryIndex].PlayerIsShipBad == 1 && aPlayer::GetPlayer() == EnemyShip)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipTurnBeforeEndOrder != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                Count = std::min<std::int32_t>(10, EstimateOrderTravelTurns());
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
            if (EnemyShip != nullptr && EnemyShip->CurrentStar == CurrentStar && EnemyShip->InNormalSpace() && Definitions[EntryIndex].ShipBadTurnBeforeEndOrder != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                Count = std::min<std::int32_t>(10, EnemyShip->EstimateOrderTravelTurns());
                if (!pas::contains(Definitions[EntryIndex].ShipBadTurnBeforeEndOrder, static_cast<std::uint32_t>(Count))) {
                    continue;
                }
            }
            if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr && Definitions[EntryIndex].ShipStatus != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipStatus, static_cast<std::uint8_t>(static_cast<aRanger::TRanger*>(this)->GetDominantCareer())) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerStatus != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerStatus, static_cast<std::uint8_t>(aPlayer::GetPlayer()->GetDominantCareer())) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipStrength != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipStrength, GetRelativeStrengthCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerStrength != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerStrength, aPlayer::GetPlayer()->GetRelativeStrengthCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipStructure != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipStructure, GetHullConditionCategory()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerStructure != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerStructure, aPlayer::GetPlayer()->GetHullConditionCategory()) ^ 1)) {
                continue;
            }
            if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr && static_cast<std::uint8_t>(static_cast<aRanger::TRanger*>(this)->ExcludedFromRating ^ 1) && Definitions[EntryIndex].ShipRating != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipRating, GetRangerRatingBand()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerRating != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerRating, aPlayer::GetPlayer()->GetRangerRatingBand()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipRank, Rank) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerRank, aPlayer::GetPlayer()->Rank) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerPirateRank != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerPirateRank, aPlayer::GetPlayer()->PirateRank) ^ 1)) {
                continue;
            }
            if (pas::class_cast_if<aRanger::TRanger*>(this) != nullptr && static_cast<std::uint8_t>(static_cast<aRanger::TRanger*>(this)->ExcludedFromRating ^ 1) && Definitions[EntryIndex].RatingShipWithPlayer != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                std::uint8_t cpp_element_6 = (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipRatingComparison(this));
                const Globals::TGreetingMask& cpp_set_6 = Definitions[EntryIndex].RatingShipWithPlayer;
                return pas::contains(cpp_set_6, cpp_element_6);
            }()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].RankShipWithPlayer != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].RankShipWithPlayer, (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipRankComparison(this))) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].RankShipWithPlayerExtra != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].RankShipWithPlayerExtra, (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipPirateRankComparison(this))) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].StrengthShipWithPlayer != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].StrengthShipWithPlayer, (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetShipStrengthComparison(this))) ^ 1)) {
                continue;
            }
            if (Good != 50) {
                if (Definitions[EntryIndex].ShipGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(CargoGoods[Good].Count, Good)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].PlayerGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(aPlayer::GetPlayer()->CargoGoods[Good].Count, Good)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].ShipHaveGoods != 2 && (Definitions[EntryIndex].ShipHaveGoods == 0 && CargoGoods[Good].Count == 0 || Definitions[EntryIndex].ShipHaveGoods == 1 && CargoGoods[Good].Count > 0)) {
                    continue;
                }
                if (Definitions[EntryIndex].PlayerHaveGoods != 2 && (Definitions[EntryIndex].PlayerHaveGoods == 0 && aPlayer::GetPlayer()->CargoGoods[Good].Count == 0 || Definitions[EntryIndex].PlayerHaveGoods == 1 && aPlayer::GetPlayer()->CargoGoods[Good].Count > 0)) {
                    continue;
                }
            }
            if (Definitions[EntryIndex].ShipGoodsTypeCnt != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].ShipGoodsTypeCnt, CountCargoGoodsTypes()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].PlayerGoodsTypeCnt != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].PlayerGoodsTypeCnt, aPlayer::GetPlayer()->CountCargoGoodsTypes()) ^ 1)) {
                continue;
            }
            if (Definitions[EntryIndex].ShipMayScanPlayer != 2 && (Definitions[EntryIndex].ShipMayScanPlayer == 0 && !(aShip::TShip_CanResolveObjectWithScanner(this, aPlayer::GetPlayer()) && aShip::TShip_GetRadarRange(this) > 0) || Definitions[EntryIndex].ShipMayScanPlayer == 1 && (aShip::TShip_CanResolveObjectWithScanner(this, aPlayer::GetPlayer()) && aShip::TShip_GetRadarRange(this) > 0))) {
                continue;
            }
            Rejected = false;
            for (ShipKind = static_cast<std::uint8_t>(0); ShipKind <= static_cast<std::uint8_t>(4); ++ShipKind) {
                switch (ShipKind) {
                    case 0: CountMask = Definitions[EntryIndex].KlingInCurStar; break;
                    case 1: CountMask = Definitions[EntryIndex].RangerInCurStar; break;
                    case 3: CountMask = Definitions[EntryIndex].PirateInCurStar; break;
                    case 4: CountMask = Definitions[EntryIndex].WarriorInCurStar; break;
                    case 2: CountMask = Definitions[EntryIndex].TransportInCurStar; break;
                }
                if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                    Count = 0;
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(CurrentStar->Ships) - 1); cpp_range_4.next(K); ) {
                        Other = pas::list_at<aShip::TShip>(CurrentStar->Ships, K);
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
                if (LastDockedPlanet == nullptr) {
                    continue;
                }
                if (!pas::in_set<0, 4, 7, 7>(LastDockedPlanet->OwnerId)) {
                    continue;
                }
                if (LastDockedPlanet->CurrentStar->Status.CustomFaction != u"") {
                    continue;
                }
                if (!pas::contains(Definitions[EntryIndex].LastPlanetRace, LastDockedPlanet->RaceId)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetRelations != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                    std::uint8_t cpp_element_7 = static_cast<std::uint8_t>(LastDockedPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()));
                    const Globals::TGreetingMask& cpp_set_7 = Definitions[EntryIndex].LastPlanetRelations;
                    return pas::contains(cpp_set_7, cpp_element_7);
                }()) ^ 1)) {
                    continue;
                }
                if (Good != 50) {
                    if (Definitions[EntryIndex].LastPlanetGoodsCnt != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].LastPlanetGoodsCnt, aGalaxy::Galaxy->ClassifyGoodsQuantity(LastDockedPlanet->Goods[Good].Count, Good)) ^ 1)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].LastPlanetGoodsSale != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(([&] {
                        std::uint8_t cpp_element_8 = ([&] {
                            std::int32_t shopGoodsPurchasePrice_2 = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, LastDockedPlanet);
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
                            std::int32_t shopGoodsSellPrice_2 = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, LastDockedPlanet);
                            aGalaxy::TGalaxy* galaxy_4 = aGalaxy::Galaxy;
                            return galaxy_4->ClassifyGoodsPrice(shopGoodsSellPrice_2, Good);
                        }());
                        const Globals::TGreetingMask& cpp_set_9 = Definitions[EntryIndex].LastPlanetGoodsBuy;
                        return pas::contains(cpp_set_9, cpp_element_9);
                    }()) ^ 1)) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].LastPlanetIsHomePlanet != 2 && (Definitions[EntryIndex].LastPlanetIsHomePlanet == 0 && !(LastDockedPlanet == HomePlanet) || Definitions[EntryIndex].LastPlanetIsHomePlanet == 1 && LastDockedPlanet == HomePlanet)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetRaceIsShipRace != 2) {
                    if (!pas::in_set<0, 4, 7, 7>(LastDockedPlanet->OwnerId)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].LastPlanetRaceIsShipRace == 0 && !(LastDockedPlanet->RaceId == PilotRace) || Definitions[EntryIndex].LastPlanetRaceIsShipRace == 1 && LastDockedPlanet->RaceId == PilotRace) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].LastPlanetRaceIsPlayerRace != 2) {
                    if (!pas::in_set<0, 4, 7, 7>(LastDockedPlanet->OwnerId)) {
                        continue;
                    }
                    if (Definitions[EntryIndex].LastPlanetRaceIsPlayerRace == 0 && !(aPlayer::GetPlayer()->PilotRace == LastDockedPlanet->RaceId) || Definitions[EntryIndex].LastPlanetRaceIsPlayerRace == 1 && aPlayer::GetPlayer()->PilotRace == LastDockedPlanet->RaceId) {
                        continue;
                    }
                }
                if (Definitions[EntryIndex].LastPlanetEconomy != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].LastPlanetEconomy, static_cast<std::uint8_t>(LastDockedPlanet->Economy)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetGovernment != pas::constant_set<Globals::TGreetingMask>({}) && static_cast<std::uint8_t>(pas::contains(Definitions[EntryIndex].LastPlanetGovernment, static_cast<std::uint8_t>(LastDockedPlanet->Government)) ^ 1)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetInCurStar != 2 && (Definitions[EntryIndex].LastPlanetInCurStar == 0 && !(LastDockedPlanet->CurrentStar == CurrentStar) || Definitions[EntryIndex].LastPlanetInCurStar == 1 && LastDockedPlanet->CurrentStar == CurrentStar)) {
                    continue;
                }
                if (Definitions[EntryIndex].LastPlanetDistToShipInTurn != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                    Count = std::min<std::int32_t>(10, EstimateTravelTurnsToPlanet(LastDockedPlanet));
                    if (Count == -1) {
                        continue;
                    }
                    if (!pas::contains(Definitions[EntryIndex].LastPlanetDistToShipInTurn, static_cast<std::uint32_t>(Count))) {
                        continue;
                    }
                }
                if (LastDockedPlanet->CurrentStar != CurrentStar) {
                    Rejected = false;
                    for (ShipKind = static_cast<std::uint8_t>(0); ShipKind <= static_cast<std::uint8_t>(4); ++ShipKind) {
                        switch (ShipKind) {
                            case 0: CountMask = Definitions[EntryIndex].KlingInLastPlanetStar; break;
                            case 1: CountMask = Definitions[EntryIndex].RangerInLastPlanetStar; break;
                            case 3: CountMask = Definitions[EntryIndex].PirateInLastPlanetStar; break;
                            case 4: CountMask = Definitions[EntryIndex].WarriorInLastPlanetStar; break;
                            case 2: CountMask = Definitions[EntryIndex].TransportInLastPlanetStar; break;
                        }
                        if (CountMask != pas::constant_set<aGalaxyStruct::TGreetingCountMask>({})) {
                            Count = 0;
                            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(LastDockedPlanet->CurrentStar->Ships) - 1); cpp_range_5.next(K); ) {
                                Other = pas::list_at<aShip::TShip>(LastDockedPlanet->CurrentStar->Ships, K);
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
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanet>"_w, pas::concat_wide({LastDockedPlanet->Name, aGalaxy::GetLocalObjectLink(LastDockedPlanet, Automatic)}), u"<color=255,240,100>"_w);
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanetStar>"_w, LastDockedPlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
                if (Good != 50) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanetGoodsSale>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, LastDockedPlanet)), u"<color=255,240,100>"_w);
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<LastPlanetGoodsBuy>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, LastDockedPlanet)), u"<color=255,240,100>"_w);
                }
            }
            if (MessageText != u"") {
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<Ship>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(this, Automatic), GetName()}), u"<color=255,240,100>"_w);
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<FullShip>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(this, Automatic), GetFullName(u" "_wref.get())}), u"<color=255,240,100>"_w);
                if (EnemyShip != nullptr) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ShipBad>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(EnemyShip, Automatic), EnemyShip->GetName()}), u"<color=255,240,100>"_w);
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<FullShipBad>"_w, pas::concat_wide_reverse({aGalaxy::GetLocalObjectLink(EnemyShip, Automatic), EnemyShip->GetFullName(u" "_wref.get())}), u"<color=255,240,100>"_w);
                }
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<ShipRank>"_w, GetRankName(), u"<color=255,240,100>"_w);
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<PlayerRank>"_w, aPlayer::GetPlayer()->GetRankName(), u"<color=255,240,100>"_w);
                MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<CurStar>"_w, CurrentStar->Name, u"<color=255,240,100>"_w);
                if (HomePlanet != nullptr) {
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<HomePlanet>"_w, pas::concat_wide({HomePlanet->Name, aGalaxy::GetLocalObjectLink(HomePlanet, Automatic)}), u"<color=255,240,100>"_w);
                    MessageText = aMyFunction::ReplaceColoredToken(MessageText, u"<HomePlanetStar>"_w, HomePlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
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

    void TNormalShip_TrainSkillsAutomatically(TNormalShip* Self) {
        float Score{};
        float BestScore{};
        aShip::TPilotSkill Skill{};
        aShip::TPilotSkill BestSkill{};
        std::uint8_t Bonus{};
        do {
            BestScore = -1.0f;
            BestSkill = aShip::psAccuracy;
            for (Bonus = static_cast<std::uint8_t>(22); Bonus <= static_cast<std::uint8_t>(27); ++Bonus) {
                Skill = static_cast<aShip::TPilotSkill>(aConst::EquipmentBonusSkills[Bonus - 22]);
                if (Self->BaseSkills[Skill] < 6) {
                    {
                        pas::Extended cpp_left = pas::sqr(static_cast<pas::Extended>(Self->virtual_TShip_EvaluateStatBonus(static_cast<aConst::TEquipmentBonusKind>(Bonus), 1)));
                        Score = pas::real_divide(cpp_left, aConst::SkillTrainingCosts[Self->BaseSkills[Skill] + 1][Skill]);
                    }
                    if (Score > BestScore) {
                        BestScore = Score;
                        BestSkill = Skill;
                    }
                }
            }
            if (BestScore < 0.0L || aConst::SkillTrainingCosts[Self->BaseSkills[BestSkill] + 1][BestSkill] > Self->FreeExperience) {
                break;
            }
        } while (Self->TrainSkill(BestSkill));
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
