#include "layout/fRuinsTalk.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_CacheFont.hpp"
#include "types/EC_Data.hpp"
#include "types/EC_Expression.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GI_XviD.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/MessagesSdk.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aGroup.hpp"
#include "types/aPirate.hpp"
#include "types/aWarrior.hpp"
#include "types/fHangar.hpp"
#include "types/fSaveManager.hpp"
#include "types/fShip2.hpp"
#include "types/fStarMap.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/MMSystem.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aNormalShip.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aRuins.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPanelRuins.hpp"
#include "units/fRuinsTalk.hpp"
#include "units/fSelectFace.hpp"
#include "units/fTalk.hpp"

namespace fRuinsTalk {
    #pragma pack(push, 1)
    struct TRoute {
        aPlanet::TPlanet* FromPlanet;
        aPlanet::TPlanet* ToPlanet;
        std::uint8_t GoodsIndex;
        std::uint8_t cpp_padding[3];
    };
    #pragma pack(pop)

    static_assert(sizeof(void*) != 4 || sizeof(fRuinsTalk::TRoute) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TRoute, FromPlanet) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TRoute, ToPlanet) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(fRuinsTalk::TRoute, GoodsIndex) == 8);

    std::int32_t BusinessQuoteSmallAmount{};

    std::int32_t BusinessQuoteMediumAmount{};

    std::int32_t BusinessQuoteLargeAmount{};

    std::int32_t BusinessQuoteLargeDueTurn{};

    std::int32_t BusinessQuoteMediumDueTurn{};

    std::int32_t BusinessQuoteSmallDueTurn{};

    float BusinessDepositQuoteInterestRate{};

    std::int32_t NodeExchangeHighPriorityModule{};

    std::int32_t NodeExchangeMediumPriorityModule{};

    std::int32_t NodeExchangeLowPriorityModule{};

    std::int32_t NodeExchangeHighPriorityCost{};

    std::int32_t NodeExchangeMediumPriorityCost{};

    std::int32_t NodeExchangeLowPriorityCost{};

    // Shared by allegiance changes and station service quotes.
    std::int32_t StationServiceQuoteCost{};

    // Rounded distance to the military base destination.
    std::int32_t MilitaryTravelDistance{};

    aGalaxy::TStar* InvestmentRangerCenterStar{};

    aGalaxy::TStar* InvestmentPirateBaseStar{};

    aGalaxy::TStar* InvestmentMilitaryBaseStar{};

    aGalaxy::TStar* InvestmentScienceBaseStar{};

    aGalaxy::TStar* InvestmentBusinessCenterStar{};

    aGalaxy::TStar* InvestmentMedicalBaseStar{};

    aPlanet::TPlanet* InvestmentDefensePlanet{};

    pas::Array<std::int32_t, 0, 11> InvestmentQuoteCosts{};

    std::uint8_t SelectedResearchSeries{};

    std::int32_t NearbyTradeAdviceCost{};

    std::int32_t DistantTradeAdviceCost{};

    pas::Array<std::int32_t, 0, 11> PirateProgramQuoteCosts{};

    pas::Array<std::int32_t, 0, 2> PirateChameleonQuoteCosts{};

    // Shared quote amounts are replaced when opening either banking dialog.
    aItem::TEquipment* StationImprovementItem{};

    aItem::TImprovementKind StationImprovementKind{};

    std::int32_t StationImprovementDetail{};

    // 0: station services; 1: hull bridge; higher values: custom bridge.
    std::uint8_t StationBridgeMode{};

    pas::Array<fRuinsTalk::TConstructionEquipment, 42, 49> ConstructionEquipment{};

    pas::Array<fRuinsTalk::TConstructionEquipment, 1, 5> ConstructionWeapons{};

    pas::Array<fRuinsTalk::TDominionTravelQuote, 1, 4> DominionTravelQuotes{};

    std::int32_t GetDominionRelocationCost(aGalaxy::TStar* Star) {
        std::uint8_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        return std::min<std::int64_t>(static_cast<std::int64_t>(100000000), ([&] {
            std::int32_t cpp_arg = aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman) * 2;
            std::int32_t cpp_arg_2 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman) / 2;
            pas::Extended cpp_right = aMyFunction::SeededRandomIntRange(cpp_arg_2, cpp_arg, Star->GenerationSeed + 1171 + aPlayer::GetPlayer()->DockedTo->CurrentStar->GenerationSeed);
            return System::Round(pas::real_divide(pas::real_divide((aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->DockedTo->CurrentStar->Position, Star->Position) + 6.4E+3L) * cpp_right, 6.4E+3L) * (100 - Discount), 1.0E+2L));
        }()));
    }

    std::int32_t ApplyRecentDominionOrderSurcharge(float Cost) {
        std::int32_t I{};
        pas::WideString EventType{};
        {
            const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + 60 < aGalaxy::Galaxy->CurrentTurn) {
                        break;
                    }
                    EventType = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType;
                    if (EventType == u"PlayerOrdersPirateAmbush" || EventType == u"PlayerOrdersPirateRaid" || EventType == u"PlayerOrdersPirateAssault") {
                        Cost = pas::real_min<pas::Extended>(1.0E+8L, Cost * 1.5L);
                    }
                }
            }
        }
        return aMyFunction::RoundAndTruncateToHundreds(Cost);
    }

    std::int32_t GetConstructionShopCost() {
        std::int32_t J{};
        aConst::TItemType Kind{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Hull, aConst::t_DefGenerator); cpp_range.next(Kind); ) {
            if (ConstructionEquipment[Kind].Item != nullptr && ConstructionEquipment[Kind].Source == 2) {
                Result += ConstructionEquipment[Kind].Item->Cost;
            }
        }
        for (J = 1; J <= 5; ++J) {
            if (ConstructionWeapons[J].Item != nullptr && ConstructionWeapons[J].Source == 2) {
                Result += ConstructionWeapons[J].Item->Cost;
            }
        }
        return Result;
    }

    std::int32_t GetConstructionFreeSpace() {
        std::int32_t J{};
        aConst::TItemType Kind{};
        std::int32_t Result = 0;
        for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Hull, aConst::t_DefGenerator); cpp_range.next(Kind); ) {
            if (ConstructionEquipment[Kind].Item != nullptr) {
                if (Kind == aConst::t_Hull) {
                    Result += ConstructionEquipment[Kind].Item->Weight;
                } else {
                    Result -= ConstructionEquipment[Kind].Item->Weight;
                }
            }
        }
        for (J = 1; J <= 5; ++J) {
            if (ConstructionWeapons[J].Item != nullptr) {
                Result -= ConstructionWeapons[J].Item->Weight;
            }
        }
        return Result;
    }

    void SelectConstructionItem(aItem::TEquipment* Item, std::uint8_t Source) {
        std::int32_t I{};
        if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_DefGenerator))) {
            ConstructionEquipment[Item->ItemType].Item = Item;
            ConstructionEquipment[Item->ItemType].Source = Source;
        } else if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
            I = 1;
            while (I <= 5 && ConstructionWeapons[I].Item != nullptr) {
                ++I;
            }
            if (I <= 5) {
                ConstructionWeapons[I].Item = Item;
                ConstructionWeapons[I].Source = Source;
            }
        }
    }

    pas::WideString GetStationBackgroundPath() {
        pas::WideString Result{};
        aShip::TShip* Station{};
        if (StationBridgeMode > 0) {
            Result = aPlayer::GetPlayer()->RuinsStatusText;
            if (Result == u"") {
                return GR_Main::GameDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"CustomBridges.", SysUtils::IntToStr(StationBridgeMode)})))->GetParam(u"BGI"sv);
            }
        } else {
            if (aPlayer::GetPlayer() != nullptr) {
                Station = aPlayer::GetPlayer()->DockedTo;
            } else {
                Station = nullptr;
            }
            if (Station == nullptr) {
                return pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[Globals::RuinsTalkScreen->StationType].Name, u"bg"});
            }
            if (Station->TypeNameOverrideKey != u"") {
                Result = pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), Station->TypeNameOverrideKey, u"bg"});
            } else {
                Result = pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[Station->TypeId].Name, aConst::OwnerInfo[Station->OwnerId].InternalName, u"bg"});
            }
            if (!GR_Main::CacheDataRoot->FileExistsByPath(Result)) {
                return pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[Station->TypeId].Name, u"bg"});
            }
        }
        return Result;
    }

    void ResetStationImprovement() {
        StationImprovementItem = nullptr;
        StationImprovementKind = aItem::ikAny;
        StationImprovementDetail = 0;
    }

    void TfRuinsTalk_Create(TfRuinsTalk* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->StationPanel = pas::construct_call<fPanelRuins::TfPanelRuins>(fPanelRuins::TfPanelRuins_Create);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfRuinsTalk_Destroy(TfRuinsTalk* Self) {
        if (Self->StationPanel != nullptr) {
            pas::free(Self->StationPanel);
            Self->StationPanel = nullptr;
        }
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfRuinsTalk::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        StationPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fRuinsTalk... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        GI_MessageLoop::TObjectGI* Panel = GetByName(u"MainPanel"sv);
        Panel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Panel->FindByNameRecursive(u"ImageBG2"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        Panel->FindByNameRecursive(u"ImageBG"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        LargePortraitLayout = false;
        PortraitTableVisible = false;
        if (static_cast<std::uint32_t>(GR_Main::GameScreenWidth) >= 1280 && static_cast<std::uint32_t>(GR_Main::GameScreenHeight) >= 960) {
            LargePortraitLayout = true;
            PortraitTableVisible = GlobalsV::UseTablesForGov;
        }
        GI_MessageLoop::TObjectGI* TalkPanel = Panel->FindByNameRecursive(u"PanelTalk"sv);
        std::int32_t TextExtra = std::min<std::int32_t>(std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0), 250) / 3;
        std::int32_t ChoiceExtra = TextExtra / 4 * 3;
        TextExtra = TextExtra * 3 - ChoiceExtra;
        if (GR_Main::ExtraScreenHeight < 0) {
            TalkPanel->SetPosition(ClassesImports::Point(TalkPanel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, TalkPanel->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
        } else {
            TalkPanel->SetPosition(ClassesImports::Point(TalkPanel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, TalkPanel->LocalPosition.Y));
        }
        TalkPanel->SetSize(ClassesImports::Point(TalkPanel->ClientSize.X, TalkPanel->ClientSize.Y + TextExtra + ChoiceExtra));
        GI_MessageLoop::TObjectGI* Child = TalkPanel->FirstChild;
        Child->SetPosition(ClassesImports::Point(Child->LocalPosition.X, Child->LocalPosition.Y + TextExtra));
        Child->SetSize(ClassesImports::Point(Child->ClientSize.X, Child->ClientSize.Y + ChoiceExtra));
        GI_MessageLoop::TObjectGI* AddButton = TalkPanel->FindByNameRecursive(u"UserMsgAdd"sv);
        AddButton->SetPosition(ClassesImports::Point(AddButton->LocalPosition.X, AddButton->LocalPosition.Y + TextExtra));
        GI_MessageLoop::TObjectGI* CloseButton = TalkPanel->FindByNameRecursive(u"ButFormClose"sv);
        CloseButton->SetPosition(ClassesImports::Point(CloseButton->LocalPosition.X, CloseButton->LocalPosition.Y + TextExtra + ChoiceExtra));
        GI_PanelScrollBar::TPanelScrollBarGI* TextPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(TalkPanel->FindByNameRecursive(u"TextScroll"sv));
        TextPanel->SetSize(ClassesImports::Point(TextPanel->ClientSize.X, TextPanel->ClientSize.Y + TextExtra));
        TextPanel->VerticalScrollBar->SetSize(ClassesImports::Point(TextPanel->VerticalScrollBar->ClientSize.X, TextPanel->VerticalScrollBar->ClientSize.Y + TextExtra));
        GI_MessageLoop::TObjectGI* TextLabel = TextPanel->FindByNameRecursive(u"TalkText"sv);
        TextLabel->SetSize(ClassesImports::Point(TextLabel->ClientSize.X, TextLabel->ClientSize.Y + TextExtra));
        GI_PanelScrollBar::TPanelScrollBarGI* ChoicePanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(TalkPanel->FindByNameRecursive(u"TalkPA"sv));
        ChoicePanel->SetPosition(ClassesImports::Point(ChoicePanel->LocalPosition.X, ChoicePanel->LocalPosition.Y + TextExtra));
        ChoicePanel->SetSize(ClassesImports::Point(ChoicePanel->ClientSize.X, ChoicePanel->ClientSize.Y + ChoiceExtra));
        ChoicePanel->VerticalScrollBar->SetPosition(ClassesImports::Point(ChoicePanel->VerticalScrollBar->LocalPosition.X, ChoicePanel->VerticalScrollBar->LocalPosition.Y + TextExtra));
        ChoicePanel->VerticalScrollBar->SetSize(ClassesImports::Point(ChoicePanel->VerticalScrollBar->ClientSize.X, ChoicePanel->VerticalScrollBar->ClientSize.Y + ChoiceExtra));
        GI_MessageLoop::TObjectGI* Border = ChoicePanel->NextSibling;
        Border->SetSize(ClassesImports::Point(Border->ClientSize.X, Border->ClientSize.Y + TextExtra + ChoiceExtra));
        GI_MessageLoop::TObjectGI* BottomBorder = Border->NextSibling;
        BottomBorder->SetPosition(ClassesImports::Point(BottomBorder->LocalPosition.X, BottomBorder->LocalPosition.Y + TextExtra + ChoiceExtra));
        GI_MessageLoop::TObjectGI* Separator = BottomBorder->NextSibling;
        Separator->SetPosition(ClassesImports::Point(Separator->LocalPosition.X, Separator->LocalPosition.Y + TextExtra));
        GI_MessageLoop::TObjectGI* Decoration = Separator->NextSibling;
        Decoration->SetPosition(ClassesImports::Point(Decoration->LocalPosition.X, Decoration->LocalPosition.Y + TextExtra));
        Panel->FindByNameRecursive(u"Film"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"sv))->UpCallback = pas::bind_method<&TfRuinsTalk::EndTurnClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"sv))->UpCallback = pas::bind_method<&TfRuinsTalk::ShipClicked>(this);
        GetByName(u"MainPanel"sv)->KeyDownCallback = pas::bind_method<&TfRuinsTalk::MainPanelKeyDown>(this);
        GI_GraphButton::TGraphButtonGI* Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"sv));
        Button->UpCallback = pas::bind_method<&TfRuinsTalk::AddMessageClicked>(this);
        GI_GraphButton::TGraphButtonGI* CloseFormButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButFormClose"sv));
        CloseFormButton->UpCallback = pas::bind_static_method<&TfRuinsTalk::CloseRuinsMode>(this);
    }

    void TfRuinsTalk::LayoutStationPortrait(GI_MessageLoop::TObjectGI* Panel) {
        std::int32_t I{};
        std::int32_t Bottom{};
        GI_MessageLoop::TObjectGI* Table2{};
        GI_MessageLoop::TObjectGI* Animation{};
        GI_MessageLoop::TObjectGI* HdAnimation{};
        if (Panel == nullptr) {
            return;
        }
        Panel->ReloadFromBlock();
        Panel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        std::int32_t HalfWidth = static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2;
        std::int32_t PortraitY = static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 10;
        std::int32_t TableY = PortraitY + Panel->FindByNameRecursive(u"Panel_Anim0"sv)->ClientSize.Y / 10 * 6;
        if (Panel->FindByNameRecursive(u"Table2"sv) != nullptr) {
            Bottom = TableY + System::Round(pas::real_min<pas::Extended>(Panel->FindByNameRecursive(u"Table"sv)->ClientSize.Y * 0.95L + Panel->FindByNameRecursive(u"Table"sv)->LocalPosition.Y, Panel->FindByNameRecursive(u"Table2"sv)->ClientSize.Y * 0.9L + Panel->FindByNameRecursive(u"Table2"sv)->LocalPosition.Y));
        } else {
            Bottom = TableY + System::Round(Panel->FindByNameRecursive(u"Table"sv)->ClientSize.Y * 0.95L + Panel->FindByNameRecursive(u"Table"sv)->LocalPosition.Y);
        }
        std::int32_t DeltaX = Panel->FindByNameRecursive(u"Panel_Anim0"sv)->LocalPosition.X - Panel->FindByNameRecursive(u"Panel_Anim1"sv)->LocalPosition.X;
        std::int32_t DeltaY = Panel->FindByNameRecursive(u"Panel_Anim0"sv)->ClientSize.Y - Panel->FindByNameRecursive(u"Panel_Anim1"sv)->ClientSize.Y;
        if (GR_Main::GameScreenHeight > Bottom) {
            PortraitY = PortraitY + GR_Main::GameScreenHeight - Bottom;
            TableY = TableY + GR_Main::GameScreenHeight - Bottom;
        }
        GI_MessageLoop::TObjectGI* Table = Panel->FindByNameRecursive(u"Table"sv);
        Table->SetPosition(ClassesImports::Point((HalfWidth - Table->ClientSize.X) / 2 + HalfWidth + Table->LocalPosition.X, Table->LocalPosition.Y + TableY));
        Table->SetActive(PortraitTableVisible);
        if (Panel->FindByNameRecursive(u"Table2"sv) != nullptr) {
            Table2 = Panel->FindByNameRecursive(u"Table2"sv);
            Table2->SetPosition(ClassesImports::Point((HalfWidth - Table2->ClientSize.X) / 2 + HalfWidth + Table2->LocalPosition.X, Table2->LocalPosition.Y + TableY));
            Table2->SetActive(false);
        }
        std::int32_t PortraitX = HalfWidth / 2 * 3 - Panel->FindByNameRecursive(u"Panel_Anim0"sv)->ClientSize.X / 2;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 1); cpp_range.next(I); ) {
            Animation = Panel->FindByNameRecursive(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Panel_Anim", SysUtils::IntToStr(I)}))));
            if (!PortraitTableVisible) {
                Animation->SetPosition(ClassesImports::Point(Animation->LocalPosition.X + GR_Main::ExtraScreenWidth, Animation->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            } else {
                Animation->SetPosition(ClassesImports::Point(PortraitX - I * DeltaX, PortraitY + I * DeltaY));
            }
            HdAnimation = Panel->FindByNameRecursive(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"PanelHD_Anim", SysUtils::IntToStr(I)}))));
            HdAnimation->SetPosition(ClassesImports::Point((HalfWidth - HdAnimation->ClientSize.X) / 2 + HdAnimation->LocalPosition.X + HalfWidth, HdAnimation->LocalPosition.Y + GR_Main::ExtraScreenHeight));
        }
        StationTransientControl = Panel;
        StationTransientControl->SetActive(true);
    }

    void TfRuinsTalk::HideStationTransientControl() {
        if (StationTransientControl != nullptr) {
            StationTransientControl->SetActive(false);
        }
    }

    // Native diagnostic name: TfRuinsTalk.BeforeRun.
    void TfRuinsTalk::OnOpen() {
        aGalaxyStruct::TOwnerId Owner{};
        std::uint8_t Kind{};
        std::int32_t Index{};
        EC_BlockPar::TBlockParEC* Block{};
        GI_MessageLoop::TObjectGI* Control{};
        GI_Image::TImageGI* Background{};
        GI_PanelScrollBar::TPanelScrollBarGI* Choices{};
        GI_GAI::TgaiGI* HdNormal{};
        GI_GAI::TgaiGI* HdAlternate{};
        GI_GAI::TgaiGI* Normal{};
        GI_GAI::TgaiGI* Alternate{};
        GI_GraphButton::TGraphButtonGI* CloseButton{};
        GI_Label::TLabelGI* TextLabel{};
        std::int32_t Stage = 0;
        try {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            }
            Stage = 1;
            LoadPanel->OnOpen();
            Stage = 2;
            if (ShowArrivalVideo && static_cast<std::uint8_t>(Globals::SkipVideo ^ 1)) {
                LoadPanel->SetShutterOpenFraction(1.0f);
                LoadPanel->Hide();
            }
            SavedChoiceScroll = -1;
            Stage = 3;
            StationOwner = aPlayer::GetPlayer()->DockedTo->OwnerId;
            StationType = aPlayer::GetPlayer()->DockedTo->TypeId;
            StationBridgeMode = aPlayer::GetPlayer()->RuinsMode;
            if (StationBridgeMode > 0) {
                aPlayer::GetPlayer()->CurrentPlanet = nullptr;
            }
            Stage = 4;
            MainPanel->OnOpen();
            Stage = 5;
            StationPanel->OnOpen();
            Stage = 6;
            if (aPlayer::GetPlayer()->DockedTo != fEquipmentShop::TemporaryShopStation) {
                SelectMusic();
                if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                    fEquipmentShop::RestoreTemporaryShopStock();
                }
                aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 0);
                Globals::PruneExpiredPersistentPlayerMessages();
                fEquipmentShop::BuildTemporaryShopSlotGrid();
            }
            if (aPlayer::GetPlayer()->PendingDockDialogue == 1) {
                aPlayer::GetPlayer()->PendingDockDialogue = 0;
            }
            Stage = 7;
            aGalaxy::Galaxy->ReleaseItemGraphics();
            Stage = 8;
            Background = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG2"sv));
            Background->SetActive(StationType == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase));
            if (Background->Active) {
                Background->SetImagePath(pas::concat_wide({u"GAI,", aPlayer::GetPlayer()->CurrentStar->GetBackgroundImagePath(Index)}));
                Background->GaiImageControl->LoadFrameSequenceFromText(u"[50,0-0]"_wref.get());
                Background->SetImageKindX(GI_Main::ikxCenter);
                Background->SetImageKindY(GI_Main::ikyCenter);
            }
            Stage = 9;
            {
                GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"sv));
                pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,", fRuinsTalk::GetStationBackgroundPath()});
                cpp_arg->SetImagePath(std::move(cpp_arg_2));
            }
            Stage = 10;
            I_Start();
            Stage = 11;
            RestartTextPresentation();
            Stage = 12;
            Choices = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"sv));
            Choices->SetVerticalScrollbarEnabled(false);
            Stage = 13;
            for (auto cpp_range = pas::for_to<aGalaxyStruct::TOwnerId>(aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal); cpp_range.next(Owner); ) {
                Control = FindControlByPath(pas::concat_wide({u"Panel", aConst::OwnerInfo[Owner].InternalName}));
                if (Control != nullptr) {
                    Control->SetActive(false);
                }
                for (auto cpp_range_2 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(6), static_cast<std::uint8_t>(12)); cpp_range_2.next(Kind); ) {
                    Control = FindControlByPath(pas::concat_wide({u"Panel", aConst::OwnerInfo[Owner].InternalName, aConst::ShipTypeNames[Kind].Name}));
                    if (Control != nullptr) {
                        Control->SetActive(false);
                    }
                }
            }
            Stage = 14;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, 255); cpp_range_3.next(Index); ) {
                if (StationBridgeMode != Index) {
                    Block = GR_Main::GameDataConfig->GetBlockByPath(u"CustomBridges"_wref.get());
                    if (Block->CountBlocks(pas::wide_int_to_str(Index)) != 0) {
                        Block = Block->GetBlockByPath(pas::wide_int_to_str(Index));
                        Control = FindControlByPath(Block->GetParam(u"PanelName"sv));
                        if (Control != nullptr) {
                            Control->SetActive(false);
                        }
                    }
                }
            }
            Stage = 15;
            if (StationBridgeMode != 0) {
                Control = ([&] {
                    const pas::WideString& param = GR_Main::GameDataConfig->GetBlockByPath(static_cast<pas::WideString>(pas::concat_ansi({"CustomBridges.", SysUtils::IntToStr(StationBridgeMode)})))->GetParam(u"PanelName"sv);
                    GI_MessageLoop::TMessageLoopGI* self = this;
                    return self->GetByName(pas::view(param));
                }());
            } else {
                Control = nullptr;
                if (aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey != u"") {
                    Control = FindControlByPath(pas::concat_wide({u"Panel", aConst::OwnerInfo[StationOwner].InternalName, aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey}));
                    if (Control == nullptr) {
                        Control = FindControlByPath(pas::concat_wide({u"Panel", aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey}));
                    }
                }
                if (Control == nullptr) {
                    Control = FindControlByPath(pas::concat_wide({u"Panel", aConst::OwnerInfo[StationOwner].InternalName, aConst::ShipTypeNames[StationType].Name}));
                }
                if (Control == nullptr) {
                    Control = FindControlByPath(pas::concat_wide({u"Panel", aConst::OwnerInfo[StationOwner].InternalName}));
                }
                if (Control == nullptr) {
                    Control = FindControlByPath(u"PanelPeoplePB"_wref.get());
                }
            }
            Stage = 16;
            LayoutStationPortrait(Control);
            Stage = 17;
            if (StationTransientControl->FindByNameRecursive(u"Table2"sv) != nullptr) {
                StationTransientControl->FindByNameRecursive(u"Table2"sv)->SetActive(false);
            }
            Stage = 18;
            StationTransientControl->FindByNameRecursive(u"Table"sv)->SetActive(LargePortraitLayout && PortraitTableVisible);
            Stage = 19;
            if (LargePortraitLayout && static_cast<std::uint8_t>(PortraitTableVisible ^ 1)) {
                Stage = 20;
                HdNormal = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"PanelHD_Anim0"sv));
                HdNormal->FirstFrameOnly = GlobalsV::AnimGov == 0;
                HdNormal->PrimeImageCaches();
                if (GlobalsV::AnimGov == 2) {
                    HdAlternate = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"PanelHD_Anim1"sv));
                    HdAlternate->FirstFrameOnly = GlobalsV::AnimGov == 0;
                    HdAlternate->PrimeImageCaches();
                }
            } else {
                Stage = 21;
                Normal = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"Panel_Anim0"sv));
                Normal->FirstFrameOnly = GlobalsV::AnimGov == 0;
                Normal->PrimeImageCaches();
                if (GlobalsV::AnimGov == 2) {
                    Alternate = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"Panel_Anim1"sv));
                    Alternate->FirstFrameOnly = GlobalsV::AnimGov == 0;
                    Alternate->PrimeImageCaches();
                }
            }
            Stage = 22;
            CloseButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButFormClose"sv));
            CloseButton->SetDisabled(StationBridgeMode == 0 || aPlayer::GetPlayer()->GetHull()->CapitalShip != StationBridgeMode && aPlayer::GetPlayer()->PendingDockDialogue == 2);
            Stage = 23;
            TextLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"sv));
            if (GlobalsV::FontDialog == 0) {
                TextLabel->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                TextLabel->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                TextLabel->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                TextLabel->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            Stage = 24;
            if (aScript::DispatchPendingScriptRequests()) {
                aGalaxy::Galaxy->PrimeIntegrityChecksum(180);
                return;
            }
            Stage = 25;
            MainPanel->RebuildMessageButtons(false);
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
            }
            Stage = 26;
            SelectPortraitAnimation(true);
            NextPortraitCycleAlternate = false;
            aGalaxy::Galaxy->PrimeIntegrityChecksum(180);
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfRuinsTalk.BeforeRun, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfRuinsTalk::OnClose() {
        aGalaxy::Galaxy->CheckIntegrityChecksum(181);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        StopScriptVideo(false);
        LoadPanel->OnClose();
        HideStationTransientControl();
        StationBridgeMode = 0;
        Globals::ScriptDialogIndex = -1;
        ClearChoices();
        MainPanel->OnClose();
        fPanelRuins::TfPanelRuins::OnClose();
        ResearchItemVisited.set_length(0);
        ResearchItemIndexes.set_length(0);
    }

    // Returns inventory count; unused sorted slots are -1.
    std::int32_t TfRuinsTalk::SortResearchItems(std::uint8_t Series) {
        std::int32_t I{};
        std::int32_t BestIndex{};
        aItem::TEquipment* Item{};
        TResearchItemSortKey BestKey{};
        TResearchItemSortKey Key{};
        auto ClearResearchItemSortKey = [&](TResearchItemSortKey& Key) -> void {
            Key.Cost = 0;
            Key.Priority = 0;
            Key.Weight = 0;
        };
        auto MakeResearchItemSortKey = [&](aItem::TEquipment* Item) -> TResearchItemSortKey {
            TResearchItemSortKey Result{};
            Result.Cost = Item->Cost;
            Result.Weight = Item->Weight;
            Result.Priority = 0;
            if (Item->OwnerId == aGalaxyStruct::oiDominator && Item->EquippedFlag == 0 && Item->NoDropFlag == 0 && Item->CustomFaction == u"" && !(pas::class_cast_if<aItem::THull*>(Item) != nullptr)) {
                if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                    if (static_cast<aGalaxyStruct::TDominatorSeries>(Series) == Item->DominatorSeries) {
                        Result.Priority = 5;
                    } else {
                        Result.Priority = 4;
                    }
                } else if (pas::class_cast_if<aItem::TCountableItem*>(Item) != nullptr) {
                    Result.Priority = 2;
                } else if (pas::class_cast_if<aItem::TMicroModule*>(Item) != nullptr) {
                    Result.Priority = 1;
                } else {
                    Result.Priority = 3;
                }
            }
            return Result;
        };
        auto CompareResearchItemSortKeys = [&](TResearchItemSortKey Left, TResearchItemSortKey Right) -> std::int32_t {
            std::int32_t Result = 0;
            if (Right.Priority > Left.Priority) {
                return 1;
            } else if (Right.Priority < Left.Priority) {
                return -1;
            } else if (Right.Cost > Left.Cost) {
                return 1;
            } else if (Right.Cost < Left.Cost) {
                return -1;
            } else if (Right.Weight > Left.Weight) {
                return 1;
            } else if (Right.Weight < Left.Weight) {
                return -1;
            } else {
                return Result;
            }
        };
        std::int32_t Count = pas::list_count(aPlayer::GetPlayer()->Inventory);
        ResearchItemVisited.set_length(Count);
        ResearchItemIndexes.set_length(Count);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            ResearchItemVisited[I] = false;
            ResearchItemIndexes[I] = -1;
        }
        std::int32_t SortedCount = 0;
        do {
            BestIndex = -1;
            ClearResearchItemSortKey(BestKey);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(I); ) {
                if (!ResearchItemVisited[I]) {
                    Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                    Key = MakeResearchItemSortKey(Item);
                    if (Key.Priority == 0) {
                        ResearchItemVisited[I] = true;
                    } else if (CompareResearchItemSortKeys(BestKey, Key) >= 0) {
                        BestKey = Key;
                        BestIndex = I;
                    }
                }
            }
            if (BestIndex >= 0) {
                ResearchItemIndexes[SortedCount] = BestIndex;
                ResearchItemVisited[BestIndex] = true;
                ++SortedCount;
            }
        } while (!(BestIndex < 0));
        return Count;
    }

    std::uint8_t TfRuinsTalk::IsResearchItemQuestLetter(aItem::TItem* Item) {
        std::int32_t I{};
        aRanger::PQuest Quest{};
        std::uint8_t Result = false;
        if (pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr && Item->ScriptItem == nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Quests) - 1); cpp_range.next(I); ) {
                Quest = pas::list_at<aRanger::TQuest>(aPlayer::GetPlayer()->Quests, I);
                if (Quest->QuestType == aGalaxyStruct::qtSendLetter) {
                    if (([&] {
                        pas::WideString cpp_string = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"Quest.SendLetter.", SysUtils::IntToStr(Quest->QuestNumber), ".SysName"})));
                        const pas::WideString& cpp_string_ref = pas::checked_cast<aItem::TUselessItem*>(Item)->ConfigBlockName;
                        return cpp_string == cpp_string_ref;
                    }())) {
                        Result = true;
                    }
                }
            }
        }
        return Result;
    }

    std::int32_t TfRuinsTalk::CountResearchRemains(std::uint8_t Series, std::int32_t Count) {
        std::int32_t I{};
        std::int32_t Index{};
        aItem::TEquipment* Item{};
        std::int32_t Matches = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Index = ResearchItemIndexes[I];
            if (Index < 0) {
                break;
            }
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, Index);
            if (Item->DominatorSeries == static_cast<aGalaxyStruct::TDominatorSeries>(Series) && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                if (static_cast<aItem::TUselessItem*>(Item)->IsDominatorRemains() && static_cast<std::uint8_t>(TfRuinsTalk::IsResearchItemQuestLetter(Item) ^ 1)) {
                    ++Matches;
                }
            }
        }
        return Matches;
    }

    std::int32_t TfRuinsTalk::CountResearchEquipment(std::int32_t Count) {
        std::int32_t I{};
        std::int32_t Index{};
        aItem::TEquipment* Item{};
        std::int32_t Matches = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Index = ResearchItemIndexes[I];
            if (Index < 0) {
                break;
            }
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, Index);
            if (!(pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) && Item->EquippedFlag == 0 && Item->ItemType != aConst::t_Protoplasm && Item->ItemType != aConst::t_MicroModule) {
                ++Matches;
            }
        }
        return Matches;
    }

    void TfRuinsTalk::BuildResearchItemChoices(std::uint8_t Series, pas::WideString& Text) {
        std::int32_t I{};
        std::int32_t Index{};
        aItem::TEquipment* Item{};
        pas::WideString Items{};
        pas::WideString Description{};
        pas::WideString Bonus{};
        Bonus = pas::concat_wide({u" ", aMyFunction::WrapTextInColor(pas::view(GR_Main::LookupLocalizedTextOrEmpty(u"FormRuins.SB.Scn.ItemsCool"_wref.get())), u"<color=255,240,100>"sv)});
        std::int32_t Number = 0;
        std::int32_t Count = SortResearchItems(Series);
        if (CountResearchRemains(Series, Count) > 0) {
            switch (Series) {
                case 0: {
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleAllUselessBlazer"_wref.get())}), Count, pas::bind_method<&TfRuinsTalk::SellResearchRemains>(this));
                    break;
                }
                case 1: {
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleAllUselessKeller"_wref.get())}), Count, pas::bind_method<&TfRuinsTalk::SellResearchRemains>(this));
                    break;
                }
                case 2: {
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleAllUselessTerron"_wref.get())}), Count, pas::bind_method<&TfRuinsTalk::SellResearchRemains>(this));
                    break;
                }
            }
        }
        if (CountResearchEquipment(Count) > 1) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleAllEq"_wref.get())}), Count, pas::bind_method<&TfRuinsTalk::SellResearchEquipment>(this));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            Index = ResearchItemIndexes[I];
            if (Index < 0) {
                break;
            }
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, Index);
            ++Number;
            Description = pas::concat_wide({aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Item->GetDisplayName())), u" (", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Item->Cost)), u"<color=255,240,100>"sv), u" cr)"});
            if (Item->DominatorSeries == static_cast<aGalaxyStruct::TDominatorSeries>(Series) && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                Description = pas::concat_wide({Description, Bonus});
            }
            Items = pas::concat_wide({Items, u"\r\n", pas::wide_int_to_str(Number), u") ", Description});
            AddChoice(pas::concat_wide({u"- ", aMyFunction::FormatText1(aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSale"_wref.get()), pas::WideString(), u"<ItemName>"_w, Description)}), Index, pas::bind_method<&TfRuinsTalk::SellResearchItem>(this));
        }
        Text = std::move(Items);
    }

    void TfRuinsTalk::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->PendingDockDialogue > 1 || StationBridgeMode > 0 || aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion) && aPlayer::GetPlayer()->DockedTo->Order == aShip::soTeleport && static_cast<std::uint32_t>(aPlayer::GetPlayer()->DockedTo->OrderStateData) > 0 && static_cast<std::uint8_t>(aPlayer::GetPlayer()->DockedTo->InHyperspace ^ 1)) {
            Globals::RuinsTalkScreen->DepartWithStation(1);
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstDominion) && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate <= aGalaxy::Galaxy->CurrentTurn) {
            Globals::RuinsTalkScreen->DepartWithStation(1);
            return;
        }
        if (aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase) && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate <= aGalaxy::Galaxy->CurrentTurn) {
            if (aPlayer::GetPlayer()->Speed <= 0) {
                Globals::RuinsTalkScreen->DepartWithStation(1);
            } else {
                StationPanel->TakeOffForStationTravel();
            }
            return;
        }
        if (LoadPanel->IsAnimatingShutters()) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(182);
        fEquipmentShop::RestoreTemporaryShopStock();
        MainPanel->EndTurnClicked(Sender);
        if (ExitCode == 0) {
            fEquipmentShop::BuildTemporaryShopSlotGrid();
            I_Start();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(183);
            RestartTextPresentation();
            MainPanel->RebuildMessageButtons(false);
        }
    }

    void TfRuinsTalk::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (LoadPanel->IsAnimatingShutters()) {
            return;
        }
        MainPanel->ShipClicked(Sender);
        if (Globals::ShipScreen->ShipStateChanged) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(300);
            I_Start();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(301);
            RestartTextPresentation();
            MainPanel->RebuildMessageButtons(false);
        }
    }

    void TfRuinsTalk::RememberChoiceScroll() {
        SavedChoiceScroll = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"sv))->VerticalScrollBar->Position;
    }

    // Native diagnostic name: TfRuinsTalk.A_Start.
    void TfRuinsTalk::ClearChoices() {
        GI_MessageLoop::TObjectGI* Child{};
        GI_MessageLoop::TObjectGI* Panel{};
        std::int32_t Stage = 0;
        try {
            ChoiceHeight = 0;
            Panel = GetByName(u"TalkPA"sv);
            Stage = 1;
            Child = Panel->FirstChild;
            while (Child != nullptr) {
                Stage = 2;
                pas::free(reinterpret_cast<pas::Object*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Child->UserValue))));
                Stage = 3;
                Child = Child->NextSibling;
                Stage = 4;
            }
            Stage = 5;
            Panel->FreeOwnedChildren();
            Stage = 6;
            Panel->Invalidate();
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfRuinsTalk.A_Start, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    // Text filters may disable or suppress the choice. Invokes the method callback with Value in EDX.
    void TfRuinsTalk::AddChoice(pas::WideString Text, std::int32_t Value, GI_MessageLoop::TDialogChoiceEventGI Callback) {
        std::int32_t I{};
        std::uint8_t BlockMode = 0;
        if (aScript::ScriptDialogBlocks != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogBlocks) - 1); cpp_range.next(I); ) {
                if (EC_Str::FindTextOffsetW(Text, pas::list_at<aScript::TDialogBlock>(aScript::ScriptDialogBlocks, I)->Text, 0) >= 0) {
                    BlockMode = std::max<std::int32_t>(static_cast<std::int32_t>(BlockMode), static_cast<std::int32_t>(pas::list_at<aScript::TDialogBlock>(aScript::ScriptDialogBlocks, I)->Mode));
                }
            }
        }
        if (BlockMode >= 2) {
            return;
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"sv));
        I = 0;
        while (I < Text.length()) {
            if (Text.read(I + 1) != u'-' && Text.read(I + 1) != u' ') {
                break;
            }
            ++I;
        }
        if (I > 0) {
            Text = pas::copy(Text, I + 1, Text.length() - I);
        }
        fTalk::TfTalkA* Choice = pas::construct_call<fTalk::TfTalkA>(fTalk::TfTalkA_Create);
        Choice->Callback = Callback;
        Choice->Value = Value;
        if (BlockMode > 0) {
            Choice->Callback = nullptr;
        }
        GI_Panel::TPanelGI* Row = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Panel);
        Row->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Choice));
        Row->SetPosition(ClassesImports::Point(0, ChoiceHeight));
        Row->SetSize(ClassesImports::Point(Panel->ClientSize.X, 20));
        Row->SetPositionModeW(true);
        Row->MouseEnterCallback = pas::bind_static_method<&TfRuinsTalk::ChoiceMouseEnter>(this);
        Row->MouseLeaveCallback = pas::bind_static_method<&TfRuinsTalk::ChoiceMouseLeave>(this);
        Row->LeftButtonDownCallback = pas::bind_static_method<&TfRuinsTalk::ChoiceMouseDown>(this);
        Row->LeftButtonUpCallback = pas::bind_method<&TfRuinsTalk::ChoiceMouseUp>(this);
        GI_Image::TImageGI* Highlight = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Row);
        Highlight->SetDepth(3.0);
        Highlight->SetPosition(ClassesImports::Point(0, 0));
        Highlight->SetSize(ClassesImports::Point(Panel->ClientSize.X, 20));
        Highlight->SetImagePath(pas::concat_wide({u"GI,Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"Line"}));
        Highlight->SetImageKindX(GI_Main::ikxLeftFill);
        Highlight->SetImageKindY(GI_Main::ikyTopFill);
        Highlight->SetActive(false);
        {
            GI_Label::TLabelGI* cpp_with = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Row);
            if (GlobalsV::FontDialog == 0) {
                cpp_with->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                cpp_with->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                cpp_with->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                cpp_with->SetFontName(GlobalsV::SmoothIntroFontName);
            }
            cpp_with->SetSize(ClassesImports::Point(Panel->ClientSize.X - GR_Main::GiScalePixels(20), 1));
            cpp_with->SetPosition(ClassesImports::Point(GR_Main::GiScalePixels(10), 0));
            cpp_with->SetWordWrapEnabled(true);
            cpp_with->SetTextAlignX(GI_Main::taxLeft);
            cpp_with->SetTextAlignY(GI_Main::tayAuto);
            if (!pas::assigned(Callback)) {
                Text = EC_Str::RemoveTextTagsW(Text);
            }
            cpp_with->SetText(pas::concat_wide({u"<Object=0,20,14,0>", EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv)}));
            cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
            if (!pas::assigned(Choice->Callback)) {
                cpp_with->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(127, 127, 127));
            }
            cpp_with->CreateEmbeddedControl = pas::bind_static_method<&TfRuinsTalk::CreateChoiceBullet>(this);
            cpp_with->SetTextAlignY(GI_Main::tayCenterEx);
            Row->SetSize(ClassesImports::Point(Row->ClientSize.X, cpp_with->ClientSize.Y + 2 * GR_Main::GiScalePixelsEx(2, 2)));
            cpp_with->SetSize(ClassesImports::Point(cpp_with->ClientSize.X, Row->ClientSize.Y));
            Highlight->SetSize(Row->ClientSize);
            ChoiceHeight += cpp_with->ClientSize.Y;
        }
    }

    void TfRuinsTalk::ChoiceMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(true);
    }

    void TfRuinsTalk::ChoiceMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        Sender->FirstChild->SetActive(false);
    }

    void TfRuinsTalk::ChoiceMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(2, 0));
        }
    }

    void TfRuinsTalk::ChoiceMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->FirstChild != nullptr && Sender->FirstChild->NextSibling != nullptr && Sender->FirstChild->NextSibling->FirstChild != nullptr && Sender->FirstChild->NextSibling->FirstChild->FirstChild != nullptr) {
            Sender->FirstChild->NextSibling->FirstChild->FirstChild->SetPosition(ClassesImports::Point(0, 0));
        }
        if (LoadPanel->IsAnimatingShutters()) {
            return;
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(184);
        fTalk::TfTalkA* Choice = reinterpret_cast<fTalk::TfTalkA*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        if (pas::assigned(Choice->Callback)) {
            Choice->Callback(Choice->Value);
        } else if (pas::assigned(Choice->FallbackCallback)) {
            Choice->FallbackCallback(Choice->FallbackText);
        } else {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(186);
            return;
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(185);
        RestartTextPresentation();
        MainPanel->RefreshMoneyAndCargo();
        MainPanel->RebuildMessageButtons(false);
        GI_Main::BreakUiMessage();
    }

    // Clears the dialogue panel state and restarts its ten-millisecond presentation timer.
    void TfRuinsTalk::RestartTextPresentation() {
        ResetPortraitCycle();
        pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"sv))->SetActive(false);
        PresentedTextLength = 0;
        if (TextPresentationTimer != nullptr) {
            CancelCallbackTimer(TextPresentationTimer);
            TextPresentationTimer = nullptr;
        }
        TextPresentationTimer = ScheduleCallbackTimer(10, 10, pas::bind_method<&TfRuinsTalk::AdvanceTextPresentation>(this), 0);
    }

    void TfRuinsTalk::AdvanceTextPresentation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        GI_PanelScrollBar::TPanelScrollBarGI* Choices{};
        GI_PanelScrollBar::TPanelScrollBarGI* TextPanel{};
        if (PresentedTextLength >= DialogText.length()) {
            Choices = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"sv));
            Choices->SetActive(true);
            {
                std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"sv))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar = Choices->VerticalScrollBar;
                verticalScrollBar->SetSmallChange(lineHeight);
            }
            Choices->VerticalScrollBar->SetLargeChange(Choices->ClientSize.Y);
            Choices->VerticalScrollBar->SetPageSize(Choices->ClientSize.Y);
            Choices->SetScrollOffset(ClassesImports::Point(0, 0));
            Choices->VerticalScrollBar->SetActive(ChoiceHeight > Choices->ClientSize.Y);
            Choices->VerticalScrollBar->SetDepth(4.0);
            Choices->SetDragScrollingEnabled(Choices->VerticalScrollBar->Active);
            Choices->UpdateScrollRanges();
            if (TextPresentationTimer != nullptr) {
                CancelCallbackTimer(TextPresentationTimer);
                TextPresentationTimer = nullptr;
            }
            if (SavedChoiceScroll >= 0) {
                Choices->VerticalScrollBar->SetPosition_2(SavedChoiceScroll);
            }
            SavedChoiceScroll = -1;
            GR_Main::PostMouseMoveMessage();
        } else {
            DialogText = pas::concat_wide_reverse({EC_Str::TrimWideString(DialogText), aConst::LocalizedTextLinePrefix});
            DialogText = EC_Str::ReplaceAllWideString(DialogText, pas::concat_wide({u"\r\n", aConst::LocalizedTextLinePrefix}), u"\r\n"sv);
            DialogText = EC_Str::ReplaceAllWideString(DialogText, u"\r\n"_wref.get(), pas::view(pas::concat_wide({u"\r\n", aConst::LocalizedTextLinePrefix})));
            PresentedTextLength = DialogText.length();
            DialogText = EC_Str::ReplaceAllWideString(DialogText, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"sv))->SetText(DialogText);
            TextPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"sv));
            TextPanel->SetScrollOffset(ClassesImports::Point(0, 0));
            TextPanel->UpdateScrollRanges();
            TextPanel->VerticalScrollBar->SetActive(pas::checked_cast<GI_Label::TLabelGI*>(TextPanel->FindByNameRecursive(u"TalkText"sv))->ClientSize.Y > TextPanel->ClientSize.Y);
            {
                std::int32_t lineHeight_2 = pas::checked_cast<GI_Label::TLabelGI*>(TextPanel->FindByNameRecursive(u"TalkText"sv))->GetLineHeight();
                GI_ScrollBar::TScrollBarGI* verticalScrollBar_2 = TextPanel->VerticalScrollBar;
                verticalScrollBar_2->SetSmallChange(lineHeight_2);
            }
            TextPanel->VerticalScrollBar->SetLargeChange(TextPanel->ClientSize.Y);
            TextPanel->VerticalScrollBar->SetPageSize(TextPanel->ClientSize.Y);
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"sv))->SetDisabled(false);
        }
    }

    void TfRuinsTalk::ResetPortraitCycle() {
        NextPortraitCycleAlternate = true;
    }

    void TfRuinsTalk::PortraitCycleComplete(GI_MessageLoop::TObjectGI* Sender) {
        if (NextPortraitCycleAlternate) {
            SelectPortraitAnimation(true);
            NextPortraitCycleAlternate = false;
        } else {
            SelectPortraitAnimation(false);
        }
    }

    void TfRuinsTalk::SelectPortraitAnimation(std::uint8_t Alternate) {
        GI_GAI::TgaiGI* HdNormal{};
        GI_GAI::TgaiGI* HdAlternate{};
        GI_GAI::TgaiGI* Normal{};
        GI_GAI::TgaiGI* AlternateAnimation{};
        if (GlobalsV::AnimGov != 2) {
            Alternate = false;
        }
        if (LargePortraitLayout && static_cast<std::uint8_t>(PortraitTableVisible ^ 1)) {
            HdNormal = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"PanelHD_Anim0"sv));
            HdNormal->CycleCompleteCallback = pas::bind_method<&TfRuinsTalk::PortraitCycleComplete>(this);
            HdNormal->SetSequenceFrame(0);
            HdNormal->StopAutoPlayback();
            if (!Alternate) {
                HdNormal->RestartPlayback();
            } else {
                HdNormal->StopAutoPlayback();
            }
            HdNormal->SetActive(static_cast<std::uint8_t>(Alternate ^ 1));
            HdAlternate = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"PanelHD_Anim1"sv));
            HdAlternate->CycleCompleteCallback = pas::bind_method<&TfRuinsTalk::PortraitCycleComplete>(this);
            HdAlternate->SetSequenceFrame(0);
            HdAlternate->StopAutoPlayback();
            if (Alternate) {
                HdAlternate->RestartPlayback();
            } else {
                HdAlternate->StopAutoPlayback();
            }
            HdAlternate->SetActive(Alternate);
        } else {
            Normal = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"Panel_Anim0"sv));
            Normal->CycleCompleteCallback = pas::bind_method<&TfRuinsTalk::PortraitCycleComplete>(this);
            Normal->SetSequenceFrame(0);
            Normal->StopAutoPlayback();
            if (!Alternate) {
                Normal->RestartPlayback();
            } else {
                Normal->StopAutoPlayback();
            }
            Normal->SetActive(static_cast<std::uint8_t>(Alternate ^ 1));
            AlternateAnimation = pas::checked_cast<GI_GAI::TgaiGI*>(StationTransientControl->FindByNameRecursive(u"Panel_Anim1"sv));
            AlternateAnimation->CycleCompleteCallback = pas::bind_method<&TfRuinsTalk::PortraitCycleComplete>(this);
            AlternateAnimation->SetSequenceFrame(0);
            AlternateAnimation->StopAutoPlayback();
            if (Alternate) {
                AlternateAnimation->RestartPlayback();
            } else {
                AlternateAnimation->StopAutoPlayback();
            }
            AlternateAnimation->SetActive(Alternate);
        }
    }

    GI_MessageLoop::TObjectGI* TfRuinsTalk::CreateChoiceBullet(GI_Label::TLabelGI* LabelControl, EC_CacheFont::PFontObjectEC Item) {
        GI_MessageLoop::TObjectGI* Result = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, LabelControl);
        GI_Image::TImageGI* Image = pas::checked_cast<GI_Image::TImageGI*>(Result);
        Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormGov2.", GR_Main::GiResourceSuffix(), u"Answer"}));
        Image->SetImageKindX(GI_Main::ikxLeft);
        return Result;
    }

    void TfRuinsTalk::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TalkPA"sv));
        if (!Panel->ContainsPoint(Point)) {
            Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"sv));
        }
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->SmallChange);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->SmallChange);
        }
    }

    void TfRuinsTalk::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        GI_GraphButton::TGraphButtonGI* Button{};
        if (ExitCode != 0 || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) || LoadPanel->IsAnimatingShutters()) {
            return;
        }
        GI_PanelScrollBar::TPanelScrollBarGI* Panel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"TextScroll"sv));
        if (Key == WindowsSdk::VK_SPACE) {
            if (GetByName(u"PM_EndTurn"sv)->Active) {
                EndTurnClicked(nullptr);
            }
        } else if (Key == 'S') {
            ShipClicked(nullptr);
        } else if (Key == WindowsSdk::VK_UP) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->SmallChange);
        } else if (Key == WindowsSdk::VK_DOWN) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->SmallChange);
        } else if (Key == WindowsSdk::VK_PRIOR) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position - Panel->VerticalScrollBar->LargeChange);
        } else if (Key == WindowsSdk::VK_NEXT) {
            Panel->VerticalScrollBar->SetPosition_2(Panel->VerticalScrollBar->Position + Panel->VerticalScrollBar->LargeChange);
        } else if (Key == WindowsSdk::VK_INSERT) {
            Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"sv));
            AddMessageClicked(Button);
        } else {
            MainPanel->ProcessKeyDown(Key);
            StationPanel->ProcessKeyDown(Key);
        }
    }

    void TfRuinsTalk::AddMessageClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Text{};
        Text = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"TalkText"sv))->GetText();
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=0,50,200>"_wref.get(), u"<color=255,240,100>"sv);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Sender)->SetDisabled(true);
        GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
        Globals::AddOrUpdatePlayerBubble(7, aGalaxy::Galaxy->CurrentTurn, Text, u""_wref.get());
        MainPanel->RebuildMessageButtons(false);
        GI_Main::BreakUiMessage();
    }

    // Advances Film over the native 138-second interval.
    void TfRuinsTalk::AdvanceScriptVideo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::uint32_t cpp_left = MMSystem::timeGetTime();
        double Progress = pas::real_divide(cpp_left - ScriptVideoStartedAt, 1.38E+5L);
        if (Progress > 1.0L) {
            Progress = 1.0;
        }
        GI_XviD::TxvidGI* Film = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"sv));
        Film->SetFramePosition(System::Round(3449.0L * Progress));
        if (Progress >= 1.0L) {
            StopScriptVideo(false);
        }
    }

    std::uint8_t TfRuinsTalk::StopScriptVideo(std::uint8_t Unused) {
        std::uint8_t Result = ScriptVideoTimer != nullptr;
        if (GlobalsV::MusicEnabled && Result) {
            GR_Main::MusicManager->StopImmediately();
            while (GR_Main::MusicManager->IsPlaying()) {
                SysUtilsImports::Sleep(1u);
            }
        }
        if (ScriptVideoTimer != nullptr) {
            CancelCallbackTimer(ScriptVideoTimer);
            ScriptVideoTimer = nullptr;
        }
        GI_XviD::TxvidGI* Film = pas::checked_cast<GI_XviD::TxvidGI*>(GetByName(u"Film"sv));
        Film->ImageClose();
        Film->SetActive(false);
        InvalidateViewport();
        if (Result && pas::list_count(aScript::QueuedVideos) > 0) {
            aScript::CompleteQueuedVideo(2);
        }
        return Result;
    }

    void TfRuinsTalk::ProcessWindowMessage(std::uint32_t Message, std::uint32_t WParam, std::int32_t LParam) {
        if (Message != MessagesSdk::WM_LBUTTONDOWN && Message != MessagesSdk::WM_RBUTTONDOWN && Message != MessagesSdk::WM_MBUTTONDOWN && Message != MessagesSdk::WM_KEYDOWN || static_cast<std::uint8_t>(StopScriptVideo(false) ^ 1)) {
            GI_MessageLoop::TMessageLoopGI::ProcessWindowMessage(Message, WParam, LParam);
        }
    }

    void TfRuinsTalk::SelectMusic() {
        pas::WideString Name{};
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
        if (pas::in_range(aPlayer::GetPlayer()->DockedTo->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
            if (aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 40) {
                Globals::StarMapScreen->BattleMusicSelected = true;
                GR_Main::MusicManager->PlayCategory(u"Destroyer"_wref.get());
            } else {
                Globals::StarMapScreen->BattleMusicSelected = false;
                Name = aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey;
                if (Name != u"") {
                    if (GR_Main::MainDataConfig->GetBlock(u"Music"sv)->CountBlocks(Name) > 0) {
                        GR_Main::MusicManager->PlayCategory(Name);
                    } else if (([&] {
                        const pas::WideString& typeNameKey = aPlayer::GetPlayer()->DockedTo->GetTypeNameKey();
                        EC_BlockPar::TBlockParEC* block = GR_Main::MainDataConfig->GetBlock(u"Music"sv);
                        return block->CountBlocks(typeNameKey);
                    }()) > 0) {
                        GR_Main::MusicManager->PlayCategory(aPlayer::GetPlayer()->DockedTo->GetTypeNameKey());
                    } else {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({u"No music found for custom ", Name, u", or for base type ", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey()})));
                    }
                } else {
                    GR_Main::MusicManager->PlayCategory(aPlayer::GetPlayer()->DockedTo->GetTypeNameKey());
                }
            }
        } else {
            if (aPlayer::GetPlayer()->CurrentPlanet == nullptr) {
                GR_Main::MusicManager->RequestFadeOut();
                return;
            }
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId)].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                }
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
            }
        }
    }

    // Rebuilds the station dialogue root; receiver-only entry verified at native prologue and CheatNextRank.
    void TfRuinsTalk::I_Start() {
        std::int32_t Stage = 0;
        try {
            if (aCalc::IsTurnCalculationRunningUI()) {
                aCalc::WaitForTurnCalculationUI();
            }
            Stage = 1;
            if (ShowArrivalVideo && StationType == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase)) {
                Stage = 2;
                ShowMilitaryBaseArrivalDialog(0);
            } else {
                Stage = 3;
                M_Main(false);
            }
            Stage = 4;
            ShowArrivalVideo = false;
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfRuinsTalk.I_Start, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    std::uint8_t TfRuinsTalk::ShowDominatorVictoryDialog() {
        Globals::TMessagePlayer* MessageEntry{};
        std::int32_t Variant{};
        std::uint8_t Result = false;
        if (aGalaxy::Galaxy->TerronSeriesResolvedTurn != 0 || aGalaxy::Galaxy->KellerSeriesResolvedTurn != 0 || aGalaxy::Galaxy->BlazerSeriesResolvedTurn != 0) {
            MessageEntry = Globals::FindPlayerBubbleByKey(u"BlazerWin"_wref.get(), false);
            if (MessageEntry != nullptr && MessageEntry->Kind == 3) {
                MessageEntry->Kind = 4;
                MessageEntry->WasRead = false;
                Result = true;
                if (aKling::BlazerShip == nullptr && aGalaxy::Galaxy->BlazerSelfDestructTurn != 0) {
                    Variant = 1;
                } else if (aGalaxy::Galaxy->BlazerLandingPlanetId != 0) {
                    Variant = 2;
                } else {
                    Variant = 3;
                }
                DialogText = ([&] {
                    pas::WideString formatGameTurnDate = aGalaxy::FormatGameTurnDate(aGalaxy::Galaxy->BlazerSeriesResolvedTurn);
                    pas::WideString localizedColorText = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"FormRuinsRC.Win.Blazer", SysUtils::IntToStr(Variant)})));
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText), u"<Date>"_w, std::move(formatGameTurnDate), u"<color=255,240,100>"_w);
                }());
                if (Variant == 2 && aKling::BlazerShip != nullptr && aKling::BlazerShip->CurrentPlanet != nullptr) {
                    DialogText = aMyFunction::ReplaceColoredToken(DialogText, u"<Planet>"_w, aKling::BlazerShip->CurrentPlanet->Name, u"<color=255,240,100>"_w);
                }
                DialogText = pas::concat_wide({DialogText, u"\r\n", u" ", u"\r\n", ([&] {
                    pas::WideString awardRandomMedal = aPlayer::GetPlayer()->AwardRandomMedal();
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuinsRC.Win.Reward"_wref.get());
                    return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_2), u"<Reward>"_w, std::move(awardRandomMedal), u"<color=255,240,100>"_w);
                }())});
                ClearChoices();
                AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsRC.Continue"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ContinueDominatorVictoryDialog>(this));
            } else {
                MessageEntry = Globals::FindPlayerBubbleByKey(u"KellerWin"_wref.get(), false);
                if (MessageEntry != nullptr && MessageEntry->Kind == 3) {
                    MessageEntry->Kind = 4;
                    MessageEntry->WasRead = false;
                    Result = true;
                    if (aKling::KellerShip == nullptr) {
                        Variant = 1;
                    } else {
                        Variant = 2;
                    }
                    DialogText = ([&] {
                        pas::WideString formatGameTurnDate_2 = aGalaxy::FormatGameTurnDate(aGalaxy::Galaxy->KellerSeriesResolvedTurn);
                        pas::WideString localizedColorText_3 = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"FormRuinsRC.Win.Keller", SysUtils::IntToStr(Variant)})));
                        return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_3), u"<Date>"_w, std::move(formatGameTurnDate_2), u"<color=255,240,100>"_w);
                    }());
                    DialogText = pas::concat_wide({DialogText, u"\r\n", u" ", u"\r\n", ([&] {
                        pas::WideString awardRandomMedal_2 = aPlayer::GetPlayer()->AwardRandomMedal();
                        pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormRuinsRC.Win.Reward"_wref.get());
                        return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_4), u"<Reward>"_w, std::move(awardRandomMedal_2), u"<color=255,240,100>"_w);
                    }())});
                    ClearChoices();
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsRC.Continue"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ContinueDominatorVictoryDialog>(this));
                } else {
                    MessageEntry = Globals::FindPlayerBubbleByKey(u"TerronWin"_wref.get(), false);
                    if (MessageEntry != nullptr && MessageEntry->Kind == 3) {
                        MessageEntry->Kind = 4;
                        MessageEntry->WasRead = false;
                        Result = true;
                        if (aGalaxy::Galaxy->TerronToStarTurn != 0) {
                            Variant = 1;
                        } else if (aGalaxy::Galaxy->TerronWeaponLockTurn != 0) {
                            Variant = 2;
                        } else if (aGalaxy::Galaxy->TerronGrowLockTurn != 0) {
                            Variant = 3;
                        } else if (aGalaxy::Galaxy->TerronLandingLockTurn != 0) {
                            Variant = 4;
                        } else if (aKling::TerronShip == nullptr) {
                            Variant = 5;
                        } else {
                            Variant = 1;
                            GR_Main::RaiseWideMessage(u"Terron status"_wref.get());
                        }
                        DialogText = ([&] {
                            pas::WideString formatGameTurnDate_3 = aGalaxy::FormatGameTurnDate(aGalaxy::Galaxy->TerronSeriesResolvedTurn);
                            pas::WideString localizedColorText_5 = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"FormRuinsRC.Win.Terron", SysUtils::IntToStr(Variant)})));
                            return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_5), u"<Date>"_w, std::move(formatGameTurnDate_3), u"<color=255,240,100>"_w);
                        }());
                        if (Variant == 1 && aKling::TerronShip != nullptr && aKling::TerronShip->CurrentStar != nullptr) {
                            DialogText = aMyFunction::ReplaceColoredToken(DialogText, u"<Star>"_w, aKling::TerronShip->CurrentStar->Name, u"<color=255,240,100>"_w);
                        }
                        DialogText = pas::concat_wide({DialogText, u"\r\n", u" ", u"\r\n", ([&] {
                            pas::WideString awardRandomMedal_3 = aPlayer::GetPlayer()->AwardRandomMedal();
                            pas::WideString localizedColorText_6 = aConst::LocalizedColorText(u"FormRuinsRC.Win.Reward"_wref.get());
                            return aMyFunction::ReplaceColoredToken(std::move(localizedColorText_6), u"<Reward>"_w, std::move(awardRandomMedal_3), u"<color=255,240,100>"_w);
                        }())});
                        ClearChoices();
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsRC.Continue"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ContinueDominatorVictoryDialog>(this));
                    }
                }
            }
        }
        return Result;
    }

    void TfRuinsTalk::ContinueDominatorVictoryDialog(std::int32_t Action) {
        if (!ShowDominatorVictoryDialog()) {
            I_Start();
        }
    }

    // Rebuilds choices; KeepText preserves the current greeting and skips rank rewards.
    void TfRuinsTalk::M_Main(std::uint8_t KeepText) {
        static const pas::Set<0, 255> AllSeries = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}});
        aScript::TScript* Script{};
        pas::WideString Text{};
        pas::WideString Prefix{};
        aItem::TItem* Item{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t SelectedIndex{};
        std::int32_t BestPriority{};
        std::int32_t ModuleIndex{};
        std::int32_t Place{};
        std::int32_t ExcludedBefore{};
        std::int32_t RangerCount{};
        std::int32_t Parts{};
        aRanger::TRanger* Ranger{};
        pas::Object* SwapEntry{};
        std::uint32_t Seed{};
        aConst::TItemType ItemType{};
        std::int32_t Weight{};
        std::int32_t Level{};
        aConst::PWeaponInfo Info{};
        float MinimumSizeFactor{};
        float MaximumSizeFactor{};
        std::int32_t Stage = 0;
        try {
            aScript::ClearScriptDialogRules();
            Stage = 1;
            ClearChoices();
            Globals::ScriptDialogIndex = -1;
            Script = nullptr;
            Stage = 2;
            if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstRangerCenter) && ShowDominatorVictoryDialog()) {
                return;
            }
            Stage = 3;
            if (aPlayer::GetPlayer()->DockedTo->ScriptShip != nullptr) {
                Script = reinterpret_cast<aScript::TScriptShip*>(aPlayer::GetPlayer()->DockedTo->ScriptShip)->Script;
                Text = reinterpret_cast<aScript::TScriptShip*>(aPlayer::GetPlayer()->DockedTo->ScriptShip)->GetGroup()->StationDialogVariable;
                if (Text != u"") {
                    Script->PublishShipContext(reinterpret_cast<aScript::TScriptShip*>(aPlayer::GetPlayer()->DockedTo->ScriptShip));
                    Script->CallDialogByVariable(Text);
                }
            }
            Stage = 4;
            if (!KeepText) {
                Stage = 5;
                if (StationBridgeMode == 1) {
                    Stage = 6;
                    DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeGreeting"_wref.get());
                    aMyFunction::ReplaceTextToken(DialogText, u"<Energy>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->GetHull()->Energy), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(DialogText, u"<EnergyMax>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->GetHull()->EnergyMax), u"<color=255,240,100>"_w);
                    if (aPlayer::GetPlayer()->GetHull()->ImpulseShieldsEnabled) {
                        pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeImpulseShieldsStatusOn"_wref.get());
                        pas::WideString& dialogText = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText, u"<ShieldMode>"_w, std::move(localizedColorText), u"<color=255,240,100>"_w);
                    } else {
                        pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeImpulseShieldsStatusOff"_wref.get());
                        pas::WideString& dialogText_2 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_2, u"<ShieldMode>"_w, std::move(localizedColorText_2), u"<color=255,240,100>"_w);
                    }
                    aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->CountActiveInterceptorTargets()), u"<color=255,240,100>"_w);
                    if (aPlayer::GetPlayer()->InHyperspace || aPlayer::GetPlayer()->RuinsSavedDockedTo != nullptr || aPlayer::GetPlayer()->RuinsSavedPlanet != nullptr) {
                        pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetNotNormalSpace"_wref.get());
                        pas::WideString& dialogText_3 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_3, u"<Ship>"_w, std::move(localizedColorText_3), u"</color>"_w);
                    } else if (aPlayer::GetPlayer()->GetHull()->Energy < aPlayer::GetPlayer()->GetInterceptorEnergyCost()) {
                        pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetNoEnergy"_wref.get());
                        pas::WideString& dialogText_4 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_4, u"<Ship>"_w, std::move(localizedColorText_4), u"</color>"_w);
                    } else if (aPlayer::GetPlayer()->GetHull()->InterceptorTarget != nullptr) {
                        pas::WideString fullName = static_cast<aShip::TShip*>(aPlayer::GetPlayer()->GetHull()->InterceptorTarget)->GetFullName(u" "_wref.get());
                        pas::WideString& dialogText_5 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_5, u"<Ship>"_w, std::move(fullName), u"<color=255,240,100>"_w);
                    } else if (aPlayer::GetPlayer()->GetHull()->InterceptorTargetingStrategy == aItem::itsManual) {
                        pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetOff"_wref.get());
                        pas::WideString& dialogText_6 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_6, u"<Ship>"_w, std::move(localizedColorText_5), u"</color>"_w);
                    } else if (aPlayer::GetPlayer()->SelectInterceptorTarget() != nullptr) {
                        pas::WideString fullName_2 = aPlayer::GetPlayer()->SelectInterceptorTarget()->GetFullName(u" "_wref.get());
                        pas::WideString& dialogText_7 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_7, u"<Ship>"_w, std::move(fullName_2), u"<color=255,240,100>"_w);
                    } else {
                        pas::WideString localizedColorText_6 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetMissing"_wref.get());
                        pas::WideString& dialogText_8 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_8, u"<Ship>"_w, std::move(localizedColorText_6), u"</color>"_w);
                    }
                    switch (aPlayer::GetPlayer()->GetHull()->InterceptorTargetingStrategy) {
                        case aItem::itsManual: {
                            pas::WideString localizedColorText_7 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyManual"_wref.get());
                            pas::WideString& dialogText_9 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_9, u"<Strategy>"_w, std::move(localizedColorText_7), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aItem::itsMostHullPoints: {
                            pas::WideString localizedColorText_8 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyHPMax"_wref.get());
                            pas::WideString& dialogText_10 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_10, u"<Strategy>"_w, std::move(localizedColorText_8), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aItem::itsFewestHullPoints: {
                            pas::WideString localizedColorText_9 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyHPMin"_wref.get());
                            pas::WideString& dialogText_11 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_11, u"<Strategy>"_w, std::move(localizedColorText_9), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aItem::itsGreatestStrength: {
                            pas::WideString localizedColorText_10 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyStrMax"_wref.get());
                            pas::WideString& dialogText_12 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_12, u"<Strategy>"_w, std::move(localizedColorText_10), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aItem::itsStrongestDefense: {
                            pas::WideString localizedColorText_11 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDefMax"_wref.get());
                            pas::WideString& dialogText_13 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_13, u"<Strategy>"_w, std::move(localizedColorText_11), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aItem::itsNearest: {
                            pas::WideString localizedColorText_12 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDistMin"_wref.get());
                            pas::WideString& dialogText_14 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_14, u"<Strategy>"_w, std::move(localizedColorText_12), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aItem::itsFarthest: {
                            pas::WideString localizedColorText_13 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDistMax"_wref.get());
                            pas::WideString& dialogText_15 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_15, u"<Strategy>"_w, std::move(localizedColorText_13), u"<color=255,240,100>"_w);
                            break;
                        }
                        default: {
                            pas::WideString localizedColorText_14 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyManual"_wref.get());
                            pas::WideString& dialogText_16 = DialogText;
                            aMyFunction::ReplaceTextToken(dialogText_16, u"<Strategy>"_w, std::move(localizedColorText_14), u"<color=255,240,100>"_w);
                            break;
                        }
                    }
                    aMyFunction::ReplaceTextToken(DialogText, u"<Duration>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aPlayer::GetPlayer()->GetInterceptorPassCount())), u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(DialogText, u"<DeployCost>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->GetInterceptorEnergyCost()), u"<color=255,240,100>"_w);
                } else if (StationBridgeMode > 1 || aPlayer::GetPlayer()->RuinsMode > 0) {
                    DialogText = u"text missing"_w;
                } else {
                    Stage = 7;
                    switch (aPlayer::GetPlayer()->DockedTo->TypeId) {
                        case aGalaxyStruct::rstRangerCenter: {
                            Stage = 8;
                            aGalaxy::Galaxy->RefreshRangerRatingPlaces();
                            DialogText = aConst::LocalizedColorText(u"FormRuins.RC.Greeting"_wref.get());
                            Place = aPlayer::GetPlayer()->PlaceInRating;
                            ExcludedBefore = 0;
                            RangerCount = pas::list_count(aGalaxy::Galaxy->Rangers);
                            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
                                Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                                if (aPlayer::GetPlayer() != Ranger && Ranger->ExcludedFromRating) {
                                    --RangerCount;
                                    if (Ranger->PlaceInRating < Place) {
                                        ++ExcludedBefore;
                                    }
                                }
                            }
                            Place -= ExcludedBefore;
                            if (Place == 1) {
                                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.RC.GreetingBest"_wref.get())});
                            } else {
                                std::int32_t cpp_case = pas::idiv(100 * Place, RangerCount);
                                if (cpp_case >= 0 && cpp_case <= 30) {
                                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.RC.GreetingGood"_wref.get())});
                                } else if (cpp_case >= 31 && cpp_case <= 66) {
                                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.RC.GreetingNormal"_wref.get())});
                                } else {
                                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.RC.GreetingBad"_wref.get())});
                                }
                            }
                            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.RC.GreetingAdd"_wref.get())});
                            aMyFunction::ReplaceTextToken(DialogText, u"<RC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                            aMyFunction::ReplaceTextToken(DialogText, u"<Number>"_w, pas::wide_int_to_str(Place), u"<color=255,240,100>"_w);
                            aMyFunction::ReplaceTextToken(DialogText, u"<BaseNod>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->BaseNodes), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aGalaxyStruct::rstPirateBase: {
                            Stage = 9;
                            // Native retains this flag comparison with an empty body.
                            static_cast<void>(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SpecialServiceActive);
                            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckBaronAchievement();
                            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.GreetingPre"_wref.get());
                            DialogText = pas::concat_wide_reverse({aConst::LocalizedColorText(u"FormRuins.PB.GreetingMod"_wref.get()), DialogText});
                            DialogText = pas::concat_wide_reverse({aConst::LocalizedColorText(u"FormRuins.PB.GreetingAft"_wref.get()), DialogText});
                            if (aPlayer::GetPlayer()->MayTakeSubCrack()) {
                                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PBGreetingAdd"_wref.get())});
                            }
                            aMyFunction::ReplaceTextToken(DialogText, u"<PB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                            aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str((static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost())), u"<color=255,240,100>"_w);
                            break;
                        }
                        case aGalaxyStruct::rstScienceBase: {
                            Stage = 10;
                            if (aGalaxy::Galaxy->IsDominatorResearchComplete(static_cast<aGalaxy::TDominatorSeriesSet>(AllSeries)) || static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(static_cast<aGalaxy::TDominatorSeriesSet>(AllSeries)) ^ 1)) {
                                DialogText = aConst::LocalizedColorText(u"FormRuins.SB.GreetingAfterScn"_wref.get());
                            } else {
                                DialogText = aConst::LocalizedColorText(u"FormRuins.SB.GreetingBeforeScn"_wref.get());
                            }
                            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.GreetingAdd"_wref.get())});
                            if (aGalaxy::Galaxy->CurrentTurn - 300 < 120) {
                                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuinsSB.History.SB"_wref.get())});
                            }
                            aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                            break;
                        }
                        case aGalaxyStruct::rstMilitaryBase: {
                            Stage = 11;
                            if (aPlayer::GetPlayer()->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                                DialogText = aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.WBAfterQuestions"_wref.get());
                                aMyFunction::ReplaceTextToken(DialogText, u"<WB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                            } else {
                                if (aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate && aPlayer::GetPlayer()->TryPromoteRank()) {
                                    Stage = 12;
                                    DialogText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.WB.", aConst::CoalitionRankNames[aPlayer::GetPlayer()->Rank], u".NewRank"}));
                                    static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckCommanderAchievement();
                                    aMyFunction::ReplaceTextToken(DialogText, u"<PredPoints>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aConst::CoalitionRankPointThresholds[aPlayer::GetPlayer()->Rank - 1])), u"<color=255,240,100>"_w);
                                    if (aPlayer::GetPlayer()->Rank == 7) {
                                        ModuleIndex = aConst::FindMicroModuleTemplateByCustomTag(u"AkrinAmplifier"sv);
                                        Item = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
                                        if (ModuleIndex >= 0) {
                                            pas::checked_cast<aItem::TMicroModule*>(Item)->Init(ModuleIndex);
                                        } else {
                                            std::int32_t selectMicroModule = aGalaxy::TGalaxy::SelectMicroModule(1, 15, aGalaxy::Galaxy->GenerationSeed + static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn), aPlayer::GetPlayer()->DockedTo);
                                            aItem::TMicroModule* cpp_arg = pas::checked_cast<aItem::TMicroModule*>(Item);
                                            cpp_arg->Init(selectMicroModule);
                                        }
                                        pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
                                        {
                                            pas::WideString plainName = pas::checked_cast<aItem::TMicroModule*>(Item)->GetPlainName();
                                            pas::WideString& dialogText_17 = DialogText;
                                            aMyFunction::ReplaceTextToken(dialogText_17, u"<MMName>"_w, std::move(plainName), u"<color=255,240,100>"_w);
                                        }
                                    }
                                    Seed = aGalaxy::Galaxy->CurrentTurn / 50 * (aPlayer::GetPlayer()->DockedTo->Id * (aPlayer::GetPlayer()->Rank + 11));
                                    MinimumSizeFactor = aConst::EquipmentSizeFactors[4];
                                    MaximumSizeFactor = aConst::EquipmentSizeFactors[2];
                                    if (aMyFunction::NextRandomIntRange(1, 100, Seed) > 70) {
                                        Info = aGalaxy::Galaxy->SelectWeaponInfo(Seed, pas::make_set<aGalaxyStruct::TWeaponAvailabilityMask>({{0}, {1}, {static_cast<std::int32_t>(aConst::OwnerWeaponAvailability[StationOwner])}}), std::min<std::int32_t>(aGalaxy::Galaxy->TechLevel + 2, 8), aGalaxy::Galaxy->TechLevel);
                                        Seed = aGalaxy::Galaxy->CurrentTurn / 33 * (aPlayer::GetPlayer()->DockedTo->Id * (aPlayer::GetPlayer()->Rank + 17));
                                        Weight = ([&] {
                                            std::int32_t round = System::Round(static_cast<long double>(Info->AverageSize) * MaximumSizeFactor);
                                            std::int32_t round_2 = System::Round(static_cast<long double>(Info->AverageSize) * MinimumSizeFactor);
                                            return aMyFunction::NextRandomIntRange(round_2, round, Seed);
                                        }());
                                        Level = System::Round(aMyFunction::RemapClamped(System::Round(aMyFunction::RemapClamped(static_cast<std::int8_t>(aPlayer::GetPlayer()->Rank * 1), 0.0, 7.0, 1.0, 5.0)), 1.0, 5.0, 3.0, 8.0));
                                        Item = aItem::CreateGeneratedWeapon(Info, Weight, Level, StationOwner);
                                    } else {
                                        ItemType = static_cast<aConst::TItemType>(aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{43, 49}})));
                                        Seed = aGalaxy::Galaxy->CurrentTurn / 33 * (aPlayer::GetPlayer()->DockedTo->Id * (aPlayer::GetPlayer()->Rank + 17));
                                        {
                                            std::int32_t round_3 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MaximumSizeFactor);
                                            std::int32_t round_4 = System::Round(static_cast<long double>(aConst::GetAverageItemSize(ItemType)) * MinimumSizeFactor);
                                            Weight = aMyFunction::NextRandomIntRange(round_4, round_3, Seed);
                                        }
                                        Level = System::Round(aMyFunction::RemapClamped(static_cast<std::int8_t>(aPlayer::GetPlayer()->Rank * 1), 0.0, 7.0, 3.0, 8.0));
                                        Item = aItem::CreateGeneratedEquipment(ItemType, Weight, Level, StationOwner);
                                    }
                                    if (Item != nullptr) {
                                        pas::WideString displayName = Item->GetDisplayName();
                                        pas::WideString& dialogText_18 = DialogText;
                                        aMyFunction::ReplaceTextToken(dialogText_18, u"<ItemName>"_w, std::move(displayName), u"<color=255,240,100>"_w);
                                    } else {
                                        GR_Main::RaiseWideMessage(u"eq=nil"_wref.get());
                                    }
                                    pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
                                } else {
                                    DialogText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.WB.", aConst::CoalitionRankNames[aPlayer::GetPlayer()->Rank], u".Greeting"}));
                                }
                                if (pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar && aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.GreetingAdd"_wref.get())});
                                    aMyFunction::ReplaceTextToken(DialogText, u"<StarEnemy>"_w, pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar->Name, u"<color=255,240,100>"_w);
                                    {
                                        pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate);
                                        pas::WideString& dialogText_19 = DialogText;
                                        aMyFunction::ReplaceTextToken(dialogText_19, u"<Date>"_w, std::move(formatTurnDate), u"<color=255,240,100>"_w);
                                    }
                                    MilitaryTravelDistance = System::Round(aMyFunction::PointDistance(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar->Position, aPlayer::GetPlayer()->CurrentStar->Position));
                                }
                                if (aPlayer::GetPlayer()->CountProgramRewardStocks() > 0) {
                                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.WB.Programms.GreetingAdd"_wref.get())});
                                }
                                aMyFunction::ReplaceTextToken(DialogText, u"<WB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                                {
                                    pas::WideString rankName = aPlayer::GetPlayer()->GetRankName();
                                    pas::WideString& dialogText_20 = DialogText;
                                    aMyFunction::ReplaceTextToken(dialogText_20, u"<Rank>"_w, std::move(rankName), u"<color=255,240,100>"_w);
                                }
                                aMyFunction::ReplaceTextToken(DialogText, u"<NeedPoints>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aPlayer::GetPlayer()->GetRankPointsToNextRank())), u"<color=255,240,100>"_w);
                            }
                            break;
                        }
                        case aGalaxyStruct::rstBusinessCenter: {
                            Stage = 13;
                            DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Greeting"_wref.get());
                            if (aPlayer::GetPlayer()->DebtAmount > 0) {
                                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.BK.AddDebtYes"_wref.get())});
                            } else {
                                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.BK.AddDebtNot"_wref.get())});
                            }
                            if (aPlayer::GetPlayer()->DebtDefaultCount >= 3) {
                                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.BK.AddDebtContinue"_wref.get())});
                            }
                            aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                            aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->DebtAmount), u"<color=255,240,100>"_w);
                            {
                                pas::WideString formatTurnDate_2 = aGalaxy::Galaxy->FormatTurnDate(aPlayer::GetPlayer()->DebtDueTurn);
                                pas::WideString& dialogText_21 = DialogText;
                                aMyFunction::ReplaceTextToken(dialogText_21, u"<Date>"_w, std::move(formatTurnDate_2), u"<color=255,240,100>"_w);
                            }
                            break;
                        }
                        case aGalaxyStruct::rstMedicalBase: {
                            Stage = 14;
                            DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Greeting"_wref.get());
                            aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                            break;
                        }
                        case aGalaxyStruct::rstDominion: {
                            Stage = 15;
                            if (aPlayer::GetPlayer()->DockedTo->InHyperspace) {
                                DialogText = aConst::LocalizedColorText(u"FormRuins.CB.GreetingHyperspace"_wref.get());
                            } else if (aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
                                DialogText = aConst::LocalizedColorText(u"FormRuins.CB.GreetingPlayerFlyToStar"_wref.get());
                                aMyFunction::ReplaceTextToken(DialogText, u"<FlyToStar>"_w, aPlayer::GetPlayer()->QueuedTravelTarget->Name, u"<color=255,240,100>"_w);
                            } else if (aPlayer::GetPlayer()->DockedTo->Order == aShip::soTeleport && static_cast<std::uint32_t>(aPlayer::GetPlayer()->DockedTo->OrderStateData) > 0) {
                                DialogText = aConst::LocalizedColorText(u"FormRuins.CB.GreetingFlyToStar"_wref.get());
                                aMyFunction::ReplaceTextToken(DialogText, u"<FlyToStar>"_w, reinterpret_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->DockedTo->OrderTarget)->Name, u"<color=255,240,100>"_w);
                            } else {
                                DialogText = aConst::LocalizedColorText(u"FormRuins.CB.GreetingNormal"_wref.get());
                            }
                            aMyFunction::ReplaceTextToken(DialogText, u"<CB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                            break;
                        }
                    }
                }
                Stage = 16;
                if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo) != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SpecialServiceActive) {
                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Before"}))});
                }
            }
            Stage = 17;
            if (Globals::ScriptDialogIndex < 0) {
                Stage = 18;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range_2.next(I); ) {
                    Script = pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, I);
                    aScript::TScript_RunDialogCode(Script);
                }
                Stage = 19;
                if (pas::list_count(aScript::ScriptDialogOverrides) > 0) {
                    Stage = 20;
                    SelectedIndex = 0;
                    BestPriority = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, 0)->Priority;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogOverrides) - 1); cpp_range_3.next(I); ) {
                        if (pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, I)->Priority > BestPriority) {
                            SelectedIndex = I;
                            BestPriority = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, I)->Priority;
                        }
                    }
                    Script = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, SelectedIndex)->Script;
                    {
                        std::uint32_t answerData = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, SelectedIndex)->AnswerData;
                        EC_Expression::TVarEC* var = Script->InitCode->LocalVar->GetVar(u"GAnswerData"_wref.get());
                        var->SetDword(answerData);
                    }
                    Text = pas::list_at<aScript::TDialogOverride>(aScript::ScriptDialogOverrides, SelectedIndex)->DialogName;
                    if (Text != u"") {
                        Script->PublishCurrentShip(aPlayer::GetPlayer()->DockedTo);
                        Script->CallDialogByVariable(Text);
                        if (Globals::ScriptDialogIndex < 0) {
                            GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({Script->ScriptFileName, u" has overriden dialog with ", Text, u" but it failed to start"})));
                        }
                    }
                    if (Globals::ScriptDialogIndex < 0) {
                        Stage = 21;
                        M_Main(true);
                    } else {
                        Stage = 22;
                        SelectScriptDialog(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Script)));
                    }
                } else {
                    Stage = 23;
                    SelectedIndex = -1;
                    BestPriority = 0;
                    if (!KeepText) {
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_4.next(I); ) {
                            if (pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->ReplaceGreeting) {
                                if (SelectedIndex < 0 || pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Priority > BestPriority) {
                                    BestPriority = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Priority;
                                    SelectedIndex = I;
                                }
                            }
                        }
                    }
                    if (SelectedIndex >= 0) {
                        DialogText = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, SelectedIndex)->Text;
                    }
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_5.next(I); ) {
                        for (auto cpp_range_6 = pas::for_downto<std::int32_t>(pas::list_count(aScript::ScriptDialogInjections) - 1, I); cpp_range_6.next(J); ) {
                            if (([&] {
                                std::int32_t cpp_left = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, J)->Priority;
                                return cpp_left > pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, J - 1)->Priority;
                            }())) {
                                SwapEntry = pas::list_at<pas::Object>(aScript::ScriptDialogInjections, J);
                                pas::list_put(aScript::ScriptDialogInjections, J, pas::list_get(aScript::ScriptDialogInjections, J - 1));
                                pas::list_put(aScript::ScriptDialogInjections, J - 1, reinterpret_cast<void*>(SwapEntry));
                            }
                        }
                    }
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(aScript::ScriptDialogInjections) - 1); cpp_range_7.next(I); ) {
                        if (!pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->ReplaceGreeting) {
                            Text = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Text;
                            if (Text != u"" && static_cast<std::uint8_t>(KeepText ^ 1)) {
                                DialogText = pas::concat_wide({DialogText, u"\r\n", Text});
                            }
                        }
                        Text = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Answer;
                        if (Text != u"") {
                            Prefix = pas::WideString();
                            Parts = EC_Str::CountDelimitedPartsW(pas::view(Text), u"~"sv);
                            if (Parts > 1) {
                                Prefix = EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u"~"sv);
                                Text = EC_Str::ExtractDelimitedRangeW(pas::view(Text), 1, Parts - 1, u"~"sv);
                            }
                            if (Prefix == u"block") {
                                AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
                            } else if (Prefix == u"snap") {
                                AddChoice(Text, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aScript::ScriptDialogInjections, I))), pas::bind_method<&TfRuinsTalk::RunInjectedDialogKeepingScroll>(this));
                            } else {
                                std::int32_t cpp_arg_2 = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aScript::ScriptDialogInjections, I)));
                                pas::WideString answer = pas::list_at<aScript::TDialogInject>(aScript::ScriptDialogInjections, I)->Answer;
                                AddChoice(std::move(answer), cpp_arg_2, pas::bind_method<&TfRuinsTalk::RunInjectedDialog>(this));
                            }
                        }
                    }
                    Stage = 24;
                    BuildBuiltinServiceOptions();
                }
            } else {
                Stage = 25;
                if (!Script->SkipGreeting) {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::SelectScriptDialog>(this);
                    pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.I_Continue"_wref.get())});
                    std::int32_t script = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Script));
                    TfRuinsTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_4), script, cpp_arg_3);
                } else {
                    Script->SkipGreeting = false;
                    SelectScriptDialog(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Script)));
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                if (aGalaxy::Galaxy == nullptr) {
                    GR_Main::AppendLogLineThreadSafe("Galaxy=nil"_a);
                } else if (aPlayer::GetPlayer() == nullptr) {
                    GR_Main::AppendLogLineThreadSafe("Player=nil"_a);
                } else if (aPlayer::GetPlayer()->DockedTo == nullptr) {
                    GR_Main::AppendLogLineThreadSafe("CurShip=nil"_a);
                }
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in procedure TfRuinsTalk.M_Main, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfRuinsTalk::CloseHullMode(std::int32_t Action) {
        aPlayer::GetPlayer()->CloseRuinsModeScreen();
    }

    void TfRuinsTalk::ToggleImpulseShields(std::int32_t Action) {
        aPlayer::GetPlayer()->GetHull()->ImpulseShieldsEnabled = static_cast<std::uint8_t>(aPlayer::GetPlayer()->GetHull()->ImpulseShieldsEnabled ^ 1);
        aPlayer::GetPlayer()->GetHull()->Energy = std::max<std::int32_t>(0, aPlayer::GetPlayer()->GetHull()->Energy - 10);
        ClearChoices();
        M_Main(false);
    }

    void TfRuinsTalk::ShowBridgeBlackHoleDialog(std::int32_t Action) {
        if (!aPlayer::GetPlayer()->NoJump) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeBHChooseDestination"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<Cost>"_w, pas::wide_int_to_str(600), u"<color=255,240,100>"_w);
            ClearChoices();
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::SelectBridgeBlackHoleDestination>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeBHToStarMap"_wref.get())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::ReturnToMain>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeBHCancel"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
            }
        } else if (aPlayer::GetPlayer()->NoJump) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeBHHyperLock"_wref.get());
            ClearChoices();
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::ReturnToMain>(this);
                pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.I_Continue"_wref.get())});
                TfRuinsTalk* self_3 = this;
                self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
            }
        }
    }

    void TfRuinsTalk::SelectBridgeBlackHoleDestination(std::int32_t Action) {
        MainPanel->NavigationLocked = true;
        GetByName(u"PM_WinMsg"sv)->SetActive(false);
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        Globals::GalaxyScreen->ViewMode = 3;
        GlobalsV::GalaxyReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenGalaxy;
        RequestClose(1);
    }

    void TfRuinsTalk::ShowInterceptorDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsChooseAction"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->CountActiveInterceptorTargets()), u"<color=255,240,100>"_w);
        if (aPlayer::GetPlayer()->InHyperspace || aPlayer::GetPlayer()->RuinsSavedDockedTo != nullptr || aPlayer::GetPlayer()->RuinsSavedPlanet != nullptr) {
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetNotNormalSpace"_wref.get());
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<Ship>"_w, std::move(localizedColorText), u"</color>"_w);
        } else if (aPlayer::GetPlayer()->GetHull()->Energy < aPlayer::GetPlayer()->GetInterceptorEnergyCost()) {
            pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetNoEnergy"_wref.get());
            pas::WideString& dialogText_2 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_2, u"<Ship>"_w, std::move(localizedColorText_2), u"</color>"_w);
        } else if (aPlayer::GetPlayer()->GetHull()->InterceptorTarget != nullptr) {
            pas::WideString fullName = static_cast<aShip::TShip*>(aPlayer::GetPlayer()->GetHull()->InterceptorTarget)->GetFullName(u" "_wref.get());
            pas::WideString& dialogText_3 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_3, u"<Ship>"_w, std::move(fullName), u"<color=255,240,100>"_w);
        } else if (aPlayer::GetPlayer()->GetHull()->InterceptorTargetingStrategy == aItem::itsManual) {
            pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetOff"_wref.get());
            pas::WideString& dialogText_4 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_4, u"<Ship>"_w, std::move(localizedColorText_3), u"</color>"_w);
        } else if (aPlayer::GetPlayer()->SelectInterceptorTarget() != nullptr) {
            pas::WideString fullName_2 = aPlayer::GetPlayer()->SelectInterceptorTarget()->GetFullName(u" "_wref.get());
            pas::WideString& dialogText_5 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_5, u"<Ship>"_w, std::move(fullName_2), u"<color=255,240,100>"_w);
        } else {
            pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsNextTargetMissing"_wref.get());
            pas::WideString& dialogText_6 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_6, u"<Ship>"_w, std::move(localizedColorText_4), u"</color>"_w);
        }
        switch (aPlayer::GetPlayer()->GetHull()->InterceptorTargetingStrategy) {
            case aItem::itsManual: {
                pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyManual"_wref.get());
                pas::WideString& dialogText_7 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_7, u"<Strategy>"_w, std::move(localizedColorText_5), u"<color=255,240,100>"_w);
                break;
            }
            case aItem::itsMostHullPoints: {
                pas::WideString localizedColorText_6 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyHPMax"_wref.get());
                pas::WideString& dialogText_8 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_8, u"<Strategy>"_w, std::move(localizedColorText_6), u"<color=255,240,100>"_w);
                break;
            }
            case aItem::itsFewestHullPoints: {
                pas::WideString localizedColorText_7 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyHPMin"_wref.get());
                pas::WideString& dialogText_9 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_9, u"<Strategy>"_w, std::move(localizedColorText_7), u"<color=255,240,100>"_w);
                break;
            }
            case aItem::itsGreatestStrength: {
                pas::WideString localizedColorText_8 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyStrMax"_wref.get());
                pas::WideString& dialogText_10 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_10, u"<Strategy>"_w, std::move(localizedColorText_8), u"<color=255,240,100>"_w);
                break;
            }
            case aItem::itsStrongestDefense: {
                pas::WideString localizedColorText_9 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDefMax"_wref.get());
                pas::WideString& dialogText_11 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_11, u"<Strategy>"_w, std::move(localizedColorText_9), u"<color=255,240,100>"_w);
                break;
            }
            case aItem::itsNearest: {
                pas::WideString localizedColorText_10 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDistMin"_wref.get());
                pas::WideString& dialogText_12 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_12, u"<Strategy>"_w, std::move(localizedColorText_10), u"<color=255,240,100>"_w);
                break;
            }
            case aItem::itsFarthest: {
                pas::WideString localizedColorText_11 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDistMax"_wref.get());
                pas::WideString& dialogText_13 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_13, u"<Strategy>"_w, std::move(localizedColorText_11), u"<color=255,240,100>"_w);
                break;
            }
            default: {
                pas::WideString localizedColorText_12 = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyManual"_wref.get());
                pas::WideString& dialogText_14 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_14, u"<Strategy>"_w, std::move(localizedColorText_12), u"<color=255,240,100>"_w);
                break;
            }
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<Duration>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aPlayer::GetPlayer()->GetInterceptorPassCount())), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<DeployCost>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->GetInterceptorEnergyCost()), u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->CountActiveInterceptorTargets() > 0) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowActiveInterceptors>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsCallOffAsk"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsCallOffAsk"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::ShowInterceptorPassDialog>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDurationChangeAsk"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
        if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && aPlayer::GetPlayer()->RuinsSavedDockedTo == nullptr && aPlayer::GetPlayer()->RuinsSavedPlanet == nullptr && aPlayer::GetPlayer()->GetHull()->Energy >= aPlayer::GetPlayer()->GetInterceptorEnergyCost()) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_6 = pas::bind_method<&TfRuinsTalk::ShowInterceptorTargetDialog>(this);
            pas::WideString cpp_arg_7 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetChangeAsk"_wref.get())});
            TfRuinsTalk* self_4 = this;
            self_4->AddChoice(std::move(cpp_arg_7), 0, cpp_arg_6);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_2 = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetChangeAsk"_wref.get())});
            TfRuinsTalk* self_5 = this;
            self_5->AddChoice(std::move(cpp_arg_8), 0, scriptDialogBlockCallback_2);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_9 = pas::bind_method<&TfRuinsTalk::ShowInterceptorStrategyDialog>(this);
            pas::WideString cpp_arg_10 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargeting"_wref.get())});
            TfRuinsTalk* self_6 = this;
            self_6->AddChoice(std::move(cpp_arg_10), 0, cpp_arg_9);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_11 = pas::bind_method<&TfRuinsTalk::ReturnToMain>(this);
            pas::WideString cpp_arg_12 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDone"_wref.get())});
            TfRuinsTalk* self_7 = this;
            self_7->AddChoice(std::move(cpp_arg_12), 0, cpp_arg_11);
        }
    }

    void TfRuinsTalk::ShowActiveInterceptors(std::int32_t Action) {
        pas::WideString ShipList{};
        pas::WideString Text{};
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsCallOffChoose"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->CountActiveInterceptorTargets()), u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::RecallAllInterceptors>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsCallOffAll"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (aPlayer::GetPlayer() == Ship->InterceptorSourceShip) {
                    ShipList = pas::concat_wide({ShipList, Ship->GetFullName(u" "_wref.get()), u"\r\n"});
                    Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsCallOffShip"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, Ship->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                    AddChoice(pas::concat_wide({u"- ", Text}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfRuinsTalk::RecallInterceptorsFromTarget>(this));
                }
            }
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<ShipList>"_w, ShipList, u"<color=255,240,100>"_w);
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::ShowInterceptorDialog>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsCallOffCancel"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
    }

    void TfRuinsTalk::RecallInterceptorsFromTarget(std::int32_t Action) {
        reinterpret_cast<aShip::TShip*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)))->ClearIncomingInterceptors();
        if (aPlayer::GetPlayer()->CountActiveInterceptorTargets() > 0) {
            ShowActiveInterceptors(0);
        } else {
            ShowInterceptorDialog(0);
        }
    }

    void TfRuinsTalk::RecallAllInterceptors(std::int32_t Action) {
        std::int32_t I{};
        std::int32_t J{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                if (aPlayer::GetPlayer() == Ship->InterceptorSourceShip) {
                    Ship->ClearIncomingInterceptors();
                }
            }
        }
        ShowInterceptorDialog(0);
    }

    void TfRuinsTalk::ShowInterceptorPassDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDurationChoose"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Duration>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aPlayer::GetPlayer()->GetInterceptorPassCount())), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<DeployCost>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->GetInterceptorEnergyCost()), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<DurationMax>"_w, pas::wide_int_to_str(10), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<DurationMin>"_w, pas::wide_int_to_str(2), u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->GetInterceptorPassCount() < 10) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::IncreaseInterceptorPassCount>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDurationMore"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDurationMore"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        if (aPlayer::GetPlayer()->GetInterceptorPassCount() > 2) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::DecreaseInterceptorPassCount>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDurationLess"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_2 = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDurationLess"_wref.get())});
            TfRuinsTalk* self_4 = this;
            self_4->AddChoice(std::move(cpp_arg_6), 0, scriptDialogBlockCallback_2);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::ShowInterceptorDialog>(this);
            pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsDurationDone"_wref.get())});
            TfRuinsTalk* self_5 = this;
            self_5->AddChoice(std::move(cpp_arg_8), 0, cpp_arg_7);
        }
    }

    void TfRuinsTalk::IncreaseInterceptorPassCount(std::int32_t Action) {
        aPlayer::GetPlayer()->GetHull()->InterceptorPassCountOverride = aPlayer::GetPlayer()->GetInterceptorPassCount() + 1;
        ShowInterceptorPassDialog(0);
    }

    void TfRuinsTalk::DecreaseInterceptorPassCount(std::int32_t Action) {
        aPlayer::GetPlayer()->GetHull()->InterceptorPassCountOverride = aPlayer::GetPlayer()->GetInterceptorPassCount() - 1;
        ShowInterceptorPassDialog(0);
    }

    void TfRuinsTalk::ShowInterceptorTargetDialog(std::int32_t Action) {
        std::int32_t I{};
        pas::WideString Text{};
        aShip::TShip* Ship{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetChangeAsk"_wref.get());
        ClearChoices();
        if (aPlayer::GetPlayer()->GetHull()->InterceptorTarget != nullptr) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetCancelManual"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ClearInterceptorTarget>(this));
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetCancelManual"_wref.get())});
            AddChoice(std::move(cpp_arg), 0, scriptDialogBlockCallback);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
            if (aPlayer::GetPlayer() != Ship && aPlayer::GetPlayer()->DockedTo != Ship && (!(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) || aPlayer::GetPlayer()->CanSelectShipTarget(Ship)) && Ship->InNormalSpace() && aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, Ship->Position) <= 1.0E+6L && Ship->InterceptorPassesRemaining <= 0) {
                Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetShip"_wref.get());
                aMyFunction::ReplaceTextToken(Text, u"<Ship>"_w, Ship->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
                AddChoice(pas::concat_wide({u"- ", Text}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Ship)), pas::bind_method<&TfRuinsTalk::SelectInterceptorTarget>(this));
            }
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetCancel"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowInterceptorDialog>(this));
    }

    void TfRuinsTalk::SelectInterceptorTarget(std::int32_t Action) {
        aPlayer::GetPlayer()->GetHull()->InterceptorTarget = reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        ShowInterceptorDialog(0);
    }

    void TfRuinsTalk::ClearInterceptorTarget(std::int32_t Action) {
        aPlayer::GetPlayer()->GetHull()->InterceptorTarget = nullptr;
        ShowInterceptorDialog(0);
    }

    void TfRuinsTalk::ShowInterceptorStrategyDialog(std::int32_t Action) {
        pas::WideString Text{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargeting"_wref.get());
        ClearChoices();
        Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingAttack"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<StrategyName>"_w, aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyManual"_wref.get()), u"<color=255,240,100>"_w);
        AddChoice(pas::concat_wide({u"- ", Text}), 0, pas::bind_method<&TfRuinsTalk::SelectInterceptorStrategy>(this));
        Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingAttack"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<StrategyName>"_w, aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyHPMax"_wref.get()), u"<color=255,240,100>"_w);
        AddChoice(pas::concat_wide({u"- ", Text}), 1, pas::bind_method<&TfRuinsTalk::SelectInterceptorStrategy>(this));
        Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingAttack"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<StrategyName>"_w, aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyHPMin"_wref.get()), u"<color=255,240,100>"_w);
        AddChoice(pas::concat_wide({u"- ", Text}), 2, pas::bind_method<&TfRuinsTalk::SelectInterceptorStrategy>(this));
        Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingAttack"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<StrategyName>"_w, aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyStrMax"_wref.get()), u"<color=255,240,100>"_w);
        AddChoice(pas::concat_wide({u"- ", Text}), 3, pas::bind_method<&TfRuinsTalk::SelectInterceptorStrategy>(this));
        Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingAttack"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<StrategyName>"_w, aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDefMax"_wref.get()), u"<color=255,240,100>"_w);
        AddChoice(pas::concat_wide({u"- ", Text}), 4, pas::bind_method<&TfRuinsTalk::SelectInterceptorStrategy>(this));
        Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingAttack"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<StrategyName>"_w, aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDistMin"_wref.get()), u"<color=255,240,100>"_w);
        AddChoice(pas::concat_wide({u"- ", Text}), 5, pas::bind_method<&TfRuinsTalk::SelectInterceptorStrategy>(this));
        Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingAttack"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<StrategyName>"_w, aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingStrategyDistMax"_wref.get()), u"<color=255,240,100>"_w);
        AddChoice(pas::concat_wide({u"- ", Text}), 6, pas::bind_method<&TfRuinsTalk::SelectInterceptorStrategy>(this));
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsTargetingCancel"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowInterceptorDialog>(this));
    }

    void TfRuinsTalk::SelectInterceptorStrategy(std::int32_t Action) {
        aPlayer::GetPlayer()->GetHull()->InterceptorTargetingStrategy = static_cast<aItem::TInterceptorTargetingStrategy>(Action);
        ShowInterceptorDialog(0);
    }

    void TfRuinsTalk::ShowBridgeHelp(std::int32_t Action) {
        std::int32_t I{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeHelpChoose"_wref.get());
        ClearChoices();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 5); cpp_range.next(I); ) {
            pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"FormRuins.Bridge.BridgeHelpQuestion", SysUtils::IntToStr(I)})))});
            std::int32_t i = I;
            AddChoice(std::move(cpp_arg), i, pas::bind_method<&TfRuinsTalk::ShowBridgeHelpAnswer>(this));
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeHelpCancel"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ReturnToMain>(this));
    }

    void TfRuinsTalk::ShowBridgeHelpAnswer(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(static_cast<pas::WideString>(pas::concat_ansi({"FormRuins.Bridge.BridgeHelpAnswer", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Action))})));
        ClearChoices();
        aMyFunction::ReplaceTextToken(DialogText, u"<SwitchCost>"_w, pas::wide_int_to_str(10), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<BHCost>"_w, pas::wide_int_to_str(600), u"<color=255,240,100>"_w);
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowBridgeHelp>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeHelpMoreQuestions"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::ReturnToMain>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeHelpNoQuestions"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
    }

    void TfRuinsTalk::BuildBuiltinServiceOptions() {
        static const pas::Set<0, 255> AllSeries = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}});
        pas::WideString Text{};
        if (StationBridgeMode == 1) {
            if (aPlayer::GetPlayer()->GetHull()->ImpulseShieldsEnabled) {
                Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeImpulseShieldsOff"_wref.get());
            } else {
                Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeImpulseShieldsOn"_wref.get());
            }
            aMyFunction::ReplaceTextToken(Text, u"<SwitchCost>"_w, pas::wide_int_to_str(10), u"<color=255,240,100>"_w);
            if (aPlayer::GetPlayer()->GetHull()->Energy >= 10) {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, pas::bind_method<&TfRuinsTalk::ToggleImpulseShields>(this));
            } else {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
            }
            Text = aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeBHAsk"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<Cost>"_w, pas::wide_int_to_str(600), u"<color=255,240,100>"_w);
            if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1) && aPlayer::GetPlayer()->RuinsSavedDockedTo == nullptr && aPlayer::GetPlayer()->RuinsSavedPlanet == nullptr && aPlayer::GetPlayer()->GetHull()->Energy >= 600) {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, pas::bind_method<&TfRuinsTalk::ShowBridgeBlackHoleDialog>(this));
            } else {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
            }
            if (aPlayer::GetPlayer()->GetHull()->InterceptorsEnabled) {
                AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeInterceptorsAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowInterceptorDialog>(this));
            }
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeHelpAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowBridgeHelp>(this));
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeExit"_wref.get())}), 0, pas::bind_static_method<&TfRuinsTalk::CloseHullMode>(this));
        } else if (StationBridgeMode > 1) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.Bridge.BridgeExit"_wref.get())}), 0, pas::bind_static_method<&TfRuinsTalk::CloseHullMode>(this));
        } else {
            switch (aPlayer::GetPlayer()->DockedTo->TypeId) {
                case aGalaxyStruct::rstRangerCenter: {
                    if (aPlayer::GetPlayer()->GetCarriedNodeCount() > 0) {
                        AddChoice(([&] {
                            pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->GetCarriedNodeCount());
                            pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.SaleNod.PlayerSend"_wref.get())});
                            return aMyFunction::FormatText1(std::move(cpp_arg), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr));
                        }()), 0, pas::bind_method<&TfRuinsTalk::DepositNodesAtRangerCenter>(this));
                    }
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowRangerCenterTakeNodeDialog>(this));
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowRangerCenterGiveNodeDialog>(this));
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.AboutNod.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowRangerCenterNodeInfo>(this));
                    if (aGalaxy::Galaxy->PirateWinTurn == 0) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.PirateClan.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowRangerCenterPirateClanAnswer>(this));
                    }
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.Rating.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowRangerCenterRatingAnswer>(this));
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.BestRanger.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowRangerCenterBestRangerAnswer>(this));
                    break;
                }
                case aGalaxyStruct::rstPirateBase: {
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.ChangeNationality"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseNationalityDialog>(this));
                    if (aPlayer::GetPlayer()->PirateClanReal && aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate && aGalaxy::Galaxy->PirateWinType != 3) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.ChangeSideToPirate"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseSideChangeDialog>(this));
                    }
                    if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.ChangeSideToNormal"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseSideChangeDialog>(this));
                    }
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Program.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseProgramDialog>(this));
                    if (aGalaxy::Galaxy->ArePirateNodesEnabled()) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Nod.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseNodeDialog>(this));
                    }
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Repair.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseRepairDialog>(this));
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseChameleonDialog>(this));
                    if (aPlayer::GetPlayer()->MayTakeSubCrack()) {
                        if ((static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost()) <= aPlayer::GetPlayer()->Money) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PlayerInfo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowPirateBaseSubCrackDialog>(this));
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PlayerInfo"_wref.get())});
                            AddChoice(std::move(cpp_arg_2), 0, scriptDialogBlockCallback);
                        }
                    }
                    break;
                }
                case aGalaxyStruct::rstMilitaryBase: {
                    if (aPlayer::GetPlayer()->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                        ClearChoices();
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerHangar"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::OpenHangar>(this));
                    } else {
                        if (aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.WarWithKlingAndPirates.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::I_WarWithKlingAndPirates>(this));
                            if (aPlayer::GetPlayer()->Rank < 6) {
                                AddChoice(pas::concat_wide({u"- ", ([&] {
                                    pas::WideString nextRankName = aPlayer::GetPlayer()->GetNextRankName();
                                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.WB.NextRank.PlayerSend"_wref.get());
                                    return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<NextRank>"_w, std::move(nextRankName));
                                }())}), 0, pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseNextRankDialog>(this));
                            }
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.Repair.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseRepairDialog>(this));
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseWarOperationDialog>(this));
                            if (pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != aPlayer::GetPlayer()->CurrentStar) {
                                AddChoice(([&] {
                                    auto name = pas::borrow(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar->Name);
                                    pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerAsk"_wref.get())});
                                    return aMyFunction::FormatText1(std::move(cpp_arg_3), u"<color=255,240,100>"_w, u"<StarEnemy>"_w, name.get());
                                }()), 0, pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseTravelDialog>(this));
                            }
                        } else {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.Repair.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseRepairDialog>(this));
                        }
                        if (aPlayer::GetPlayer()->CountProgramRewardStocks() > 0) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.Programms.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseProgramsDialog>(this));
                        }
                    }
                    break;
                }
                case aGalaxyStruct::rstScienceBase: {
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowScienceBaseImprovementDialog>(this));
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Repair.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowScienceBaseRepairDialog>(this));
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Satellite.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowScienceBaseSatelliteOfferDialog>(this));
                    if (aGalaxy::Galaxy->HasUnresolvedDominatorSeries(static_cast<aGalaxy::TDominatorSeriesSet>(AllSeries)) && static_cast<std::uint8_t>(aGalaxy::Galaxy->IsDominatorResearchComplete(static_cast<aGalaxy::TDominatorSeriesSet>(AllSeries)) ^ 1)) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowScienceBaseResearchDialog>(this));
                    }
                    if (aGalaxy::Galaxy->IsDominatorSeriesUnresolved(aGalaxyStruct::dsBlazer) && aGalaxy::Galaxy->IsDominatorResearchComplete(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}})) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->HasProgram(aGalaxyStruct::prgLogicalNegation) ^ 1)) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerBuyTechBlazer"_wref.get())}), 1, pas::bind_method<&TfRuinsTalk::BuyScienceBaseResearchProgram>(this));
                    }
                    if (aGalaxy::Galaxy->IsDominatorSeriesUnresolved(aGalaxyStruct::dsKeller) && aGalaxy::Galaxy->IsDominatorResearchComplete(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsKeller}})) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->HasProgram(aGalaxyStruct::prgDematerial) ^ 1)) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerBuyTechKeller"_wref.get())}), 2, pas::bind_method<&TfRuinsTalk::BuyScienceBaseResearchProgram>(this));
                    }
                    if (aGalaxy::Galaxy->IsDominatorSeriesUnresolved(aGalaxyStruct::dsTerron) && aGalaxy::Galaxy->IsDominatorResearchComplete(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsTerron}})) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->HasProgram(aGalaxyStruct::prgEnergotron) ^ 1)) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerBuyTechTerron"_wref.get())}), 3, pas::bind_method<&TfRuinsTalk::BuyScienceBaseResearchProgram>(this));
                    }
                    if (aGalaxy::Galaxy->CurrentTurn - 300 < 120) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsSB.History.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowScienceBaseHistoryDialog>(this));
                    }
                    break;
                }
                case aGalaxyStruct::rstBusinessCenter: {
                    if (aPlayer::GetPlayer()->DebtAmount == 0) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowBusinessCenterDebtDialog>(this));
                    } else if (aPlayer::GetPlayer()->Money > aPlayer::GetPlayer()->DebtAmount) {
                        AddChoice(([&] {
                            pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->DebtAmount);
                            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.RetDebt.PlayerSend"_wref.get())});
                            return aMyFunction::FormatText1(std::move(cpp_arg_4), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
                        }()), 0, pas::bind_method<&TfRuinsTalk::RepayBusinessCenterDebt>(this));
                    } else {
                        GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_2 = fTalk::ScriptDialogBlockCallback;
                        pas::WideString formatText1 = ([&] {
                            pas::WideString intToStr_3 = pas::wide_int_to_str(aPlayer::GetPlayer()->DebtAmount);
                            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.RetDebt.PlayerSend"_wref.get())});
                            return aMyFunction::FormatText1(std::move(cpp_arg_5), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_3));
                        }());
                        AddChoice(std::move(formatText1), 0, scriptDialogBlockCallback_2);
                    }
                    if (aPlayer::GetPlayer()->DebtDefaultCount < 3) {
                        if (aPlayer::GetPlayer()->DepositAmount == 0) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Deposit.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowBusinessCenterDepositDialog>(this));
                        } else if (aGalaxy::Galaxy->CurrentTurn - aPlayer::GetPlayer()->DepositStartTurn > 30) {
                            AddChoice(([&] {
                                pas::WideString intToStr_4 = pas::wide_int_to_str(aPlayer::GetPlayer()->ComputeDepositAccruedValue());
                                pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.RetDeposit.PlayerSend"_wref.get())});
                                return aMyFunction::FormatText1(std::move(cpp_arg_6), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_4));
                            }()), 0, pas::bind_method<&TfRuinsTalk::WithdrawBusinessCenterDeposit>(this));
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_3 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString formatText1_2 = ([&] {
                                pas::WideString intToStr_5 = pas::wide_int_to_str(aPlayer::GetPlayer()->ComputeDepositAccruedValue());
                                pas::WideString cpp_arg_7 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.RetDeposit.PlayerSend"_wref.get())});
                                return aMyFunction::FormatText1(std::move(cpp_arg_7), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_5));
                            }());
                            AddChoice(std::move(formatText1_2), 0, scriptDialogBlockCallback_3);
                        }
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Investment.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowBusinessCenterInvestmentDialog>(this));
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Trade.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowBusinessCenterTradeDialog>(this));
                        if (aPlayer::GetPlayer()->MedicalPolicyTicks <= 0) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Policy.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowBusinessCenterMedicalPolicyDialog>(this));
                        }
                    }
                    break;
                }
                case aGalaxyStruct::rstMedicalBase: {
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowMedicalCenterIllnessTreatmentDialog>(this));
                    AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowMedicalCenterStimulantDialog>(this));
                    break;
                }
                case aGalaxyStruct::rstDominion: {
                    if (aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.ToHangar"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::OpenHangar>(this));
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CancelFly"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionCancelTravelDialog>(this));
                    } else {
                        if (aPlayer::GetPlayer()->PirateLicenseTicks == 0) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionPirateLicenseDialog>(this));
                        } else if (aPlayer::GetPlayer()->PirateLicenseTicks < 305) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.PlayerSendProlongate"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionPirateLicenseDialog>(this));
                        }
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionShipConstructionDialog>(this));
                        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionImprovementDialog>(this));
                        if (aPlayer::GetPlayer()->DockedTo->InNormalSpace() && aPlayer::GetPlayer()->DockedTo->CurrentStar->Dominion == aPlayer::GetPlayer()->DockedTo) {
                            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionTravelDialog>(this));
                            if (aGalaxy::Galaxy->PirateWinType != 3) {
                                AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::I_CBWarWithKlingAndCoalition>(this));
                            }
                        } else {
                            {
                                GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_4 = fTalk::ScriptDialogBlockCallback;
                                pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.PlayerSend"_wref.get())});
                                AddChoice(std::move(cpp_arg_8), 0, scriptDialogBlockCallback_4);
                            }
                            if (aGalaxy::Galaxy->PirateWinType != 3) {
                                GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_5 = fTalk::ScriptDialogBlockCallback;
                                pas::WideString cpp_arg_9 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.PlayerAsk"_wref.get())});
                                AddChoice(std::move(cpp_arg_9), 0, scriptDialogBlockCallback_5);
                            }
                        }
                    }
                    break;
                }
            }
        }
        if (pas::class_cast_if<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo) != nullptr && pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SpecialServiceActive) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Ask"}))}), 0, pas::bind_method<&TfRuinsTalk::ShowStationSpecialShipDialog>(this));
        }
        if (StationBridgeMode == 0) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.GN.Modern.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::OpenStationModernization>(this));
        }
    }

    void TfRuinsTalk::ContinueScriptDialog() {
        aScript::CurrentScript->ExecuteDialogAnswer(aScript::CurrentScript->CurrentAnswer);
        M_Main(true);
    }

    void TfRuinsTalk::AddScriptTakeoffChoice(pas::WideString Caption) {
        if (Caption == u"") {
            pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.I_TakeOff"_wref.get())});
            std::int32_t currentAnswer = aScript::CurrentScript->CurrentAnswer;
            AddChoice(std::move(cpp_arg), currentAnswer, pas::bind_method<&TfRuinsTalk::RunScriptTakeoff>(this));
        } else {
            AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfRuinsTalk::RunScriptTakeoff>(this));
        }
    }

    void TfRuinsTalk::AddScriptNewsExitChoice(pas::WideString Caption) {
        AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfRuinsTalk::RunScriptNewsExit>(this));
    }

    void TfRuinsTalk::AddScriptHangarChoice(pas::WideString Caption) {
        AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfRuinsTalk::RunScriptHangar>(this));
    }

    void TfRuinsTalk::AddScriptGoodsChoice(pas::WideString Caption) {
        AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfRuinsTalk::RunScriptGoods>(this));
    }

    void TfRuinsTalk::AddScriptGameEndChoice(pas::WideString Caption) {
        AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfRuinsTalk::RunScriptGameEnd>(this));
    }

    // Choice callback value carries a TScript pointer.
    void TfRuinsTalk::SelectScriptDialog(std::int32_t ScriptValue) {
        ClearChoices();
        aScript::CurrentScript = reinterpret_cast<aScript::TScript*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(ScriptValue)));
        aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
    }

    void TfRuinsTalk::RunScriptAnswer(std::int32_t Answer) {
        ClearChoices();
        Globals::ScriptDialogIndex = -1;
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        if (Globals::ScriptDialogIndex < 0) {
            GR_Main::RaiseWideMessage(u"I_Script"_wref.get());
        }
        aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
    }

    void TfRuinsTalk::RunScriptAnswerKeepingScroll(std::int32_t Answer) {
        RememberChoiceScroll();
        RunScriptAnswer(Answer);
    }

    void TfRuinsTalk::RunScriptTakeoff(std::int32_t Answer) {
        GR_Main::CaptureSavePreview();
        fGalaxy2::CaptureGalaxyPreview(this);
        GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
        aSaveLoad::SaveGameToFile(fSaveManager::TfSaveManager::GetAutoSavePath(), u"as"_w);
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        if (!fHangar::TfHangar::TryTakeOff()) {
            GlobalsV::RequestedScreenId = GlobalsV::screenHangar;
        }
        RequestClose(1);
    }

    void TfRuinsTalk::RunScriptNewsExit(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        StationPanel->InformationClicked(nullptr);
    }

    void TfRuinsTalk::RunScriptHangar(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        StationPanel->HangarClicked(nullptr);
    }

    void TfRuinsTalk::OpenHangar(std::int32_t Action) {
        StationPanel->HangarClicked(nullptr);
    }

    void TfRuinsTalk::RunScriptGoods(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        StationPanel->GoodsShopClicked(nullptr);
    }

    void TfRuinsTalk::RunScriptGameEnd(std::int32_t Answer) {
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        GlobalsV::GameEndReason = GlobalsV::gerDefault;
        GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
        RequestClose(1);
    }

    void TfRuinsTalk::ReturnToMain(std::int32_t Action) {
        M_Main(false);
    }

    // Zero requests a quote; a nonzero quote is charged and enables sponsorship. Sponsored stations open the equipment-refit screen.
    void TfRuinsTalk::OpenStationModernization(std::int32_t QuotedCost) {
        pas::WideString Text{};
        std::int32_t Cost = 0;
        aRuins::TRuins* Station = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo);
        if (static_cast<std::uint32_t>(QuotedCost) > 0) {
            aPlayer::GetPlayer()->SetMoney(std::max<std::int32_t>(0, aPlayer::GetPlayer()->Money - System::Trunc(static_cast<std::uint32_t>(QuotedCost))));
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            Station->ModernizationSponsor = true;
        } else {
            Cost = Station->CalculateEquippedItemCostWithoutHull();
        }
        if (Station->ModernizationSponsor) {
            Globals::ShipScreen->ShipToInspect = aPlayer::GetPlayer()->DockedTo;
            MainPanel->ShipClicked(nullptr);
            Globals::ShipScreen->ShipToInspect = nullptr;
            MainPanel->RebuildMessageButtons(false);
            MainPanel->RefreshMoneyAndCargo();
            M_Main(false);
        } else {
            switch (aPlayer::GetPlayer()->DockedTo->TypeId) {
                case aGalaxyStruct::rstPirateBase: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Modern.Answer"_wref.get());
                    break;
                }
                case aGalaxyStruct::rstMilitaryBase: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Modern.Answer"_wref.get());
                    break;
                }
                case aGalaxyStruct::rstDominion: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.CB.Modern.Answer"_wref.get());
                    break;
                }
                default: DialogText = aConst::LocalizedColorText(u"FormRuins.GN.Modern.Answer"_wref.get()); break;
            }
            DialogText = aMyFunction::FormatText1(DialogText, u"<color=255,240,100>"_w, u"<Money>"_w, pas::wide_int_to_str(Cost));
            ClearChoices();
            Text = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.GN.Modern.PlayerOk"_wref.get())});
            if (aPlayer::GetPlayer()->Money >= Cost) {
                AddChoice(Text, Cost, pas::bind_method<&TfRuinsTalk::OpenStationModernization>(this));
            } else {
                AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
            }
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.GN.Modern.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineStationModernization>(this));
        }
    }

    void TfRuinsTalk::DeclineStationModernization(std::int32_t Action) {
        switch (aPlayer::GetPlayer()->DockedTo->TypeId) {
            case aGalaxyStruct::rstPirateBase: {
                DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Modern.AfterNo"_wref.get());
                break;
            }
            case aGalaxyStruct::rstMilitaryBase: {
                DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Modern.AfterNo"_wref.get());
                break;
            }
            case aGalaxyStruct::rstDominion: {
                DialogText = aConst::LocalizedColorText(u"FormRuins.CB.Modern.AfterNo"_wref.get());
                break;
            }
            default: DialogText = aConst::LocalizedColorText(u"FormRuins.GN.Modern.AfterNo"_wref.get()); break;
        }
        M_Main(true);
    }

    // Deposits every carried node stack.
    void TfRuinsTalk::DepositNodesAtRangerCenter(std::int32_t Action) {
        std::int32_t Count = aPlayer::GetPlayer()->GetCarriedNodeCount();
        aPlayer::GetPlayer()->DepositCarriedNodes();
        static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckNodesAchievement();
        aGalaxy::Galaxy->RefreshRangerRatingPlaces();
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.SaleNod.RCAnswer"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(Count), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<BaseNod>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->BaseNodes), u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowRangerCenterTakeNodeDialog(std::int32_t Action) {
        std::int32_t I{};
        pas::WideString Text{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.RCAnswer"_wref.get());
        for (I = 0; I <= 50; ++I) {
            NodeExchangeLowPriorityModule = aRuins::TRuins_SelectServiceMicroModule(reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo), 2, I, false);
            if (aPlayer::GetPlayer()->NeedsMicroModule(NodeExchangeLowPriorityModule + 1)) {
                break;
            }
        }
        NodeExchangeLowPriorityCost = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[NodeExchangeLowPriorityModule].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
        {
            std::int32_t round = System::Round(NodeExchangeLowPriorityCost * 1.2L);
            std::int32_t round_2 = System::Round(NodeExchangeLowPriorityCost * 0.8L);
            NodeExchangeLowPriorityCost = aMyFunction::SeededRandomIntRange(round_2, round, NodeExchangeLowPriorityCost + aPlayer::GetPlayer()->DockedTo->Id);
        }
        NodeExchangeLowPriorityCost = aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(NodeExchangeLowPriorityCost, 1.5L));
        Text = aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.RCAnswerBig"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(NodeExchangeLowPriorityCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, aConst::MicroModuleTemplates[NodeExchangeLowPriorityModule].Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Text>"_w, aItem::GetMicroModuleInfoText(NodeExchangeLowPriorityModule, u"<color=255,240,100>"_w), pas::WideString());
        DialogText = pas::concat_wide({DialogText, u"\r\n", Text});
        for (I = 0; I <= 50; ++I) {
            NodeExchangeMediumPriorityModule = aRuins::TRuins_SelectServiceMicroModule(reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo), 1, I, false);
            if (aPlayer::GetPlayer()->NeedsMicroModule(NodeExchangeMediumPriorityModule + 1)) {
                break;
            }
        }
        NodeExchangeMediumPriorityCost = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[NodeExchangeMediumPriorityModule].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
        {
            std::int32_t round_3 = System::Round(NodeExchangeMediumPriorityCost * 1.2L);
            std::int32_t round_4 = System::Round(NodeExchangeMediumPriorityCost * 0.8L);
            NodeExchangeMediumPriorityCost = aMyFunction::SeededRandomIntRange(round_4, round_3, NodeExchangeMediumPriorityCost + aPlayer::GetPlayer()->DockedTo->Id);
        }
        NodeExchangeMediumPriorityCost = aMyFunction::RoundAndTruncateToHundreds(pas::real_min<pas::Extended>(static_cast<pas::Extended>(NodeExchangeLowPriorityCost / 2), pas::real_divide(NodeExchangeMediumPriorityCost, 1.5L)));
        Text = aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.RCAnswerAverage"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(NodeExchangeMediumPriorityCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, aConst::MicroModuleTemplates[NodeExchangeMediumPriorityModule].Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Text>"_w, aItem::GetMicroModuleInfoText(NodeExchangeMediumPriorityModule, u"<color=255,240,100>"_w), pas::WideString());
        DialogText = pas::concat_wide({DialogText, u"\r\n", Text});
        for (I = 0; I <= 50; ++I) {
            NodeExchangeHighPriorityModule = aRuins::TRuins_SelectServiceMicroModule(reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo), 0, I, false);
            if (aPlayer::GetPlayer()->NeedsMicroModule(NodeExchangeHighPriorityModule + 1)) {
                break;
            }
        }
        NodeExchangeHighPriorityCost = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[NodeExchangeHighPriorityModule].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
        {
            std::int32_t round_5 = System::Round(NodeExchangeHighPriorityCost * 1.2L);
            std::int32_t round_6 = System::Round(NodeExchangeHighPriorityCost * 0.8L);
            NodeExchangeHighPriorityCost = aMyFunction::SeededRandomIntRange(round_6, round_5, NodeExchangeHighPriorityCost + aPlayer::GetPlayer()->DockedTo->Id);
        }
        NodeExchangeHighPriorityCost = aMyFunction::RoundAndTruncateToTens(pas::real_min<pas::Extended>(static_cast<pas::Extended>(NodeExchangeMediumPriorityCost / 2), pas::real_divide(NodeExchangeHighPriorityCost, 1.5L)));
        Text = aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.RCAnswerSmall"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(NodeExchangeHighPriorityCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, aConst::MicroModuleTemplates[NodeExchangeHighPriorityModule].Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Text>"_w, aItem::GetMicroModuleInfoText(NodeExchangeHighPriorityModule, u"<color=255,240,100>"_w), pas::WideString());
        DialogText = pas::concat_wide({DialogText, u"\r\n", Text});
        DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.RCAnswerEnd"_wref.get())});
        aMyFunction::ReplaceTextToken(DialogText, u"<BaseNod>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->BaseNodes), u"<color=255,240,100>"_w);
        ClearChoices();
        Text = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.PlayerOk"_wref.get())});
        aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(NodeExchangeLowPriorityCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, aConst::MicroModuleTemplates[NodeExchangeLowPriorityModule].Name, u"<color=255,240,100>"_w);
        if (aPlayer::GetPlayer()->BaseNodes >= NodeExchangeLowPriorityCost) {
            AddChoice(Text, 3, pas::bind_method<&TfRuinsTalk::BuyRangerCenterMicroModule>(this));
        } else {
            AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
        }
        Text = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.PlayerOk"_wref.get())});
        aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(NodeExchangeMediumPriorityCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, aConst::MicroModuleTemplates[NodeExchangeMediumPriorityModule].Name, u"<color=255,240,100>"_w);
        if (aPlayer::GetPlayer()->BaseNodes >= NodeExchangeMediumPriorityCost) {
            AddChoice(Text, 2, pas::bind_method<&TfRuinsTalk::BuyRangerCenterMicroModule>(this));
        } else {
            AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
        }
        Text = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.PlayerOk"_wref.get())});
        aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(NodeExchangeHighPriorityCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Name>"_w, aConst::MicroModuleTemplates[NodeExchangeHighPriorityModule].Name, u"<color=255,240,100>"_w);
        if (aPlayer::GetPlayer()->BaseNodes >= NodeExchangeHighPriorityCost) {
            AddChoice(Text, 1, pas::bind_method<&TfRuinsTalk::BuyRangerCenterMicroModule>(this));
        } else {
            AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::DeclineRangerCenterNodeDeposit>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.PlayerNo"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
    }

    void TfRuinsTalk::BuyRangerCenterMicroModule(std::int32_t Action) {
        std::int32_t Cost{};
        std::int32_t ModuleIndex{};
        switch (Action) {
            case 1: {
                ModuleIndex = NodeExchangeHighPriorityModule;
                Cost = NodeExchangeHighPriorityCost;
                break;
            }
            case 2: {
                ModuleIndex = NodeExchangeMediumPriorityModule;
                Cost = NodeExchangeMediumPriorityCost;
                break;
            }
            case 3: {
                ModuleIndex = NodeExchangeLowPriorityModule;
                Cost = NodeExchangeLowPriorityCost;
                break;
            }
            default: {
                GR_Main::RaiseWideMessage(u"\u041a\u043e\u0441\u044f\u043a\u0438.TfRuinsTalk.I_TakeNodPlayerOk"_wref.get());
                return;
            }
        }
        std::int32_t BaseNodes = aPlayer::GetPlayer()->BaseNodes;
        aPlayer::GetPlayer()->BaseNodes -= Cost;
        if (aPlayer::GetPlayer()->BaseNodes != BaseNodes - Cost && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
        BaseNodes = aPlayer::GetPlayer()->BaseNodes;
        aItem::TMicroModule* Item = pas::construct_call<aItem::TMicroModule>(aItem::TEquipment_Create);
        Item->Init(ModuleIndex);
        pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item));
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.RCAfterPlayerOk"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<NodCnt>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Name>"_w, aConst::MicroModuleTemplates[ModuleIndex].Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<RC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        aGalaxyEvent::TGalaxyEvent* Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerReceivesMM"_w, nullptr);
        Event->AddData(Item->Id);
        Event->AddData(ModuleIndex);
        Event->AddData(Cost);
        SysUtilsImports::Sleep(1u);
        if (aPlayer::GetPlayer()->BaseNodes != BaseNodes && static_cast<std::uint8_t>(GR_Main::CCInterface->GetTamperDetected() ^ 1)) {
            GR_Main::CCInterface->SetTamperDetected(true);
        }
        M_Main(true);
    }

    void TfRuinsTalk::DeclineRangerCenterNodeDeposit(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.TakeNod.RCAfterPlayerNo"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowRangerCenterGiveNodeDialog(std::int32_t Action) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aItem::TMicroModule* Module{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.Answer"_wref.get());
        DialogText = pas::concat_wide({DialogText, u"\r\n", u"----------------------------"});
        std::int32_t Attempts = 1;
        do {
            ++Attempts;
            if ((static_cast<std::uint32_t>(aGalaxy::Galaxy->CurrentTurn) + aPlayer::GetPlayer()->RandomState) % 33 == 0) {
                NodeExchangeLowPriorityModule = aGalaxy::TGalaxy::SelectMicroModule(0, 20, 17 * Attempts + (aGalaxy::Galaxy->CurrentTurn / 57 + 2938629) + aPlayer::GetPlayer()->DockedTo->Id, aPlayer::GetPlayer()->DockedTo);
            } else {
                NodeExchangeLowPriorityModule = aGalaxy::TGalaxy::SelectMicroModule(10, 30, 17 * Attempts + (aGalaxy::Galaxy->CurrentTurn / 57 + 32465621) + aPlayer::GetPlayer()->DockedTo->Id, aPlayer::GetPlayer()->DockedTo);
            }
            if (Attempts > 50) {
                break;
            }
        } while (!aPlayer::GetPlayer()->NeedsMicroModule(NodeExchangeLowPriorityModule + 1));
        NodeExchangeLowPriorityCost = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[NodeExchangeLowPriorityModule].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
        {
            std::int32_t round = System::Round(NodeExchangeLowPriorityCost * 1.2L);
            std::int32_t round_2 = System::Round(NodeExchangeLowPriorityCost * 0.8L);
            NodeExchangeLowPriorityCost = aMyFunction::SeededRandomIntRange(round_2, round, NodeExchangeLowPriorityCost + aPlayer::GetPlayer()->DockedTo->Id);
        }
        NodeExchangeLowPriorityCost = aMyFunction::RoundAndTruncateToHundreds(pas::real_divide(NodeExchangeLowPriorityCost, 1.5L));
        Attempts = 1;
        do {
            ++Attempts;
            NodeExchangeMediumPriorityModule = aGalaxy::TGalaxy::SelectMicroModule(31, 69, 17 * Attempts + (aGalaxy::Galaxy->CurrentTurn / 57 + 2351417) + aPlayer::GetPlayer()->DockedTo->Id, aPlayer::GetPlayer()->DockedTo);
            if (Attempts > 50) {
                break;
            }
        } while (!aPlayer::GetPlayer()->NeedsMicroModule(NodeExchangeMediumPriorityModule + 1));
        NodeExchangeMediumPriorityCost = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[NodeExchangeMediumPriorityModule].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
        {
            std::int32_t round_3 = System::Round(NodeExchangeMediumPriorityCost * 1.2L);
            std::int32_t round_4 = System::Round(NodeExchangeMediumPriorityCost * 0.8L);
            NodeExchangeMediumPriorityCost = aMyFunction::SeededRandomIntRange(round_4, round_3, NodeExchangeMediumPriorityCost + aPlayer::GetPlayer()->DockedTo->Id);
        }
        NodeExchangeMediumPriorityCost = aMyFunction::RoundAndTruncateToHundreds(pas::real_min<pas::Extended>(static_cast<pas::Extended>(NodeExchangeLowPriorityCost / 2), pas::real_divide(NodeExchangeMediumPriorityCost, 1.5L)));
        Attempts = 1;
        do {
            NodeExchangeHighPriorityModule = aGalaxy::TGalaxy::SelectMicroModule(70, 100, 17 * Attempts + aGalaxy::Galaxy->CurrentTurn / 57 + aPlayer::GetPlayer()->DockedTo->Id, aPlayer::GetPlayer()->DockedTo);
            ++Attempts;
            if (Attempts > 50) {
                break;
            }
        } while (!aPlayer::GetPlayer()->NeedsMicroModule(NodeExchangeHighPriorityModule + 1));
        NodeExchangeHighPriorityCost = System::Round(aMyFunction::RemapClamped(aConst::MicroModuleTemplates[NodeExchangeHighPriorityModule].Priority, 0.0, 1.0E+2, 2.0E+3, 1.0E+2));
        {
            std::int32_t round_5 = System::Round(NodeExchangeHighPriorityCost * 1.2L);
            std::int32_t round_6 = System::Round(NodeExchangeHighPriorityCost * 0.8L);
            NodeExchangeHighPriorityCost = aMyFunction::SeededRandomIntRange(round_6, round_5, NodeExchangeHighPriorityCost + aPlayer::GetPlayer()->DockedTo->Id);
        }
        NodeExchangeHighPriorityCost = aMyFunction::RoundAndTruncateToTens(pas::real_min<pas::Extended>(static_cast<pas::Extended>(NodeExchangeMediumPriorityCost / 2), pas::real_divide(NodeExchangeHighPriorityCost, 1.5L)));
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->ItemType == aConst::t_MicroModule) {
                Module = pas::checked_cast<aItem::TMicroModule*>(Item);
                DialogText = pas::concat_wide({DialogText, u"\r\n", ([&] {
                    pas::WideString highlightedName = Module->GetHighlightedName();
                    pas::WideString intToStr = pas::wide_int_to_str(Module->CalculateNodeExchangeValue(NodeExchangeLowPriorityCost, NodeExchangeMediumPriorityCost));
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.Nod"_wref.get());
                    return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(highlightedName), u"<Count>"_w, std::move(intToStr));
                }())});
                ++Count;
            }
        }
        if (Count > 0) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"----------------------------"});
            DialogText = pas::concat_wide({DialogText, u"\r\n", ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->BaseNodes);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.Sum"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<BaseNod>"_w, std::move(intToStr_2));
            }())});
        } else {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.Nothing"_wref.get())});
        }
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineRangerCenterNodeReward>(this));
        if (Count > 0) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Inventory, I);
                if (Item->ItemType == aConst::t_MicroModule) {
                    Module = pas::checked_cast<aItem::TMicroModule*>(Item);
                    {
                        pas::WideString formatText2 = ([&] {
                            pas::WideString highlightedName_2 = Module->GetHighlightedName();
                            pas::WideString intToStr_3 = pas::wide_int_to_str(Module->CalculateNodeExchangeValue(NodeExchangeLowPriorityCost, NodeExchangeMediumPriorityCost));
                            pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.PlayerOk"_wref.get())});
                            return aMyFunction::FormatText2(std::move(cpp_arg), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(highlightedName_2), u"<Count>"_w, std::move(intToStr_3));
                        }());
                        std::int32_t id = Module->Id;
                        AddChoice(std::move(formatText2), id, pas::bind_method<&TfRuinsTalk::ExchangeMicroModuleForNodes>(this));
                    }
                }
            }
        }
    }

    void TfRuinsTalk::ExchangeMicroModuleForNodes(std::int32_t Action) {
        std::int32_t I{};
        aItem::TItem* Item{};
        aItem::TMicroModule* Module = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->ItemType == aConst::t_MicroModule) {
                Module = pas::checked_cast<aItem::TMicroModule*>(Item);
                if (Module->Id == Action) {
                    break;
                }
                Module = nullptr;
            }
        }
        if (Module == nullptr) {
            GR_Main::RaiseWideMessage(static_cast<pas::WideString>(pas::concat_ansi({"\312\356\361\377\352: TfRuinsTalk.I_GiveNodOk [FId = ", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Action)), "]"})));
        }
        std::int32_t Value = Module->CalculateNodeExchangeValue(NodeExchangeLowPriorityCost, NodeExchangeMediumPriorityCost);
        DialogText = ([&] {
            pas::WideString highlightedName = Module->GetHighlightedName();
            pas::WideString intToStr = pas::wide_int_to_str(Value);
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.AfterOk"_wref.get());
            return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(highlightedName), u"<Count>"_w, std::move(intToStr));
        }());
        pas::list_delete(aPlayer::GetPlayer()->Inventory, pas::list_indexof(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Module)));
        pas::free(Module);
        aPlayer::GetPlayer()->BaseNodes += Value;
        static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckNodesAchievement();
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        ShowRangerCenterGiveNodeDialog(Action);
    }

    void TfRuinsTalk::DeclineRangerCenterNodeReward(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.GiveNod.AfterNo"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowRangerCenterNodeInfo(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.AboutNod.RCAnswer"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Percent>"_w, pas::wide_int_to_str(30), u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowRangerCenterNodeInfoContinuation>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.I_Continue"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
    }

    void TfRuinsTalk::ShowRangerCenterNodeInfoContinuation(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.AboutNod.RCAnswerAdd"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Percent>"_w, pas::wide_int_to_str(30), u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowRangerCenterRatingAnswer(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.Rating.RCAnswer"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowRangerCenterPirateClanAnswer(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.PirateClan.RCAnswer"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowRangerCenterBestRangerAnswer(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.RC.BestRanger.RCAnswer"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowPirateBaseNationalityDialog(std::int32_t Action) {
        pas::WideString Text{};
        std::int32_t I{};
        std::int32_t DE{};
        std::int32_t CDE{};
        std::int32_t BCDE{};
        std::int32_t MinimumCost{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.AnswerChangeNationality"_wref.get());
        Text = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PBNext"_wref.get());
        double Factor = 1.0;
        {
            const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + 365 < aGalaxy::Galaxy->CurrentTurn) {
                        break;
                    }
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType == u"PlayerChangesNationality") {
                        Factor = Factor * 1.5L;
                    }
                }
            }
        }
        aMyFunction::ReplaceTextToken(Text, u"<MoneyMaloc>"_w, pas::wide_int64_to_str(System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiMaloc)) * Factor)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<MoneyPeleng>"_w, pas::wide_int64_to_str(System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiPeleng)) * Factor)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<MoneyPeople>"_w, pas::wide_int64_to_str(System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiHuman)) * Factor)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<MoneyFei>"_w, pas::wide_int64_to_str(System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiFeyan)) * Factor)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<MoneyGaal>"_w, pas::wide_int64_to_str(System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiGaal)) * Factor)), u"<color=255,240,100>"_w);
        DialogText = pas::concat_wide({DialogText, Text});
        ClearChoices();
        std::int32_t A = aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiMaloc);
        std::int32_t B = aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiPeleng);
        std::int32_t C = aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiHuman);
        std::int32_t D = aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiFeyan);
        std::int32_t E = aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiGaal);
        if (D < E) {
            DE = D;
        } else {
            DE = E;
        }
        if (C < DE) {
            CDE = C;
        } else {
            CDE = DE;
        }
        if (B < CDE) {
            BCDE = B;
        } else {
            BCDE = CDE;
        }
        if (A < BCDE) {
            MinimumCost = A;
        } else {
            MinimumCost = BCDE;
        }
        if (aPlayer::GetPlayer()->Money >= System::Round(static_cast<long double>(MinimumCost) * Factor)) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::AcceptPirateBaseNationality>(this));
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PlayerOk"_wref.get())});
            AddChoice(std::move(cpp_arg), 0, scriptDialogBlockCallback);
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclinePirateBaseNationality>(this));
    }

    void TfRuinsTalk::AcceptPirateBaseNationality(std::int32_t Action) {
        static const pas::Set<0, 255> RelationShipTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htRanger}, {aGalaxyStruct::htPirate, aGalaxyStruct::htDiplomat}});
        std::int32_t RangerIndex{};
        std::int32_t J{};
        std::uint8_t Relation{};
        std::int32_t I{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        double Factor = 1.0;
        {
            const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + 365 < aGalaxy::Galaxy->CurrentTurn) {
                        break;
                    }
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType == u"PlayerChangesNationality") {
                        Factor = Factor * 1.5L;
                    }
                }
            }
        }
        Globals::SelectFaceScreen->PlayerRace = aPlayer::GetPlayer()->PilotRace;
        Globals::SelectFaceScreen->CaptainPortraitIndex = aPlayer::GetPlayer()->PortraitFaceId;
        Globals::SelectFaceScreen->PlayerName = aPlayer::GetPlayer()->Name;
        Globals::SelectFaceScreen->NationalityCosts[aGalaxyStruct::oiMaloc] = System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiMaloc)) * Factor);
        Globals::SelectFaceScreen->NationalityCosts[aGalaxyStruct::oiPeleng] = System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiPeleng)) * Factor);
        Globals::SelectFaceScreen->NationalityCosts[aGalaxyStruct::oiHuman] = System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiHuman)) * Factor);
        Globals::SelectFaceScreen->NationalityCosts[aGalaxyStruct::oiFeyan] = System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiFeyan)) * Factor);
        Globals::SelectFaceScreen->NationalityCosts[aGalaxyStruct::oiGaal] = System::Round(static_cast<long double>(aGalaxy::Galaxy->ComputeScaledBigMoney(aGalaxyStruct::oiGaal)) * Factor);
        Globals::SelectFaceScreen->AvailableMoney = aPlayer::GetPlayer()->Money;
        aGalaxy::Galaxy->PrimeIntegrityChecksum(320);
        if (fSelectFace::RunSelectFaceDialog(this)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(321);
            aPlayer::GetPlayer()->PortraitFaceId = Globals::SelectFaceScreen->CaptainPortraitIndex;
            aPlayer::GetPlayer()->PilotRace = Globals::SelectFaceScreen->PlayerRace;
            if (aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                aPlayer::GetPlayer()->OwnerId = aConst::RaceToOwner(Globals::SelectFaceScreen->PlayerRace);
            }
            aPlayer::GetPlayer()->Name = Globals::SelectFaceScreen->PlayerName;
            Globals::LastLoadedPlayerName = aPlayer::GetPlayer()->Name;
            aPlayer::GetPlayer()->SetMoney(std::max<std::int32_t>(0, aPlayer::GetPlayer()->Money - std::max<std::int32_t>(0, Globals::SelectFaceScreen->AcceptedCost)));
            aPlayer::GetPlayer()->AddPirateCareerActivity(8);
            ++aPlayer::GetPlayer()->NationalityChangeCount;
            Achievements::TryAddAchievementProgress(u"MANYFACES"_w, 1);
            for (J = 1; J <= 12; ++J) {
                if (!aConst::CaptainHealthDefinitions[J].Disabled) {
                    if (static_cast<std::uint8_t>(pas::contains(aConst::CaptainHealthDefinitions[J].AllowedOwners, aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) ^ 1) && aPlayer::GetPlayer()->CaptainHealth[J].Progress < 1.0E+2L) {
                        aPlayer::GetPlayer()->CaptainHealth[J].Progress = 0.0;
                        aPlayer::GetPlayer()->StatusEffectSourceNames[J] = pas::WideString();
                    }
                }
            }
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmRaiseTo, 70, aConst::PlanetOwnerMasks.Coalition);
            aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmRaiseTo, 70, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), aConst::PlanetOwnerMasks.Coalition);
            if (aPlanet::MainPiratePlanet != nullptr) {
                RangerIndex = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(aPlayer::GetPlayer()));
                Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aPlanet::MainPiratePlanet->RangerRelations, RangerIndex)));
                if (Relation < 45) {
                    pas::list_put(aPlanet::MainPiratePlanet->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(45))));
                }
            }
            aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmRaiseTo, 45, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), aConst::PlanetOwnerMasks.PirateClan);
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerChangesNationality"_w, nullptr);
            Event->AddData(aPlayer::GetPlayer()->PilotRace);
            switch (Globals::SelectFaceScreen->PlayerRace) {
                case aGalaxyStruct::oiMaloc: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.AfterOperationMaloc"_wref.get());
                    break;
                }
                case aGalaxyStruct::oiPeleng: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.AfterOperationPeleng"_wref.get());
                    break;
                }
                case aGalaxyStruct::oiHuman: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.AfterOperationPeople"_wref.get());
                    break;
                }
                case aGalaxyStruct::oiFeyan: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.AfterOperationFei"_wref.get());
                    break;
                }
                case aGalaxyStruct::oiGaal: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.AfterOperationGaal"_wref.get());
                    break;
                }
            }
        } else {
            aGalaxy::Galaxy->CheckIntegrityChecksum(322);
            if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PBAfterNo"_wref.get());
            } else {
                DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PBAfterNoAlt"_wref.get());
            }
        }
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::DeclinePirateBaseNationality(std::int32_t Action) {
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PBAfterNo"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeNationality.PBAfterNoAlt"_wref.get());
        }
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowPirateBaseSideChangeDialog(std::int32_t Action) {
        std::int32_t I{};
        if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.AnswerChangeSideToNormal"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.AnswerChangeSideToPirate"_wref.get());
        }
        StationServiceQuoteCost = aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman);
        {
            const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + 365 < aGalaxy::Galaxy->CurrentTurn) {
                        break;
                    }
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType == u"PlayerChangesSide") {
                        StationServiceQuoteCost = std::min<std::int64_t>(static_cast<std::int64_t>(100000000), System::Round(StationServiceQuoteCost * 1.5L));
                    }
                }
            }
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<Cost>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptPirateBaseSideChange>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.PlayerOk"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.PlayerOk"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::DeclinePirateBaseSideChange>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.PlayerNo"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
    }

    // Recalculates the fee at acceptance time.
    void TfRuinsTalk::AcceptPirateBaseSideChange(std::int32_t Action) {
        static const pas::Set<0, 255> RelationShipTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htRanger}, {aGalaxyStruct::htPirate, aGalaxyStruct::htDiplomat}});
        std::int32_t I{};
        std::uint8_t Relation{};
        if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
            aPlayer::GetPlayer()->OwnerId = aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace);
        } else {
            aPlayer::GetPlayer()->OwnerId = aGalaxyStruct::oiPirate;
        }
        StationServiceQuoteCost = aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman);
        {
            const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->Turn + 365 < aGalaxy::Galaxy->CurrentTurn) {
                        break;
                    }
                    if (pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I)->EventType == u"PlayerChangesSide") {
                        StationServiceQuoteCost = std::min<std::int64_t>(static_cast<std::int64_t>(100000000), System::Round(StationServiceQuoteCost * 1.5L));
                    }
                }
            }
        }
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        ++aPlayer::GetPlayer()->SideChangeCount;
        Achievements::TrySetAchievementProgress(u"SIDECHANGER"_w, aPlayer::GetPlayer()->SideChangeCount);
        std::int32_t RangerIndex = pas::list_indexof(aGalaxy::Galaxy->Rangers, reinterpret_cast<void*>(aPlayer::GetPlayer()));
        if (aPlanet::MainPiratePlanet != nullptr) {
            Relation = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(pas::list_get(aPlanet::MainPiratePlanet->RangerRelations, RangerIndex)));
        } else {
            Relation = 0;
        }
        if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
            if (Relation < 45 && aPlanet::MainPiratePlanet != nullptr) {
                pas::list_put(aPlanet::MainPiratePlanet->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(45))));
            }
            aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmRaiseTo, 45, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), aConst::PlanetOwnerMasks.PirateClan);
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmCapAt, 20, aConst::PlanetOwnerMasks.Coalition);
            aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmCapAt, 20, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), aConst::PlanetOwnerMasks.Coalition);
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.AnswerPlayerOkPirate"_wref.get());
        } else {
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmRaiseTo, 45, aConst::PlanetOwnerMasks.Coalition);
            aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmRaiseTo, 45, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), aConst::PlanetOwnerMasks.Coalition);
            if (Relation > 20 && aPlanet::MainPiratePlanet != nullptr) {
                pas::list_put(aPlanet::MainPiratePlanet->RangerRelations, RangerIndex, reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(20))));
            }
            aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmCapAt, 20, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), aConst::PlanetOwnerMasks.PirateClan);
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.AnswerPlayerOkNormal"_wref.get());
        }
        aGalaxyEvent::TGalaxyEvent* Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerChangesSide"_w, nullptr);
        Event->AddData(aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate);
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::DeclinePirateBaseSideChange(std::int32_t Action) {
        if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.AnswerPlayerNoNormal"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.ChangeSide.AnswerPlayerNoPirate"_wref.get());
        }
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowPirateBaseNodeDialog(std::int32_t Action) {
        std::int32_t Count{};
        std::int32_t Cost{};
        std::int32_t DiscountedCost{};
        pas::WideString Text{};
        aRuins::TRuins* OtherBase{};
        std::uint8_t Discount{};
        if (aPlayer::GetPlayer()->DockedTo->NodeReserve > 0) {
            Count = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->GetNodeSaleBatchSize();
            Cost = aGalaxy::Galaxy->ScaleGoodsPriceByGalaxyAge(Count * 10 * 3);
            Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
            DiscountedCost = std::max<std::int64_t>(static_cast<std::int64_t>(1), static_cast<std::int64_t>(Cost - System::Round(pas::real_divide(Cost, 1.0E+2L) * Discount)));
            Text = aConst::LocalizedColorText(u"FormRuins.PB.Nod.PBStart"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(Count), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<MoneyAll>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Discount)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<MoneyDec>"_w, pas::wide_int_to_str(DiscountedCost), u"<color=255,240,100>"_w);
            DialogText = Text;
            ClearChoices();
            if (aPlayer::GetPlayer()->Money >= DiscountedCost) {
                AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Nod.PlayerOk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::BuyPirateBaseNodes>(this));
            }
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Nod.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclinePirateBaseNodes>(this));
        } else {
            Text = aConst::LocalizedColorText(u"FormRuins.PB.Nod.PBEnd"_wref.get());
            OtherBase = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FindPirateBaseWithNodes();
            if (OtherBase != nullptr) {
                Text = pas::concat_wide({Text, u"\r\n", aConst::LocalizedColorText(u"FormRuins.PB.Nod.PBEndPlus"_wref.get())});
                aMyFunction::ReplaceTextToken(Text, u"<ToSector>"_w, OtherBase->CurrentStar->Constellation->GetName(), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<ToBase>"_w, OtherBase->Name, u"<color=255,240,100>"_w);
            }
            DialogText = Text;
            ClearChoices();
            M_Main(true);
        }
    }

    void TfRuinsTalk::BuyPirateBaseNodes(std::int32_t Action) {
        aItem::TProtoplasm* Stack{};
        aItem::TItem* Item{};
        std::int32_t I{};
        std::int32_t Count = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->GetNodeSaleBatchSize();
        std::int32_t Cost = aGalaxy::Galaxy->ScaleGoodsPriceByGalaxyAge(Count * 10 * 3);
        std::uint8_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        std::int32_t DiscountedCost = std::max<std::int64_t>(static_cast<std::int64_t>(1), static_cast<std::int64_t>(Cost - System::Round(pas::real_divide(Cost, 1.0E+2L) * Discount)));
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - DiscountedCost);
        aPlayer::GetPlayer()->DockedTo->NodeReserve -= Count;
        aPlayer::GetPlayer()->AddPirateCareerActivity(4);
        std::uint8_t NewStack = true;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Inventory, I);
            if (aItem::TProtoplasm* protoplasm = pas::class_cast_if<aItem::TProtoplasm*>(Item)) {
                Stack = protoplasm;
                Stack->Init_2(Stack->StackCount + Count, 1);
                NewStack = false;
                break;
            }
        }
        if (NewStack) {
            Stack = pas::construct_call<aItem::TProtoplasm>(aItem::TEquipment_Create);
            Stack->Init_2(Count, 1);
            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Stack));
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Nod.PBSell"_wref.get());
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::DeclinePirateBaseNodes(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Nod.PBAfterNo"_wref.get());
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowPirateBaseProgramDialog(std::int32_t Action) {
        std::uint8_t I{};
        pas::WideString Text{};
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(11)); cpp_range.next(I); ) {
            PirateProgramQuoteCosts[I] = 0;
        }
        std::int32_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        Text = aConst::LocalizedColorText(u"FormRuins.PB.Program.PBStart"_wref.get());
        Text = aMyFunction::FormatText1(Text, u"<color=255,240,100>"_w, u"<Percent>"_w, aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Discount)), u"<color=255,240,100>"sv));
        Text = aMyFunction::FormatText1(Text, u"<color=255,240,100>"_w, u"<NodTrum>"_w, aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr))), u"<color=255,240,100>"sv));
        Text = aMyFunction::FormatText1(Text, u"<color=255,240,100>"_w, u"<NodAcc>"_w, aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->BaseNodes)), u"<color=255,240,100>"sv));
        Text = pas::concat_wide({Text, u"\r\n"});
        for (auto cpp_range_2 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(11)); cpp_range_2.next(I); ) {
            if (aConst::PirateProgramBatchSizes[I] != 0) {
                Text = pas::concat_wide({Text, aMyFunction::WrapTextInColor(pas::view((static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(I))), u"<color=255,240,100>"sv), u" - "});
                Text = pas::concat_wide({Text, ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aConst::PirateProgramBatchSizes[I]);
                    pas::WideString localizedText = aConst::LocalizedText(pas::concat_wide({u"Programms.", aConst::ProgramNames[I], u".Text"}));
                    return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr));
                }()), u"\r\n"});
                PirateProgramQuoteCosts[I] = std::max<std::int64_t>(static_cast<std::int64_t>(100), static_cast<std::int64_t>(aConst::PirateProgramBaseCosts[I] - System::Round(pas::real_divide(aConst::PirateProgramBaseCosts[I], 1.0E+2L) * Discount)));
                Text = pas::concat_wide({Text, ([&] {
                    pas::WideString intToStr_2 = pas::wide_int_to_str(PirateProgramQuoteCosts[I]);
                    pas::WideString localizedText_2 = aConst::LocalizedText(u"FormRuins.PB.Program.NodCost"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<Cost>"_w, std::move(intToStr_2));
                }())});
                Text = pas::concat_wide({Text, u"\r\n", u"\r\n"});
            }
        }
        DialogText = Text;
        ClearChoices();
        std::int32_t Nodes = aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) + aPlayer::GetPlayer()->BaseNodes;
        for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(11)); cpp_range_3.next(I); ) {
            if (aConst::PirateProgramBatchSizes[I] != 0) {
                Text = pas::concat_wide({u" - ", aConst::LocalizedColorText(u"FormRuins.PB.Program.PlayerOk"_wref.get())});
                Text = aMyFunction::FormatText1(Text, u"<color=255,240,100>"_w, u"<Nod>"_w, aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(PirateProgramQuoteCosts[I])), u"<color=255,240,100>"sv));
                Text = aMyFunction::FormatText1(Text, u"<color=255,240,100>"_w, u"<Text>"_w, aMyFunction::WrapTextInColor(pas::view((static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(I))), u"<color=255,240,100>"sv));
                if (PirateProgramQuoteCosts[I] <= Nodes) {
                    AddChoice(Text, I, pas::bind_method<&TfRuinsTalk::BuyPirateBaseProgram>(this));
                } else {
                    AddChoice(Text, 0, fTalk::ScriptDialogBlockCallback);
                }
            }
        }
        AddChoice(aConst::LocalizedColorText(u"FormRuins.PB.Program.PlayerNo"_wref.get()), 0, pas::bind_method<&TfRuinsTalk::DeclinePirateBaseProgram>(this));
    }

    void TfRuinsTalk::BuyPirateBaseProgram(std::int32_t Action) {
        std::uint8_t ProgramIndex = Action;
        std::int32_t Cost = PirateProgramQuoteCosts[ProgramIndex];
        std::int32_t CarriedNodes = aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr);
        std::int32_t BaseNodes = aPlayer::GetPlayer()->BaseNodes;
        if (CarriedNodes + BaseNodes >= Cost) {
            if (CarriedNodes > 0) {
                aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Cost, nullptr);
            }
            if (Cost > CarriedNodes) {
                aPlayer::GetPlayer()->BaseNodes = std::max<std::int32_t>(0, aPlayer::GetPlayer()->BaseNodes - (Cost - CarriedNodes));
            }
            aPlayer::GetPlayer()->ProgramCounts[ProgramIndex] += aConst::PirateProgramBatchSizes[ProgramIndex];
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            aPlayer::GetPlayer()->AddPirateCareerActivity(3);
            DialogText = ([&] {
                pas::WideString programName = (static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(ProgramIndex));
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.PB.Program.PBAfterOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Text>"_w, std::move(programName));
            }());
            ClearChoices();
            M_Main(true);
        } else {
            DeclinePirateBaseProgram(0);
        }
    }

    void TfRuinsTalk::DeclinePirateBaseProgram(std::int32_t Action) {
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Program.PBAfterNo"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Program.PBAfterNoAlt"_wref.get());
        }
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowPirateBaseRepairDialog(std::int32_t Action) {
        pas::WideString Text{};
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t NodeCost = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->EquippedFlag != 0) {
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); weapon != nullptr && weapon->GetWeaponInfo()->Availability == aGalaxyStruct::waNotSoldAndNodeRepair && Item->NeedsRepair()) {
                    if (aShip::TShip_CanRepairEquipmentTech(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo), Item)) {
                        NodeCost += aItem::TEquipment_CalculateRepairCost(Item);
                    }
                }
            }
        }
        if (NodeCost > 0) {
            NodeCost = System::Round(NodeCost * 0.0025L);
            if (NodeCost == 0) {
                NodeCost = 1;
            }
        }
        std::int32_t Cost = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->GetRepairCost(aPlayer::GetPlayer());
        std::uint8_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        std::int32_t DiscountedCost = std::max<std::int64_t>(static_cast<std::int64_t>(1), static_cast<std::int64_t>(Cost - System::Round(pas::real_divide(Cost, 1.0E+2L) * Discount)));
        if (Cost == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Repair.PBYouNotNeedRepair"_wref.get());
            ClearChoices();
            M_Main(true);
        } else {
            Text = aConst::LocalizedColorText(u"FormRuins.PB.Repair.PBYouNeedRepair"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<MoneyAll>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Percent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Discount)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<MoneyDec>"_w, pas::wide_int_to_str(DiscountedCost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<PB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            DialogText = Text;
            if (NodeCost != 0) {
                DialogText = pas::concat_wide_reverse({aConst::LocalizedColorText(u"FormRuins.PB.Repair.PBCostAnswerNeedNode"_wref.get()), DialogText});
                aMyFunction::ReplaceTextToken(DialogText, u"<NeedNode>"_w, pas::wide_int_to_str(NodeCost), u"<color=255,240,100>"_w);
            }
            ClearChoices();
            if (aPlayer::GetPlayer()->Money >= DiscountedCost) {
                if (NodeCost == 0 || NodeCost > 0 && aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) >= NodeCost) {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptPirateBaseRepair>(this);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Repair.PlayerOk"_wref.get())});
                    TfRuinsTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
                }
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclinePirateBaseRepair>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Repair.PlayerNo"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
            }
        }
    }

    void TfRuinsTalk::AcceptPirateBaseRepair(std::int32_t Action) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t NodeCost = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->EquippedFlag != 0) {
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); weapon != nullptr && weapon->GetWeaponInfo()->Availability == aGalaxyStruct::waNotSoldAndNodeRepair && Item->NeedsRepair()) {
                    if (aShip::TShip_CanRepairEquipmentTech(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo), Item)) {
                        NodeCost += aItem::TEquipment_CalculateRepairCost(Item);
                    }
                }
            }
        }
        if (NodeCost > 0) {
            NodeCost = System::Round(NodeCost * 0.0025L);
            if (NodeCost == 0) {
                NodeCost = 1;
            }
        }
        std::int32_t Cost = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->GetRepairCost(aPlayer::GetPlayer());
        std::uint8_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        std::int32_t DiscountedCost = std::max<std::int64_t>(static_cast<std::int64_t>(1), static_cast<std::int64_t>(Cost - System::Round(pas::real_divide(Cost, 1.0E+2L) * Discount)));
        if (aPlayer::GetPlayer()->Money >= DiscountedCost && aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) >= NodeCost) {
            aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), NodeCost, nullptr);
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + (Cost - DiscountedCost));
            pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RepairShipEquipment(aPlayer::GetPlayer());
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            aPlayer::GetPlayer()->AddPirateCareerActivity(4);
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Repair.PBAfterOk"_wref.get());
            GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
            ClearChoices();
            M_Main(true);
        } else {
            DeclinePirateBaseRepair(0);
        }
    }

    void TfRuinsTalk::DeclinePirateBaseRepair(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Repair.PBAfterNo"_wref.get());
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowPirateBaseSubCrackDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PBInfo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<PB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str((static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost())), u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ConfirmPirateBaseSubCrack>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PlayerContinue"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
    }

    void TfRuinsTalk::ConfirmPirateBaseSubCrack(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PBContinue"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<PB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str((static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost())), u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::BuyPirateBaseSubCrack>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr = pas::wide_int_to_str((static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost()));
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PlayerOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::BuyPirateBaseSubCrackHalfPrice>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PlayerOkHalf"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::DeclinePirateBaseSubCrack>(this);
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PlayerNo"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
        }
    }

    void TfRuinsTalk::BuyPirateBaseSubCrack(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PBAfterOk"_wref.get());
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost()));
        aMyFunction::ReplaceTextToken(DialogText, u"<PB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str((static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost())), u"<color=255,240,100>"_w);
        aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgSabCrack] = 1;
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::BuyPirateBaseSubCrackHalfPrice(std::int32_t Action) {
        std::int32_t Cost = (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::GetSubCrackCost()) / 2;
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PBAfterOkHalf"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<PB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
        aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgSabCrack] = 1;
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::DeclinePirateBaseSubCrack(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.SabCrack.PBAfterNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<PB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowPirateBaseChameleonDialog(std::int32_t Action) {
        std::uint8_t I{};
        pas::WideString Text{};
        pas::WideString SeriesName{};
        std::int32_t Value{};
        std::int32_t Cost{};
        float BaseCost = std::max<std::int32_t>(1000, aGalaxy::Galaxy->ComputeScaledBigMoney(aPlayer::GetPlayer()->DockedTo->OwnerId));
        float SeriesCost = 0.0f;
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(2)); cpp_range.next(I); ) {
            switch (I) {
                case 1: SeriesCost = BaseCost * 1.1L; break;
                case 0: SeriesCost = BaseCost * 1.2L; break;
                case 2: SeriesCost = BaseCost * 1.3L; break;
            }
            PirateChameleonQuoteCosts[I] = System::Round(SeriesCost + static_cast<long double>(aPlayer::GetPlayer()->ChameleonCharges[I]) * SeriesCost * 0.1L);
        }
        Text = pas::concat_wide({u"-----------------------", u"\r\n"});
        for (auto cpp_range_2 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(2)); cpp_range_2.next(I); ) {
            SeriesName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[I], u".0"}));
            Cost = PirateChameleonQuoteCosts[I];
            Text = pas::concat_wide({Text, ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Cost);
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PlayerOk"_wref.get());
                return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Series>"_w, SeriesName, u"<Cost>"_w, std::move(intToStr));
            }()), u"\r\n"});
        }
        Text = pas::concat_wide({Text, u"-----------------------"});
        DialogText = aMyFunction::FormatText1(aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PBAsk"_wref.get()), pas::WideString(), u"<List>"_w, Text);
        ClearChoices();
        for (auto cpp_range_3 = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(2)); cpp_range_3.next(I); ) {
            Value = I;
            SeriesName = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"ShipType.Dominator.", aConst::DominatorSeriesNames[I], u".0"}));
            Cost = PirateChameleonQuoteCosts[I];
            Text = ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PlayerOk"_wref.get());
                return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Series>"_w, SeriesName, u"<Cost>"_w, std::move(intToStr_2));
            }());
            if (aPlayer::GetPlayer()->Money >= Cost) {
                AddChoice(pas::concat_wide({u"- ", Text}), Value, pas::bind_method<&TfRuinsTalk::BuyPirateBaseChameleon>(this));
            } else {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
            }
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclinePirateBaseChameleon>(this));
    }

    void TfRuinsTalk::BuyPirateBaseChameleon(std::int32_t Action) {
        std::uint8_t Series = Action;
        std::int32_t Cost = PirateChameleonQuoteCosts[Series];
        ++aPlayer::GetPlayer()->ChameleonCharges[Series];
        aPlayer::GetPlayer()->SetMoney(std::max<std::int32_t>(0, aPlayer::GetPlayer()->Money - Cost));
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PBAfterOk"_wref.get());
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::DeclinePirateBaseChameleon(std::int32_t Action) {
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PBAfterNo"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.PB.Chameleon.PBAfterNoAlt"_wref.get());
        }
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::I_WarWithKlingAndPirates(std::int32_t Action) {
        pas::WideString Key{};
        std::int32_t I{};
        aGalaxy::TStar* EnemyStar = nullptr;
        std::uint8_t CoalitionPercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition);
        std::uint8_t DominatorPercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
        std::uint8_t PiratePercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            if (pas::is_one_of<aGalaxyStruct::sfDominators, aGalaxyStruct::sfPirates>(pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star))->Status.ControlFaction) && pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star))->Constellation->Id != 20) {
                EnemyStar = aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star;
                break;
            }
        }
        if (EnemyStar == nullptr) {
            if (static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) ^ 1) && aGalaxy::Galaxy->PirateWinType != 3) {
                Key = u"FormRuins.WB.WarWithKlingAndPirates.PiratesOnly.WBAnswerWeControl100Percent"_w;
            } else if (aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) && aGalaxy::Galaxy->PirateWinType == 3) {
                Key = u"FormRuins.WB.WarWithKlingAndPirates.KlingOnly.WBAnswerWeControl100Percent"_w;
            } else {
                Key = u"FormRuins.WB.WarWithKlingAndPirates.KlingAndPirates.WBAnswerWeControl100Percent"_w;
            }
        } else {
            if (DominatorPercent > PiratePercent * 3) {
                Key = u"FormRuins.WB.WarWithKlingAndPirates.KlingOnly."_w;
            } else if (DominatorPercent * 3 < PiratePercent) {
                Key = u"FormRuins.WB.WarWithKlingAndPirates.PiratesOnly."_w;
            } else {
                Key = u"FormRuins.WB.WarWithKlingAndPirates.KlingAndPirates."_w;
            }
            {
                std::uint8_t cpp_case = CoalitionPercent;
                if (cpp_case >= 0 && cpp_case <= 9) {
                    Key = pas::concat_wide({Key, u"WBAnswerWeControlMore00Percent"});
                } else if (cpp_case >= 10 && cpp_case <= 22) {
                    Key = pas::concat_wide({Key, u"WBAnswerWeControlMore10Percent"});
                } else if (cpp_case >= 23 && cpp_case <= 35) {
                    Key = pas::concat_wide({Key, u"WBAnswerWeControlMore30Percent"});
                } else if (cpp_case >= 36 && cpp_case <= 59) {
                    Key = pas::concat_wide({Key, u"WBAnswerWeControlMore50Percent"});
                } else if (cpp_case >= 60 && cpp_case <= 89) {
                    Key = pas::concat_wide({Key, u"WBAnswerWeControlMore70Percent"});
                } else if (cpp_case >= 90 && cpp_case <= 99) {
                    Key = pas::concat_wide({Key, u"WBAnswerWeControlMore90Percent"});
                } else if (cpp_case == 100) {
                    Key = pas::concat_wide({Key, u"WBAnswerWeControl100Percent"});
                } else {
                    DialogText = u"Error in procedure TfRuinsTalk.I_WarWithKlingAndPirates"_w;
                }
            }
        }
        DialogText = aConst::LocalizedColorText(Key);
        aMyFunction::ReplaceTextToken(DialogText, u"<WB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Percent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(CoalitionPercent)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<DominatorsPercent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(DominatorPercent)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<PiratesPercent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(PiratePercent)), u"<color=255,240,100>"_w);
        if (EnemyStar != nullptr) {
            aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, EnemyStar->Name, u"<color=255,240,100>"_w);
            {
                pas::WideString name = EnemyStar->Constellation->GetName();
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<Sector>"_w, std::move(name), u"<color=255,240,100>"_w);
            }
        }
        M_Main(true);
    }

    // Does not promote the player.
    void TfRuinsTalk::ShowMilitaryBaseNextRankDialog(std::int32_t Action) {
        std::uint8_t I{};
        pas::WideString Token{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.NextRank.WBAnswer"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<WB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        {
            pas::WideString nextRankName = aPlayer::GetPlayer()->GetNextRankName();
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<NextRank>"_w, std::move(nextRankName), u"<color=255,240,100>"_w);
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<NeedPoints>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aPlayer::GetPlayer()->GetRankPointsToNextRank())), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<RankPointsForLiberationSystem>"_w, pas::wide_int_to_str(30), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<RankPointsForDeadPirates>"_w, pas::wide_int_to_str(10), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<RankPointsForDeadPiratesInGiperSpace>"_w, pas::wide_int_to_str(2), u"<color=255,240,100>"_w);
        for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(7); ++I) {
            if (I != 0) {
                Token = static_cast<pas::WideString>(pas::concat_ansi({"<Name", SysUtils::IntToStr(I), ">"}));
                aMyFunction::ReplaceTextToken(DialogText, Token, aConst::DominatorShipDefinitions[I].DisplayNames[aGalaxyStruct::dsBlazer], pas::WideString());
                Token = pas::concat_wide({u"<RankPointsFor", aConst::DominatorShipTypeNames[I], u">"});
                aMyFunction::ReplaceTextToken(DialogText, Token, pas::wide_int_to_str(static_cast<std::int32_t>(aConst::DominatorShipDefinitions[I].RankPoints)), u"<color=255,240,100>"_w);
            }
        }
        M_Main(true);
    }

    void TfRuinsTalk::ShowMilitaryBaseRepairDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Repair.WBAnswer"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<WB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseRepairQuote>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.Repair.PlayerCostAsk"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
    }

    void TfRuinsTalk::ShowMilitaryBaseRepairQuote(std::int32_t Action) {
        std::int32_t Cost = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->GetRepairCost(aPlayer::GetPlayer());
        if (Cost == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Repair.WBCostAnswerNonEquipmentsForRepair"_wref.get());
            M_Main(true);
        } else {
            if (Cost < aPlayer::GetPlayer()->Wealth / 10) {
                DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Repair.WBCostAnswerYouHaveGoodEquipments"_wref.get());
            } else {
                DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Repair.WBCostAnswerYouHaveBadEquipments"_wref.get());
            }
            aMyFunction::ReplaceTextToken(DialogText, u"<WB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
            ClearChoices();
            if (Cost > 0 && aPlayer::GetPlayer()->Money >= Cost) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptMilitaryBaseRepair>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(Cost);
                    pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.WB.Repair.PlayerOk"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
                }())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineMilitaryBaseRepair>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.Repair.PlayerNo"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
            }
        }
    }

    void TfRuinsTalk::AcceptMilitaryBaseRepair(std::int32_t Action) {
        pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RepairShipEquipment(aPlayer::GetPlayer());
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Repair.WBAfterOk"_wref.get());
        GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::DeclineMilitaryBaseRepair(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Repair.WBAfterNo"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowMilitaryBaseProgramsDialog(std::int32_t Action) {
        pas::WideString Text{};
        pas::WideString Info{};
        std::uint8_t I{};
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(11)); cpp_range.next(I); ) {
            if (aPlayer::GetPlayer()->ProgramRewardStocks[I] > 0) {
                Info = aConst::LocalizedColorText(u"FormRuins.WB.Programms.Info"_wref.get());
                aMyFunction::ReplaceTextToken(Info, u"<Name>"_w, (static_cast<void>(aPlayer::GetPlayer()), aRanger::TRanger::GetProgramName(I)), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Info, u"<Text>"_w, ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->ProgramRewardStocks[I]);
                    pas::WideString localizedText = aConst::LocalizedText(pas::concat_wide({u"Programms.", aConst::ProgramNames[I], u".Text"}));
                    return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr));
                }()), pas::WideString());
                aMyFunction::ReplaceTextToken(Info, u"<Count>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ProgramRewardStocks[I]), u"<color=255,240,100>"_w);
                if (Text == u"") {
                    Text = Info;
                } else {
                    Text = pas::concat_wide({Text, u"\r\n", Info});
                }
            }
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Programms.WBAnswer"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<WB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Programms>"_w, Text, pas::WideString());
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptMilitaryBasePrograms>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.Programms.PlayerOk"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
    }

    void TfRuinsTalk::AcceptMilitaryBasePrograms(std::int32_t Action) {
        std::uint8_t I{};
        for (I = static_cast<std::uint8_t>(0); I <= static_cast<std::uint8_t>(11); ++I) {
            if (aPlayer::GetPlayer()->ProgramRewardStocks[I] > 0) {
                aPlayer::GetPlayer()->ProgramCounts[I] += aPlayer::GetPlayer()->ProgramRewardStocks[I];
                aPlayer::GetPlayer()->ProgramRewardStocks[I] = 0;
            }
        }
        aPlayer::GetPlayer()->LastDominatorProgramRewardTurn = aGalaxy::Galaxy->CurrentTurn;
        aPlayer::GetPlayer()->DestroyedDominatorHullMass = 0;
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.Programms.WBAfterOk"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowMilitaryBaseWarOperationDialog(std::int32_t Action) {
        BusinessQuoteSmallAmount = aMyFunction::RoundAndTruncateToTens(aGalaxy::Galaxy->ComputeScaledSmallMoney(aGalaxyStruct::oiHuman));
        StationServiceQuoteCost = aMyFunction::RoundAndTruncateToHundreds(aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman));
        StationServiceQuoteCost = aMyFunction::RoundAndTruncateToHundreds(static_cast<long double>(StationServiceQuoteCost) * aMyFunction::RemapClamped(aGalaxy::Galaxy->CurrentTurn - aPlayer::GetPlayer()->StationServiceLastUseTurns[aGalaxyStruct::cpWarOperation], 0.0, aConst::StationServiceRepeatPeriods[aGalaxyStruct::cpWarOperation], 7.7, 1.0));
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.WB"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<DecMoney>"_w, pas::wide_int_to_str(BusinessQuoteSmallAmount), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptMilitaryBaseWarOperation>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(StationServiceQuoteCost);
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.PlayerOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(StationServiceQuoteCost);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.PlayerOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
            }())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::DeclineMilitaryBaseWarOperation>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.PlayerNo"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
    }

    void TfRuinsTalk::AcceptMilitaryBaseWarOperation(std::int32_t Action) {
        pas::WideString Names{};
        std::int32_t I{};
        aShip::TShip* Ship{};
        aGroup::TGroup* Group{};
        aGalaxy::TStar* FromStar{};
        aGalaxy::TStar* ToStar{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.WBAfterOk"_wref.get());
        if (aGalaxy::Galaxy->TryCreateLiberationGroup()) {
            Group = pas::list_at<aGroup::TGroup>(aGalaxy::Galaxy->LiberationGroups, pas::list_count(aGalaxy::Galaxy->LiberationGroups) - 1);
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
            aPlayer::GetPlayer()->StationServiceLastUseTurns[aGalaxyStruct::cpWarOperation] = aGalaxy::Galaxy->CurrentTurn;
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 25, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}}));
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 15, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}}));
            aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 5, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}}));
            {
                const std::int32_t cpp_first = pas::list_count(Group->Ships) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        Ship = pas::list_at<aShip::TShip>(Group->Ships, I);
                        if (Names == u"") {
                            Names = Ship->GetFullName(u" "_wref.get());
                        } else {
                            Names = pas::concat_wide({Names, u"\r\n", Ship->GetFullName(u" "_wref.get())});
                        }
                    }
                }
            }
            FromStar = pas::checked_cast<aGalaxy::TStar*>(Group->Route[0].Target);
            ToStar = pas::checked_cast<aGalaxy::TStar*>(Group->Route[3].Target);
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.WBAfterOkGood"_wref.get())});
            aMyFunction::ReplaceTextToken(DialogText, u"<Names>"_w, Names, pas::WideString());
            aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<StarNormal>"_w, FromStar->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<StarEnemy>"_w, ToStar->Name, u"<color=255,240,100>"_w);
            {
                pas::WideString name = FromStar->Constellation->GetName();
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<SectorNormal>"_w, std::move(name), u"<color=255,240,100>"_w);
            }
            {
                pas::WideString name_2 = ToStar->Constellation->GetName();
                pas::WideString& dialogText_2 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_2, u"<SectorEnemy>"_w, std::move(name_2), u"<color=255,240,100>"_w);
            }
            {
                pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(Group->Route[2].WaitUntilTurn);
                pas::WideString& dialogText_3 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_3, u"<Date>"_w, std::move(formatTurnDate), u"<color=255,240,100>"_w);
            }
        } else {
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - BusinessQuoteSmallAmount);
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.WBAfterOkBad"_wref.get())});
            aMyFunction::ReplaceTextToken(DialogText, u"<DecMoney>"_w, pas::wide_int_to_str(BusinessQuoteSmallAmount), u"<color=255,240,100>"_w);
        }
        M_Main(true);
    }

    void TfRuinsTalk::DeclineMilitaryBaseWarOperation(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.WarOperation.WBAfterNo"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowMilitaryBaseTravelDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.WBToChamber"_wref.get());
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerToChamber"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ConfirmMilitaryBaseTravel>(this));
    }

    void TfRuinsTalk::ConfirmMilitaryBaseTravel(std::int32_t Action) {
        if (LargePortraitLayout && PortraitTableVisible && StationTransientControl->FindByNameRecursive(u"Table2"sv) != nullptr) {
            StationTransientControl->FindByNameRecursive(u"Table2"sv)->SetActive(true);
            StationTransientControl->FindByNameRecursive(u"Table"sv)->SetActive(false);
        }
        {
            GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"sv));
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), u"WBbg2"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.WBInChamber"_wref.get());
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerFly"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DepartWithStation>(this));
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerNotFly"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineMilitaryBaseTravel>(this));
    }

    void TfRuinsTalk::DepartWithStation(std::int32_t Action) {
        if (Action == 1) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(200);
        }
        GlobalsV::RequestedScreenId = GlobalsV::screenJump;
        ClearChoices();
        fEquipmentShop::RestoreTemporaryShopStock();
        if (Globals::SkipVideo) {
            fPanelLoad::ActiveLoadPanel->SelectBackgroundStyle(2);
            fPanelLoad::ActiveLoadPanel->RefreshBackgroundImages();
            fPanelLoad::ActiveLoadPanel->StartClosingShutters();
        } else {
            pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId])->RequestClose(1);
        }
        if (Action == 0) {
            GI_Main::BreakUiMessage();
        }
    }

    void TfRuinsTalk::ShowMilitaryBaseArrivalDialog(std::int32_t Action) {
        if (LargePortraitLayout && PortraitTableVisible && StationTransientControl->FindByNameRecursive(u"Table2"sv) != nullptr) {
            StationTransientControl->FindByNameRecursive(u"Table2"sv)->SetActive(true);
            StationTransientControl->FindByNameRecursive(u"Table"sv)->SetActive(false);
        }
        {
            GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"sv));
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), u"WBbg2"});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.WBInStarEnemy"_wref.get());
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerUp"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseArrivalInfo>(this));
    }

    void TfRuinsTalk::DeclineMilitaryBaseTravel(std::int32_t Action) {
        if (LargePortraitLayout && PortraitTableVisible && StationTransientControl->FindByNameRecursive(u"Table2"sv) != nullptr) {
            StationTransientControl->FindByNameRecursive(u"Table"sv)->SetActive(true);
            StationTransientControl->FindByNameRecursive(u"Table2"sv)->SetActive(false);
        }
        {
            GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"sv));
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,", fRuinsTalk::GetStationBackgroundPath()});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.WBAfterNotFly"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowMilitaryBaseArrivalInfo(std::int32_t Action) {
        std::int32_t I{};
        aShip::TShip* Ship{};
        pas::WideString Names{};
        if (LargePortraitLayout && PortraitTableVisible && StationTransientControl->FindByNameRecursive(u"Table2"sv) != nullptr) {
            StationTransientControl->FindByNameRecursive(u"Table2"sv)->SetActive(true);
            StationTransientControl->FindByNameRecursive(u"Table"sv)->SetActive(false);
        }
        {
            GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageBG"sv));
            pas::WideString cpp_arg_2 = pas::concat_wide({u"GI,", fRuinsTalk::GetStationBackgroundPath()});
            cpp_arg->SetImagePath(std::move(cpp_arg_2));
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I);
            if (Ship->TypeId == aGalaxyStruct::stKling && static_cast<std::uint8_t>(Ship->InHyperspace ^ 1)) {
                if (Names == u"") {
                    Names = Ship->GetName();
                } else {
                    Names = pas::concat_wide({Names, u", ", Ship->GetName()});
                }
            }
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.WBStarEnemyInfo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<N>"_w, pas::wide_int_to_str(MilitaryTravelDistance), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Ships>"_w, Names, u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::OpenHangar>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerHangar"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::ShowMilitaryBaseArrivalQuestions>(this);
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerQuestions"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
        }
    }

    void TfRuinsTalk::ShowMilitaryBaseArrivalQuestions(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.WBAfterQuestions"_wref.get());
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.WB.FlyToEnemy.PlayerHangar"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::OpenHangar>(this));
    }

    void TfRuinsTalk::ShowScienceBaseImprovementDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBAnswer"_wref.get());
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowScienceBaseImprovementItems>(this));
    }

    void TfRuinsTalk::ShowScienceBaseImprovementItems(std::int32_t Action) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TItem* Artefact{};
        pas::WideString Text{};
        fRuinsTalk::ResetStationImprovement();
        ClearChoices();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                if (Item->CanImprove() && aShip::TShip_CanUseEquipmentTech(aPlayer::GetPlayer(), Item) && aShip::TShip_CanRepairEquipmentTech(aPlayer::GetPlayer(), Item)) {
                    ++Count;
                    Text = pas::concat_wide({Text, u"\r\n", pas::wide_int_to_str(Count), u") ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.ItemReadyForImprovement"_wref.get())});
                    {
                        GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseImprovementQuote>(this);
                        pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Item->GetDisplayName()))});
                        std::int32_t item = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item));
                        TfRuinsTalk* self = this;
                        self->AddChoice(std::move(cpp_arg_2), item, cpp_arg);
                    }
                }
                aMyFunction::ReplaceTextToken(Text, u"<ItemName>"_w, aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Item->GetDisplayName())), pas::WideString());
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Item->Cost), u"<color=255,240,100>"_w);
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Artefacts) - 1); cpp_range_2.next(I); ) {
            Artefact = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Artefacts, I);
            if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Artefact)) {
                Item = static_cast<aShip::TShip*>(artefactTranclucator->Ship)->GetHull();
                if (Item->CanImprove()) {
                    ++Count;
                    Text = pas::concat_wide({Text, u"\r\n", pas::wide_int_to_str(Count), u") ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.ItemReadyForImprovement"_wref.get())});
                    {
                        GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::ShowScienceBaseImprovementQuote>(this);
                        pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Artefact->GetDisplayName()))});
                        std::int32_t item_2 = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item));
                        TfRuinsTalk* self_2 = this;
                        self_2->AddChoice(std::move(cpp_arg_4), item_2, cpp_arg_3);
                    }
                }
                aMyFunction::ReplaceTextToken(Text, u"<ItemName>"_w, aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Artefact->GetDisplayName())), pas::WideString());
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Item->Cost), u"<color=255,240,100>"_w);
            }
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBSeeItems"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<ListItems>"_w, Text, pas::WideString());
        if (Count > 0) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBSeeItemsHaveItems"_wref.get())});
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseImprovement>(this);
                pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerNothing"_wref.get())});
                TfRuinsTalk* self_3 = this;
                self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
            }
        } else {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBSeeItemsNotHaveItems"_wref.get())});
            M_Main(true);
        }
    }

    void TfRuinsTalk::DeclineScienceBaseImprovement(std::int32_t Action) {
        fRuinsTalk::ResetStationImprovement();
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBAnswerNothing"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowScienceBaseImprovementQuote(std::int32_t Action) {
        aItem::TEquipment* Item{};
        std::int32_t Nodes{};
        Item = reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        StationImprovementItem = Item;
        if (Item->OwnerId == aGalaxyStruct::oiDominator) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBNeedCostImprovementNodes"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<MinNode>"_w, pas::wide_int64_to_str(System::Round(Item->CalculateImprovementCost(aItem::ikMinor) * 0.01L)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<AverageNode>"_w, pas::wide_int64_to_str(System::Round(Item->CalculateImprovementCost(aItem::ikMedium) * 0.01L)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MaxNode>"_w, pas::wide_int64_to_str(System::Round(Item->CalculateImprovementCost(aItem::ikMajor) * 0.01L)), u"<color=255,240,100>"_w);
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBNeedCostImprovement"_wref.get());
        }
        {
            pas::WideString normalizeTextHighlightColors = aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Item->GetDisplayName()));
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<FullName>"_w, std::move(normalizeTextHighlightColors), u"<color=255,240,100>"_w);
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Item->Cost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Min>"_w, pas::wide_int_to_str(Item->CalculateImprovementCost(aItem::ikMinor)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Average>"_w, pas::wide_int_to_str(Item->CalculateImprovementCost(aItem::ikMedium)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Max>"_w, pas::wide_int_to_str(Item->CalculateImprovementCost(aItem::ikMajor)), u"<color=255,240,100>"_w);
        ClearChoices();
        if (Item->OwnerId == aGalaxyStruct::oiDominator) {
            Nodes = aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr);
            if (Nodes >= System::Round(Item->CalculateImprovementCost(aItem::ikMajor) * 0.01L) && Item->CalculateImprovementCost(aItem::ikMajor) <= aPlayer::GetPlayer()->Money) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::SelectScienceBaseImprovementKind>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerOkMax"_wref.get())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 2, cpp_arg);
            }
            if (Nodes >= System::Round(Item->CalculateImprovementCost(aItem::ikMedium) * 0.01L) && Item->CalculateImprovementCost(aItem::ikMedium) <= aPlayer::GetPlayer()->Money) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::SelectScienceBaseImprovementKind>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerOkAverage"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 1, cpp_arg_3);
            }
            if (Nodes >= System::Round(Item->CalculateImprovementCost(aItem::ikMinor) * 0.01L) && Item->CalculateImprovementCost(aItem::ikMinor) <= aPlayer::GetPlayer()->Money) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::SelectScienceBaseImprovementKind>(this);
                pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerOkMin"_wref.get())});
                TfRuinsTalk* self_3 = this;
                self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
            }
        } else {
            if (Item->CalculateImprovementCost(aItem::ikMajor) <= aPlayer::GetPlayer()->Money) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::SelectScienceBaseImprovementKind>(this);
                pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerOkMax"_wref.get())});
                TfRuinsTalk* self_4 = this;
                self_4->AddChoice(std::move(cpp_arg_8), 2, cpp_arg_7);
            }
            if (Item->CalculateImprovementCost(aItem::ikMedium) <= aPlayer::GetPlayer()->Money) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_9 = pas::bind_method<&TfRuinsTalk::SelectScienceBaseImprovementKind>(this);
                pas::WideString cpp_arg_10 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerOkAverage"_wref.get())});
                TfRuinsTalk* self_5 = this;
                self_5->AddChoice(std::move(cpp_arg_10), 1, cpp_arg_9);
            }
            if (Item->CalculateImprovementCost(aItem::ikMinor) <= aPlayer::GetPlayer()->Money) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_11 = pas::bind_method<&TfRuinsTalk::SelectScienceBaseImprovementKind>(this);
                pas::WideString cpp_arg_12 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerOkMin"_wref.get())});
                TfRuinsTalk* self_6 = this;
                self_6->AddChoice(std::move(cpp_arg_12), 0, cpp_arg_11);
            }
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_13 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseImprovement>(this);
            pas::WideString cpp_arg_14 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerNo"_wref.get())});
            TfRuinsTalk* self_7 = this;
            self_7->AddChoice(std::move(cpp_arg_14), 0, cpp_arg_13);
        }
    }

    void TfRuinsTalk::SelectScienceBaseImprovementKind(std::int32_t Action) {
        aItem::TEquipment* Item{};
        pas::WideString Text{};
        std::int32_t Detail{};
        StationImprovementKind = static_cast<aItem::TImprovementKind>(Action);
        Item = StationImprovementItem;
        Detail = 1;
        std::int32_t Count = 0;
        while (true) {
            if (!(pas::class_cast_if<aItem::TEngine*>(Item) != nullptr) && !(pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) && !(pas::class_cast_if<aItem::TCargoHook*>(Item) != nullptr)) {
                break;
            }
            if (!(Detail == 2 && pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && pas::in_range(static_cast<aItem::TWeapon*>(Item)->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)))) {
                Text = aConst::LocalizedColorText(pas::concat_wide({u"Items.", Item->GetCategoryConfigName(), u".Detail.", pas::wide_int_to_str(Detail)}));
                if (Text.length() != 0) {
                    if (Detail == 1) {
                        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBDetailImprovement"_wref.get());
                        ClearChoices();
                    }
                    {
                        pas::WideString cpp_arg = pas::concat_wide({u"- ", aMyFunction::FormatText1(aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerDetailOk"_wref.get()), u"<color=255,240,100>"_w, u"<Attr>"_w, Text)});
                        std::int32_t detail = Detail;
                        AddChoice(std::move(cpp_arg), detail, pas::bind_method<&TfRuinsTalk::AcceptScienceBaseImprovement>(this));
                    }
                    ++Count;
                }
            }
            ++Detail;
            if (Detail > 100) {
                break;
            }
        }
        if (Count > 1) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerDetailNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineScienceBaseImprovement>(this));
        } else {
            AcceptScienceBaseImprovement(0);
        }
    }

    void TfRuinsTalk::AcceptScienceBaseImprovement(std::int32_t Action) {
        aItem::TEquipment* Item{};
        std::int32_t Nodes{};
        StationImprovementDetail = Action;
        Item = StationImprovementItem;
        aItem::TImprovementKind Kind = StationImprovementKind;
        std::int32_t Cost = Item->CalculateImprovementCost(Kind);
        if (Item->OwnerId == aGalaxyStruct::oiDominator) {
            Nodes = System::Round(Cost * 0.01L);
        } else {
            Nodes = 0;
        }
        if (aPlayer::GetPlayer()->Money >= Cost && aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) >= Nodes) {
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
            if (Item->OwnerId == aGalaxyStruct::oiDominator) {
                aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Nodes, nullptr);
            }
            Item->DetailImprovement = StationImprovementDetail;
            Item->Improve(Kind);
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            switch (Kind) {
                case aItem::ikMinor: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBAfterOkMin"_wref.get());
                    break;
                }
                case aItem::ikMedium: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBAfterOkAverage"_wref.get());
                    break;
                }
                case aItem::ikMajor: {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBAfterOkMax"_wref.get());
                    break;
                }
                default: GR_Main::RaiseWideMessage(u"SB: Improvement = any"_wref.get()); break;
            }
            {
                pas::WideString wideLowerCase = SysUtilsImports::WideLowerCase(Item->GetShortName());
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<ShortName>"_w, std::move(wideLowerCase), pas::WideString());
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            ClearChoices();
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseImprovementItems>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerRepeatOk"_wref.get())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseRepeatImprovement>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Improvement.PlayerRepeatNo"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
            }
            fRuinsTalk::ResetStationImprovement();
        } else {
            DeclineScienceBaseRepeatImprovement(0);
        }
    }

    void TfRuinsTalk::DeclineScienceBaseRepeatImprovement(std::int32_t Action) {
        fRuinsTalk::ResetStationImprovement();
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Improvement.SBAfterRepeatNo"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowScienceBaseRepairDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Repair.SBAnswer"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseRepairQuote>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Repair.PlayerCostAsk"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
    }

    void TfRuinsTalk::ShowScienceBaseRepairQuote(std::int32_t Action) {
        std::int32_t EquipmentCost{};
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t NodeCost = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->EquippedFlag != 0) {
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); weapon != nullptr && weapon->GetWeaponInfo()->Availability == aGalaxyStruct::waNotSoldAndNodeRepair && Item->NeedsRepair()) {
                    if (aShip::TShip_CanRepairEquipmentTech(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo), Item)) {
                        NodeCost += aItem::TEquipment_CalculateRepairCost(Item);
                    }
                }
            }
        }
        if (NodeCost > 0) {
            NodeCost = System::Round(NodeCost * 0.0025L);
            if (NodeCost == 0) {
                NodeCost = 1;
            }
        }
        std::int32_t Cost = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->CalculateRepairCost(aPlayer::GetPlayer(), EquipmentCost);
        if (Cost == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Repair.SBCostAnswerNonEquipmentsForRepair"_wref.get());
            M_Main(true);
        } else {
            if (Cost < aPlayer::GetPlayer()->Wealth / 10) {
                DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Repair.SBCostAnswerYouHaveGoodEquipments"_wref.get());
            } else {
                DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Repair.SBCostAnswerYouHaveBadEquipments"_wref.get());
            }
            aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<EqMoney>"_w, pas::wide_int_to_str(EquipmentCost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<ArtMoney>"_w, pas::wide_int_to_str(Cost - EquipmentCost), u"<color=255,240,100>"_w);
            if (NodeCost != 0) {
                DialogText = pas::concat_wide_reverse({aConst::LocalizedColorText(u"FormRuins.SB.Repair.SBCostAnswerNeedNode"_wref.get()), DialogText});
                aMyFunction::ReplaceTextToken(DialogText, u"<NeedNode>"_w, pas::wide_int_to_str(NodeCost), u"<color=255,240,100>"_w);
            }
            ClearChoices();
            if (Cost > 0 && aPlayer::GetPlayer()->Money >= Cost) {
                if (NodeCost == 0 || NodeCost > 0 && aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) >= NodeCost) {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptScienceBaseRepair>(this);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Repair.PlayerOk"_wref.get())});
                    TfRuinsTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
                }
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseRepair>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Repair.PlayerNo"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
            }
        }
    }

    void TfRuinsTalk::AcceptScienceBaseRepair(std::int32_t Action) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t NodeCost = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (Item->EquippedFlag != 0) {
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); weapon != nullptr && weapon->GetWeaponInfo()->Availability == aGalaxyStruct::waNotSoldAndNodeRepair && Item->NeedsRepair()) {
                    if (aShip::TShip_CanRepairEquipmentTech(pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo), Item)) {
                        NodeCost += aItem::TEquipment_CalculateRepairCost(Item);
                    }
                }
            }
        }
        if (NodeCost > 0) {
            NodeCost = System::Round(NodeCost * 0.0025L);
            if (NodeCost == 0) {
                NodeCost = 1;
            }
        }
        if (aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) >= NodeCost) {
            aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), NodeCost, nullptr);
            pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RepairShipEquipment(aPlayer::GetPlayer());
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Repair.SBAfterOk"_wref.get());
            GR_Main::SoundManager->PlaySound(u"Sound.Repair"_wref.get());
            M_Main(true);
        } else {
            DeclineScienceBaseRepair(0);
        }
    }

    void TfRuinsTalk::DeclineScienceBaseRepair(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Repair.SBAfterNo"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowScienceBaseSatelliteOfferDialog(std::int32_t Refresh) {
        pas::WideString Text{};
        aItem::TSatellite* Satellite = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SatelliteOffer;
        if (Satellite == nullptr) {
            GR_Main::RaiseWideMessage(u"I_SBSatelliteAsk sat=nil"_wref.get());
        }
        if (Refresh == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Satellite.SBInfo"_wref.get());
            {
                std::int32_t cpp_left = aGalaxy::Galaxy->CountExistingSatellites();
                if (cpp_left < aPlayer::GetPlayer()->GetSatelliteLimit()) {
                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.Satellite.SBInfoOk"_wref.get())});
                } else {
                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.Satellite.SBInfoNo"_wref.get())});
                }
            }
            {
                pas::WideString displayName = Satellite->GetDisplayName();
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<SatName>"_w, std::move(displayName), u"<color=255,240,100>"_w);
            }
            {
                pas::WideString infoText = Satellite->virtual_TItem_GetInfoText(pas::WideString(), nullptr);
                pas::WideString& dialogText_2 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_2, u"<SatText>"_w, std::move(infoText), pas::WideString());
            }
            aMyFunction::ReplaceTextToken(DialogText, u"<SatSize>"_w, pas::wide_int_to_str(Satellite->Weight), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<SatMoney>"_w, pas::wide_int_to_str(Satellite->Cost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<SatCurCount>"_w, pas::wide_int_to_str(aGalaxy::Galaxy->CountExistingSatellites()), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<SatMayCount>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->GetSatelliteLimit()), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        }
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowSatelliteInstructions>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Satellite.PlayerInstruction"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
        Text = aConst::LocalizedColorText(u"FormRuins.SB.Satellite.PlayerOk"_wref.get());
        aMyFunction::ReplaceTextToken(Text, u"<SatName>"_w, Satellite->GetDisplayName(), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<SatMoney>"_w, pas::wide_int_to_str(Satellite->Cost), u"<color=255,240,100>"_w);
        if (([&] {
            std::int32_t cpp_left_2 = aGalaxy::Galaxy->CountExistingSatellites();
            return cpp_left_2 < aPlayer::GetPlayer()->GetSatelliteLimit();
        }()) && aPlayer::GetPlayer()->Money >= Satellite->Cost) {
            AddChoice(pas::concat_wide({u"- ", Text}), 0, pas::bind_method<&TfRuinsTalk::BuyScienceBaseSatellite>(this));
        } else {
            AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseSatellite>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Satellite.PlayerNo"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
    }

    void TfRuinsTalk::ShowSatelliteInstructions(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Satellite.SBInstruction"_wref.get());
        ShowScienceBaseSatelliteOfferDialog(1);
    }

    // Transfers the existing SatelliteOffer into inventory.
    void TfRuinsTalk::BuyScienceBaseSatellite(std::int32_t Action) {
        aItem::TSatellite* Satellite = pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SatelliteOffer;
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Satellite.SBAfterOk"_wref.get());
        {
            pas::WideString displayName = Satellite->GetDisplayName();
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<SatName>"_w, std::move(displayName), u"<color=255,240,100>"_w);
        }
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Satellite->Cost);
        pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Satellite));
        pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SatelliteOffer = nullptr;
        pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RegenerateSatelliteOffer();
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::DeclineScienceBaseSatellite(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Satellite.SBAfterNo"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowScienceBaseResearchDialog(std::int32_t Action) {
        pas::WideString Text{};
        pas::WideString Info{};
        aGalaxyStruct::TDominatorSeries Series{};
        Text = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSectionInfo"_wref.get());
        ClearChoices();
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(Series); ) {
            if (static_cast<std::uint8_t>(aGalaxy::Galaxy->IsDominatorResearchComplete(pas::make_set<aGalaxy::TDominatorSeriesSet>({{static_cast<std::int32_t>(Series)}})) ^ 1) && aGalaxy::Galaxy->IsDominatorSeriesUnresolved(Series)) {
                Info = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSectionInfoAdd"_wref.get());
                if (aPlayer::GetPlayer()->CountUnequippedDominatorEquipment() > 0) {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::SelectScienceBaseResearchSection>(this);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.Section", aConst::DominatorSeriesNames[Series]}))});
                    std::int32_t cpp_arg_3 = Series + 1;
                    TfRuinsTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_2), cpp_arg_3, cpp_arg);
                } else {
                    GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                    pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.Section", aConst::DominatorSeriesNames[Series]}))});
                    TfRuinsTalk* self_2 = this;
                    self_2->AddChoice(std::move(cpp_arg_4), 0, scriptDialogBlockCallback);
                }
                aMyFunction::ReplaceTextToken(Info, u"<Count>"_w, pas::wide_int_to_str(aGalaxy::Galaxy->DominatorResearch[Series].Material), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Info, u"<Speed>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(aGalaxy::Galaxy->GetDominatorResearchEfficiency(Series))), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Info, u"<Day>"_w, pas::wide_int64_to_str(System::Round(pas::real_max<pas::Extended>(1.0L, pas::real_divide(1.0E+2L - aGalaxy::Galaxy->DominatorResearch[Series].Progress, aGalaxy::Galaxy->GetDominatorResearchRate(Series))))), u"<color=255,240,100>"_w);
            } else {
                Info = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSectionInfoEnd"_wref.get());
            }
            switch (Series) {
                case aGalaxyStruct::dsBlazer: {
                    aMyFunction::ReplaceTextToken(Text, u"<SBAnswerInfoBlazer>"_w, Info, pas::WideString());
                    break;
                }
                case aGalaxyStruct::dsKeller: {
                    aMyFunction::ReplaceTextToken(Text, u"<SBAnswerInfoKeller>"_w, Info, pas::WideString());
                    break;
                }
                case aGalaxyStruct::dsTerron: {
                    aMyFunction::ReplaceTextToken(Text, u"<SBAnswerInfoTerron>"_w, Info, pas::WideString());
                    break;
                }
            }
        }
        if (Action == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBAnswer"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBAnswer2"_wref.get());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<SBSectionInfo>"_w, Text, pas::WideString());
        aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::SelectScienceBaseResearchSection>(this);
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.SectionNone"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
        }
    }

    void TfRuinsTalk::SelectScienceBaseResearchSection(std::int32_t Action) {
        pas::WideString Text{};
        std::uint8_t Series{};
        ClearChoices();
        if (Action == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBAfterSectionNone"_wref.get());
            M_Main(true);
        } else {
            Text = pas::WideString();
            Series = Action - 1;
            SelectedResearchSeries = Series;
            BuildResearchItemChoices(Series, Text);
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSection1"_wref.get());
            {
                pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.Section", aConst::DominatorSeriesNames[Series]}));
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<SectionName>"_w, std::move(localizedColorText), pas::WideString());
            }
            aMyFunction::ReplaceTextToken(DialogText, u"<Items>"_w, Text, pas::WideString());
            {
                pas::WideString lookupLocalizedTextOrEmpty = GR_Main::LookupLocalizedTextOrEmpty(u"FormRuins.SB.Scn.ItemsCool"_wref.get());
                pas::WideString& dialogText_2 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_2, u"<ItemsCool>"_w, std::move(lookupLocalizedTextOrEmpty), u"<color=255,240,100>"_w);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseResearchDialog>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSectionChoose"_wref.get())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 1, cpp_arg);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearch>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleNo"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 1, cpp_arg_3);
            }
        }
    }

    void TfRuinsTalk::SellResearchRemains(std::int32_t Action) {
        pas::WideString Text{};
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Money = 0;
        {
            const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->Inventory) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                    if (Item->OwnerId == aGalaxyStruct::oiDominator && Item->DominatorSeries == static_cast<aGalaxyStruct::TDominatorSeries>(SelectedResearchSeries) && Item->NoDropFlag == 0 && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
                        if (static_cast<std::uint8_t>(TfRuinsTalk::IsResearchItemQuestLetter(pas::checked_cast<aItem::TUselessItem*>(Item)) ^ 1) && Item->CustomFaction == u"") {
                            aGalaxy::Galaxy->DominatorResearch[SelectedResearchSeries].Material += Item->Weight;
                            Money += 2 * Item->Cost;
                            pas::list_delete(aPlayer::GetPlayer()->Inventory, I);
                            pas::free(Item);
                        }
                    }
                }
            }
        }
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Money);
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        ClearChoices();
        BuildResearchItemChoices(SelectedResearchSeries, Text);
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSection2"_wref.get());
        if (Text != u"") {
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSection2Add"_wref.get())});
        }
        {
            pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.Section", aConst::DominatorSeriesNames[SelectedResearchSeries]}));
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<SectionName>"_w, std::move(localizedColorText), pas::WideString());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<Items>"_w, Text, pas::WideString());
        aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(aGalaxy::Galaxy->DominatorResearch[SelectedResearchSeries].Material), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Money), u"<color=255,240,100>"_w);
        {
            pas::WideString lookupLocalizedTextOrEmpty = GR_Main::LookupLocalizedTextOrEmpty(u"FormRuins.SB.Scn.ItemsCool"_wref.get());
            pas::WideString& dialogText_2 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_2, u"<ItemsCool>"_w, std::move(lookupLocalizedTextOrEmpty), u"<color=255,240,100>"_w);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseResearchDialog>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSectionChoose"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 1, cpp_arg);
        }
        if (Text != u"") {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearch>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleNo"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 2, cpp_arg_3);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearch>(this);
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleNo"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_6), 3, cpp_arg_5);
        }
    }

    void TfRuinsTalk::SellResearchEquipment(std::int32_t Action) {
        pas::WideString Text{};
        std::int32_t I{};
        aItem::TEquipment* Item{};
        std::int32_t Money = 0;
        {
            const std::int32_t cpp_first = pas::list_count(aPlayer::GetPlayer()->Inventory) - 1;
            if (cpp_first >= 0) {
                for (I = cpp_first; I >= 0; --I) {
                    Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                    if (Item->OwnerId == aGalaxyStruct::oiDominator && !(pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) && Item->EquippedFlag == 0 && Item->NoDropFlag == 0 && Item->ItemType != aConst::t_Protoplasm && Item->ItemType != aConst::t_MicroModule && Item->CustomFaction == u"") {
                        aGalaxy::Galaxy->DominatorResearch[SelectedResearchSeries].Material += Item->Weight;
                        Money += Item->Cost;
                        pas::list_delete(aPlayer::GetPlayer()->Inventory, I);
                        pas::free(Item);
                    }
                }
            }
        }
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Money);
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        ClearChoices();
        BuildResearchItemChoices(SelectedResearchSeries, Text);
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSection2"_wref.get());
        if (Text != u"") {
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSection2Add"_wref.get())});
        }
        {
            pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.Section", aConst::DominatorSeriesNames[SelectedResearchSeries]}));
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<SectionName>"_w, std::move(localizedColorText), pas::WideString());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<Items>"_w, Text, pas::WideString());
        aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(aGalaxy::Galaxy->DominatorResearch[SelectedResearchSeries].Material), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Money), u"<color=255,240,100>"_w);
        {
            pas::WideString lookupLocalizedTextOrEmpty = GR_Main::LookupLocalizedTextOrEmpty(u"FormRuins.SB.Scn.ItemsCool"_wref.get());
            pas::WideString& dialogText_2 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_2, u"<ItemsCool>"_w, std::move(lookupLocalizedTextOrEmpty), u"<color=255,240,100>"_w);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseResearchDialog>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSectionChoose"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 1, cpp_arg);
        }
        if (Text != u"") {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearch>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleNo"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 2, cpp_arg_3);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearch>(this);
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleNo"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_6), 3, cpp_arg_5);
        }
    }

    void TfRuinsTalk::SellResearchItem(std::int32_t Action) {
        pas::WideString Text{};
        std::int32_t Money{};
        aItem::TEquipment* Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, Action);
        aGalaxy::Galaxy->DominatorResearch[SelectedResearchSeries].Material += Item->Weight;
        if (Item->DominatorSeries == static_cast<aGalaxyStruct::TDominatorSeries>(SelectedResearchSeries) && pas::class_cast_if<aItem::TUselessItem*>(Item) != nullptr) {
            Money = Item->Cost * 2;
        } else {
            Money = Item->Cost;
        }
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Money);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        pas::list_delete(aPlayer::GetPlayer()->Inventory, pas::list_indexof(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item)));
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        pas::free(Item);
        ClearChoices();
        BuildResearchItemChoices(SelectedResearchSeries, Text);
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSection2"_wref.get());
        if (Text != u"") {
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSection2Add"_wref.get())});
        }
        {
            pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.Section", aConst::DominatorSeriesNames[SelectedResearchSeries]}));
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<SectionName>"_w, std::move(localizedColorText), pas::WideString());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<Items>"_w, Text, pas::WideString());
        aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(aGalaxy::Galaxy->DominatorResearch[SelectedResearchSeries].Material), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Money), u"<color=255,240,100>"_w);
        {
            pas::WideString lookupLocalizedTextOrEmpty = GR_Main::LookupLocalizedTextOrEmpty(u"FormRuins.SB.Scn.ItemsCool"_wref.get());
            pas::WideString& dialogText_2 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_2, u"<ItemsCool>"_w, std::move(lookupLocalizedTextOrEmpty), u"<color=255,240,100>"_w);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseResearchDialog>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSectionChoose"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 1, cpp_arg);
        }
        if (Text != u"") {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearch>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleNo"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 2, cpp_arg_3);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearch>(this);
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerSaleNo"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_6), 3, cpp_arg_5);
        }
    }

    void TfRuinsTalk::DeclineScienceBaseResearch(std::int32_t Action) {
        ClearChoices();
        if (Action == 1) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBAfterPlayerSaleNo1"_wref.get());
        } else if (Action == 2) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBAfterPlayerSaleNo2"_wref.get());
        } else if (Action == 3) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBSectionEnd"_wref.get());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::BuyScienceBaseResearchProgram(std::int32_t Action) {
        SelectedResearchSeries = Action - 1;
        std::int32_t Cost = aMyFunction::RoundAndTruncateToHundreds(static_cast<long double>(std::min<std::int32_t>(aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman) * 2, aPlayer::GetPlayer()->Wealth / 30)) * aConst::ResearchProgramCostFactors[SelectedResearchSeries]);
        DialogText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.SBBuyTech", aConst::DominatorSeriesNames[SelectedResearchSeries]}));
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->Money >= Cost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptScienceBaseResearchProgram>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(Cost);
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerBuyTechOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), Cost, cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerBuyTechOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
            }())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::DeclineScienceBaseResearchProgram>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.SB.Scn.PlayerBuyTechNo"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
    }

    void TfRuinsTalk::AcceptScienceBaseResearchProgram(std::int32_t Action) {
        std::int32_t Cost = Action;
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        switch (SelectedResearchSeries) {
            case aGalaxyStruct::dsBlazer: {
                aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgLogicalNegation] = 1;
                break;
            }
            case aGalaxyStruct::dsKeller: aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgDematerial] = 1; break;
            case aGalaxyStruct::dsTerron: aPlayer::GetPlayer()->ProgramCounts[aGalaxyStruct::prgEnergotron] = 1; break;
        }
        DialogText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.SB.Scn.SBAfterPlayerBuyTech", aConst::DominatorSeriesNames[SelectedResearchSeries]}));
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::DeclineScienceBaseResearchProgram(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.SB.Scn.SBAfterPlayerBuyTechNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowScienceBaseHistoryDialog(std::int32_t Action) {
        ClearChoices();
        switch (Action) {
            case 0: {
                DialogText = aConst::LocalizedColorText(u"FormRuinsSB.History.SBOk"_wref.get());
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowScienceBaseHistoryDialog>(this);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsSB.Continue"_wref.get())});
                    TfRuinsTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_2), 1, cpp_arg);
                }
                break;
            }
            case 1: {
                DialogText = aConst::LocalizedColorText(u"FormRuinsSB.History.SBOk1"_wref.get());
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::ShowScienceBaseHistoryDialog>(this);
                    pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsSB.Continue"_wref.get())});
                    TfRuinsTalk* self_2 = this;
                    self_2->AddChoice(std::move(cpp_arg_4), 2, cpp_arg_3);
                }
                break;
            }
            case 2: {
                DialogText = aConst::LocalizedColorText(u"FormRuinsSB.History.SBOk2"_wref.get());
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::ShowScienceBaseHistoryDialog>(this);
                    pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsSB.Continue"_wref.get())});
                    TfRuinsTalk* self_3 = this;
                    self_3->AddChoice(std::move(cpp_arg_6), 3, cpp_arg_5);
                }
                break;
            }
            case 3: {
                DialogText = aConst::LocalizedColorText(u"FormRuinsSB.History.SBOk3"_wref.get());
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::ShowScienceBaseHistoryDialog>(this);
                    pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuinsSB.Continue"_wref.get())});
                    TfRuinsTalk* self_4 = this;
                    self_4->AddChoice(std::move(cpp_arg_8), 4, cpp_arg_7);
                }
                break;
            }
            case 4: {
                DialogText = aConst::LocalizedColorText(u"FormRuinsSB.History.SBOk4"_wref.get());
                M_Main(true);
                break;
            }
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<SB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
    }

    void TfRuinsTalk::ShowBusinessCenterDebtDialog(std::int32_t Action) {
        std::int32_t Days{};
        std::int32_t I{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        double Factor{};
        if (aPlayer::GetPlayer()->CurrentStar->Status.Battle != 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.BK.DebtNoWar"_wref.get());
            M_Main(true);
        } else {
            {
                const std::int32_t cpp_first = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
                if (cpp_first >= 0) {
                    for (I = cpp_first; I >= 0; --I) {
                        Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I);
                        if (Event->Turn + 1825 < aGalaxy::Galaxy->CurrentTurn) {
                            break;
                        }
                        if ((Event->EventType == u"PlayerKillsShip" || Event->EventType == u"PlayerCompanionKillsShip" || Event->EventType == u"PlayerTranclucatorKillsShip") && Event->GetData(0) == 10) {
                            DialogText = aConst::LocalizedColorText(u"FormRuins.BK.DebtNoPenalty"_wref.get());
                            {
                                pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(Event->Turn + 1825);
                                pas::WideString& dialogText = DialogText;
                                aMyFunction::ReplaceTextToken(dialogText, u"<Date>"_w, std::move(formatTurnDate), u"<color=255,240,100>"_w);
                            }
                            M_Main(true);
                            return;
                        }
                    }
                }
            }
            Factor = 1.0;
            {
                const std::int32_t cpp_first_2 = pas::list_count(aGalaxy::Galaxy->GalaxyEvents) - 1;
                if (cpp_first_2 >= 0) {
                    for (I = cpp_first_2; I >= 0; --I) {
                        Event = pas::list_at<aGalaxyEvent::TGalaxyEvent>(aGalaxy::Galaxy->GalaxyEvents, I);
                        if (Event->Turn + 1095 < aGalaxy::Galaxy->CurrentTurn) {
                            break;
                        }
                        if (Event->EventType == u"PlayerDebtNullified") {
                            Factor = Factor * 0.5L;
                        }
                    }
                }
            }
            BusinessQuoteMediumAmount = aMyFunction::RoundAndTruncateToHundreds(static_cast<long double>(std::min<std::int64_t>(static_cast<std::int64_t>(1000000), static_cast<std::int64_t>(([&] {
                std::int64_t cpp_right = System::Round(aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn / 191 + aPlayer::GetPlayer()->DockedTo->Seed), 0.0, 1.0, 3.0, 6.0));
                return pas::idiv(aGalaxy::Galaxy->AverageRangerCapital, cpp_right);
            }())))) * Factor);
            BusinessQuoteSmallAmount = aMyFunction::RoundAndTruncateToHundreds(BusinessQuoteMediumAmount / 2);
            BusinessQuoteLargeAmount = BusinessQuoteMediumAmount * 2;
            Days = System::Round(aMyFunction::RemapClamped(aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn / 377 + aPlayer::GetPlayer()->DockedTo->Seed), 0.0, 1.0, 1.0, 3.0) * 3.6E+2L);
            BusinessQuoteMediumDueTurn = aGalaxy::Galaxy->CurrentTurn + Days;
            BusinessQuoteLargeDueTurn = aGalaxy::Galaxy->CurrentTurn + Days / 2;
            BusinessQuoteSmallDueTurn = aGalaxy::Galaxy->CurrentTurn + Days * 3;
            DialogText = aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.BK"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MinMoney>"_w, pas::wide_int_to_str(BusinessQuoteSmallAmount), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<AveMoney>"_w, pas::wide_int_to_str(BusinessQuoteMediumAmount), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MaxMoney>"_w, pas::wide_int_to_str(BusinessQuoteLargeAmount), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MinDay>"_w, pas::wide_int_to_str(BusinessQuoteLargeDueTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<AveDay>"_w, pas::wide_int_to_str(BusinessQuoteMediumDueTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MaxDay>"_w, pas::wide_int_to_str(BusinessQuoteSmallDueTurn - aGalaxy::Galaxy->CurrentTurn), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MinMoneyAdd>"_w, pas::wide_int64_to_str(System::Round(BusinessQuoteSmallAmount * 1.1L) - BusinessQuoteSmallAmount), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<AveMoneyAdd>"_w, pas::wide_int64_to_str(System::Round(BusinessQuoteMediumAmount * 1.15L) - BusinessQuoteMediumAmount), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MaxMoneyAdd>"_w, pas::wide_int64_to_str(System::Round(BusinessQuoteLargeAmount * 1.2L) - BusinessQuoteLargeAmount), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MinMoneyReturn>"_w, pas::wide_int64_to_str(System::Round(BusinessQuoteSmallAmount * 1.1L)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<AveMoneyReturn>"_w, pas::wide_int64_to_str(System::Round(BusinessQuoteMediumAmount * 1.15L)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<MaxMoneyReturn>"_w, pas::wide_int64_to_str(System::Round(BusinessQuoteLargeAmount * 1.2L)), u"<color=255,240,100>"_w);
            ClearChoices();
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterDebtQuote>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.PlayerOkMaxMoney"_wref.get())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 1, cpp_arg);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterDebtQuote>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.PlayerOkAveMoney"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 2, cpp_arg_3);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterDebtQuote>(this);
                pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.PlayerOkMinMoney"_wref.get())});
                TfRuinsTalk* self_3 = this;
                self_3->AddChoice(std::move(cpp_arg_6), 3, cpp_arg_5);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::DeclineBusinessCenterDebtDialog>(this);
                pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.PlayerNo"_wref.get())});
                TfRuinsTalk* self_4 = this;
                self_4->AddChoice(std::move(cpp_arg_8), 0, cpp_arg_7);
            }
        }
    }

    // Quotes 1/2/3 use the large/medium/small principal, with 20/15/10 percent interest included in DebtAmount.
    void TfRuinsTalk::AcceptBusinessCenterDebtQuote(std::int32_t Quote) {
        std::int32_t Amount{};
        switch (Quote) {
            case 1: {
                aPlayer::GetPlayer()->DebtAmount = System::Round(BusinessQuoteLargeAmount * 1.2L);
                aPlayer::GetPlayer()->DebtDueTurn = BusinessQuoteLargeDueTurn;
                Amount = BusinessQuoteLargeAmount;
                break;
            }
            case 2: {
                aPlayer::GetPlayer()->DebtAmount = System::Round(BusinessQuoteMediumAmount * 1.15L);
                aPlayer::GetPlayer()->DebtDueTurn = BusinessQuoteMediumDueTurn;
                Amount = BusinessQuoteMediumAmount;
                break;
            }
            case 3: {
                aPlayer::GetPlayer()->DebtAmount = System::Round(BusinessQuoteSmallAmount * 1.1L);
                aPlayer::GetPlayer()->DebtDueTurn = BusinessQuoteSmallDueTurn;
                Amount = BusinessQuoteSmallAmount;
                break;
            }
            default: {
                DialogText = aConst::LocalizedColorText(u"Error I_TakeDebtOk data<>1,2,3"_wref.get());
                M_Main(true);
                return;
            }
        }
        aPlayer::GetPlayer()->DebtDefaultCount = 0;
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + Amount);
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.BKAfterOk"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<SendMoney>"_w, pas::wide_int_to_str(Amount), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<DebtMoney>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->DebtAmount), u"<color=255,240,100>"_w);
        {
            pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(aPlayer::GetPlayer()->DebtDueTurn);
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<Date>"_w, std::move(formatTurnDate), u"<color=255,240,100>"_w);
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::DeclineBusinessCenterDebtDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.TakeDebt.BKAfterNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    // Requires the menu's prior affordability check.
    void TfRuinsTalk::RepayBusinessCenterDebt(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.RetDebt.BK"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aPlayer::GetPlayer()->DebtDefaultCount = 0;
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - aPlayer::GetPlayer()->DebtAmount);
        aPlayer::GetPlayer()->DebtAmount = 0;
        aPlayer::GetPlayer()->DebtDueTurn = 0;
        M_Main(true);
    }

    void TfRuinsTalk::ShowBusinessCenterDepositDialog(std::int32_t Action) {
        pas::WideString Text{};
        BusinessQuoteLargeAmount = std::min<std::int32_t>(10000000, std::max<std::int32_t>(1000, aPlayer::GetPlayer()->Money));
        BusinessQuoteMediumAmount = std::min<std::int32_t>(10000000, std::max<std::int32_t>(1000, aPlayer::GetPlayer()->Money / 2));
        BusinessQuoteSmallAmount = std::min<std::int32_t>(10000000, std::max<std::int32_t>(1000, aPlayer::GetPlayer()->Money / 4));
        BusinessDepositQuoteInterestRate = MathImports::RoundTo(aMyFunction::RemapClamped(aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators), 5.0, 95.0, 7.0, 1.0), -1);
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Deposit.BK"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Percent>"_w, static_cast<pas::WideString>(SysUtilsImports::FloatToStrF(BusinessDepositQuoteInterestRate, SysUtilsImports::ffFixed, 1, 1)), u"<color=255,240,100>"_w);
        ClearChoices();
        Text = ([&] {
            pas::WideString intToStr = pas::wide_int_to_str(BusinessQuoteLargeAmount);
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.BK.Deposit.PlayerOkMaxMoney"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<MaxMoney>"_w, std::move(intToStr));
        }());
        if (aPlayer::GetPlayer()->Money >= BusinessQuoteLargeAmount) {
            AddChoice(pas::concat_wide({u"- ", Text}), 1, pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterDepositQuote>(this));
        } else {
            AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
        }
        if (BusinessQuoteMediumAmount != BusinessQuoteLargeAmount) {
            Text = ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(BusinessQuoteMediumAmount);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.BK.Deposit.PlayerOkAveMoney"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<AveMoney>"_w, std::move(intToStr_2));
            }());
            if (aPlayer::GetPlayer()->Money >= BusinessQuoteMediumAmount) {
                AddChoice(pas::concat_wide({u"- ", Text}), 2, pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterDepositQuote>(this));
            } else {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
            }
        }
        if (BusinessQuoteSmallAmount != BusinessQuoteLargeAmount && BusinessQuoteSmallAmount != BusinessQuoteMediumAmount) {
            Text = ([&] {
                pas::WideString intToStr_3 = pas::wide_int_to_str(BusinessQuoteSmallAmount);
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRuins.BK.Deposit.PlayerOkMinMoney"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<MinMoney>"_w, std::move(intToStr_3));
            }());
            if (aPlayer::GetPlayer()->Money >= BusinessQuoteSmallAmount) {
                AddChoice(pas::concat_wide({u"- ", Text}), 3, pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterDepositQuote>(this));
            } else {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
            }
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::DeclineBusinessCenterDepositDialog>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Deposit.PlayerNo"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
    }

    // Quotes 1/2/3 select large/medium/small amounts; a new deposit resets accrued days.
    void TfRuinsTalk::AcceptBusinessCenterDepositQuote(std::int32_t Quote) {
        switch (Quote) {
            case 1: aPlayer::GetPlayer()->DepositAmount = System::Round(BusinessQuoteLargeAmount); break;
            case 2: aPlayer::GetPlayer()->DepositAmount = System::Round(BusinessQuoteMediumAmount); break;
            case 3: aPlayer::GetPlayer()->DepositAmount = System::Round(BusinessQuoteSmallAmount); break;
        }
        aPlayer::GetPlayer()->DepositInterestRate = BusinessDepositQuoteInterestRate;
        aPlayer::GetPlayer()->DepositDayCount = 0;
        aPlayer::GetPlayer()->DepositStartTurn = aGalaxy::Galaxy->CurrentTurn;
        aPlayer::GetPlayer()->SetMoney(std::max<std::int32_t>(0, aPlayer::GetPlayer()->Money - aPlayer::GetPlayer()->DepositAmount));
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Deposit.BKAfterOk"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<SendMoney>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->DepositAmount), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Percent>"_w, static_cast<pas::WideString>(SysUtilsImports::FloatToStrF(BusinessDepositQuoteInterestRate, SysUtilsImports::ffFixed, 1, 1)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::DeclineBusinessCenterDepositDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Deposit.BKAfterNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::WithdrawBusinessCenterDeposit(std::int32_t Action) {
        std::int32_t Profit{};
        if (aPlayer::GetPlayer()->CurrentStar->Status.Battle != 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.BK.RetDeposit.War"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.BK.RetDeposit.BK"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            Profit = aPlayer::GetPlayer()->ComputeDepositAccruedValue() - aPlayer::GetPlayer()->DepositAmount;
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money + aPlayer::GetPlayer()->ComputeDepositAccruedValue());
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckInvestorAchievement(Profit);
            aPlayer::GetPlayer()->DepositAmount = 0;
            aPlayer::GetPlayer()->DepositStartTurn = 0;
            aPlayer::GetPlayer()->DepositDayCount = 0;
            aPlayer::GetPlayer()->DepositInterestRate = 0.0f;
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        }
        M_Main(true);
    }

    void TfRuinsTalk::ShowBusinessCenterMedicalPolicyDialog(std::int32_t Refresh) {
        if (Refresh == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Policy.BK"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(aGalaxy::Galaxy->ComputeScaledAverageMoney(aGalaxyStruct::oiHuman)), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Year>"_w, pas::wide_int_to_str(5), u"<color=255,240,100>"_w);
        }
        ClearChoices();
        if (aGalaxy::Galaxy->ComputeScaledAverageMoney(aGalaxyStruct::oiHuman) <= aPlayer::GetPlayer()->Money) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::BuyBusinessCenterMedicalPolicy>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Policy.PlayerOk"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Policy.PlayerOk"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        if (Refresh == 0) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::ShowBusinessCenterPolicyDetails>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Policy.PlayerAsk"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_6 = pas::bind_method<&TfRuinsTalk::DeclineBusinessCenterPolicy>(this);
            pas::WideString cpp_arg_7 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Policy.PlayerNo"_wref.get())});
            TfRuinsTalk* self_4 = this;
            self_4->AddChoice(std::move(cpp_arg_7), 0, cpp_arg_6);
        }
    }

    // Policy duration is 1825 ticks.
    void TfRuinsTalk::BuyBusinessCenterMedicalPolicy(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Policy.BKAfterOk"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(aGalaxy::Galaxy->ComputeScaledAverageMoney(aGalaxyStruct::oiHuman)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Year>"_w, pas::wide_int_to_str(5), u"<color=255,240,100>"_w);
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - aGalaxy::Galaxy->ComputeScaledAverageMoney(aGalaxyStruct::oiHuman));
        aPlayer::GetPlayer()->MedicalPolicyTicks = 1825;
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowBusinessCenterPolicyDetails(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Policy.BKAfterAsk"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        ClearChoices();
        ShowBusinessCenterMedicalPolicyDialog(1);
    }

    void TfRuinsTalk::DeclineBusinessCenterPolicy(std::int32_t Action) {
        pas::AnsiString YearText{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Policy.BKAfterNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        System::TDateTime Date = aGalaxy::Galaxy->TurnToDateTime(-1);
        SysUtilsImports::DateTimeToString(YearText, "yyyy"_a, Date);
        aMyFunction::ReplaceTextToken(DialogText, u"<CurYear>"_w, static_cast<pas::WideString>(YearText), u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowBusinessCenterInvestmentDialog(std::int32_t Action) {
        static const pas::Set<0, 255> StationTypes = pas::constant_set<pas::Set<0, 255>>({{6, 12}});
        std::int32_t I{};
        std::int32_t Index{};
        std::int32_t BoundA{};
        std::int32_t BoundB{};
        std::int32_t BestScore{};
        std::int32_t Score{};
        std::uint8_t Kind{};
        std::uint8_t Choice{};
        pas::WideString Offers{};
        pas::WideString Text{};
        pas::WideString Name{};
        aGalaxy::TStar* Star{};
        aGalaxy::TStar* BestStar{};
        aPlanet::TPlanet* Planet{};
        aPlanet::TPlanet* BestPlanet{};
        ClearChoices();
        for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(11)); cpp_range.next(Kind); ) {
            Choice = Kind;
            Text = pas::WideString();
            BoundA = 1;
            BoundB = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
            Index = aMyFunction::SeededRandomIntRange(BoundA, BoundB, aPlayer::GetPlayer()->DockedTo->Seed + aGalaxy::Galaxy->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 1743 + 731 * Kind);
            if (aPlayer::GetPlayer()->StationServiceLastUseTurns[Kind] <= aGalaxy::Galaxy->CurrentTurn - aConst::StationServiceRepeatPeriods[Kind]) {
                switch (Kind) {
                    case aGalaxyStruct::cpCreateRangerCenter: {
                        if (aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::rstRangerCenter] > aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 0.33L) {
                            continue;
                        }
                        BestStar = nullptr;
                        BestScore = 0;
                        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
                            aMyFunction::IncrementWrapped(Index, BoundA, BoundB);
                            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                            if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 7281) >= 0.6L && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstRangerCenter] <= 0 && Star->Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) < pas::list_count(Star->Constellation->Stars) && Star->ShipTypeCounts[aGalaxyStruct::rstRangerCenter] <= 0 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) <= 2 && Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && (Star->Status.Battle == 0 || Star->CountPirateShips(false) <= 0) && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->CurrentStar != Star && Star->DaysSincePlayerVisit >= 30 && Star->IsConstellationVisible()) {
                                {
                                    std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(1, 50, Star->GenerationSeed);
                                    std::int32_t cpp_left_3 = std::min<std::int32_t>(40, Star->DaysSincePlayerVisit) + 200 - cpp_right;
                                    std::int32_t cpp_left_2 = cpp_left_3 - 10 * Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstRangerCenter];
                                    std::int32_t cpp_left = cpp_left_2 - Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes));
                                    Score = cpp_left - System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, Star->Position));
                                }
                                if (BestScore <= Score) {
                                    BestScore = Score;
                                    BestStar = Star;
                                }
                            }
                        }
                        if (BestStar == nullptr) {
                            continue;
                        }
                        InvestmentRangerCenterStar = BestStar;
                        {
                            std::int32_t cpp_arg = 2 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_2 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 2;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_2, cpp_arg, 1171 * (Kind + 13) + InvestmentRangerCenterStar->GenerationSeed);
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Star>"_w, BestStar->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
                            }())});
                            TfRuinsTalk* self = this;
                            self->AddChoice(std::move(cpp_arg_4), Choice, cpp_arg_3);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_2 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
                            }())});
                            TfRuinsTalk* self_2 = this;
                            self_2->AddChoice(std::move(cpp_arg_5), 0, scriptDialogBlockCallback);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpCreatePirateBase: {
                        if (aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::rstPirateBase] > aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 0.22L) {
                            continue;
                        }
                        BestStar = nullptr;
                        BestScore = 0;
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(I); ) {
                            aMyFunction::IncrementWrapped(Index, BoundA, BoundB);
                            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                            if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 113223) >= 0.6L && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstPirateBase] <= 0 && Star->Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) < pas::list_count(Star->Constellation->Stars) && Star->ShipTypeCounts[aGalaxyStruct::rstPirateBase] <= 0 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) <= 1 && Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && pas::is_one_of<aGalaxyStruct::sfCoalition, aGalaxyStruct::sfPirates>(Star->Status.ControlFaction) && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->CurrentStar != Star && Star->DaysSincePlayerVisit >= 30 && Star->IsConstellationVisible()) {
                                {
                                    std::int32_t cpp_right_2 = aMyFunction::SeededRandomIntRange(1, 50, Star->GenerationSeed);
                                    std::int32_t cpp_left_6 = std::min<std::int32_t>(40, Star->DaysSincePlayerVisit) + 200 - cpp_right_2;
                                    std::int32_t cpp_left_5 = cpp_left_6 - 10 * Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstPirateBase];
                                    std::int32_t cpp_left_4 = cpp_left_5 - Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes));
                                    Score = cpp_left_4 - System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, Star->Position));
                                }
                                if (BestScore <= Score) {
                                    BestScore = Score;
                                    BestStar = Star;
                                }
                            }
                        }
                        if (BestStar == nullptr) {
                            continue;
                        }
                        InvestmentPirateBaseStar = BestStar;
                        {
                            std::int32_t cpp_arg_6 = 2 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_7 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 2;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_7, cpp_arg_6, 1171 * (Kind + 13) + InvestmentPirateBaseStar->GenerationSeed);
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Star>"_w, BestStar->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_8 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_9 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_3 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_3));
                            }())});
                            TfRuinsTalk* self_3 = this;
                            self_3->AddChoice(std::move(cpp_arg_9), Choice, cpp_arg_8);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_2 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_10 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_4 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_4 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_4));
                            }())});
                            TfRuinsTalk* self_4 = this;
                            self_4->AddChoice(std::move(cpp_arg_10), 0, scriptDialogBlockCallback_2);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpCreateMilitaryBase: {
                        if (aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::rstMilitaryBase] > aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 0.22L) {
                            continue;
                        }
                        BestStar = nullptr;
                        BestScore = 0;
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_4.next(I); ) {
                            aMyFunction::IncrementWrapped(Index, BoundA, BoundB);
                            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                            if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 17823) >= 0.6L && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstMilitaryBase] <= 0 && Star->Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) < pas::list_count(Star->Constellation->Stars) && Star->ShipTypeCounts[aGalaxyStruct::rstMilitaryBase] <= 0 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) <= 1 && Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && (Star->Status.Battle == 0 || Star->CountPirateShips(false) <= 0) && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->CurrentStar != Star && Star->DaysSincePlayerVisit >= 30 && Star->IsConstellationVisible()) {
                                {
                                    std::int32_t cpp_right_3 = aMyFunction::SeededRandomIntRange(1, 50, Star->GenerationSeed);
                                    std::int32_t cpp_left_9 = std::min<std::int32_t>(40, Star->DaysSincePlayerVisit) + 200 - cpp_right_3;
                                    std::int32_t cpp_left_8 = cpp_left_9 - 10 * Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstMilitaryBase];
                                    std::int32_t cpp_left_7 = cpp_left_8 - Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes));
                                    Score = cpp_left_7 - System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, Star->Position));
                                }
                                if (BestScore <= Score) {
                                    BestScore = Score;
                                    BestStar = Star;
                                }
                            }
                        }
                        if (BestStar == nullptr) {
                            continue;
                        }
                        InvestmentMilitaryBaseStar = BestStar;
                        {
                            std::int32_t cpp_arg_11 = 3 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_12 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 2;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_12, cpp_arg_11, 1171 * (Kind + 13) + InvestmentMilitaryBaseStar->GenerationSeed);
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Star>"_w, BestStar->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_13 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_14 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_5 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_5 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_5), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_5));
                            }())});
                            TfRuinsTalk* self_5 = this;
                            self_5->AddChoice(std::move(cpp_arg_14), Choice, cpp_arg_13);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_3 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_15 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_6 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_6 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_6), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_6));
                            }())});
                            TfRuinsTalk* self_6 = this;
                            self_6->AddChoice(std::move(cpp_arg_15), 0, scriptDialogBlockCallback_3);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpCreateScienceBase: {
                        if (aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::rstScienceBase] > aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 0.15L) {
                            continue;
                        }
                        BestStar = nullptr;
                        BestScore = 0;
                        for (auto cpp_range_5 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_5.next(I); ) {
                            aMyFunction::IncrementWrapped(Index, BoundA, BoundB);
                            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                            if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 11123) >= 0.6L && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstScienceBase] <= 0 && Star->Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) < pas::list_count(Star->Constellation->Stars) && Star->ShipTypeCounts[aGalaxyStruct::rstScienceBase] <= 0 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) <= 1 && Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && (Star->Status.Battle == 0 || Star->CountPirateShips(false) <= 0) && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->CurrentStar != Star && Star->DaysSincePlayerVisit >= 30 && Star->IsConstellationVisible()) {
                                {
                                    std::int32_t cpp_right_4 = aMyFunction::SeededRandomIntRange(1, 50, Star->GenerationSeed);
                                    std::int32_t cpp_left_12 = std::min<std::int32_t>(40, Star->DaysSincePlayerVisit) + 200 - cpp_right_4;
                                    std::int32_t cpp_left_11 = cpp_left_12 - 10 * Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstScienceBase];
                                    std::int32_t cpp_left_10 = cpp_left_11 - Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes));
                                    Score = cpp_left_10 - System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, Star->Position));
                                }
                                if (BestScore <= Score) {
                                    BestScore = Score;
                                    BestStar = Star;
                                }
                            }
                        }
                        if (BestStar == nullptr) {
                            continue;
                        }
                        InvestmentScienceBaseStar = BestStar;
                        {
                            std::int32_t cpp_arg_16 = 4 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t computeScaledHugeMoney = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(computeScaledHugeMoney, cpp_arg_16, 1172 * (Kind + 13) + InvestmentScienceBaseStar->GenerationSeed);
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Star>"_w, BestStar->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_17 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_18 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_7 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_7 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_7), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_7));
                            }())});
                            TfRuinsTalk* self_7 = this;
                            self_7->AddChoice(std::move(cpp_arg_18), Choice, cpp_arg_17);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_4 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_19 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_8 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_8 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_8), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_8));
                            }())});
                            TfRuinsTalk* self_8 = this;
                            self_8->AddChoice(std::move(cpp_arg_19), 0, scriptDialogBlockCallback_4);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpCreateBusinessCenter: {
                        if (aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::rstBusinessCenter] > aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 0.1L) {
                            continue;
                        }
                        BestStar = nullptr;
                        BestScore = 0;
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_6.next(I); ) {
                            aMyFunction::IncrementWrapped(Index, BoundA, BoundB);
                            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                            if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 9112323) >= 0.6L && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstBusinessCenter] <= 0 && Star->Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) < pas::list_count(Star->Constellation->Stars) && Star->ShipTypeCounts[aGalaxyStruct::rstBusinessCenter] <= 0 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) <= 1 && Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && (Star->Status.Battle == 0 || Star->CountPirateShips(false) <= 0) && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->CurrentStar != Star && Star->DaysSincePlayerVisit >= 30 && Star->IsConstellationVisible()) {
                                {
                                    std::int32_t cpp_right_5 = aMyFunction::SeededRandomIntRange(1, 50, Star->GenerationSeed);
                                    std::int32_t cpp_left_15 = std::min<std::int32_t>(40, Star->DaysSincePlayerVisit) + 200 - cpp_right_5;
                                    std::int32_t cpp_left_14 = cpp_left_15 - 10 * Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstBusinessCenter];
                                    std::int32_t cpp_left_13 = cpp_left_14 - Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes));
                                    Score = cpp_left_13 - System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, Star->Position));
                                }
                                if (BestScore <= Score) {
                                    BestScore = Score;
                                    BestStar = Star;
                                }
                            }
                        }
                        if (BestStar == nullptr) {
                            continue;
                        }
                        InvestmentBusinessCenterStar = BestStar;
                        {
                            std::int32_t cpp_arg_20 = 2 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_21 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 2;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_21, cpp_arg_20, 1173 * (Kind + 13) + InvestmentBusinessCenterStar->GenerationSeed);
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Star>"_w, BestStar->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_22 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_23 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_9 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_9 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_9), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_9));
                            }())});
                            TfRuinsTalk* self_9 = this;
                            self_9->AddChoice(std::move(cpp_arg_23), Choice, cpp_arg_22);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_5 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_24 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_10 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_10 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_10), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_10));
                            }())});
                            TfRuinsTalk* self_10 = this;
                            self_10->AddChoice(std::move(cpp_arg_24), 0, scriptDialogBlockCallback_5);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpCreateMedicalBase: {
                        if (aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::rstMedicalBase] > aGalaxy::Galaxy->CountFactionStars(aGalaxyStruct::sfCoalition) * 0.15L) {
                            continue;
                        }
                        BestStar = nullptr;
                        BestScore = 0;
                        for (auto cpp_range_7 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_7.next(I); ) {
                            aMyFunction::IncrementWrapped(Index, BoundA, BoundB);
                            Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                            if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 1123087) >= 0.6L && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstMedicalBase] <= 0 && Star->Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) < pas::list_count(Star->Constellation->Stars) && Star->ShipTypeCounts[aGalaxyStruct::rstMedicalBase] <= 0 && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes)) <= 1 && Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Status.ControlFaction == aGalaxyStruct::sfCoalition && (Star->Status.Battle == 0 || Star->CountPirateShips(false) <= 0) && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->CurrentStar != Star && Star->DaysSincePlayerVisit >= 30 && Star->IsConstellationVisible()) {
                                {
                                    std::int32_t cpp_right_6 = aMyFunction::SeededRandomIntRange(1, 50, Star->GenerationSeed);
                                    std::int32_t cpp_left_18 = std::min<std::int32_t>(40, Star->DaysSincePlayerVisit) + 200 - cpp_right_6;
                                    std::int32_t cpp_left_17 = cpp_left_18 - 10 * Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstMedicalBase];
                                    std::int32_t cpp_left_16 = cpp_left_17 - Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationTypes));
                                    Score = cpp_left_16 - System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, Star->Position));
                                }
                                if (BestScore <= Score) {
                                    BestScore = Score;
                                    BestStar = Star;
                                }
                            }
                        }
                        if (BestStar == nullptr) {
                            continue;
                        }
                        InvestmentMedicalBaseStar = BestStar;
                        {
                            std::int32_t cpp_arg_25 = 2 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_26 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 2;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_26, cpp_arg_25, 1174 * (Kind + 13) + InvestmentMedicalBaseStar->GenerationSeed);
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Star>"_w, BestStar->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_27 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_28 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_11 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_11 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_11), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_11));
                            }())});
                            TfRuinsTalk* self_11 = this;
                            self_11->AddChoice(std::move(cpp_arg_28), Choice, cpp_arg_27);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_6 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_29 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_12 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_12 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_12), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_12));
                            }())});
                            TfRuinsTalk* self_12 = this;
                            self_12->AddChoice(std::move(cpp_arg_29), 0, scriptDialogBlockCallback_6);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpRangersSubsidy: {
                        if (aGalaxy::Galaxy->CountEligibleRangers() < 20) {
                            continue;
                        }
                        if (aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn / 71 + aPlayer::GetPlayer()->DockedTo->Seed + 16689) < 0.5L) {
                            continue;
                        }
                        {
                            std::int32_t cpp_arg_30 = 2 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_31 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 4;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_31, cpp_arg_30, 1123475 * (Kind + 13));
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_32 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_33 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_13 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_13 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_13), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_13));
                            }())});
                            TfRuinsTalk* self_13 = this;
                            self_13->AddChoice(std::move(cpp_arg_33), Choice, cpp_arg_32);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_7 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_34 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_14 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_14 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_14), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_14));
                            }())});
                            TfRuinsTalk* self_14 = this;
                            self_14->AddChoice(std::move(cpp_arg_34), 0, scriptDialogBlockCallback_7);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpPiratesSubsidy: {
                        if (aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn / 71 + aPlayer::GetPlayer()->DockedTo->Seed + 5789) < 0.7L) {
                            continue;
                        }
                        {
                            std::int32_t cpp_arg_35 = 2 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_36 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 2;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_36, cpp_arg_35, 1175234 * (Kind + 13));
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_37 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_38 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_15 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_15 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_15), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_15));
                            }())});
                            TfRuinsTalk* self_15 = this;
                            self_15->AddChoice(std::move(cpp_arg_38), Choice, cpp_arg_37);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_8 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_39 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_16 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_16 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_16), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_16));
                            }())});
                            TfRuinsTalk* self_16 = this;
                            self_16->AddChoice(std::move(cpp_arg_39), 0, scriptDialogBlockCallback_8);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpTransportSubsidy: {
                        if (aMyFunction::SeededRandomUnitFloat(aGalaxy::Galaxy->CurrentTurn / 71 + aPlayer::GetPlayer()->DockedTo->Seed + 23739) < 0.5L) {
                            continue;
                        }
                        {
                            std::int32_t computeScaledHugeMoney_2 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t cpp_arg_40 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace)) / 4;
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(cpp_arg_40, computeScaledHugeMoney_2, 117627 * (Kind + 13));
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_41 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_42 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_17 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_17 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_17), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_17));
                            }())});
                            TfRuinsTalk* self_17 = this;
                            self_17->AddChoice(std::move(cpp_arg_42), Choice, cpp_arg_41);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_9 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_43 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_18 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_18 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_18), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_18));
                            }())});
                            TfRuinsTalk* self_18 = this;
                            self_18->AddChoice(std::move(cpp_arg_43), 0, scriptDialogBlockCallback_9);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpLostSubsidy: {
                        {
                            std::int32_t cpp_arg_44 = 4 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t computeScaledHugeMoney_3 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(computeScaledHugeMoney_3, cpp_arg_44, 1177961 * (Kind + 13));
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_45 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_46 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_19 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_19 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_19), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_19));
                            }())});
                            TfRuinsTalk* self_19 = this;
                            self_19->AddChoice(std::move(cpp_arg_46), Choice, cpp_arg_45);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_10 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_47 = pas::concat_wide({u"- ", ([&] {
                                pas::WideString intToStr_20 = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText_20 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_20), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_20));
                            }())});
                            TfRuinsTalk* self_20 = this;
                            self_20->AddChoice(std::move(cpp_arg_47), 0, scriptDialogBlockCallback_10);
                        }
                        break;
                    }
                    case aGalaxyStruct::cpWarSubsidy: {
                        BestPlanet = nullptr;
                        BestScore = 0;
                        BoundA = 0;
                        BoundB = pas::list_count(aGalaxy::Galaxy->Planets) - 1;
                        Index = aMyFunction::SeededRandomIntRange(BoundA, BoundB, aPlayer::GetPlayer()->DockedTo->Seed + aGalaxy::Galaxy->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 174313 + 73163 * Kind);
                        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Planets) - 1); cpp_range_8.next(I); ) {
                            aMyFunction::IncrementWrapped(Index, BoundA, BoundB);
                            Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::Galaxy->Planets, Index);
                            if (Planet->IsCoalitionOwned && static_cast<std::uint8_t>(Planet->IsMainPiratePlanet ^ 1) && aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Planet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 5889) >= 0.9L && Planet->CurrentStar->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Planet->OwnerId != aGalaxyStruct::oiPirate && (Planet->CurrentStar->Status.Battle == 0 || Planet->CurrentStar->CountPirateShips(false) <= 0) && Planet->CurrentStar->DaysSincePlayerVisit >= 30 && Planet->CurrentStar->IsConstellationVisible()) {
                                {
                                    std::int32_t cpp_right_7 = aMyFunction::SeededRandomIntRange(1, 50, Planet->GenerationSeed);
                                    std::int32_t cpp_left_20 = std::min<std::int32_t>(40, Planet->CurrentStar->DaysSincePlayerVisit) + 200 - cpp_right_7;
                                    std::int32_t cpp_left_19 = cpp_left_20 - 10 * pas::list_count(Planet->Warriors);
                                    Score = cpp_left_19 - System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, Planet->CurrentStar->Position));
                                }
                                if (BestScore <= Score) {
                                    BestScore = Score;
                                    BestPlanet = Planet;
                                }
                            }
                        }
                        if (BestPlanet == nullptr) {
                            continue;
                        }
                        InvestmentDefensePlanet = BestPlanet;
                        {
                            std::int32_t cpp_arg_48 = 5 * aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            std::int32_t computeScaledHugeMoney_4 = aGalaxy::Galaxy->ComputeScaledHugeMoney(aConst::RaceToOwner(aPlayer::GetPlayer()->PilotRace));
                            StationServiceQuoteCost = aMyFunction::SeededRandomIntRange(computeScaledHugeMoney_4, cpp_arg_48, 1178 * (Kind + 13) + InvestmentDefensePlanet->GenerationSeed);
                        }
                        Name = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Name"}));
                        aMyFunction::ReplaceTextToken(Name, u"<Planet>"_w, BestPlanet->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Star>"_w, BestPlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Name, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                        Text = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKInvestment"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<InvestmentFullName>"_w, Name, pas::WideString());
                        if (Offers == u"") {
                            Offers = pas::concat_wide({Offers, Text});
                        } else {
                            Offers = pas::concat_wide({Offers, u"\r\n", Text});
                        }
                        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_49 = pas::bind_method<&TfRuinsTalk::AcceptBusinessCenterInvestment>(this);
                            pas::WideString cpp_arg_50 = pas::concat_wide({u"- ", ([&] {
                                auto name = pas::borrow(InvestmentDefensePlanet->Name);
                                pas::WideString localizedColorText_21 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_21), u"<color=255,240,100>"_w, u"<Planet>"_w, name.get());
                            }())});
                            TfRuinsTalk* self_21 = this;
                            self_21->AddChoice(std::move(cpp_arg_50), Choice, cpp_arg_49);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_11 = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_51 = pas::concat_wide({u"- ", ([&] {
                                auto name_2 = pas::borrow(InvestmentDefensePlanet->Name);
                                pas::WideString localizedColorText_22 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".PlayerSend"}));
                                return aMyFunction::FormatText1(std::move(localizedColorText_22), u"<color=255,240,100>"_w, u"<Planet>"_w, name_2.get());
                            }())});
                            TfRuinsTalk* self_22 = this;
                            self_22->AddChoice(std::move(cpp_arg_51), 0, scriptDialogBlockCallback_11);
                        }
                        break;
                    }
                }
                InvestmentQuoteCosts[Kind] = StationServiceQuoteCost;
            }
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BK"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<BKInvestment>"_w, Offers, pas::WideString());
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_52 = pas::bind_method<&TfRuinsTalk::DeclineBusinessCenterInvestment>(this);
            pas::WideString cpp_arg_53 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Investment.PlayerNo"_wref.get())});
            TfRuinsTalk* self_23 = this;
            self_23->AddChoice(std::move(cpp_arg_53), 0, cpp_arg_52);
        }
    }

    void TfRuinsTalk::AcceptBusinessCenterInvestment(std::int32_t Action) {
        static const pas::Set<0, 255> RangerTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htRanger}});
        static const pas::Set<0, 255> FriendlyTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htRanger}, {aGalaxyStruct::htTransport, aGalaxyStruct::htDiplomat}});
        static const pas::Set<0, 255> PirateTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htPirate}});
        static const pas::Set<0, 255> TransportTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htRanger, 15}}) - pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htRanger, aGalaxyStruct::htPirate}, {aGalaxyStruct::htDiplomat, 15}});
        std::uint8_t Kind{};
        aRuins::TRuins* RangerCenter{};
        aRuins::TRuins* PirateBase{};
        aRuins::TRuins* MilitaryBase{};
        aRuins::TRuins* ScienceBase{};
        aRuins::TRuins* BusinessCenter{};
        aRuins::TRuins* MedicalBase{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t Experience{};
        std::int32_t RankPoints{};
        std::int32_t Count{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        aRanger::TRanger* Ranger{};
        aWarrior::TWarrior* Warrior{};
        pas::WideString Text{};
        pas::WideString ShipNames{};
        Kind = Action;
        StationServiceQuoteCost = InvestmentQuoteCosts[Kind];
        aPlayer::GetPlayer()->StationServiceLastUseTurns[Kind] = aGalaxy::Galaxy->CurrentTurn;
        switch (Kind) {
            case aGalaxyStruct::cpCreateRangerCenter: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                RangerCenter = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                RangerCenter->Init(aGalaxyStruct::rstRangerCenter, InvestmentRangerCenterStar, pas::WideString());
                {
                    pas::WideString formatText3 = ([&] {
                        pas::WideString name = RangerCenter->GetName();
                        auto name_2 = pas::borrow(RangerCenter->CurrentStar->Name);
                        pas::WideString name_3 = RangerCenter->CurrentStar->Constellation->GetName();
                        pas::WideString pickLocalizedTextVariant = aConst::PickLocalizedTextVariant(u"GalaxyNews.CreateNewObject.RC"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name), u"<Star>"_w, name_2.get(), u"<Sector>"_w, std::move(name_3));
                    }());
                    aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                    galaxy->AddPlanetNewsWithPlayerBubble(41, std::move(formatText3));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText, u"<InvestmentText>"_w, std::move(localizedColorText), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Name>"_w, RangerCenter->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, RangerCenter->CurrentStar->Name, u"<color=255,240,100>"_w);
                Experience = aMyFunction::SeededRandomIntRange(1000, 1500, RangerCenter->Seed);
                aPlayer::GetPlayer()->GainExperience(Experience, 0);
                aMyFunction::ReplaceTextToken(DialogText, u"<Point>"_w, pas::wide_int_to_str(Experience), u"<color=255,240,100>"_w);
                aGalaxy::Galaxy->UpdateConstellationMilitaryStats();
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 10, aConst::PlanetOwnerMasks.Coalition);
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmIncrease, 40, static_cast<aConst::THullShipTypeMask>(RangerTypes), aConst::PlanetOwnerMasks.Coalition);
                Achievements::TryAddAchievementProgress(u"RUINS"_w, 1);
                break;
            }
            case aGalaxyStruct::cpCreatePirateBase: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                PirateBase = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                PirateBase->Init(aGalaxyStruct::rstPirateBase, InvestmentPirateBaseStar, pas::WideString());
                {
                    pas::WideString formatText3_2 = ([&] {
                        pas::WideString name_4 = PirateBase->GetName();
                        auto name_5 = pas::borrow(PirateBase->CurrentStar->Name);
                        pas::WideString name_6 = PirateBase->CurrentStar->Constellation->GetName();
                        pas::WideString pickLocalizedTextVariant_2 = aConst::PickLocalizedTextVariant(u"GalaxyNews.CreateNewObject.PB"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_2), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_4), u"<Star>"_w, name_5.get(), u"<Sector>"_w, std::move(name_6));
                    }());
                    aGalaxy::TGalaxy* galaxy_2 = aGalaxy::Galaxy;
                    galaxy_2->AddPlanetNewsWithPlayerBubble(41, std::move(formatText3_2));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_2 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_2 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_2, u"<InvestmentText>"_w, std::move(localizedColorText_2), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Name>"_w, PirateBase->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, PirateBase->CurrentStar->Name, u"<color=255,240,100>"_w);
                aGalaxy::Galaxy->UpdateConstellationMilitaryStats();
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmDecreaseWithFloor20, 30, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}}));
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmDecreaseWithFloor20, 10, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}, {aGalaxyStruct::oiHuman}}));
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 20, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}}));
                break;
            }
            case aGalaxyStruct::cpCreateMilitaryBase: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                MilitaryBase = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                MilitaryBase->Init(aGalaxyStruct::rstMilitaryBase, InvestmentMilitaryBaseStar, pas::WideString());
                {
                    pas::WideString formatText3_3 = ([&] {
                        pas::WideString name_7 = MilitaryBase->GetName();
                        auto name_8 = pas::borrow(MilitaryBase->CurrentStar->Name);
                        pas::WideString name_9 = MilitaryBase->CurrentStar->Constellation->GetName();
                        pas::WideString pickLocalizedTextVariant_3 = aConst::PickLocalizedTextVariant(u"GalaxyNews.CreateNewObject.WB"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_3), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_7), u"<Star>"_w, name_8.get(), u"<Sector>"_w, std::move(name_9));
                    }());
                    aGalaxy::TGalaxy* galaxy_3 = aGalaxy::Galaxy;
                    galaxy_3->AddPlanetNewsWithPlayerBubble(41, std::move(formatText3_3));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                if (aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                    pas::WideString localizedColorText_3 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_3 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_3, u"<InvestmentText>"_w, std::move(localizedColorText_3), pas::WideString());
                } else {
                    pas::WideString localizedColorText_4 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".TextAlt"}));
                    pas::WideString& dialogText_4 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_4, u"<InvestmentText>"_w, std::move(localizedColorText_4), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Name>"_w, MilitaryBase->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, MilitaryBase->CurrentStar->Name, u"<color=255,240,100>"_w);
                if (aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                    RankPoints = aMyFunction::SeededRandomIntRange(50, 200, MilitaryBase->Seed);
                    aPlayer::GetPlayer()->AddRankPoints(RankPoints);
                    aMyFunction::ReplaceTextToken(DialogText, u"<Point>"_w, pas::wide_int_to_str(RankPoints), u"<color=255,240,100>"_w);
                }
                aGalaxy::Galaxy->UpdateConstellationMilitaryStats();
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 30, aConst::PlanetOwnerMasks.Coalition);
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmIncrease, 10, static_cast<aConst::THullShipTypeMask>(FriendlyTypes), aConst::PlanetOwnerMasks.Coalition);
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmDecreaseWithFloor20, 30, static_cast<aConst::THullShipTypeMask>(PirateTypes), aConst::PlanetOwnerMasks.Coalition);
                Achievements::TryAddAchievementProgress(u"RUINS"_w, 1);
                break;
            }
            case aGalaxyStruct::cpCreateScienceBase: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                ScienceBase = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                ScienceBase->Init(aGalaxyStruct::rstScienceBase, InvestmentScienceBaseStar, pas::WideString());
                {
                    pas::WideString formatText3_4 = ([&] {
                        pas::WideString name_10 = ScienceBase->GetName();
                        auto name_11 = pas::borrow(ScienceBase->CurrentStar->Name);
                        pas::WideString name_12 = ScienceBase->CurrentStar->Constellation->GetName();
                        pas::WideString pickLocalizedTextVariant_4 = aConst::PickLocalizedTextVariant(u"GalaxyNews.CreateNewObject.SB"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_4), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_10), u"<Star>"_w, name_11.get(), u"<Sector>"_w, std::move(name_12));
                    }());
                    aGalaxy::TGalaxy* galaxy_4 = aGalaxy::Galaxy;
                    galaxy_4->AddPlanetNewsWithPlayerBubble(41, std::move(formatText3_4));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_5 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_5 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_5, u"<InvestmentText>"_w, std::move(localizedColorText_5), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Name>"_w, ScienceBase->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, ScienceBase->CurrentStar->Name, u"<color=255,240,100>"_w);
                aGalaxy::Galaxy->UpdateConstellationMilitaryStats();
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmIncrease, 25, static_cast<aConst::THullShipTypeMask>(RangerTypes), aConst::PlanetOwnerMasks.Coalition);
                Achievements::TryAddAchievementProgress(u"RUINS"_w, 1);
                break;
            }
            case aGalaxyStruct::cpCreateBusinessCenter: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                BusinessCenter = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                BusinessCenter->Init(aGalaxyStruct::rstBusinessCenter, InvestmentBusinessCenterStar, pas::WideString());
                {
                    pas::WideString formatText3_5 = ([&] {
                        pas::WideString name_13 = BusinessCenter->GetName();
                        auto name_14 = pas::borrow(BusinessCenter->CurrentStar->Name);
                        pas::WideString name_15 = BusinessCenter->CurrentStar->Constellation->GetName();
                        pas::WideString pickLocalizedTextVariant_5 = aConst::PickLocalizedTextVariant(u"GalaxyNews.CreateNewObject.BK"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_5), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_13), u"<Star>"_w, name_14.get(), u"<Sector>"_w, std::move(name_15));
                    }());
                    aGalaxy::TGalaxy* galaxy_5 = aGalaxy::Galaxy;
                    galaxy_5->AddPlanetNewsWithPlayerBubble(41, std::move(formatText3_5));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_6 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_6 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_6, u"<InvestmentText>"_w, std::move(localizedColorText_6), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Name>"_w, BusinessCenter->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, BusinessCenter->CurrentStar->Name, u"<color=255,240,100>"_w);
                aGalaxy::Galaxy->UpdateConstellationMilitaryStats();
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmIncrease, 30, static_cast<aConst::THullShipTypeMask>(TransportTypes), aConst::PlanetOwnerMasks.Coalition);
                Achievements::TryAddAchievementProgress(u"RUINS"_w, 1);
                break;
            }
            case aGalaxyStruct::cpCreateMedicalBase: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                MedicalBase = pas::construct_call<aRuins::TRuins>(aRuins::TRuins_Create);
                MedicalBase->Init(aGalaxyStruct::rstMedicalBase, InvestmentMedicalBaseStar, pas::WideString());
                {
                    pas::WideString formatText3_6 = ([&] {
                        pas::WideString name_16 = MedicalBase->GetName();
                        auto name_17 = pas::borrow(MedicalBase->CurrentStar->Name);
                        pas::WideString name_18 = MedicalBase->CurrentStar->Constellation->GetName();
                        pas::WideString pickLocalizedTextVariant_6 = aConst::PickLocalizedTextVariant(u"GalaxyNews.CreateNewObject.MC"_wref.get(), aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText3(std::move(pickLocalizedTextVariant_6), u"<color=255,240,100>"_w, u"<Name>"_w, std::move(name_16), u"<Star>"_w, name_17.get(), u"<Sector>"_w, std::move(name_18));
                    }());
                    aGalaxy::TGalaxy* galaxy_6 = aGalaxy::Galaxy;
                    galaxy_6->AddPlanetNewsWithPlayerBubble(41, std::move(formatText3_6));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_7 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_7 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_7, u"<InvestmentText>"_w, std::move(localizedColorText_7), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Name>"_w, MedicalBase->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, MedicalBase->CurrentStar->Name, u"<color=255,240,100>"_w);
                aGalaxy::Galaxy->UpdateConstellationMilitaryStats();
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmIncrease, 30, static_cast<aConst::THullShipTypeMask>(FriendlyTypes), aConst::PlanetOwnerMasks.Coalition);
                Achievements::TryAddAchievementProgress(u"RUINS"_w, 1);
                break;
            }
            case aGalaxyStruct::cpRangersSubsidy: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                Count = 0;
                for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range.next(I); ) {
                    Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                    if (aPlayer::GetPlayer() != Ranger && static_cast<std::uint8_t>(Ranger->ExcludedFromRating ^ 1) && Ranger->Wealth <= aGalaxy::Galaxy->AverageRangerCapital) {
                        ++Count;
                    }
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Rangers) - 1); cpp_range_2.next(I); ) {
                    Ranger = pas::list_at<aRanger::TRanger>(aGalaxy::Galaxy->Rangers, I);
                    if (aPlayer::GetPlayer() != Ranger && static_cast<std::uint8_t>(Ranger->ExcludedFromRating ^ 1) && Ranger->Wealth <= aGalaxy::Galaxy->AverageRangerCapital) {
                        Ranger->SetMoney(Ranger->Money + System::Round(pas::real_divide(StationServiceQuoteCost, Count)));
                    }
                }
                {
                    pas::WideString formatText1 = ([&] {
                        pas::WideString intToStr = pas::wide_int_to_str(StationServiceQuoteCost);
                        pas::WideString pickLocalizedTextVariant_7 = aConst::PickLocalizedTextVariant(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".GalaxyMessage"}), Kind + aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText1(std::move(pickLocalizedTextVariant_7), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
                    }());
                    aGalaxy::TGalaxy* galaxy_7 = aGalaxy::Galaxy;
                    galaxy_7->AddPlanetNewsWithPlayerBubble(42, std::move(formatText1));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_8 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_8 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_8, u"<InvestmentText>"_w, std::move(localizedColorText_8), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                break;
            }
            case aGalaxyStruct::cpPiratesSubsidy: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                Count = std::max<std::int32_t>(1, aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::stPirate]);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(I); ) {
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_4.next(J); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                        if (Ship->TypeId == aGalaxyStruct::stPirate) {
                            Ship->SetMoney(Ship->Money + pas::idiv(StationServiceQuoteCost, Count));
                        }
                    }
                }
                {
                    pas::WideString formatText2 = ([&] {
                        pas::WideString intToStr_2 = pas::wide_int_to_str(StationServiceQuoteCost);
                        auto name_19 = pas::borrow(aPlayer::GetPlayer()->DockedTo->Name);
                        pas::WideString pickLocalizedTextVariant_8 = aConst::PickLocalizedTextVariant(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".GalaxyMessage"}), Kind + aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_8), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2), u"<BK>"_w, name_19.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_8 = aGalaxy::Galaxy;
                    galaxy_8->AddPlanetNewsWithPlayerBubble(42, std::move(formatText2));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_9 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_9 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_9, u"<InvestmentText>"_w, std::move(localizedColorText_9), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                break;
            }
            case aGalaxyStruct::cpTransportSubsidy: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                Count = std::max<std::int32_t>(1, aGalaxy::Galaxy->ShipTypeCounts[aGalaxyStruct::stTransport]);
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_5.next(I); ) {
                    Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_6.next(J); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, J);
                        if (Ship->TypeId == aGalaxyStruct::stTransport) {
                            Ship->SetMoney(Ship->Money + pas::idiv(StationServiceQuoteCost, Count));
                        }
                    }
                }
                {
                    pas::WideString formatText2_2 = ([&] {
                        pas::WideString intToStr_3 = pas::wide_int_to_str(StationServiceQuoteCost);
                        auto name_20 = pas::borrow(aPlayer::GetPlayer()->DockedTo->Name);
                        pas::WideString pickLocalizedTextVariant_9 = aConst::PickLocalizedTextVariant(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".GalaxyMessage"}), Kind + aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_9), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_3), u"<BK>"_w, name_20.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_9 = aGalaxy::Galaxy;
                    galaxy_9->AddPlanetNewsWithPlayerBubble(42, std::move(formatText2_2));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_10 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_10 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_10, u"<InvestmentText>"_w, std::move(localizedColorText_10), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                break;
            }
            case aGalaxyStruct::cpLostSubsidy: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                {
                    pas::WideString formatText2_3 = ([&] {
                        pas::WideString intToStr_4 = pas::wide_int_to_str(StationServiceQuoteCost);
                        auto name_21 = pas::borrow(aPlayer::GetPlayer()->DockedTo->Name);
                        pas::WideString pickLocalizedTextVariant_10 = aConst::PickLocalizedTextVariant(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".GalaxyMessage"}), Kind + aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                        return aMyFunction::FormatText2(std::move(pickLocalizedTextVariant_10), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_4), u"<BK>"_w, name_21.get());
                    }());
                    aGalaxy::TGalaxy* galaxy_10 = aGalaxy::Galaxy;
                    galaxy_10->AddPlanetNewsWithPlayerBubble(42, std::move(formatText2_3));
                }
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_11 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_11 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_11, u"<InvestmentText>"_w, std::move(localizedColorText_11), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 30, aConst::PlanetOwnerMasks.Coalition);
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmIncrease, 20, static_cast<aConst::THullShipTypeMask>(FriendlyTypes), aConst::PlanetOwnerMasks.Coalition);
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmDecreaseWithFloor20, 20, static_cast<aConst::THullShipTypeMask>(PirateTypes), aConst::PlanetOwnerMasks.Coalition);
                break;
            }
            case aGalaxyStruct::cpWarSubsidy: {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
                Count = aMyFunction::SeededRandomIntRange(4, 7, InvestmentDefensePlanet->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60);
                ShipNames = u"\r\n"_w;
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(1, Count); cpp_range_7.next(I); ) {
                    Warrior = pas::checked_cast<aWarrior::TWarrior*>(static_cast<pas::Object*>(InvestmentDefensePlanet->BuyWarrior(100)));
                    Warrior->Name = pas::concat_wide({Warrior->Name, u" ", aPlayer::GetPlayer()->Name});
                    ShipNames = pas::concat_wide({ShipNames, Warrior->GetName(), u"\r\n"});
                }
                Text = aConst::PickLocalizedTextVariant(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".GalaxyMessage"}), Kind + aGalaxy::Galaxy->CurrentTurn / 10 * aPlayer::GetPlayer()->DockedTo->Seed);
                aMyFunction::ReplaceTextToken(Text, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(Count), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Planet>"_w, InvestmentDefensePlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Star>"_w, InvestmentDefensePlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                aGalaxy::Galaxy->AddPlanetNewsWithPlayerBubble(42, Text);
                DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterInvestment"_wref.get());
                {
                    pas::WideString localizedColorText_12 = aConst::LocalizedColorText(pas::concat_wide({u"Investment.", aConst::CoalitionProjectNames[Kind], u".Text"}));
                    pas::WideString& dialogText_12 = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText_12, u"<InvestmentText>"_w, std::move(localizedColorText_12), pas::WideString());
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<ShipsName>"_w, ShipNames, pas::WideString());
                aMyFunction::ReplaceTextToken(DialogText, u"<Count>"_w, pas::wide_int_to_str(Count), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Planet>"_w, InvestmentDefensePlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, InvestmentDefensePlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
                InvestmentDefensePlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 20, aConst::PlanetOwnerMasks.Coalition);
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmDecreaseWithFloor20, 30, static_cast<aConst::THullShipTypeMask>(PirateTypes), aConst::PlanetOwnerMasks.Coalition);
                break;
            }
        }
        M_Main(true);
    }

    void TfRuinsTalk::DeclineBusinessCenterInvestment(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Investment.BKAfterPlayerNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowBusinessCenterTradeDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Trade.BK"_wref.get());
        {
            std::int32_t cpp_arg_3 = aPlayer::GetPlayer()->Money / 100;
            std::int32_t cpp_arg = 2 * aGalaxy::Galaxy->ComputeScaledMiniMoney(aGalaxyStruct::oiHuman);
            std::int32_t cpp_arg_2 = aGalaxy::Galaxy->ComputeScaledMiniMoney(aGalaxyStruct::oiHuman) / 2;
            std::int32_t seededRandomIntRange = aMyFunction::SeededRandomIntRange(cpp_arg_2, cpp_arg, aGalaxy::Galaxy->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 10);
            NearbyTradeAdviceCost = aMyFunction::RoundAndTruncateToTens(std::min<std::int32_t>(cpp_arg_3, seededRandomIntRange) + 30);
        }
        DistantTradeAdviceCost = aMyFunction::RoundAndTruncateToTens(aMyFunction::SeededRandomIntRange(NearbyTradeAdviceCost / 3, NearbyTradeAdviceCost / 2, aGalaxy::Galaxy->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 10 + 1231341) + 10);
        std::uint8_t Discount = System::Round(pas::real_divide(aPlayer::GetPlayer()->CareerStatus[aGalaxyStruct::rcTrader], 1.3L)) + 1;
        NearbyTradeAdviceCost = std::max<std::int64_t>(static_cast<std::int64_t>(10), static_cast<std::int64_t>(NearbyTradeAdviceCost - System::Round(pas::real_divide(NearbyTradeAdviceCost, 1.0E+2L) * Discount)));
        DistantTradeAdviceCost = std::max<std::int64_t>(static_cast<std::int64_t>(5), static_cast<std::int64_t>(DistantTradeAdviceCost - System::Round(pas::real_divide(DistantTradeAdviceCost, 1.0E+2L) * Discount)));
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<NeaMoney>"_w, pas::wide_int_to_str(NearbyTradeAdviceCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<FarMoney>"_w, pas::wide_int_to_str(DistantTradeAdviceCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Percent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Discount)), u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->Money >= NearbyTradeAdviceCost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::BuyBusinessCenterTradeAdvice>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(NearbyTradeAdviceCost);
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.BK.Trade.PlayerNea"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<NeaMoney>"_w, std::move(intToStr));
            }())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_5), 1, cpp_arg_4);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(NearbyTradeAdviceCost);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.BK.Trade.PlayerNea"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<NeaMoney>"_w, std::move(intToStr_2));
            }())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_6), 0, scriptDialogBlockCallback);
        }
        if (aPlayer::GetPlayer()->Money >= DistantTradeAdviceCost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::BuyBusinessCenterTradeAdvice>(this);
            pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr_3 = pas::wide_int_to_str(DistantTradeAdviceCost);
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRuins.BK.Trade.PlayerFar"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<FarMoney>"_w, std::move(intToStr_3));
            }())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_8), 2, cpp_arg_7);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_2 = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_9 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr_4 = pas::wide_int_to_str(DistantTradeAdviceCost);
                pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormRuins.BK.Trade.PlayerFar"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<FarMoney>"_w, std::move(intToStr_4));
            }())});
            TfRuinsTalk* self_4 = this;
            self_4->AddChoice(std::move(cpp_arg_9), 0, scriptDialogBlockCallback_2);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_10 = pas::bind_method<&TfRuinsTalk::DeclineBusinessCenterTradeAdvice>(this);
            pas::WideString cpp_arg_11 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.BK.Trade.PlayerNo"_wref.get())});
            TfRuinsTalk* self_5 = this;
            self_5->AddChoice(std::move(cpp_arg_11), 0, cpp_arg_10);
        }
    }

    void TfRuinsTalk::BuyBusinessCenterTradeAdvice(std::int32_t Action) {
        pas::WideString Text{};
        std::int32_t Count{};
        pas::WideString Paths{};
        std::uint8_t Nearby{};
        std::uint8_t Found{};
        pas::WideString Dialog{};
        pas::WideString Greeting{};
        pas::WideString Panel{};
        auto FindBusinessCenterTradeRoutes = [&]() -> void {
            std::int32_t I{};
            std::int32_t Attempts{};
            std::int32_t J{};
            aGalaxyStruct::TItemTypeMask GoodsMask{};
            pas::Array<TRoute, 1, 3> Routes{};
            GoodsMask = pas::constant_set<aGalaxyStruct::TItemTypeMask>({{0, 7}});
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 3); cpp_range.next(I); ) {
                Routes[I].FromPlanet = nullptr;
                Routes[I].ToPlanet = nullptr;
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, 3); cpp_range_2.next(I); ) {
                Attempts = 0;
                do {
                    ++Attempts;
                    if (Attempts > 100) {
                        if (I == 1) {
                            Count = 0;
                            Paths = pas::WideString();
                        }
                        return;
                    }
                    {
                        pas::Var<aPlanet::TPlanet*> fromPlanet = pas::Var<aPlanet::TPlanet*>(&Routes[I].FromPlanet);
                        pas::Var<aPlanet::TPlanet*> toPlanet = pas::Var<aPlanet::TPlanet*>(&Routes[I].ToPlanet);
                        std::uint8_t& goodsIndex = Routes[I].GoodsIndex;
                        aGalaxyStruct::TItemTypeMask goodsMask = GoodsMask;
                        std::uint32_t cpp_arg = aMyFunction::AdvanceRandomSeed(aGalaxy::Galaxy->RandomState) + static_cast<std::uint8_t>(Nearby);
                        aPlayer::TPlayer* player = aPlayer::GetPlayer();
                        Found = player->FindProfitableTradeRoute(Nearby, cpp_arg, fromPlanet, toPlanet, goodsIndex, goodsMask);
                    }
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, I - 1); cpp_range_3.next(J); ) {
                        if (Routes[I].GoodsIndex == Routes[J].GoodsIndex) {
                            Found = false;
                        }
                    }
                } while (!Found);
                pas::exclude_at(&GoodsMask, Routes[I].GoodsIndex);
                ++Count;
                Text = aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKFindTradePath"_wref.get());
                aMyFunction::ReplaceTextToken(Text, u"<Num>"_w, pas::wide_int_to_str(I), pas::WideString());
                aMyFunction::ReplaceTextToken(Text, u"<Goods>"_w, aConst::GoodsMarket[Routes[I].GoodsIndex].DisplayName, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<FromPlanet>"_w, Routes[I].FromPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<FromStar>"_w, Routes[I].FromPlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Buy>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Routes[I].GoodsIndex, Routes[I].FromPlanet)), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Cnt>"_w, pas::wide_int_to_str(Routes[I].FromPlanet->Goods[Routes[I].GoodsIndex].Count), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<ToPlanet>"_w, Routes[I].ToPlanet->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<ToStar>"_w, Routes[I].ToPlanet->CurrentStar->Name, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Sale>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Routes[I].GoodsIndex, Routes[I].ToPlanet)), u"<color=255,240,100>"_w);
                if (Paths == u"") {
                    Paths = Text;
                } else {
                    Paths = pas::concat_wide({Paths, u"\r\n", Text});
                }
            }
        };
        Paths = pas::WideString();
        Count = 0;
        if (Action == 1) {
            Nearby = true;
        } else {
            Nearby = false;
        }
        if (Nearby) {
            Greeting = aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKAfterOkNea"_wref.get());
        } else {
            Greeting = aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKAfterOkFar"_wref.get());
        }
        if (Nearby) {
            Panel = aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKAfterOkNeaPanel"_wref.get());
        } else {
            Panel = aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKAfterOkFarPanel"_wref.get());
        }
        aMyFunction::ReplaceTextToken(Panel, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn), u"<color=0,255,0>"_w);
        aMyFunction::ReplaceTextToken(Panel, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=0,255,0>"_w);
        FindBusinessCenterTradeRoutes();
        if (Count == 0) {
            DialogText = pas::concat_wide({Greeting, u"\r\n", aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKNotVariant"_wref.get())});
        } else {
            Dialog = pas::concat_wide({Greeting, u"\r\n", aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKAfterOk"_wref.get())});
            aMyFunction::ReplaceTextToken(Dialog, u"<BKFindTradePath>"_w, Paths, pas::WideString());
            aMyFunction::ReplaceTextToken(Dialog, u"<Count>"_w, pas::wide_int_to_str(Count), u"<color=255,240,100>"_w);
            DialogText = Dialog;
            if (Action == 1) {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - NearbyTradeAdviceCost);
            } else {
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - DistantTradeAdviceCost);
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            Globals::AddOrUpdatePlayerBubble(7, aGalaxy::Galaxy->CurrentTurn, pas::concat_wide({Panel, u"\r\n", Paths}), u""_wref.get());
            MainPanel->RebuildMessageButtons(false);
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::DeclineBusinessCenterTradeAdvice(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.BK.Trade.BKAfterNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BK>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowMedicalCenterIllnessTreatmentDialog(std::int32_t Refresh) {
        std::int32_t I{};
        std::int32_t Cost{};
        std::int32_t TotalCost{};
        std::int32_t AllCost{};
        pas::WideString Text{};
        pas::WideString IllnessText{};
        pas::WideString Key{};
        if (Refresh == 0) {
            if (aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSee"_wref.get());
            } else {
                DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeePirate"_wref.get());
            }
        }
        std::uint8_t HasDisease = aPlayer::GetPlayer()->HasPresentDisease();
        if (HasDisease || aPlayer::GetPlayer()->HasRadiationSickness()) {
            if (Refresh == 0) {
                if (HasDisease) {
                    if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                        DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeIllness"_wref.get())});
                    } else {
                        DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeIllnessPirate"_wref.get())});
                    }
                } else {
                    DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeCureless"_wref.get())});
                }
            }
            aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            TotalCost = 0;
            IllnessText = pas::WideString();
            if (HasDisease) {
                for (I = 1; I <= 12; ++I) {
                    Text = pas::WideString();
                    if (aPlayer::GetPlayer()->CaptainHealth[I].Progress != 0.0L) {
                        Text = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeIll"_wref.get());
                        aMyFunction::ReplaceTextToken(Text, u"<IllName>"_w, aConst::CaptainHealthDefinitions[I].Name, u"<color=255,240,100>"_w);
                        if (aPlayer::GetPlayer()->CaptainHealth[I].Progress >= 1.0E+2L) {
                            aMyFunction::ReplaceTextToken(Text, u"<MCSeeIllType>"_w, aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeIllType1"_wref.get()), pas::WideString());
                        } else {
                            aMyFunction::ReplaceTextToken(Text, u"<MCSeeIllType>"_w, aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeIllType2"_wref.get()), pas::WideString());
                            ++aPlayer::GetPlayer()->CaptainHealth[I].ApplicationCount;
                            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckAllDiseasesAchievement();
                        }
                        aMyFunction::ReplaceTextToken(Text, u"<Date>"_w, aGalaxy::Galaxy->FormatTurnDate(aPlayer::GetPlayer()->CaptainHealth[I].AppliedTurn), u"<color=255,240,100>"_w);
                        aMyFunction::ReplaceTextToken(Text, u"<InfectionObjectName>"_w, aPlayer::GetPlayer()->StatusEffectSourceNames[I], u"<color=255,240,100>"_w);
                        Cost = ([&] {
                            std::int32_t computeScaledAverageMoney = aGalaxy::Galaxy->ComputeScaledAverageMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                            std::int32_t computeScaledMiniMoney = aGalaxy::Galaxy->ComputeScaledMiniMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                            return aConst::GenerateValueForSizeLevel(aConst::CaptainHealthDefinitions[I].MedicalPriceSizeLevel, computeScaledMiniMoney, computeScaledAverageMoney, 50, aGalaxy::Galaxy->CurrentTurn / 10 * aGalaxy::Galaxy->GenerationSeed * I);
                        }());
                        TotalCost += Cost;
                        aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
                        if (IllnessText == u"") {
                            IllnessText = pas::concat_wide({IllnessText, Text});
                        } else {
                            IllnessText = pas::concat_wide({IllnessText, u"\r\n", Text});
                        }
                    }
                }
            }
            if (aPlayer::GetPlayer()->HasRadiationSickness()) {
                Text = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeRadiation"_wref.get());
                if (IllnessText == u"") {
                    IllnessText = pas::concat_wide({IllnessText, Text});
                } else {
                    IllnessText = pas::concat_wide({IllnessText, u"\r\n", Text});
                }
            }
            if (Refresh == 0) {
                aMyFunction::ReplaceTextToken(DialogText, u"<MCSeeIll>"_w, IllnessText, pas::WideString());
            }
            ClearChoices();
            if (HasDisease) {
                for (I = 1; I <= 12; ++I) {
                    if (aPlayer::GetPlayer()->CaptainHealth[I].Progress != 0.0L) {
                        Cost = ([&] {
                            std::int32_t computeScaledAverageMoney_2 = aGalaxy::Galaxy->ComputeScaledAverageMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                            std::int32_t computeScaledMiniMoney_2 = aGalaxy::Galaxy->ComputeScaledMiniMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                            return aConst::GenerateValueForSizeLevel(aConst::CaptainHealthDefinitions[I].MedicalPriceSizeLevel, computeScaledMiniMoney_2, computeScaledAverageMoney_2, 50, aGalaxy::Galaxy->CurrentTurn / 10 * aGalaxy::Galaxy->GenerationSeed * I);
                        }());
                        if (aPlayer::GetPlayer()->MedicalPolicyTicks > 0 && aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                            Cost = Cost / 2;
                        }
                        if (aPlayer::GetPlayer()->Money >= Cost) {
                            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::TreatSelectedDiseaseAtMedicalCenter>(this);
                            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", ([&] {
                                auto name = pas::borrow(aConst::CaptainHealthDefinitions[I].Name);
                                pas::WideString intToStr = pas::wide_int_to_str(Cost);
                                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerIll"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<IllName>"_w, name.get(), u"<Money>"_w, std::move(intToStr));
                            }())});
                            TfRuinsTalk* self = this;
                            self->AddChoice(std::move(cpp_arg_2), I, cpp_arg);
                        } else {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", ([&] {
                                auto name_2 = pas::borrow(aConst::CaptainHealthDefinitions[I].Name);
                                pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerIll"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<IllName>"_w, name_2.get(), u"<Money>"_w, std::move(intToStr_2));
                            }())});
                            TfRuinsTalk* self_2 = this;
                            self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
                        }
                    }
                }
                AllCost = TotalCost / 3 + aGalaxy::Galaxy->ComputeScaledSmallMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                if (aPlayer::GetPlayer()->Money >= AllCost) {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::TreatAllDiseasesAtMedicalCenter>(this);
                    pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", ([&] {
                        pas::WideString intToStr_3 = pas::wide_int_to_str(AllCost);
                        pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerIllAll"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_3));
                    }())});
                    TfRuinsTalk* self_3 = this;
                    self_3->AddChoice(std::move(cpp_arg_5), AllCost, cpp_arg_4);
                } else {
                    GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_2 = fTalk::ScriptDialogBlockCallback;
                    pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", ([&] {
                        pas::WideString intToStr_4 = pas::wide_int_to_str(AllCost);
                        pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerIllAll"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedColorText_4), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_4));
                    }())});
                    TfRuinsTalk* self_4 = this;
                    self_4->AddChoice(std::move(cpp_arg_6), 0, scriptDialogBlockCallback_2);
                }
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::DeclineMedicalCenterTreatment>(this);
                    pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerNo"_wref.get())});
                    TfRuinsTalk* self_5 = this;
                    self_5->AddChoice(std::move(cpp_arg_8), 0, cpp_arg_7);
                }
            } else if (aPlayer::GetPlayer()->OwnerId != aGalaxyStruct::oiPirate) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_9 = pas::bind_method<&TfRuinsTalk::LeaveMedicalCenterTreatment>(this);
                pas::WideString cpp_arg_10 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerExit"_wref.get())});
                TfRuinsTalk* self_6 = this;
                self_6->AddChoice(std::move(cpp_arg_10), 0, cpp_arg_9);
            } else {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_11 = pas::bind_method<&TfRuinsTalk::LeaveMedicalCenterTreatment>(this);
                pas::WideString cpp_arg_12 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.PlayerExitPirate"_wref.get())});
                TfRuinsTalk* self_7 = this;
                self_7->AddChoice(std::move(cpp_arg_12), 0, cpp_arg_11);
            }
        } else {
            if (Refresh == 0) {
                Key = u"FormRuins.MC.Illnes.MCSeeGood"_w;
                if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                    Key = pas::concat_wide({Key, u"PirateTo"});
                } else {
                    Key = pas::concat_wide({Key, u"NormalTo"});
                }
                if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate) {
                    Key = pas::concat_wide({Key, u"Pirate"});
                } else {
                    Key = pas::concat_wide({Key, u"Normal"});
                }
                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(Key)});
            }
            aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
            M_Main(true);
        }
    }

    // Treats disease indexes 1..12; valid insurance halves the fee outside pirate-owned systems.
    void TfRuinsTalk::TreatSelectedDiseaseAtMedicalCenter(std::int32_t DiseaseIndex) {
        std::int32_t I{};
        std::int32_t Cost{};
        pas::WideString Name{};
        for (I = 1; I <= 12; ++I) {
            if (I == DiseaseIndex) {
                if (I == 3) {
                    aGalaxy::Galaxy->GraphDominatorSurfacesEnabled = true;
                    aGalaxy::Galaxy->DisableDominatorSurfaces();
                }
                aPlayer::GetPlayer()->CaptainHealth[I].Progress = 0.0;
                aPlayer::GetPlayer()->StatusEffectSourceNames[I] = pas::WideString();
                Name = aConst::CaptainHealthDefinitions[I].Name;
                Cost = ([&] {
                    std::int32_t computeScaledAverageMoney = aGalaxy::Galaxy->ComputeScaledAverageMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                    std::int32_t computeScaledMiniMoney = aGalaxy::Galaxy->ComputeScaledMiniMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                    return aConst::GenerateValueForSizeLevel(aConst::CaptainHealthDefinitions[I].MedicalPriceSizeLevel, computeScaledMiniMoney, computeScaledAverageMoney, 50, aGalaxy::Galaxy->CurrentTurn / 10 * aGalaxy::Galaxy->GenerationSeed * I);
                }());
                if (aPlayer::GetPlayer()->MedicalPolicyTicks > 0 && aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                    Cost = Cost / 2;
                }
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
                aPlayer::GetPlayer()->DiseaseImmunity = std::min<std::int32_t>(100, aPlayer::GetPlayer()->DiseaseImmunity + 40);
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                break;
            }
        }
        if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeAfterIll"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeAfterIllPirate"_wref.get());
        }
        if (aPlayer::GetPlayer()->HasRadiationSickness() && static_cast<std::uint8_t>(aPlayer::GetPlayer()->HasPresentDisease() ^ 1)) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeAfterIllRadiation"_wref.get())});
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<IllName>"_w, Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        ClearChoices();
        ShowMedicalCenterIllnessTreatmentDialog(1);
    }

    // Trusts QuotedCost from the menu.
    void TfRuinsTalk::TreatAllDiseasesAtMedicalCenter(std::int32_t QuotedCost) {
        std::int32_t I{};
        pas::AnsiString YearText{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeAfterIllAll"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        System::TDateTime Date = SysUtilsImports::Now();
        SysUtilsImports::DateTimeToString(YearText, "yyyy"_a, Date);
        aMyFunction::ReplaceTextToken(DialogText, u"<CurrentYear>"_w, static_cast<pas::WideString>(YearText), u"<color=255,240,100>"_w);
        for (I = 1; I <= 12; ++I) {
            if (aPlayer::GetPlayer()->CaptainHealth[I].Progress != 0.0L) {
                if (I == 3) {
                    aGalaxy::Galaxy->GraphDominatorSurfacesEnabled = true;
                    aGalaxy::Galaxy->DisableDominatorSurfaces();
                }
                aPlayer::GetPlayer()->CaptainHealth[I].Progress = 0.0;
                aPlayer::GetPlayer()->StatusEffectSourceNames[I] = pas::WideString();
            }
        }
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - QuotedCost);
        aPlayer::GetPlayer()->DiseaseImmunity = std::min<std::int32_t>(100, aPlayer::GetPlayer()->DiseaseImmunity + 80);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::DeclineMedicalCenterTreatment(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeAfterNo"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::LeaveMedicalCenterTreatment(std::int32_t Action) {
        if (aPlayer::GetPlayer()->OwnerId == aGalaxyStruct::oiPirate && aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeAfterExitPirate"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Illnes.MCSeeAfterExit"_wref.get());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowMedicalCenterStimulantDialog(std::int32_t Action) {
        std::int32_t I{};
        std::int32_t Cost{};
        std::int32_t Duration{};
        std::uint32_t Seed{};
        pas::WideString Text{};
        pas::WideString StimulantText{};
        pas::WideString Key{};
        pas::Set<8, 39> Offers{};
        std::uint8_t Rank{};
        Offers = pas::constant_set<pas::Set<8, 39>>({});
        std::int32_t OfferCount = 0;
        Seed = aGalaxy::Galaxy->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 13);
        if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            Rank = aPlayer::GetPlayer()->PirateRank;
        } else {
            Rank = aPlayer::GetPlayer()->Rank;
        }
        while (true) {
            aMyFunction::AdvanceRandomSeed(Seed);
            I = aMyFunction::SeededRandomIntRange(13, 24, Seed);
            if (static_cast<std::uint8_t>(pas::contains(Offers, I) ^ 1) && ([&] {
                pas::Extended cpp_left = aMyFunction::SeededRandomUnitFloat(Seed);
                return cpp_left <= aConst::CaptainHealthDefinitions[I].InfectionChance;
            }())) {
                pas::include_at(&Offers, I);
                ++OfferCount;
                if (OfferCount > std::max<std::int32_t>(2, pas::shr(static_cast<std::int32_t>(Rank), 1) + 1)) {
                    break;
                }
            }
        }
        std::int32_t cpp_left_2 = aPlayer::GetPlayer()->GetTotalStatBonus(aConst::bonStimCapacity);
        std::int32_t Bonus = cpp_left_2 + aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtBio);
        std::int32_t countActiveStimulants = aPlayer::GetPlayer()->CountActiveStimulants();
        std::int32_t cpp_arg = MathImports::Floor(static_cast<long double>(aMyFunction::SeededRandomFloatRange(aGalaxy::Galaxy->CurrentTurn / 70 * aPlayer::GetPlayer()->DockedTo->Id, 0.0, 1.0)) * (std::max<std::int32_t>(2, std::max<std::int32_t>(2, static_cast<std::int32_t>(Rank)) + Bonus) - 1)) + 2;
        std::int32_t MaxStimulants = std::max<std::int32_t>(countActiveStimulants, cpp_arg);
        std::int32_t LawStimulants = std::max<std::int32_t>(2, static_cast<std::int32_t>(Rank));
        DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MC1"_wref.get());
        if (MaxStimulants < LawStimulants) {
            DialogText = pas::concat_wide({DialogText, u".", u"\r\n"});
            Key = u"FormRuins.MC.Stimulants.MC3"_w;
        } else {
            DialogText = pas::concat_wide({DialogText, u" "});
            Key = u"FormRuins.MC.Stimulants.MC2"_w;
        }
        if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            Key = pas::concat_wide({Key, u"Pirate"});
            if (!aPlayer::GetPlayer()->PirateClanReal) {
                Key = pas::concat_wide({Key, u"NoRank"});
            }
        }
        DialogText = pas::concat_wide_reverse({aConst::LocalizedColorText(Key), DialogText});
        DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MC4"_wref.get())});
        aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<MaxStim>"_w, pas::wide_int_to_str(MaxStimulants), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<CurStim>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->CountActiveStimulants()), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<AddStim>"_w, pas::wide_int_to_str(MaxStimulants - aPlayer::GetPlayer()->CountActiveStimulants()), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<LawStim>"_w, pas::wide_int_to_str(LawStimulants), u"<color=255,240,100>"_w);
        {
            pas::WideString rankName = aPlayer::GetPlayer()->GetRankName();
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<Rank>"_w, std::move(rankName), u"<color=255,240,100>"_w);
        }
        {
            pas::WideString pirateRankName = aPlayer::GetPlayer()->GetPirateRankName();
            pas::WideString& dialogText_2 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_2, u"<PirateRank>"_w, std::move(pirateRankName), u"<color=255,240,100>"_w);
        }
        if (aPlayer::GetPlayer()->CountActiveStimulants() < MaxStimulants) {
            StimulantText = pas::WideString();
            for (I = 13; I <= 24; ++I) {
                if (pas::contains(Offers, I)) {
                    Text = pas::WideString();
                    Text = aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MCStimInfo"_wref.get());
                    aMyFunction::ReplaceTextToken(Text, u"<StimName>"_w, aConst::CaptainHealthDefinitions[I].Name, u"<color=255,240,100>"_w);
                    aMyFunction::ReplaceTextToken(Text, u"<StimText>"_w, aConst::CaptainHealthDefinitions[I].Text, pas::WideString());
                    Duration = ([&] {
                        std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(aConst::CaptainHealthDefinitions[I].Duration / 10, aConst::CaptainHealthDefinitions[I].Duration / 3, aPlayer::GetPlayer()->DockedTo->Id + I + aGalaxy::Galaxy->CurrentTurn / 13);
                        return aConst::CaptainHealthDefinitions[I].Duration + cpp_right;
                    }());
                    Duration = System::Round(pas::real_divide(Duration, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor));
                    aMyFunction::ReplaceTextToken(Text, u"<Month>"_w, pas::wide_int_to_str(Duration / 30), u"<color=255,240,100>"_w);
                    Cost = ([&] {
                        std::int32_t cpp_arg_2 = 2 * aGalaxy::Galaxy->ComputeScaledAverageMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                        std::int32_t computeScaledSmallMoney = aGalaxy::Galaxy->ComputeScaledSmallMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                        return aConst::GenerateValueForSizeLevel(aConst::CaptainHealthDefinitions[I].MedicalPriceSizeLevel, computeScaledSmallMoney, cpp_arg_2, 50, aGalaxy::Galaxy->CurrentTurn / 13 * aGalaxy::Galaxy->GenerationSeed * I);
                    }());
                    if (aPlayer::GetPlayer()->MedicalPolicyTicks > 0 && aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                        Cost = Cost / 2;
                    }
                    aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
                    if (StimulantText == u"") {
                        StimulantText = pas::concat_wide({StimulantText, Text});
                    } else {
                        StimulantText = pas::concat_wide({StimulantText, u"\r\n", Text});
                    }
                }
            }
            if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MC5MedPolicy"_wref.get())});
            }
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MC5"_wref.get())});
            aMyFunction::ReplaceTextToken(DialogText, u"<MCStimInfo>"_w, StimulantText, pas::WideString());
            ClearChoices();
            for (I = 13; I <= 24; ++I) {
                if (pas::contains(Offers, I)) {
                    Cost = ([&] {
                        std::int32_t cpp_arg_3 = 2 * aGalaxy::Galaxy->ComputeScaledAverageMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                        std::int32_t computeScaledSmallMoney_2 = aGalaxy::Galaxy->ComputeScaledSmallMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                        return aConst::GenerateValueForSizeLevel(aConst::CaptainHealthDefinitions[I].MedicalPriceSizeLevel, computeScaledSmallMoney_2, cpp_arg_3, 50, aGalaxy::Galaxy->CurrentTurn / 13 * aGalaxy::Galaxy->GenerationSeed * I);
                    }());
                    if (aPlayer::GetPlayer()->MedicalPolicyTicks > 0 && aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                        Cost = Cost / 2;
                    }
                    if (aPlayer::GetPlayer()->Money < Cost || aPlayer::GetPlayer()->CaptainHealth[I].Progress == 1.0E+2L) {
                        GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                        pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", ([&] {
                            auto name = pas::borrow(aConst::CaptainHealthDefinitions[I].Name);
                            pas::WideString intToStr = pas::wide_int_to_str(Cost);
                            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.PlayerStim"_wref.get());
                            return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<StimName>"_w, name.get(), u"<Money>"_w, std::move(intToStr));
                        }())});
                        TfRuinsTalk* self = this;
                        self->AddChoice(std::move(cpp_arg_4), 0, scriptDialogBlockCallback);
                    } else {
                        GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::BuySelectedStimulantAtMedicalCenter>(this);
                        pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", ([&] {
                            auto name_2 = pas::borrow(aConst::CaptainHealthDefinitions[I].Name);
                            pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                            pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.PlayerStim"_wref.get());
                            return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<StimName>"_w, name_2.get(), u"<Money>"_w, std::move(intToStr_2));
                        }())});
                        TfRuinsTalk* self_2 = this;
                        self_2->AddChoice(std::move(cpp_arg_6), I, cpp_arg_5);
                    }
                }
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::DeclineMedicalCenterStimulants>(this);
                pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.PlayerNo"_wref.get())});
                TfRuinsTalk* self_3 = this;
                self_3->AddChoice(std::move(cpp_arg_8), 0, cpp_arg_7);
            }
        } else {
            ClearChoices();
            M_Main(true);
        }
    }

    // Stimulants use effect indices 13..24; valid insurance halves the fee outside pirate-owned systems.
    void TfRuinsTalk::BuySelectedStimulantAtMedicalCenter(std::int32_t StimulantIndex) {
        std::int32_t I{};
        std::int32_t Cost{};
        std::int32_t Duration{};
        pas::WideString Name{};
        for (I = 13; I <= 24; ++I) {
            if (I == StimulantIndex) {
                aPlayer::GetPlayer()->CaptainHealth[I].Progress = 1.0E+2;
                aPlayer::GetPlayer()->StatusEffectSourceNames[I] = pas::WideString();
                Name = aConst::CaptainHealthDefinitions[I].Name;
                Duration = ([&] {
                    std::int32_t cpp_right = aMyFunction::SeededRandomIntRange(aConst::CaptainHealthDefinitions[I].Duration / 10, aConst::CaptainHealthDefinitions[I].Duration / 3, aPlayer::GetPlayer()->DockedTo->Id + I + aGalaxy::Galaxy->CurrentTurn / 13);
                    return aConst::CaptainHealthDefinitions[I].Duration + cpp_right;
                }());
                Duration = System::Round(pas::real_divide(Duration, aConst::GalaxyDifficultyTuning[aGalaxy::Galaxy->DifficultyLevels[7]].GoodsEventDurationFactor));
                aPlayer::GetPlayer()->CaptainHealth[I].AppliedTurn = aGalaxy::Galaxy->CurrentTurn;
                aPlayer::GetPlayer()->CaptainHealth[I].ExpireTurn = Duration + aGalaxy::Galaxy->CurrentTurn;
                Cost = ([&] {
                    std::int32_t cpp_arg = 2 * aGalaxy::Galaxy->ComputeScaledAverageMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                    std::int32_t computeScaledSmallMoney = aGalaxy::Galaxy->ComputeScaledSmallMoney(aPlayer::GetPlayer()->DockedTo->OwnerId);
                    return aConst::GenerateValueForSizeLevel(aConst::CaptainHealthDefinitions[I].MedicalPriceSizeLevel, computeScaledSmallMoney, cpp_arg, 50, aGalaxy::Galaxy->CurrentTurn / 13 * aGalaxy::Galaxy->GenerationSeed * I);
                }());
                if (aPlayer::GetPlayer()->MedicalPolicyTicks > 0 && aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                    Cost = Cost / 2;
                }
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
                aPlayer::GetPlayer()->DiseaseImmunity = std::max<std::int32_t>(0, aPlayer::GetPlayer()->DiseaseImmunity - 20);
                ++aPlayer::GetPlayer()->StimulantPurchaseCount;
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MCAfterPlayerStim"_wref.get());
                aMyFunction::ReplaceTextToken(DialogText, u"<StimName>"_w, Name, u"<color=255,240,100>"_w);
                {
                    pas::WideString formatTurnDate = aGalaxy::Galaxy->FormatTurnDate(aPlayer::GetPlayer()->CaptainHealth[I].ExpireTurn);
                    pas::WideString& dialogText = DialogText;
                    aMyFunction::ReplaceTextToken(dialogText, u"<Date>"_w, std::move(formatTurnDate), u"<color=255,240,100>"_w);
                }
                aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
                ++aPlayer::GetPlayer()->CaptainHealth[I].ApplicationCount;
                static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckAllDrugsAchievement();
                M_Main(true);
                break;
            }
        }
    }

    void TfRuinsTalk::DeclineMedicalCenterStimulants(std::int32_t Action) {
        if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MCAfterPlayerNoPirate"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.MC.Stimulants.MCAfterPlayerNo"_wref.get());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<MC>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowStationSpecialShipDialog(std::int32_t Action) {
        std::int32_t I{};
        std::int32_t CompletedQuests{};
        aRanger::PPlayerOldQuest Quest{};
        std::uint8_t CanBuy = true;
        aItem::THull* Hull = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
        if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
            Hull->Init(1000, 8, aPlayer::GetPlayer()->DockedTo->OwnerId, 9, -1, false);
            aItem::ApplySpecialMicroModule(aConst::FindMicroModuleTemplateByCustomTag(u"SuperHullPB"sv), Hull);
        } else if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase)) {
            Hull->Init(1000, 8, aPlayer::GetPlayer()->DockedTo->OwnerId, 9, -1, false);
            aItem::ApplySpecialMicroModule(aConst::FindMicroModuleTemplateByCustomTag(u"SuperHullWB"sv), Hull);
        } else if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstScienceBase)) {
            Hull->Init(1000, 8, aPlayer::GetPlayer()->DockedTo->OwnerId, 9, -1, false);
            aItem::ApplySpecialMicroModule(aConst::FindMicroModuleTemplateByCustomTag(u"SuperHullSB"sv), Hull);
        } else {
            GR_Main::RaiseWideMessage(u"Ask special ship"_wref.get());
        }
        std::int32_t Price = Hull->GetConditionAdjustedCost();
        pas::free(Hull);
        DialogText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Info"}));
        if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
            aMyFunction::ReplaceTextToken(DialogText, u"<KillCnt>"_w, pas::wide_int_to_str(100), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Price>"_w, pas::wide_int_to_str(Price), u"<color=255,240,100>"_w);
            if (aPlayer::GetPlayer()->CivilianKillCount < 100) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<KillComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<KillComplate>"_w, std::move(localizedColorText), u"<color=255,240,100>"_w);
            }
            if (aPlayer::GetPlayer()->GetDominantCareer() != aGalaxyStruct::rcPirate) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<PirateComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_2 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_2, u"<PirateComplate>"_w, std::move(localizedColorText_2), u"<color=255,240,100>"_w);
            }
            if (aPlayer::GetPlayer()->Rank < 5) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<RankComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_3 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_3, u"<RankComplate>"_w, std::move(localizedColorText_3), u"<color=255,240,100>"_w);
            }
        } else if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase)) {
            aMyFunction::ReplaceTextToken(DialogText, u"<KillCnt>"_w, pas::wide_int_to_str(50), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Price>"_w, pas::wide_int_to_str(Price), u"<color=255,240,100>"_w);
            if (aPlayer::GetPlayer()->PirateKillCount < 50) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<KillComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_4 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_4 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_4, u"<KillComplate>"_w, std::move(localizedColorText_4), u"<color=255,240,100>"_w);
            }
            if (aPlayer::GetPlayer()->GetDominantCareer() != aGalaxyStruct::rcWarrior) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<WarriorComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_5 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_5 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_5, u"<WarriorComplate>"_w, std::move(localizedColorText_5), u"<color=255,240,100>"_w);
            }
            if (aPlayer::GetPlayer()->Rank < 5) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<RankComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_6 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_6 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_6, u"<RankComplate>"_w, std::move(localizedColorText_6), u"<color=255,240,100>"_w);
            }
        } else if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstScienceBase)) {
            aMyFunction::ReplaceTextToken(DialogText, u"<KillCnt>"_w, pas::wide_int_to_str(500), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<Price>"_w, pas::wide_int_to_str(Price), u"<color=255,240,100>"_w);
            if (aPlayer::GetPlayer()->DominatorKillCount < 500) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<KillComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_7 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_7 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_7, u"<KillComplate>"_w, std::move(localizedColorText_7), u"<color=255,240,100>"_w);
            }
            if (aPlayer::GetPlayer()->Rank < 5) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<RankComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_8 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_8 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_8, u"<RankComplate>"_w, std::move(localizedColorText_8), u"<color=255,240,100>"_w);
            }
            CompletedQuests = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aRanger::PlayerOldQuests) - 1); cpp_range.next(I); ) {
                Quest = pas::list_at<aRanger::TPlayerOldQuest>(aRanger::PlayerOldQuests, I);
                if (Quest->Successful) {
                    ++CompletedQuests;
                }
            }
            aMyFunction::ReplaceTextToken(DialogText, u"<QuestCnt>"_w, pas::wide_int_to_str(20), u"<color=255,240,100>"_w);
            if (CompletedQuests < 20) {
                CanBuy = false;
                aMyFunction::ReplaceTextToken(DialogText, u"<QuestComplate>"_w, pas::WideString(), pas::WideString());
            } else {
                pas::WideString localizedColorText_9 = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.Complate"}));
                pas::WideString& dialogText_9 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_9, u"<QuestComplate>"_w, std::move(localizedColorText_9), u"<color=255,240,100>"_w);
            }
        } else {
            GR_Main::RaiseWideMessage(u"Ask special ship 2"_wref.get());
        }
        ClearChoices();
        if (CanBuy && aPlayer::GetPlayer()->Money >= Price) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::BuyStationSpecialShip>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.PlayerBuy"}))});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineStationSpecialShip>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.PlayerNo"}))});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
    }

    void TfRuinsTalk::DeclineStationSpecialShip(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.AfterNo"}));
        M_Main(true);
    }

    void TfRuinsTalk::BuyStationSpecialShip(std::int32_t Action) {
        static const pas::Set<0, 255> RelationShipTypes = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::htRanger}, {aGalaxyStruct::htPirate, aGalaxyStruct::htDiplomat}});
        static const pas::Set<0, 255> PirateOwners = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::oiPeleng}});
        static const pas::Set<0, 255> CoalitionOwners = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::oiMaloc}, {aGalaxyStruct::oiHuman, aGalaxyStruct::oiGaal}});
        pas::checked_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SpecialServiceActive = false;
        aItem::THull* Hull = pas::construct_call<aItem::THull>(aItem::TEquipment_Create);
        if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
            Hull->Init(1000, 8, aPlayer::GetPlayer()->DockedTo->OwnerId, 9, -1, false);
            aItem::ApplySpecialMicroModule(aConst::FindMicroModuleTemplateByCustomTag(u"SuperHullPB"sv), Hull);
        } else if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase)) {
            Hull->Init(1000, 8, aPlayer::GetPlayer()->DockedTo->OwnerId, 9, -1, false);
            aItem::ApplySpecialMicroModule(aConst::FindMicroModuleTemplateByCustomTag(u"SuperHullWB"sv), Hull);
        } else if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstScienceBase)) {
            Hull->Init(1000, 8, aPlayer::GetPlayer()->DockedTo->OwnerId, 9, -1, false);
            aItem::ApplySpecialMicroModule(aConst::FindMicroModuleTemplateByCustomTag(u"SuperHullSB"sv), Hull);
        } else {
            GR_Main::RaiseWideMessage(u"Buy special ship"_wref.get());
        }
        std::int32_t Price = Hull->GetConditionAdjustedCost();
        if (aPlayer::GetPlayer()->Money >= Price) {
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Price);
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                aPlayer::GetPlayer()->AddItemToPlayerStorage(Hull, aPlayer::GetPlayer()->CurrentPlanet, -1);
            } else {
                aPlayer::GetPlayer()->AddItemToPlayerStorage(Hull, aPlayer::GetPlayer()->DockedTo, -1);
            }
            if (aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstPirateBase)) {
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmIncrease, 30, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPeleng}}));
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmIncrease, 30, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), static_cast<aGalaxyStruct::TOwnerMask>(PirateOwners));
                aRanger::TRanger_ChangePlanetRelations(aPlayer::GetPlayer(), nullptr, aRanger::rcmDecreaseWithFloor20, 50, pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiMaloc}, {aGalaxyStruct::oiHuman}, {aGalaxyStruct::oiFeyan}, {aGalaxyStruct::oiGaal}}));
                aPlayer::GetPlayer()->ChangeShipRelations(nullptr, aRanger::rcmDecreaseWithFloor20, 50, static_cast<aConst::THullShipTypeMask>(RelationShipTypes), static_cast<aGalaxyStruct::TOwnerMask>(CoalitionOwners));
            }
        } else {
            pas::free(Hull);
        }
        DialogText = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.", aPlayer::GetPlayer()->DockedTo->GetTypeNameKey(), u".SpecialShip.AfterBuy"}));
        M_Main(true);
    }

    void TfRuinsTalk::RunInjectedDialog(std::int32_t Action) {
        pas::WideString Text{};
        aScript::PScriptDialogInjection Injection = reinterpret_cast<aScript::PScriptDialogInjection>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        if (Injection->ActionCode != u"") {
            aScript::CurrentScript = Injection->ActionScript;
            aScript::ExecuteScriptText(Injection->ActionCode, aScript::CurrentScript->InitCode->LocalVar);
        }
        Text = Injection->Answer;
        std::int32_t Parts = EC_Str::CountDelimitedPartsW(pas::view(Text), u"~"sv);
        if (Parts > 1) {
            Text = EC_Str::ExtractDelimitedPartW(pas::view(static_cast<pas::WideString>(SysUtilsImports::LowerCase(static_cast<pas::AnsiString>(Text)))), 0, u"~"sv);
            if (Text == u"snap") {
                RememberChoiceScroll();
            }
        }
        ClearChoices();
        Injection->Script->PublishCurrentShip(aPlayer::GetPlayer()->DockedTo);
        {
            EC_Expression::TVarEC* var = aScript::CurrentScript->InitCode->LocalVar->GetVar(u"GAnswerData"_wref.get());
            std::uint32_t answerData = Injection->AnswerData;
            var->SetDword(answerData);
        }
        Globals::ScriptDialogIndex = -1;
        aScript::CurrentScript->CallDialogByVariable(Injection->DialogName);
        if (Globals::ScriptDialogIndex < 0) {
            M_Main(true);
        } else {
            aScript::CurrentScript->CallDialogMessage(Globals::ScriptDialogIndex);
        }
    }

    void TfRuinsTalk::RunInjectedDialogKeepingScroll(std::int32_t Action) {
        RememberChoiceScroll();
        RunInjectedDialog(Action);
    }

    void TfRuinsTalk::RunScriptRestart(std::int32_t Answer) {
        DialogText = pas::WideString();
        aScript::CurrentScript->ExecuteDialogAnswer(Answer);
        M_Main(false);
    }

    void TfRuinsTalk::AddScriptRestartChoice(pas::WideString Caption) {
        AddChoice(pas::concat_wide({u"- ", Caption}), aScript::CurrentScript->CurrentAnswer, pas::bind_method<&TfRuinsTalk::RunScriptRestart>(this));
    }

    void TfRuinsTalk::CloseRuinsMode(GI_MessageLoop::TObjectGI* Sender) {
        aPlayer::GetPlayer()->CloseRuinsModeScreen();
    }

    std::int32_t TfRuinsTalk::BuildConstructionItemChoices(aConst::TItemType Kind) {
        std::int32_t I{};
        fEquipmentShop::TShopSlot* Slot{};
        aItem::TEquipment* Item{};
        pas::WideString Text{};
        aPlayer::PStorageEntry Entry{};
        auto IsConstructionItemEligible = [&](aItem::TEquipment* Item) -> std::uint8_t {
            std::int32_t I{};
            std::uint8_t Result = false;
            if (Item->ScriptItem != nullptr && reinterpret_cast<aScript::TScriptItem*>(Item->ScriptItem)->Name != u"") {
                return Result;
            }
            if (pas::in_range(Kind, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) && static_cast<std::uint8_t>(pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) ^ 1)) {
                return Result;
            }
            if (static_cast<std::uint8_t>(pas::in_range(Kind, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon)) ^ 1) && Kind != Item->ItemType) {
                return Result;
            }
            if (Kind == aConst::t_Hull) {
                if (static_cast<std::uint8_t>(pas::is_one_of<aGalaxyStruct::htPirate, aGalaxyStruct::htSpecial>(reinterpret_cast<aItem::THull*>(Item)->HullType) ^ 1) || reinterpret_cast<aItem::THull*>(Item)->GetSlotCount(aConst::sskCargoHook) < 1 || reinterpret_cast<aItem::THull*>(Item)->CapitalShip != 0) {
                    return Result;
                }
                if (reinterpret_cast<aItem::THull*>(Item)->HullType == aGalaxyStruct::htSpecial) {
                    if (Item->SpecialModuleIndex == 0) {
                        return Result;
                    }
                    if (!pas::contains(aConst::MicroModuleTemplates[Item->SpecialModuleIndex - 1].OfferStationTypes, aPlayer::GetPlayer()->DockedTo->TypeId)) {
                        return Result;
                    }
                } else if (!pas::contains(aConst::PlanetOwnerMasks.Coalition, Item->OwnerId)) {
                    return Result;
                }
            }
            if (pas::in_range(Kind, static_cast<std::int32_t>(aConst::t_Weapon1), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                for (I = 1; I <= 5; ++I) {
                    if (ConstructionWeapons[I].Item == Item) {
                        return Result;
                    }
                }
            }
            return true;
        };
        auto FormatConstructionItem = [&](pas::WideString& Text, aItem::TEquipment* Item) -> void {
            pas::WideString Stats{};
            aMyFunction::ReplaceTextToken(Text, u"<ItemName>"_w, EC_Str::RemoveTextTagsW(Item->GetDisplayName()), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Size>"_w, pas::wide_int_to_str(Item->Weight), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Cost>"_w, pas::wide_int_to_str(Item->Cost), u"<color=255,240,100>"_w);
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr) {
                Stats = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.StatsWeapon"_wref.get());
            } else {
                Stats = aConst::LocalizedColorText(pas::concat_wide({u"FormRuins.CB.ConstructPirate.Stats", aConst::ItemTypeNames[Item->ItemType]}));
            }
            Item->ReplaceInfoTokens(Stats, u"<color=255,240,100>"_w, nullptr);
            if (aItem::THull* hull = pas::class_cast_if<aItem::THull*>(Item); hull != nullptr && hull->HullSeries != -1) {
                Stats = pas::concat_wide({Stats, u", ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.StatsSeries"_wref.get())});
                aMyFunction::ReplaceTextToken(Stats, u"<SeriesName>"_w, pas::concat_wide({u"\"", aConst::HullSeriesDefinitions[reinterpret_cast<aItem::THull*>(Item)->HullSeries].Name, u"\""}), u"<color=255,240,100>"_w);
            }
            if (pas::class_cast_if<aItem::TWeapon*>(Item) != nullptr && Item->SpecialModuleIndex > 0) {
                Stats = pas::concat_wide({Stats, u", ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.StatsSeries"_wref.get())});
                aMyFunction::ReplaceTextToken(Stats, u"<SeriesName>"_w, Item->GetSpecialModuleName(), u"<color=255,240,100>"_w);
            }
            aMyFunction::ReplaceTextToken(Text, u"<Stats>"_w, Stats, pas::WideString());
            Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
            Text = EC_Str::ReplaceAllWideString(Text, u"<color=0,255,0>"_wref.get(), u"<color=0,130,0>"sv);
        };
        std::int32_t Count = 0;
        if (Kind != aConst::t_Hull) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
                Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
                if (Item->EquippedFlag == 0 && IsConstructionItemEligible(Item)) {
                    Text = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.InHold"_wref.get());
                    FormatConstructionItem(Text, Item);
                    if (Kind != aConst::t_Hull && fRuinsTalk::GetConstructionFreeSpace() - Item->Weight < 0) {
                        AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
                    } else {
                        AddChoice(pas::concat_wide({u"- ", Text}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item)), pas::bind_method<&TfRuinsTalk::SelectConstructionHeldItem>(this));
                    }
                    ++Count;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range_2.next(I); ) {
            Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
            if (aPlayer::GetPlayer()->DockedTo == Entry->LocationOwner) {
                if (Entry != nullptr && Entry->Item != nullptr) {
                    Item = reinterpret_cast<aItem::TEquipment*>(Entry->Item);
                    if (IsConstructionItemEligible(Item)) {
                        Text = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.InStorage"_wref.get());
                        FormatConstructionItem(Text, Item);
                        if (Kind != aConst::t_Hull && fRuinsTalk::GetConstructionFreeSpace() - Item->Weight < 0) {
                            AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", Text}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item)), pas::bind_method<&TfRuinsTalk::SelectConstructionStoredItem>(this));
                        }
                        ++Count;
                    }
                }
            }
        }
        if (fEquipmentShop::TemporaryShopSlots != nullptr) {
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_3.next(I); ) {
                Slot = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, I);
                if (Slot != nullptr) {
                    Item = reinterpret_cast<aItem::TEquipment*>(Slot->Item);
                    if (Item != nullptr && IsConstructionItemEligible(Item)) {
                        Text = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.InShop"_wref.get());
                        FormatConstructionItem(Text, Item);
                        if (Item->Cost + fRuinsTalk::GetConstructionShopCost() > aPlayer::GetPlayer()->Money) {
                            AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
                        } else if (Kind != aConst::t_Hull && fRuinsTalk::GetConstructionFreeSpace() - Item->Weight < 0) {
                            AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", Text}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item)), pas::bind_method<&TfRuinsTalk::SelectConstructionShopItem>(this));
                        }
                        ++Count;
                    }
                }
            }
        }
        if (static_cast<std::uint8_t>(pas::in_set<aConst::t_Hull, aConst::t_Engine, aConst::t_CargoHook, aConst::t_CargoHook>(Kind) ^ 1) && (Kind != aConst::t_Weapon1 || ConstructionWeapons[1].Item != nullptr)) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.skip"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::SkipConstructionItem>(this));
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.cancel"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineDominionShipConstruction>(this));
        return Count;
    }

    void TfRuinsTalk::ShowDominionShipConstructionDialog(std::int32_t Action) {
        std::int32_t J{};
        aConst::TItemType Kind{};
        ClearChoices();
        for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Hull, aConst::t_DefGenerator); cpp_range.next(Kind); ) {
            ConstructionEquipment[Kind].Item = nullptr;
        }
        for (J = 1; J <= 5; ++J) {
            ConstructionWeapons[J].Item = nullptr;
        }
        if (aPlayer::GetPlayer()->GetMaxDominionShips() <= pas::list_count(aPlayer::GetPlayer()->PiratePartners)) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.WarningPirateCnt"_wref.get());
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.WarningConfirm"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ConfirmDominionConstructionLimit>(this));
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.cancel"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineDominionShipConstruction>(this));
        } else {
            BuildConstructionItemChoices(aConst::t_Hull);
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickHull"_wref.get());
        }
    }

    void TfRuinsTalk::ConfirmDominionConstructionLimit(std::int32_t Action) {
        ClearChoices();
        BuildConstructionItemChoices(aConst::t_Hull);
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickHull"_wref.get());
    }

    void TfRuinsTalk::DeclineDominionShipConstruction(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.Cancelled"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::SelectConstructionHeldItem(std::int32_t Action) {
        fRuinsTalk::SelectConstructionItem(reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action))), 0);
        ContinueDominionConstruction(EC_Str::RemoveTextTagsW(reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)))->GetDisplayName()));
    }

    void TfRuinsTalk::SelectConstructionStoredItem(std::int32_t Action) {
        fRuinsTalk::SelectConstructionItem(reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action))), 1);
        ContinueDominionConstruction(EC_Str::RemoveTextTagsW(reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)))->GetDisplayName()));
    }

    void TfRuinsTalk::SelectConstructionShopItem(std::int32_t Action) {
        fRuinsTalk::SelectConstructionItem(reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action))), 2);
        ContinueDominionConstruction(EC_Str::RemoveTextTagsW(reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)))->GetDisplayName()));
    }

    void TfRuinsTalk::SkipConstructionItem(std::int32_t Action) {
        ContinueDominionConstruction(pas::WideString());
    }

    void TfRuinsTalk::AppendConstructionItemList() {
        std::int32_t I{};
        aConst::TItemType Kind{};
        aItem::TEquipment* Item{};
        pas::WideString Text{};
        DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.ItemList"_wref.get())});
        for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Hull, aConst::t_DefGenerator); cpp_range.next(Kind); ) {
            Item = ConstructionEquipment[Kind].Item;
            if (Item != nullptr) {
                Text = pas::concat_wide({Text, Item->GetShortName(), u" - ", EC_Str::RemoveTextTagsW(Item->GetDisplayName()), u"\r\n"});
            } else {
                Text = pas::concat_wide({Text, aConst::LocalizedText(pas::concat_wide({u"Items.", aConst::ItemTypeNames[Kind], u".ShortName"})), u" - "});
                if (reinterpret_cast<aItem::THull*>(ConstructionEquipment[aConst::t_Hull].Item)->GetSlotCount(aConst::ItemTypeToSlotKind(Kind)) > 0) {
                    Text = pas::concat_wide({Text, aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.NotInstalled"_wref.get()), u"\r\n"});
                } else {
                    Text = pas::concat_wide({Text, aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.NotAvailable"_wref.get()), u"\r\n"});
                }
            }
        }
        for (I = 1; I <= 5; ++I) {
            Text = pas::concat_wide({Text, aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.weaponN"_wref.get()), pas::wide_int_to_str(I), u" - "});
            Item = ConstructionWeapons[I].Item;
            if (Item != nullptr) {
                Text = pas::concat_wide({Text, EC_Str::RemoveTextTagsW(Item->GetDisplayName()), u"\r\n"});
            } else if (reinterpret_cast<aItem::THull*>(ConstructionEquipment[aConst::t_Hull].Item)->GetSlotCount(aConst::sskWeapon) >= I) {
                Text = pas::concat_wide({Text, aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.NotInstalled"_wref.get()), u"\r\n"});
            } else {
                Text = pas::concat_wide({Text, aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.NotAvailable"_wref.get()), u"\r\n"});
            }
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<ItemList>"_w, Text, pas::WideString());
        aMyFunction::ReplaceTextToken(DialogText, u"<TotalCost>"_w, pas::wide_int_to_str(fRuinsTalk::GetConstructionShopCost()), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<FreeSpace>"_w, pas::wide_int_to_str(fRuinsTalk::GetConstructionFreeSpace()), u"<color=255,240,100>"_w);
    }

    void TfRuinsTalk::ContinueDominionConstruction(pas::WideString PreviousItem) {
        std::uint8_t CanAdd{};
        aItem::THull* Hull{};
        if (PreviousItem != u"") {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.AddedEq"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.AddedNothing"_wref.get());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<PrevItem>"_w, PreviousItem, u"<color=255,240,100>"_w);
        AppendConstructionItemList();
        ClearChoices();
        if (ConstructionEquipment[aConst::t_Engine].Item == nullptr) {
            BuildConstructionItemChoices(aConst::t_Engine);
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickEngine"_wref.get())});
        } else if (ConstructionEquipment[aConst::t_FuelTanks].Item == nullptr) {
            BuildConstructionItemChoices(aConst::t_FuelTanks);
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickFuelTanks"_wref.get())});
        } else if (ConstructionEquipment[aConst::t_CargoHook].Item == nullptr) {
            BuildConstructionItemChoices(aConst::t_CargoHook);
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickCargoHook"_wref.get())});
        } else if (ConstructionWeapons[1].Item == nullptr) {
            BuildConstructionItemChoices(aConst::t_Weapon1);
            DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickWeapon"_wref.get())});
        } else {
            CanAdd = false;
            Hull = reinterpret_cast<aItem::THull*>(ConstructionEquipment[aConst::t_Hull].Item);
            if (ConstructionWeapons[Hull->GetSlotCount(aConst::sskWeapon)].Item == nullptr) {
                CanAdd = true;
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::PickConstructionWeapon>(this);
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.AddWeapon"_wref.get())});
                    TfRuinsTalk* self = this;
                    self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
                }
            }
            if (Hull->GetSlotCount(aConst::sskRadar) > 0 && ConstructionEquipment[aConst::t_Radar].Item == nullptr) {
                CanAdd = true;
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::PickConstructionRadar>(this);
                    pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.AddRadar"_wref.get())});
                    TfRuinsTalk* self_2 = this;
                    self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
                }
            }
            if (Hull->GetSlotCount(aConst::sskScanner) > 0 && ConstructionEquipment[aConst::t_Scaner].Item == nullptr) {
                CanAdd = true;
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::PickConstructionScanner>(this);
                    pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.AddScaner"_wref.get())});
                    TfRuinsTalk* self_3 = this;
                    self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
                }
            }
            if (Hull->GetSlotCount(aConst::sskRepairRobot) > 0 && ConstructionEquipment[aConst::t_RepairRobot].Item == nullptr) {
                CanAdd = true;
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::PickConstructionRepairRobot>(this);
                    pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.AddRepairRobot"_wref.get())});
                    TfRuinsTalk* self_4 = this;
                    self_4->AddChoice(std::move(cpp_arg_8), 0, cpp_arg_7);
                }
            }
            if (Hull->GetSlotCount(aConst::sskDefGenerator) > 0 && ConstructionEquipment[aConst::t_DefGenerator].Item == nullptr) {
                CanAdd = true;
                {
                    GI_MessageLoop::TDialogChoiceEventGI cpp_arg_9 = pas::bind_method<&TfRuinsTalk::PickConstructionDefGenerator>(this);
                    pas::WideString cpp_arg_10 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.AddDefGenerator"_wref.get())});
                    TfRuinsTalk* self_5 = this;
                    self_5->AddChoice(std::move(cpp_arg_10), 0, cpp_arg_9);
                }
            }
            if (CanAdd) {
                DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.ReadyCanAddMore"_wref.get())});
            } else {
                DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.ReadyCanNotAddMore"_wref.get())});
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_11 = pas::bind_method<&TfRuinsTalk::CompleteDominionConstruction>(this);
                pas::WideString cpp_arg_12 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.confirm"_wref.get())});
                TfRuinsTalk* self_6 = this;
                self_6->AddChoice(std::move(cpp_arg_12), 0, cpp_arg_11);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_13 = pas::bind_method<&TfRuinsTalk::DeclineDominionShipConstruction>(this);
                pas::WideString cpp_arg_14 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.cancel"_wref.get())});
                TfRuinsTalk* self_7 = this;
                self_7->AddChoice(std::move(cpp_arg_14), 0, cpp_arg_13);
            }
        }
    }

    void TfRuinsTalk::PickConstructionWeapon(std::int32_t Action) {
        DialogText = pas::WideString();
        AppendConstructionItemList();
        ClearChoices();
        BuildConstructionItemChoices(aConst::t_Weapon1);
        DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickWeapon"_wref.get())});
    }

    void TfRuinsTalk::PickConstructionRadar(std::int32_t Action) {
        DialogText = pas::WideString();
        AppendConstructionItemList();
        ClearChoices();
        BuildConstructionItemChoices(aConst::t_Radar);
        DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickRadar"_wref.get())});
    }

    void TfRuinsTalk::PickConstructionScanner(std::int32_t Action) {
        DialogText = pas::WideString();
        AppendConstructionItemList();
        ClearChoices();
        BuildConstructionItemChoices(aConst::t_Scaner);
        DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickScaner"_wref.get())});
    }

    void TfRuinsTalk::PickConstructionRepairRobot(std::int32_t Action) {
        DialogText = pas::WideString();
        AppendConstructionItemList();
        ClearChoices();
        BuildConstructionItemChoices(aConst::t_RepairRobot);
        DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickRepairRobot"_wref.get())});
    }

    void TfRuinsTalk::PickConstructionDefGenerator(std::int32_t Action) {
        DialogText = pas::WideString();
        AppendConstructionItemList();
        ClearChoices();
        BuildConstructionItemChoices(aConst::t_DefGenerator);
        DialogText = pas::concat_wide({DialogText, u"\r\n", u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.PickDefGenerator"_wref.get())});
    }

    void TfRuinsTalk::CompleteDominionConstruction(std::int32_t Action) {
        aConst::TItemType Kind{};
        std::int32_t J{};
        std::int32_t Months{};
        aItem::TEquipment* Item{};
        auto RemoveConstructionStoredItem = [&](aItem::TEquipment* Item) -> void {
            std::int32_t I{};
            aPlayer::PStorageEntry Entry{};
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->StorageEntries) - 1); cpp_range.next(I); ) {
                Entry = pas::list_at<aPlayer::TStorageEntry>(aPlayer::GetPlayer()->StorageEntries, I);
                if (Entry != nullptr && Entry->Item == Item) {
                    pas::list_delete(aPlayer::GetPlayer()->StorageEntries, I);
                    aPlayer::GetPlayer()->RefreshStorageBubbles();
                    pas::dispose(Entry);
                    break;
                }
            }
        };
        auto RemoveConstructionShopItem = [&](aItem::TEquipment* Item) -> void {
            aRuins::TRuins* Station{};
            if (fEquipmentShop::TemporaryShopSlots != nullptr) {
                fEquipmentShop::RestoreTemporaryShopStock();
                Station = reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo);
                pas::list_delete(Station->EquipmentShop, pas::list_indexof(Station->EquipmentShop, reinterpret_cast<void*>(Item)));
                fEquipmentShop::BuildTemporaryShopSlotGrid();
                Globals::EquipmentShopScreen->ClearGoodsControls();
                Globals::EquipmentShopScreen->BuildGoodsControls();
                Globals::EquipmentShopScreen->UpdateScrollButtons();
            }
        };
        std::int32_t Price = fRuinsTalk::GetConstructionShopCost();
        if (Price > 0) {
            aPlayer::GetPlayer()->SetMoney(std::max<std::int32_t>(0, aPlayer::GetPlayer()->Money - Price));
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        }
        aPirate::TPirate* Ship = pas::construct_call<aPirate::TPirate>(aNormalShip::TNormalShip_Create);
        aPlanet::TPlanet* Planet = static_cast<aPlanet::TPlanet*>(aPlayer::GetPlayer()->DockedTo->CurrentStar->SelectRandomInhabitedPlanet());
        std::int32_t TotalCost = 0;
        for (auto cpp_range = pas::for_to<aConst::TItemType>(aConst::t_Hull, aConst::t_DefGenerator); cpp_range.next(Kind); ) {
            if (ConstructionEquipment[Kind].Item != nullptr) {
                Item = ConstructionEquipment[Kind].Item;
                if (ConstructionEquipment[Kind].Source == 0) {
                    pas::list_delete(aPlayer::GetPlayer()->Inventory, pas::list_indexof(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item)));
                } else if (ConstructionEquipment[Kind].Source == 1) {
                    RemoveConstructionStoredItem(Item);
                } else {
                    RemoveConstructionShopItem(Item);
                }
                pas::list_add(Ship->Inventory, reinterpret_cast<void*>(Item));
                Ship->EquipItem(Item);
                TotalCost += ConstructionEquipment[Kind].Item->Cost;
            }
        }
        for (J = 1; J <= 5; ++J) {
            if (ConstructionWeapons[J].Item != nullptr) {
                Item = ConstructionWeapons[J].Item;
                if (ConstructionWeapons[J].Source == 0) {
                    pas::list_delete(aPlayer::GetPlayer()->Inventory, pas::list_indexof(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Item)));
                } else if (ConstructionWeapons[J].Source == 1) {
                    RemoveConstructionStoredItem(Item);
                } else {
                    RemoveConstructionShopItem(Item);
                }
                pas::list_add(Ship->Inventory, reinterpret_cast<void*>(Item));
                Ship->EquipItem(Item);
                TotalCost += ConstructionWeapons[J].Item->Cost;
            }
        }
        Ship->InitGenerated(Planet, TotalCost / 10 + 1000, 0);
        Ship->DockedTo = aPlayer::GetPlayer()->DockedTo;
        Ship->TrainSkillsAutomatically();
        Ship->RefreshEquipmentEvaluationMetrics();
        Ship->ChangeRelationToRanger(aPlayer::GetPlayer(), 100);
        if (aPlayer::GetPlayer()->GetMaxDominionShips() > pas::list_count(aPlayer::GetPlayer()->PiratePartners)) {
            Ship->PartnerShip = aPlayer::GetPlayer();
            Months = Ship->CalculatePartnershipMonths(TotalCost, Ship);
            Ship->PartnershipDaysRemaining = 30 * Months;
            pas::list_add(aPlayer::GetPlayer()->PiratePartners, reinterpret_cast<void*>(Ship));
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckMasterAchievement();
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.CompletionText"_wref.get());
        } else {
            Months = 0;
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ConstructPirate.CompletionTextNotPartner"_wref.get());
        }
        {
            pas::WideString fullName = Ship->GetFullName(u" "_wref.get());
            pas::WideString& dialogText = DialogText;
            aMyFunction::ReplaceTextToken(dialogText, u"<FullName>"_w, std::move(fullName), u"<color=255,240,100>"_w);
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<CntMonth>"_w, pas::wide_int_to_str(Months), u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowDominionImprovementDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBAnswer"_wref.get());
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerAsk"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionImprovementItems>(this));
    }

    void TfRuinsTalk::ShowDominionImprovementItems(std::int32_t Action) {
        std::int32_t I{};
        aItem::TEquipment* Item{};
        aItem::TItem* Artefact{};
        pas::WideString Text{};
        ClearChoices();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Inventory) - 1); cpp_range.next(I); ) {
            Item = pas::list_at<aItem::TEquipment>(aPlayer::GetPlayer()->Inventory, I);
            if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
                if (Item->CanImprove() && aShip::TShip_CanUseEquipmentTech(aPlayer::GetPlayer(), Item) && aShip::TShip_CanRepairEquipmentTech(aPlayer::GetPlayer(), Item)) {
                    ++Count;
                    Text = pas::concat_wide({Text, u"\r\n", pas::wide_int_to_str(Count), u") ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.ItemReadyForImprovement"_wref.get())});
                    {
                        GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowDominionImprovementQuote>(this);
                        pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Item->GetDisplayName()))});
                        std::int32_t item = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item));
                        TfRuinsTalk* self = this;
                        self->AddChoice(std::move(cpp_arg_2), item, cpp_arg);
                    }
                }
                aMyFunction::ReplaceTextToken(Text, u"<ItemName>"_w, aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Item->GetDisplayName())), pas::WideString());
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Item->Cost), u"<color=255,240,100>"_w);
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->Artefacts) - 1); cpp_range_2.next(I); ) {
            Artefact = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->Artefacts, I);
            if (aItem::TArtefactTranclucator* artefactTranclucator = pas::class_cast_if<aItem::TArtefactTranclucator*>(Artefact)) {
                Item = static_cast<aShip::TShip*>(artefactTranclucator->Ship)->GetHull();
                if (Item->CanImprove()) {
                    ++Count;
                    Text = pas::concat_wide({Text, u"\r\n", pas::wide_int_to_str(Count), u") ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.ItemReadyForImprovement"_wref.get())});
                    {
                        GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::ShowDominionImprovementQuote>(this);
                        pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Artefact->GetDisplayName()))});
                        std::int32_t item_2 = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Item));
                        TfRuinsTalk* self_2 = this;
                        self_2->AddChoice(std::move(cpp_arg_4), item_2, cpp_arg_3);
                    }
                }
                aMyFunction::ReplaceTextToken(Text, u"<ItemName>"_w, aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Artefact->GetDisplayName())), pas::WideString());
                aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Item->Cost), u"<color=255,240,100>"_w);
            }
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBSeeItems"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<ListItems>"_w, Text, pas::WideString());
        if (Count > 0) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBSeeItemsHaveItems"_wref.get())});
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_5 = pas::bind_method<&TfRuinsTalk::DeclineDominionImprovement>(this);
                pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerNothing"_wref.get())});
                TfRuinsTalk* self_3 = this;
                self_3->AddChoice(std::move(cpp_arg_6), 0, cpp_arg_5);
            }
        } else {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBSeeItemsNotHaveItems"_wref.get())});
            M_Main(true);
        }
    }

    void TfRuinsTalk::DeclineDominionImprovement(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBAnswerNothing"_wref.get());
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowDominionImprovementQuote(std::int32_t Action) {
        aItem::TEquipment* Item{};
        pas::WideString Text{};
        std::uint8_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        Item = reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        std::int32_t NodeCost = 0;
        if (Item->OwnerId == aGalaxyStruct::oiDominator) {
            Text = aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBNeedCostImprovementNodes"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<Nodes>"_w, pas::wide_int64_to_str(System::Round(Item->CalculateImprovementCost(aItem::ikMajor) * 0.01L * 1.5L)), u"<color=255,240,100>"_w);
            NodeCost = System::Round(pas::real_divide(Item->CalculateImprovementCost(aItem::ikMajor) * 0.01L * 1.5L * (100 - Discount), 1.0E+2L));
            aMyFunction::ReplaceTextToken(Text, u"<NodesDiscount>"_w, pas::wide_int_to_str(NodeCost), u"<color=255,240,100>"_w);
        } else {
            Text = aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBNeedCostImprovement"_wref.get());
        }
        aMyFunction::ReplaceTextToken(Text, u"<Money>"_w, pas::wide_int_to_str(Item->CalculateImprovementCost(aItem::ikMajor)), u"<color=255,240,100>"_w);
        std::int32_t MoneyCost = System::Round(pas::real_divide(Item->CalculateImprovementCost(aItem::ikMajor) * (100 - Discount), 1.0E+2L));
        aMyFunction::ReplaceTextToken(Text, u"<MoneyDiscount>"_w, pas::wide_int_to_str(MoneyCost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<Discount>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Discount)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(Text, u"<FullName>"_w, aMyFunction::NormalizeTextHighlightColors(EC_Str::RemoveTextTagsW(Item->GetDisplayName())), u"<color=255,240,100>"_w);
        DialogText = Text;
        ClearChoices();
        if (aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) >= NodeCost && aPlayer::GetPlayer()->Money >= MoneyCost) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerOk"_wref.get())}), Action, pas::bind_method<&TfRuinsTalk::AcceptDominionImprovement>(this));
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerOk"_wref.get())});
            AddChoice(std::move(cpp_arg), 0, scriptDialogBlockCallback);
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineDominionImprovement>(this));
    }

    void TfRuinsTalk::AcceptDominionImprovement(std::int32_t Action) {
        aItem::TEquipment* Item{};
        std::int32_t Nodes{};
        std::uint8_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        Item = reinterpret_cast<aItem::TEquipment*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        std::int32_t Cost = Item->CalculateImprovementCost(aItem::ikMajor);
        if (Item->OwnerId == aGalaxyStruct::oiDominator) {
            Nodes = System::Round(pas::real_divide(Cost * 0.01L * 1.5L * (100 - Discount), 1.0E+2L));
        } else {
            Nodes = 0;
        }
        Cost = System::Round(pas::real_divide(Cost * (100 - Discount), 1.0E+2L));
        if (aPlayer::GetPlayer()->Money >= Cost && aPlayer::GetPlayer()->GetAvailableNodeCount(nullptr) >= Nodes) {
            aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Cost);
            if (Item->OwnerId == aGalaxyStruct::oiDominator) {
                aPlayer::TPlayer_ConsumeAvailableNodes(aPlayer::GetPlayer(), Nodes, nullptr);
            }
            Item->ImproveAtScientificBase();
            aPlayer::GetPlayer()->RefreshDerivedStats(true);
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBAfterOk"_wref.get());
            {
                pas::WideString wideLowerCase = SysUtilsImports::WideLowerCase(Item->GetShortName());
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<ShortName>"_w, std::move(wideLowerCase), pas::WideString());
            }
            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
            ClearChoices();
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::ShowDominionImprovementItems>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerRepeatOk"_wref.get())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
            }
            {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineDominionRepeatImprovement>(this);
                pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.Improvement.PlayerRepeatNo"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
            }
        } else {
            DeclineScienceBaseRepeatImprovement(0);
        }
    }

    void TfRuinsTalk::DeclineDominionRepeatImprovement(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.Improvement.CBAfterRepeatNo"_wref.get());
        ClearChoices();
        M_Main(true);
    }

    void TfRuinsTalk::ShowDominionPirateLicenseDialog(std::int32_t Action) {
        std::uint8_t Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        if (aPlayer::GetPlayer()->PirateLicenseTicks == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.CBAnswer"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.CBAnswerProlongate"_wref.get());
        }
        std::int32_t Cost = aMyFunction::RoundAndTruncateToTens(aMyFunction::RemapClamped(aPlayer::GetPlayer()->PirateLicenseTicks, 0.0, 365.0, aGalaxy::Galaxy->ComputeScaledAverageMoney(aGalaxyStruct::oiHuman), 0.0));
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Discount>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(Discount)), u"<color=255,240,100>"_w);
        Cost = System::Round(pas::real_divide(Cost * (100 - Discount), 1.0E+2L));
        aMyFunction::ReplaceTextToken(DialogText, u"<DiscountMoney>"_w, pas::wide_int_to_str(Cost), u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->PirateLicenseTicks == 0) {
            if (aPlayer::GetPlayer()->Money >= Cost) {
                GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::BuyDominionPirateLicense>(this);
                pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.PlayerOk"_wref.get())});
                TfRuinsTalk* self = this;
                self->AddChoice(std::move(cpp_arg_2), Cost, cpp_arg);
            } else {
                GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.PlayerOk"_wref.get())});
                TfRuinsTalk* self_2 = this;
                self_2->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
            }
        } else if (aPlayer::GetPlayer()->Money >= Cost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::BuyDominionPirateLicense>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.PlayerOkProlongate"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_5), Cost, cpp_arg_4);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback_2 = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_6 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.PlayerOkProlongate"_wref.get())});
            TfRuinsTalk* self_4 = this;
            self_4->AddChoice(std::move(cpp_arg_6), 0, scriptDialogBlockCallback_2);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_7 = pas::bind_method<&TfRuinsTalk::DeclineDominionPirateLicense>(this);
            pas::WideString cpp_arg_8 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.PlayerNo"_wref.get())});
            TfRuinsTalk* self_5 = this;
            self_5->AddChoice(std::move(cpp_arg_8), 0, cpp_arg_7);
        }
    }

    void TfRuinsTalk::BuyDominionPirateLicense(std::int32_t Action) {
        if (aPlayer::GetPlayer()->PirateLicenseTicks == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.CBAfterOk"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.CBAfterOkProlongate"_wref.get());
        }
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - Action);
        aPlayer::GetPlayer()->PirateLicenseTicks = 365;
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::DeclineDominionPirateLicense(std::int32_t Action) {
        if (aPlayer::GetPlayer()->PirateLicenseTicks == 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.CBAfterNo"_wref.get());
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.PirateLicense.CBAfterNoProlongate"_wref.get());
        }
        aMyFunction::ReplaceTextToken(DialogText, u"<Days>"_w, pas::wide_int_to_str(aPlayer::GetPlayer()->PirateLicenseTicks), u"<color=255,240,100>"_w);
        M_Main(true);
    }

    std::uint8_t TfRuinsTalk::CheckDominionServiceStanding(std::uint8_t RequiredRank, pas::WideString Prefix, float CreditCost) {
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer()->PirateRank < RequiredRank) {
            DialogText = pas::concat_wide({Prefix, u" ", aConst::LocalizedColorText(u"FormRuins.CB.GenericRefuseNeedRank"_wref.get())});
            {
                pas::WideString localizedText = aConst::LocalizedText(pas::concat_wide({u"RankPirate.", aConst::PirateRankNames[RequiredRank], u".Name"}));
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<PirateRank>"_w, std::move(localizedText), u"<color=255,240,100>"_w);
            }
            return Result;
        } else if (pas::real_divide(static_cast<long double>(aGalaxy::Galaxy->AverageRangerCapital) * CreditCost, 6.0E+2L) > aPlayer::GetPlayer()->PirateLicenseCash && aPlayer::GetPlayer()->PirateRank < 7) {
            if (aPlayer::GetPlayer()->PirateLicenseTicks > 0) {
                DialogText = pas::concat_wide({Prefix, u" ", aConst::LocalizedColorText(u"FormRuins.CB.GenericRefuseNeedPoints"_wref.get())});
            } else {
                DialogText = pas::concat_wide({Prefix, u" ", aConst::LocalizedColorText(u"FormRuins.CB.GenericRefuseNeedLicense"_wref.get())});
            }
            return Result;
        } else {
            return true;
        }
    }

    void TfRuinsTalk::SpendDominionServiceCredit(float CreditCost) {
        aPlayer::GetPlayer()->PirateLicenseCash = std::max<std::int64_t>(static_cast<std::int64_t>(0), System::Round((aPlayer::GetPlayer()->PirateLicenseCash - pas::real_divide(static_cast<long double>(aGalaxy::Galaxy->AverageRangerCapital) * CreditCost, 6.0E+2L)) * 0.85L));
    }

    std::uint8_t TfRuinsTalk::CheckDominionAvailable() {
        if (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar != nullptr || aPlayer::GetPlayer()->DockedTo->Order == aShip::soTeleport || aPlayer::GetPlayer()->DockedTo->HasScriptControl()) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.GenericRefuseHaveOtherPlans"_wref.get());
            return false;
        }
        return true;
    }

    void TfRuinsTalk::ShowDominionTravelDialog(std::int32_t Action) {
        pas::WideString Text{};
        std::uint32_t Seed{};
        aGalaxy::TStar* Star{};
        std::int32_t I{};
        std::int32_t J{};
        std::uint8_t Duplicate{};
        std::uint8_t Discount{};
        auto GetDominionTravelQuoteCost = [&](std::int32_t Index) -> std::int32_t {
            return std::min<std::int64_t>(static_cast<std::int64_t>(100000000), System::Round(pas::real_divide(pas::real_divide(pas::real_divide(aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman), DominionTravelQuotes[Index].DrawCount) * aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->CurrentStar->Position, DominionTravelQuotes[Index].Star->Position), 1.6E+3L) * (100 - Discount), 1.0E+2L)));
        };
        Discount = aPlayer::GetPlayer()->GetPirateServiceDiscount();
        if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.Battle != 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.GenericRefuseBattle"_wref.get());
            M_Main(true);
            return;
        }
        if (aPlayer::GetPlayer()->DockedTo->InHyperspace) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBNoInHyper"_wref.get());
            M_Main(true);
            return;
        }
        if (!CheckDominionServiceStanding(2, aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBRefusePrefix"_wref.get()), 0.5f)) {
            M_Main(true);
            return;
        }
        if (!CheckDominionAvailable()) {
            M_Main(true);
            return;
        }
        if (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge < 45) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBNoEnergy"_wref.get());
            M_Main(true);
            return;
        }
        Seed = aPlayer::GetPlayer()->DockedTo->Seed + aGalaxy::Galaxy->CurrentTurn / 31 + aPlayer::GetPlayer()->CurrentStar->GenerationSeed;
        std::int32_t Count = 0;
        for (I = 1; I <= 4; ++I) {
            {
                std::int32_t nextRandomIntRange = aMyFunction::NextRandomIntRange(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1, Seed);
                aMyFunction::TObjectList* stars = aGalaxy::Galaxy->Stars;
                Star = pas::list_at<aGalaxy::TStar>(stars, nextRandomIntRange);
            }
            if (aPlayer::GetPlayer()->CurrentStar != Star && Star->IsConstellationVisible() && (Star->Constellation->Id != 20 || aGalaxy::Galaxy->CoalitionDefeatedTurn != 0)) {
                Duplicate = false;
                for (auto cpp_range = pas::for_to<std::int32_t>(1, Count); cpp_range.next(J); ) {
                    if (DominionTravelQuotes[J].Star == Star) {
                        Duplicate = true;
                        ++DominionTravelQuotes[J].DrawCount;
                        DominionTravelQuotes[J].Cost = GetDominionTravelQuoteCost(J);
                    }
                }
                if (!Duplicate) {
                    ++Count;
                    DominionTravelQuotes[Count].Star = Star;
                    DominionTravelQuotes[Count].DrawCount = 1;
                    DominionTravelQuotes[Count].Cost = GetDominionTravelQuoteCost(Count);
                }
            }
        }
        if (Count <= 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBNoPath"_wref.get());
            M_Main(true);
            return;
        }
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBChoseDestination"_wref.get());
        ClearChoices();
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Count); cpp_range_2.next(I); ) {
            Text = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.ToStar"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<ToStar>"_w, DominionTravelQuotes[I].Star->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Cost>"_w, pas::wide_int_to_str(DominionTravelQuotes[I].Cost), u"<color=255,240,100>"_w);
            if (aPlayer::GetPlayer()->Money >= DominionTravelQuotes[I].Cost) {
                AddChoice(pas::concat_wide({u"- ", Text}), I, pas::bind_method<&TfRuinsTalk::ConfirmDominionTravel>(this));
            } else {
                AddChoice(pas::concat_wide({u"- ", Text}), 0, fTalk::ScriptDialogBlockCallback);
            }
            Text = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.ToList"_wref.get());
            aMyFunction::ReplaceTextToken(Text, u"<ToStar>"_w, DominionTravelQuotes[I].Star->Name, u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Cost>"_w, pas::wide_int_to_str(DominionTravelQuotes[I].Cost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(Text, u"<Dist>"_w, pas::wide_int64_to_str(System::Round(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, DominionTravelQuotes[I].Star->Position))), u"<color=255,240,100>"_w);
            DialogText = pas::concat_wide({DialogText, u"\r\n", pas::wide_int_to_str(I), u") ", Text});
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.Refuse"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineDominionTravel>(this));
    }

    void TfRuinsTalk::ConfirmDominionTravel(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBConfirmation"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<ToStar>"_w, DominionTravelQuotes[Action].Star->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Cost>"_w, pas::wide_int_to_str(DominionTravelQuotes[Action].Cost), u"<color=255,240,100>"_w);
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptDominionTravel>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.Confirm"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_2), Action, cpp_arg);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_3 = pas::bind_method<&TfRuinsTalk::DeclineDominionTravelConfirmation>(this);
            pas::WideString cpp_arg_4 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.NoConfirm"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_4), 0, cpp_arg_3);
        }
    }

    void TfRuinsTalk::DeclineDominionTravel(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBAfterRefuse"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::AcceptDominionTravel(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBAfterConfirm"_wref.get());
        aPlayer::GetPlayer()->QueuedTravelTarget = DominionTravelQuotes[Action].Star;
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - DominionTravelQuotes[Action].Cost);
        TfRuinsTalk::SpendDominionServiceCredit(0.5f);
        reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge = (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge - 45) / 2;
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<FlyToStar>"_w, aPlayer::GetPlayer()->QueuedTravelTarget->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::DeclineDominionTravelConfirmation(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBAfterNoConfirm"_wref.get());
        M_Main(true);
    }

    void TfRuinsTalk::ShowDominionCancelTravelDialog(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBCancelTeleport"_wref.get());
        ClearChoices();
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CancelYes"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::CancelDominionTravel>(this));
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CancelNo"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineDominionCancelTravel>(this));
    }

    void TfRuinsTalk::CancelDominionTravel(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBAfterCancelTeleport"_wref.get());
        aPlayer::GetPlayer()->QueuedTravelTarget = nullptr;
        M_Main(true);
    }

    void TfRuinsTalk::DeclineDominionCancelTravel(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.ShuffleTeleport.CBAfterNoCancel"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<FlyToStar>"_w, aPlayer::GetPlayer()->QueuedTravelTarget->Name, u"<color=255,240,100>"_w);
        M_Main(true);
    }

    void TfRuinsTalk::ShowDominionRelocationDialog(std::int32_t Action) {
        static const pas::Set<0, 255> StationMask = pas::constant_set<pas::Set<0, 255>>({{6, 12}});
        std::int32_t I{};
        std::int32_t Index{};
        std::int32_t Cost{};
        aGalaxy::TStar* Star{};
        ClearChoices();
        if (!CheckDominionServiceStanding(3, aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.CBRefusePrefix"_wref.get()), 1.0f)) {
            BuildDominionWarOptions();
            return;
        }
        if (!CheckDominionAvailable()) {
            BuildDominionWarOptions();
            return;
        }
        if (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge < 150) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.CBNoEnergy"_wref.get());
            BuildDominionWarOptions();
            return;
        }
        std::int32_t Count = 0;
        std::int32_t First = 1;
        std::int32_t Last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
        Index = aMyFunction::SeededRandomIntRange(First, Last, aPlayer::GetPlayer()->DockedTo->Seed + aGalaxy::Galaxy->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 1141);
        pas::List* Constellations = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            if (!aPlayer::GetPlayer()->NoJump) {
                aMyFunction::IncrementWrapped(Index, First, Last);
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                if (static_cast<std::uint8_t>(Star->NoComeKling ^ 1) && pas::list_indexof(Constellations, reinterpret_cast<void*>(Star->Constellation)) < 0) {
                    if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 1387) >= 0.6L && Star->Constellation->Id != 20 && (Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstDominion] <= 0 || aPlayer::GetPlayer()->DockedTo->CurrentStar->Constellation == Star->Constellation) && Star->Constellation->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationMask)) < pas::list_count(Star->Constellation->Stars) && Star->Dominion == nullptr && Star->CountShipsByTypeMask(static_cast<aGalaxyStruct::TShipTypeMask>(StationMask)) <= 2 && Star->ShipTypeCounts[aGalaxyStruct::stKling] <= 0 && Star->Status.ControlFaction == aGalaxyStruct::sfPirates && Star->Status.Battle == 0 && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->DockedTo->CurrentStar != Star && Star->IsConstellationVisible()) {
                        ++Count;
                        Cost = fRuinsTalk::GetDominionRelocationCost(Star);
                        if (aPlayer::GetPlayer()->Money < Cost) {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg = pas::concat_wide({u"- ", ([&] {
                                auto name = pas::borrow(Star->Name);
                                pas::WideString intToStr = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.ToStar"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<ToStar>"_w, name.get(), u"<Money>"_w, std::move(intToStr));
                            }())});
                            AddChoice(std::move(cpp_arg), 0, scriptDialogBlockCallback);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", ([&] {
                                auto name_2 = pas::borrow(Star->Name);
                                pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.ToStar"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<ToStar>"_w, name_2.get(), u"<Cost>"_w, std::move(intToStr_2));
                            }())}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Star)), pas::bind_method<&TfRuinsTalk::AcceptDominionRelocation>(this));
                        }
                        // Native disabled choice uses the shared quote and <Money>, unlike the enabled choice.
                        pas::list_add(Constellations, reinterpret_cast<void*>(Star->Constellation));
                    }
                }
            }
        }
        pas::free(Constellations);
        if (Count <= 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.CBNoPath"_wref.get());
            BuildDominionWarOptions();
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.CBChoseDestination"_wref.get());
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.Refuse"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineDominionRelocation>(this));
        }
    }

    void TfRuinsTalk::AcceptDominionRelocation(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.CBAfterConfirm"_wref.get());
        aGalaxy::TStar* Star = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        aMyFunction::ReplaceTextToken(DialogText, u"<ToStar>"_w, Star->Name, u"<color=255,240,100>"_w);
        {
            std::int32_t cpp_right = fRuinsTalk::GetDominionRelocationCost(Star);
            std::int32_t cpp_arg = aPlayer::GetPlayer()->Money - cpp_right;
            aShip::TShip* player = aPlayer::GetPlayer();
            player->SetMoney(cpp_arg);
        }
        TfRuinsTalk::SpendDominionServiceCredit(1.0f);
        reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge = (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge - 150) / 2;
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        {
            EC_Struct::TPointF selectTeleportArrivalPoint = reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->SelectTeleportArrivalPoint(Star);
            aShip::TShip* dockedTo = aPlayer::GetPlayer()->DockedTo;
            dockedTo->OrderTeleport(Star, selectTeleportArrivalPoint, 10, true);
        }
        aPlayer::GetPlayer()->DockedTo->CurrentStar->Dominion = nullptr;
        Star->Dominion = aPlayer::GetPlayer()->DockedTo;
        ClearChoices();
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_2 = pas::bind_method<&TfRuinsTalk::OpenHangar>(this);
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.ToHangar"_wref.get())});
            TfRuinsTalk* self = this;
            self->AddChoice(std::move(cpp_arg_3), 0, cpp_arg_2);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::ReturnToMain>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.IgnoreWarning"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
    }

    void TfRuinsTalk::DeclineDominionRelocation(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.CBAfterRefuse"_wref.get());
        BuildDominionWarOptions();
    }

    void TfRuinsTalk::BuildDominionWarOptions() {
        ClearChoices();
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.PlayerAskAboutRanks"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionRanksAnswer>(this));
        }
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.PlayerAskAboutCapture"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionCaptureAnswer>(this));
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionWarOperationDialog>(this));
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Relocate.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionRelocationDialog>(this));
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionAmbushDialog>(this));
        }
        if (aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.PlayerSend"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ShowDominionAssaultDialog>(this));
        }
        if (aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}}))) {
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.PlayerSend2"_wref.get())}), 1, pas::bind_method<&TfRuinsTalk::ShowDominionAssaultDialog>(this));
        }
        AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.PlayerNoQuestions"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::ReturnToMain>(this));
    }

    void TfRuinsTalk::I_CBWarWithKlingAndCoalition(std::int32_t Action) {
        pas::WideString Path{};
        std::int32_t I{};
        if (aPlayer::GetPlayer()->DockedTo->CurrentStar->Status.Battle != 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.GenericRefuseBattle"_wref.get());
            M_Main(true);
            return;
        }
        aGalaxy::TStar* Star = nullptr;
        std::uint8_t CoalitionPercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfCoalition);
        std::uint8_t DominatorPercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfDominators);
        std::uint8_t PiratePercent = aGalaxy::Galaxy->GetFactionControlPercent(aGalaxyStruct::sfPirates);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            if (pas::is_one_of<aGalaxyStruct::sfCoalition, aGalaxyStruct::sfDominators>(pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star))->Status.ControlFaction) && pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star))->Constellation->Id != 20) {
                Star = aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star;
                break;
            }
        }
        if (Star == nullptr) {
            if (static_cast<std::uint8_t>(aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) ^ 1) && aGalaxy::Galaxy->CoalitionDefeatedTurn == 0) {
                Path = u"FormRuins.CB.WarPlans.WarWithKlingAndCoalition.CoalitionOnly.CBAnswerWeControl100Percent"_w;
            } else if (aGalaxy::Galaxy->HasUnresolvedDominatorSeries(pas::constant_set<aGalaxy::TDominatorSeriesSet>({{aGalaxyStruct::dsBlazer}, {aGalaxyStruct::dsKeller}, {aGalaxyStruct::dsTerron}})) && aGalaxy::Galaxy->CoalitionDefeatedTurn != 0) {
                Path = u"FormRuins.CB.WarPlans.WarWithKlingAndCoalition.KlingOnly.CBAnswerWeControl100Percent"_w;
            } else {
                Path = u"FormRuins.CB.WarPlans.WarWithKlingAndCoalition.KlingAndCoalition.CBAnswerWeControl100Percent"_w;
            }
        } else {
            if (DominatorPercent > CoalitionPercent * 3) {
                Path = u"FormRuins.CB.WarPlans.WarWithKlingAndCoalition.KlingOnly."_w;
            } else if (DominatorPercent * 3 < CoalitionPercent) {
                Path = u"FormRuins.CB.WarPlans.WarWithKlingAndCoalition.CoalitionOnly."_w;
            } else {
                Path = u"FormRuins.CB.WarPlans.WarWithKlingAndCoalition.KlingAndCoalition."_w;
            }
            {
                std::uint8_t cpp_case = PiratePercent;
                if (cpp_case >= 0 && cpp_case <= 9) {
                    Path = pas::concat_wide({Path, u"CBAnswerWeControlMore00Percent"});
                } else if (cpp_case >= 10 && cpp_case <= 22) {
                    Path = pas::concat_wide({Path, u"CBAnswerWeControlMore10Percent"});
                } else if (cpp_case >= 23 && cpp_case <= 35) {
                    Path = pas::concat_wide({Path, u"CBAnswerWeControlMore30Percent"});
                } else if (cpp_case >= 36 && cpp_case <= 59) {
                    Path = pas::concat_wide({Path, u"CBAnswerWeControlMore50Percent"});
                } else if (cpp_case >= 60 && cpp_case <= 89) {
                    Path = pas::concat_wide({Path, u"CBAnswerWeControlMore70Percent"});
                } else if (cpp_case >= 90 && cpp_case <= 99) {
                    Path = pas::concat_wide({Path, u"CBAnswerWeControlMore90Percent"});
                } else if (cpp_case == 100) {
                    Path = pas::concat_wide({Path, u"CBAnswerWeControl100Percent"});
                } else {
                    DialogText = u"Error in procedure TfRuinsTalk.I_CBWarWithKlingAndCoalition"_w;
                }
            }
        }
        DialogText = aConst::LocalizedColorText(Path);
        aMyFunction::ReplaceTextToken(DialogText, u"<CB>"_w, aPlayer::GetPlayer()->DockedTo->Name, u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<Percent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(CoalitionPercent)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<DominatorsPercent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(DominatorPercent)), u"<color=255,240,100>"_w);
        aMyFunction::ReplaceTextToken(DialogText, u"<PiratesPercent>"_w, pas::wide_int_to_str(static_cast<std::int32_t>(PiratePercent)), u"<color=255,240,100>"_w);
        DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarWithKlingAndCoalition.ExtraTextAboutRanks"_wref.get())});
        BuildDominionWarOptions();
    }

    void TfRuinsTalk::ShowDominionCaptureAnswer(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.CBAnswersAboutCapture"_wref.get());
        BuildDominionWarOptions();
    }

    void TfRuinsTalk::ShowDominionRanksAnswer(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.CBAnswersAboutRanks"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<BaseName>"_w, aPlayer::GetPlayer()->DockedTo->Name, pas::WideString());
        BuildDominionWarOptions();
    }

    void TfRuinsTalk::ShowDominionWarOperationDialog(std::int32_t Action) {
        if (!([&] {
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.CBRefusePrefix"_wref.get());
            TfRuinsTalk* self = this;
            return self->CheckDominionServiceStanding(1, std::move(localizedColorText), 0.0f);
        }())) {
            BuildDominionWarOptions();
            return;
        }
        StationServiceQuoteCost = fRuinsTalk::ApplyRecentDominionOrderSurcharge(aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman));
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.CBAboutWarOperation"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
        ClearChoices();
        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptDominionWarOperation>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr = pas::wide_int_to_str(StationServiceQuoteCost);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.PlayerOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
            }())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_2), 0, cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", ([&] {
                pas::WideString intToStr_2 = pas::wide_int_to_str(StationServiceQuoteCost);
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.PlayerOk"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr_2));
            }())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::DeclineDominionWarOperation>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.PlayerNo"_wref.get())});
            TfRuinsTalk* self_4 = this;
            self_4->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
    }

    void TfRuinsTalk::AcceptDominionWarOperation(std::int32_t Action) {
        pas::WideString Names{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        std::int32_t Available{};
        std::int32_t Total{};
        aShip::TShip* Ship{};
        pas::Extended Strength{};
        aGalaxy::TStar* Candidate{};
        aGalaxy::TStar* Origin{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.CBAfterOk"_wref.get());
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        StationServiceQuoteCost = fRuinsTalk::ApplyRecentDominionOrderSurcharge(aGalaxy::Galaxy->ComputeScaledHugeMoney(aGalaxyStruct::oiHuman));
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
        if (aPlanet::MainPiratePlanet != nullptr) {
            aPlanet::MainPiratePlanet->ChangeRelationToRanger(aPlayer::GetPlayer(), 10);
        }
        aGalaxy::TStar* Target = nullptr;
        std::int32_t BestAvailable = 0;
        std::int32_t BestTotal = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Candidate = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (Candidate->Constellation->Id != 20 && Candidate->IsConstellationVisible() && static_cast<std::uint8_t>(Candidate->NoComeKling ^ 1) && static_cast<std::uint8_t>(aScript::IsStarProtectedByScript(Candidate) ^ 1) && Candidate->Status.ControlFaction != aGalaxyStruct::sfPirates) {
                Available = 0;
                Total = 0;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(J); ) {
                    Origin = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, J);
                    if (Candidate != Origin && Origin->Status.ControlFaction == aGalaxyStruct::sfPirates && Origin->Status.CustomFaction == u"" && Origin->Status.Battle == 0 && Origin->CountForcesByOwnerGroups(Strength, true, true, false, false) <= 0) {
                        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Origin->Ships) - 1); cpp_range_3.next(K); ) {
                            Ship = pas::list_at<aShip::TShip>(Origin->Ships, K);
                            if (pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate && Ship->PartnerShip == nullptr) {
                                if (!(Ship->GetFuelTanks() == nullptr || Ship->GetEngine() == nullptr || static_cast<long double>(aMyFunction::PointDistance(Origin->Position, Candidate->Position)) > std::min<std::int32_t>(static_cast<std::int32_t>(Ship->GetFuelTanks()->Capacity), static_cast<std::int32_t>(Ship->GetEngine()->JumpRange)) + 10)) {
                                    ++Total;
                                    if (static_cast<std::uint8_t>(Ship->OrderAbsolute ^ 1) && Ship->AbsoluteScriptOrder == 0 && static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && Ship->ScriptShip == nullptr) {
                                        ++Available;
                                    }
                                }
                            }
                        }
                    }
                }
                if (Available > BestAvailable) {
                    BestAvailable = Available;
                    BestTotal = Total;
                    Target = Candidate;
                }
            }
        }
        if (Target == nullptr || BestTotal < 10 || BestAvailable < 7) {
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.CBAfterOkBad"_wref.get())});
            ClearChoices();
            M_Main(true);
        } else {
            BestAvailable = std::min<std::int32_t>(aPlayer::GetPlayer()->PirateRank + 7, BestAvailable);
            // Native completes each pass even after the requested ship count is reached.
            while (BestAvailable > 0) {
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_4.next(J); ) {
                    Origin = Target->StarDistances[J].Star;
                    if (Origin->Status.ControlFaction == aGalaxyStruct::sfPirates && Origin->Status.CustomFaction == u"" && Origin->Status.Battle == 0 && Origin->CountForcesByOwnerGroups(Strength, true, true, false, false) <= 0) {
                        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Origin->Ships) - 1); cpp_range_5.next(K); ) {
                            Ship = pas::list_at<aShip::TShip>(Origin->Ships, K);
                            if (pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate && Ship->PartnerShip == nullptr) {
                                if (!(Ship->GetFuelTanks() == nullptr || Ship->GetEngine() == nullptr || static_cast<long double>(aMyFunction::PointDistance(Origin->Position, Target->Position)) > std::min<std::int32_t>(static_cast<std::int32_t>(Ship->GetFuelTanks()->Capacity), static_cast<std::int32_t>(Ship->GetEngine()->JumpRange)) + 10)) {
                                    if (static_cast<std::uint8_t>(Ship->OrderAbsolute ^ 1) && Ship->AbsoluteScriptOrder == 0 && static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && Ship->ScriptShip == nullptr) {
                                        if (aMyFunction::NextRandomIntRange(1, 10, aPlayer::GetPlayer()->DockedTo->RandomState) < 3) {
                                            Ship->OrderJump(Target, true);
                                            reinterpret_cast<aPirate::TPirate*>(Ship)->RaidPressure = reinterpret_cast<aPirate::TPirate*>(Ship)->RaidPressure + 1.0E+1L;
                                            if (Names == u"") {
                                                Names = Ship->GetFullName(u" "_wref.get());
                                            } else {
                                                Names = pas::concat_wide({Names, u"\r\n", Ship->GetFullName(u" "_wref.get())});
                                            }
                                            --BestAvailable;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerOrdersPirateRaid"_w, nullptr);
            Event->AddData(Target->Id);
            DialogText = pas::concat_wide({DialogText, u"\r\n", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.CBAfterOkGood"_wref.get())});
            aMyFunction::ReplaceTextToken(DialogText, u"<Names>"_w, Names, pas::WideString());
            aMyFunction::ReplaceTextToken(DialogText, u"<Money>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
            aMyFunction::ReplaceTextToken(DialogText, u"<StarEnemy>"_w, Target->Name, u"<color=255,240,100>"_w);
            {
                pas::WideString name = Target->Constellation->GetName();
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<SectorEnemy>"_w, std::move(name), u"<color=255,240,100>"_w);
            }
            ClearChoices();
            M_Main(true);
        }
    }

    void TfRuinsTalk::DeclineDominionWarOperation(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.WarOperation.CBAfterNo"_wref.get());
        BuildDominionWarOptions();
    }

    void TfRuinsTalk::ShowDominionAmbushDialog(std::int32_t Action) {
        std::int32_t I{};
        aGalaxy::TStar* Star{};
        aGroup::TGroup* Group{};
        aShip::TShip* Ship{};
        aGroup::TGroupRouteOrder Entry{};
        if (!([&] {
            pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBRefusePrefix"_wref.get());
            TfRuinsTalk* self = this;
            return self->CheckDominionServiceStanding(4, std::move(localizedColorText), 1.5f);
        }())) {
            BuildDominionWarOptions();
            return;
        }
        if (!CheckDominionAvailable()) {
            BuildDominionWarOptions();
            return;
        }
        if (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge < 150) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBNoEnergy"_wref.get());
            BuildDominionWarOptions();
            return;
        }
        aGalaxy::TStar* Target = nullptr;
        std::int32_t Turn = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->LiberationGroups) - 1); cpp_range.next(I); ) {
            Group = pas::list_at<aGroup::TGroup>(aGalaxy::Galaxy->LiberationGroups, I);
            if (Group != nullptr) {
                if (pas::list_count(Group->Ships) > 0) {
                    Entry = Group->Route[Group->Route.length() - 1];
                    if (Entry.Kind == 3) {
                        Star = reinterpret_cast<aGalaxy::TStar*>(Entry.Target);
                        if (Star != nullptr) {
                            if (Star->IsConstellationVisible() && (Star->Status.ControlFaction != aGalaxyStruct::sfCoalition || Star->Status.CustomFaction != u"") && (Group->Route[2].WaitUntilTurn < Turn || Target == nullptr)) {
                                Target = Star;
                                Turn = Group->Route[2].WaitUntilTurn;
                            }
                        }
                    }
                }
            }
        }
        aRuins::TRuins* Base = static_cast<aRuins::TRuins*>(aGalaxy::TGalaxy::FindMilitaryBaseInTransit());
        if (Base != nullptr) {
            Star = Base->FlyToStar;
            // The native fallback is outside the destination eligibility conjunction.
            if (Star != nullptr && Star->IsConstellationVisible() && (Star->Status.ControlFaction != aGalaxyStruct::sfCoalition || Star->Status.CustomFaction != u"") && Base->FlyDate < Turn || Target == nullptr) {
                Target = Star;
                Turn = Base->FlyDate;
            }
        }
        if (Target == nullptr) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBAnswerNoOperations"_wref.get());
            BuildDominionWarOptions();
            return;
        }
        if (Turn - aGalaxy::Galaxy->CurrentTurn > 21) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBAnswerNoOperationsSoon"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, Target->Name, u"<color=255,240,100>"_w);
            {
                pas::WideString formatGameTurnDate = aGalaxy::FormatGameTurnDate(Turn);
                pas::WideString& dialogText = DialogText;
                aMyFunction::ReplaceTextToken(dialogText, u"<Date>"_w, std::move(formatGameTurnDate), u"<color=255,240,100>"_w);
            }
            BuildDominionWarOptions();
            return;
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_2.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (Star->Dominion != nullptr) {
                if (reinterpret_cast<aRuins::TRuins*>(Star->Dominion)->FlyToStar == Target) {
                    DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBAnswerAmbushAlreadyInProgress"_wref.get());
                    aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, Target->Name, u"<color=255,240,100>"_w);
                    {
                        pas::WideString formatGameTurnDate_2 = aGalaxy::FormatGameTurnDate(Turn);
                        pas::WideString& dialogText_2 = DialogText;
                        aMyFunction::ReplaceTextToken(dialogText_2, u"<Date>"_w, std::move(formatGameTurnDate_2), u"<color=255,240,100>"_w);
                    }
                    BuildDominionWarOptions();
                    return;
                }
            }
        }
        std::int32_t Count = 0;
        Star = aPlayer::GetPlayer()->DockedTo->CurrentStar;
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(I); ) {
            // Preserve index-before-receiver evaluation under DCC32 O-.
            Ship = pas::list_at<aShip::TShip>(Star->Ships, I * 1);
            if (static_cast<std::uint8_t>(Ship->InHyperspace ^ 1) && pas::class_cast_if<aPirate::TPirate*>(Ship) != nullptr && Ship->OwnerId == aGalaxyStruct::oiPirate && Ship->PartnerShip == nullptr && Ship->ScriptShip == nullptr && static_cast<std::uint8_t>(Ship->HasScriptControl() ^ 1)) {
                ++Count;
            }
        }
        if (Count < 8) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBAnswerNotEnoughPirates"_wref.get());
            aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, Target->Name, u"<color=255,240,100>"_w);
            {
                pas::WideString formatGameTurnDate_3 = aGalaxy::FormatGameTurnDate(Turn);
                pas::WideString& dialogText_3 = DialogText;
                aMyFunction::ReplaceTextToken(dialogText_3, u"<Date>"_w, std::move(formatGameTurnDate_3), u"<color=255,240,100>"_w);
            }
            BuildDominionWarOptions();
            return;
        }
        ClearChoices();
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBAnswerOperationSoon"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<Star>"_w, Target->Name, u"<color=255,240,100>"_w);
        {
            pas::WideString formatGameTurnDate_4 = aGalaxy::FormatGameTurnDate(Turn);
            pas::WideString& dialogText_4 = DialogText;
            aMyFunction::ReplaceTextToken(dialogText_4, u"<Date>"_w, std::move(formatGameTurnDate_4), u"<color=255,240,100>"_w);
        }
        StationServiceQuoteCost = fRuinsTalk::ApplyRecentDominionOrderSurcharge(fRuinsTalk::GetDominionRelocationCost(Target) * 1.5L);
        aMyFunction::ReplaceTextToken(DialogText, u"<Cost>"_w, pas::wide_int_to_str(StationServiceQuoteCost), u"<color=255,240,100>"_w);
        if (aPlayer::GetPlayer()->Money >= StationServiceQuoteCost) {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg = pas::bind_method<&TfRuinsTalk::AcceptDominionAmbush>(this);
            pas::WideString cpp_arg_2 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.Confirm"_wref.get())});
            TfRuinsTalk* self_2 = this;
            self_2->AddChoice(std::move(cpp_arg_2), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Target)), cpp_arg);
        } else {
            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
            pas::WideString cpp_arg_3 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.Confirm"_wref.get())});
            TfRuinsTalk* self_3 = this;
            self_3->AddChoice(std::move(cpp_arg_3), 0, scriptDialogBlockCallback);
        }
        {
            GI_MessageLoop::TDialogChoiceEventGI cpp_arg_4 = pas::bind_method<&TfRuinsTalk::DeclineDominionAmbush>(this);
            pas::WideString cpp_arg_5 = pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.Refuse"_wref.get())});
            TfRuinsTalk* self_4 = this;
            self_4->AddChoice(std::move(cpp_arg_5), 0, cpp_arg_4);
        }
    }

    void TfRuinsTalk::AcceptDominionAmbush(std::int32_t Action) {
        aGalaxy::TStar* Star = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBAfterOk"_wref.get());
        aMyFunction::ReplaceTextToken(DialogText, u"<ToStar>"_w, Star->Name, u"<color=255,240,100>"_w);
        StationServiceQuoteCost = fRuinsTalk::ApplyRecentDominionOrderSurcharge(fRuinsTalk::GetDominionRelocationCost(Star) * 1.5L);
        aGalaxyEvent::TGalaxyEvent* Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerOrdersPirateAmbush"_w, nullptr);
        Event->AddData(Star->Id);
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - StationServiceQuoteCost);
        TfRuinsTalk::SpendDominionServiceCredit(1.5f);
        reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge = (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge - 150) / 2;
        reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar = Star;
        if (Star->Status.ControlFaction == aGalaxyStruct::sfPirates) {
            reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate = aGalaxy::Galaxy->CurrentTurn + 12;
        } else {
            reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate = aGalaxy::Galaxy->CurrentTurn + 16;
        }
        M_Main(true);
    }

    void TfRuinsTalk::DeclineDominionAmbush(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Ambush.CBAfterRefuse"_wref.get());
        BuildDominionWarOptions();
    }

    void TfRuinsTalk::ShowDominionAssaultDialog(std::int32_t Action) {
        std::int32_t I{};
        std::int32_t Index{};
        std::int32_t Cost{};
        aGalaxy::TStar* Star{};
        aGalaxyStruct::TStarFaction Faction{};
        std::uint8_t Rank{};
        float CreditCost{};
        ClearChoices();
        if (Action == 0) {
            Faction = aGalaxyStruct::sfCoalition;
            Rank = 5;
            CreditCost = 2.0f;
        } else {
            Faction = aGalaxyStruct::sfDominators;
            Rank = 6;
            CreditCost = 3.0f;
        }
        if (!CheckDominionServiceStanding(Rank, aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.CBRefusePrefix"_wref.get()), CreditCost)) {
            BuildDominionWarOptions();
            return;
        }
        if (!CheckDominionAvailable()) {
            BuildDominionWarOptions();
            return;
        }
        if (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge < 150) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.CBNoEnergy"_wref.get());
            BuildDominionWarOptions();
            return;
        }
        std::int32_t Count = 0;
        std::int32_t First = 1;
        std::int32_t Last = pas::list_count(aGalaxy::Galaxy->Stars) - 1;
        Index = aMyFunction::SeededRandomIntRange(First, Last, aPlayer::GetPlayer()->DockedTo->Seed + aGalaxy::Galaxy->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 1141);
        pas::List* Constellations = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            if (!aPlayer::GetPlayer()->NoJump) {
                aMyFunction::IncrementWrapped(Index, First, Last);
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[Index].Star));
                if (static_cast<std::uint8_t>(Star->NoComeKling ^ 1) && pas::list_indexof(Constellations, reinterpret_cast<void*>(Star->Constellation)) < 0) {
                    if (aMyFunction::SeededRandomUnitFloat(aPlayer::GetPlayer()->DockedTo->Seed + Star->GenerationSeed + aGalaxy::Galaxy->CurrentTurn / 60 + 1387) >= 0.8L && Star->Constellation->Id != 20 && Star->Constellation->ShipTypeCounts[aGalaxyStruct::rstDominion] <= 0 && Star->Dominion == nullptr && Star->Status.ControlFaction == Faction && Star->Status.CustomFaction == u"" && aPlayer::GetPlayer()->DockedTo->CurrentStar != Star && Star->IsConstellationVisible()) {
                        ++Count;
                        Cost = fRuinsTalk::ApplyRecentDominionOrderSurcharge(fRuinsTalk::GetDominionRelocationCost(Star) * 2.5L);
                        if (aPlayer::GetPlayer()->Money < Cost) {
                            GI_MessageLoop::TDialogChoiceEventGI scriptDialogBlockCallback = fTalk::ScriptDialogBlockCallback;
                            pas::WideString cpp_arg = pas::concat_wide({u"- ", ([&] {
                                auto name = pas::borrow(Star->Name);
                                pas::WideString intToStr = pas::wide_int_to_str(StationServiceQuoteCost);
                                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.ToStar"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<ToStar>"_w, name.get(), u"<Money>"_w, std::move(intToStr));
                            }())});
                            AddChoice(std::move(cpp_arg), 0, scriptDialogBlockCallback);
                        } else {
                            AddChoice(pas::concat_wide({u"- ", ([&] {
                                auto name_2 = pas::borrow(Star->Name);
                                pas::WideString intToStr_2 = pas::wide_int_to_str(Cost);
                                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.ToStar"_wref.get());
                                return aMyFunction::FormatText2(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<ToStar>"_w, name_2.get(), u"<Cost>"_w, std::move(intToStr_2));
                            }())}), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Star)), pas::bind_method<&TfRuinsTalk::AcceptDominionAssault>(this));
                        }
                        // Native disabled choice uses the shared quote and <Money>, unlike the enabled choice.
                        pas::list_add(Constellations, reinterpret_cast<void*>(Star->Constellation));
                    }
                }
            }
        }
        pas::free(Constellations);
        if (Count <= 0) {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.CBNoPath"_wref.get());
            BuildDominionWarOptions();
        } else {
            DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.CBChoseDestination"_wref.get());
            AddChoice(pas::concat_wide({u"- ", aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.Refuse"_wref.get())}), 0, pas::bind_method<&TfRuinsTalk::DeclineDominionAssault>(this));
        }
    }

    void TfRuinsTalk::AcceptDominionAssault(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.CBAfterConfirm"_wref.get());
        aGalaxy::TStar* Star = reinterpret_cast<aGalaxy::TStar*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Action)));
        aMyFunction::ReplaceTextToken(DialogText, u"<ToStar>"_w, Star->Name, u"<color=255,240,100>"_w);
        {
            std::int32_t cpp_right = fRuinsTalk::ApplyRecentDominionOrderSurcharge(fRuinsTalk::GetDominionRelocationCost(Star) * 2.5L);
            std::int32_t cpp_arg = aPlayer::GetPlayer()->Money - cpp_right;
            aShip::TShip* player = aPlayer::GetPlayer();
            player->SetMoney(cpp_arg);
        }
        TfRuinsTalk::SpendDominionServiceCredit(1.0f);
        reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge = (reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->RelocationAge - 150) / 2;
        GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
        reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyToStar = Star;
        reinterpret_cast<aRuins::TRuins*>(aPlayer::GetPlayer()->DockedTo)->FlyDate = aGalaxy::Galaxy->CurrentTurn + 12;
        M_Main(true);
    }

    void TfRuinsTalk::DeclineDominionAssault(std::int32_t Action) {
        DialogText = aConst::LocalizedColorText(u"FormRuins.CB.WarPlans.Assault.CBAfterRefuse"_wref.get());
        BuildDominionWarOptions();
    }

    void TfRuinsTalk::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10009);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20009);
        }
    }

    void TfRuinsTalk::p_destroy() {
        fRuinsTalk::TfRuinsTalk_Destroy(this);
    }

    pas::Method<void(std::uint32_t, std::uint32_t, std::int32_t)> TfRuinsTalk::bind_ProcessWindowMessage() {
        return pas::bind_method<fRuinsTalk::TfRuinsTalk_ProcessWindowMessage>(this);
    }

} // namespace fRuinsTalk
