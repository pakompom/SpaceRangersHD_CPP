#include "layout/fGoodsShop2.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Data.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_GraphBuf.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_Label.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Space.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRanger.hpp"
#include "types/aRuins.hpp"
#include "types/fRuinsTalk.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GR_Main.hpp"
#include "units/GR_Sound.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fCount2.hpp"
#include "units/fEquipmentShop.hpp"
#include "units/fGoodsShop2.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fPanelPlanet.hpp"
#include "units/fPanelRuins.hpp"
#include "units/fShip2.hpp"

namespace fGoodsShop2 {
    // Native unit-local copy of the goods presentation order.
    const aGalaxyStruct::TGoodsTextOrder ShopGoodsOrder = aGalaxyStruct::TGoodsTextOrder{{static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(5), static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(3), static_cast<std::uint8_t>(2), static_cast<std::uint8_t>(6), static_cast<std::uint8_t>(7)}};

    // Native style-derived out-of-stock price markup.
    pas::WideString OutOfStockColor{};

    // Native modal wrapper used while talking to another ship.
    std::uint8_t RunGoodsShop(GI_MessageLoop::TMessageLoopGI* ParentLoop) {
        std::uint8_t Result{};
        ParentLoop->RootUiObject->NativeHook50();
        Globals::GoodsShopScreen->ParentLoop = ParentLoop;
        ParentLoop->ChildLoop = Globals::GoodsShopScreen;
        if (Globals::GoodsShopScreen->Run() == 1) {
            Result = true;
        } else {
            Result = false;
        }
        Globals::GoodsShopScreen->ParentLoop = nullptr;
        ParentLoop->ChildLoop = nullptr;
        ParentLoop->RootUiObject->NativeHook48();
        return Result;
    }

    void TfGoodsShop2_Create(TfGoodsShop2* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->PlanetPanel = pas::construct_call<fPanelPlanet::TfPanelPlanet>(fPanelPlanet::TfPanelPlanet_Create);
        Self->StationPanel = pas::construct_call<fPanelRuins::TfPanelRuins>(fPanelRuins::TfPanelRuins_Create);
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
        Self->AmbientSound = pas::construct_call<GR_Sound::TSoundBufferControl>(GR_Sound::TSoundBufferControl_Create);
        Self->AmbientSound->Configure(u"Sound.GoodsLoop"_wref.get(), 0, true);
    }

    void TfGoodsShop2_Destroy(TfGoodsShop2* Self) {
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
        pas::free(Self->AmbientSound);
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfGoodsShop2::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        PlanetPanel->InitializeLayout(this);
        StationPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        SetHelpCallback(pas::bind_method<&TfGoodsShop2::ShowControlHelp>(this));
        GR_Main::AppendLogTextThreadSafe("fGoodsShop2... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGCity2"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGCity"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ADD_WarningSpace = MainPanel->FindByNameRecursive(u"ADD_WarningSpace"_wref.get());
                ADD_WarningSpace->SetPosition(ClassesImports::Point(ADD_WarningSpace->LocalPosition.X + GR_Main::ExtraScreenWidth, ADD_WarningSpace->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ADD_WarningMoney = MainPanel->FindByNameRecursive(u"ADD_WarningMoney"_wref.get());
                ADD_WarningMoney->SetPosition(ClassesImports::Point(ADD_WarningMoney->LocalPosition.X + GR_Main::ExtraScreenWidth, ADD_WarningMoney->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ADD_Space = MainPanel->FindByNameRecursive(u"ADD_Space"_wref.get());
                ADD_Space->SetPosition(ClassesImports::Point(ADD_Space->LocalPosition.X + GR_Main::ExtraScreenWidth, ADD_Space->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* ADD_Money = MainPanel->FindByNameRecursive(u"ADD_Money"_wref.get());
                ADD_Money->SetPosition(ClassesImports::Point(ADD_Money->LocalPosition.X + GR_Main::ExtraScreenWidth, ADD_Money->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* GS_Help = MainPanel->FindByNameRecursive(u"GS_Help"_wref.get());
                GS_Help->SetPosition(ClassesImports::Point(GS_Help->LocalPosition.X, GS_Help->LocalPosition.Y + GR_Main::ExtraScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* GoodsPanel = MainPanel->FindByNameRecursive(u"GoodsPanel"_wref.get());
                GoodsPanel->SetPosition(ClassesImports::Point(GoodsPanel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, GoodsPanel->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            MainPanel->FindByNameRecursive(u"BGShrLight"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->UpCallback = pas::bind_method<&TfGoodsShop2::EndTurnClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"_wref.get()))->UpCallback = pas::bind_method<&TfGoodsShop2::ShipClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Gal"_wref.get()))->UpCallback = pas::bind_method<&TfGoodsShop2::GalaxyClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Quest"_wref.get()))->UpCallback = pas::bind_method<&TfGoodsShop2::QuestClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Logo"_wref.get()))->UpCallback = pas::bind_method<&TfGoodsShop2::MenuClicked>(this);
        GetByName(u"MainPanel"_wref.get())->KeyDownCallback = pas::bind_method<&TfGoodsShop2::MainPanelKeyDown>(this);
        GetByName(u"MainPanel"_wref.get())->MouseMoveCallback = pas::bind_method<&TfGoodsShop2::GoodsMouseMove>(this);
        GetByName(u"MainPanel"_wref.get())->LeftButtonUpCallback = pas::bind_method<&TfGoodsShop2::GoodsMouseUp>(this);
        GetByName(u"MainPanel"_wref.get())->RightButtonDownCallback = pas::bind_method<&TfGoodsShop2::GoodsRightMouseDown>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()))->UpCallback = pas::bind_method<&TfGoodsShop2::SavePricesClicked>(this);
        NameFaceHeight = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()))->ClientSize.Y;
        FaceCaptionHeight = ([&] {
            std::int32_t cpp_left = ([&] {
                std::int32_t cpp_left_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharFace"_wref.get()))->LocalPosition.Y;
                return cpp_left_2 + pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharFace"_wref.get()))->ClientSize.Y;
            }());
            return cpp_left - pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()))->LocalPosition.Y;
        }());
        OutOfStockColor = GR_Main::GetStyleColorTagGI(u"GoodsShop.GoodsColorOutOfStock"_w, 127, 127, 127);
    }

    void TfGoodsShop2::OnOpen() {
        std::int32_t I{};
        pas::WideString BackgroundPath{};
        if (!FlagEC) {
            LoadPanel->OnOpen();
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        if (aPlayer::GetPlayer()->IsOnPlanet() || aPlayer::GetPlayer()->IsDockedToShip()) {
            MainPanel->Show();
            MainPanel->OnOpen();
        } else {
            MainPanel->Hide();
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 17); cpp_range.next(I); ) {
            if (I != 8 && I != 9) {
                {
                    GI_Label::TLabelGI* cpp_with = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovCnt", SysUtils::IntToStr(I)}))));
                    if (GlobalsV::FontDialog == 0) {
                        cpp_with->SetFontName(GlobalsV::NormalFontName);
                    } else if (GlobalsV::FontDialog == 1) {
                        cpp_with->SetFontName(GlobalsV::SmoothBigFontName);
                    } else if (GlobalsV::FontDialog == 2) {
                        cpp_with->SetFontName(GlobalsV::SmoothHugeFontName);
                    } else if (GlobalsV::FontDialog >= 3) {
                        cpp_with->SetFontName(GlobalsV::SmoothIntroFontName);
                    }
                }
                {
                    GI_Label::TLabelGI* cpp_with_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovPrice", SysUtils::IntToStr(I)}))));
                    if (GlobalsV::FontDialog == 0) {
                        cpp_with_2->SetFontName(GlobalsV::NormalFontName);
                    } else if (GlobalsV::FontDialog == 1) {
                        cpp_with_2->SetFontName(GlobalsV::SmoothBigFontName);
                    } else if (GlobalsV::FontDialog == 2) {
                        cpp_with_2->SetFontName(GlobalsV::SmoothHugeFontName);
                    } else if (GlobalsV::FontDialog >= 3) {
                        cpp_with_2->SetFontName(GlobalsV::SmoothIntroFontName);
                    }
                }
            }
        }
        GetByName(u"GS_Help"_wref.get())->SetActive(false);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()))->SetDisabled(false);
        {
            GI_GraphButton::TGraphButtonGI* ButFormClose = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButFormClose"_wref.get()));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                ButFormClose->UpCallback = pas::bind_method<&fPanelPlanet::TfPanelPlanet::PlanetClicked>(PlanetPanel);
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                ButFormClose->UpCallback = pas::bind_method<&fPanelRuins::TfPanelRuins::ServicesClicked>(StationPanel);
            } else {
                ButFormClose->UpCallback = pas::bind_method<&TfGoodsShop2::CloseClicked>(this);
            }
        }
        DraggedGoodsIndex = -1;
        {
            WindowsSdk::TPoint point = ClassesImports::Point(pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()))->ClientSize.X, NameFaceHeight);
            GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()));
            cpp_arg->SetSize(point);
        }
        GetByName(u"GraphBufFace"_wref.get())->SetActive(false);
        GetByName(u"CloseLine"_wref.get())->SetActive(false);
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            {
                const pas::WideString& fullName = aPlayer::GetPlayer()->CurrentPlanet->GetFullName(u"\r\n"_w);
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()));
                cpp_arg_2->SetText(fullName);
            }
            if (aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharFace"_wref.get()))->SetText(u""_wref.get());
            } else {
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharFace"_wref.get()));
                const pas::WideString& cpp_arg_4 = pas::concat_wide({aConst::PlanetEconomyInfo[aPlayer::GetPlayer()->CurrentPlanet->Economy].ShortDisplayName, u"\r\n", aConst::PlanetGovernmentMarket[aPlayer::GetPlayer()->CurrentPlanet->Government].DisplayName});
                cpp_arg_3->SetText(cpp_arg_4);
            }
            {
                GI_Image::TImageGI* cpp_arg_5 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageFace"_wref.get()));
                pas::WideString cpp_arg_6 = pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), u"PlanetL"});
                cpp_arg_5->SetImagePath(std::move(cpp_arg_6));
            }
            {
                GI_GraphBuf::TGraphBufGI* GraphBufFace = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GraphBufFace"_wref.get()));
                GraphBufFace->SetActive(true);
                GraphBufFace->SourceHasPerPixelAlpha = true;
                aPlayer::GetPlayer()->CurrentPlanet->Graphic->RenderToBuffer(this, GraphBufFace->GraphBuf, false);
                GraphBufFace->GraphBuf->RescaleBilinearRgba(GraphBufFace->ClientSize.X, GraphBufFace->ClientSize.Y);
            }
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            {
                const pas::WideString& fullName_2 = aPlayer::GetPlayer()->DockedTo->GetFullName(u"\r\n"_wref.get());
                GI_Label::TLabelGI* cpp_arg_7 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()));
                cpp_arg_7->SetText(fullName_2);
            }
            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharFace"_wref.get()))->SetText(u""_wref.get());
            GetByName(u"CloseLine"_wref.get())->SetActive(true);
            {
                WindowsSdk::TPoint point_2 = ClassesImports::Point(pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()))->ClientSize.X, FaceCaptionHeight);
                GI_Label::TLabelGI* cpp_arg_8 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()));
                cpp_arg_8->SetSize(point_2);
            }
            {
                GI_Image::TImageGI* cpp_arg_9 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageFace"_wref.get()));
                pas::WideString cpp_arg_10 = pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), u"AllL"});
                cpp_arg_9->SetImagePath(std::move(cpp_arg_10));
            }
            {
                GI_GraphBuf::TGraphBufGI* GraphBufFace_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GraphBufFace"_wref.get()));
                GraphBufFace_2->SetActive(true);
                GraphBufFace_2->SourceHasPerPixelAlpha = true;
                if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(aPlayer::GetPlayer()->DockedTo->Graphic) != nullptr) {
                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(aPlayer::GetPlayer()->DockedTo->Graphic)->StaticImagePath, 1, u","_wref.get()), GraphBufFace_2->GraphBuf);
                } else {
                    const pas::WideString& extractDelimitedPartW = EC_Str::ExtractDelimitedPartW(aPlayer::GetPlayer()->DockedTo->GetShipPortraitImagePath(), 1, u","_wref.get());
                    GR_GraphBuf::TGraphBufGR* graphBuf = GraphBufFace_2->GraphBuf;
                    GI_GI::LoadGiByPathIntoGraphBuf(extractDelimitedPartW, graphBuf);
                }
                if (static_cast<std::uint32_t>(GraphBufFace_2->GraphBuf->Width) >= static_cast<std::uint32_t>(GraphBufFace_2->GraphBuf->Height)) {
                    GraphBufFace_2->GraphBuf->RescaleRgba(GraphBufFace_2->ClientSize.X, System::Round(pas::real_divide(GraphBufFace_2->ClientSize.X, static_cast<std::uint32_t>(GraphBufFace_2->GraphBuf->Width)) * static_cast<std::uint32_t>(GraphBufFace_2->GraphBuf->Height)), 5);
                } else {
                    GraphBufFace_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(GraphBufFace_2->ClientSize.Y, static_cast<std::uint32_t>(GraphBufFace_2->GraphBuf->Height)) * static_cast<std::uint32_t>(GraphBufFace_2->GraphBuf->Width)), GraphBufFace_2->ClientSize.Y, 5);
                }
            }
        } else if (aPlayer::GetPlayer()->InNormalSpace()) {
            if (pas::class_cast_if<aRanger::TRanger*>(Globals::TalkShip) != nullptr) {
                {
                    const pas::WideString& fullName_3 = Globals::TalkShip->GetFullName(u"\r\n"_wref.get());
                    GI_Label::TLabelGI* cpp_arg_11 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()));
                    cpp_arg_11->SetText(fullName_3);
                }
                {
                    const pas::WideString& characterName = pas::checked_cast<aRanger::TRanger*>(Globals::TalkShip)->GetCharacterName();
                    GI_Label::TLabelGI* cpp_arg_12 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharFace"_wref.get()));
                    cpp_arg_12->SetText(characterName);
                }
            } else {
                {
                    const pas::WideString& fullName_4 = Globals::TalkShip->GetFullName(u"\r\n"_wref.get());
                    GI_Label::TLabelGI* cpp_arg_13 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()));
                    cpp_arg_13->SetText(fullName_4);
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharFace"_wref.get()))->SetText(u""_wref.get());
                GetByName(u"CloseLine"_wref.get())->SetActive(true);
                {
                    WindowsSdk::TPoint point_3 = ClassesImports::Point(pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()))->ClientSize.X, FaceCaptionHeight);
                    GI_Label::TLabelGI* cpp_arg_14 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameFace"_wref.get()));
                    cpp_arg_14->SetSize(point_3);
                }
            }
            {
                GI_Image::TImageGI* cpp_arg_15 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageFace"_wref.get()));
                pas::WideString cpp_arg_16 = pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), u"AllL"});
                cpp_arg_15->SetImagePath(std::move(cpp_arg_16));
            }
            {
                GI_GraphBuf::TGraphBufGI* GraphBufFace_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GraphBufFace"_wref.get()));
                GraphBufFace_3->SetActive(true);
                GraphBufFace_3->SourceHasPerPixelAlpha = true;
                if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(Globals::TalkShip->Graphic) != nullptr) {
                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(Globals::TalkShip->Graphic)->StaticImagePath, 1, u","_wref.get()), GraphBufFace_3->GraphBuf);
                } else {
                    const pas::WideString& extractDelimitedPartW_2 = EC_Str::ExtractDelimitedPartW(Globals::TalkShip->GetShipPortraitImagePath(), 1, u","_wref.get());
                    GR_GraphBuf::TGraphBufGR* graphBuf_2 = GraphBufFace_3->GraphBuf;
                    GI_GI::LoadGiByPathIntoGraphBuf(extractDelimitedPartW_2, graphBuf_2);
                }
                if (static_cast<std::uint32_t>(GraphBufFace_3->GraphBuf->Width) >= static_cast<std::uint32_t>(GraphBufFace_3->GraphBuf->Height)) {
                    GraphBufFace_3->GraphBuf->RescaleRgba(GraphBufFace_3->ClientSize.X, System::Round(pas::real_divide(GraphBufFace_3->ClientSize.X, static_cast<std::uint32_t>(GraphBufFace_3->GraphBuf->Width)) * static_cast<std::uint32_t>(GraphBufFace_3->GraphBuf->Height)), 5);
                } else {
                    GraphBufFace_3->GraphBuf->RescaleRgba(System::Round(pas::real_divide(GraphBufFace_3->ClientSize.Y, static_cast<std::uint32_t>(GraphBufFace_3->GraphBuf->Height)) * static_cast<std::uint32_t>(GraphBufFace_3->GraphBuf->Width)), GraphBufFace_3->ClientSize.Y, 5);
                }
            }
        }
        if (FlagEC) {
            {
                GI_GAI::TgaiGI* FaceA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
                FaceA->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                FaceA->RestartPlayback();
            }
            {
                GI_GAI::TgaiGI* CaptainA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"_wref.get()));
                CaptainA->RestartPlayback();
            }
        } else {
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                {
                    GI_Image::TImageGI* FaceI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"FaceI"_wref.get()));
                    FaceI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), u"ShopBot1i"}));
                    FaceI->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                    FaceI->SetImageKindX(GI_Main::ikxCenter);
                    FaceI->SetImageKindY(GI_Main::ikyCenter);
                    FaceI->SetActive(true);
                }
                {
                    GI_GAI::TgaiGI* FaceA_2 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
                    FaceA_2->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                    FaceA_2->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), u"ShopBot1a"}));
                    FaceA_2->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                    FaceA_2->SequenceIndex = 0;
                    FaceA_2->UpdateAutoGeometry();
                    FaceA_2->SetSequenceFrame(aMyFunction::RandomIntRange(0, FaceA_2->SequenceFrameCount - 1));
                    FaceA_2->SetImageKindX(GI_Main::ikxCenter);
                    FaceA_2->SetImageKindY(GI_Main::ikyCenter);
                    FaceA_2->SetActive(true);
                    FaceA_2->RestartPlayback();
                    FaceA_2->UserValue = 1;
                    FaceA_2->CycleCompleteCallback = pas::bind_method<&TfGoodsShop2::MerchantAnimationComplete>(this);
                }
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                {
                    GI_Image::TImageGI* FaceI_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"FaceI"_wref.get()));
                    FaceI_2->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), u"ShopBot1i"}));
                    FaceI_2->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                    FaceI_2->SetImageKindX(GI_Main::ikxCenter);
                    FaceI_2->SetImageKindY(GI_Main::ikyCenter);
                    FaceI_2->SetActive(true);
                }
                {
                    GI_GAI::TgaiGI* FaceA_3 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
                    FaceA_3->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                    FaceA_3->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), u"ShopBot1a"}));
                    FaceA_3->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                    FaceA_3->SequenceIndex = 0;
                    FaceA_3->UpdateAutoGeometry();
                    FaceA_3->SetSequenceFrame(aMyFunction::RandomIntRange(0, FaceA_3->SequenceFrameCount - 1));
                    FaceA_3->SetImageKindX(GI_Main::ikxCenter);
                    FaceA_3->SetImageKindY(GI_Main::ikyCenter);
                    FaceA_3->SetActive(true);
                    FaceA_3->RestartPlayback();
                    FaceA_3->UserValue = 1;
                    FaceA_3->CycleCompleteCallback = pas::bind_method<&TfGoodsShop2::MerchantAnimationComplete>(this);
                }
            } else if (aPlayer::GetPlayer()->InNormalSpace()) {
                {
                    GI_Image::TImageGI* FaceI_3 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"FaceI"_wref.get()));
                    FaceI_3->SetImagePath(pas::concat_wide({u"GI,", aShip::TShip_GetCaptainPortraitResourceBase(Globals::TalkShip), u"i"}));
                    FaceI_3->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                    FaceI_3->SetImageKindX(GI_Main::ikxCenter);
                    FaceI_3->SetImageKindY(GI_Main::ikyCenter);
                    FaceI_3->SetActive(true);
                }
                {
                    GI_GAI::TgaiGI* FaceA_4 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
                    FaceA_4->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                    FaceA_4->SetImagePath(pas::concat_wide({aShip::TShip_GetCaptainPortraitResourceBase(Globals::TalkShip), u"a"}));
                    FaceA_4->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                    FaceA_4->SequenceIndex = 0;
                    FaceA_4->UpdateAutoGeometry();
                    FaceA_4->SetSequenceFrame(aMyFunction::RandomIntRange(0, FaceA_4->SequenceFrameCount - 1));
                    FaceA_4->SetImageKindX(GI_Main::ikxCenter);
                    FaceA_4->SetImageKindY(GI_Main::ikyCenter);
                    FaceA_4->SetActive(true);
                    FaceA_4->RestartPlayback();
                    FaceA_4->CycleCompleteCallback = nullptr;
                }
            }
            {
                GI_GAI::TgaiGI* FaceA_5 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
                FaceA_5->FrameAdvancedCallback = pas::bind_method<&TfGoodsShop2::CaptureMerchantBackground>(this);
            }
            GetByName(u"FaceGB"_wref.get())->SetActive(false);
            {
                GI_Image::TImageGI* CaptainI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"CaptainI"_wref.get()));
                CaptainI->SetImagePath(pas::concat_wide({u"GI,", aShip::TShip_GetCaptainPortraitResourceBase(aPlayer::GetPlayer()), u"i"}));
                CaptainI->SetImageKindX(GI_Main::ikxCenter);
                CaptainI->SetImageKindY(GI_Main::ikyCenter);
                CaptainI->SetActive(true);
            }
            {
                GI_GAI::TgaiGI* CaptainA_2 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"CaptainA"_wref.get()));
                CaptainA_2->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                CaptainA_2->SetImagePath(pas::concat_wide({aShip::TShip_GetCaptainPortraitResourceBase(aPlayer::GetPlayer()), u"a"}));
                CaptainA_2->SequenceIndex = 0;
                CaptainA_2->UpdateAutoGeometry();
                CaptainA_2->SetImageKindX(GI_Main::ikxCenter);
                CaptainA_2->SetImageKindY(GI_Main::ikyCenter);
                CaptainA_2->SetActive(true);
                CaptainA_2->RestartPlayback();
            }
            {
                const pas::WideString& fullName_5 = aPlayer::GetPlayer()->GetFullName(u"\r\n"_wref.get());
                GI_Label::TLabelGI* cpp_arg_17 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"NameCaptain"_wref.get()));
                cpp_arg_17->SetText(fullName_5);
            }
            {
                const pas::WideString& characterName_2 = aPlayer::GetPlayer()->GetCharacterName();
                GI_Label::TLabelGI* cpp_arg_18 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"CharCaptain"_wref.get()));
                cpp_arg_18->SetText(characterName_2);
            }
        }
        {
            GI_Image::TImageGI* cpp_arg_19 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImageCaptain"_wref.get()));
            pas::WideString cpp_arg_20 = pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), u"AllR"});
            cpp_arg_19->SetImagePath(std::move(cpp_arg_20));
        }
        {
            GI_GraphBuf::TGraphBufGI* GraphBufCaptain = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GraphBufCaptain"_wref.get()));
            GraphBufCaptain->SetActive(true);
            GraphBufCaptain->SourceHasPerPixelAlpha = true;
            if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(aPlayer::GetPlayer()->Graphic) != nullptr) {
                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(aPlayer::GetPlayer()->Graphic)->StaticImagePath, 1, u","_wref.get()), GraphBufCaptain->GraphBuf);
            } else {
                const pas::WideString& extractDelimitedPartW_3 = EC_Str::ExtractDelimitedPartW(aPlayer::GetPlayer()->GetShipPortraitImagePath(), 1, u","_wref.get());
                GR_GraphBuf::TGraphBufGR* graphBuf_3 = GraphBufCaptain->GraphBuf;
                GI_GI::LoadGiByPathIntoGraphBuf(extractDelimitedPartW_3, graphBuf_3);
            }
            if (static_cast<std::uint32_t>(GraphBufCaptain->GraphBuf->Width) >= static_cast<std::uint32_t>(GraphBufCaptain->GraphBuf->Height)) {
                GraphBufCaptain->GraphBuf->RescaleRgba(GraphBufCaptain->ClientSize.X, System::Round(pas::real_divide(GraphBufCaptain->ClientSize.X, static_cast<std::uint32_t>(GraphBufCaptain->GraphBuf->Width)) * static_cast<std::uint32_t>(GraphBufCaptain->GraphBuf->Height)), 5);
            } else {
                GraphBufCaptain->GraphBuf->RescaleRgba(System::Round(pas::real_divide(GraphBufCaptain->ClientSize.Y, static_cast<std::uint32_t>(GraphBufCaptain->GraphBuf->Height)) * static_cast<std::uint32_t>(GraphBufCaptain->GraphBuf->Width)), GraphBufCaptain->ClientSize.Y, 5);
            }
        }
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnOpen();
            PlanetPanel->Show();
            StationPanel->Hide();
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            PlanetPanel->Hide();
            StationPanel->OnOpen();
            StationPanel->Show();
        } else {
            PlanetPanel->Hide();
            StationPanel->Hide();
        }
        {
            GI_Image::TImageGI* BGCity2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity2"_wref.get()));
            BGCity2->SetActive(aPlayer::GetPlayer()->IsDockedToShip() && aPlayer::GetPlayer()->DockedTo->TypeId == static_cast<std::uint8_t>(aGalaxyStruct::rstMilitaryBase));
            if (BGCity2->Active) {
                BGCity2->SetImagePath(pas::concat_wide({u"GAI,", aPlayer::GetPlayer()->CurrentStar->GetBackgroundImagePath(I)}));
                BGCity2->GaiImageControl->LoadFrameSequenceFromText(u"[50,0-0]"_wref.get());
                BGCity2->SetImageKindX(GI_Main::ikxCenter);
                BGCity2->SetImageKindY(GI_Main::ikyCenter);
            }
        }
        {
            GI_Image::TImageGI* BGCity = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"BGCity"_wref.get()));
            if (aPlayer::GetPlayer()->IsOnPlanet()) {
                BGCity->SetActive(true);
                {
                    pas::WideString governmentBackgroundGraph = aPlayer::GetPlayer()->CurrentPlanet->GetGovernmentBackgroundGraph();
                    GI_Image::TImageGI* cpp_arg_21 = pas::checked_cast<GI_Image::TImageGI*>(BGCity->FindByNameRecursive(u"BGCity"_wref.get()));
                    cpp_arg_21->SetImagePath(std::move(governmentBackgroundGraph));
                }
            } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
                BGCity->SetActive(true);
                if (aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey != u"") {
                    BackgroundPath = pas::concat_wide({u"Bm.FormRuins.", GR_Main::GiResourceSuffix(), aPlayer::GetPlayer()->DockedTo->TypeNameOverrideKey, u"bg"});
                    if (GR_Main::CacheDataRoot->FileExistsByPath(BackgroundPath)) {
                        BGCity->SetImagePath(pas::concat_wide({u"GI,", BackgroundPath}));
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
        {
            GI_GraphBuf::TGraphBufGI* BGBuf = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()));
            if (aPlayer::GetPlayer()->InNormalSpace()) {
                if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
                    GR_Main::CaptureScreenBackground(true, 0);
                }
                BGBuf->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
                BGBuf->SetActive(true);
            } else {
                BGBuf->SetActive(false);
            }
        }
        GetByName(u"BGShrLight"_wref.get())->SetActive(aPlayer::GetPlayer()->InNormalSpace() && static_cast<std::uint8_t>(GlobalsV::BackgroundShade ^ 1));
        MainPanel->RebuildMessageButtons(false);
        RefreshGoodsDisplay();
        CaptureMerchantBackground(nullptr);
        MoneyWarningActive = false;
        CargoWarningActive = false;
        RefreshMoneyWarning();
        RefreshCargoWarning();
        FlagEC = false;
        AmbientSound->SetVolume(1.0f);
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        if (!aPlayer::GetPlayer()->InNormalSpace()) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(134);
        }
    }

    void TfGoodsShop2::OnClose() {
        if (aPlayer::GetPlayer() != nullptr) {
            if (!aPlayer::GetPlayer()->InNormalSpace()) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(135);
            }
        }
        if (aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        LoadPanel->OnClose();
        if (MoneyWarningTimer != nullptr) {
            CancelCallbackTimer(MoneyWarningTimer);
            MoneyWarningTimer = nullptr;
        }
        if (CargoWarningTimer != nullptr) {
            CancelCallbackTimer(CargoWarningTimer);
            CargoWarningTimer = nullptr;
        }
        if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->IsOnPlanet() || aPlayer::GetPlayer()->IsDockedToShip()) {
                MainPanel->OnClose();
            }
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"GraphBufFace"_wref.get()))->GraphBuf->Clear();
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsOnPlanet()) {
            fPanelPlanet::TfPanelPlanet::OnClose();
        } else if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->IsDockedToShip()) {
                fPanelRuins::TfPanelRuins::OnClose();
            }
        }
        if (!FlagEC) {
            AmbientSound->SetVolume(0.0f);
        }
    }

    // Rebuilds cargo/market controls and prices for the current trading context.
    void TfGoodsShop2::RefreshGoodsDisplay() {
        std::uint8_t Good{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t IconCount{};
        std::int32_t SellPrice{};
        std::int32_t OldCost{};
        GI_Panel::TPanelGI* Panel{};
        GI_Image::TImageGI* Image{};
        pas::WideString ImageName{};
        pas::WideString Color{};
        pas::Object* Location{};
        if (aPlayer::GetPlayer()->InNormalSpace()) {
            for (auto cpp_range = pas::for_to<std::uint8_t>(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(7)); cpp_range.next(Good); ) {
                TradeRows[Good].Count = Globals::TalkShip->CargoGoods[Good].Count;
                TradeRows[Good].MaximumPrice = aConst::GoodsMarket[Good].MaxPrice;
                if (Globals::TalkShip->CargoGoods[Good].Count <= 0) {
                    TradeRows[Good].PurchasePrice = System::Round(pas::real_max<pas::Extended>(aConst::GoodsMarket[Good].AveragePrice * 1.1L, 0.0L));
                    TradeRows[Good].BaseSalePrice = System::Round(std::max<std::int32_t>(aConst::GoodsMarket[Good].MinPrice, 0));
                } else {
                    TradeRows[Good].PurchasePrice = System::Round(pas::real_max<pas::Extended>(aConst::GoodsMarket[Good].AveragePrice * 1.1L, pas::real_divide(Globals::TalkShip->CargoGoods[Good].TotalCost, Globals::TalkShip->CargoGoods[Good].Count) * 1.3L));
                    TradeRows[Good].BaseSalePrice = System::Round(pas::real_max<pas::Extended>(static_cast<pas::Extended>(aConst::GoodsMarket[Good].MinPrice), pas::real_divide(Globals::TalkShip->CargoGoods[Good].TotalCost, Globals::TalkShip->CargoGoods[Good].Count) * 0.7L));
                }
            }
            {
                std::int32_t cpp_left = Globals::TalkShip->GetCargoFreeSpace();
                PartnerCargoLimit = cpp_left - Globals::TalkShip->GetDesiredCargoFreeSpace();
            }
            PartnerMoneyLimit = Globals::TalkShip->Money;
        }
        {
            GI_MessageLoop::TObjectGI* DownLeft = GetByName(u"DownLeft"_wref.get());
            DownLeft->SetActive(DraggedGoodsIndex >= 10 && DraggedGoodsIndex < 20);
        }
        {
            GI_MessageLoop::TObjectGI* DownRight = GetByName(u"DownRight"_wref.get());
            DownRight->SetActive(DraggedGoodsIndex >= 0 && DraggedGoodsIndex < 10);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 7); cpp_range_2.next(I); ) {
            Good = ShopGoodsOrder[I];
            {
                GI_GraphButton::TGraphButtonGI* cpp_with_3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(I)}))));
                cpp_with_3->MouseEnterCallback = pas::bind_method<&TfGoodsShop2::GoodsMouseEnter>(this);
                cpp_with_3->MouseLeaveCallback = pas::bind_method<&TfGoodsShop2::GoodsMouseLeave>(this);
                cpp_with_3->SetDisabled((DraggedGoodsIndex >= 10 && DraggedGoodsIndex < 20 || aPlayer::GetPlayer()->GetLocationGoodsEntry(ShopGoodsOrder[I])->Count <= 0) && (DraggedGoodsIndex < 10 || DraggedGoodsIndex >= 20 || DraggedGoodsIndex % 10 != I));
            }
            {
                GI_GraphButton::TGraphButtonGI* cpp_with_4 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(I + 10)}))));
                cpp_with_4->MouseEnterCallback = pas::bind_method<&TfGoodsShop2::GoodsMouseEnter>(this);
                cpp_with_4->MouseLeaveCallback = pas::bind_method<&TfGoodsShop2::GoodsMouseLeave>(this);
                cpp_with_4->SetDisabled((DraggedGoodsIndex >= 0 && DraggedGoodsIndex < 10 || aPlayer::GetPlayer()->CargoGoods[ShopGoodsOrder[I]].Count <= 0) && (DraggedGoodsIndex < 0 || DraggedGoodsIndex >= 10 || DraggedGoodsIndex % 10 != I));
            }
            GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovPermit", SysUtils::IntToStr(I + 1)})))->SetActive(aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(ShopGoodsOrder[I]));
            {
                std::uint8_t active = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovPermit", SysUtils::IntToStr(I + 1)})))->Active;
                GI_MessageLoop::TObjectGI* byName = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovPermit", SysUtils::IntToStr(I + 10 + 1)})));
                byName->SetActive(active);
            }
            Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovCool", SysUtils::IntToStr(I + 1)}))));
            Panel->Invalidate();
            Panel->FreeOwnedChildren();
            if (aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count <= 0) {
                IconCount = 0;
            } else {
                std::int32_t shopGoodsPurchasePrice = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, nullptr);
                std::uint8_t good = Good;
                aGalaxy::TGalaxy* galaxy = aGalaxy::Galaxy;
                IconCount = System::Round(aMyFunction::RemapClamped(aGalaxy::TGalaxy::GetGoodsPricePercent(good, shopGoodsPurchasePrice) & 0x0000007f, 0.0, 3.0E+1, 3.0, 0.0));
            }
            ImageName = u"Good"_w;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, IconCount - 1); cpp_range_3.next(J); ) {
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), ImageName}));
                Image->SetSize(Image->GetContentSize());
                Image->SetPosition(ClassesImports::Point(Panel->ClientSize.X - (Image->ClientSize.X + 1) * (J + 1), 0));
            }
            Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovCool", SysUtils::IntToStr(I + 10 + 1)}))));
            Panel->Invalidate();
            Panel->FreeOwnedChildren();
            OldCost = System::Round(aPlayer::GetPlayer()->GetAverageCargoCost(Good));
            SellPrice = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr);
            if (aPlayer::GetPlayer()->CargoGoods[Good].Count > 0) {
                if (SellPrice > OldCost) {
                    IconCount = 1;
                } else {
                    IconCount = 0;
                }
            } else {
                IconCount = 0;
            }
            ImageName = u"Good"_w;
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, IconCount - 1); cpp_range_4.next(J); ) {
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), ImageName}));
                Image->SetSize(Image->GetContentSize());
                Image->SetPosition(ClassesImports::Point(Panel->ClientSize.X - (Image->ClientSize.X + 1) * (J + 1), 0));
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, 7); cpp_range_5.next(I); ) {
            Good = ShopGoodsOrder[I];
            Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovImgCnt", SysUtils::IntToStr(I)}))));
            Panel->FreeOwnedChildren();
            Panel->Invalidate();
            IconCount = System::Round(([&] {
                pas::Extended cpp_left_2 = aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count;
                return pas::real_divide(cpp_left_2, aConst::GoodsMarket[Good].BaseStock);
            }()) * 6.0L);
            if (IconCount <= 0 && aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count >= 1) {
                IconCount = 1;
            } else if (IconCount > 6) {
                IconCount = 6;
            }
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, IconCount - 1); cpp_range_6.next(J); ) {
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), u"Goods", pas::wide_int_to_str(I + 1)}));
                if (pas::in_set<2, 2, 4, 6>(I)) {
                    Image->SetPosition(ClassesImports::Point(J * GR_Main::GiScalePixels(12), 0));
                } else {
                    Image->SetPosition(ClassesImports::Point(J * GR_Main::GiScalePixels(11), 0));
                }
                Image->SetDepth(J);
                Image->SetSize(Image->GetContentSize());
            }
            Color = pas::WideString();
            if (aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count < 0) {
                GR_Main::RaiseWideMessage(u"Player.ShopGoods(it).Cnt<0"_wref.get());
            } else if (aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count == 0) {
                Color = OutOfStockColor;
            }
            // Native writes this sell-price label once with the market color, then again with the cargo color below.
            {
                const pas::WideString& wrapTextInColor = ([&] {
                    pas::WideString intToStr = pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr));
                    pas::WideString color = Color;
                    return aMyFunction::WrapTextInColor(std::move(intToStr), std::move(color));
                }());
                GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovPrice", SysUtils::IntToStr(I + 10)}))));
                cpp_arg->SetText(wrapTextInColor);
            }
            {
                const pas::WideString& wrapTextInColor_2 = ([&] {
                    pas::WideString intToStr_2 = pas::wide_int_to_str(aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count);
                    pas::WideString color_2 = Color;
                    return aMyFunction::WrapTextInColor(std::move(intToStr_2), std::move(color_2));
                }());
                GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovCnt", SysUtils::IntToStr(I)}))));
                cpp_arg_2->SetText(wrapTextInColor_2);
            }
            {
                const pas::WideString& wrapTextInColor_3 = ([&] {
                    pas::WideString intToStr_3 = pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, nullptr));
                    pas::WideString color_3 = Color;
                    return aMyFunction::WrapTextInColor(std::move(intToStr_3), std::move(color_3));
                }());
                GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovPrice", SysUtils::IntToStr(I)}))));
                cpp_arg_3->SetText(wrapTextInColor_3);
            }
            Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovImgCnt", SysUtils::IntToStr(I + 10)}))));
            Panel->FreeOwnedChildren();
            IconCount = System::Round(pas::real_divide(aPlayer::GetPlayer()->CargoGoods[Good].Count, aConst::GoodsMarket[Good].BaseStock) * 6.0L);
            if (IconCount <= 0 && aPlayer::GetPlayer()->CargoGoods[Good].Count >= 1) {
                IconCount = 1;
            } else if (IconCount > 6) {
                IconCount = 6;
            }
            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, IconCount - 1); cpp_range_7.next(J); ) {
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormGoods2.", GR_Main::GiResourceSuffix(), u"Goods", pas::wide_int_to_str(I + 1)}));
                // Unlike the market row, native cargo-icon spacing does not use GiScalePixels.
                if (pas::in_set<4, 6>(I)) {
                    Image->SetPosition(ClassesImports::Point(J * 12, 0));
                } else {
                    Image->SetPosition(ClassesImports::Point(J * 11, 0));
                }
                Image->SetDepth(J);
                Image->SetSize(Image->GetContentSize());
            }
            Color = pas::WideString();
            if (aPlayer::GetPlayer()->CargoGoods[Good].Count < 0) {
                GR_Main::RaiseWideMessage(u"Player.FGoods[it].Cnt<0"_wref.get());
            } else if (aPlayer::GetPlayer()->CargoGoods[Good].Count == 0) {
                Color = OutOfStockColor;
            }
            {
                const pas::WideString& wrapTextInColor_4 = aMyFunction::WrapTextInColor(pas::wide_int_to_str(aPlayer::GetPlayer()->CargoGoods[Good].Count), Color);
                GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovCnt", SysUtils::IntToStr(I + 10)}))));
                cpp_arg_4->SetText(wrapTextInColor_4);
            }
            {
                const pas::WideString& wrapTextInColor_5 = ([&] {
                    pas::WideString intToStr_4 = pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr));
                    pas::WideString color_4 = Color;
                    return aMyFunction::WrapTextInColor(std::move(intToStr_4), std::move(color_4));
                }());
                GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"TovPrice", SysUtils::IntToStr(I + 10)}))));
                cpp_arg_5->SetText(wrapTextInColor_5);
            }
        }
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            Location = aPlayer::GetPlayer()->CurrentPlanet;
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            Location = aPlayer::GetPlayer()->DockedTo;
        } else if (aPlayer::GetPlayer()->InNormalSpace()) {
            Location = Globals::TalkShip;
        } else {
            Location = nullptr;
        }
        {
            GI_GraphButton::TGraphButtonGI* UserMsgAdd = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()));
            if (aPlayer::GetPlayer()->InNormalSpace()) {
                UserMsgAdd->SetDisabled(true);
            } else {
                UserMsgAdd->SetDisabled(Globals::FindPlayerBubbleByText(TfGoodsShop2::BuildPriceText(Location), false) != nullptr);
            }
        }
    }

    void TfGoodsShop2::GoodsMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t I{};
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t Price{};
        std::int32_t Maximum{};
        std::int32_t TotalLimit{};
        std::int32_t Available{};
        std::int32_t Limit{};
        pas::WideString Description{};
        std::uint8_t OverMarket = false;
        std::uint8_t OverCargo = false;
        if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"DownLeft"_wref.get()))->HitTestPixel(Point)) {
            OverMarket = true;
        } else if (pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"DownRight"_wref.get()))->HitTestPixel(Point)) {
            OverCargo = true;
        } else {
            if (DraggedGoodsIndex >= 0) {
                DraggedGoodsIndex = -1;
                UpdateActionCursor(false);
                RefreshGoodsDisplay();
            }
            return;
        }
        Index = -1;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
            {
                GI_GraphButton::TGraphButtonGI* cpp_with = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(I)}))));
                if (cpp_with->ContainsPoint(GetCursorPoint())) {
                    Index = I;
                    break;
                }
            }
            {
                GI_GraphButton::TGraphButtonGI* cpp_with_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(I + 10)}))));
                if (cpp_with_2->ContainsPoint(GetCursorPoint())) {
                    Index = I + 10;
                    break;
                }
            }
        }
        if (Index < 0 && (DraggedGoodsIndex < 0 || OverMarket && DraggedGoodsIndex >= 0 && DraggedGoodsIndex < 10 || OverCargo && DraggedGoodsIndex >= 10 && DraggedGoodsIndex < 20)) {
            DraggedGoodsIndex = -1;
            UpdateActionCursor(false);
            RefreshGoodsDisplay();
        } else if (DraggedGoodsIndex < 0 || DraggedGoodsIndex >= 0 && DraggedGoodsIndex < 10 && Index >= 0 && Index < 10 || DraggedGoodsIndex >= 10 && DraggedGoodsIndex < 20 && Index >= 10 && Index < 20) {
            if (Index == DraggedGoodsIndex) {
                DraggedGoodsIndex = -1;
            } else if (TfGoodsShop2::GetAvailableGoodsCount(Index) > 0) {
                DraggedGoodsIndex = Index;
            }
            UpdateActionCursor(false);
            RefreshGoodsDisplay();
            {
                GI_GraphButton::TGraphButtonGI* cpp_with_3 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(Index)}))));
                cpp_with_3->SetDown(false);
                cpp_with_3->SetHovered(false);
                cpp_with_3->SetHovered(true);
            }
        }
        if (DraggedGoodsIndex >= 0) {
            Count = GetMaximumTradeCount(DraggedGoodsIndex, false);
            if (Count <= 0 && (DraggedGoodsIndex >= 10 || aPlayer::GetPlayer()->InNormalSpace() || GetMaximumTradeCount(DraggedGoodsIndex, true) <= 0)) {
                if (aPlayer::GetPlayer()->InNormalSpace()) {
                    if (DraggedGoodsIndex >= 10 && DraggedGoodsIndex < 20) {
                        if (PartnerCargoLimit <= 0) {
                            const pas::WideString& lookupTalkText = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.NoSpace"_wref.get());
                            GI_MessageLoop::TMessageLoopGI* self = this;
                            GI_MessageBox::ShowMessageBoxGI(self, lookupTalkText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
                        } else {
                            const pas::WideString& lookupTalkText_2 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.NoMoney"_wref.get());
                            GI_MessageLoop::TMessageLoopGI* self_2 = this;
                            GI_MessageBox::ShowMessageBoxGI(self_2, lookupTalkText_2, GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                        }
                    } else if (DraggedGoodsIndex < 10) {
                        if (aPlayer::GetPlayer()->GetLocationGoodsEntry(ShopGoodsOrder[DraggedGoodsIndex])->Count > 0) {
                            if (aPlayer::GetPlayer()->GetCargoFreeSpace() <= 0) {
                                FlashCargoWarning();
                            }
                            {
                                std::int32_t cpp_left = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[DraggedGoodsIndex], nullptr);
                                if (cpp_left > aPlayer::GetPlayer()->Money) {
                                    FlashMoneyWarning();
                                }
                            }
                            DraggedGoodsIndex = -1;
                            UpdateActionCursor(false);
                            RefreshGoodsDisplay();
                            GI_Main::BreakUiMessage();
                            return;
                        }
                    }
                } else if (DraggedGoodsIndex < 10) {
                    if (aPlayer::GetPlayer()->GetLocationGoodsEntry(ShopGoodsOrder[DraggedGoodsIndex])->Count > 0) {
                        if (aPlayer::GetPlayer()->GetCargoFreeSpace() <= 0) {
                            MainPanel->FlashCargoWarning();
                        }
                        {
                            std::int32_t cpp_left_2 = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[DraggedGoodsIndex], nullptr);
                            if (cpp_left_2 > aPlayer::GetPlayer()->Money) {
                                MainPanel->FlashMoneyWarning();
                            }
                        }
                    }
                }
            } else {
                Limit = std::max<std::int32_t>(0, Count);
                if (DraggedGoodsIndex < 10) {
                    Description = ([&] {
                        pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aConst::GoodsMarket[ShopGoodsOrder[DraggedGoodsIndex % 10]].TradeName);
                        pas::WideString localizedText = aConst::LocalizedText(u"FormGS.Buy"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString));
                    }());
                    Price = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[DraggedGoodsIndex % 10], nullptr);
                    Maximum = aPlayer::GetPlayer()->GetLocationGoodsEntry(ShopGoodsOrder[DraggedGoodsIndex])->Count;
                    Available = aPlayer::GetPlayer()->GetCargoFreeSpace();
                    TotalLimit = aPlayer::GetPlayer()->Money;
                    if (!aPlayer::GetPlayer()->InNormalSpace()) {
                        Limit = std::min<std::int32_t>(Maximum, pas::idiv(TotalLimit, Price));
                    }
                } else {
                    Description = ([&] {
                        pas::WideString lowerCaseWideString_2 = EC_Str::LowerCaseWideString(aConst::GoodsMarket[ShopGoodsOrder[DraggedGoodsIndex % 10]].TradeName);
                        pas::WideString localizedText_2 = aConst::LocalizedText(u"FormGS.Sell"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText_2), u"<color=0,50,200>"_w, u"<Name>"_w, std::move(lowerCaseWideString_2));
                    }());
                    Price = aPlayer::GetPlayer()->ShopGoodsSellPrice(ShopGoodsOrder[DraggedGoodsIndex % 10], nullptr);
                    Maximum = aPlayer::GetPlayer()->CargoGoods[ShopGoodsOrder[DraggedGoodsIndex - 10]].Count;
                    if (!aPlayer::GetPlayer()->InNormalSpace()) {
                        Available = 1000000000;
                        TotalLimit = 1000000000;
                    } else {
                        Available = PartnerCargoLimit;
                        TotalLimit = PartnerMoneyLimit;
                        Limit = 1000000000;
                    }
                }
                Count = std::max<std::int32_t>(0, Count);
                if (fCount2::ShowCountDialog(this, pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(ShopGoodsOrder[DraggedGoodsIndex % 10]))}), Description, 0, Maximum, Limit, Price, std::max<std::int32_t>(0, Available), TotalLimit, Count) == 1) {
                    if (([&] {
                        std::int32_t cpp_left_3 = GetMaximumTradeCount(DraggedGoodsIndex, false);
                        return cpp_left_3 < Count;
                    }()) && DraggedGoodsIndex >= 10 && aPlayer::GetPlayer()->InNormalSpace()) {
                        if (Count > PartnerCargoLimit) {
                            const pas::WideString& lookupTalkText_3 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.NoSpace"_wref.get());
                            GI_MessageLoop::TMessageLoopGI* self_3 = this;
                            GI_MessageBox::ShowMessageBoxGI(self_3, lookupTalkText_3, GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
                        } else {
                            const pas::WideString& lookupTalkText_4 = aShip::TShip_LookupTalkText(Globals::TalkShip, u"Talk.Trade.NoMoney"_wref.get());
                            GI_MessageLoop::TMessageLoopGI* self_4 = this;
                            GI_MessageBox::ShowMessageBoxGI(self_4, lookupTalkText_4, GI_MessageBox::mbgOK | GI_MessageBox::mbgError, 0, 0, 0);
                        }
                    } else if (Count > 0 && ([&] {
                        std::int32_t cpp_left_4 = GetMaximumTradeCount(DraggedGoodsIndex, static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1));
                        return cpp_left_4 >= Count;
                    }())) {
                        // Native legality check uses the hovered row, even when a different
                        // row supplied the dragged good; preserve that original selection.
                        if (aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(ShopGoodsOrder[Index % 10])) {
                            if (([&] {
                                const pas::WideString& paramByPathOrMarker = GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormGS.NotPermitGoods"_wref.get());
                                GI_MessageLoop::TMessageLoopGI* self_5 = this;
                                return GI_MessageBox::ShowMessageBoxGI(self_5, paramByPathOrMarker, GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0);
                            }()) != GI_MessageBox::mbgResultOK) {
                                DraggedGoodsIndex = -1;
                                UpdateActionCursor(false);
                                RefreshGoodsDisplay();
                                FinishModalTrade();
                                GI_Main::BreakUiMessage();
                                return;
                            }
                        }
                        if (!aPlayer::GetPlayer()->InNormalSpace()) {
                            aGalaxy::Galaxy->CheckIntegrityChecksum(136);
                        }
                        if (aPlayer::GetPlayer()->InNormalSpace()) {
                            if (DraggedGoodsIndex < 10) {
                                {
                                    std::int32_t cpp_right_2 = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[DraggedGoodsIndex % 10], nullptr);
                                    std::int32_t cpp_right = Count * cpp_right_2;
                                    std::int32_t cpp_arg = Globals::TalkShip->Money + cpp_right;
                                    aShip::TShip* talkShip = Globals::TalkShip;
                                    talkShip->SetMoney(cpp_arg);
                                }
                                {
                                    std::int32_t cpp_step = ([&] {
                                        std::int32_t cpp_right_3 = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[DraggedGoodsIndex % 10], nullptr);
                                        return Count * cpp_right_3;
                                    }());
                                    std::int32_t& cpp_target = Globals::TalkShip->CargoGoods[ShopGoodsOrder[DraggedGoodsIndex]].TotalCost;
                                    cpp_target -= cpp_step;
                                }
                                Globals::TalkShip->CargoGoods[ShopGoodsOrder[DraggedGoodsIndex]].Count -= Count;
                            } else {
                                {
                                    std::int32_t cpp_right_5 = aPlayer::GetPlayer()->ShopGoodsSellPrice(ShopGoodsOrder[DraggedGoodsIndex % 10], nullptr);
                                    std::int32_t cpp_right_4 = Count * cpp_right_5;
                                    std::int32_t cpp_arg_2 = Globals::TalkShip->Money - cpp_right_4;
                                    aShip::TShip* talkShip_2 = Globals::TalkShip;
                                    talkShip_2->SetMoney(cpp_arg_2);
                                }
                                {
                                    std::int32_t cpp_step_2 = ([&] {
                                        std::int32_t cpp_right_6 = aPlayer::GetPlayer()->ShopGoodsSellPrice(ShopGoodsOrder[DraggedGoodsIndex % 10], nullptr);
                                        return Count * cpp_right_6;
                                    }());
                                    std::int32_t& cpp_target_2 = Globals::TalkShip->CargoGoods[ShopGoodsOrder[DraggedGoodsIndex % 10]].TotalCost;
                                    cpp_target_2 += cpp_step_2;
                                }
                                Globals::TalkShip->CargoGoods[ShopGoodsOrder[DraggedGoodsIndex % 10]].Count += Count;
                            }
                            Globals::TalkShip->RefreshDerivedStats(true);
                        }
                        if (DraggedGoodsIndex < 10) {
                            GR_Main::SoundManager->PlaySound(u"Sound.Buy"_wref.get());
                            aPlayer::GetPlayer()->BuyGoodsFromLocation(ShopGoodsOrder[DraggedGoodsIndex], Count);
                        } else {
                            GR_Main::SoundManager->PlaySound(u"Sound.Sell"_wref.get());
                            aPlayer::GetPlayer()->SellGoodsToLocation(ShopGoodsOrder[DraggedGoodsIndex - 10], Count);
                        }
                        if (!aPlayer::GetPlayer()->InNormalSpace()) {
                            aGalaxy::Galaxy->PrimeIntegrityChecksum(137);
                        }
                        if (aPlayer::GetPlayer()->IsOnPlanet()) {
                            if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
                                GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
                                RequestClose(1);
                            }
                        }
                    }
                }
            }
            DraggedGoodsIndex = -1;
            UpdateActionCursor(false);
            RefreshGoodsDisplay();
            FinishModalTrade();
        }
        GI_Main::BreakUiMessage();
    }

    void TfGoodsShop2::GoodsRightMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (DraggedGoodsIndex >= 0) {
            DraggedGoodsIndex = -1;
            UpdateActionCursor(false);
            RefreshGoodsDisplay();
        }
    }

    void TfGoodsShop2::GoodsMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t Count{};
        std::int32_t Price{};
        GI_MessageLoop::TObjectGI* Control{};
        std::int32_t I{};
        std::int32_t Profit{};
        pas::WideString Action{};
        pas::WideString Color{};
        pas::WideString Text{};
        std::uint8_t Good{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        std::int32_t Index = -1;
        Text = pas::WideString();
        if (DraggedGoodsIndex >= 0) {
            Index = DraggedGoodsIndex;
        } else {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(I); ) {
                Control = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(I)})));
                {
                    GI_GraphButton::TGraphButtonGI* cpp_with = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control);
                    if (cpp_with->ContainsPoint(GetCursorPoint())) {
                        Index = I;
                        break;
                    }
                }
                Control = GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(I + 10)})));
                {
                    GI_GraphButton::TGraphButtonGI* cpp_with_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Control);
                    if (cpp_with_2->ContainsPoint(GetCursorPoint())) {
                        Index = I + 10;
                        break;
                    }
                }
            }
        }
        if (Index >= 0) {
            Good = ShopGoodsOrder[Index % 10];
            if (Index < 10 && aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count > 0 || Index >= 10 && aPlayer::GetPlayer()->CargoGoods[Good].Count > 0) {
                if (Index < 10) {
                    Action = u"Buy"_w;
                    Price = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, nullptr);
                    Count = aPlayer::GetPlayer()->GetLocationGoodsEntry(Good)->Count;
                    Profit = 0;
                } else {
                    Action = u"Sale"_w;
                    Price = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr);
                    Count = aPlayer::GetPlayer()->CargoGoods[Good].Count;
                    {
                        std::int32_t cpp_left = aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, nullptr);
                        Profit = cpp_left - static_cast<std::int32_t>(System::Round(aPlayer::GetPlayer()->GetAverageCargoCost(Good)));
                    }
                }
                Text = GR_Main::LookupLocalizedTextByKey(pas::concat_wide({u"FormGS.", Action, u"Help"}));
                aMyFunction::ReplaceTextToken(Text, u"<Goods>"_w, aConst::GoodsMarket[Good].TradeName, u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Count>"_w, pas::wide_int_to_str(Count), u"<color=255,240,100>"_w);
                aMyFunction::ReplaceTextToken(Text, u"<Cost>"_w, pas::wide_int_to_str(Price), u"<color=255,240,100>"_w);
                if (Profit > 0) {
                    Color = u"<color=0,255,0>"_w;
                } else if (Profit < 0) {
                    Color = u"<color=255,0,0>"_w;
                } else if (Profit == 0) {
                    Color = pas::WideString();
                }
                aMyFunction::ReplaceTextToken(Text, u"<OldCost>"_w, pas::wide_int64_to_str(System::Round(aPlayer::GetPlayer()->GetAverageCargoCost(Good))), Color);
                aMyFunction::ReplaceTextToken(Text, u"<Profit>"_w, pas::wide_int_to_str(Profit), u"<color=255,240,100>"_w);
            } else if (Index < 10) {
                if (aPlayer::GetPlayer()->IsOutsideStarSpace()) {
                    Text = ([&] {
                        pas::WideString lowerCaseWideString = EC_Str::LowerCaseWideString(aConst::GoodsMarket[Good].DisplayName);
                        pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"FormGS.NotGoodsForBuyHelp"_wref.get());
                        return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey), u"<color=255,240,100>"_w, u"<Goods>"_w, std::move(lowerCaseWideString));
                    }());
                } else {
                    Text = ([&] {
                        pas::WideString lowerCaseWideString_2 = EC_Str::LowerCaseWideString(aConst::GoodsMarket[Good].DisplayName);
                        pas::WideString lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"FormGS.NotGoodsForBuyHelpInShip"_wref.get());
                        return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_2), u"<color=255,240,100>"_w, u"<Goods>"_w, std::move(lowerCaseWideString_2));
                    }());
                }
            } else {
                Text = ([&] {
                    pas::WideString lowerCaseWideString_3 = EC_Str::LowerCaseWideString(aConst::GoodsMarket[Good].DisplayName);
                    pas::WideString lookupLocalizedTextByKey_3 = GR_Main::LookupLocalizedTextByKey(u"FormGS.NotGoodsForSaleHelp"_wref.get());
                    return aMyFunction::FormatText1(std::move(lookupLocalizedTextByKey_3), u"<color=255,240,100>"_w, u"<Goods>"_w, std::move(lowerCaseWideString_3));
                }());
            }
        }
        if (HoveredControl == nullptr || HoveredControl->HelpText == u"") {
            ShowHelpText(Text, Text != u"");
        }
    }

    void TfGoodsShop2::UpdateActionCursor(std::uint8_t CanTake) {
        std::uint8_t Found{};
        std::int32_t Index{};
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (DraggedGoodsIndex >= 0) {
            SetCursorImage(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aConst::TItemType>(ShopGoodsOrder[DraggedGoodsIndex % 10]))}), ClassesImports::Point(16, 16));
        } else {
            Found = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, 7); cpp_range.next(Index); ) {
                {
                    GI_GraphButton::TGraphButtonGI* cpp_with = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(Index)}))));
                    if (cpp_with->ContainsPoint(GetCursorPoint())) {
                        if (aPlayer::GetPlayer()->GetLocationGoodsEntry(ShopGoodsOrder[Index])->Count > 0) {
                            Found = true;
                            break;
                        }
                    }
                }
                {
                    GI_GraphButton::TGraphButtonGI* cpp_with_2 = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"Tov", SysUtils::IntToStr(Index + 10)}))));
                    if (cpp_with_2->ContainsPoint(GetCursorPoint())) {
                        if (aPlayer::GetPlayer()->CargoGoods[ShopGoodsOrder[Index]].Count > 0) {
                            Found = true;
                            break;
                        }
                    }
                }
            }
            if (Found) {
                if (!IsCursorImageSelected(u"Take"_wref.get())) {
                    SetCursorByName(u"Take"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"Main"_wref.get())) {
                SetCursorByName(u"Main"_wref.get());
            }
        }
    }

    void TfGoodsShop2::GoodsMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        UpdateActionCursor(false);
    }

    void TfGoodsShop2::GoodsMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        UpdateActionCursor(false);
    }

    std::int32_t TfGoodsShop2::GetMaximumTradeCount(std::int32_t Index, std::uint8_t IgnoreCargoSpace) {
        std::int32_t Result{};
        double Total{};
        if (Index >= 0 && Index < 10) {
            Result = aPlayer::GetPlayer()->GetLocationGoodsEntry(ShopGoodsOrder[Index])->Count;
            if (aPlayer::GetPlayer()->GetCargoFreeSpace() < Result && static_cast<std::uint8_t>(IgnoreCargoSpace ^ 1)) {
                Result = aPlayer::GetPlayer()->GetCargoFreeSpace();
            }
            Total = Result;
            Total = static_cast<long double>(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[Index], nullptr)) * Total;
            if (([&] {
                std::int32_t cpp_left = Result * aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[Index], nullptr);
                return cpp_left > aPlayer::GetPlayer()->Money;
            }()) || Total > 1.0E+8L) {
                std::int32_t cpp_right = aPlayer::GetPlayer()->ShopGoodsPurchasePrice(ShopGoodsOrder[Index], nullptr);
                return pas::idiv(aPlayer::GetPlayer()->Money, cpp_right);
            }
        } else {
            Result = aPlayer::GetPlayer()->CargoGoods[ShopGoodsOrder[Index - 10]].Count;
            if (aPlayer::GetPlayer()->InNormalSpace()) {
                if (PartnerCargoLimit < Result) {
                    Result = PartnerCargoLimit;
                }
                {
                    std::int32_t cpp_left_2 = Result * Globals::TalkShip->ShopGoodsSellPrice(ShopGoodsOrder[Index - 10], nullptr);
                    if (cpp_left_2 > PartnerMoneyLimit) {
                        std::int32_t cpp_right_2 = Globals::TalkShip->ShopGoodsSellPrice(ShopGoodsOrder[Index - 10], nullptr);
                        return pas::idiv(PartnerMoneyLimit, cpp_right_2);
                    }
                }
            }
        }
        return Result;
    }

    std::int32_t TfGoodsShop2::GetAvailableGoodsCount(std::int32_t Index) {
        if (Index >= 0 && Index < 10) {
            return aPlayer::GetPlayer()->GetLocationGoodsEntry(ShopGoodsOrder[Index])->Count;
        }
        return aPlayer::GetPlayer()->CargoGoods[ShopGoodsOrder[Index - 10]].Count;
    }

    pas::WideString TfGoodsShop2::BuildPriceText(pas::Object* Location) {
        std::int32_t RowNumber{};
        std::int32_t SeparatorLength{};
        std::int32_t Stock{};
        pas::WideString Text{};
        pas::WideString Title{};
        pas::WideString Info{};
        pas::WideString Separator{};
        std::uint8_t Good{};
        std::uint8_t Index{};
        aPlanet::TPlanet* SavedPlanet{};
        aShip::TShip* SavedDockedTo{};
        if (GR_Main::GiResourceVariant() == 1) {
            SeparatorLength = 76;
        } else {
            SeparatorLength = 95;
        }
        for (auto cpp_range = pas::for_to<std::int32_t>(1, SeparatorLength); cpp_range.next(RowNumber); ) {
            Separator = pas::concat_wide({Separator, u"-"});
        }
        if (pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) {
            Title = ([&] {
                auto name = pas::borrow(pas::checked_cast<aPlanet::TPlanet*>(Location)->Name);
                pas::WideString localizedColorText = aConst::LocalizedColorText(u"FormGS.PlanetInfo"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText), u"<color=255,240,100>"_w, u"<Planet>"_w, name.get());
            }());
        } else if (pas::class_cast_if<aRuins::TRuins*>(Location) != nullptr) {
            Title = aMyFunction::WrapTextInColor(pas::checked_cast<aRuins::TRuins*>(Location)->GetColoredFullName(u"<color=255,240,100>"_wref.get()), pas::WideString());
        } else if (aPlayer::GetPlayer()->InNormalSpace()) {
            Title = aMyFunction::WrapTextInColor(Globals::TalkShip->GetFullName(u" "_wref.get()), u"<color=255,240,100>"_w);
        }
        Text = pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"<td=", SysUtils::IntToStr(GR_Main::GiScalePixels(0)), ">", "<align=left>"})), Title, u"</align>"});
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(230)), u">", u"<align=center>", aMyFunction::WrapTextInColor(aGalaxy::Galaxy->FormatTurnDate(aGalaxy::Galaxy->CurrentTurn), u"<color=0,255,0>"_w), u"</align>"});
        if (pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) {
            Info = ([&] {
                auto name_2 = pas::borrow(pas::checked_cast<aPlanet::TPlanet*>(Location)->CurrentStar->Name);
                pas::WideString localizedColorText_2 = aConst::LocalizedColorText(u"FormGS.StarInfo"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_2), u"<color=255,240,100>"_w, u"<Star>"_w, name_2.get());
            }());
        } else if (pas::class_cast_if<aShip::TShip*>(Location) != nullptr) {
            Info = ([&] {
                auto name_3 = pas::borrow(pas::checked_cast<aShip::TShip*>(Location)->CurrentStar->Name);
                pas::WideString localizedColorText_3 = aConst::LocalizedColorText(u"FormGS.StarInfo"_wref.get());
                return aMyFunction::FormatText1(std::move(localizedColorText_3), u"<color=255,240,100>"_w, u"<Star>"_w, name_3.get());
            }());
        }
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(400)), u">", u"<align=center>", aMyFunction::WrapTextInColor(Info, pas::WideString()), u"</align>"});
        Text = pas::concat_wide({Text, u"\r\n", Separator});
        Text = pas::concat_wide({Text, u"\r\n", u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(10)), u">", u"<align=center>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormGS.ColumnNumber"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(30)), u">", u"<align=left>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormGS.ColumnName"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(190)), u">", u"<align=center>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormGS.ColumnCount"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(300)), u">", u"<align=center>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormGS.ColumnCost"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(410)), u">", u"<align=center>", aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormGS.ColumnLegality"_wref.get()), u"<color=255,240,100>"_w), u"</align>"});
        Text = pas::concat_wide({Text, u"\r\n", Separator});
        RowNumber = 1;
        for (Index = static_cast<std::uint8_t>(0); Index <= static_cast<std::uint8_t>(7); ++Index) {
            Good = aConst::GoodsTextOrder[Index];
            Stock = 0;
            if (pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) {
                Stock = pas::checked_cast<aPlanet::TPlanet*>(Location)->Goods[Good].Count;
            } else if (pas::class_cast_if<aRuins::TRuins*>(Location) != nullptr) {
                Stock = pas::checked_cast<aRuins::TRuins*>(Location)->ShopGoods[Good].Count;
            } else {
                GR_Main::RaiseWideMessage(u"no goods shop"_wref.get());
            }
            Text = pas::concat_wide({Text, u"\r\n", u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(10)), u">", u"<align=center>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(RowNumber), pas::WideString()), u"</align>"});
            ++RowNumber;
            Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(30)), u">", u"", aMyFunction::WrapTextInColor(aConst::GoodsMarket[Good].DisplayName, pas::WideString()), u""});
            Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(190)), u">", u"<align=center>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(Stock), pas::WideString()), u"</align>"});
            Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(285)), u"><align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsPurchasePrice(Good, Location)), pas::WideString()), u"</align>"});
            Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(300)), u"><align=center>", aMyFunction::WrapTextInColor(u"/"_w, pas::WideString()), u"</align>"});
            Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(335)), u"><align=right>", aMyFunction::WrapTextInColor(pas::wide_int_to_str(aPlayer::GetPlayer()->ShopGoodsSellPrice(Good, Location)), pas::WideString()), u"</align>"});
            SavedPlanet = aPlayer::GetPlayer()->CurrentPlanet;
            SavedDockedTo = aPlayer::GetPlayer()->DockedTo;
            // Both native writes clear CurrentPlanet; retain the original duplicate assignment.
            aPlayer::GetPlayer()->CurrentPlanet = nullptr;
            aPlayer::GetPlayer()->CurrentPlanet = nullptr;
            if (pas::class_cast_if<aPlanet::TPlanet*>(Location) != nullptr) {
                aPlayer::GetPlayer()->CurrentPlanet = reinterpret_cast<aPlanet::TPlanet*>(Location);
            } else if (pas::class_cast_if<aRuins::TRuins*>(Location) != nullptr) {
                aPlayer::GetPlayer()->DockedTo = reinterpret_cast<aShip::TShip*>(Location);
            }
            if (!aPlayer::GetPlayer()->IsCargoGoodIllegalOnCurrentPlanet(Good)) {
                Info = GR_Main::LookupLocalizedTextByKey(u"FormGS.LegalityOk"_wref.get());
            } else {
                Info = aMyFunction::WrapTextInColor(GR_Main::LookupLocalizedTextByKey(u"FormGS.LegalityNo"_wref.get()), u"<color=255,0,0>"_w);
            }
            aPlayer::GetPlayer()->CurrentPlanet = SavedPlanet;
            aPlayer::GetPlayer()->DockedTo = SavedDockedTo;
            Text = pas::concat_wide({Text, u"<td=", pas::wide_int_to_str(GR_Main::GiScalePixels(405)), u"><align=center>", aMyFunction::WrapTextInColor(Info, pas::WideString()), u"</align>"});
        }
        return Text;
    }

    void TfGoodsShop2::SavePricesClicked(GI_MessageLoop::TObjectGI* Sender) {
        pas::Object* Location{};
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()))->SetDisabled(true);
        GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            Location = aPlayer::GetPlayer()->CurrentPlanet;
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            Location = aPlayer::GetPlayer()->DockedTo;
        } else if (aPlayer::GetPlayer()->InNormalSpace()) {
            Location = Globals::TalkShip;
        } else {
            Location = nullptr;
        }
        {
            const pas::WideString& priceSnapshotKey = fStarMap::TfStarMap::GetPriceSnapshotKey(Location);
            const pas::WideString& buildPriceText = TfGoodsShop2::BuildPriceText(Location);
            std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
            Globals::AddOrUpdatePlayerBubble(7, currentTurn, buildPriceText, priceSnapshotKey);
        }
        MainPanel->RebuildMessageButtons(false);
        FinishModalTrade();
    }

    void TfGoodsShop2::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aPlayer::GetPlayer() == nullptr) {
            return;
        }
        if (aPlayer::GetPlayer()->QueuedTravelTarget != nullptr) {
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
                return;
            }
            StationPanel->TakeOffForStationTravel();
        } else {
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"UserMsgAdd"_wref.get()))->SetDisabled(false);
            if (DraggedGoodsIndex >= 0) {
                DraggedGoodsIndex = -1;
                UpdateActionCursor(false);
                RefreshGoodsDisplay();
            }
            if (!aPlayer::GetPlayer()->InNormalSpace()) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(138);
            }
            fEquipmentShop::RestoreTemporaryShopStock();
            MainPanel->EndTurnClicked(Sender);
            if (ExitCode == 0) {
                fEquipmentShop::BuildTemporaryShopSlotGrid();
                if (!aPlayer::GetPlayer()->InNormalSpace()) {
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(139);
                }
                RefreshGoodsDisplay();
                MainPanel->RebuildMessageButtons(false);
            }
        }
    }

    void TfGoodsShop2::GalaxyClicked(GI_MessageLoop::TObjectGI* Sender) {
        AmbientSound->SetVolume(0.0f);
        MainPanel->GalaxyClicked(Sender);
        AmbientSound->SetVolume(1.0f);
    }

    void TfGoodsShop2::QuestClicked(GI_MessageLoop::TObjectGI* Sender) {
        AmbientSound->SetVolume(0.0f);
        MainPanel->QuestClicked(Sender);
        AmbientSound->SetVolume(1.0f);
    }

    void TfGoodsShop2::MenuClicked(GI_MessageLoop::TObjectGI* Sender) {
        AmbientSound->SetVolume(0.0f);
        MainPanel->MenuClicked(Sender);
        AmbientSound->SetVolume(1.0f);
    }

    void TfGoodsShop2::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ExitCode != 0) {
            return;
        }
        if (DraggedGoodsIndex >= 0) {
            DraggedGoodsIndex = -1;
            UpdateActionCursor(false);
            RefreshGoodsDisplay();
        }
        AmbientSound->SetVolume(0.0f);
        SetCursorActive(false);
        Present();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
        Globals::ShipScreen->PlayTransitionSounds = true;
        if (!aPlayer::GetPlayer()->InNormalSpace()) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(142);
        }
        while (true) {
            fShip2::RunShipEquipment(this);
            MainPanel->RefreshMoneyAndCargo();
            MainPanel->RebuildMessageButtons(false);
            RefreshGoodsDisplay();
            if (!Globals::ShipScreen->FlagD4) {
                break;
            }
            SetCursorActive(false);
            DrawQueuedUpdateRects();
            GR_Main::CaptureScreenBackground(true, 0);
            SetCursorActive(true);
        }
        if (!aPlayer::GetPlayer()->InNormalSpace()) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum(143);
        }
        AmbientSound->SetVolume(1.0f);
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (aPlayer::GetPlayer()->CurrentPlanet->GetRelationLevelToShip(aPlayer::GetPlayer()) == aGalaxyStruct::rlHostile) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGovernment;
                RequestClose(1);
            }
        }
    }

    void TfGoodsShop2::FinishModalTrade() {
        if (ParentLoop != nullptr) {
            FlagEC = true;
            RequestClose(1);
            GI_Main::BreakUiMessage();
        }
    }

    void TfGoodsShop2::CaptureMerchantBackground(GI_MessageLoop::TObjectGI* Sender) {
        WindowsSdk::TPoint Position{};
        if (!GlobalsV::HardwareRenderingEnabled) {
            GI_GraphBuf::TGraphBufGI* FaceGB = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"FaceGB"_wref.get()));
            FaceGB->SetActive(false);
            if (!GR_Main::ShowSystemMouse) {
                SetCursorActive(false);
            }
            DrawQueuedUpdateRects();
            if (!GR_Main::ShowSystemMouse) {
                SetCursorActive(true);
            }
            Position = FaceGB->ToAbsolutePoint(ClassesImports::Point(0, 0));
            FaceGB->GraphBuf->AllocateNative(FaceGB->ClientSize.X, FaceGB->ClientSize.Y);
            {
                std::uint8_t* cpp_left = static_cast<std::uint8_t*>(GR_Main::ScreenRenderBuffer->GetPixels()) + Position.X * 2;
                void* cpp_arg = cpp_left + Position.Y * GR_Main::ScreenRenderBuffer->PitchBytes;
                std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                std::int32_t x = FaceGB->ClientSize.X;
                std::int32_t y = FaceGB->ClientSize.Y;
                void* pixels = FaceGB->GraphBuf->GetPixels();
                std::int32_t pitchBytes_2 = FaceGB->GraphBuf->PitchBytes;
                GR_Main::Ex_OKGR_Copy_XY_XY_WORD(pixels, pitchBytes_2, 0, 0, cpp_arg, pitchBytes, 0, 0, x, y);
            }
            FaceGB->GraphBuf->FlipHorizontal16();
            FaceGB->SetActive(true);
        }
    }

    void TfGoodsShop2::MerchantAnimationComplete(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Sequence{};
        if (Sender->UserValue == 1) {
            Sequence = aMyFunction::RandomIntRange(1, 5);
            if (pas::in_set<1, 2>(Sequence)) {
                Sequence = 1;
            } else if (pas::in_set<3, 4>(Sequence)) {
                Sequence = 2;
            } else if (pas::in_range(Sequence, 5, 5)) {
                Sequence = 3;
            } else {
                Sequence = 1;
            }
        } else {
            Sequence = 1;
        }
        if (Sender->UserValue != Sequence) {
            Sender->UserValue = Sequence;
            {
                GI_Image::TImageGI* FaceI = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"FaceI"_wref.get()));
                FaceI->SetImagePath(pas::concat_wide({u"GI,Bm.Captain.", GR_Main::GiResourceSuffix(), u"ShopBot", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Sender->UserValue))), u"i"}));
                FaceI->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                FaceI->SetImageKindX(GI_Main::ikxCenter);
                FaceI->SetImageKindY(GI_Main::ikyCenter);
                FaceI->SetActive(true);
            }
            {
                GI_GAI::TgaiGI* FaceA = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"FaceA"_wref.get()));
                FaceA->FirstFrameOnly = static_cast<std::uint8_t>(GlobalsV::AnimCaptain ^ 1);
                FaceA->SetImagePath(pas::concat_wide({u"Bm.Captain.", GR_Main::GiResourceSuffix(), u"ShopBot", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Sender->UserValue))), u"a"}));
                FaceA->SetHardwareMirrorHorizontal(GlobalsV::HardwareRenderingEnabled);
                FaceA->SequenceIndex = 0;
                FaceA->UpdateAutoGeometry();
                FaceA->SetSequenceFrame(0);
                FaceA->SetImageKindX(GI_Main::ikxCenter);
                FaceA->SetImageKindY(GI_Main::ikyCenter);
                FaceA->SetActive(true);
                FaceA->RestartPlayback();
                FaceA->CycleCompleteCallback = pas::bind_method<&TfGoodsShop2::MerchantAnimationComplete>(this);
            }
        }
    }

    void TfGoodsShop2::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (ParentLoop != nullptr) {
            RequestClose(1);
            GI_Main::BreakUiMessage();
        }
    }

    void TfGoodsShop2::RefreshMoneyWarning() {
        if (MoneyWarningActive && !((MoneyWarningTicks & 1) != 0)) {
            GetByName(u"ADD_WarningMoney"_wref.get())->SetActive(true);
            {
                GI_Label::TLabelGI* ADD_Money = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ADD_Money"_wref.get()));
                ADD_Money->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 128, 61));
                ADD_Money->SetText(pas::wide_int_to_str(aPlayer::GetPlayer()->Money));
                ADD_Money->SetActive(true);
            }
        } else {
            GetByName(u"ADD_WarningMoney"_wref.get())->SetActive(false);
            GetByName(u"ADD_Money"_wref.get())->SetActive(false);
        }
    }

    void TfGoodsShop2::FlashMoneyWarning() {
        if (MoneyWarningTimer != nullptr) {
            CancelCallbackTimer(MoneyWarningTimer);
            MoneyWarningTimer = nullptr;
        }
        MoneyWarningTimer = ScheduleCallbackTimer(100, 100, pas::bind_method<&TfGoodsShop2::MoneyWarningTick>(this), 0);
        MoneyWarningActive = true;
        MoneyWarningTicks = 6;
        RefreshMoneyWarning();
    }

    void TfGoodsShop2::MoneyWarningTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        --MoneyWarningTicks;
        if (MoneyWarningTicks <= 0) {
            if (MoneyWarningTimer != nullptr) {
                CancelCallbackTimer(MoneyWarningTimer);
                MoneyWarningTimer = nullptr;
            }
            MoneyWarningActive = false;
        }
        RefreshMoneyWarning();
    }

    void TfGoodsShop2::RefreshCargoWarning() {
        if (CargoWarningActive && !((CargoWarningTicks & 1) != 0)) {
            GetByName(u"ADD_WarningSpace"_wref.get())->SetActive(true);
            {
                GI_Label::TLabelGI* ADD_Space = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ADD_Space"_wref.get()));
                ADD_Space->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 128, 61));
                ADD_Space->SetText(pas::wide_int_to_str(aPlayer::GetPlayer()->GetCargoFreeSpace()));
                ADD_Space->SetActive(true);
            }
        } else {
            GetByName(u"ADD_WarningSpace"_wref.get())->SetActive(false);
            GetByName(u"ADD_Space"_wref.get())->SetActive(false);
        }
    }

    void TfGoodsShop2::FlashCargoWarning() {
        if (CargoWarningTimer != nullptr) {
            CancelCallbackTimer(CargoWarningTimer);
            CargoWarningTimer = nullptr;
        }
        CargoWarningTimer = ScheduleCallbackTimer(100, 100, pas::bind_method<&TfGoodsShop2::CargoWarningTick>(this), 0);
        CargoWarningActive = true;
        CargoWarningTicks = 6;
        RefreshCargoWarning();
    }

    void TfGoodsShop2::CargoWarningTick(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        --CargoWarningTicks;
        if (CargoWarningTicks <= 0) {
            if (CargoWarningTimer != nullptr) {
                CancelCallbackTimer(CargoWarningTimer);
                CargoWarningTimer = nullptr;
            }
            CargoWarningActive = false;
        }
        RefreshCargoWarning();
    }

    void TfGoodsShop2::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            return;
        }
        if (Key == WindowsSdk::VK_ESCAPE && DraggedGoodsIndex >= 0) {
            DraggedGoodsIndex = -1;
            UpdateActionCursor(false);
            RefreshGoodsDisplay();
        } else if (Key == WindowsSdk::VK_ESCAPE && aPlayer::GetPlayer()->InNormalSpace()) {
            CloseClicked(nullptr);
        } else if (Key == WindowsSdk::VK_SPACE && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            if (GetByName(u"PM_EndTurn"_wref.get())->Active) {
                EndTurnClicked(nullptr);
            }
        } else if (Key == 'S' && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InNormalSpace() ^ 1)) {
            ShipClicked(nullptr);
        } else if (!aPlayer::GetPlayer()->InNormalSpace()) {
            if (Key == 'M' || Key == 'R' || Key == WindowsSdk::VK_ESCAPE || Key == WindowsSdk::VK_F2 || Key == WindowsSdk::VK_F3) {
                AmbientSound->SetVolume(0.0f);
            }
            MainPanel->ProcessKeyDown(Key);
            PlanetPanel->ProcessKeyDown(Key);
            StationPanel->ProcessKeyDown(Key);
            if (Key == 'M' || Key == 'R' || Key == WindowsSdk::VK_ESCAPE || Key == WindowsSdk::VK_F2 || Key == WindowsSdk::VK_F3) {
                AmbientSound->SetVolume(1.0f);
            }
        }
    }

    void TfGoodsShop2::ShowControlHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Visible) {
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace()) {
            GI_Label::TLabelGI* GS_Help = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"GS_Help"_wref.get()));
            if (Sender == nullptr || Sender->HelpText == u"") {
                Visible = false;
            }
            GS_Help->SetActive(Visible);
            if (Visible) {
                GS_Help->SetText(Sender->HelpText);
            }
        } else {
            MainPanel->ShowControlHelp(Sender, Visible);
        }
    }

    void TfGoodsShop2::ShowHelpText(pas::WideString Value, std::uint8_t Visible) {
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InNormalSpace()) {
            GI_Label::TLabelGI* GS_Help = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"GS_Help"_wref.get()));
            GS_Help->SetActive(Visible);
            if (Visible) {
                GS_Help->SetText(Value);
            }
        } else {
            MainPanel->ShowHelpText(Value, Visible);
        }
    }

    void TfGoodsShop2::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop != nullptr && ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(2);
        }
    }

    void TfGoodsShop2::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (MainPanel->NavigationLocked) {
            return;
        }
        if (GR_Main::ExitScreenLoop) {
            return;
        }
        if (pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10006);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20006);
        }
    }

    void TfGoodsShop2::SelectMusic() {
        if (fPanelLoad::ActiveLoadPanel != nullptr && fPanelLoad::ActiveLoadPanel->GetShutterDirection() == -1) {
            return;
        }
        if (!GlobalsV::MusicInPlanetEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
            return;
        }
        if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                if (!aPlayer::GetPlayer()->CurrentPlanet->IsMainPiratePlanet) {
                    GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->CurrentPlanet->RaceId) & 0x0000007f].InternalName, u"Pirate"}));
                } else {
                    GR_Main::MusicManager->PlayCategory(u"Nation.PiratePlanetMain"_wref.get());
                }
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aPlayer::GetPlayer()->CurrentPlanet->OwnerId].InternalName}));
            }
        } else if (aPlayer::GetPlayer()->IsDockedToShip()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (pas::in_set<7, 7, 12, 12>(aPlayer::GetPlayer()->DockedTo->TypeId)) {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName, u"Pirate"}));
            } else {
                GR_Main::MusicManager->PlayCategory(pas::concat_wide({u"Nation.", aConst::OwnerInfo[aConst::RaceToOwner(aPlayer::GetPlayer()->DockedTo->PilotRace) & 0x0000007f].InternalName}));
            }
        }
    }

    void TfGoodsShop2::p_destroy() {
        fGoodsShop2::TfGoodsShop2_Destroy(this);
    }

} // namespace fGoodsShop2
