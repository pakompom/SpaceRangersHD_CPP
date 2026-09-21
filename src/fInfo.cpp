#include "layout/fInfo.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Data.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_Edit.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_PanelScrollBar.hpp"
#include "types/GI_ScrollBar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Sound.hpp"
#include "types/MessagesSdk.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Space.hpp"
#include "types/SE_Star.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/aTransport.hpp"
#include "types/fGov.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fShip2.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fInfo.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPanelPlanet.hpp"
#include "units/fPanelRuins.hpp"

namespace fInfo {
    // Nested in RunSearch.
    pas::WideString GetInfoQualityGrade(float Quality);

    // Nested in RunSearch.
    pas::WideString GetInfoEquipmentSummary(aItem::TItem* Item);

    // Nested in RunSearch; captures location, count, and displayed objects.
    void AddInfoSearchResult(pas::Object* Value, TfInfo* Self, aShip::TShip*& Ship, std::int32_t& ResultCount, pas::List*& Shown, std::uint8_t& Found, aRuins::TRuins*& Station, pas::WideString& Description, pas::WideString& Heading, aPlanet::TPlanet*& Planet, aGalaxy::TStar*& Star, std::int32_t& Bearing);

    pas::Array<pas::WideString, 0, 19> InfoNewsAnimationNames = pas::Array<pas::WideString, 0, 19>{{
        u"a"_w, u"b"_w, u"c"_w, u"d"_w, u"e"_w, u"f"_w, u"g"_w, u"h"_w,
        u"k"_w, u"l"_w, u"m"_w, u"n"_w, u"o"_w, u"p"_w, u"q"_w, u"r"_w,
        u"s"_w, u"t"_w, u"u"_w, u"v"_w,
    }};

    // Native selection adds one to each weight.
    pas::Array<std::int32_t, 0, 19> InfoNewsAnimationWeights = pas::Array<std::int32_t, 0, 19>{{
        9, 5, 9, 9, 7, 5, 7, 1,
        3, 1, 0, 3, 3, 2, 3, 2,
        3, 4, 5, 3,
    }};

    // initializes I=1; increments and repeats until I=7.
    // The indexed FLD therefore reads these six remaining grades, not PirateSlotBonusWeights[-13+I].
    pas::Array<float, 0, 6> InfoQualityGrades = pas::Array<float, 0, 6>{{0.7f, 0.8f, 0.9f, 1.0f, 1.2f, 1.4f, 1.6f}};

    // Search is already lowercase; positions are one-based, zero means absent.
    std::int32_t FindLowercaseInfoTextFrom(const pas::WideString& Search, const pas::WideString& Text, std::int32_t StartPosition) {
        return ([&] {
            const pas::WideString& wideLowerCase = SysUtilsImports::WideLowerCase(Text);
            const pas::WideString& search = Search;
            return EC_Str::FindTextOffsetW(wideLowerCase, search, StartPosition - 1);
        }()) + 1;
    }

    std::int32_t FindLowercaseInfoText(const pas::WideString& Search, const pas::WideString& Text) {
        return fInfo::FindLowercaseInfoTextFrom(Search, Text, 1);
    }

    // Search terms must already be lowercase.
    std::uint8_t ItemMatchesInfoSearch(aItem::TItem* Item, pas::WideString Search) {
        std::int32_t I{};
        // Nested helper captures Item.
        auto ItemMatchesInfoSearchTerm = [&](pas::WideString Term) -> std::uint8_t {
            std::uint8_t Result = true;
            if (([&] {
                const pas::WideString& wideLowerCase = SysUtilsImports::WideLowerCase(EC_Str::RemoveTextTagsW(Item->GetDisplayName()));
                const pas::WideString& term = Term;
                return EC_Str::FindTextOffsetW(wideLowerCase, term, 0);
            }()) >= 0) {
                return Result;
            }
            if (aItem::THull* hull = pas::class_cast_if<aItem::THull*>(Item); hull != nullptr && hull->HullSeries != -1) {
                if (([&] {
                    const pas::WideString& wideLowerCase_2 = SysUtilsImports::WideLowerCase(EC_Str::RemoveTextTagsW(aConst::HullSeriesDefinitions[reinterpret_cast<aItem::THull*>(Item)->HullSeries].Name));
                    const pas::WideString& term_2 = Term;
                    return EC_Str::FindTextOffsetW(wideLowerCase_2, term_2, 0);
                }()) >= 0) {
                    return Result;
                }
            }
            if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item); weapon != nullptr && weapon->SpecialModuleIndex != 0) {
                if (([&] {
                    const pas::WideString& wideLowerCase_3 = SysUtilsImports::WideLowerCase(EC_Str::RemoveTextTagsW(pas::checked_cast<aItem::TWeapon*>(Item)->GetSpecialModuleName()));
                    const pas::WideString& term_3 = Term;
                    return EC_Str::FindTextOffsetW(wideLowerCase_3, term_3, 0);
                }()) >= 0) {
                    return Result;
                }
            }
            return false;
        };
        std::uint8_t Result = false;
        std::int32_t Count = EC_Str::CountDelimitedPartsW(pas::view(Search), u" "sv);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(I); ) {
            if (!ItemMatchesInfoSearchTerm(EC_Str::ExtractDelimitedPartW(pas::view(Search), I, u" "sv))) {
                return Result;
            }
        }
        return true;
    }

    void TfInfo_Create(TfInfo* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->PlanetPanel = pas::construct_call<fPanelPlanet::TfPanelPlanet>(fPanelPlanet::TfPanelPlanet_Create);
        Self->StationPanel = pas::construct_call<fPanelRuins::TfPanelRuins>(fPanelRuins::TfPanelRuins_Create);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfInfo_Destroy(TfInfo* Self) {
        if (Self->PlanetPanel != nullptr) {
            pas::free(Self->PlanetPanel);
            Self->PlanetPanel = nullptr;
        }
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

    void TfInfo::InitializeLayout() {
        std::int32_t ExtraHeight{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        PlanetPanel->InitializeLayout(this);
        StationPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fInfo... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGCity2"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGCity"sv)->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            ExtraHeight = std::min<std::int32_t>(std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0), 432) / 3 * 3;
            {
                GI_MessageLoop::TObjectGI* ButFormClose = MainPanel->FindByNameRecursive(u"ButFormClose"sv);
                ButFormClose->SetPosition(ClassesImports::Point(ButFormClose->LocalPosition.X, ButFormClose->LocalPosition.Y + ExtraHeight));
                {
                    GI_MessageLoop::TObjectGI* cpp_with_3 = ButFormClose->Parent;
                    cpp_with_3->SetPosition(ClassesImports::Point(cpp_with_3->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_3->LocalPosition.Y + (std::max<std::int32_t>(GR_Main::ExtraScreenHeight, 0) - ExtraHeight) / 2));
                    cpp_with_3->SetSize(ClassesImports::Point(cpp_with_3->ClientSize.X, cpp_with_3->ClientSize.Y + ExtraHeight));
                    {
                        GI_MessageLoop::TObjectGI* cpp_with_4 = cpp_with_3->FirstChild->NextSibling;
                        cpp_with_4->SetSize(ClassesImports::Point(cpp_with_4->ClientSize.X, cpp_with_4->ClientSize.Y + ExtraHeight));
                        {
                            GI_MessageLoop::TObjectGI* cpp_with_5 = cpp_with_4->NextSibling;
                            cpp_with_5->SetPosition(ClassesImports::Point(cpp_with_5->LocalPosition.X, cpp_with_5->LocalPosition.Y + ExtraHeight));
                        }
                    }
                    {
                        GI_MessageLoop::TObjectGI* ButSearch = cpp_with_3->FindByNameRecursive(u"ButSearch"sv);
                        ButSearch->SetPosition(ClassesImports::Point(ButSearch->LocalPosition.X, ButSearch->LocalPosition.Y + ExtraHeight));
                    }
                    {
                        GI_MessageLoop::TObjectGI* ButNews = cpp_with_3->FindByNameRecursive(u"ButNews"sv);
                        ButNews->SetPosition(ClassesImports::Point(ButNews->LocalPosition.X, ButNews->LocalPosition.Y + ExtraHeight));
                    }
                    {
                        GI_PanelScrollBar::TPanelScrollBarGI* PanelInfo = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(cpp_with_3->FindByNameRecursive(u"PanelInfo"sv));
                        PanelInfo->SetSize(ClassesImports::Point(PanelInfo->ClientSize.X, PanelInfo->ClientSize.Y + ExtraHeight));
                        PanelInfo->VerticalScrollBar->SetSize(ClassesImports::Point(PanelInfo->VerticalScrollBar->ClientSize.X, PanelInfo->VerticalScrollBar->ClientSize.Y + ExtraHeight));
                    }
                }
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"sv))->UpCallback = pas::bind_method<&TfInfo::EndTurnClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"sv))->UpCallback = pas::bind_method<&TfInfo::ShipClicked>(this);
        InfoPanel = pas::checked_cast<GI_PanelScrollBar::TPanelScrollBarGI*>(GetByName(u"PanelInfo"sv));
        pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextSearch"sv))->ClearFocusOnEnter = false;
    }

    void TfInfo::OnOpen() {
        std::int32_t I{};
        pas::WideString Path{};
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        MainPanel->OnOpen();
        LoadPanel->OnOpen();
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnOpen();
            PlanetPanel->Show();
            StationPanel->Hide();
        } else {
            PlanetPanel->Hide();
            StationPanel->OnOpen();
            StationPanel->Show();
        }
        {
            GI_GraphButton::TGraphButtonGI* ButFormClose = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButFormClose"sv));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                ButFormClose->UpCallback = pas::bind_method<&fPanelPlanet::TfPanelPlanet::PlanetClicked>(PlanetPanel);
            } else {
                ButFormClose->UpCallback = pas::bind_method<&fPanelRuins::TfPanelRuins::ServicesClicked>(StationPanel);
            }
        }
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->KeyDownCallback = pas::bind_method<&TfInfo::MainPanelKeyDown>(this);
            MainPanel->KeyUpCallback = pas::bind_static_method<&TfInfo::MainPanelKeyUp>(this);
            MainPanel->LeftButtonDownCallback = pas::bind_method<&TfInfo::MainPanelMouseDown>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"OkSearch"sv))->UpCallback = pas::bind_method<&TfInfo::SearchClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButClear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearchText>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCopy"sv))->UpCallback = pas::bind_method<&TfInfo::CopySearchText>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPaste"sv))->UpCallback = pas::bind_method<&TfInfo::PasteSearchText>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M01Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch01Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M02Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch02Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M03Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch03Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M04Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch04Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M05Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch05Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M06Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch06Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M07Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch07Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M09Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch09Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M10Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch10Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch11Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M12Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch12Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch13Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15Clear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch15Filters>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M12ButClear"sv))->UpCallback = pas::bind_method<&TfInfo::ClearSearch12Name>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M12ButCopy"sv))->UpCallback = pas::bind_method<&TfInfo::CopySearch12Name>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M12ButPaste"sv))->UpCallback = pas::bind_method<&TfInfo::PasteSearch12Name>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15ButCopy"sv))->UpCallback = pas::bind_method<&TfInfo::CopySearch15Name>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15ButPaste"sv))->UpCallback = pas::bind_method<&TfInfo::PasteSearch15Name>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S03"sv))->SetDown(true);
        this->MainPanel->RebuildMessageButtons(false);
        SearchMode = false;
        {
            GI_GraphButton::TGraphButtonGI* ButSearch = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButSearch"sv));
            ButSearch->UpCallback = pas::bind_method<&TfInfo::ToggleSearchMode>(this);
            ButSearch->SetDisabled(SearchMode);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButNews = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNews"sv));
            ButNews->UpCallback = pas::bind_method<&TfInfo::ToggleSearchMode>(this);
            ButNews->SetDisabled(static_cast<std::uint8_t>(SearchMode ^ 1));
        }
        GetByName(u"PanelSearch"sv)->SetActive(SearchMode);
        {
            GI_Image::TImageGI* BGCity2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity2"sv));
            BGCity2->SetActive(aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase));
            if (BGCity2->Active) {
                BGCity2->SetImagePath(pas::concat_wide({u"GAI,", aPlayer::GetPlayer()->CurrentStar->GetBackgroundImagePath(I)}));
                BGCity2->GaiImageControl->LoadFrameSequenceFromText(u"[50,0-0]"_wref.get());
                BGCity2->SetImageKindX(GI_Main::ikxCenter);
                BGCity2->SetImageKindY(GI_Main::ikyCenter);
            }
        }
        {
            GI_Image::TImageGI* BGCity = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity"sv));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                BGCity->SetActive(true);
                {
                    pas::WideString governmentBackgroundGraph = aPlayer::GetPlayer()->CurrentPlanet->GetGovernmentBackgroundGraph();
                    GI_Image::TImageGI* cpp_arg = pas::checked_cast<GI_Image::TImageGI*>(BGCity->FindByNameRecursive(u"BGCity"sv));
                    cpp_arg->SetImagePath(std::move(governmentBackgroundGraph));
                }
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                BGCity->SetActive(true);
                if (aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey != u"") {
                    Path = pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey, u"bg"});
                    if (GR_Main::CacheDataRoot->FileExistsByPath(Path)) {
                        BGCity->SetImagePath(pas::concat_wide({u"GI,", Path}));
                    } else {
                        BGCity->SetImagePath(pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[aPlayer::GetPlayer()->DockedTo->TypeId].Name, u"bg"}));
                    }
                } else {
                    BGCity->SetImagePath(pas::concat_wide({u"GI,Bm.FormRuins.", GR_Main::GiResourceSuffix(), aConst::ShipTypeNames[aPlayer::GetPlayer()->DockedTo->TypeId].Name, u"bg"}));
                }
            } else {
                BGCity->SetActive(false);
            }
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 15); cpp_range.next(I); ) {
            GI_GraphButton::TGraphButtonGI* cpp_with_7 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"ButMM_", EC_Str::IntToFixedWidthWideString(I, 2)}))));
            cpp_with_7->StateChangedCallback = pas::bind_method<&TfInfo::CategoryStateChanged>(this);
            cpp_with_7->UpCallback = pas::bind_method<&TfInfo::CategoryClicked>(this);
        }
        BindFilterLabels(GetByName(u"PanelInfo"sv)->Parent);
        RefreshNewsAnimation(nullptr);
        SetFocusedControl(nullptr);
        ShowNews();
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(201);
    }

    void TfInfo::OnClose() {
        aGalaxy::Galaxy->CheckIntegrityChecksum(202);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        InfoPanel->FreeOwnedChildren();
        MainPanel->OnClose();
        LoadPanel->OnClose();
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnClose();
        } else {
            fPanelRuins::TfPanelRuins::OnClose();
        }
    }

    void TfInfo::RefreshNewsAnimation(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        std::int32_t Weight = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 19); cpp_range.next(I); ) {
            Weight += InfoNewsAnimationWeights[I] + 1;
        }
        Weight = aMyFunction::RandomIntRange(0, Weight - 1);
        I = 0;
        while (I < 19) {
            Weight -= InfoNewsAnimationWeights[I] + 1;
            if (Weight < 0) {
                break;
            }
            ++I;
        }
        {
            GI_GAI::TgaiGI* TV = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"TV"sv));
            if (SearchMode) {
                TV->SetFirstFrameImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), u"FindI"}));
                TV->SetImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), u"FindA"}));
                {
                    const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"[75,0-", SysUtils::IntToStr(TV->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* cpp_arg_2 = TV;
                    cpp_arg_2->LoadFrameSequenceFromText(cpp_arg);
                }
            } else {
                TV->SetFirstFrameImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), InfoNewsAnimationNames[I], u"i"}));
                TV->SetImagePath(pas::concat_wide({u"Bm.News.", GR_Main::GiResourceSuffix(), InfoNewsAnimationNames[I], u"a"}));
                {
                    const pas::WideString& cpp_arg_3 = static_cast<pas::WideString>(pas::concat_ansi({"[75,0-", SysUtils::IntToStr(TV->GetMainImageFrameCount() - 1), "]"}));
                    GI_GAI::TgaiGI* cpp_arg_4 = TV;
                    cpp_arg_4->LoadFrameSequenceFromText(cpp_arg_3);
                }
            }
            TV->UpdateAutoGeometry();
            TV->CycleCompleteCallback = pas::bind_method<&TfInfo::RefreshNewsAnimation>(this);
            TV->RestartPlayback();
        }
    }

    void TfInfo::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
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
        if (SearchMode) {
            ToggleSearchMode(nullptr);
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(203);
        fEquipmentShop::RestoreTemporaryShopStock();
        MainPanel->EndTurnClicked(Sender);
        MainPanel->RebuildMessageButtons(false);
        if (ExitCode == 0) {
            fEquipmentShop::BuildTemporaryShopSlotGrid();
            ShowNews();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(204);
        }
    }

    void TfInfo::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        MainPanel->ShipClicked(Sender);
        if (Globals::ShipScreen->ShipStateChanged) {
            MainPanel->RebuildMessageButtons(false);
            MainPanel->RefreshMoneyAndCargo();
        }
    }

    void TfInfo::ClearInfoContents() {
        InfoContentHeight = 0;
        InfoPanel->FreeOwnedChildren();
        InfoPanel->SetScrollOffset(ClassesImports::Point(0, 0));
        InfoPanel->Invalidate();
    }

    void TfInfo::FinishInfoLayout() {
        InfoPanel->UpdateScrollRanges();
        InfoPanel->VerticalScrollBar->SetRange(0, InfoPanel->VerticalScrollBar->Maximum);
        InfoPanel->VerticalScrollBar->SetActive(InfoPanel->ClientSize.Y < InfoContentHeight);
        {
            std::int32_t lineHeight = pas::checked_cast<GI_Label::TLabelGI*>(Globals::GovernmentScreen->GetByName(u"TalkText"sv))->GetLineHeight();
            GI_ScrollBar::TScrollBarGI* verticalScrollBar = InfoPanel->VerticalScrollBar;
            verticalScrollBar->SetSmallChange(lineHeight);
        }
        InfoPanel->VerticalScrollBar->SetLargeChange(InfoPanel->ClientSize.Y);
        InfoPanel->VerticalScrollBar->SetPageSize(InfoPanel->ClientSize.Y);
        InfoPanel->SetScrollOffset(ClassesImports::Point(0, 0));
        InfoPanel->Invalidate();
    }

    void TfInfo::AddInfoSpacing(std::int32_t Pixels) {
        InfoContentHeight += GR_Main::GiScalePixels(Pixels);
    }

    void TfInfo::AddInfoSeparator() {
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, InfoPanel);
        Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"Line"}));
        Image->SetSize(Image->GetContentSize());
        Image->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X / 2 - Image->ClientSize.X / 2, InfoContentHeight));
        Image->SetDepth(1.0);
        Image->SetImageKindX(GI_Main::ikxLeftFill);
        Image->SetImageKindY(GI_Main::ikyCenter);
        InfoContentHeight += Image->ClientSize.Y;
        Image->SetPositionModeW(true);
    }

    // LayoutKind zero centers the heading; nonzero aligns it to the right.
    void TfInfo::AddInfoHeading(pas::WideString Title, pas::WideString BookmarkText, std::int32_t LayoutKind, std::int32_t BookmarkIndex, std::int32_t GoodsReference) {
        GI_GraphButton::TGraphButtonGI* Button{};
        Title = EC_Str::ReplaceAllWideString(Title, u"<color=255,240,100>"_wref.get(), u"<color=0,0,0>"sv);
        Title = EC_Str::ReplaceAllWideString(Title, u"<color=0,255,0>"_wref.get(), u"<color=255,255,0>"sv);
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, InfoPanel);
        if (LayoutKind == 0) {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"CaptionL"}));
        } else {
            Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"Caption"}));
        }
        Image->SetSize(Image->GetContentSize());
        if (LayoutKind == 0) {
            Image->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X / 2 - Image->ClientSize.X / 2, InfoContentHeight));
        } else {
            Image->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X - Image->ClientSize.X, InfoContentHeight));
        }
        Image->SetDepth(1.0);
        Image->SetImageKindX(GI_Main::ikxLeftFill);
        Image->SetImageKindY(GI_Main::ikyCenter);
        InfoContentHeight += Image->ClientSize.Y;
        Image->SetPositionModeW(true);
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        if (GlobalsV::FontDialog == 0 || SearchMode) {
            Caption->SetFontName(GlobalsV::NormalFontName);
        } else if (GlobalsV::FontDialog == 1) {
            Caption->SetFontName(GlobalsV::SmoothBigFontName);
        } else if (GlobalsV::FontDialog == 2) {
            Caption->SetFontName(GlobalsV::SmoothHugeFontName);
        } else if (GlobalsV::FontDialog >= 3) {
            Caption->SetFontName(GlobalsV::SmoothIntroFontName);
        }
        Caption->SetDepth(-1.0);
        Caption->SetSize(Image->ClientSize);
        if (LayoutKind != 0) {
            Caption->SetSize(ClassesImports::Point(Caption->ClientSize.X - GR_Main::GiScalePixels(20), Caption->ClientSize.Y));
        }
        Caption->SetPosition(ClassesImports::Point(Image->LocalPosition.X + Image->ClientSize.X - Caption->ClientSize.X, Image->LocalPosition.Y));
        Caption->SetWordWrapEnabled(false);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(GI_Main::taxCenter);
        Caption->SetTextAlignY(GI_Main::tayCenterEx);
        Caption->SetText(Title);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        if (BookmarkText != u"") {
            Button = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, InfoPanel);
            Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemN"}));
            Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemA"}));
            Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemD"}));
            Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"MemH"}));
            Button->SetSize(Button->GetMaxStateImageSize());
            if (LayoutKind == 0) {
                Button->SetPosition(ClassesImports::Point(Image->LocalPosition.X + Image->ClientSize.X - Button->ClientSize.X - GR_Main::GiScalePixels(10), Image->LocalPosition.Y));
            } else {
                Button->SetPosition(ClassesImports::Point(Image->LocalPosition.X + Image->ClientSize.X - Button->ClientSize.X - GR_Main::GiScalePixels(10), Image->LocalPosition.Y - GR_Main::GiScalePixels(5)));
            }
            Button->SetKind(GI_GraphButton::gbkDisable);
            Button->SetPositionModeW(true);
            Button->HelpText = BookmarkText;
            Button->UpCallback = pas::bind_method<&TfInfo::BookmarkClicked>(this);
            Button->SetName(static_cast<pas::WideString>(pas::concat_ansi({"MemBtn", SysUtils::IntToStr(BookmarkIndex)})));
            Button->SetDown(false);
            Button->SetDisabled(Globals::FindPlayerBubbleByText(BookmarkText, false) != nullptr);
            Button->UserValue = GoodsReference;
            Button->SetHovered(true);
            Button->Invalidate();
            Button->SetHovered(false);
        }
    }

    void TfInfo::AddSearchPriceLabel(pas::WideString Text) {
        GI_Image::TImageGI* Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, InfoPanel);
        Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormInfo2.", GR_Main::GiResourceSuffix(), u"FindPrice"}));
        Image->SetSize(Image->GetContentSize());
        Image->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X - Image->ClientSize.X - GR_Main::GiScalePixels(40), InfoContentHeight));
        Image->SetDepth(1.0);
        Image->SetImageKindX(GI_Main::ikxLeftFill);
        Image->SetImageKindY(GI_Main::ikyCenter);
        InfoContentHeight += Image->ClientSize.Y;
        Image->SetPositionModeW(true);
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetDepth(-1.0);
        Caption->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - (InfoPanel->ClientSize.X - Image->LocalPosition.X) - GR_Main::GiScalePixels(10), Image->ClientSize.Y));
        Caption->SetPosition(ClassesImports::Point(0, Image->LocalPosition.Y));
        Caption->SetWordWrapEnabled(false);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(GI_Main::taxRight);
        Caption->SetTextAlignY(GI_Main::tayCenterEx);
        Caption->SetText(Text);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
    }

    void TfInfo::AddInfoText(pas::WideString Text, GI_Main::TTextAlignXGI Alignment, pas::WideString Font) {
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        if (Font == u"") {
            if (GlobalsV::FontDialog == 0) {
                Caption->SetFontName(GlobalsV::NormalFontName);
            } else if (GlobalsV::FontDialog == 1) {
                Caption->SetFontName(GlobalsV::SmoothBigFontName);
            } else if (GlobalsV::FontDialog == 2) {
                Caption->SetFontName(GlobalsV::SmoothHugeFontName);
            } else if (GlobalsV::FontDialog >= 3) {
                Caption->SetFontName(GlobalsV::SmoothIntroFontName);
            }
        } else {
            Caption->SetFontName(Font);
        }
        Caption->SetPosition(ClassesImports::Point(0, InfoContentHeight));
        Caption->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X, 1));
        Caption->SetWordWrapEnabled(true);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(Alignment);
        Caption->SetTextAlignY(GI_Main::tayAuto);
        Caption->SetText(Text);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        InfoContentHeight += Caption->ClientSize.Y;
    }

    void TfInfo::AddInfoImageText(pas::WideString ImagePath, pas::WideString Text) {
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
        std::int32_t Size = GR_Main::GiScalePixels(64);
        GI_GraphBuf::TGraphBufGI* Image = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, InfoPanel, false);
        Image->SetPositionModeW(true);
        Image->SetPosition(ClassesImports::Point(0, InfoContentHeight));
        Image->SetSize(ClassesImports::Point(Size, Size));
        Image->SourceHasPerPixelAlpha = true;
        GI_GI::LoadGiByPathIntoGraphBuf(ImagePath, Image->GraphBuf);
        if (static_cast<std::uint32_t>(Image->GraphBuf->Width) >= static_cast<std::uint32_t>(Image->GraphBuf->Height)) {
            Image->GraphBuf->RescaleRgba(Image->ClientSize.X, System::Round(pas::real_divide(Image->ClientSize.X, static_cast<std::uint32_t>(Image->GraphBuf->Width)) * static_cast<std::uint32_t>(Image->GraphBuf->Height)), 5);
        } else {
            Image->GraphBuf->RescaleRgba(System::Round(pas::real_divide(Image->ClientSize.Y, static_cast<std::uint32_t>(Image->GraphBuf->Height)) * static_cast<std::uint32_t>(Image->GraphBuf->Width)), Image->ClientSize.Y, 5);
        }
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetPosition(ClassesImports::Point(Size + 10, InfoContentHeight));
        Caption->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - Caption->LocalPosition.X, Size));
        Caption->SetWordWrapEnabled(true);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(GI_Main::taxLeft);
        Caption->SetTextAlignY(GI_Main::tayAuto);
        Caption->SetText(Text);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        Size = std::max<std::int32_t>(Size, Caption->ClientSize.Y);
        InfoContentHeight = InfoContentHeight + Size + 5;
    }

    void TfInfo::AddPlanetInfoText(aPlanet::TPlanet* Planet, pas::WideString Text) {
        GI_Image::TImageGI* Emblem{};
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
        std::int32_t Size = GR_Main::GiScalePixels(64);
        GI_GraphBuf::TGraphBufGI* Image = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, InfoPanel, false);
        Image->SetPositionModeW(true);
        Image->SetPosition(ClassesImports::Point(0, InfoContentHeight));
        Image->SetSize(ClassesImports::Point(Size, Size));
        Image->SourceHasPerPixelAlpha = true;
        Planet->Graphic->RenderToBuffer(this, Image->GraphBuf, false);
        if (static_cast<std::uint32_t>(Image->GraphBuf->Width) >= static_cast<std::uint32_t>(Image->GraphBuf->Height)) {
            Image->GraphBuf->RescaleRgba(Image->ClientSize.X, System::Round(pas::real_divide(Image->ClientSize.X, static_cast<std::uint32_t>(Image->GraphBuf->Width)) * static_cast<std::uint32_t>(Image->GraphBuf->Height)), 5);
        } else {
            Image->GraphBuf->RescaleRgba(System::Round(pas::real_divide(Image->ClientSize.Y, static_cast<std::uint32_t>(Image->GraphBuf->Height)) * static_cast<std::uint32_t>(Image->GraphBuf->Width)), Image->ClientSize.Y, 5);
        }
        if (Planet->OwnerId != aGalaxyStruct::oiUninhabited) {
            Emblem = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, InfoPanel);
            Emblem->SetPositionModeW(true);
            Emblem->SetImagePath(aConst::GetFactionEmblemPath(Planet->GetFactionResourceName()));
            Emblem->SetSize(Emblem->GetContentSize());
            Emblem->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X - Emblem->ClientSize.X - GR_Main::GiScalePixels(10), InfoContentHeight));
            Emblem->SetImageKindX(GI_Main::ikxCenter);
            Emblem->SetImageKindY(GI_Main::ikyCenter);
        }
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetPosition(ClassesImports::Point(Size + 10, InfoContentHeight));
        Caption->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - Caption->LocalPosition.X, Size));
        Caption->SetWordWrapEnabled(true);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(GI_Main::taxLeft);
        Caption->SetTextAlignY(GI_Main::tayAuto);
        Caption->SetText(Text);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        Size = std::max<std::int32_t>(Size, Caption->ClientSize.Y);
        Caption->SetTextAlignY(GI_Main::tayCenter);
        Caption->SetSize(ClassesImports::Point(Caption->ClientSize.X, Size));
        InfoContentHeight = InfoContentHeight + Size + 5;
    }

    void TfInfo::AddItemInfoText(aItem::TItem* Item, pas::WideString Text) {
        GI_Image::TImageGI* Emblem{};
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=0,255,0>"_wref.get(), u"<color=0,130,0>"sv);
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,167,84>"_wref.get(), u"<color=240,100,30>"sv);
        std::int32_t Size = GR_Main::GiScalePixels(64);
        GI_GraphBuf::TGraphBufGI* Image = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, InfoPanel, false);
        Image->SetPositionModeW(true);
        Image->SetPosition(ClassesImports::Point(0, InfoContentHeight));
        Image->SetSize(ClassesImports::Point(Size, Size));
        Image->SourceHasPerPixelAlpha = true;
        {
            const pas::WideString& cpp_arg = pas::concat_wide({Item->GetBitmapResourceName(), u"i"});
            GR_GraphBuf::TGraphBufGR* graphBuf = Image->GraphBuf;
            GI_GI::LoadGiByPathIntoGraphBuf(cpp_arg, graphBuf);
        }
        if (static_cast<std::uint32_t>(Image->GraphBuf->Width) >= static_cast<std::uint32_t>(Image->GraphBuf->Height)) {
            Image->GraphBuf->RescaleRgba(Image->ClientSize.X, System::Round(pas::real_divide(Image->ClientSize.X, static_cast<std::uint32_t>(Image->GraphBuf->Width)) * static_cast<std::uint32_t>(Image->GraphBuf->Height)), 5);
        } else {
            Image->GraphBuf->RescaleRgba(System::Round(pas::real_divide(Image->ClientSize.Y, static_cast<std::uint32_t>(Image->GraphBuf->Height)) * static_cast<std::uint32_t>(Image->GraphBuf->Width)), Image->ClientSize.Y, 5);
        }
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetPosition(ClassesImports::Point(Size + 10, InfoContentHeight));
        Caption->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - Caption->LocalPosition.X, Size + 20));
        Caption->SetWordWrapEnabled(true);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(GI_Main::taxLeft);
        Caption->SetTextAlignY(GI_Main::tayCenter);
        Caption->SetText(Text);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr) {
            Emblem = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, InfoPanel);
            Emblem->SetPositionModeW(true);
            Emblem->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(pas::checked_cast<aItem::TEquipment*>(Item))));
            Emblem->SetSize(Emblem->GetContentSize());
            Emblem->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X - Emblem->ClientSize.X - GR_Main::GiScalePixels(10), InfoContentHeight));
            Emblem->SetImageKindX(GI_Main::ikxCenter);
            Emblem->SetImageKindY(GI_Main::ikyCenter);
        }
        InfoContentHeight = InfoContentHeight + Size + 21;
    }

    void TfInfo::AddStarInfoText(aGalaxy::TStar* Star, pas::WideString Text) {
        GI_Image::TImageGI* Emblem{};
        // Nested in AddStarInfoText; static link unused.
        auto GetInfoStarFactionName = [&](aGalaxy::TStar* Star) -> pas::WideString {
            if (Star->Status.CustomFaction != u"") {
                return Star->Status.CustomFaction;
            } else if (Star->Status.ControlFaction == aGalaxyStruct::sfDominators) {
                return aConst::DominatorSeriesNames[Star->Status.DominatorSeries];
            } else if (Star->Status.ControlFaction == aGalaxyStruct::sfPirates) {
                return aConst::OwnerInfo[aGalaxyStruct::oiPirate].InternalName;
            } else {
                return aConst::OwnerInfo[aGalaxyStruct::oiUninhabited].InternalName;
            }
        };
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
        std::int32_t Size = GR_Main::GiScalePixels(64);
        GI_GraphBuf::TGraphBufGI* Image = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, InfoPanel, false);
        Image->SetPositionModeW(true);
        Image->SetPosition(ClassesImports::Point(0, InfoContentHeight));
        Image->SetSize(ClassesImports::Point(Size, Size));
        Image->SourceHasPerPixelAlpha = true;
        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(reinterpret_cast<SE_Star::TStarSE*>(Star->Graphic)->StaticImagePath), 1, u","sv), Image->GraphBuf);
        if (static_cast<std::uint32_t>(Image->GraphBuf->Width) >= static_cast<std::uint32_t>(Image->GraphBuf->Height)) {
            Image->GraphBuf->RescaleRgba(Image->ClientSize.X, System::Round(pas::real_divide(Image->ClientSize.X, static_cast<std::uint32_t>(Image->GraphBuf->Width)) * static_cast<std::uint32_t>(Image->GraphBuf->Height)), 5);
        } else {
            Image->GraphBuf->RescaleRgba(System::Round(pas::real_divide(Image->ClientSize.Y, static_cast<std::uint32_t>(Image->GraphBuf->Height)) * static_cast<std::uint32_t>(Image->GraphBuf->Width)), Image->ClientSize.Y, 5);
        }
        if (Star->Status.ControlFaction != aGalaxyStruct::sfCoalition || Star->Status.CustomFaction != u"") {
            Emblem = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, InfoPanel);
            Emblem->SetPositionModeW(true);
            Emblem->SetImagePath(aConst::GetFactionEmblemPath(GetInfoStarFactionName(Star)));
            Emblem->SetSize(Emblem->GetContentSize());
            Emblem->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X - Emblem->ClientSize.X - GR_Main::GiScalePixels(10), InfoContentHeight));
            Emblem->SetImageKindX(GI_Main::ikxCenter);
            Emblem->SetImageKindY(GI_Main::ikyCenter);
        }
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        Caption->SetFontName(GlobalsV::NormalFontName);
        Caption->SetPosition(ClassesImports::Point(Size + 10, InfoContentHeight));
        Caption->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - Caption->LocalPosition.X, Size));
        Caption->SetWordWrapEnabled(true);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(GI_Main::taxLeft);
        Caption->SetTextAlignY(GI_Main::tayAuto);
        Caption->SetText(Text);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        Size = std::max<std::int32_t>(Size, Caption->ClientSize.Y);
        Caption->SetTextAlignY(GI_Main::tayCenter);
        Caption->SetSize(ClassesImports::Point(Caption->ClientSize.X, Size));
        InfoContentHeight = InfoContentHeight + Size + 5;
    }

    void TfInfo::FocusSearchField(std::uint8_t Force) {
        if (SearchMode) {
            if (SelectedSearchCategory == 0) {
                SetFocusedControl(GetByName(u"TextSearch"sv));
            } else if (FocusedControl == nullptr || Force) {
                if (SelectedSearchCategory == 1) {
                    SetFocusedControl(GetByName(u"M01Speed"sv));
                } else if (SelectedSearchCategory == 2) {
                    SetFocusedControl(GetByName(u"M02Capacity"sv));
                } else if (SelectedSearchCategory == 3) {
                    SetFocusedControl(GetByName(u"M03Range"sv));
                } else if (SelectedSearchCategory == 4) {
                    SetFocusedControl(GetByName(u"M04Power"sv));
                } else if (SelectedSearchCategory == 5) {
                    SetFocusedControl(GetByName(u"M05Power"sv));
                } else if (SelectedSearchCategory == 6) {
                    SetFocusedControl(GetByName(u"M06ObjSize"sv));
                } else if (SelectedSearchCategory == 7) {
                    SetFocusedControl(GetByName(u"M07Block"sv));
                } else if (SelectedSearchCategory == 8) {
                    SetFocusedControl(GetByName(u"M08Range"sv));
                } else if (SelectedSearchCategory == 9) {
                    SetFocusedControl(GetByName(u"M09Const"sv));
                } else if (SelectedSearchCategory == 10) {
                    SetFocusedControl(GetByName(u"M10Const"sv));
                } else if (SelectedSearchCategory == 11) {
                    SetFocusedControl(GetByName(u"M11Size"sv));
                } else if (SelectedSearchCategory == 12) {
                    SetFocusedControl(GetByName(u"M12DamageMin"sv));
                } else if (SelectedSearchCategory == 13) {
                    SetFocusedControl(GetByName(u"M13Range"sv));
                } else if (SelectedSearchCategory == 15) {
                    SetFocusedControl(GetByName(u"M15Const"sv));
                }
            }
        } else {
            SetFocusedControl(nullptr);
        }
    }

    void TfInfo::ToggleSearchMode(GI_MessageLoop::TObjectGI* Sender) {
        SearchMode = static_cast<std::uint8_t>(SearchMode ^ 1);
        RefreshNewsAnimation(nullptr);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButSearch"sv))->SetDisabled(SearchMode);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNews"sv))->SetDisabled(static_cast<std::uint8_t>(SearchMode ^ 1));
        GetByName(u"PanelSearch"sv)->SetActive(SearchMode);
        FocusSearchField(true);
        if (!SearchMode) {
            ShowNews();
        } else {
            ShowSearch();
        }
    }

    void TfInfo::MainPanelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        FocusSearchField(false);
    }

    void TfInfo::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        std::int32_t NewPosition{};
        Windows::TMsg Msg{};
        if (Key == WindowsSdk::VK_ESCAPE && ParentLoop != nullptr) {
            RequestClose(1);
        } else if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
            if (Key == 'C') {
                if (SelectedSearchCategory == 12) {
                    CopySearch12Name(nullptr);
                } else if (SelectedSearchCategory == 15) {
                    CopySearch15Name(nullptr);
                } else {
                    CopySearchText(nullptr);
                }
            } else if (Key == 'V') {
                if (SelectedSearchCategory == 12) {
                    PasteSearch12Name(nullptr);
                } else if (SelectedSearchCategory == 15) {
                    PasteSearch15Name(nullptr);
                } else {
                    PasteSearchText(nullptr);
                }
            } else if (Key == WindowsSdk::VK_BACK) {
                if (SelectedSearchCategory == 12) {
                    ClearSearch12Name(nullptr);
                } else if (SelectedSearchCategory == 15) {
                    ClearSearch15Name(nullptr);
                } else {
                    ClearSearchText(nullptr);
                }
            }
        } else if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1) && (static_cast<std::uint8_t>(SearchMode ^ 1) || Key < 'A' || Key > 'Z')) {
            if (TfInfo::IsAtBusinessCenter() && Key == WindowsSdk::VK_INSERT) {
                ToggleVisibleBookmark();
            }
            NewPosition = InfoPanel->VerticalScrollBar->Position;
            if (Key == WindowsSdk::VK_PRIOR) {
                NewPosition -= InfoPanel->VerticalScrollBar->LargeChange;
            }
            if (Key == WindowsSdk::VK_NEXT) {
                NewPosition += InfoPanel->VerticalScrollBar->LargeChange;
            }
            if (Key == WindowsSdk::VK_UP) {
                NewPosition -= InfoPanel->VerticalScrollBar->SmallChange;
            }
            if (Key == WindowsSdk::VK_DOWN) {
                NewPosition += InfoPanel->VerticalScrollBar->SmallChange;
            }
            if (NewPosition < InfoPanel->VerticalScrollBar->Minimum) {
                NewPosition = InfoPanel->VerticalScrollBar->Minimum;
            }
            if (NewPosition > InfoPanel->VerticalScrollBar->Maximum) {
                NewPosition = InfoPanel->VerticalScrollBar->Maximum;
            }
            InfoPanel->VerticalScrollBar->SetPosition_2(NewPosition);
            if (Key == WindowsSdk::VK_HOME) {
                if (!SearchMode) {
                    InfoPanel->SetScrollOffset(ClassesImports::Point(0, 0));
                }
            } else if (Key == WindowsSdk::VK_SPACE) {
                if (static_cast<std::uint8_t>(SearchMode ^ 1) && GetByName(u"PM_EndTurn"sv)->Active) {
                    EndTurnClicked(nullptr);
                }
            } else if (Key == WindowsSdk::VK_RETURN && SearchMode) {
                SearchClicked(nullptr);
            } else if (Key == 'F' && static_cast<std::uint8_t>(SearchMode ^ 1)) {
                ToggleSearchMode(nullptr);
            } else if (Key == 'I' && static_cast<std::uint8_t>(SearchMode ^ 1)) {
                WindowsSdk::PeekMessage(Msg, 0u, MessagesSdk::WM_CHAR, MessagesSdk::WM_CHAR, WindowsSdk::PM_REMOVE);
                ToggleSearchMode(nullptr);
            } else if (Key == 'S') {
                ShipClicked(nullptr);
            } else {
                MainPanel->ProcessKeyDown(Key);
                PlanetPanel->ProcessKeyDown(Key);
                StationPanel->ProcessKeyDown(Key);
            }
        }
    }

    void TfInfo::MainPanelKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
    }

    void TfInfo::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        if (Delta == WindowsSdk::WHEEL_DELTA) {
            InfoPanel->VerticalScrollBar->SetPosition_2(InfoPanel->VerticalScrollBar->Position - InfoPanel->VerticalScrollBar->SmallChange * 5);
        } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
            InfoPanel->VerticalScrollBar->SetPosition_2(InfoPanel->VerticalScrollBar->Position + InfoPanel->VerticalScrollBar->SmallChange * 5);
        }
    }

    void TfInfo::BookmarkClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::WideString Key{};
        GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
        if (Sender->UserValue != 0) {
            Key = static_cast<pas::WideString>(pas::concat_ansi({"GOODS ", SysUtils::Int64ToStr(static_cast<std::uint32_t>(Sender->UserValue))}));
        } else {
            Key = pas::WideString();
        }
        Globals::AddOrUpdatePlayerBubble(7, aGalaxy::Galaxy->CurrentTurn, Sender->HelpText, Key);
        MainPanel->RebuildMessageButtons(false);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Sender)->SetDisabled(true);
        GI_Main::BreakUiMessage();
    }

    void TfInfo::SelectMusic() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId)].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                }
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
            }
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
                return;
            }
            if (pas::in_set<7, 7, 12, 12>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace)].InternalName}));
            }
        }
    }

    void TfInfo::ShowNews() {
        std::int32_t I{};
        aGalaxy::PPlanetNewsEntry Entry{};
        pas::WideString Source{};
        InfoPanel->SetActive(true);
        InfoPanel->VerticalScrollBar->SetActive(true);
        {
            GI_GraphButton::TGraphButtonGI* ButPrev = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPrev"sv));
            ButPrev->SetActive(false);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButNext = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNext"sv));
            ButNext->SetActive(false);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 15); cpp_range.next(I); ) {
            GI_MessageLoop::TObjectGI* cpp_with_3 = GetByName(pas::view(pas::concat_wide({u"PanelM", EC_Str::IntToFixedWidthWideString(I, 2)})));
            cpp_with_3->SetActive(false);
        }
        ClearInfoContents();
        AddInfoSpacing(10);
        for (auto cpp_range_2 = pas::for_downto<std::int32_t>(pas::list_count(aGalaxy::Galaxy->PlanetNews) - 1, 0); cpp_range_2.next(I); ) {
            Entry = pas::list_at<aGalaxyStruct::TPlanetNews>(aGalaxy::Galaxy->PlanetNews, I);
            // Native retains this empty local string in the heading expression.
            {
                pas::WideString cpp_arg = pas::concat_wide({aMyFunction::WrapTextInColor(pas::view(aGalaxy::Galaxy->FormatTurnDate(Entry->Turn)), u"<color=255,240,100>"sv), u"\r\n", u" ", u"\r\n", Entry->Text});
                pas::WideString wrapTextInColor = aMyFunction::WrapTextInColor(pas::view(pas::concat_wide({aGalaxy::Galaxy->FormatTurnDate(Entry->Turn), Source})), u"<color=255,240,100>"sv);
                AddInfoHeading(std::move(wrapTextInColor), std::move(cpp_arg), 1, 0, 0);
            }
            AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
            AddInfoText(Entry->Text, GI_Main::taxAuto, pas::WideString());
            AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
            AddInfoSpacing(10);
        }
        FinishInfoLayout();
    }

    void TfInfo::ShowSearch() {
        std::int32_t I{};
        SelectedSearchCategory = 0;
        PreviousSearchCategory = 0;
        HasSearchResults = false;
        InfoPanel->SetActive(false);
        InfoPanel->VerticalScrollBar->SetActive(false);
        {
            GI_GraphButton::TGraphButtonGI* ButPrev = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPrev"sv));
            ButPrev->UpCallback = pas::bind_method<&TfInfo::CategoryClicked>(this);
            ButPrev->SetActive(true);
            ButPrev->SetDisabled(true);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButNext = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNext"sv));
            ButNext->UpCallback = pas::bind_method<&TfInfo::NextSearchPageClicked>(this);
            ButNext->SetActive(true);
            ButNext->SetDisabled(true);
        }
        GetByName(u"PanelSearch"sv)->SetActive(true);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 15); cpp_range.next(I); ) {
            {
                GI_MessageLoop::TObjectGI* cpp_with_3 = GetByName(pas::view(pas::concat_wide({u"PanelM", EC_Str::IntToFixedWidthWideString(I, 2)})));
                cpp_with_3->SetActive(false);
            }
            if (I != 14) {
                GI_GraphButton::TGraphButtonGI* cpp_with_4 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(I, 2), u"Search"}))));
                cpp_with_4->UpCallback = pas::bind_method<&TfInfo::SearchClicked>(this);
            }
        }
        GetByName(u"ImgMM_C1"sv)->SetActive(false);
        GetByName(u"ImgMM_C2"sv)->SetActive(false);
        ClearInfoContents();
        FinishInfoLayout();
    }

    void TfInfo::SearchClicked(GI_MessageLoop::TObjectGI* Sender) {
        RunSearch(Sender);
    }

    void TfInfo::RunSearch(GI_MessageLoop::TObjectGI* Sender) {
        aShip::TShip* Ship{};
        std::int32_t ResultCount{};
        pas::List* Shown{};
        std::uint8_t Found{};
        aRuins::TRuins* Station{};
        pas::WideString Description{};
        pas::WideString Heading{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TStar* Star{};
        std::int32_t Bearing{};
        std::int32_t MinSpeed{};
        std::int32_t RangeFilter{};
        std::int32_t SizeFilter{};
        std::int32_t MaxCost{};
        aGalaxyStruct::TOwnerMask Owners{};
        std::int32_t MinFuel{};
        std::int32_t MinPower{};
        std::int32_t MinPickup{};
        std::int32_t MinDefense{};
        std::uint8_t IncludeDominators{};
        std::uint8_t IncludeCoalition{};
        std::uint8_t IncludePirates{};
        std::uint8_t IncludeUninhabited{};
        pas::WideString ConstellationFilter{};
        pas::WideString StarFilter{};
        aGalaxyStruct::TShipTypeMask StationTypes{};
        std::uint8_t IncludeRangerType{};
        std::uint8_t IncludeWarriorType{};
        std::uint8_t IncludePirateType{};
        std::uint8_t IncludeTransportType{};
        std::uint8_t IncludeLinerType{};
        std::uint8_t IncludeDiplomatType{};
        std::int32_t MinArmor{};
        std::int32_t WeaponSlots{};
        std::int32_t ScannerSlots{};
        std::int32_t RadarSlots{};
        std::int32_t DroidSlots{};
        std::int32_t HookSlots{};
        std::int32_t DefenseSlots{};
        std::int32_t ArtifactSlots{};
        std::int32_t AfterburnerSlots{};
        std::uint8_t IncludeEnergy{};
        std::uint8_t IncludeSplinter{};
        std::uint8_t IncludeMissile{};
        std::int32_t MinDamage{};
        std::int32_t MaxDamage{};
        pas::WideString NameFilter{};
        std::int32_t I{};
        std::int32_t Index{};
        std::int32_t ItemIndex{};
        aItem::TItem* Item{};
        pas::WideString SearchText{};
        std::uint8_t GoodsIndex{};
        GI_MessageLoop::TObjectGI* Control{};
        std::uint8_t StationType{};
        std::int32_t MinGoodsCount{};
        std::int32_t MinSellPrice{};
        std::int32_t MaxBuyPrice{};
        pas::Array<std::uint8_t, 0, 7> GoodsSelected{};
        std::uint8_t Good{};
        // Nested in RunSearch; applies the active category filters.
        auto CheckInfoSearchResult = [&](pas::Object* Value) -> void {
            aItem::TEngine* Engine{};
            aItem::TFuelTanks* Fuel{};
            aItem::TRadar* Radar{};
            aItem::TScaner* Scanner{};
            aItem::TRepairRobot* Droid{};
            aItem::TCargoHook* Hook{};
            aItem::TDefGenerator* Defense{};
            aGalaxy::TStar* CandidateStar{};
            aPlanet::TPlanet* CandidatePlanet{};
            aRuins::TRuins* CandidateStation{};
            aItem::THull* Hull{};
            aItem::TWeapon* Weapon{};
            aShip::TShip* CandidateShip{};
            std::int32_t I{};
            std::int32_t EffectiveRange{};
            aItem::PExtraSpecial Entry{};
            if (this->SelectedSearchCategory == 1) {
                if (aItem::TEngine* engine = pas::class_cast_if<aItem::TEngine*>(Value)) {
                    Engine = engine;
                    if ((MinSpeed == 0 || MinSpeed <= Engine->Speed) && (RangeFilter == 0 || Engine->JumpRange >= RangeFilter) && (SizeFilter == 0 || SizeFilter >= Engine->Weight) && (MaxCost == 0 || Engine->GetConditionAdjustedCost() <= MaxCost) && pas::contains(Owners, Engine->OwnerId)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 2) {
                if (aItem::TFuelTanks* fuelTanks = pas::class_cast_if<aItem::TFuelTanks*>(Value)) {
                    Fuel = fuelTanks;
                    if ((MinFuel == 0 || Fuel->Capacity >= MinFuel) && (SizeFilter == 0 || Fuel->Weight <= SizeFilter) && (MaxCost == 0 || Fuel->GetConditionAdjustedCost() <= MaxCost) && pas::contains(Owners, Fuel->OwnerId)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 3) {
                if (aItem::TRadar* radar = pas::class_cast_if<aItem::TRadar*>(Value)) {
                    Radar = radar;
                    if ((RangeFilter == 0 || Radar->Range >= RangeFilter) && (SizeFilter == 0 || Radar->Weight <= SizeFilter) && (MaxCost == 0 || Radar->GetConditionAdjustedCost() <= MaxCost) && pas::contains(Owners, Radar->OwnerId)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 4) {
                if (aItem::TScaner* scaner = pas::class_cast_if<aItem::TScaner*>(Value)) {
                    Scanner = scaner;
                    if ((MinPower == 0 || Scanner->ScanPower >= MinPower) && (SizeFilter == 0 || Scanner->Weight <= SizeFilter) && (MaxCost == 0 || Scanner->GetConditionAdjustedCost() <= MaxCost) && pas::contains(Owners, Scanner->OwnerId)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 5) {
                if (aItem::TRepairRobot* repairRobot = pas::class_cast_if<aItem::TRepairRobot*>(Value)) {
                    Droid = repairRobot;
                    if ((MinPower == 0 || Droid->RepairPoints >= MinPower) && (SizeFilter == 0 || Droid->Weight <= SizeFilter) && (MaxCost == 0 || Droid->GetConditionAdjustedCost() <= MaxCost) && pas::contains(Owners, Droid->OwnerId)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 6) {
                if (aItem::TCargoHook* cargoHook = pas::class_cast_if<aItem::TCargoHook*>(Value)) {
                    Hook = cargoHook;
                    if ((MinPickup == 0 || Hook->PickupPower >= MinPickup) && (SizeFilter == 0 || Hook->Weight <= SizeFilter) && (MaxCost == 0 || Hook->GetConditionAdjustedCost() <= MaxCost) && pas::contains(Owners, Hook->OwnerId)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 7) {
                if (aItem::TDefGenerator* defGenerator = pas::class_cast_if<aItem::TDefGenerator*>(Value)) {
                    Defense = defGenerator;
                    if ((MinDefense == 0 || aItem::DefenseDamageFactorToPercent(Defense->DamageFactor) >= MinDefense) && (SizeFilter == 0 || Defense->Weight <= SizeFilter) && (MaxCost == 0 || Defense->GetConditionAdjustedCost() <= MaxCost) && pas::contains(Owners, Defense->OwnerId)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 8) {
                if (aGalaxy::TStar* star = pas::class_cast_if<aGalaxy::TStar*>(Value)) {
                    CandidateStar = star;
                    if (CandidateStar->Status.ControlFaction == aGalaxyStruct::sfDominators && static_cast<std::uint8_t>(IncludeDominators ^ 1)) {
                        return;
                    }
                    if (CandidateStar->Status.ControlFaction == aGalaxyStruct::sfCoalition && static_cast<std::uint8_t>(IncludeCoalition ^ 1)) {
                        return;
                    }
                    if (CandidateStar->Status.ControlFaction == aGalaxyStruct::sfPirates && static_cast<std::uint8_t>(IncludePirates ^ 1)) {
                        return;
                    }
                    if (CandidateStar->Status.CustomFaction != u"" && (static_cast<std::uint8_t>(IncludeDominators ^ 1) || static_cast<std::uint8_t>(IncludeCoalition ^ 1) || static_cast<std::uint8_t>(IncludePirates ^ 1))) {
                        return;
                    }
                    if (RangeFilter != 0 && RangeFilter < System::Round(aMyFunction::PointDistance(CandidateStar->Position, aPlayer::GetPlayer()->CurrentStar->Position))) {
                        return;
                    }
                    fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                }
            } else if (this->SelectedSearchCategory == 9) {
                if (aPlanet::TPlanet* planet = pas::class_cast_if<aPlanet::TPlanet*>(Value)) {
                    CandidatePlanet = planet;
                    if (CandidatePlanet->OwnerId == aGalaxyStruct::oiDominator && CandidatePlanet->CurrentStar->Status.CustomFaction == u"" && static_cast<std::uint8_t>(IncludeDominators ^ 1)) {
                        return;
                    }
                    if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(CandidatePlanet->OwnerId) && CandidatePlanet->CurrentStar->Status.CustomFaction == u"" && static_cast<std::uint8_t>(IncludeCoalition ^ 1)) {
                        return;
                    }
                    if (CandidatePlanet->CurrentStar->Status.CustomFaction != u"" && (static_cast<std::uint8_t>(IncludeDominators ^ 1) || static_cast<std::uint8_t>(IncludeCoalition ^ 1))) {
                        return;
                    }
                    if (CandidatePlanet->OwnerId == aGalaxyStruct::oiUninhabited && static_cast<std::uint8_t>(IncludeUninhabited ^ 1)) {
                        return;
                    }
                    if (CandidatePlanet->OwnerId == aGalaxyStruct::oiPirate && CandidatePlanet->CurrentStar->Status.CustomFaction == u"" && pas::contains(Owners, CandidatePlanet->OwnerId) && IncludeCoalition || pas::contains(Owners, aConst::RaceToOwner(CandidatePlanet->RaceId)) || CandidatePlanet->CurrentStar->Status.CustomFaction != u"" || static_cast<std::uint8_t>(IncludeDominators ^ 1) && static_cast<std::uint8_t>(IncludeCoalition ^ 1)) {
                        if (RangeFilter != 0 && RangeFilter < System::Round(aMyFunction::PointDistance(CandidatePlanet->CurrentStar->Position, aPlayer::GetPlayer()->CurrentStar->Position))) {
                            return;
                        }
                        if (ConstellationFilter != u"" && ([&] {
                            const pas::WideString& wideLowerCase = SysUtilsImports::WideLowerCase(CandidatePlanet->CurrentStar->Constellation->GetName());
                            const pas::WideString& constellationFilter = ConstellationFilter;
                            return fInfo::FindLowercaseInfoText(constellationFilter, wideLowerCase);
                        }()) <= 0) {
                            return;
                        }
                        if (StarFilter != u"" && ([&] {
                            const pas::WideString& wideLowerCase_2 = SysUtilsImports::WideLowerCase(CandidatePlanet->CurrentStar->Name);
                            const pas::WideString& starFilter = StarFilter;
                            return fInfo::FindLowercaseInfoText(starFilter, wideLowerCase_2);
                        }()) <= 0) {
                            return;
                        }
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 10) {
                if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Value)) {
                    CandidateStation = ruins;
                    if (CandidateStation->TypeNameOverrideKey != u"") {
                        if (StationTypes != pas::constant_set<aGalaxyStruct::TShipTypeMask>({})) {
                            return;
                        }
                    } else if (!pas::contains(StationTypes, CandidateStation->TypeId)) {
                        return;
                    }
                    if (CandidateStation->OwnerId == aGalaxyStruct::oiDominator) {
                        return;
                    }
                    if (CandidateStation->HasIndependentScriptFaction()) {
                        return;
                    }
                    if (RangeFilter != 0 && RangeFilter < System::Round(aMyFunction::PointDistance(CandidateStation->CurrentStar->Position, aPlayer::GetPlayer()->CurrentStar->Position))) {
                        return;
                    }
                    if (ConstellationFilter != u"" && ([&] {
                        const pas::WideString& wideLowerCase_3 = SysUtilsImports::WideLowerCase(CandidateStation->CurrentStar->Constellation->GetName());
                        const pas::WideString& constellationFilter_2 = ConstellationFilter;
                        return fInfo::FindLowercaseInfoText(constellationFilter_2, wideLowerCase_3);
                    }()) <= 0) {
                        return;
                    }
                    if (StarFilter != u"" && ([&] {
                        const pas::WideString& wideLowerCase_4 = SysUtilsImports::WideLowerCase(CandidateStation->CurrentStar->Name);
                        const pas::WideString& starFilter_2 = StarFilter;
                        return fInfo::FindLowercaseInfoText(starFilter_2, wideLowerCase_4);
                    }()) <= 0) {
                        return;
                    }
                    fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                }
            } else if (this->SelectedSearchCategory == 11) {
                if (aItem::THull* hull = pas::class_cast_if<aItem::THull*>(Value)) {
                    Hull = hull;
                    if (pas::contains(Owners, Hull->OwnerId) && (Hull->HullType != aGalaxyStruct::htRanger || IncludeRangerType) && (Hull->HullType != aGalaxyStruct::htWarrior || IncludeWarriorType) && (Hull->HullType != aGalaxyStruct::htPirate || IncludePirateType) && (Hull->HullType != aGalaxyStruct::htTransport || IncludeTransportType) && (Hull->HullType != aGalaxyStruct::htLiner || IncludeLinerType) && (Hull->HullType != aGalaxyStruct::htDiplomat || IncludeDiplomatType) && (SizeFilter == 0 || Hull->Weight >= SizeFilter) && (MinArmor == 0 || Hull->Armor >= MinArmor) && (MaxCost == 0 || Hull->GetConditionAdjustedCost() <= MaxCost) && (WeaponSlots == 0 || Hull->GetSlotCount(aConst::sskWeapon) >= WeaponSlots) && (ScannerSlots == 0 || Hull->GetSlotCount(aConst::sskScanner) >= ScannerSlots) && (RadarSlots == 0 || Hull->GetSlotCount(aConst::sskRadar) >= RadarSlots) && (DroidSlots == 0 || Hull->GetSlotCount(aConst::sskRepairRobot) >= DroidSlots) && (HookSlots == 0 || Hull->GetSlotCount(aConst::sskCargoHook) >= HookSlots) && (DefenseSlots == 0 || Hull->GetSlotCount(aConst::sskDefGenerator) >= DefenseSlots) && (ArtifactSlots == 0 || Hull->GetSlotCount(aConst::sskArtefact) >= ArtifactSlots) && (AfterburnerSlots == 0 || Hull->GetSlotCount(aConst::sskAfterburner) >= AfterburnerSlots)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            } else if (this->SelectedSearchCategory == 12) {
                if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Value)) {
                    Weapon = weapon;
                    if (!pas::contains(Owners, Weapon->OwnerId)) {
                        return;
                    }
                    if (RangeFilter != 0) {
                        EffectiveRange = Weapon->Range;
                        if (Weapon->ExtraSpecials != nullptr) {
                            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Weapon->ExtraSpecials) - 1); cpp_range.next(I); ) {
                                Entry = pas::list_at<aItem::TExtraSpecial>(Weapon->ExtraSpecials, I);
                                EffectiveRange += aConst::MicroModuleTemplates[Entry->ModuleIndexPlusOne - 1].StatBonuses[aConst::bonWRadius] * Entry->Count;
                            }
                        }
                        if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                            EffectiveRange = std::min<std::int32_t>(EffectiveRange - Weapon->CalculateGeneratedRange() + Weapon->GetWeaponInfo()->MissileRange, Weapon->GetWeaponInfo()->MissileRange);
                        }
                        if (RangeFilter > EffectiveRange) {
                            return;
                        }
                    }
                    if (SizeFilter != 0 && Weapon->Weight > SizeFilter) {
                        return;
                    }
                    if (MaxCost != 0 && Weapon->GetConditionAdjustedCost() > MaxCost) {
                        return;
                    }
                    if ((std::bit_cast<std::uint32_t>(Weapon->GetDamageFlags()) & 1) != 0 && static_cast<std::uint8_t>(IncludeEnergy ^ 1)) {
                        return;
                    }
                    if ((std::bit_cast<std::uint32_t>(Weapon->GetDamageFlags()) & 2) != 0 && static_cast<std::uint8_t>(IncludeSplinter ^ 1)) {
                        return;
                    }
                    if (pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && static_cast<std::uint8_t>(IncludeMissile ^ 1)) {
                        return;
                    }
                    if (MinDamage != 0) {
                        if ((std::bit_cast<std::uint32_t>(Weapon->GetDamageFlags()) & 0x00100000) != 0) {
                            if (MinDamage > std::max<std::int32_t>(Weapon->MaxDamage, Weapon->MinDamage)) {
                                return;
                            }
                        } else if (Weapon->MinDamage < MinDamage) {
                            return;
                        }
                    }
                    if (MaxDamage != 0 && Weapon->MaxDamage < MaxDamage) {
                        return;
                    }
                    if (NameFilter != u"" && static_cast<std::uint8_t>(fInfo::ItemMatchesInfoSearch(Weapon, NameFilter) ^ 1)) {
                        return;
                    }
                    fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                }
            } else if (this->SelectedSearchCategory == 15) {
                if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Value)) {
                    CandidateShip = ship;
                    // The native category tests these four TypeId values and the six
                    // checkbox slots directly, including their historical UI mapping.
                    if (pas::contains(Owners, CandidateShip->OwnerId) && (pas::contains(Owners, aConst::RaceToOwner(CandidateShip->PilotRace)) || Owners == pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPirate}})) && static_cast<std::uint8_t>(CandidateShip->HasScriptStateText() ^ 1) && pas::in_range(CandidateShip->TypeId, aGalaxyStruct::stRanger, aGalaxyStruct::stWarrior) && (CandidateShip->TypeNameOverrideKey == u"" || IncludeRangerType && IncludeWarriorType && IncludePirateType && IncludeTransportType && IncludeLinerType && IncludeDiplomatType) && (CandidateShip->TypeId != aGalaxyStruct::stRanger || IncludeRangerType) && (CandidateShip->TypeId != aGalaxyStruct::stWarrior || IncludeWarriorType) && (CandidateShip->TypeId != aGalaxyStruct::stPirate || IncludePirateType) && (CandidateShip->TypeId != aGalaxyStruct::stTransport || pas::checked_cast<aTransport::TTransport*>(CandidateShip)->TransportType != aTransport::ttTransport || IncludeTransportType) && (CandidateShip->TypeId != aGalaxyStruct::stTransport || pas::checked_cast<aTransport::TTransport*>(CandidateShip)->TransportType != aTransport::ttLiner || IncludeLinerType) && (CandidateShip->TypeId != aGalaxyStruct::stTransport || pas::checked_cast<aTransport::TTransport*>(CandidateShip)->TransportType != aTransport::ttDiplomat || IncludeDiplomatType) && (ConstellationFilter == u"" || ([&] {
                        const pas::WideString& wideLowerCase_5 = SysUtilsImports::WideLowerCase(CandidateShip->CurrentStar->Constellation->GetName());
                        const pas::WideString& constellationFilter_3 = ConstellationFilter;
                        return fInfo::FindLowercaseInfoText(constellationFilter_3, wideLowerCase_5);
                    }()) > 0) && (StarFilter == u"" || ([&] {
                        const pas::WideString& wideLowerCase_6 = SysUtilsImports::WideLowerCase(CandidateShip->CurrentStar->Name);
                        const pas::WideString& starFilter_3 = StarFilter;
                        return fInfo::FindLowercaseInfoText(starFilter_3, wideLowerCase_6);
                    }()) > 0) && (NameFilter == u"" || ([&] {
                        const pas::WideString& wideLowerCase_7 = SysUtilsImports::WideLowerCase(CandidateShip->GetFullName(u" "_wref.get()));
                        const pas::WideString& nameFilter = NameFilter;
                        return fInfo::FindLowercaseInfoText(nameFilter, wideLowerCase_7);
                    }()) > 0 || ([&] {
                        const pas::WideString& wideLowerCase_8 = SysUtilsImports::WideLowerCase(CandidateShip->GetLocalizedTypeName());
                        const pas::WideString& nameFilter_2 = NameFilter;
                        return fInfo::FindLowercaseInfoText(nameFilter_2, wideLowerCase_8);
                    }()) > 0)) {
                        fInfo::AddInfoSearchResult(Value, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                    }
                }
            }
        };
        // Nested in RunSearch; captures the owner filter set.
        auto ReadInfoSearchOwners = [&](std::int32_t Category) -> void {
            Owners = pas::constant_set<aGalaxyStruct::TOwnerMask>({});
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(Category, 2), u"Maloc"}))))->Down) {
                pas::include_at(&Owners, aGalaxyStruct::oiMaloc);
            }
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(Category, 2), u"Peleng"}))))->Down) {
                pas::include_at(&Owners, aGalaxyStruct::oiPeleng);
            }
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(Category, 2), u"People"}))))->Down) {
                pas::include_at(&Owners, aGalaxyStruct::oiHuman);
            }
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(Category, 2), u"Fei"}))))->Down) {
                pas::include_at(&Owners, aGalaxyStruct::oiFeyan);
            }
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(Category, 2), u"Gaal"}))))->Down) {
                pas::include_at(&Owners, aGalaxyStruct::oiGaal);
            }
            if (FindControlByPath(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(Category, 2), u"Pirate"})) != nullptr) {
                if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(pas::view(pas::concat_wide({u"M", EC_Str::IntToFixedWidthWideString(Category, 2), u"Pirate"}))))->Down) {
                    pas::include_at(&Owners, aGalaxyStruct::oiPirate);
                }
            }
            if (Owners == pas::constant_set<aGalaxyStruct::TOwnerMask>({})) {
                Owners = pas::constant_set<aGalaxyStruct::TOwnerMask>({{aGalaxyStruct::oiPirate}});
            }
        };
        PreviousSearchCategory = SelectedSearchCategory;
        HasSearchResults = true;
        InfoPanel->SetActive(true);
        InfoPanel->VerticalScrollBar->SetActive(true);
        GetByName(u"PanelSearch"sv)->SetActive(false);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 15); cpp_range.next(I); ) {
            GI_MessageLoop::TObjectGI* cpp_with = GetByName(pas::view(pas::concat_wide({u"PanelM", EC_Str::IntToFixedWidthWideString(I, 2)})));
            cpp_with->SetActive(false);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButPrev = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPrev"sv));
            ButPrev->SetDisabled(false);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButNext = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNext"sv));
            ButNext->SetDisabled(true);
        }
        if (aPlayer::GetPlayer()->Money < 3) {
            ClearInfoContents();
            {
                pas::WideString formatText1 = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(3);
                    pas::WideString localizedText = aConst::LocalizedText(u"FormInfo.NotMoney"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Money>"_w, std::move(intToStr));
                }());
                TfInfo* self = this;
                self->AddInfoText(std::move(formatText1), GI_Main::taxCenter, pas::WideString());
            }
            FinishInfoLayout();
            MainPanel->FlashMoneyWarning();
        } else {
            Shown = pas::make_object<pas::List>();
            aGalaxy::Galaxy->CheckIntegrityChecksum(207);
            Found = false;
            ResultCount = 0;
            ClearInfoContents();
            AddInfoSpacing(10);
            SearchText = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextSearch"sv))->Text));
            if (SearchText == u"") {
                SearchText = u"   "_w;
            }
            if (SelectedSearchCategory == 1) {
                MinSpeed = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M01Speed"sv))->Text));
                RangeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M01Range"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M01Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M01Cost"sv))->Text));
                ReadInfoSearchOwners(1);
            } else if (SelectedSearchCategory == 2) {
                MinFuel = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M02Capacity"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M02Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M02Cost"sv))->Text));
                ReadInfoSearchOwners(2);
            } else if (SelectedSearchCategory == 3) {
                RangeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M03Range"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M03Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M03Cost"sv))->Text));
                ReadInfoSearchOwners(3);
            } else if (SelectedSearchCategory == 4) {
                MinPower = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M04Power"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M04Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M04Cost"sv))->Text));
                ReadInfoSearchOwners(4);
            } else if (SelectedSearchCategory == 5) {
                MinPower = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M05Power"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M05Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M05Cost"sv))->Text));
                ReadInfoSearchOwners(5);
            } else if (SelectedSearchCategory == 6) {
                MinPickup = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M06ObjSize"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M06Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M06Cost"sv))->Text));
                ReadInfoSearchOwners(6);
            } else if (SelectedSearchCategory == 7) {
                MinDefense = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M07Block"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M07Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M07Cost"sv))->Text));
                ReadInfoSearchOwners(7);
            } else if (SelectedSearchCategory == 8) {
                IncludeCoalition = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M08CtrlCol"sv))->Down;
                IncludeDominators = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M08CtrlDom"sv))->Down;
                IncludePirates = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M08CtrlPirate"sv))->Down;
                RangeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M08Range"sv))->Text));
            } else if (SelectedSearchCategory == 9) {
                ConstellationFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M09Const"sv))->Text));
                StarFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M09Star"sv))->Text));
                IncludeCoalition = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M09CtrlCol"sv))->Down;
                IncludeDominators = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M09CtrlDom"sv))->Down;
                IncludeUninhabited = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M09CtrlNo"sv))->Down;
                RangeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M09Range"sv))->Text));
                ReadInfoSearchOwners(9);
            } else if (SelectedSearchCategory == 10) {
                ConstellationFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M10Const"sv))->Text));
                StarFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M10Star"sv))->Text));
                StationTypes = pas::constant_set<aGalaxyStruct::TShipTypeMask>({});
                for (auto cpp_range_2 = pas::for_to<std::uint8_t>(aGalaxyStruct::rstRangerCenter, aGalaxyStruct::rstDominion); cpp_range_2.next(StationType); ) {
                    Control = FindControlByPath(pas::concat_wide({u"M10Type", aConst::ShipTypeNames[StationType].Name}));
                    if (Control != nullptr && pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control)->Down) {
                        pas::include_at(&StationTypes, StationType);
                    }
                }
                RangeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M10Range"sv))->Text));
            } else if (SelectedSearchCategory == 11) {
                IncludeRangerType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11TypeRanger"sv))->Down;
                IncludeWarriorType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11TypeWarrior"sv))->Down;
                IncludePirateType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11TypePirat"sv))->Down;
                IncludeTransportType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11TypeTransport"sv))->Down;
                IncludeLinerType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11TypeLiner"sv))->Down;
                IncludeDiplomatType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11TypeDiplomat"sv))->Down;
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M11Size"sv))->Text));
                MinArmor = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M11Def"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M11Cost"sv))->Text));
                WeaponSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S01"sv))->Down) {
                    ++WeaponSlots;
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S02"sv))->Down) {
                    ++WeaponSlots;
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S03"sv))->Down) {
                    ++WeaponSlots;
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S04"sv))->Down) {
                    ++WeaponSlots;
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S05"sv))->Down) {
                    ++WeaponSlots;
                }
                ScannerSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S08"sv))->Down) {
                    ++ScannerSlots;
                }
                RadarSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S09"sv))->Down) {
                    ++RadarSlots;
                }
                DroidSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S10"sv))->Down) {
                    ++DroidSlots;
                }
                HookSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S11"sv))->Down) {
                    ++HookSlots;
                }
                DefenseSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S12"sv))->Down) {
                    ++DefenseSlots;
                }
                ArtifactSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S13"sv))->Down) {
                    ++ArtifactSlots;
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S14"sv))->Down) {
                    ++ArtifactSlots;
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S15"sv))->Down) {
                    ++ArtifactSlots;
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S16"sv))->Down) {
                    ++ArtifactSlots;
                }
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(18, aConst::DefaultHullSlotCounts[aConst::sskArtefact] + 13); cpp_range_3.next(I); ) {
                    Control = FindControlByPath(pas::concat_wide({u"M11S", EC_Str::IntToWideString(I)}));
                    if (Control != nullptr && pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control)->Down) {
                        ++ArtifactSlots;
                    }
                }
                AfterburnerSlots = 0;
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M11S17"sv))->Down) {
                    ++AfterburnerSlots;
                }
                ReadInfoSearchOwners(11);
            } else if (SelectedSearchCategory == 12) {
                MinDamage = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12DamageMin"sv))->Text));
                MaxDamage = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12DamageMax"sv))->Text));
                RangeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12Range"sv))->Text));
                NameFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12Name"sv))->Text));
                SizeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12Size"sv))->Text));
                MaxCost = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12Cost"sv))->Text));
                IncludeEnergy = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M12TypeEne"sv))->Down;
                IncludeSplinter = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M12TypeOsk"sv))->Down;
                IncludeMissile = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M12TypeRak"sv))->Down;
                ReadInfoSearchOwners(12);
            } else if (SelectedSearchCategory == 13) {
                MinGoodsCount = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M13Cnt"sv))->Text));
                MinSellPrice = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M13PriceBuy"sv))->Text));
                MaxBuyPrice = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M13PriceSell"sv))->Text));
                RangeFilter = EC_Str::ExtractDigitsToIntW(pas::view(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M13Range"sv))->Text));
                GoodsSelected[0] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods0"sv))->Down ^ 1);
                GoodsSelected[1] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods1"sv))->Down ^ 1);
                GoodsSelected[5] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods2"sv))->Down ^ 1);
                GoodsSelected[4] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods3"sv))->Down ^ 1);
                GoodsSelected[3] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods4"sv))->Down ^ 1);
                GoodsSelected[2] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods5"sv))->Down ^ 1);
                GoodsSelected[6] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods6"sv))->Down ^ 1);
                GoodsSelected[7] = static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M13Goods7"sv))->Down ^ 1);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_4.next(I); ) {
                    if (ResultCount >= TfInfo::GetSearchResultLimit()) {
                        break;
                    }
                    Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star));
                    if (Star->Id == 71 || Star->Id == 72) {
                        continue;
                    }
                    if (RangeFilter != 0 && RangeFilter < System::Round(aMyFunction::PointDistance(Star->Position, aPlayer::GetPlayer()->CurrentStar->Position))) {
                        continue;
                    }
                    if (Star->Status.CustomFaction == u"") {
                        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_5.next(Index); ) {
                            if (ResultCount >= TfInfo::GetSearchResultLimit()) {
                                break;
                            }
                            Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, Index);
                            if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId)) {
                                Heading = pas::WideString();
                                for (GoodsIndex = static_cast<std::uint8_t>(0); GoodsIndex <= static_cast<std::uint8_t>(7); ++GoodsIndex) {
                                    Good = aConst::GoodsTextOrder[GoodsIndex];
                                    if ((MinGoodsCount == 0 || Planet->Goods[Good].Count >= MinGoodsCount) && GoodsSelected[Good] && (MaxBuyPrice == 0 || aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet) <= MaxBuyPrice) && (MinSellPrice == 0 || aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet) >= MinSellPrice)) {
                                        Heading = pas::concat_wide({Heading, u"\r\n", u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(5)), u">", u"<align=center>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(GoodsIndex + 1)), u""sv), u".", u"</align>"});
                                        if (aConst::GoodsLegalOnPlanet[Good][Planet->RaceId][Planet->Government] || Planet->OwnerId == aGalaxyStruct::oiPirate) {
                                            SearchText = pas::WideString();
                                        } else {
                                            SearchText = u"<color=255,0,0>"_w;
                                        }
                                        Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(15)), u">", u"", aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[Good].DisplayName), pas::view(SearchText)), u""});
                                        Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(160)), u">", u"<align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Planet->Goods[Good].Count)), u""sv), u"</align>"});
                                        Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(205)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet))), u""sv), u"</align>"});
                                        Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(215)), u"><align=center>", aMyFunction::WrapTextInColor(u"/"sv, u""sv), u"</align>"});
                                        Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(255)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet))), u""sv), u"</align>"});
                                    }
                                }
                                if (Heading != u"") {
                                    Description = ([&] {
                                        pas::WideString name = Planet->CurrentStar->Constellation->GetName();
                                        pas::WideString localizedText_2 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                                        return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name));
                                    }());
                                    Description = pas::concat_wide({Description, u"\r\n", ([&] {
                                        auto name_2 = pas::borrow(Planet->CurrentStar->Name);
                                        pas::WideString localizedText_3 = aConst::LocalizedText(u"FormInfo.Star"_wref.get());
                                        return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=255,240,100>"_w, u"<StarName>"_w, name_2.get());
                                    }())});
                                    Description = pas::concat_wide({Description, u"\r\n", Planet->GetInfoText(false)});
                                    Description = pas::concat_wide({Description, Heading});
                                    Heading = aMyFunction::WrapTextInColor(pas::view(pas::concat_wide({u"- ", aMyFunction::WrapTextInColor(pas::view(Planet->GetFullName(u" "_w)), u"<color=255,240,100>"sv), u" -"})), u"<color=255,240,100>"sv);
                                    AddInfoHeading(Heading, pas::concat_wide({Heading, u"\r\n", Description}), 0, 0, Planet->Id);
                                    AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                                    AddPlanetInfoText(Planet, Description);
                                    Found = true;
                                    ++ResultCount;
                                }
                            }
                        }
                        Planet = nullptr;
                        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_6.next(Index); ) {
                            Ship = pas::list_at<aShip::TShip>(Star->Ships, Index);
                            if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Ship); ruins != nullptr && (Ship->CurrentPlanet == nullptr || Ship->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited)) {
                                Station = ruins;
                                // Native exits here, bypassing the later list release and checksum.
                                if (Station->OwnerId == aGalaxyStruct::oiDominator || Station->HasIndependentScriptFaction()) {
                                    return;
                                }
                                if (!Station->NoLanding) {
                                    Heading = pas::WideString();
                                    for (GoodsIndex = static_cast<std::uint8_t>(0); GoodsIndex <= static_cast<std::uint8_t>(7); ++GoodsIndex) {
                                        Good = aConst::GoodsTextOrder[GoodsIndex];
                                        if ((MinGoodsCount == 0 || Station->ShopGoods[Good].Count >= MinGoodsCount) && GoodsSelected[Good] && (MaxBuyPrice == 0 || aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Station) <= MaxBuyPrice) && (MinSellPrice == 0 || aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Station) >= MinSellPrice)) {
                                            Heading = pas::concat_wide({Heading, u"\r\n", u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(5)), u">", u"<align=center>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(GoodsIndex + 1)), u""sv), u".", u"</align>"});
                                            SearchText = pas::WideString();
                                            Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(15)), u">", u"", aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[Good].DisplayName), pas::view(SearchText)), u""});
                                            Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(160)), u">", u"<align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Station->ShopGoods[Good].Count)), u""sv), u"</align>"});
                                            Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(205)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Station))), u""sv), u"</align>"});
                                            Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(215)), u"><align=center>", aMyFunction::WrapTextInColor(u"/"sv, u""sv), u"</align>"});
                                            Heading = pas::concat_wide({Heading, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(255)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Station))), u""sv), u"</align>"});
                                        }
                                    }
                                    if (Heading != u"") {
                                        Description = ([&] {
                                            pas::WideString name_3 = Station->CurrentStar->Constellation->GetName();
                                            pas::WideString localizedText_4 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                                            return aMyFunction::FormatText1(std::move(localizedText_4), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name_3));
                                        }());
                                        Description = pas::concat_wide({Description, u"\r\n", ([&] {
                                            auto name_4 = pas::borrow(Station->CurrentStar->Name);
                                            pas::WideString localizedText_5 = aConst::LocalizedText(u"FormInfo.Star"_wref.get());
                                            return aMyFunction::FormatText1(std::move(localizedText_5), u"<color=255,240,100>"_w, u"<StarName>"_w, name_4.get());
                                        }())});
                                        Description = pas::concat_wide({Description, Heading});
                                        Heading = aMyFunction::WrapTextInColor(pas::view(pas::concat_wide({u"- ", aMyFunction::WrapTextInColor(pas::view(Station->GetFullName(u" "_wref.get())), u"<color=255,240,100>"sv), u" -"})), u"<color=255,240,100>"sv);
                                        AddInfoHeading(Heading, pas::concat_wide({Heading, u"\r\n", Description}), 0, 0, static_cast<std::uint32_t>(Station->Id) | 0x80000000u);
                                        AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                                        {
                                            pas::WideString extractDelimitedPartW = EC_Str::ExtractDelimitedPartW(pas::view(Station->GetShipPortraitImagePath()), 1, u","sv);
                                            TfInfo* self_2 = this;
                                            self_2->AddInfoImageText(std::move(extractDelimitedPartW), Description);
                                        }
                                        Found = true;
                                        ++ResultCount;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (SelectedSearchCategory == 15) {
                NameFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M15Name"sv))->Text));
                ConstellationFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M15Const"sv))->Text));
                StarFilter = EC_Str::TrimWideString(SysUtilsImports::WideLowerCase(pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M15Star"sv))->Text));
                IncludeRangerType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15TypeRanger"sv))->Down;
                IncludeWarriorType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15TypeWarrior"sv))->Down;
                IncludePirateType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15TypePirat"sv))->Down;
                IncludeTransportType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15TypeTransport"sv))->Down;
                IncludeLinerType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15TypeLiner"sv))->Down;
                IncludeDiplomatType = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"M15TypeDiplomat"sv))->Down;
                ReadInfoSearchOwners(15);
            }
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_7.next(I); ) {
                Star = pas::checked_cast<aGalaxy::TStar*>(static_cast<pas::Object*>(aPlayer::GetPlayer()->CurrentStar->StarDistances[I].Star));
                if (Star->Constellation->Id != 20) {
                    if (SelectedSearchCategory == 0) {
                        if (([&] {
                            const pas::WideString& wideLowerCase = SysUtilsImports::WideLowerCase(Star->Name);
                            const pas::WideString& searchText = SearchText;
                            return fInfo::FindLowercaseInfoText(searchText, wideLowerCase);
                        }()) > 0) {
                            fInfo::AddInfoSearchResult(Star, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                        }
                    } else {
                        CheckInfoSearchResult(Star);
                    }
                    Planet = nullptr;
                    for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_8.next(Index); ) {
                        Ship = pas::list_at<aShip::TShip>(Star->Ships, Index);
                        if (Ship->CurrentPlanet == nullptr || Ship->CurrentPlanet->OwnerId != aGalaxyStruct::oiUninhabited) {
                            if (SelectedSearchCategory == 0) {
                                if (([&] {
                                    const pas::WideString& wideLowerCase_2 = SysUtilsImports::WideLowerCase(Ship->GetFullName(u" "_wref.get()));
                                    const pas::WideString& searchText_2 = SearchText;
                                    return fInfo::FindLowercaseInfoText(searchText_2, wideLowerCase_2);
                                }()) > 0 || ([&] {
                                    const pas::WideString& wideLowerCase_3 = SysUtilsImports::WideLowerCase(Ship->GetLocalizedTypeName());
                                    const pas::WideString& searchText_3 = SearchText;
                                    return fInfo::FindLowercaseInfoText(searchText_3, wideLowerCase_3);
                                }()) > 0) {
                                    fInfo::AddInfoSearchResult(Ship, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                                }
                            } else {
                                CheckInfoSearchResult(Ship);
                            }
                            if (aPlayer::GetPlayer()->DockedTo == Ship && fEquipmentShop::TemporaryShopSlots != nullptr) {
                                for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_9.next(ItemIndex); ) {
                                    Item = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, ItemIndex)->Item;
                                    if (Item != nullptr) {
                                        if (SelectedSearchCategory == 0) {
                                            if (fInfo::ItemMatchesInfoSearch(Item, SearchText)) {
                                                fInfo::AddInfoSearchResult(Item, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                                            }
                                        } else {
                                            CheckInfoSearchResult(Item);
                                        }
                                    }
                                }
                            } else if (aRuins::TRuins* ruins_2 = pas::class_cast_if<aRuins::TRuins*>(Ship); ruins_2 != nullptr && Ship->OwnerId != aGalaxyStruct::oiDominator && static_cast<std::uint8_t>(Ship->HasIndependentScriptFaction() ^ 1) && static_cast<std::uint8_t>(ruins_2->NoLanding ^ 1)) {
                                for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(ruins_2->EquipmentShop) - 1); cpp_range_10.next(ItemIndex); ) {
                                    Item = pas::list_at<aItem::TItem>(reinterpret_cast<aRuins::TRuins*>(Ship)->EquipmentShop, ItemIndex);
                                    if (SelectedSearchCategory == 0) {
                                        if (fInfo::ItemMatchesInfoSearch(Item, SearchText)) {
                                            fInfo::AddInfoSearchResult(Item, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                                        }
                                    } else {
                                        CheckInfoSearchResult(Item);
                                    }
                                }
                            }
                        }
                    }
                    for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_11.next(Index); ) {
                        Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, Index);
                        if (SelectedSearchCategory == 0) {
                            if (([&] {
                                const pas::WideString& wideLowerCase_4 = SysUtilsImports::WideLowerCase(Planet->GetFullName(u" "_w));
                                const pas::WideString& searchText_4 = SearchText;
                                return fInfo::FindLowercaseInfoText(searchText_4, wideLowerCase_4);
                            }()) > 0) {
                                fInfo::AddInfoSearchResult(Planet, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                            }
                        } else {
                            CheckInfoSearchResult(Planet);
                        }
                        if (Planet->IsCoalitionOwned || Planet->OwnerId == aGalaxyStruct::oiPirate) {
                            if (aPlayer::GetPlayer()->CurrentPlanet != nullptr && aPlayer::GetPlayer()->CurrentPlanet == Planet && fEquipmentShop::TemporaryShopSlots != nullptr) {
                                for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, pas::list_count(fEquipmentShop::TemporaryShopSlots) - 1); cpp_range_12.next(ItemIndex); ) {
                                    Item = pas::list_at<fEquipmentShop::TShopSlot>(fEquipmentShop::TemporaryShopSlots, ItemIndex)->Item;
                                    if (Item != nullptr) {
                                        if (SelectedSearchCategory == 0) {
                                            if (fInfo::ItemMatchesInfoSearch(Item, SearchText)) {
                                                fInfo::AddInfoSearchResult(Item, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                                            }
                                        } else {
                                            CheckInfoSearchResult(Item);
                                        }
                                    }
                                }
                            } else {
                                for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->EquipmentShop) - 1); cpp_range_13.next(ItemIndex); ) {
                                    Item = pas::list_at<aItem::TItem>(Planet->EquipmentShop, ItemIndex);
                                    if (SelectedSearchCategory == 0) {
                                        if (fInfo::ItemMatchesInfoSearch(Item, SearchText)) {
                                            fInfo::AddInfoSearchResult(Item, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                                        }
                                    } else {
                                        CheckInfoSearchResult(Item);
                                    }
                                }
                            }
                            for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_14.next(ItemIndex); ) {
                                Ship = pas::list_at<aShip::TShip>(Planet->Warriors, ItemIndex);
                                if (pas::list_indexof(Ship->CurrentStar->Ships, reinterpret_cast<void*>(Ship)) < 0) {
                                    if (SelectedSearchCategory == 0) {
                                        if (([&] {
                                            const pas::WideString& wideLowerCase_5 = SysUtilsImports::WideLowerCase(Ship->GetFullName(u" "_wref.get()));
                                            const pas::WideString& searchText_5 = SearchText;
                                            return fInfo::FindLowercaseInfoText(searchText_5, wideLowerCase_5);
                                        }()) > 0 || ([&] {
                                            const pas::WideString& wideLowerCase_6 = SysUtilsImports::WideLowerCase(Ship->GetLocalizedTypeName());
                                            const pas::WideString& searchText_6 = SearchText;
                                            return fInfo::FindLowercaseInfoText(searchText_6, wideLowerCase_6);
                                        }()) > 0) {
                                            fInfo::AddInfoSearchResult(Ship, this, Ship, ResultCount, Shown, Found, Station, Description, Heading, Planet, Star, Bearing);
                                        }
                                    } else {
                                        CheckInfoSearchResult(Ship);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (Found) {
                AddInfoSpacing(10);
                AddInfoSeparator();
                AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                {
                    pas::WideString formatText1_2 = ([&] {
                        pas::WideString intToStr_2 = pas::wide_int_to_str(ResultCount);
                        pas::WideString localizedText_6 = aConst::LocalizedText(u"FormInfo.ObjectFoundEnd"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_6), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr_2));
                    }());
                    TfInfo* self_3 = this;
                    self_3->AddInfoText(std::move(formatText1_2), GI_Main::taxCenter, pas::WideString());
                }
                aPlayer::GetPlayer()->SetMoney(aPlayer::GetPlayer()->Money - 3);
                GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                SetFocusedControl(nullptr);
            } else {
                ClearInfoContents();
                AddInfoSpacing(15);
                {
                    pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"FormInfo.NotFound"_wref.get());
                    TfInfo* self_4 = this;
                    self_4->AddInfoText(std::move(lookupLocalizedTextByKey), GI_Main::taxCenter, pas::WideString());
                }
                GR_Main::SoundManager->PlaySound(u"Sound.NoMoney"_wref.get());
            }
            AddInfoSpacing(10);
            AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
            FinishInfoLayout();
            aGalaxy::Galaxy->PrimeIntegrityChecksum(208);
            pas::free(Shown);
        }
    }

    pas::WideString GetInfoQualityGrade(float Quality) {
        std::int32_t I{};
        std::int32_t Grade{};
        float Distance{};
        Grade = 0;
        float BestDistance = std::fabs(static_cast<long double>(InfoQualityGrades[0]) - Quality);
        for (I = 1; I <= 6; ++I) {
            Distance = std::fabs(static_cast<long double>(InfoQualityGrades[I]) - Quality);
            if (Distance < BestDistance) {
                BestDistance = Distance;
                Grade = I;
            }
        }
        return aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"FormInfo.Equipment.QualityGrade", SysUtils::IntToStr(Grade)})));
    }

    pas::WideString GetInfoEquipmentSummary(aItem::TItem* Item) {
        pas::WideString Detail{};
        pas::WideString Text{};
        Text = pas::concat_wide({Text, ([&] {
            pas::WideString intToStr = pas::wide_int_to_str(Item->GetConditionAdjustedCost());
            pas::WideString localizedText = aConst::LocalizedText(u"FormInfo.Equipment.Cost"_wref.get());
            return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<val>"_w, std::move(intToStr));
        }())});
        if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Hull), static_cast<std::int32_t>(aConst::t_CustomWeapon))) {
            Text = pas::concat_wide({Text, u", "});
        }
        if (pas::class_cast_if<aItem::TScaner*>(Item) != nullptr) {
            Detail = aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Scaner"_wref.get()), pas::WideString(), u"<val>"_w, u"<Percent>"_w);
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
        } else if (pas::class_cast_if<aItem::TRadar*>(Item) != nullptr) {
            Detail = aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Radar"_wref.get()), pas::WideString(), u"<val>"_w, u"<Radius>"_w);
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
        } else if (pas::class_cast_if<aItem::TFuelTanks*>(Item) != nullptr) {
            Detail = aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Fuel"_wref.get()), pas::WideString(), u"<val>"_w, u"<Capacity>"_w);
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
        } else if (pas::class_cast_if<aItem::TEngine*>(Item) != nullptr) {
            Detail = pas::concat_wide({aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Speed"_wref.get()), pas::WideString(), u"<val>"_w, u"<Speed>"_w), u", ", aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Jump"_wref.get()), pas::WideString(), u"<val>"_w, u"<Parsec>"_w)});
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
        } else if (pas::class_cast_if<aItem::TRepairRobot*>(Item) != nullptr) {
            Detail = aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Droid"_wref.get()), pas::WideString(), u"<val>"_w, u"<RecoverHitPoints>"_w);
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
        } else if (pas::class_cast_if<aItem::TCargoHook*>(Item) != nullptr) {
            Detail = pas::concat_wide({aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Hook"_wref.get()), pas::WideString(), u"<val>"_w, u"<PickUpSize>"_w), u", ", aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.HookRadius"_wref.get()), pas::WideString(), u"<val>"_w, u"<Radius>"_w)});
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
        } else if (pas::class_cast_if<aItem::TDefGenerator*>(Item) != nullptr) {
            Detail = aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Defend"_wref.get()), pas::WideString(), u"<val>"_w, u"<Percent>"_w);
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
        } else if (aItem::TWeapon* weapon = pas::class_cast_if<aItem::TWeapon*>(Item)) {
            if ((std::bit_cast<std::uint32_t>(weapon->GetDamageFlags()) & 0x00100000) != 0) {
                Detail = pas::concat_wide({aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Damage"_wref.get()), pas::WideString(), u"<min>-<max>"_w, u"<MaxDamage><Bonus>"_w), u", ", aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Radius"_wref.get()), pas::WideString(), u"<val>"_w, u"<Radius>"_w)});
            } else {
                Detail = pas::concat_wide({aMyFunction::FormatText2(aConst::LocalizedText(u"FormInfo.Equipment.Damage"_wref.get()), pas::WideString(), u"<min>"_w, u"<MinDamage>"_w, u"<max>"_w, u"<MaxDamage><Bonus>"_w), u", ", aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Radius"_wref.get()), pas::WideString(), u"<val>"_w, u"<Radius>"_w)});
            }
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
            if (pas::checked_cast<aItem::TWeapon*>(Item)->SpecialModuleIndex != 0 && aConst::MicroModuleTemplates[static_cast<aItem::TWeapon*>(Item)->SpecialModuleIndex - 1].TextReplace == u"") {
                Text = pas::concat_wide({Text, u"\r\n", aConst::LocalizedText(u"Items.Weapon.WSpecial"_wref.get()), u" ", aMyFunction::WrapTextInColor(pas::view(reinterpret_cast<aItem::TEquipment*>(Item)->GetSpecialModuleName()), u"<color=255,240,100>"sv)});
            }
        } else if (pas::class_cast_if<aItem::THull*>(Item) != nullptr) {
            Detail = pas::concat_wide({aMyFunction::FormatText1(aConst::LocalizedText(u"FormInfo.Equipment.Protect"_wref.get()), pas::WideString(), u"<val>"_w, u"<HitProtect>"_w), u"\r\n", aConst::LocalizedText(u"FormInfo.Equipment.Susceptibility"_wref.get())});
            reinterpret_cast<aItem::TEquipment*>(Item)->ReplaceInfoTokens(Detail, u"<color=255,240,100>"_w, nullptr);
            Text = pas::concat_wide({Text, Detail});
            if (reinterpret_cast<aItem::THull*>(Item)->HullSeries != -1) {
                Text = pas::concat_wide({Text, u"\r\n", ([&] {
                    auto name = pas::borrow(aConst::HullSeriesDefinitions[reinterpret_cast<aItem::THull*>(Item)->HullSeries].Name);
                    pas::WideString localizedText_2 = aConst::LocalizedText(u"FormInfo.Equipment.Series"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<val>"_w, name.get());
                }())});
            }
        }
        if (pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr && pas::in_set<aConst::t_FuelTanks, aConst::t_CustomWeapon, aConst::t_Satellite, aConst::t_Satellite>(Item->ItemType)) {
            Text = pas::concat_wide({Text, u"\r\n", ([&] {
                pas::WideString infoQualityGrade = fInfo::GetInfoQualityGrade(reinterpret_cast<aItem::TEquipment*>(Item)->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
                pas::WideString localizedText_3 = aConst::LocalizedText(u"FormInfo.Equipment.Reliability"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=255,240,100>"_w, u"<val>"_w, std::move(infoQualityGrade));
            }())});
        }
        return Text;
    }

    void AddInfoSearchResult(pas::Object* Value, TfInfo* Self, aShip::TShip*& Ship, std::int32_t& ResultCount, pas::List*& Shown, std::uint8_t& Found, aRuins::TRuins*& Station, pas::WideString& Description, pas::WideString& Heading, aPlanet::TPlanet*& Planet, aGalaxy::TStar*& Star, std::int32_t& Bearing) {
        pas::WideString GoodsText{};
        pas::WideString Color{};
        std::uint8_t Good{};
        std::uint8_t GoodIndex{};
        if (pas::class_cast_if<aShip::TShip*>(Value) != nullptr && Ship != nullptr && static_cast<std::uint8_t>(pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Ship->OwnerId) ^ 1)) {
            return;
        }
        if (ResultCount < TfInfo::GetSearchResultLimit() && pas::list_indexof(Shown, reinterpret_cast<void*>(Value)) < 0) {
            pas::list_add(Shown, reinterpret_cast<void*>(Value));
            if (!Found) {
                {
                    pas::WideString formatText1 = ([&] {
                        pas::WideString intToStr = pas::wide_int_to_str(TfInfo::GetSearchResultLimit());
                        pas::WideString localizedText = aConst::LocalizedText(u"FormInfo.ObjectFoundStart"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Count>"_w, std::move(intToStr));
                    }());
                    TfInfo* self = Self;
                    self->AddInfoText(std::move(formatText1), GI_Main::taxCenter, pas::WideString());
                }
                Self->AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
            }
            if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Value); ruins != nullptr && static_cast<std::uint8_t>(ruins->NoLanding ^ 1)) {
                Station = ruins;
                Description = ([&] {
                    pas::WideString name = Station->CurrentStar->Constellation->GetName();
                    pas::WideString localizedText_2 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name));
                }());
                Description = pas::concat_wide({Description, u"\r\n", ([&] {
                    auto name_2 = pas::borrow(Station->CurrentStar->Name);
                    pas::WideString localizedText_3 = aConst::LocalizedText(u"FormInfo.Star"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_3), u"<color=255,240,100>"_w, u"<StarName>"_w, name_2.get());
                }())});
                GoodsText = pas::WideString();
                for (GoodIndex = static_cast<std::uint8_t>(0); GoodIndex <= static_cast<std::uint8_t>(7); ++GoodIndex) {
                    Good = aConst::GoodsTextOrder[GoodIndex];
                    GoodsText = pas::concat_wide({GoodsText, u"\r\n", u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(5)), u">", u"<align=center>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(GoodIndex + 1)), u""sv), u".", u"</align>"});
                    Color = pas::WideString();
                    GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(15)), u">", u"", aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[Good].DisplayName), pas::view(Color)), u""});
                    GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(160)), u">", u"<align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Station->ShopGoods[Good].Count)), u""sv), u"</align>"});
                    GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(205)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Station))), u""sv), u"</align>"});
                    GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(215)), u"><align=center>", aMyFunction::WrapTextInColor(u"/"sv, u""sv), u"</align>"});
                    GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(255)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Station))), u""sv), u"</align>"});
                }
                Description = pas::concat_wide({Description, GoodsText});
                Heading = aMyFunction::WrapTextInColor(pas::view(pas::concat_wide({u"- ", aMyFunction::WrapTextInColor(pas::view(Station->GetFullName(u" "_wref.get())), u"<color=255,240,100>"sv), u" -"})), u"<color=255,240,100>"sv);
                Self->AddInfoHeading(Heading, pas::concat_wide({Heading, u"\r\n", Description}), 0, 0, static_cast<std::uint32_t>(Station->Id) | 0x80000000u);
                Self->AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                {
                    pas::WideString extractDelimitedPartW = EC_Str::ExtractDelimitedPartW(pas::view(Station->GetShipPortraitImagePath()), 1, u","sv);
                    TfInfo* self_2 = Self;
                    self_2->AddInfoImageText(std::move(extractDelimitedPartW), Description);
                }
            } else if (pas::class_cast_if<aShip::TShip*>(Value) != nullptr) {
                Description = ([&] {
                    pas::WideString name_3 = Ship->CurrentStar->Constellation->GetName();
                    pas::WideString localizedText_4 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_4), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name_3));
                }());
                Description = pas::concat_wide({Description, u"\r\n", ([&] {
                    auto name_4 = pas::borrow(Ship->CurrentStar->Name);
                    pas::WideString localizedText_5 = aConst::LocalizedText(u"FormInfo.Star"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_5), u"<color=255,240,100>"_w, u"<StarName>"_w, name_4.get());
                }())});
                if (Ship->CurrentPlanet != nullptr) {
                    Description = pas::concat_wide({Description, u"\r\n", ([&] {
                        auto name_5 = pas::borrow(Ship->CurrentPlanet->Name);
                        pas::WideString localizedText_6 = aConst::LocalizedText(u"FormInfo.Planet"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_6), u"<color=255,240,100>"_w, u"<PlanetName>"_w, name_5.get());
                    }())});
                }
                if (aShip::TShip_IsInPrison(Ship)) {
                    Description = pas::concat_wide({Description, u"\r\n", aConst::LocalizedText(u"FormInfo.InPrison"_wref.get())});
                }
                Ship->DaysSincePlayerSeen = 0;
                Heading = aMyFunction::WrapTextInColor(pas::view(pas::concat_wide({u"- ", aMyFunction::WrapTextInColor(pas::view(Ship->GetFullName(u" "_wref.get())), u"<color=255,240,100>"sv), u" -"})), u"<color=255,240,100>"sv);
                Self->AddInfoHeading(Heading, pas::concat_wide({Heading, u"\r\n", Description}), 0, 0, 0);
                Self->AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                {
                    pas::WideString extractDelimitedPartW_2 = EC_Str::ExtractDelimitedPartW(pas::view(Ship->GetShipPortraitImagePath()), 1, u","sv);
                    TfInfo* self_3 = Self;
                    self_3->AddInfoImageText(std::move(extractDelimitedPartW_2), Description);
                }
            } else if (pas::class_cast_if<aPlanet::TPlanet*>(Value) != nullptr) {
                Description = ([&] {
                    pas::WideString name_6 = Planet->CurrentStar->Constellation->GetName();
                    pas::WideString localizedText_7 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_7), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name_6));
                }());
                Description = pas::concat_wide({Description, u"\r\n", ([&] {
                    auto name_7 = pas::borrow(Planet->CurrentStar->Name);
                    pas::WideString localizedText_8 = aConst::LocalizedText(u"FormInfo.Star"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_8), u"<color=255,240,100>"_w, u"<StarName>"_w, name_7.get());
                }())});
                Description = pas::concat_wide({Description, u"\r\n", Planet->GetInfoText(true)});
                GoodsText = pas::WideString();
                if ((Planet->IsCoalitionOwned || Planet->OwnerId == aGalaxyStruct::oiPirate) && Planet->CurrentStar->Status.CustomFaction == u"") {
                    for (GoodIndex = static_cast<std::uint8_t>(0); GoodIndex <= static_cast<std::uint8_t>(7); ++GoodIndex) {
                        Good = aConst::GoodsTextOrder[GoodIndex];
                        GoodsText = pas::concat_wide({GoodsText, u"\r\n", u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(5)), u">", u"<align=center>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(GoodIndex + 1)), u""sv), u".", u"</align>"});
                        if (aConst::GoodsLegalOnPlanet[Good][Planet->RaceId][Planet->Government] || Planet->OwnerId == aGalaxyStruct::oiPirate) {
                            Color = pas::WideString();
                        } else {
                            Color = u"<color=255,0,0>"_w;
                        }
                        GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(15)), u">", u"", aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[Good].DisplayName), pas::view(Color)), u""});
                        GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(160)), u">", u"<align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Planet->Goods[Good].Count)), u""sv), u"</align>"});
                        GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(205)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Planet))), u""sv), u"</align>"});
                        GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(215)), u"><align=center>", aMyFunction::WrapTextInColor(u"/"sv, u""sv), u"</align>"});
                        GoodsText = pas::concat_wide({GoodsText, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(255)), u"><align=right>", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Planet))), u""sv), u"</align>"});
                    }
                }
                Description = pas::concat_wide({Description, GoodsText});
                Heading = aMyFunction::WrapTextInColor(pas::view(pas::concat_wide({u"- ", aMyFunction::WrapTextInColor(pas::view(Planet->GetFullName(u" "_w)), u"<color=255,240,100>"sv), u" -"})), u"<color=255,240,100>"sv);
                Self->AddInfoHeading(Heading, pas::concat_wide({Heading, u"\r\n", Description}), 0, 0, Planet->Id);
                Self->AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                Self->AddPlanetInfoText(pas::checked_cast<aPlanet::TPlanet*>(Value), Description);
            } else if (pas::class_cast_if<aGalaxy::TStar*>(Value) != nullptr) {
                Description = ([&] {
                    pas::WideString name_8 = Star->Constellation->GetName();
                    pas::WideString localizedText_9 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedText_9), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name_8));
                }());
                Heading = ([&] {
                    auto name_9 = pas::borrow(pas::checked_cast<aGalaxy::TStar*>(Value)->Name);
                    pas::WideString cpp_arg = pas::concat_wide({u"- ", aConst::LocalizedText(u"FormInfo.StarInfo.StarName"_wref.get()), u" -"});
                    return aMyFunction::FormatText1(std::move(cpp_arg), u"<color=255,240,100>"_w, u"<StarName>"_w, name_9.get());
                }());
                if (reinterpret_cast<aGalaxy::TStar*>(Value)->Status.CustomFaction != u"") {
                    Description = pas::concat_wide({Description, u"\r\n", aConst::LocalizedText(pas::concat_wide({u"FormInfo.StarInfo.ControlledBy", reinterpret_cast<aGalaxy::TStar*>(Value)->Status.CustomFaction}))});
                } else {
                    switch (reinterpret_cast<aGalaxy::TStar*>(Value)->Status.ControlFaction) {
                        case aGalaxyStruct::sfCoalition: {
                            Description = pas::concat_wide({Description, u"\r\n", aConst::LocalizedText(u"FormInfo.StarInfo.ControlledByCoalition"_wref.get())});
                            break;
                        }
                        case aGalaxyStruct::sfPirates: {
                            Description = pas::concat_wide({Description, u"\r\n", aConst::LocalizedText(u"FormInfo.StarInfo.ControlledByPirates"_wref.get())});
                            break;
                        }
                        case aGalaxyStruct::sfDominators: {
                            Description = pas::concat_wide({Description, u"\r\n", aConst::LocalizedText(u"FormInfo.StarInfo.ControlledByDominators"_wref.get())});
                            break;
                        }
                    }
                }
                if (aPlayer::GetPlayer()->CurrentStar != Value) {
                    Description = pas::concat_wide({Description, u"\r\n", aConst::LocalizedText(u"FormInfo.StarInfo.RelativeLocation"_wref.get())});
                    Description = pas::concat_wide({Description, u"\r\n", ([&] {
                        pas::WideString int64ToStr = pas::wide_int64_to_str(System::Round(aMyFunction::PointDistance(pas::checked_cast<aGalaxy::TStar*>(Value)->Position, aPlayer::GetPlayer()->CurrentStar->Position)));
                        pas::WideString localizedText_10 = aConst::LocalizedText(u"FormInfo.StarInfo.RelativeLocationDist"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_10), u"<color=255,240,100>"_w, u"<Dist>"_w, std::move(int64ToStr));
                    }())});
                    Bearing = System::Round(aMyFunction::PointBearingDegrees(pas::checked_cast<aGalaxy::TStar*>(Value)->Position, aPlayer::GetPlayer()->CurrentStar->Position));
                    Bearing = Bearing + 180 - 15;
                    if (Bearing >= 360) {
                        Bearing -= 360;
                    }
                    Bearing = Bearing / 30 + 1;
                    if (Bearing == 1) {
                        Description = pas::concat_wide({Description, u"\r\n", ([&] {
                            pas::WideString intToStr_2 = pas::wide_int_to_str(Bearing);
                            pas::WideString localizedText_11 = aConst::LocalizedText(u"FormInfo.StarInfo.RelativeLocationAngle0"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText_11), u"<color=255,240,100>"_w, u"<Angle>"_w, std::move(intToStr_2));
                        }())});
                    }
                    if (Bearing > 1 && Bearing <= 4) {
                        Description = pas::concat_wide({Description, u"\r\n", ([&] {
                            pas::WideString intToStr_3 = pas::wide_int_to_str(Bearing);
                            pas::WideString localizedText_12 = aConst::LocalizedText(u"FormInfo.StarInfo.RelativeLocationAngle1"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText_12), u"<color=255,240,100>"_w, u"<Angle>"_w, std::move(intToStr_3));
                        }())});
                    }
                    if (Bearing > 4) {
                        Description = pas::concat_wide({Description, u"\r\n", ([&] {
                            pas::WideString intToStr_4 = pas::wide_int_to_str(Bearing);
                            pas::WideString localizedText_13 = aConst::LocalizedText(u"FormInfo.StarInfo.RelativeLocationAngle2"_wref.get());
                            return aMyFunction::FormatText1(std::move(localizedText_13), u"<color=255,240,100>"_w, u"<Angle>"_w, std::move(intToStr_4));
                        }())});
                    }
                }
                if (pas::checked_cast<aGalaxy::TStar*>(Value)->Status.Battle != 0) {
                    Description = pas::concat_wide({Description, u"\r\n", aConst::LocalizedText(u"FormInfo.StarInfo.BattleInSystem"_wref.get())});
                }
                Self->AddInfoHeading(Heading, pas::concat_wide({Heading, u"\r\n", Description}), 0, 0, 0);
                Self->AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                Self->AddStarInfoText(pas::checked_cast<aGalaxy::TStar*>(Value), Description);
            } else if (pas::class_cast_if<aItem::TEquipment*>(Value) != nullptr) {
                if (Planet != nullptr) {
                    Description = ([&] {
                        pas::WideString name_10 = Planet->CurrentStar->Constellation->GetName();
                        pas::WideString localizedText_14 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_14), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name_10));
                    }());
                    Description = pas::concat_wide({Description, u"\r\n", ([&] {
                        auto name_11 = pas::borrow(Planet->CurrentStar->Name);
                        pas::WideString localizedText_15 = aConst::LocalizedText(u"FormInfo.Star"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_15), u"<color=255,240,100>"_w, u"<StarName>"_w, name_11.get());
                    }())});
                    Description = pas::concat_wide({Description, u"\r\n", ([&] {
                        auto name_12 = pas::borrow(Planet->Name);
                        pas::WideString localizedText_16 = aConst::LocalizedText(u"FormInfo.Planet"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_16), u"<color=255,240,100>"_w, u"<PlanetName>"_w, name_12.get());
                    }())});
                } else {
                    Description = ([&] {
                        pas::WideString name_13 = Ship->CurrentStar->Constellation->GetName();
                        pas::WideString localizedText_17 = aConst::LocalizedText(u"FormInfo.Sector"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_17), u"<color=255,240,100>"_w, u"<SectorName>"_w, std::move(name_13));
                    }());
                    Description = pas::concat_wide({Description, u"\r\n", ([&] {
                        auto name_14 = pas::borrow(Ship->CurrentStar->Name);
                        pas::WideString localizedText_18 = aConst::LocalizedText(u"FormInfo.Star"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_18), u"<color=255,240,100>"_w, u"<StarName>"_w, name_14.get());
                    }())});
                    Description = pas::concat_wide({Description, u"\r\n", aMyFunction::WrapTextInColor(pas::view(Ship->GetFullName(u" "_wref.get())), u"<color=255,240,100>"sv)});
                }
                Heading = aMyFunction::WrapTextInColor(pas::view(pas::concat_wide({u"- ", aMyFunction::WrapTextInColor(pas::view(EC_Str::RemoveTextTagsW(reinterpret_cast<aItem::TItem*>(Value)->GetDisplayName())), u"<color=255,240,100>"sv), u" [", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(reinterpret_cast<aItem::TItem*>(Value)->Weight)), u"<color=0,255,0>"sv), u"]", u" -"})), u"<color=255,240,100>"sv);
                Description = pas::concat_wide({Description, u"\r\n", fInfo::GetInfoEquipmentSummary(reinterpret_cast<aItem::TItem*>(Value))});
                Self->AddInfoHeading(Heading, pas::concat_wide({Heading, u"\r\n", Description}), 0, 0, 0);
                Self->AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
                if (aItem::THull* hull = pas::class_cast_if<aItem::THull*>(Value)) {
                    Self->AddEquipmentInfoText(static_cast<aItem::TItem*>(hull), Description);
                } else {
                    Self->AddItemInfoText(pas::checked_cast<aItem::TItem*>(Value), Description);
                }
            }
            Found = true;
            ++ResultCount;
        }
    }

    void TfInfo::AddEquipmentInfoText(aItem::TItem* Item, pas::WideString Text) {
        GI_GraphBuf::TGraphBufGI* Slots{};
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,240,100>"_wref.get(), u"<color=0,50,200>"sv);
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=0,255,0>"_wref.get(), u"<color=0,130,0>"sv);
        Text = EC_Str::ReplaceAllWideString(Text, u"<color=255,167,84>"_wref.get(), u"<color=240,100,30>"sv);
        std::int32_t Size = GR_Main::GiScalePixels(64);
        std::int32_t Height = Size;
        if (pas::class_cast_if<aItem::THull*>(Item) != nullptr) {
            Slots = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, InfoPanel, false);
            Slots->SetPositionModeW(true);
            Slots->SetPosition(ClassesImports::Point(InfoPanel->ClientSize.X - 80, InfoContentHeight));
            Slots->SetSize(ClassesImports::Point(80, 90));
            Slots->SourceHasPerPixelAlpha = true;
            GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.FormNote.", GR_Main::GiResourceSuffix(), u"HP_Bg"}), Slots->GraphBuf);
            Globals::EquipmentShopScreen->BuildHullSlotOverlays(InfoPanel, pas::checked_cast<aItem::THull*>(Item), InfoPanel->ClientSize.X - 80, InfoContentHeight);
            Height = 90;
        } else {
            static_cast<void>(pas::class_cast_if<aItem::TEquipment*>(Item) != nullptr);
        }
        GI_GraphBuf::TGraphBufGI* Image = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, InfoPanel, false);
        Image->SetPositionModeW(true);
        Image->SetPosition(ClassesImports::Point(0, InfoContentHeight));
        Image->SetSize(ClassesImports::Point(Size, Height));
        Image->SourceHasPerPixelAlpha = true;
        {
            const pas::WideString& cpp_arg = pas::concat_wide({Item->GetBitmapResourceName(), u"i"});
            GR_GraphBuf::TGraphBufGR* graphBuf = Image->GraphBuf;
            GI_GI::LoadGiByPathIntoGraphBuf(cpp_arg, graphBuf);
        }
        if (static_cast<std::uint32_t>(Image->GraphBuf->Width) >= static_cast<std::uint32_t>(Image->GraphBuf->Height)) {
            Image->GraphBuf->RescaleRgba(Image->ClientSize.X, System::Round(pas::real_divide(Image->ClientSize.X, static_cast<std::uint32_t>(Image->GraphBuf->Width)) * static_cast<std::uint32_t>(Image->GraphBuf->Height)), 5);
        } else {
            Image->GraphBuf->RescaleRgba(System::Round(pas::real_divide(Image->ClientSize.Y, static_cast<std::uint32_t>(Image->GraphBuf->Height)) * static_cast<std::uint32_t>(Image->GraphBuf->Width)), Image->ClientSize.Y, 5);
        }
        GI_Label::TLabelGI* Caption = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, InfoPanel);
        if (GR_Main::GiResourceVariant() == 2 || pas::class_cast_if<aItem::THull*>(Item) != nullptr) {
            Caption->SetFontName(GlobalsV::NormalFontName);
        } else {
            Caption->SetFontName(GlobalsV::SmallFontName);
        }
        Caption->SetPosition(ClassesImports::Point(Size + 10, InfoContentHeight));
        Caption->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - Caption->LocalPosition.X - 70, Height));
        Caption->SetWordWrapEnabled(true);
        Caption->SetPositionModeW(true);
        Caption->SetTextAlignX(GI_Main::taxLeft);
        Caption->SetTextAlignY(GI_Main::tayCenter);
        Caption->SetText(Text);
        Caption->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        InfoContentHeight = InfoContentHeight + Height + 5;
    }

    // The explicit receiver value preserves native loading before the False argument.
    void TfInfo::ToggleVisibleBookmark() {
        std::int32_t I{};
        std::int32_t ScrollPosition{};
        GI_GraphButton::TGraphButtonGI* Button{};
        std::uint8_t Finished{};
        Globals::TMessagePlayer* Entry{};
        if (SearchMode) {
            ScrollPosition = InfoPanel->VerticalScrollBar->Position;
            I = 0;
            Finished = false;
            Button = nullptr;
            while (!Finished) {
                Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(InfoPanel->FindByNameRecursive(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"MemBtn", SysUtils::IntToStr(I)})))));
                if (Button == nullptr) {
                    return;
                }
                if (Button->LocalPosition.Y >= ScrollPosition) {
                    break;
                }
                ++I;
            }
            if (Button != nullptr) {
                Entry = Globals::FindPlayerBubbleByText(Button->HelpText, false);
                if (Entry != nullptr) {
                    Globals::RemovePersistentPlayerMessage(Entry, false);
                    MainPanel->Screen->GetByName(u"PM_WinMsg"sv)->SetActive(false);
                    reinterpret_cast<fPanelMain::TfPanelMain*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(MainPanel)) + 0)))->RebuildMessageButtons(false);
                    Button->SetDisabled(false);
                    GR_Main::SoundManager->PlaySound(u"Sound.DelMsg"_wref.get());
                } else {
                    BookmarkClicked(Button);
                }
            }
        }
    }

    std::uint8_t TfInfo::IsAtBusinessCenter() {
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->DockedTo != nullptr && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstRangerCenter)) {
            return true;
        }
        return Result;
    }

    void TfInfo::CategoryStateChanged(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        GI_GraphButton::TGraphButtonGI* Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Sender);
        if (I >= 1 && I <= 7) {
            GI_MessageLoop::TObjectGI* ImgMM_C1 = GetByName(u"ImgMM_C1"sv);
            ImgMM_C1->Invalidate();
            ImgMM_C1->SetActive(Button->IsHovered());
        }
        if (I >= 8 && I <= 10) {
            GI_MessageLoop::TObjectGI* ImgMM_C2 = GetByName(u"ImgMM_C2"sv);
            ImgMM_C2->Invalidate();
            ImgMM_C2->SetActive(Button->IsHovered());
        }
    }

    void TfInfo::CategoryClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        if (static_cast<std::uint8_t>(InfoPanel->Active ^ 1) || SelectedSearchCategory == 14) {
            SelectedSearchCategory = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
            if (SelectedSearchCategory != 0 && SelectedSearchCategory != PreviousSearchCategory) {
                HasSearchResults = false;
            }
        }
        if (SelectedSearchCategory != 0) {
            PreviousSearchCategory = SelectedSearchCategory;
        }
        InfoPanel->SetActive(false);
        InfoPanel->VerticalScrollBar->SetActive(false);
        {
            GI_MessageLoop::TObjectGI* PanelSearch = GetByName(u"PanelSearch"sv);
            PanelSearch->SetActive(SelectedSearchCategory == 0);
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 15); cpp_range.next(I); ) {
            GI_MessageLoop::TObjectGI* cpp_with_2 = GetByName(pas::view(pas::concat_wide({u"PanelM", EC_Str::IntToFixedWidthWideString(I, 2)})));
            cpp_with_2->SetActive(SelectedSearchCategory == I);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButPrev = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPrev"sv));
            ButPrev->SetDisabled(SelectedSearchCategory == 0);
        }
        {
            GI_GraphButton::TGraphButtonGI* ButNext = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNext"sv));
            ButNext->SetDisabled(!(SelectedSearchCategory == 0 && (PreviousSearchCategory != 0 || HasSearchResults) || SelectedSearchCategory != 0 && HasSearchResults));
        }
        FocusSearchField(true);
        if (SelectedSearchCategory == 14) {
            InfoPanel->SetActive(true);
            InfoPanel->VerticalScrollBar->SetActive(true);
            ClearInfoContents();
            AddInfoSpacing(2);
            {
                auto normalBoldFontName = pas::borrow(GlobalsV::NormalBoldFontName);
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormInfo.SearchInfo1"_wref.get());
                AddInfoText(std::move(localizedColorText), GI_Main::taxCenter, normalBoldFontName.get());
            }
            AddInfoSpacing(10);
            AddInfoSeparator();
            AddInfoSpacing(5);
            {
                auto smallBoldFontName = pas::borrow(GlobalsV::SmallBoldFontName);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormInfo.SearchInfo6"_wref.get());
                AddInfoText(std::move(localizedColorText_2), GI_Main::taxCenter, smallBoldFontName.get());
            }
            AddInfoSpacing(10);
            AddInfoSeparator();
            AddInfoSpacing(10);
            AddSearchPriceLabel(aConst::LocalizedColorText(u"FormInfo.SearchInfo2"_wref.get()));
            AddInfoSpacing(5);
            {
                auto smallFontName = pas::borrow(GlobalsV::SmallFontName);
                pas::WideString formatText1 = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(TfInfo::GetSearchResultLimit());
                    pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormInfo.SearchInfo3"_wref.get());
                    return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=0,50,200>"_w, u"<Count>"_w, std::move(intToStr));
                }());
                AddInfoText(std::move(formatText1), GI_Main::taxAuto, smallFontName.get());
            }
            AddInfoSpacing(10);
            AddInfoSeparator();
            AddInfoSpacing(5);
            {
                auto smallBoldFontName_2 = pas::borrow(GlobalsV::SmallBoldFontName);
                pas::WideString localizedColorText_4 = aConst::LocalizedColorText(u"FormInfo.SearchInfo7"_wref.get());
                AddInfoText(std::move(localizedColorText_4), GI_Main::taxLeft, smallBoldFontName_2.get());
            }
            AddInfoSpacing(5);
            {
                auto smallFontName_2 = pas::borrow(GlobalsV::SmallFontName);
                pas::WideString localizedColorText_5 = aConst::LocalizedColorText(u"FormInfo.SearchInfo8"_wref.get());
                AddInfoText(std::move(localizedColorText_5), GI_Main::taxLeft, smallFontName_2.get());
            }
            AddInfoSpacing(10);
            AddInfoSeparator();
            AddInfoSpacing(10);
            {
                auto normalFontName = pas::borrow(GlobalsV::NormalFontName);
                pas::WideString localizedColorText_6 = aConst::LocalizedColorText(u"FormInfo.SearchInfo4"_wref.get());
                AddInfoText(std::move(localizedColorText_6), GI_Main::taxCenter, normalFontName.get());
            }
            AddInfoText(u" ."_w, GI_Main::taxCenter, pas::WideString());
            FinishInfoLayout();
        }
    }

    void TfInfo::NextSearchPageClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t I{};
        if (SelectedSearchCategory == 0 && PreviousSearchCategory != 0 && PreviousSearchCategory != 14) {
            CategoryClicked(GetByName(pas::view(pas::concat_wide({u"PanelM", EC_Str::IntToFixedWidthWideString(PreviousSearchCategory, 2)}))));
        } else {
            InfoPanel->VerticalScrollBar->SetActive(true);
            InfoPanel->SetActive(true);
            GetByName(u"PanelSearch"sv)->SetActive(false);
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 15); cpp_range.next(I); ) {
                GI_MessageLoop::TObjectGI* cpp_with = GetByName(pas::view(pas::concat_wide({u"PanelM", EC_Str::IntToFixedWidthWideString(I, 2)})));
                cpp_with->SetActive(false);
            }
            {
                GI_GraphButton::TGraphButtonGI* ButPrev = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPrev"sv));
                ButPrev->SetDisabled(false);
            }
            {
                GI_GraphButton::TGraphButtonGI* ButNext = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButNext"sv));
                ButNext->SetDisabled(true);
            }
            FinishInfoLayout();
        }
    }

    void TfInfo::BindFilterLabels(GI_MessageLoop::TObjectGI* Parent) {
        GI_MessageLoop::TObjectGI* Caption{};
        GI_GraphButton::TGraphButtonGI* Button{};
        std::int32_t Y{};
        std::int32_t X{};
        GI_MessageLoop::TObjectGI* Child = Parent->FirstChild;
        while (Child != nullptr) {
            if (GI_GraphButton::TGraphButtonGI* graphButtonGI = pas::class_cast_if<GI_GraphButton::TGraphButtonGI*>(Child)) {
                Button = graphButtonGI;
                if (Button->ImageNormal != nullptr && EC_Str::FindTextOffsetW(Button->ImageNormal->GetImagePath(), u"Check"_wref.get(), 0) >= 0) {
                    X = Button->LocalPosition.X + Button->ClientSize.X + GR_Main::GiScalePixels(20);
                    Y = Button->ClientSize.Y / 2 + Button->LocalPosition.Y;
                    Caption = Parent->FirstChild;
                    while (Caption != nullptr) {
                        if (Caption != Child && pas::class_cast_if<GI_Label::TLabelGI*>(Caption) != nullptr && X >= Caption->LocalPosition.X && X < Caption->LocalPosition.X + Caption->ClientSize.X && Y >= Caption->LocalPosition.Y && Y < Caption->LocalPosition.Y + Caption->ClientSize.Y) {
                            break;
                        }
                        Caption = Caption->NextSibling;
                    }
                    if (Caption != nullptr) {
                        Caption->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Button));
                        Caption->MouseEnterCallback = pas::bind_static_method<&TfInfo::FilterLabelMouseEnter>(this);
                        Caption->MouseLeaveCallback = pas::bind_static_method<&TfInfo::FilterLabelMouseLeave>(this);
                        Caption->LeftButtonDownCallback = pas::bind_static_method<&TfInfo::FilterLabelMouseDown>(this);
                        Caption->LeftButtonUpCallback = pas::bind_static_method<&TfInfo::FilterLabelMouseUp>(this);
                    }
                }
            }
            BindFilterLabels(Child);
            Child = Child->NextSibling;
        }
    }

    void TfInfo::FilterLabelMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        GI_GraphButton::TGraphButtonGI* cpp_with = reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        cpp_with->SetHovered(true);
    }

    void TfInfo::FilterLabelMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        GI_GraphButton::TGraphButtonGI* cpp_with = reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        cpp_with->SetHovered(false);
    }

    void TfInfo::FilterLabelMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
    }

    void TfInfo::FilterLabelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        GI_GraphButton::TGraphButtonGI* cpp_with = reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        cpp_with->SetDown(static_cast<std::uint8_t>(cpp_with->Down ^ 1));
    }

    void TfInfo::ClearSearchText(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* TextSearch = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextSearch"sv));
        TextSearch->SetText(pas::WideString());
    }

    void TfInfo::CopySearchText(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* TextSearch = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextSearch"sv));
        GR_Main::SetClipboardWideText(TextSearch->Text);
    }

    void TfInfo::PasteSearchText(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* TextSearch = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"TextSearch"sv));
        TextSearch->SetText(GR_Main::GetClipboardWideText());
    }

    void TfInfo::ClearSearch12Name(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* M12Name = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12Name"sv));
        M12Name->SetText(pas::WideString());
    }

    void TfInfo::CopySearch12Name(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* M12Name = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12Name"sv));
        GR_Main::SetClipboardWideText(M12Name->Text);
    }

    void TfInfo::PasteSearch12Name(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* M12Name = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M12Name"sv));
        M12Name->SetText(GR_Main::GetClipboardWideText());
    }

    void TfInfo::ClearSearch15Name(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* M15Name = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M15Name"sv));
        M15Name->SetText(pas::WideString());
    }

    void TfInfo::CopySearch15Name(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* M15Name = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M15Name"sv));
        GR_Main::SetClipboardWideText(M15Name->Text);
    }

    void TfInfo::PasteSearch15Name(GI_MessageLoop::TObjectGI* Sender) {
        GI_Edit::TEditGI* M15Name = pas::checked_cast<GI_Edit::TEditGI*>(GetByName(u"M15Name"sv));
        M15Name->SetText(GR_Main::GetClipboardWideText());
    }

    void TfInfo::ClearSearchField(pas::WideString Name) {
        pas::checked_cast<GI_Edit::TEditGI*>(GetByName(pas::view(Name)))->SetText(pas::WideString());
    }

    void TfInfo::ClearSearch01Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M01Speed"_w);
        ClearSearchField(u"M01Range"_w);
        ClearSearchField(u"M01Size"_w);
        ClearSearchField(u"M01Cost"_w);
    }

    void TfInfo::ClearSearch02Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M02Capacity"_w);
        ClearSearchField(u"M02Size"_w);
        ClearSearchField(u"M02Cost"_w);
    }

    void TfInfo::ClearSearch03Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M03Range"_w);
        ClearSearchField(u"M03Size"_w);
        ClearSearchField(u"M03Cost"_w);
    }

    void TfInfo::ClearSearch04Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M04Power"_w);
        ClearSearchField(u"M04Size"_w);
        ClearSearchField(u"M04Cost"_w);
    }

    void TfInfo::ClearSearch05Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M05Power"_w);
        ClearSearchField(u"M05Size"_w);
        ClearSearchField(u"M05Cost"_w);
    }

    void TfInfo::ClearSearch06Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M06ObjSize"_w);
        ClearSearchField(u"M06Size"_w);
        ClearSearchField(u"M06Cost"_w);
    }

    void TfInfo::ClearSearch07Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M07Block"_w);
        ClearSearchField(u"M07Size"_w);
        ClearSearchField(u"M07Cost"_w);
    }

    void TfInfo::ClearSearch09Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M09Const"_w);
        ClearSearchField(u"M09Star"_w);
        ClearSearchField(u"M09Range"_w);
    }

    void TfInfo::ClearSearch10Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M10Const"_w);
        ClearSearchField(u"M10Star"_w);
        ClearSearchField(u"M10Range"_w);
    }

    void TfInfo::ClearSearch11Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M11Size"_w);
        ClearSearchField(u"M11Def"_w);
        ClearSearchField(u"M11Cost"_w);
    }

    void TfInfo::ClearSearch12Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M12DamageMin"_w);
        ClearSearchField(u"M12DamageMax"_w);
        ClearSearchField(u"M12Range"_w);
        ClearSearchField(u"M12Name"_w);
        ClearSearchField(u"M12Size"_w);
        ClearSearchField(u"M12Cost"_w);
    }

    void TfInfo::ClearSearch13Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M13Range"_w);
        ClearSearchField(u"M13PriceSell"_w);
        ClearSearchField(u"M13Cnt"_w);
        ClearSearchField(u"M13PriceBuy"_w);
    }

    void TfInfo::ClearSearch15Filters(GI_MessageLoop::TObjectGI* Sender) {
        ClearSearchField(u"M15Const"_w);
        ClearSearchField(u"M15Star"_w);
        ClearSearchField(u"M15Name"_w);
    }

    std::int32_t TfInfo::GetSearchResultLimit() {
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->DockedTo != nullptr && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstRangerCenter)) {
            return GlobalsV::MaxSearchResult;
        }
        return 30;
    }

    void TfInfo::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10010);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20010);
        }
    }

    void TfInfo::p_destroy() {
        fInfo::TfInfo_Destroy(this);
    }

} // namespace fInfo
