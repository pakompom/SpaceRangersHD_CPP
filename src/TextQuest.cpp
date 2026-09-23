#include "layout/TextQuest.hpp"
#include "types/CPDiapClass.hpp"
#include "types/TextFieldClass.hpp"
#include "types/TextQuestInterface.hpp"
#include "units/CalcParseClass.hpp"
#include "units/Dialogs.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EventClass.hpp"
#include "units/LocationClass.hpp"
#include "units/MessageText.hpp"
#include "units/ParameterClass.hpp"
#include "units/PathClass.hpp"
#include "units/SequenceClass.hpp"
#include "units/System.hpp"
#include "units/TextQuest.hpp"

namespace TextQuest {
    // Returns zero counts for start, success, failure, or already grouped locations. Callee pops 8 bytes; caller pops ParentFrame.
    void CountLegacySequenceConnections(LocationClass::TLocation* Location, std::int32_t& IncomingCount, std::int32_t& OutgoingCount, PathClass::TPath*& IncomingPath, PathClass::TPath*& OutgoingPath, TTextQuest* Self);

    void PrependLegacySequencePaths(PathClass::TPath* Path, TTextQuest* Self, SequenceClass::TSequence*& Sequence);

    void AppendLegacySequencePaths(PathClass::TPath* Path, TTextQuest* Self, SequenceClass::TSequence*& Sequence);

    void TTextQuest_Create(TTextQuest* Self) {
        Self->PlayerInterface = nullptr;
        Self->TextShown = false;
        Self->OutcomeEvent = pas::construct_call<EventClass::TEvent>(EventClass::TEvent_Create);
        Self->Outcome = TextQuestInterface::qoNone;
        Self->DisplayedEvent = nullptr;
        Self->MajorVersion = 1;
        Self->MinorVersion = 0;
        Self->ChangeLogText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->QuestSuccessGovMessageText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->QuestDescriptionText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->ToStarText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->ToPlanetText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->DateText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->MoneyText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->FromPlanetText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->FromStarText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->RangerText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Locations = pas::make_object<pas::List>();
        Self->Paths = pas::make_object<pas::List>();
        Self->Parameters = pas::make_object<pas::List>();
        Self->Reset();
        {
            void* cpp_arg = reinterpret_cast<void*>(pas::construct_call<ParameterClass::TParameter>(ParameterClass::TParameter_Create, 1));
            pas::List* parameters = Self->Parameters;
            pas::list_add(parameters, cpp_arg);
        }
    }

    void TTextQuest_Destroy(TTextQuest* Self) {
        Self->Reset();
        if (Self->ChangeLogText != nullptr) {
            pas::free(Self->ChangeLogText);
            Self->ChangeLogText = nullptr;
        }
        if (Self->QuestSuccessGovMessageText != nullptr) {
            pas::free(Self->QuestSuccessGovMessageText);
            Self->QuestSuccessGovMessageText = nullptr;
        }
        if (Self->QuestDescriptionText != nullptr) {
            pas::free(Self->QuestDescriptionText);
            Self->QuestDescriptionText = nullptr;
        }
        if (Self->OutcomeEvent != nullptr) {
            pas::free(Self->OutcomeEvent);
            Self->OutcomeEvent = nullptr;
        }
        if (Self->ToStarText != nullptr) {
            pas::free(Self->ToStarText);
            Self->ToStarText = nullptr;
        }
        if (Self->ToPlanetText != nullptr) {
            pas::free(Self->ToPlanetText);
            Self->ToPlanetText = nullptr;
        }
        if (Self->DateText != nullptr) {
            pas::free(Self->DateText);
            Self->DateText = nullptr;
        }
        if (Self->MoneyText != nullptr) {
            pas::free(Self->MoneyText);
            Self->MoneyText = nullptr;
        }
        if (Self->FromPlanetText != nullptr) {
            pas::free(Self->FromPlanetText);
            Self->FromPlanetText = nullptr;
        }
        if (Self->FromStarText != nullptr) {
            pas::free(Self->FromStarText);
            Self->FromStarText = nullptr;
        }
        if (Self->RangerText != nullptr) {
            pas::free(Self->RangerText);
            Self->RangerText = nullptr;
        }
        pas::free(Self->Locations);
        Self->Locations = nullptr;
        pas::free(Self->Paths);
        Self->Paths = nullptr;
        pas::free(Self->Parameters);
        Self->Parameters = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Retains PlayerInterface and the owned text/event containers.
    void TTextQuest::Reset() {
        std::int32_t i{};
        FormatVersion = 1111111127;
        MajorVersion = 1;
        MinorVersion = 0;
        ChangeLogText->ClearText();
        CompleteOnFinish = true;
        IssuerRaces = pas::constant_set<TQuestRaceSet>({{qrMaloc, qrGaal}});
        TargetRaces = pas::constant_set<TQuestRaceSet>({{qrUninhabited}});
        PlayerRaces = pas::constant_set<TQuestRaceSet>({{qrMaloc, qrGaal}});
        PlayerCareers = pas::constant_set<TQuestPlayerCareerSet>({{qpcTrader, qpcWarrior}});
        EditorScreenWidth = 0;
        EditorScreenHeight = 0;
        DefaultTraversalLimit = 0;
        Difficulty = 50;
        SuccessRelationDelta = 0;
        EditorGridWidth = 10;
        EditorGridHeight = 8;
        QuestSuccessGovMessageText->Text = MessageText::QuestMessages->GetText(u"GameContent.QuestSuccessGovMessage"_w);
        QuestDescriptionText->Text = MessageText::QuestMessages->GetText(u"GameContent.QuestDecription"_w);
        ToStarText->Text = MessageText::QuestMessages->GetText(u"GameContent.RToStar"_w);
        ToPlanetText->Text = MessageText::QuestMessages->GetText(u"GameContent.RToPlanet"_w);
        DateText->Text = MessageText::QuestMessages->GetText(u"GameContent.RDate"_w);
        MoneyText->Text = MessageText::QuestMessages->GetText(u"GameContent.RMoney"_w);
        FromPlanetText->Text = MessageText::QuestMessages->GetText(u"GameContent.RFromPLanet"_w);
        FromStarText->Text = MessageText::QuestMessages->GetText(u"GameContent.RFromStar"_w);
        RangerText->Text = MessageText::QuestMessages->GetText(u"GameContent.RRanger"_w);
        {
            const std::int32_t cpp_first = GetLocationCount();
            if (cpp_first >= 1) {
                for (i = cpp_first; i >= 1; --i) {
                    pas::free(GetLocation(i));
                }
            }
        }
        pas::list_clear(Locations);
        {
            const std::int32_t cpp_first_2 = GetPathCount();
            if (cpp_first_2 >= 1) {
                for (i = cpp_first_2; i >= 1; --i) {
                    pas::free(GetPath(i));
                }
            }
        }
        pas::list_clear(Paths);
        {
            const std::int32_t cpp_first_3 = GetParameterCount();
            if (cpp_first_3 >= 1) {
                for (i = cpp_first_3; i >= 1; --i) {
                    pas::free(GetParameter(i));
                }
            }
        }
        pas::list_clear(Parameters);
        OutcomeEvent->ClearTextFields();
        Outcome = TextQuestInterface::qoNone;
        DisplayedEvent = nullptr;
    }

    std::int32_t TTextQuest::GetLocationCount() {
        return pas::list_count(Locations);
    }

    std::int32_t TTextQuest::GetPathCount() {
        return pas::list_count(Paths);
    }

    // Index is one-based.
    LocationClass::TLocation* TTextQuest::GetLocation(std::int32_t Index) {
        return pas::list_at<LocationClass::TLocation>(Locations, Index - 1);
    }

    // Index is one-based.
    PathClass::TPath* TTextQuest::GetPath(std::int32_t Index) {
        return pas::list_at<PathClass::TPath>(Paths, Index - 1);
    }

    std::int32_t TTextQuest::GetParameterCount() {
        return pas::list_count(Parameters);
    }

    // Index is one-based; the list includes an extra trailing parameter.
    ParameterClass::TParameter* TTextQuest::GetParameter(std::int32_t Index) {
        return pas::list_at<ParameterClass::TParameter>(Parameters, Index - 1);
    }

    // HeaderOnly still loads parameters and quest text.
    void TTextQuest::LoadFromReader(EC_Buf::TBufEC* Reader, std::uint8_t HeaderOnly) {
        std::int32_t ParameterCount{};
        TextFieldClass::TTextField* TemporaryText{};
        Reset();
        FormatVersion = EC_Buf::TBufEC_GetInt32(Reader);
        if (FormatVersion >= 1111111127) {
            MajorVersion = EC_Buf::TBufEC_GetInt32(Reader);
            MinorVersion = EC_Buf::TBufEC_GetInt32(Reader);
            ChangeLogText->LoadTextLinesFromReader(Reader);
        } else {
            MajorVersion = 1;
            MinorVersion = 0;
            ChangeLogText->ClearText();
        }
        std::int32_t i = 0;
        if (FormatVersion <= 1111111111) {
            i = FormatVersion;
            FormatVersion = 1111111111;
        } else if (FormatVersion < 1111111125) {
            i = EC_Buf::TBufEC_GetInt32(Reader);
        }
        if (FormatVersion >= 1111111119) {
            Reader->ReadBytes(&IssuerRaces, 1);
        } else {
            switch (i) {
                case -1: IssuerRaces = pas::constant_set<TQuestRaceSet>({{qrUninhabited}}); break;
                case 0: IssuerRaces = pas::constant_set<TQuestRaceSet>({{qrMaloc}}); break;
                case 1: IssuerRaces = pas::constant_set<TQuestRaceSet>({{qrPeleng}}); break;
                case 2: IssuerRaces = pas::constant_set<TQuestRaceSet>({{qrHuman}}); break;
                case 3: IssuerRaces = pas::constant_set<TQuestRaceSet>({{qrFeyan}}); break;
                case 4: IssuerRaces = pas::constant_set<TQuestRaceSet>({{qrGaal}}); break;
                default: IssuerRaces = pas::constant_set<TQuestRaceSet>({}); break;
            }
        }
        if (FormatVersion >= 1111111112) {
            CompleteOnFinish = EC_Buf::TBufEC_GetBoolean(Reader);
        }
        if (FormatVersion < 1111111125) {
            i = EC_Buf::TBufEC_GetInt32(Reader);
        }
        if (FormatVersion >= 1111111119) {
            Reader->ReadBytes(&TargetRaces, 1);
        } else {
            switch (i) {
                case -1: TargetRaces = pas::constant_set<TQuestRaceSet>({{qrUninhabited}}); break;
                case 0: TargetRaces = pas::constant_set<TQuestRaceSet>({{qrMaloc}}); break;
                case 1: TargetRaces = pas::constant_set<TQuestRaceSet>({{qrPeleng}}); break;
                case 2: TargetRaces = pas::constant_set<TQuestRaceSet>({{qrHuman}}); break;
                case 3: TargetRaces = pas::constant_set<TQuestRaceSet>({{qrFeyan}}); break;
                case 4: TargetRaces = pas::constant_set<TQuestRaceSet>({{qrGaal}}); break;
                default: TargetRaces = pas::constant_set<TQuestRaceSet>({}); break;
            }
        }
        if (FormatVersion < 1111111125) {
            i = EC_Buf::TBufEC_GetInt32(Reader);
        }
        if (FormatVersion >= 1111111120) {
            Reader->ReadBytes(&PlayerCareers, 1);
        } else {
            switch (i) {
                case -1: PlayerCareers = pas::constant_set<TQuestPlayerCareerSet>({{qpcTrader, qpcWarrior}}); break;
                case 0: PlayerCareers = pas::constant_set<TQuestPlayerCareerSet>({{qpcTrader}}); break;
                case 1: PlayerCareers = pas::constant_set<TQuestPlayerCareerSet>({{qpcPirate}}); break;
                case 2: PlayerCareers = pas::constant_set<TQuestPlayerCareerSet>({{qpcWarrior}}); break;
                default: PlayerCareers = pas::constant_set<TQuestPlayerCareerSet>({}); break;
            }
        }
        if (FormatVersion < 1111111125) {
            i = EC_Buf::TBufEC_GetInt32(Reader);
        }
        if (FormatVersion >= 1111111120) {
            Reader->ReadBytes(&PlayerRaces, 1);
        } else {
            switch (i) {
                case -1: PlayerRaces = pas::constant_set<TQuestRaceSet>({{qrMaloc, qrGaal}}); break;
                case 0: PlayerRaces = pas::constant_set<TQuestRaceSet>({{qrMaloc}}); break;
                case 1: PlayerRaces = pas::constant_set<TQuestRaceSet>({{qrPeleng}}); break;
                case 2: PlayerRaces = pas::constant_set<TQuestRaceSet>({{qrHuman}}); break;
                case 3: PlayerRaces = pas::constant_set<TQuestRaceSet>({{qrFeyan}}); break;
                case 4: PlayerRaces = pas::constant_set<TQuestRaceSet>({{qrGaal}}); break;
                default: PlayerRaces = pas::constant_set<TQuestRaceSet>({}); break;
            }
        }
        SuccessRelationDelta = EC_Buf::TBufEC_GetInt32(Reader);
        EditorScreenWidth = EC_Buf::TBufEC_GetInt32(Reader);
        EditorScreenHeight = EC_Buf::TBufEC_GetInt32(Reader);
        EditorGridWidth = EC_Buf::TBufEC_GetInt32(Reader);
        EditorGridHeight = EC_Buf::TBufEC_GetInt32(Reader);
        if (FormatVersion < 1111111125) {
            EC_Buf::TBufEC_GetInt32(Reader);
        }
        if (FormatVersion >= 1111111120) {
            DefaultTraversalLimit = EC_Buf::TBufEC_GetInt32(Reader);
        }
        if (FormatVersion >= 1111111121) {
            Difficulty = EC_Buf::TBufEC_GetInt32(Reader);
        }
        if (FormatVersion >= 1111111125) {
            ParameterCount = EC_Buf::TBufEC_GetInt32(Reader);
        } else if (FormatVersion >= 1111111124) {
            ParameterCount = 96;
        } else if (FormatVersion >= 1111111123) {
            ParameterCount = 48;
        } else if (FormatVersion >= 1111111121) {
            ParameterCount = 24;
        } else if (FormatVersion >= 1111111119) {
            ParameterCount = 24;
        } else if (FormatVersion >= 1111111118) {
            ParameterCount = 12;
        } else if (FormatVersion >= 1111111115) {
            ParameterCount = 12;
        } else if (FormatVersion >= 1111111113) {
            ParameterCount = 9;
        } else {
            ParameterCount = 9;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range.next(i); ) {
            void* cpp_arg = reinterpret_cast<void*>(pas::construct_call<ParameterClass::TParameter>(ParameterClass::TParameter_Create, i));
            pas::List* parameters = Parameters;
            pas::list_add(parameters, cpp_arg);
        }
        if (FormatVersion >= 1111111125) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_2.next(i); ) {
                GetParameter(i)->LoadFromReader(Reader);
            }
        } else if (FormatVersion >= 1111111124) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_3.next(i); ) {
                GetParameter(i)->LoadLegacyV4FromReader(Reader);
            }
        } else if (FormatVersion >= 1111111123) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_4.next(i); ) {
                GetParameter(i)->LoadLegacyV4FromReader(Reader);
            }
        } else if (FormatVersion >= 1111111121) {
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_5.next(i); ) {
                GetParameter(i)->LoadLegacyV4FromReader(Reader);
            }
        } else if (FormatVersion >= 1111111119) {
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_6.next(i); ) {
                GetParameter(i)->LoadLegacyV3FromReader(Reader);
            }
        } else if (FormatVersion >= 1111111118) {
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_7.next(i); ) {
                GetParameter(i)->LoadLegacyV2FromReader(Reader);
            }
        } else if (FormatVersion >= 1111111115) {
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_8.next(i); ) {
                GetParameter(i)->LoadLegacyV1FromReader(Reader);
            }
        } else if (FormatVersion >= 1111111113) {
            for (auto cpp_range_9 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_9.next(i); ) {
                GetParameter(i)->LoadLegacyV1FromReader(Reader);
            }
        } else {
            for (auto cpp_range_10 = pas::for_to<std::int32_t>(1, ParameterCount); cpp_range_10.next(i); ) {
                GetParameter(i)->LoadLegacyV0FromReader(Reader);
            }
        }
        {
            void* cpp_arg_2 = reinterpret_cast<void*>(pas::construct_call<ParameterClass::TParameter>(ParameterClass::TParameter_Create, ParameterCount + 1));
            pas::List* parameters_2 = Parameters;
            pas::list_add(parameters_2, cpp_arg_2);
        }
        ToStarText->LoadTextLinesFromReader(Reader);
        if (FormatVersion < 1111111125) {
            TemporaryText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
            TemporaryText->LoadTextLinesFromReader(Reader);
            TemporaryText->ClearText();
            TemporaryText->LoadTextLinesFromReader(Reader);
            pas::free(TemporaryText);
        }
        ToPlanetText->LoadTextLinesFromReader(Reader);
        DateText->LoadTextLinesFromReader(Reader);
        MoneyText->LoadTextLinesFromReader(Reader);
        FromPlanetText->LoadTextLinesFromReader(Reader);
        FromStarText->LoadTextLinesFromReader(Reader);
        RangerText->LoadTextLinesFromReader(Reader);
        std::int32_t LocationCount = EC_Buf::TBufEC_GetInt32(Reader);
        std::int32_t PathCount = EC_Buf::TBufEC_GetInt32(Reader);
        QuestSuccessGovMessageText->LoadTextLinesFromReader(Reader);
        QuestDescriptionText->LoadTextLinesFromReader(Reader);
        if (!HeaderOnly) {
            if (FormatVersion < 1111111125) {
                TemporaryText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
                TemporaryText->LoadTextLinesFromReader(Reader);
                pas::free(TemporaryText);
            }
            for (auto cpp_range_11 = pas::for_to<std::int32_t>(1, PathCount); cpp_range_11.next(i); ) {
                void* cpp_arg_3 = reinterpret_cast<void*>(pas::construct_call<PathClass::TPath>(PathClass::TPath_Create));
                pas::List* paths = Paths;
                pas::list_add(paths, cpp_arg_3);
            }
            for (auto cpp_range_12 = pas::for_to<std::int32_t>(1, LocationCount); cpp_range_12.next(i); ) {
                void* cpp_arg_4 = reinterpret_cast<void*>(pas::construct_call<LocationClass::TLocation>(LocationClass::TLocation_Create));
                pas::List* locations = Locations;
                pas::list_add(locations, cpp_arg_4);
            }
            if (FormatVersion >= 1111111126) {
                for (auto cpp_range_13 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_13.next(i); ) {
                    GetLocation(i)->LoadFromReader(Reader);
                }
            } else if (FormatVersion >= 1111111125) {
                for (auto cpp_range_14 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_14.next(i); ) {
                    GetLocation(i)->LoadLegacyV8FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111124) {
                for (auto cpp_range_15 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_15.next(i); ) {
                    GetLocation(i)->LoadLegacyV7FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111123) {
                for (auto cpp_range_16 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_16.next(i); ) {
                    GetLocation(i)->LoadLegacyV6FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111121) {
                for (auto cpp_range_17 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_17.next(i); ) {
                    GetLocation(i)->LoadLegacyV5FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111119) {
                for (auto cpp_range_18 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_18.next(i); ) {
                    GetLocation(i)->LoadLegacyV4FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111117) {
                for (auto cpp_range_19 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_19.next(i); ) {
                    GetLocation(i)->LoadLegacyV3FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111116) {
                for (auto cpp_range_20 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_20.next(i); ) {
                    GetLocation(i)->LoadLegacyV2FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111115) {
                for (auto cpp_range_21 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_21.next(i); ) {
                    GetLocation(i)->LoadLegacyV1FromReader(Reader);
                }
            } else {
                for (auto cpp_range_22 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_22.next(i); ) {
                    GetLocation(i)->LoadLegacyV0FromReader(Reader);
                }
            }
            if (FormatVersion >= 1111111125) {
                for (auto cpp_range_23 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_23.next(i); ) {
                    GetPath(i)->LoadFromReader(Reader, Parameters);
                }
            } else if (FormatVersion >= 1111111124) {
                for (auto cpp_range_24 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_24.next(i); ) {
                    GetPath(i)->LoadLegacyV9FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111123) {
                for (auto cpp_range_25 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_25.next(i); ) {
                    GetPath(i)->LoadLegacyV8FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111122) {
                for (auto cpp_range_26 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_26.next(i); ) {
                    GetPath(i)->LoadLegacyV7FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111119) {
                for (auto cpp_range_27 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_27.next(i); ) {
                    GetPath(i)->LoadLegacyV6FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111117) {
                for (auto cpp_range_28 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_28.next(i); ) {
                    GetPath(i)->LoadLegacyV5FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111116) {
                for (auto cpp_range_29 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_29.next(i); ) {
                    GetPath(i)->LoadLegacyV4FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111115) {
                for (auto cpp_range_30 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_30.next(i); ) {
                    GetPath(i)->LoadLegacyV3FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111114) {
                for (auto cpp_range_31 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_31.next(i); ) {
                    GetPath(i)->LoadLegacyV2FromReader(Reader);
                }
            } else if (FormatVersion >= 1111111112) {
                for (auto cpp_range_32 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_32.next(i); ) {
                    GetPath(i)->LoadLegacyV1FromReader(Reader);
                }
            } else {
                for (auto cpp_range_33 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_33.next(i); ) {
                    GetPath(i)->LoadLegacyV0FromReader(Reader);
                }
            }
            for (auto cpp_range_34 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_34.next(i); ) {
                GetPath(i)->PruneParameterChanges(Parameters);
            }
            for (auto cpp_range_35 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_35.next(i); ) {
                GetLocation(i)->PruneParameterChanges(Parameters);
            }
            if (FormatVersion < 1111111126) {
                BuildLegacySequences();
                InferLegacyVisitLimits();
                FreeLegacySequences();
            }
        }
    }

    void TTextQuest::ResetEventIndices() {
        std::int32_t i{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range.next(i); ) {
            GetLocation(i)->NextEventIndex = 1;
        }
    }

    // Returns the last matching one-based index; displays a message and returns zero when absent.
    std::int32_t TTextQuest::FindLocationIndex(std::int32_t LocationId) {
        std::int32_t i{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range.next(i); ) {
            if (GetLocation(i)->Id == LocationId) {
                Result = i;
            }
        }
        if (Result == 0) {
            Dialogs::ShowMessage(static_cast<pas::AnsiString>(pas::concat_wide({u"Cannot find Location with Location Number ", EC_Str::IntToWideString(LocationId)})));
        }
        return Result;
    }

    // Returns the last matching one-based index; displays a message and returns zero when absent.
    std::int32_t TTextQuest::FindPathIndex(std::int32_t PathId) {
        std::int32_t i{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range.next(i); ) {
            if (GetPath(i)->Id == PathId) {
                Result = i;
            }
        }
        if (Result == 0) {
            Dialogs::ShowMessage("Cannot find Path by Path Number - error"_a);
        }
        return Result;
    }

    // Used before quest version 1111111126.
    void TTextQuest::BuildLegacySequences() {
        SequenceClass::TSequence* Sequence{};
        std::int32_t i{};
        std::int32_t IncomingCount{};
        std::int32_t OutgoingCount{};
        LocationClass::TLocation* Location{};
        PathClass::TPath* IncomingPath{};
        PathClass::TPath* OutgoingPath{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range.next(i); ) {
            Location = GetLocation(i);
            TextQuest::CountLegacySequenceConnections(Location, IncomingCount, OutgoingCount, IncomingPath, OutgoingPath, this);
            if (IncomingCount == 1 || OutgoingCount == 1) {
                Sequence = pas::construct_call<SequenceClass::TSequence>(SequenceClass::TSequence_Create);
                Sequence->AddLocation(Location);
                if (IncomingCount == 1) {
                    TextQuest::PrependLegacySequencePaths(IncomingPath, this, Sequence);
                }
                if (OutgoingCount == 1) {
                    TextQuest::AppendLegacySequencePaths(OutgoingPath, this, Sequence);
                }
                Sequence->RecomputeTraversalLimit();
            }
        }
    }

    void CountLegacySequenceConnections(LocationClass::TLocation* Location, std::int32_t& IncomingCount, std::int32_t& OutgoingCount, PathClass::TPath*& IncomingPath, PathClass::TPath*& OutgoingPath, TTextQuest* Self) {
        std::int32_t i{};
        PathClass::TPath* Path{};
        IncomingCount = 0;
        OutgoingCount = 0;
        IncomingPath = nullptr;
        OutgoingPath = nullptr;
        if (Location->IsStart) {
            return;
        }
        if (Location->IsSuccess) {
            return;
        }
        if (Location->IsFailure) {
            return;
        }
        if (Location->Sequence != nullptr) {
            return;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Self->GetPathCount()); cpp_range.next(i); ) {
            Path = Self->GetPath(i);
            if (Path->ToLocationId == Location->Id) {
                IncomingPath = Path;
                ++IncomingCount;
            }
            if (Path->FromLocationId == Location->Id) {
                OutgoingPath = Path;
                ++OutgoingCount;
            }
            if (IncomingCount > 1 && OutgoingCount > 1) {
                return;
            }
        }
    }

    void PrependLegacySequencePaths(PathClass::TPath* Path, TTextQuest* Self, SequenceClass::TSequence*& Sequence) {
        std::int32_t IncomingCount{};
        std::int32_t OutgoingCount{};
        PathClass::TPath* IncomingPath{};
        PathClass::TPath* OutgoingPath{};
        Sequence->PrependPath(Path);
        LocationClass::TLocation* Location = Self->GetLocation(Self->FindLocationIndex(Path->FromLocationId));
        TextQuest::CountLegacySequenceConnections(Location, IncomingCount, OutgoingCount, IncomingPath, OutgoingPath, Self);
        if (OutgoingCount == 1) {
            Sequence->AddLocation(Location);
            if (IncomingCount == 1) {
                TextQuest::PrependLegacySequencePaths(IncomingPath, Self, Sequence);
            }
        }
    }

    void AppendLegacySequencePaths(PathClass::TPath* Path, TTextQuest* Self, SequenceClass::TSequence*& Sequence) {
        std::int32_t IncomingCount{};
        std::int32_t OutgoingCount{};
        PathClass::TPath* IncomingPath{};
        PathClass::TPath* OutgoingPath{};
        Sequence->AddPath(Path);
        LocationClass::TLocation* Location = Self->GetLocation(Self->FindLocationIndex(Path->ToLocationId));
        TextQuest::CountLegacySequenceConnections(Location, IncomingCount, OutgoingCount, IncomingPath, OutgoingPath, Self);
        if (IncomingCount == 1) {
            Sequence->AddLocation(Location);
            if (OutgoingCount == 1) {
                TextQuest::AppendLegacySequencePaths(OutgoingPath, Self, Sequence);
            }
        }
    }

    void TTextQuest::FreeLegacySequences() {
        std::int32_t i{};
        LocationClass::TLocation* Location{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range.next(i); ) {
            Location = GetLocation(i);
            if (Location->Sequence != nullptr) {
                pas::free(Location->Sequence);
            }
        }
    }

    // Skips success and death locations; ordinary failure locations still participate.
    void TTextQuest::InferLegacyVisitLimits() {
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t Limit{};
        std::uint8_t Unlimited{};
        std::uint8_t HasIncoming{};
        LocationClass::TLocation* Location{};
        PathClass::TPath* Path{};
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range.next(i); ) {
            Location = GetLocation(i);
            if (static_cast<std::uint8_t>(Location->IsSuccess ^ 1) && static_cast<std::uint8_t>(Location->IsDeath ^ 1)) {
                Limit = 0;
                HasIncoming = false;
                Unlimited = false;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_2.next(j); ) {
                    Path = GetPath(j);
                    if (Path->ToLocationId == Location->Id) {
                        HasIncoming = true;
                    }
                    if (Path->FromLocationId == Location->Id) {
                        if (Path->TraversalLimit <= 0) {
                            Unlimited = true;
                            break;
                        } else {
                            Limit += Path->TraversalLimit;
                        }
                    }
                }
                if (!Unlimited) {
                    if (HasIncoming) {
                        if (Location->Sequence == nullptr) {
                            Location->VisitLimit = Limit;
                        } else if (Location->VisitLimit == 0 || Location->VisitLimit > Limit) {
                            Location->Sequence->SetTraversalLimit(Limit);
                        }
                    }
                }
            }
        }
    }

    // Supports {formula}, [pN], [dN], and [dN:formula], including parameter-name aliases. Recursive display expansion has no cycle guard.
    pas::WideString TTextQuest::ExpandText(pas::WideString Text, std::uint8_t Colorize) {
        std::int32_t i{};
        std::int32_t Position{};
        std::int32_t ContentStart{};
        std::int32_t ContentLength{};
        std::int32_t Depth{};
        pas::WideString Output{};
        pas::WideString ValueText{};
        pas::WideString Fragment{};
        CalcParseClass::TCalcParse* Calc{};
        pas::WideString ColorEnd{};
        pas::WideString ColorStart{};
        if (!Colorize) {
            ColorStart = pas::WideString();
            ColorEnd = pas::WideString();
        } else {
            ColorStart = u"<clr>"_w;
            ColorEnd = u"<clrEnd>"_w;
        }
        Calc = pas::construct_call<CalcParseClass::TCalcParse>(CalcParseClass::TCalcParse_Create);
        i = 1;
        std::int32_t Count = Text.length();
        while (i <= Count) {
            if (Text.read(i) != u'{') {
                Output = pas::concat_wide({Output, Text.read(i)});
                ++i;
            } else {
                ++i;
                ValueText = pas::WideString();
                while (i <= Count && Text.read(i) != u'}') {
                    ValueText = pas::concat_wide({ValueText, Text.read(i)});
                    ++i;
                }
                if (ValueText != u"") {
                    Calc->Reset();
                    Calc->Prepare(ValueText, 0);
                    if (static_cast<std::uint8_t>(Calc->HasError ^ 1) && static_cast<std::uint8_t>(Calc->UsesDefaultParameter ^ 1)) {
                        Calc->Evaluate(Parameters);
                        if (!Calc->HasError) {
                            Output = pas::concat_wide({Output, ColorStart, EC_Str::IntToWideString(Calc->ResultValue), ColorEnd});
                        } else {
                            Output = pas::concat_wide({Output, u"{", ValueText});
                        }
                    } else {
                        Output = pas::concat_wide({Output, u"{", ValueText});
                    }
                }
                ++i;
            }
        }
        Text = std::move(Output);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterCount()); cpp_range.next(i); ) {
            Text = ([&] {
                const pas::WideString& cpp_arg = pas::concat_wide({u"[p", EC_Str::IntToWideString(i), u"]"});
                const pas::WideString& cpp_arg_2 = pas::concat_wide({u"[", EC_Str::TrimWideString(GetParameter(i)->NameText->Text), u"]"});
                const pas::WideString& text = Text;
                return EC_Str::ReplaceAllWideString(text, cpp_arg_2, pas::view(cpp_arg));
            }());
            Text = ([&] {
                const pas::WideString& cpp_arg_3 = pas::concat_wide({u"[d", EC_Str::IntToWideString(i), u":"});
                const pas::WideString& cpp_arg_4 = pas::concat_wide({u"[", EC_Str::TrimWideString(GetParameter(i)->NameText->Text), u":"});
                const pas::WideString& text_2 = Text;
                return EC_Str::ReplaceAllWideString(text_2, cpp_arg_4, pas::view(cpp_arg_3));
            }());
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, GetParameterCount()); cpp_range_2.next(i); ) {
            Position = pas::pos(pas::concat_wide({u"[d", EC_Str::IntToWideString(i), u":"}), Text);
            while (Position > 0) {
                ContentStart = Position + 3 + EC_Str::IntToWideString(i).length();
                ContentLength = 0;
                Depth = 1;
                while (ContentStart + ContentLength < Text.length()) {
                    if (Text.read(ContentStart + ContentLength) == u']') {
                        --Depth;
                    }
                    if (Text.read(ContentStart + ContentLength) == u'[') {
                        ++Depth;
                    }
                    if (Depth == 0) {
                        break;
                    }
                    ++ContentLength;
                }
                if (ContentStart >= Text.length()) {
                    break;
                }
                ValueText = pas::concat_wide({ColorStart, u"err", ColorEnd});
                Fragment = pas::copy(Text, ContentStart, ContentLength + 1);
                if (Fragment != u"") {
                    Calc->Reset();
                    Calc->Prepare(Fragment, 1);
                    if (!Calc->HasError) {
                        if (!Calc->UsesDefaultParameter) {
                            Calc->Evaluate(Parameters);
                            if (!Calc->HasError) {
                                ValueText = GetParameter(i)->GetValueText(Calc->ResultValue);
                                ValueText = EC_Str::ReplaceAllWideString(ValueText, u"<>"_wref.get(), pas::view(EC_Str::IntToWideString(Calc->ResultValue)));
                                ValueText = ExpandText(ValueText, true);
                            }
                        }
                    }
                }
                Fragment = pas::copy(Text, Position, ContentStart - Position + ContentLength + 1);
                Text = EC_Str::ReplaceAllWideString(Text, Fragment, pas::view(pas::concat_wide({ColorStart, ValueText, ColorEnd})));
                Position = pas::pos(pas::concat_wide({u"[d", EC_Str::IntToWideString(i), u":"}), Text);
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, GetParameterCount()); cpp_range_3.next(i); ) {
            Text = ([&] {
                const pas::WideString& cpp_arg_5 = pas::concat_wide({ColorStart, EC_Str::IntToWideString(GetParameter(i)->Value), ColorEnd});
                const pas::WideString& cpp_arg_6 = pas::concat_wide({u"[p", EC_Str::IntToWideString(i), u"]"});
                return EC_Str::ReplaceAllWideString(Text, cpp_arg_6, pas::view(cpp_arg_5));
            }());
            if (pas::pos(pas::concat_wide({u"[d", EC_Str::IntToWideString(i), u"]"}), Text) > 0) {
                ValueText = ([&] {
                    std::int32_t value = GetParameter(i)->Value;
                    ParameterClass::TParameter* parameter = GetParameter(i);
                    return parameter->GetValueText(value);
                }());
                ValueText = EC_Str::ReplaceAllWideString(ValueText, u"<>"_wref.get(), pas::view(EC_Str::IntToWideString(GetParameter(i)->Value)));
                ValueText = ExpandText(ValueText, true);
                Text = ([&] {
                    const pas::WideString& cpp_arg_7 = pas::concat_wide({u"[d", EC_Str::IntToWideString(i), u"]"});
                    const pas::WideString& cpp_arg_8 = pas::concat_wide({ColorStart, ValueText, ColorEnd});
                    return EC_Str::ReplaceAllWideString(Text, cpp_arg_7, pas::view(cpp_arg_8));
                }());
            }
        }
        pas::free(Calc);
        return Text;
    }

    // Outcome precedence: death, failure, success.
    std::uint8_t TTextQuest::CheckCriticalParameters() {
        std::int32_t i{};
        std::int32_t Selected{};
        ParameterClass::TParameter* Parameter{};
        EventClass::TEvent* Event{};
        pas::Array<std::int32_t, 1, 3> Outcomes{};
        std::uint8_t Result = false;
        for (i = 1; i <= 3; ++i) {
            Outcomes[i] = -1;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterCount()); cpp_range.next(i); ) {
            Parameter = GetParameter(i);
            if (Parameter->Enabled && Parameter->CriticalOutcome != TextQuestInterface::qoNone && (static_cast<std::uint8_t>(Parameter->CriticalAtMinimum ^ 1) || Parameter->Value <= Parameter->MinValue) && (Parameter->CriticalAtMinimum || Parameter->Value >= Parameter->MaxValue)) {
                if (Outcomes[static_cast<std::int32_t>(Parameter->CriticalOutcome)] < 0) {
                    Outcomes[static_cast<std::int32_t>(Parameter->CriticalOutcome)] = i;
                }
            }
        }
        if (Outcomes[3] >= 0) {
            Selected = Outcomes[3];
        } else if (Outcomes[1] >= 0) {
            Selected = Outcomes[1];
        } else if (Outcomes[2] >= 0) {
            Selected = Outcomes[2];
        } else {
            return Result;
        }
        Result = true;
        OutcomeEvent->Assign(GetParameter(Selected)->CriticalEvent);
        Outcome = GetParameter(Selected)->CriticalOutcome;
        Event = GetParameter(Selected)->CriticalEventOverride;
        if (Event != nullptr) {
            if (EC_Str::TrimWideString(Event->Text->Text) != u"") {
                OutcomeEvent->Text->Text = Event->Text->Text;
            }
            if (EC_Str::TrimWideString(Event->Picture->Text) != u"") {
                OutcomeEvent->Picture->Text = Event->Picture->Text;
            }
            if (EC_Str::TrimWideString(Event->Sound->Text) != u"") {
                OutcomeEvent->Sound->Text = Event->Sound->Text;
            }
            if (EC_Str::TrimWideString(Event->Music->Text) != u"") {
                OutcomeEvent->Music->Text = Event->Music->Text;
            }
        }
        return Result;
    }

    // External names begin with ext_; negative Money uses the initial range. Requires PlayerInterface.
    void TTextQuest::Start(std::int32_t Money, std::uint8_t PreserveExternalParameters) {
        std::int32_t i{};
        std::int32_t StartId{};
        if (PlayerInterface != nullptr) {
            TextShown = false;
            OutcomeEvent->ClearTextFields();
            Outcome = TextQuestInterface::qoNone;
            DisplayedEvent = nullptr;
            StartId = -1;
            for (auto cpp_range = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range.next(i); ) {
                if (GetLocation(i)->IsStart) {
                    StartId = GetLocation(i)->Id;
                    break;
                }
            }
            if (StartId < 0) {
                Dialogs::ShowMessage("Cant find starting location"_a);
                return;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, GetParameterCount()); cpp_range_2.next(i); ) {
                if (GetParameter(i)->Enabled) {
                    GetParameter(i)->Hidden = false;
                    GetParameter(i)->CriticalEventOverride = nullptr;
                    if (GetParameter(i)->IsMoney && Money >= 0) {
                        GetParameter(i)->Value = Money;
                    } else if (static_cast<std::uint8_t>(PreserveExternalParameters ^ 1) || EC_Str::FindTextPosW(u"ext_"_wref.get(), GetParameter(i)->NameText->Text) != 1) {
                        if (GetParameter(i)->InitialRange->RangeCount > 0) {
                            GetParameter(i)->Value = System::Trunc(GetParameter(i)->InitialRange->GetRandomValue());
                        }
                    }
                }
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_3.next(i); ) {
                GetLocation(i)->VisitCount = 0;
            }
            ResetEventIndices();
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range_4.next(i); ) {
                GetPath(i)->TraversalCount = 0;
            }
            EnterLocation(StartId);
        }
    }

    void TTextQuest::EnterLocation(std::int32_t LocationId) {
        LocationClass::TLocation* Location{};
        LocationClass::TLocation* Target{};
        pas::WideString Caption{};
        pas::WideString GroupCaption{};
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t Selected{};
        PathClass::TPath* Path{};
        PathClass::TPath* Other{};
        std::uint8_t Eligible{};
        double MaxPriority{};
        double TotalPriority{};
        double RandomPriority{};
        EventClass::TEvent* Event{};
        if (PlayerInterface == nullptr) {
            return;
        }
        {
            std::int32_t findLocationIndex = FindLocationIndex(LocationId);
            TTextQuest* self = this;
            Location = self->GetLocation(findLocationIndex);
        }
        if (TextShown && static_cast<std::uint8_t>(Location->IsEmpty ^ 1)) {
            TextShown = false;
            PlayerInterface->AddLocationContinueAction(LocationId);
            return;
        }
        Location->ApplyParameterChanges(Parameters);
        if (Location->Days > 0) {
            PlayerInterface->AdvanceDays(Location->Days);
        }
        ++Location->VisitCount;
        ShowParameters();
        std::uint8_t Critical = CheckCriticalParameters();
        Event = Location->SelectEvent(Parameters);
        if (Event != nullptr) {
            if (static_cast<std::uint8_t>(TextShown ^ 1) || static_cast<std::uint8_t>(Location->IsEmpty ^ 1)) {
                ShowEvent(Event);
                if (EC_Str::TrimWideString(Event->Text->Text) != u"") {
                    LastEventSource = pas::concat_wide({u"Location ", EC_Str::IntToWideString(Location->Id)});
                }
            }
        }
        if (Critical) {
            if (TextShown) {
                PlayerInterface->AddContinueAction();
            } else {
                ShowOutcome();
            }
            return;
        }
        if (Location->IsSuccess) {
            PlayerInterface->AddSuccessAction();
            return;
        }
        if (Location->IsDeath) {
            PlayerInterface->AddDeathAction();
            return;
        }
        if (Location->IsFailure) {
            PlayerInterface->AddFailureAction();
            return;
        }
        pas::List* Pending = pas::make_object<pas::List>();
        pas::List* Group = pas::make_object<pas::List>();
        pas::List* Chosen = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, GetPathCount()); cpp_range.next(i); ) {
            Path = GetPath(i);
            if (Location->Id != Path->FromLocationId || Path->TraversalLimit > 0 && Path->TraversalCount >= Path->TraversalLimit) {
                continue;
            }
            Eligible = false;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, GetLocationCount()); cpp_range_2.next(j); ) {
                Target = GetLocation(j);
                if (Target->Id == Path->ToLocationId) {
                    Eligible = Target->VisitLimit == 0 || Target->VisitLimit > Target->VisitCount;
                    break;
                }
            }
            if (!Eligible) {
                continue;
            }
            Path->CheckAvailable(Parameters);
            if (!Path->Available) {
                if (!Path->AlwaysShow) {
                    continue;
                }
                if (EC_Str::TrimWideString(Path->Caption->Text) == u"") {
                    continue;
                }
            }
            pas::list_add(Pending, reinterpret_cast<void*>(Path));
        }
        while (pas::list_count(Pending) > 0) {
            Path = pas::list_at<PathClass::TPath>(Pending, 0);
            GroupCaption = ([&] {
                pas::WideString trimWideString = EC_Str::TrimWideString(Path->Caption->Text);
                TTextQuest* self_2 = this;
                return self_2->ExpandText(std::move(trimWideString), false);
            }());
            pas::list_add(Group, reinterpret_cast<void*>(Path));
            pas::list_delete(Pending, 0);
            {
                const std::int32_t cpp_first = pas::list_count(Pending) - 1;
                if (cpp_first >= 0) {
                    for (i = cpp_first; i >= 0; --i) {
                        Path = pas::list_at<PathClass::TPath>(Pending, i);
                        if (GroupCaption == ([&] {
                            pas::WideString trimWideString_2 = EC_Str::TrimWideString(Path->Caption->Text);
                            TTextQuest* self_3 = this;
                            return self_3->ExpandText(std::move(trimWideString_2), false);
                        }())) {
                            pas::list_delete(Pending, i);
                            if (Path->Available || pas::list_count(Group) <= 0) {
                                pas::list_add(Group, reinterpret_cast<void*>(Path));
                                if (pas::list_count(Group) > 1) {
                                    Path = pas::list_at<PathClass::TPath>(Group, 0);
                                    if (!Path->Available) {
                                        pas::list_delete(Group, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            MaxPriority = 0.0;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Group) - 1); cpp_range_3.next(i); ) {
                Path = pas::list_at<PathClass::TPath>(Group, i);
                if (MaxPriority < Path->Priority) {
                    MaxPriority = Path->Priority;
                }
            }
            if (pas::list_count(Group) == 1) {
                if (pas::random_real(&System::RandSeed) <= MaxPriority) {
                    pas::list_add(Chosen, pas::list_get(Group, 0));
                }
            } else {
                {
                    const std::int32_t cpp_first_2 = pas::list_count(Group) - 1;
                    if (cpp_first_2 >= 0) {
                        for (i = cpp_first_2; i >= 0; --i) {
                            Path = pas::list_at<PathClass::TPath>(Group, i);
                            if (Path->Priority <= MaxPriority * 0.01L) {
                                pas::list_delete(Group, i);
                            }
                        }
                    }
                }
                TotalPriority = 0.0;
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Group) - 1); cpp_range_4.next(i); ) {
                    Path = pas::list_at<PathClass::TPath>(Group, i);
                    TotalPriority = static_cast<long double>(TotalPriority) + Path->Priority;
                }
                RandomPriority = pas::random_real(&System::RandSeed) * TotalPriority;
                Selected = pas::list_count(Group) - 1;
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Group) - 1); cpp_range_5.next(i); ) {
                    Path = pas::list_at<PathClass::TPath>(Group, i);
                    if (Path->Priority > RandomPriority) {
                        Selected = i;
                        break;
                    }
                    RandomPriority = static_cast<long double>(RandomPriority) - Path->Priority;
                }
                pas::list_add(Chosen, pas::list_get(Group, Selected));
            }
            pas::list_clear(Group);
        }
        if (pas::list_count(Chosen) == 0) {
            pas::free(Chosen);
            pas::free(Group);
            pas::free(Pending);
            Dialogs::ShowMessage(static_cast<pas::AnsiString>(pas::concat_wide({u"No available answers from location ", EC_Str::IntToWideString(Location->Id)})));
            return;
        }
        if (pas::list_count(Chosen) == 1) {
            Path = pas::list_at<PathClass::TPath>(Chosen, 0);
            if (EC_Str::TrimWideString(Path->Caption->Text) == u"") {
                pas::free(Chosen);
                pas::free(Group);
                pas::free(Pending);
                FollowPath(Path->Id);
                return;
            }
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, pas::list_count(Chosen) * 2); cpp_range_6.next(i); ) {
            Selected = pas::random(pas::list_count(Chosen), &System::RandSeed);
            Path = pas::list_at<PathClass::TPath>(Chosen, Selected);
            j = pas::random(pas::list_count(Chosen), &System::RandSeed);
            pas::list_put(Chosen, Selected, pas::list_get(Chosen, j));
            pas::list_put(Chosen, j, reinterpret_cast<void*>(Path));
        }
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(2, pas::list_count(Chosen)); cpp_range_7.next(i); ) {
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Chosen) - i); cpp_range_8.next(j); ) {
                Path = pas::list_at<PathClass::TPath>(Chosen, j);
                Other = pas::list_at<PathClass::TPath>(Chosen, j + 1);
                if (Other->DisplayOrder < Path->DisplayOrder) {
                    pas::list_put(Chosen, j, reinterpret_cast<void*>(Other));
                    pas::list_put(Chosen, j + 1, reinterpret_cast<void*>(Path));
                }
            }
        }
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(Chosen) - 1); cpp_range_9.next(i); ) {
            Path = pas::list_at<PathClass::TPath>(Chosen, i);
            Caption = EC_Str::TrimWideString(Path->Caption->Text);
            if (Caption != u"") {
                Caption = ExpandText(Caption, true);
                if (Path->Available) {
                    PlayerInterface->AddPathAction(Caption, Path->Id);
                } else {
                    PlayerInterface->AddDisabledPath(Caption);
                }
            }
        }
        pas::free(Chosen);
        pas::free(Group);
        pas::free(Pending);
        TextShown = false;
    }

    void TTextQuest::FollowPath(std::int32_t PathId) {
        PathClass::TPath* Path{};
        std::uint8_t Critical{};
        if (PlayerInterface != nullptr) {
            {
                std::int32_t findPathIndex = FindPathIndex(PathId);
                TTextQuest* self = this;
                Path = self->GetPath(findPathIndex);
            }
            if (TextShown && EC_Str::TrimWideString(Path->Event->Text->Text) != u"") {
                TextShown = false;
                PlayerInterface->AddPathContinueAction(PathId);
            } else {
                Path->ApplyParameterChanges(Parameters);
                if (Path->Days > 0) {
                    PlayerInterface->AdvanceDays(Path->Days);
                }
                ++Path->TraversalCount;
                ShowParameters();
                Critical = CheckCriticalParameters();
                ShowEvent(Path->Event);
                LastEventSource = pas::concat_wide({u"Path ", EC_Str::IntToWideString(Path->Id)});
                if (Critical) {
                    if (TextShown) {
                        PlayerInterface->AddContinueAction();
                    } else {
                        ShowOutcome();
                    }
                } else {
                    EnterLocation(Path->ToLocationId);
                }
            }
        }
    }

    void TTextQuest::ShowEvent(EventClass::TEvent* Event) {
        pas::WideString Text{};
        if (PlayerInterface != nullptr && Event != nullptr) {
            DisplayedEvent = Event;
            Text = EC_Str::TrimWideString(Event->Text->Text);
            if (Text != u"") {
                Text = ExpandText(Text, true);
                PlayerInterface->ShowText(Text);
                TextShown = true;
            }
            if (EC_Str::TrimWideString(Event->Picture->Text) != u"") {
                pas::WideString trimWideString = EC_Str::TrimWideString(Event->Picture->Text);
                TextQuestInterface::TTextQuestInterface* playerInterface = PlayerInterface;
                playerInterface->ShowPicture(std::move(trimWideString));
            }
            if (EC_Str::TrimWideString(Event->Music->Text) != u"") {
                pas::WideString trimWideString_2 = EC_Str::TrimWideString(Event->Music->Text);
                TextQuestInterface::TTextQuestInterface* playerInterface_2 = PlayerInterface;
                playerInterface_2->PlayMusic(std::move(trimWideString_2));
            }
            if (EC_Str::TrimWideString(Event->Sound->Text) != u"") {
                pas::WideString trimWideString_3 = EC_Str::TrimWideString(Event->Sound->Text);
                TextQuestInterface::TTextQuestInterface* playerInterface_3 = PlayerInterface;
                playerInterface_3->PlaySound(std::move(trimWideString_3));
            }
        }
    }

    void TTextQuest::ShowOutcome() {
        if (PlayerInterface != nullptr) {
            ShowEvent(OutcomeEvent);
            LastEventSource = u"Critical param value"_w;
            switch (Outcome) {
                case TextQuestInterface::qoFailure: PlayerInterface->AddFailureAction(); break;
                case TextQuestInterface::qoSuccess: PlayerInterface->AddSuccessAction(); break;
                case TextQuestInterface::qoDeath: PlayerInterface->AddDeathAction(); break;
            }
        }
    }

    void TTextQuest::ShowParameters() {
        pas::WideString ValueText{};
        pas::WideString Text{};
        std::int32_t i{};
        if (PlayerInterface != nullptr) {
            Text = pas::WideString();
            for (auto cpp_range = pas::for_to<std::int32_t>(1, GetParameterCount()); cpp_range.next(i); ) {
                if (GetParameter(i)->Enabled && static_cast<std::uint8_t>(GetParameter(i)->Hidden ^ 1) && (GetParameter(i)->Value != 0 || GetParameter(i)->ShowWhenZero)) {
                    ValueText = ([&] {
                        std::int32_t value = GetParameter(i)->Value;
                        ParameterClass::TParameter* parameter = GetParameter(i);
                        return parameter->GetValueText(value);
                    }());
                    ValueText = EC_Str::ReplaceAllWideString(ValueText, u"<>"_wref.get(), pas::view(EC_Str::IntToWideString(GetParameter(i)->Value)));
                    Text = pas::concat_wide({Text, ValueText, u"\r\n"});
                }
            }
            Text = ExpandText(Text, true);
            PlayerInterface->ShowParameters(Text);
        }
    }

    void TTextQuest::p_destroy() {
        TextQuest::TTextQuest_Destroy(this);
    }

} // namespace TextQuest
