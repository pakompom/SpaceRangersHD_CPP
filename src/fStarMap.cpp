#include "layout/fStarMap.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Buf.hpp"
#include "types/EC_Thread.hpp"
#include "types/GI_Circle.hpp"
#include "types/GI_MultiImage.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_SpaceCircle.hpp"
#include "types/GI_SpaceImg.hpp"
#include "types/GI_StarField.hpp"
#include "types/GI_StarFieldImg.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Rect.hpp"
#include "types/GR_Sound.hpp"
#include "types/SE_Asteroid.hpp"
#include "types/SE_Container.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Missile.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Star.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/aAsteroid.hpp"
#include "types/aEFilm.hpp"
#include "types/aEObjInfo.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aMissile.hpp"
#include "types/aPath.hpp"
#include "types/aPirate.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/ab_MainForm.hpp"
#include "types/fFilmFile.hpp"
#include "types/fLoadRobot.hpp"
#include "types/fSaveManager.hpp"
#include "units/Achievements.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_GraphButton.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/Robot.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/ThreadCalc.hpp"
#include "units/TypesImports.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aEFilmEnd.hpp"
#include "units/aGalaxy.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlanet.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fGoodsShop2.hpp"
#include "units/fLoad.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fPanelMain.hpp"
#include "units/fShip2.hpp"
#include "units/fStarMap.hpp"
#include "units/fTalk.hpp"

namespace fStarMap {
    pas::Array<float, 0, 15> PanelSlideCurve = pas::Array<float, 0, 15>{{
        0.0f, 0.033f, 0.112f, 0.269f, 0.456f, 0.675f, 0.882f, 1.023f,
        1.09f, 1.127f, 1.129f, 1.111f, 1.064f, 1.036f, 1.011f, 1.0f,
    }};

    std::int32_t FilmCameraLookAheadSteps = 30;

    // Returns milliseconds, adjusted by the configured film speed.
    std::int32_t GetTurnFilmFrameInterval(std::int32_t Activity) {
        std::int32_t Result{};
        if (Activity == 0) {
            Result = 18;
        } else if (Activity == 1) {
            Result = 14;
        } else {
            Result = 10;
        }
        if (GlobalsV::FilmSpeed == 0) {
            Result += 2;
            return Result;
        } else if (GlobalsV::FilmSpeed == 2) {
            Result -= 2;
            return Result;
        } else if (GlobalsV::FilmSpeed == 3) {
            return System::Round(pas::real_divide(Result, 2.0L));
        } else {
            return Result;
        }
    }

    void TfStarMap_Create(TfStarMap* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->UpdateRectsEnabled = false;
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
        Self->ReservedEntries1.set_length(250);
        Self->ReservedEntries2.set_length(250);
        Self->FilmCameraSpeed = 1.0f;
        Self->ShipToInspect = nullptr;
        Self->PendingHoleRefresh = nullptr;
        Self->PendingSceneObjects = pas::make_object<pas::List>();
    }

    void TfStarMap_Destroy(TfStarMap* Self) {
        Self->ReservedEntries1 = nullptr;
        Self->ReservedEntries2 = nullptr;
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        if (Self->PendingSceneObjects != nullptr) {
            pas::free(Self->PendingSceneObjects);
        }
        Self->PendingSceneObjects = nullptr;
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    WindowsSdk::TPoint TfStarMap::GetMapCenter() {
        WindowsSdk::TPoint Result{};
        if (MapControls == nullptr) {
            MapControls = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
        }
        Result = MapControls->ScrollOffset;
        return Result;
    }

    // Disables automatic film-camera following.
    void TfStarMap::SetMapCenterManually(WindowsSdk::TPoint Point) {
        if (MapControls == nullptr) {
            MapControls = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
        }
        MapControls->SetScrollOffset(Point);
        if (Globals::SpaceProcess->IsSpaceOpen()) {
            Globals::SpaceProcess->Space->MapScrollChanged(nullptr);
        }
        Globals::FilmCameraFollow = false;
    }

    void TfStarMap::SetMapCenter(WindowsSdk::TPoint Center) {
        if (MapControls == nullptr) {
            MapControls = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
        }
        MapControls->SetScrollOffset(Center);
        if (Globals::SpaceProcess->IsSpaceOpen()) {
            Globals::SpaceProcess->Space->MapScrollChanged(nullptr);
        }
    }

    void TfStarMap::CenterMapForTalk(EC_Struct::TPointF Position) {
        WindowsSdk::TPoint Offset{};
        Offset = ClassesImports::Point(0, 0);
        ShowObjectInfo(nullptr);
        if (Globals::TalkShip != nullptr) {
            SetMapCenter(EC_Struct::SubtractPoints(EC_Struct::TruncatePointF(Position), Offset));
        } else if (Globals::TalkPlanet != nullptr && Globals::TalkPlanet->CurrentStar == aPlayer::GetPlayer()->CurrentStar) {
            SetMapCenter(EC_Struct::SubtractPoints(EC_Struct::TruncatePointF(Position), Offset));
        }
    }

    void TfStarMap::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        LoadPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fStarMap... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"sv);
            MainPanel->Parent->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->SetPosition(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
            MainPanel->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* CircleActionShr = MainPanel->FindByNameRecursive(u"CircleActionShr"sv);
                CircleActionShr->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                CircleActionShr->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
            }
            {
                GI_MessageLoop::TObjectGI* LargeHelp = MainPanel->FindByNameRecursive(u"LargeHelp"sv);
                LargeHelp->SetPosition(ClassesImports::Point(LargeHelp->LocalPosition.X, LargeHelp->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* MapPanel = MainPanel->FindByNameRecursive(u"MapPanel"sv);
                MapPanel->SetPosition(ClassesImports::Point(MapPanel->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, MapPanel->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* CenterShip = MainPanel->FindByNameRecursive(u"CenterShip"sv);
                CenterShip->SetPosition(ClassesImports::Point(CenterShip->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, CenterShip->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* MapPanelA = MainPanel->FindByNameRecursive(u"MapPanelA"sv);
                MapPanelA->SetPosition(ClassesImports::Point(MapPanelA->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, MapPanelA->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
                {
                    GI_MessageLoop::TObjectGI* cpp_with_7 = MapPanelA->NextSibling;
                    cpp_with_7->SetPosition(ClassesImports::Point(cpp_with_7->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, cpp_with_7->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
                }
            }
            {
                GI_MessageLoop::TObjectGI* FPS = MainPanel->FindByNameRecursive(u"FPS"sv);
                FPS->SetPosition(ClassesImports::Point(FPS->LocalPosition.X, FPS->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* Mods = MainPanel->FindByNameRecursive(u"Mods"sv);
                Mods->SetPosition(ClassesImports::Point(Mods->LocalPosition.X, Mods->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            MainPanel->FindByNameRecursive(u"PanelMain"sv)->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
            {
                GI_MessageLoop::TObjectGI* PanelSpace = MainPanel->FindByNameRecursive(u"PanelSpace"sv);
                PanelSpace->SetPosition(ClassesImports::Point(PanelSpace->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelSpace->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
            if (GR_Main::GiResourceVariant() == 2) {
                MainPanel->FindByNameRecursive(u"PanelLoad"sv)->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
            } else if (GR_Main::GiResourceVariant() == 1) {
                MainPanel->FindByNameRecursive(u"PanelLoad"sv)->SetPosition(ClassesImports::Point(-400, -300));
            }
            {
                GI_MessageLoop::TObjectGI* MapPartnerDuty_Parent = MainPanel->FindByNameRecursive(u"MapPartnerDuty"sv)->Parent;
                MapPartnerDuty_Parent->SetPosition(ClassesImports::Point(MapPartnerDuty_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, MapPartnerDuty_Parent->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* CircleActionColor = MainPanel->FindByNameRecursive(u"CircleActionColor"sv);
                CircleActionColor->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
                CircleActionColor->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* CircleActionWeaponColor = MainPanel->FindByNameRecursive(u"CircleActionWeaponColor"sv);
                CircleActionWeaponColor->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
                CircleActionWeaponColor->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* CircleActionWeaponColor2 = MainPanel->FindByNameRecursive(u"CircleActionWeaponColor2"sv);
                CircleActionWeaponColor2->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
                CircleActionWeaponColor2->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* CircleActionWeaponColor3 = MainPanel->FindByNameRecursive(u"CircleActionWeaponColor3"sv);
                CircleActionWeaponColor3->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
                CircleActionWeaponColor3->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* CircleActionWeaponColor4 = MainPanel->FindByNameRecursive(u"CircleActionWeaponColor4"sv);
                CircleActionWeaponColor4->SetPosition(ClassesImports::Point(-pas::shr(GR_Main::GameScreenWidth, 1), -pas::shr(GR_Main::GameScreenHeight, 1)));
                CircleActionWeaponColor4->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* SpaceImg = MainPanel->FindByNameRecursive(u"SpaceImg"sv);
                SpaceImg->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                SpaceImg->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* StarField = MainPanel->FindByNameRecursive(u"StarField"sv);
                StarField->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                StarField->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* StarFieldImg = MainPanel->FindByNameRecursive(u"StarFieldImg"sv);
                StarFieldImg->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                StarFieldImg->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* StarFieldM = MainPanel->FindByNameRecursive(u"StarFieldM"sv);
                StarFieldM->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                StarFieldM->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            }
            {
                GI_MessageLoop::TObjectGI* Info = MainPanel->FindByNameRecursive(u"Info"sv);
                Info->SetPosition(ClassesImports::Point(Info->LocalPosition.X - GR_Main::ExtraScreenWidth / 2, Info->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* InfoStd = MainPanel->FindByNameRecursive(u"InfoStd"sv);
                InfoStd->SetPosition(ClassesImports::Point(InfoStd->LocalPosition.X - GR_Main::ExtraScreenWidth / 2, InfoStd->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* InfoShip = MainPanel->FindByNameRecursive(u"InfoShip"sv);
                InfoShip->SetPosition(ClassesImports::Point(InfoShip->LocalPosition.X - GR_Main::ExtraScreenWidth / 2, InfoShip->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* InfoPlanet = MainPanel->FindByNameRecursive(u"InfoPlanet"sv);
                InfoPlanet->SetPosition(ClassesImports::Point(InfoPlanet->LocalPosition.X - GR_Main::ExtraScreenWidth / 2, InfoPlanet->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* InfoStar = MainPanel->FindByNameRecursive(u"InfoStar"sv);
                InfoStar->SetPosition(ClassesImports::Point(InfoStar->LocalPosition.X - GR_Main::ExtraScreenWidth / 2, InfoStar->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
            {
                GI_MessageLoop::TObjectGI* InfoItem = MainPanel->FindByNameRecursive(u"InfoItem"sv);
                InfoItem->SetPosition(ClassesImports::Point(InfoItem->LocalPosition.X - GR_Main::ExtraScreenWidth / 2, InfoItem->LocalPosition.Y - GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        MapControls = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MainPanel"sv));
        PartnerPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MapPartner"sv));
        SecondaryPartnerPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"MapPartner2"sv));
        InfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"Info"sv));
        InfoTextLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoText"sv));
        ItemInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"InfoItem"sv));
        ShipInfoPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoShip"sv));
        PlanetInfoPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoPlanet"sv));
        StarInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"InfoStar"sv));
        StandardInfoPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoStd"sv));
        this->StarField = pas::checked_cast<GI_StarField::TStarFieldGI*>(GetByName(u"StarField"sv));
        ActionCircle = pas::checked_cast<GI_Circle::TCircleGI*>(GetByName(u"CircleActionShr"sv));
        ActionColorCircle = pas::checked_cast<GI_SpaceCircle::TSpaceCircleGI*>(GetByName(u"CircleActionColor"sv));
        WeaponColorCircles[0] = pas::checked_cast<GI_SpaceCircle::TSpaceCircleGI*>(GetByName(u"CircleActionWeaponColor"sv));
        WeaponColorCircles[1] = pas::checked_cast<GI_SpaceCircle::TSpaceCircleGI*>(GetByName(u"CircleActionWeaponColor2"sv));
        WeaponColorCircles[2] = pas::checked_cast<GI_SpaceCircle::TSpaceCircleGI*>(GetByName(u"CircleActionWeaponColor3"sv));
        WeaponColorCircles[3] = pas::checked_cast<GI_SpaceCircle::TSpaceCircleGI*>(GetByName(u"CircleActionWeaponColor4"sv));
        WeaponButtons[0] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_W0"sv));
        WeaponButtons[1] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_W1"sv));
        WeaponButtons[2] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_W2"sv));
        WeaponButtons[3] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_W3"sv));
        WeaponButtons[4] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_W4"sv));
        WeaponImages[0] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PS_W0I"sv));
        WeaponImages[1] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PS_W1I"sv));
        WeaponImages[2] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PS_W2I"sv));
        WeaponImages[3] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PS_W3I"sv));
        WeaponImages[4] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PS_W4I"sv));
        AllWeaponsButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_WA"sv));
        HideSpacePanelButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_Hide"sv));
        ShowSpacePanelButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_Show"sv));
        ScannerButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_Scaner"sv));
        TalkButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_Talk"sv));
        TurnFilmButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_Film"sv));
        MapControls->KeyDownCallback = pas::bind_method<&TfStarMap::MapKeyDown>(this);
        MapControls->KeyUpCallback = pas::bind_method<&TfStarMap::MapKeyUp>(this);
        MapControls->SetDragScrollingEnabled(true);
        MapControls->ScrollType = GI_Panel::pstSimple;
        WeaponBackgroundImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PS_ImageWeaponBG"sv));
        SpaceBackgroundImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"PS_ImageBG"sv));
        LargeHelpBuffer = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"LargeHelp"sv));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"sv))->UpCallback = pas::bind_method<&TfStarMap::ShipClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Gal"sv))->UpCallback = pas::bind_method<&TfStarMap::GalaxyClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PS_Film"sv))->UpCallback = pas::bind_method<&TfStarMap::OpenFilmHistoryClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"sv))->UpCallback = pas::bind_method<&TfStarMap::EndTurnClicked>(this);
        CenterShipButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"CenterShip"sv));
        AllWeaponsButton->UpCallback = pas::bind_method<&TfStarMap::AllWeaponsClicked>(this);
        ScannerButton->UpCallback = pas::bind_method<&TfStarMap::ScannerClicked>(this);
        TalkButton->UpCallback = pas::bind_method<&TfStarMap::TalkClicked>(this);
        HideSpacePanelButton->UpCallback = pas::bind_method<&TfStarMap::ToggleWeaponPanelClicked>(this);
        ShowSpacePanelButton->UpCallback = pas::bind_method<&TfStarMap::ToggleWeaponPanelClicked>(this);
        WeaponPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PS_Up"sv));
        WeaponPanelRestTop = WeaponPanel->LocalPosition.Y;
        SpacePanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelSpace"sv));
        SpacePanelRestTop = SpacePanel->LocalPosition.Y;
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"MapPanel"sv))->BindExternalGraphBuf(GR_Main::RenderScratchBuffer);
        BattleMusicSelected = false;
        EndTurnAfterOpen = false;
    }

    void TfStarMap::OnOpen() {
        aEFilmEnd::PEFilmEndEntry Entry{};
        std::int32_t MapIndex{};
        pas::WideString StartText{};
        pas::WideString WinText{};
        pas::WideString LossText{};
        pas::WideString TerronName{};
        std::uint8_t BattleFailed{};
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->RuinsMode > 0) {
            aPlayer::GetPlayer()->ExitRuinsMode();
        }
        if (aScript::DispatchPendingScriptRequests()) {
            return;
        }
        EC_Cache::EvictMainMenuShipCachesWhenAddressSpaceHigh();
        if (!Globals::ShipScreen->ReopenRequested) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(1116);
        } else {
            aGalaxy::TGalaxy::ClearIntegrityStatus();
        }
        LoadPanel->OnOpen();
        BreakOnNextFilm = false;
        AnimateSpacePanelOnResume = false;
        if (PlanetBattleState == 1) {
            MapIndex = Globals::FindRobotMapById(PlanetBattleMapId);
            StartText = Globals::RobotMapDefinitions[MapIndex].RobotsStart;
            aMyFunction::ReplaceTextToken(StartText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
            aMyFunction::ReplaceTextToken(StartText, u"<Player>"_w, aPlayer::GetPlayer()->Name, aMyFunction::TextHighlightColorTag);
            aConst::ExpandLocalizedTextMarkupAndPrefixLines(StartText);
            StartText = pas::concat_wide({pas::wide_int_to_str(1), StartText});
            StartText = pas::concat_wide({pas::wide_int_to_str(std::min<std::int32_t>(static_cast<std::int32_t>(aGalaxy::Galaxy->GetDifficultyTierIndex()), 3) + 1), StartText});
            StartText = pas::concat_wide({pas::wide_int_to_str(6), StartText});
            WinText = Globals::RobotMapDefinitions[MapIndex].RobotsWin;
            aMyFunction::ReplaceTextToken(WinText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
            aMyFunction::ReplaceTextToken(WinText, u"<Player>"_w, aPlayer::GetPlayer()->Name, aMyFunction::TextHighlightColorTag);
            aConst::ExpandLocalizedTextMarkupAndPrefixLines(WinText);
            LossText = Globals::RobotMapDefinitions[MapIndex].RobotsLoss;
            aMyFunction::ReplaceTextToken(LossText, u"<Star>"_w, aPlayer::GetPlayer()->CurrentStar->Name, aMyFunction::TextHighlightColorTag);
            aMyFunction::ReplaceTextToken(LossText, u"<Player>"_w, aPlayer::GetPlayer()->Name, aMyFunction::TextHighlightColorTag);
            aConst::ExpandLocalizedTextMarkupAndPrefixLines(LossText);
            if (aKling::TerronShip != nullptr) {
                TerronName = aKling::TerronShip->GetFullName(u" "_wref.get());
            }
            if (ThreadCalc::IsTurnCalculationRunning() && WindowsSdk::WaitForSingleObject(Globals::ScriptUiRequestEvent, 0u) != WindowsSdk::WAIT_OBJECT_0) {
                ThreadCalc::WaitForTurnCalculation();
            }
            if (!GlobalsV::MemorySnapshotActive) {
                aSaveLoad::SaveGameToMemorySnapshot();
            }
            LoadPanel->OnOpen();
            LoadPanel->SelectBackgroundStyle(3);
            LoadPanel->RefreshBackgroundImages();
            try {
                BattleFailed = false;
                PlanetBattleState = Robot::FRun(Globals::RobotMapDefinitions[MapIndex].Map, StartText, WinText, LossText, TerronName);
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    BattleFailed = true;
                } else {
                    throw;
                }
            }
            if (GlobalsV::MemorySnapshotActive) {
                aSaveLoad::RestoreGameFromMemorySnapshot();
            }
            if (BattleFailed) {
                if (GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"FormGov.BattlePlanetQuestCrashed"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                    PlanetBattleState = 3;
                } else {
                    pas::raise(pas::make_exception<pas::Exception>("Error in Matrix.dll"_a));
                }
            }
            if (PlanetBattleState != 0) {
                if (PlanetBattleState == 1) {
                    PlanetBattleState = 0;
                    if (aSaveLoad::MemorySnapshotBuffer != nullptr) {
                        pas::free(aSaveLoad::MemorySnapshotBuffer);
                    }
                    aSaveLoad::MemorySnapshotBuffer = nullptr;
                    GlobalsV::MemorySnapshotActive = false;
                    if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1)) {
                        pas::free(aGalaxy::Galaxy);
                    }
                    aGalaxy::Galaxy = nullptr;
                    Globals::ScreenLoadMode = 4;
                    GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
                    GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
                    GR_DX::ReleaseAllTextureSurfaces();
                    RequestClose(1);
                } else {
                    GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
                    RequestClose(1);
                }
            }
        } else if (pas::in_set<2, 2, 4, 4>(PlanetBattleState)) {
            aPlayer::GetPlayer()->PlanetBattleHistory.set_length(aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1 + 1 + 1);
            {
                aPlayer::TPlanetBattleHistoryEntry& cpp_with = aPlayer::GetPlayer()->PlanetBattleHistory[aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1];
                cpp_with.MapId = PlanetBattleMapId;
                cpp_with.Statistics.SignedTimeMs = Robot::RobotBattleStatistics.SignedTimeMs;
                cpp_with.Statistics.RobotsBuilt = Robot::RobotBattleStatistics.RobotsBuilt;
                cpp_with.Statistics.RobotsDestroyed = Robot::RobotBattleStatistics.RobotsDestroyed;
                cpp_with.Statistics.TurretsBuilt = Robot::RobotBattleStatistics.TurretsBuilt;
                cpp_with.Statistics.TurretsDestroyed = Robot::RobotBattleStatistics.TurretsDestroyed;
                cpp_with.Statistics.BuildingsDestroyed = Robot::RobotBattleStatistics.BuildingsDestroyed;
                cpp_with.ResultCode = 1;
                cpp_with.CompletionMode = PlanetBattleState;
                cpp_with.DateTurn = aGalaxy::Galaxy->CurrentTurn;
            }
            aGalaxy::Galaxy->TerronLandingLockTurn = 0;
            PlanetBattleState = 0;
            aPlayer::GetPlayer()->OrderTakeoff();
            Globals::FilmCameraFollow = true;
            aGalaxy::PlayerStar->RefreshSpaceObjectPositions();
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(aGalaxyStruct::heHolyFanaticism)) {
                aGalaxy::Galaxy->EnableDominatorSurfaces();
            } else {
                aGalaxy::Galaxy->DisableDominatorSurfaces();
            }
            aCalc::CalculatePlayerStarTurnAndWait();
            if (GR_Main::ExitScreenLoop) {
                return;
            }
            if (aPlayer::GetPlayer() != nullptr) {
                aCalc::CalculateGalaxyTurnAndWait();
            }
            if (aPlayer::GetPlayer() == nullptr) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
                RequestClose(1);
                return;
            }
            LoadPanel->OnOpen();
            LoadPanel->SelectBackgroundStyle(0);
            LoadPanel->RefreshBackgroundImages();
            Globals::StarMapScreen->ResumeMode = smrTurnFilm;
            Globals::ScreenLoadMode = 2;
            GlobalsV::PostLoadScreenId = GlobalsV::screenStarMap;
            GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
            GR_DX::ReleaseAllTextureSurfaces();
            RequestClose(1);
        } else if (PlanetBattleState == 3) {
            aPlayer::GetPlayer()->PlanetBattleHistory.set_length(aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1 + 1 + 1);
            {
                aPlayer::TPlanetBattleHistoryEntry& cpp_with_2 = aPlayer::GetPlayer()->PlanetBattleHistory[aPlayer::GetPlayer()->PlanetBattleHistory.length() - 1];
                cpp_with_2.MapId = PlanetBattleMapId;
                cpp_with_2.Statistics.SignedTimeMs = Robot::RobotBattleStatistics.SignedTimeMs;
                cpp_with_2.Statistics.RobotsBuilt = Robot::RobotBattleStatistics.RobotsBuilt;
                cpp_with_2.Statistics.RobotsDestroyed = Robot::RobotBattleStatistics.RobotsDestroyed;
                cpp_with_2.Statistics.TurretsBuilt = Robot::RobotBattleStatistics.TurretsBuilt;
                cpp_with_2.Statistics.TurretsDestroyed = Robot::RobotBattleStatistics.TurretsDestroyed;
                cpp_with_2.Statistics.BuildingsDestroyed = Robot::RobotBattleStatistics.BuildingsDestroyed;
                cpp_with_2.ResultCode = 1;
                cpp_with_2.CompletionMode = PlanetBattleState;
                cpp_with_2.DateTurn = aGalaxy::Galaxy->CurrentTurn;
            }
            ++aPlayer::GetPlayer()->PlanetBattles;
            PlanetBattleState = 0;
            Globals::LoadRobotScreen->LoadCompletionData();
            Globals::LoadRobotScreen->RecordCompletion(PlanetBattleMapId, -Robot::RobotBattleStatistics.SignedTimeMs / 1000, 2);
            Globals::LoadRobotScreen->SaveCompletionData();
            Achievements::TryAddAchievementProgress(u"IRONMAN"_w, 1);
            if (aKling::TerronShip != nullptr) {
                aKling::TerronShip->DestroyQueued = true;
            }
            aPlayer::GetPlayer()->OrderTakeoff();
            Globals::FilmCameraFollow = true;
            aGalaxy::PlayerStar->RefreshSpaceObjectPositions();
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(aGalaxyStruct::heHolyFanaticism)) {
                aGalaxy::Galaxy->EnableDominatorSurfaces();
            } else {
                aGalaxy::Galaxy->DisableDominatorSurfaces();
            }
            aCalc::CalculatePlayerStarTurnAndWait();
            if (GR_Main::ExitScreenLoop) {
                return;
            }
            if (aPlayer::GetPlayer() != nullptr) {
                aCalc::CalculateGalaxyTurnAndWait();
            }
            if (aPlayer::GetPlayer() == nullptr) {
                GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
                RequestClose(1);
                return;
            }
            LoadPanel->OnOpen();
            LoadPanel->SelectBackgroundStyle(0);
            LoadPanel->RefreshBackgroundImages();
            Globals::StarMapScreen->ResumeMode = smrTurnFilm;
            Globals::ScreenLoadMode = 2;
            GlobalsV::PostLoadScreenId = GlobalsV::screenStarMap;
            GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
            GR_DX::ReleaseAllTextureSurfaces();
            RequestClose(1);
        } else {
            PlanetBattleState = 0;
            if (!Globals::ShipScreen->ReopenRequested) {
                aPlayer::GetPlayer()->CancelInvalidTravelOrder();
            }
            ClearMapAnimations();
            GetByName(u"FPS"sv)->SetActive(GR_Main::ShowFrameRate);
            RefreshScoreModsLabel();
            SetMapCenter(EC_Struct::TruncatePointF(Globals::SpaceViewPosition));
            MainPanel->OnOpen();
            MainPanel->Hide();
            GetByName(u"PanelSpace"sv)->SetActive(false);
            UpdateRectsEnabled = true;
            MapControls->Invalidate();
            UpdateRectsEnabled = false;
            aGalaxy::PlayerStar->OpenSpaceScene(MapControls, GetByName(u"MapPanel"sv), this);
            if (ResumeMode != smrTurnFilm) {
                aGalaxy::PlayerStar->RefreshSpaceObjectPositions();
            }
            {
                GI_SpaceImg::TSpaceImgGI* cpp_arg = pas::checked_cast<GI_SpaceImg::TSpaceImgGI*>(GetByName(u"SpaceImg"sv));
                GI_StarField::TStarFieldGI* cpp_arg_2 = pas::checked_cast<GI_StarField::TStarFieldGI*>(GetByName(u"StarField"sv));
                TfStarMap::BuildSpaceBackground(cpp_arg_2, cpp_arg, aGalaxy::PlayerStar->GenerationSeed, aGalaxy::PlayerStar->BackgroundImage);
            }
            MapScrollTimer = ScheduleCallbackTimer(GlobalsV::ScrollTime, GlobalsV::ScrollTime, pas::bind_method<&TfStarMap::ScrollMap>(this), 0);
            {
                GI_StarFieldImg::TStarFieldImgGI* StarFieldImg = pas::checked_cast<GI_StarFieldImg::TStarFieldImgGI*>(GetByName(u"StarFieldImg"sv));
                StarFieldImg->SetActive(GlobalsV::Wind >= 2);
                if (StarFieldImg->StarCount <= 0) {
                    StarFieldImg->SeedStars();
                }
            }
            {
                GI_MessageLoop::TObjectGI* findControlByPath = FindControlByPath(u"StarFieldM"_wref.get());
                std::uint8_t cpp_arg_3 = GlobalsV::Wind >= 1;
                findControlByPath->SetActive(cpp_arg_3);
            }
            if (ResumeMode == smrTurnFilm) {
                SelectMusic();
                RebuildPartnerButtons();
                StartTurnFilm();
            } else if (ResumeMode == smrWaitForTurn) {
                if (!GlobalsV::MusicInSpaceEnabled) {
                    GR_Main::MusicManager->RequestFadeOut();
                }
                WaitForTurnOrTalk();
            } else {
                if (!GlobalsV::MusicInSpaceEnabled) {
                    GR_Main::MusicManager->RequestFadeOut();
                }
                StartOrderMode();
            }
            ResumeMode = smrNormal;
            GetByName(u"MapPanelA"sv)->SetActive(aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(aGalaxyStruct::heBlindness));
            if (Globals::ShipScreen->ReopenRequested) {
                SetCursorActive(false);
                MainPanel->RefreshMoneyAndCargo();
                GR_Main::FullFrameRedrawRequested = true;
                DrawFrame();
                GR_Main::CaptureScreenBackground(true, 0);
                GlobalsV::ShipReturnScreenId = GlobalsV::FormToId(this);
                GlobalsV::RequestedScreenId = GlobalsV::screenShip;
                RequestClose(1);
                HideLargeHelp();
                aGalaxy::TGalaxy::ClearIntegrityStatus();
            } else {
                if (Globals::TrailingFilmEffects != nullptr) {
                    Entry = Globals::TrailingFilmEffects->FirstEntry;
                    while (Entry != nullptr) {
                        Entry->SceneObject->AttachToSpace(Globals::SpaceProcess->Space);
                        if (Entry->RelatedObject1 != nullptr) {
                            Entry->RelatedObject1->AttachToSpace(Globals::SpaceProcess->Space);
                        }
                        Entry = Entry->Next;
                    }
                }
                RestorePendingSceneObjects();
                SetCursorActive(true);
                ShipToInspect = nullptr;
                HideLargeHelp();
                aGalaxy::PlayerStar->RefreshMovementStepParameters();
                if (aPlayer::GetPlayer() != nullptr) {
                    aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnEnteringForm, nullptr, nullptr, 0);
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(1117);
                if (EndTurnAfterOpen) {
                    EndTurnAfterOpen = false;
                    Globals::ShipScreen->ReopenRequested = false;
                    EndTurnClicked(nullptr);
                }
            }
        }
    }

    void TfStarMap::OnClose() {
        std::int32_t Stage = 0;
        try {
            if (aGalaxy::Galaxy != nullptr && Mode == smmOrders) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(2);
            }
            Stage = 1;
            if (DeferredEndTurnTimer != nullptr) {
                CancelCallbackTimer(DeferredEndTurnTimer);
                DeferredEndTurnTimer = nullptr;
            }
            if (PlayerPathTimer != nullptr) {
                CancelCallbackTimer(PlayerPathTimer);
                PlayerPathTimer = nullptr;
            }
            Stage = 2;
            if (LargeHelpTimer != nullptr) {
                CancelCallbackTimer(LargeHelpTimer);
                LargeHelpTimer = nullptr;
            }
            Stage = 3;
            if (TerronFadeImage != nullptr) {
                pas::free(TerronFadeImage);
                TerronFadeImage = nullptr;
            }
            Stage = 4;
            ClearPartnerButtons();
            Stage = 5;
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(GlobalsV::MemorySnapshotActive ^ 1)) {
                SaveSpaceBackground();
            }
            Stage = 6;
            if (MapScrollTimer != nullptr) {
                CancelCallbackTimer(MapScrollTimer);
                MapScrollTimer = nullptr;
            }
            Stage = 7;
            if (Globals::TrailingFilmEffects != nullptr) {
                pas::free(Globals::TrailingFilmEffects);
                Globals::TrailingFilmEffects = nullptr;
            }
            Stage = 8;
            if (Globals::SecondaryFilm != nullptr) {
                Globals::SecondaryFilm->ReleaseWeaponSceneObjects();
            }
            Stage = 9;
            pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoStarPanel"sv))->FreeOwnedChildren();
            Stage = 10;
            MainPanel->OnClose();
            Stage = 11;
            LoadPanel->OnClose();
            Stage = 12;
            if (aGalaxy::Galaxy != nullptr) {
                if (Mode == smmOrders) {
                    StopOrderMode();
                } else if (Mode == smmTurnFilm) {
                    StopTurnFilm(true);
                }
            }
            Stage = 13;
            if (aGalaxy::Galaxy != nullptr) {
                Globals::SpaceProcess->CloseSpace();
            }
            Stage = 14;
            if (Globals::CacheLoader->IsRunning()) {
                Globals::CacheLoader->WaitForIdle(WindowsSdk::INFINITE);
            }
            Stage = 15;
            ThreadCalc::WaitForTurnCalculation();
            Stage = 16;
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnLeavingForm, nullptr, nullptr, 0);
            }
            GR_Main::CacheLoadLoggingEnabled = false;
            aGalaxy::TGalaxy::ClearIntegrityStatus();
            Stage = 17;
            if (GR_Main::AuxRenderBuffer != nullptr) {
                GR_Main::AuxRenderBuffer->Clear();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TfStarMap.AfterRun, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfStarMap::RestorePendingSceneObjects() {
        aEFilmEnd::PEFilmEndEntry Entry{};
        std::int32_t Index{};
        if (pas::list_count(PendingSceneObjects) > 0) {
            if (Globals::TrailingFilmEffects == nullptr) {
                Globals::TrailingFilmEffects = pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(PendingSceneObjects) - 1); cpp_range.next(Index); ) {
                Entry = Globals::TrailingFilmEffects->AppendEntry();
                SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject), pas::list_at<SE_Space::TObjectSE>(PendingSceneObjects, Index));
                SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1), nullptr);
                Entry->SceneObject->AttachToSpace(Globals::SpaceProcess->Space);
            }
            pas::list_clear(PendingSceneObjects);
        }
        if (PendingHoleRefresh != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Holes) - 1); cpp_range_2.next(Index); ) {
                if (pas::list_get(aGalaxy::Galaxy->Holes, Index) == PendingHoleRefresh) {
                    aGalaxy::THole* cpp_with = PendingHoleRefresh;
                    if (aPlayer::GetPlayer()->CurrentStar == cpp_with->Star1 || aPlayer::GetPlayer()->CurrentStar == cpp_with->Star2) {
                        if (aPlayer::GetPlayer()->CurrentStar == cpp_with->Star1) {
                            cpp_with->Graphic->SetPosition(cpp_with->Position1);
                        } else {
                            cpp_with->Graphic->SetPosition(cpp_with->Position2);
                        }
                        cpp_with->Graphic->DetachFromSpace();
                        reinterpret_cast<SE_Hole::THoleSE*>(cpp_with->Graphic)->SetState(1);
                        cpp_with->Graphic->AttachToSpace(Globals::SpaceProcess->Space);
                    }
                    break;
                }
            }
            PendingHoleRefresh = nullptr;
        }
    }

    void TfStarMap::BuildSpaceBackground(GI_StarField::TStarFieldGI* StarField, GI_SpaceImg::TSpaceImgGI* SpaceImage, std::uint32_t Seed, std::int32_t BackgroundIndex) {
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t Index{};
        std::int32_t Attempts{};
        float Depth{};
        GI_SpaceImg::PSpaceImageGI Image{};
        pas::WideString Text{};
        pas::Array<std::uint32_t, 0, 15> Colors{};
        Seed = aMyFunction::StepRandomSeed(Seed);
        std::int32_t Radius = System::Round(aGalaxy::PlayerStar->ComputeMapDiameter() / 2);
        std::int32_t Diameter = aGalaxy::PlayerStar->ComputeMapDiameter();
        double Scale = aMyFunction::RemapClamped(Radius, 2.5E+3, 4.5E+3, 1.0, 2.0);
        std::int32_t Count = System::Round(Scale * 2.0E+3L);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 3); cpp_range.next(Index); ) {
            Colors[Index] = GR_Main::CurrentPixelFormat->PackRgbBytes(100 + 16 * Index, 100 + 16 * Index, 100 + 16 * Index);
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 3); cpp_range_2.next(Index); ) {
            Colors[Index + 4] = GR_Main::CurrentPixelFormat->PackRgbBytes(100 + 40 * Index, 1, 1);
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 3); cpp_range_3.next(Index); ) {
            Colors[Index + 8] = GR_Main::CurrentPixelFormat->PackRgbBytes(1, 100 + 40 * Index, 1);
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 3); cpp_range_4.next(Index); ) {
            Colors[Index + 12] = GR_Main::CurrentPixelFormat->PackRgbBytes(1, 1, 100 + 40 * Index);
        }
        StarField->Stars->Clear();
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(System::Round(Count * 0.6L))); cpp_range_5.next(Index); ) {
            X = aMyFunction::SeededRandomIntRange(-Radius * 5, Radius * 5, Seed);
            Seed = aMyFunction::StepRandomSeed(Seed);
            Y = aMyFunction::SeededRandomIntRange(-Radius * 5, Radius * 5, Seed);
            Seed = aMyFunction::StepRandomSeed(Seed);
            Depth = aMyFunction::SeededRandomIntRange(2, 8, Seed);
            Seed = aMyFunction::StepRandomSeed(Seed);
            {
                std::int32_t cpp_arg = Colors[aMyFunction::RandomIntRange(4, 15)];
                GI_StarField::TStarFieldList* stars = StarField->Stars;
                stars->AddPoint(X, Y, Depth, cpp_arg);
            }
        }
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(System::Round(Count * 0.3L))); cpp_range_6.next(Index); ) {
            X = aMyFunction::SeededRandomIntRange(-Diameter, Diameter, Seed);
            Seed = aMyFunction::StepRandomSeed(Seed);
            Y = aMyFunction::SeededRandomIntRange(-Diameter, Diameter, Seed);
            Seed = aMyFunction::StepRandomSeed(Seed);
            Depth = pas::real_divide(aMyFunction::SeededRandomIntRange(1, 100, Seed), 1.0E+2L) + 1.0L;
            Seed = aMyFunction::StepRandomSeed(Seed);
            {
                std::int32_t cpp_arg_2 = Colors[aMyFunction::RandomIntRange(4, 15)];
                GI_StarField::TStarFieldList* stars_2 = StarField->Stars;
                stars_2->AddPoint(X, Y, Depth, cpp_arg_2);
            }
        }
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, static_cast<std::int32_t>(System::Round(Count * 0.1L))); cpp_range_7.next(Index); ) {
            X = aMyFunction::SeededRandomIntRange(-Radius, Radius, Seed);
            Seed = aMyFunction::StepRandomSeed(Seed);
            Y = aMyFunction::SeededRandomIntRange(-Radius, Radius, Seed);
            Seed = aMyFunction::StepRandomSeed(Seed);
            Depth = 1.1f;
            {
                std::int32_t cpp_arg_3 = Colors[aMyFunction::RandomIntRange(4, 15)];
                GI_StarField::TStarFieldList* stars_3 = StarField->Stars;
                stars_3->AddPoint(X, Y, Depth, cpp_arg_3);
            }
        }
        StarField->MarkViewDirty();
        StarField->Invalidate();
        System::RandSeed = aGalaxy::PlayerStar->GenerationSeed;
        Count = System::Round(aMyFunction::RemapClamped(Scale, 1.0, 2.0, 1.0, 2.0));
        if (GlobalsV::SpaceImage == 1 && Count > 1) {
            Count = 1;
        }
        if (BackgroundIndex < 10) {
            Text = ([&] {
                const pas::WideString& cpp_arg_4 = static_cast<pas::WideString>(pas::concat_ansi({"0", SysUtils::IntToStr(BackgroundIndex)}));
                EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"StyleNebula"_wref.get());
                return blockByPath->GetParam(pas::view(cpp_arg_4));
            }());
        } else {
            Text = ([&] {
                const pas::WideString& intToStr = pas::wide_int_to_str(BackgroundIndex);
                EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(u"StyleNebula"_wref.get());
                return blockByPath_2->GetParam(pas::view(intToStr));
            }());
        }
        Index = aMyFunction::SeededRandomIntRange(0, EC_Str::CountDelimitedPartsW(pas::view(Text), u","sv) - 1, Seed);
        Seed = aMyFunction::StepRandomSeed(Seed);
        std::int32_t Kind = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), Index, u","sv)));
        SpaceImage->ClearImages();
        if (GlobalsV::SpaceImage > 0) {
            for (auto cpp_range_8 = pas::for_to<std::int32_t>(1, Count); cpp_range_8.next(Index); ) {
                Attempts = 0;
                do {
                    X = aMyFunction::SeededRandomIntRange(-Diameter, Diameter, Seed);
                    Seed = aMyFunction::StepRandomSeed(Seed);
                    Y = aMyFunction::SeededRandomIntRange(-Diameter, Diameter, Seed);
                    Seed = aMyFunction::StepRandomSeed(Seed);
                    {
                        pas::Extended cpp_left = aMyFunction::RandomFloatRange(5.5, 6.0) * 1.1L;
                        Depth = cpp_left + aMyFunction::RemapClamped(Radius, 2.5E+3, 4.0E+3, 0.0, 3.0);
                    }
                    Seed = aMyFunction::StepRandomSeed(Seed);
                    ++Attempts;
                } while (!(([&] {
                    pas::Extended cpp_left_2 = SpaceImage->NearestImageDistance(X, Y);
                    return cpp_left_2 > aMyFunction::RemapClamped(Radius, 2.5E+3, 4.0E+3, 3.0E+3, 6.0E+3);
                }()) || Attempts > 100));
                if (Attempts > 100) {
                    break;
                }
                SpaceImage->AddImage(Globals::SelectSpaceImageTemplate(Kind), X, Y, Depth);
                Seed = aMyFunction::StepRandomSeed(Seed);
            }
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->IsHealthEffectActive(aGalaxyStruct::heChekumash)) {
                Attempts = 0;
                do {
                    X = aMyFunction::SeededRandomIntRange(-Radius / 2, Radius / 2, Seed);
                    Seed = aMyFunction::StepRandomSeed(Seed);
                    Y = aMyFunction::SeededRandomIntRange(-Radius / 2, Radius / 2, Seed);
                    Seed = aMyFunction::StepRandomSeed(Seed);
                    {
                        pas::Extended cpp_left_3 = aMyFunction::RandomFloatRange(2.5, 3.0) * 1.1L;
                        Depth = cpp_left_3 + aMyFunction::RemapClamped(Radius, 2.5E+3, 4.0E+3, 0.0, 3.0);
                    }
                    if (aGalaxy::Galaxy->SpaceEffectKind == 1) {
                        X = -pas::abs(X);
                        Y = pas::abs(X);
                        Depth = 1.5f;
                    }
                    Seed = aMyFunction::StepRandomSeed(Seed);
                    ++Attempts;
                } while (!(([&] {
                    pas::Extended cpp_left_4 = SpaceImage->NearestImageDistance(X, Y);
                    return cpp_left_4 > aMyFunction::RemapClamped(Radius, 1.5E+3, 2.0E+3, 1.5E+3, 3.0E+3);
                }()) || Attempts > 100));
                SpaceImage->AddImage(Globals::SelectSpaceImageTemplate(aGalaxy::Galaxy->SpaceEffectKind + 10), X, Y, Depth);
            }
            if (aGalaxy::Galaxy->SpaceBackgroundEntries.length() - 1 + 1 <= 0) {
                aGalaxy::Galaxy->GenerateSpaceBackground(BackgroundIndex);
            }
            for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, aGalaxy::Galaxy->SpaceBackgroundEntries.length() - 1); cpp_range_9.next(Index); ) {
                Image = SpaceImage->AddImage(aGalaxy::Galaxy->SpaceBackgroundEntries[Index].ImageIndex, aGalaxy::Galaxy->SpaceBackgroundEntries[Index].Position.X, aGalaxy::Galaxy->SpaceBackgroundEntries[Index].Position.Y, aGalaxy::Galaxy->SpaceBackgroundEntries[Index].Position.Z);
                pas::store_unaligned<EC_Struct::TVector3D>(&Image->OrbitCenter, aGalaxy::Galaxy->SpaceBackgroundEntries[Index].OrbitCenter);
                Image->OrbitStepDegrees = aGalaxy::Galaxy->SpaceBackgroundEntries[Index].OrbitStepDegrees;
                Image->SavedTemplateIndex = aGalaxy::Galaxy->SpaceBackgroundEntries[Index].ImageIndex;
                Image->FrameIndex = aGalaxy::Galaxy->SpaceBackgroundEntries[Index].FrameIndex;
                GI_SpaceImg::TSpaceImgGI::UpdateImageOrbitAndFrame(Image);
            }
            SpaceImage->AnimateImages(nullptr, 0);
            SpaceImage->ProjectImages();
            SpaceImage->Invalidate();
        }
        if (BackgroundIndex < 10) {
            StarField->SetBackgroundImage(static_cast<pas::WideString>(pas::concat_ansi({"Bm.BGO.bg0", SysUtils::IntToStr(BackgroundIndex)})));
        } else {
            StarField->SetBackgroundImage(static_cast<pas::WideString>(pas::concat_ansi({"Bm.BGO.bg", SysUtils::IntToStr(BackgroundIndex)})));
        }
        StarField->BackgroundScale = pas::real_divide(Diameter * 3, 2.0E+3L);
    }

    void TfStarMap::SaveSpaceImageState(GI_SpaceImg::TSpaceImgGI* SpaceImage) {
        std::int32_t Index{};
        GI_SpaceImg::PSpaceImageGI Image{};
        aGalaxy::Galaxy->SpaceBackgroundEntries.set_length(SpaceImage->ImageCount);
        std::int32_t SavedCount = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, SpaceImage->ImageCount - 1); cpp_range.next(Index); ) {
            Image = SpaceImage->GetImage(Index);
            if (Image->TemplateIndex >= 100) {
                aGalaxy::Galaxy->SpaceBackgroundEntries[SavedCount].ImageIndex = Image->SavedTemplateIndex;
                pas::store_unaligned<EC_Struct::TVector3D>(&aGalaxy::Galaxy->SpaceBackgroundEntries[SavedCount].OrbitCenter, Image->OrbitCenter);
                pas::store_unaligned<EC_Struct::TVector3D>(&aGalaxy::Galaxy->SpaceBackgroundEntries[SavedCount].Position, EC_Struct::MakeVector3D(Image->X, Image->Y, Image->Depth));
                pas::store_unaligned<EC_Struct::TVector3D>(&aGalaxy::Galaxy->SpaceBackgroundEntries[SavedCount].Unknown38, Image->Unknown38);
                aGalaxy::Galaxy->SpaceBackgroundEntries[SavedCount].OrbitStepDegrees = Image->OrbitStepDegrees;
                aGalaxy::Galaxy->SpaceBackgroundEntries[SavedCount].FrameIndex = Image->FrameIndex;
                ++SavedCount;
            }
        }
        aGalaxy::Galaxy->SpaceBackgroundEntries.set_length(SavedCount);
    }

    void TfStarMap::SaveSpaceBackground() {
        TfStarMap::SaveSpaceImageState(pas::checked_cast<GI_SpaceImg::TSpaceImgGI*>(GetByName(u"SpaceImg"sv)));
    }

    void TfStarMap::DeferredEndTurn(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (DeferredEndTurnTimer != nullptr) {
            CancelCallbackTimer(DeferredEndTurnTimer);
            DeferredEndTurnTimer = nullptr;
        }
        EndTurnClicked(nullptr);
    }

    void TfStarMap::EndTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::uint32_t WaitResult{};
        pas::Array<WindowsImports::THandle, 0, 1> Events{};
        void* EventList{};
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(Globals::ShipScreen->ReopenRequested ^ 1)) {
            if (Globals::TrailingFilmEffects != nullptr) {
                Globals::TrailingFilmEffects->RemoveLinkedWeaponEffects();
            }
            aGalaxy::Galaxy->CheckIntegrityChecksum(3);
            if (static_cast<std::uint8_t>(aCalc::IsTurnCalculationRunningUI() ^ 1) && aCalc::TurnCalculationPhase != ThreadCalc::tcpGalaxyRunning && aCalc::TurnCalculationPhase != ThreadCalc::tcpPlayerStarRunning) {
                if (aPlayer::GetPlayer()->CalculateSpeed() == 0) {
                    aPlayer::GetPlayer()->OrderNone(false);
                }
                if (aRanger::PendingPlayerFollowTarget != nullptr) {
                    aRanger::TRanger_ProcessPendingPlayerFollowTargeting(aPlayer::GetPlayer());
                }
                Globals::PreviousFilmActivity = 0u;
                MainPanel->TryAutoTurnSave();
                GR_Main::SoundManager->PlaySound(u"Sound.Turn"_wref.get());
                Globals::PruneExpiredPersistentPlayerMessages();
                ClearPathOverlay(true);
                ClearPathOverlay(false);
                StopOrderMode();
                BreakOnNextFilm = GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT);
                aRanger::PlayerAutomaticControl = false;
                Globals::FilmCameraFollow = GlobalsV::ViewFollowShip;
                if (!Globals::PlayerStarDayPrepared) {
                    GR_Main::AppendLogLineThreadSafe("Not calc NextDay header"_a);
                }
                aPlayer::GetPlayer()->virtual_TShip_NextDay();
                aCalc::QueuePlayerStarTurnCalculation();
                Events[0] = Globals::TurnCalculationThread->IdleEvent;
                Events[1] = Globals::TalkRequestEvent;
                EventList = &Events;
                WaitResult = WindowsSdk::WaitForMultipleObjects(2u, static_cast<WindowsSdk::PWOHandleArray>(EventList), 0, WindowsSdk::INFINITE);
                if (!GR_Main::ExitScreenLoop) {
                    if (Globals::TurnCalculationThread->IdleEvent == 0 || WaitResult == WindowsSdk::WAIT_OBJECT_0) {
                        RebuildPartnerButtons();
                        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1)) {
                            aCalc::QueueGalaxyTurnCalculation();
                        }
                        StartTurnFilm();
                    } else if (WaitResult == WindowsSdk::WAIT_FAILED) {
                        pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error GetLastError()=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
                    } else if (WaitResult == WindowsSdk::WAIT_OBJECT_0 + 1) {
                        Globals::ScriptDialogIndex = -1;
                        RunTalkDialogs();
                        WaitForTurnOrTalk();
                    }
                    pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Break"sv))->SetHovered(false);
                    GR_Main::PostMouseMoveMessage();
                }
            }
        }
    }

    void TfStarMap::MapKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (MainPanel->NavigationLocked || Globals::ShipScreen->ReopenRequested) {
            return;
        }
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && Key == WindowsSdk::VK_ADD) {
            GR_Main::MusicManager->RequestFadeOut();
            SelectMusic();
            return;
        }
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Mode == smmOrders && static_cast<std::uint8_t>(aCalc::IsTurnCalculationRunningUI() ^ 1)) {
                if (Key == WindowsSdk::VK_F2) {
                    if (aGalaxy::Galaxy->IronWill) {
                        GI_MessageBox::ShowMessageBoxGI(this, aConst::LocalizedColorText(u"FormGameSet2.IronWillText"_wref.get()), GI_MessageBox::mbgCancel | GI_MessageBox::mbgUnused04, 0, 0, 0);
                        GR_Main::FullFrameRedrawRequested = true;
                        DrawFrame();
                        return;
                    }
                    if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
                        return;
                    }
                    GR_Main::CaptureSavePreview();
                    aGalaxy::Galaxy->CheckIntegrityChecksum(91);
                    fGalaxy2::CaptureGalaxyPreview(this);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(92);
                    ResumeMode = smrOrders;
                    GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
                    Globals::SaveManagerMode = fSaveManager::smmSave;
                    GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
                    RequestClose(1);
                } else if (Key == WindowsSdk::VK_F3) {
                    ResumeMode = smrOrders;
                    GlobalsV::SaveManagerReturnScreenId = GlobalsV::FormToId(this);
                    Globals::SaveManagerMode = fSaveManager::smmLoad;
                    GlobalsV::RequestedScreenId = GlobalsV::screenSaveManager;
                    RequestClose(1);
                } else if (Key == WindowsSdk::VK_F5) {
                    if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
                        return;
                    }
                    MainPanel->QuickSave();
                } else if (Key == WindowsSdk::VK_F6) {
                    MainPanel->QuickLoad(3);
                } else if (Key == WindowsSdk::VK_F7) {
                    MainPanel->QuickLoad(2);
                } else if (Key == WindowsSdk::VK_F8) {
                    MainPanel->QuickLoad(1);
                } else if (Key == WindowsSdk::VK_NUMPAD0) {
                    aGalaxy::Galaxy->CheckIntegrityChecksum(55);
                    aPlayer::GetPlayer()->TogglePickupTargets(false);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(56);
                    RebuildTargetMarkers();
                } else if (Key == WindowsSdk::VK_DECIMAL) {
                    aGalaxy::Galaxy->CheckIntegrityChecksum(55);
                    aPlayer::GetPlayer()->TogglePickupTargets(true);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(56);
                    RebuildTargetMarkers();
                } else if (Key == WindowsSdk::VK_INSERT) {
                    if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
                        return;
                    }
                    SaveVisiblePriceSnapshots();
                } else if (Key == WindowsSdk::VK_F11) {
                    if (!MainPanel->RemoveDismissibleMessages(u"GOODS"_w)) {
                        MainPanel->RemoveDismissibleMessages(pas::WideString());
                    }
                } else if (Key == WindowsSdk::VK_F1) {
                    MainPanel->JournalClicked(nullptr);
                }
            }
            if (Key == WindowsSdk::VK_LEFT) {
                ScrollLeftHeld = true;
                if (Mode == smmOrders) {
                    ShowObjectInfo(nullptr);
                }
            } else if (Key == WindowsSdk::VK_RIGHT) {
                ScrollRightHeld = true;
                if (Mode == smmOrders) {
                    ShowObjectInfo(nullptr);
                }
            } else if (Key == WindowsSdk::VK_UP) {
                ScrollUpHeld = true;
                if (Mode == smmOrders) {
                    ShowObjectInfo(nullptr);
                }
            } else if (Key == WindowsSdk::VK_DOWN) {
                ScrollDownHeld = true;
                if (Mode == smmOrders) {
                    ShowObjectInfo(nullptr);
                }
            } else if (Key == 'C') {
                if (Mode == smmOrders) {
                    CenterShipClicked(nullptr);
                } else {
                    CenterFilmShipClicked(nullptr);
                }
            } else if (Key == 'X') {
                if (Mode == smmOrders) {
                    CenterOnDominator(1);
                }
            } else if (Key == 'Z') {
                if (Mode == smmOrders) {
                    CenterOnDominator(2);
                }
            } else if (Mode == smmOrders && Key == WindowsSdk::VK_SPACE) {
                if (GetByName(u"PM_EndTurn"sv)->Active) {
                    EndTurnClicked(nullptr);
                }
            } else if (Mode == smmTurnFilm && Key == WindowsSdk::VK_SPACE) {
                if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Break"sv))->Disabled) {
                    BreakTurnClicked(nullptr);
                }
            }
        }
    }

    void TfStarMap::MapKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (Mode == smmOrders) {
            DisplayedObject = nullptr;
        }
        if (Key == WindowsSdk::VK_LEFT) {
            ScrollLeftHeld = false;
            ShowObjectInfo(FindObjectAtCursor());
        } else if (Key == WindowsSdk::VK_RIGHT) {
            ScrollRightHeld = false;
            ShowObjectInfo(FindObjectAtCursor());
        } else if (Key == WindowsSdk::VK_UP) {
            ScrollUpHeld = false;
            ShowObjectInfo(FindObjectAtCursor());
        } else if (Key == WindowsSdk::VK_DOWN) {
            ScrollDownHeld = false;
            ShowObjectInfo(FindObjectAtCursor());
        }
    }

    void TfStarMap::OpenFilmHistoryClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Mode == smmOrders && static_cast<std::uint8_t>(aCalc::IsTurnCalculationRunningUI() ^ 1) && Globals::FilmHistory->GetCount() > 0) {
            GlobalsV::RequestedScreenId = GlobalsV::screenFilm;
            RequestClose(1);
        }
    }

    void TfStarMap::RefreshScoreModsLabel() {
        pas::WideString Text{};
        {
            GI_Label::TLabelGI* Mods = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Mods"sv));
            if (aGalaxy::Galaxy->HasVisibleScoreModFlags()) {
                Text = EC_Str::DecodeTextW(aGalaxy::Galaxy->FinalizationNameEncoded);
                if (Text.length() == 0 && GR_Main::CCInterface->GetEditableStateApplied()) {
                    Text = GR_Main::LookupLocalizedTextByKey(u"Cheat.Warning"_wref.get());
                } else if (Text.length() > 0 && aGalaxy::Galaxy->GetCheatPoints() != 0) {
                    Text = pas::concat_wide({Text, u" + ", GR_Main::LookupLocalizedTextByKey(u"Cheat.Warning"_wref.get())});
                }
                if (aGalaxy::Galaxy->DominatorModLevel > 0 && Text.length() > 0) {
                    Text = pas::concat_wide({Text, u" + "});
                }
                if (aGalaxy::Galaxy->DominatorModLevel == 1) {
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_DomikHorrible"_wref.get()), Text});
                } else if (aGalaxy::Galaxy->DominatorModLevel == 2) {
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_DomikNightmare"_wref.get()), Text});
                } else if (aGalaxy::Galaxy->DominatorModLevel == 3) {
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_DomikHellish"_wref.get()), Text});
                }
                if (aGalaxy::Galaxy->TechnicModEnabled == 1) {
                    if (Text.length() > 0) {
                        Text = pas::concat_wide({Text, u" + "});
                    }
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_Technic"_wref.get()), Text});
                }
                if (aGalaxy::Galaxy->AmmoModEnabled == 1) {
                    if (Text.length() > 0) {
                        Text = pas::concat_wide({Text, u" + "});
                    }
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_Ammo"_wref.get()), Text});
                }
                if (aGalaxy::Galaxy->GodModEnabled == 1) {
                    if (Text.length() > 0) {
                        Text = pas::concat_wide({Text, u" + "});
                    }
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_God"_wref.get()), Text});
                }
                if (aGalaxy::Galaxy->UltraScanModEnabled == 1) {
                    if (Text.length() > 0) {
                        Text = pas::concat_wide({Text, u" + "});
                    }
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_Ultrascan"_wref.get()), Text});
                }
                if (aGalaxy::Galaxy->StasisModEnabled == 1) {
                    if (Text.length() > 0) {
                        Text = pas::concat_wide({Text, u" + "});
                    }
                    Text = pas::concat_wide_reverse({GR_Main::LookupLocalizedTextByKey(u"Cheat.Mod_Stasis"_wref.get()), Text});
                }
                Mods->SetText(Text);
                Mods->SetActive(true);
            } else if (aGalaxy::Galaxy->GetCheatPoints() != 0) {
                Mods->SetText(GR_Main::LookupLocalizedTextByKey(u"Cheat.Warning"_wref.get()));
                Mods->SetActive(true);
            } else {
                Mods->SetText(u""_wref.get());
                Mods->SetActive(false);
            }
        }
    }

    // May rebuild the ship movement path and update its order destination.
    void TfStarMap::BuildShipPathOverlay(aShip::TShip* Ship, std::uint8_t DelayEndImage, pas::WideString InitialImagePath) {
        EC_Struct::TPointF Point{};
        EC_Struct::TPointF TargetPosition{};
        WindowsSdk::TPoint ImageSize{};
        aPath::PSPathNode Cursor{};
        aPath::PSPathNode Node{};
        aPath::PSPathNode LastNode{};
        GI_GAI::TgaiGI* EndImage{};
        aShip::TShip* TargetShip{};
        aShip::TShip* OtherShip{};
        float Distance{};
        float Angle{};
        float AngleOffset{};
        float Radius{};
        float PathLength{};
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t LandingTurns{};
        GI_Label::TLabelGI* LabelGI{};
        WindowsSdk::TPoint EndPosition{};
        GI_MultiImage::TMultiImageGI* PathImages{};
        GI_MultiImage::TMultiImageUnitGI* UnitImage{};
        EC_Struct::PPointF Positions{};
        WindowsSdk::PSingle WritePosition{};
        if (Ship == aPlayer::GetPlayer() && PlayerPathTimer != nullptr) {
            CancelCallbackTimer(PlayerPathTimer);
            PlayerPathTimer = nullptr;
        }
        GI_MessageLoop::TObjectGI* Owner = MapControls;
        LandingTurns = -1;
        if (Ship == aKling::TerronShip) {
            return;
        }
        if (Ship->Order == aShip::soFollowShip || Ship == aPlayer::GetPlayer() && aRanger::PendingPlayerFollowTarget != nullptr) {
            if (Ship == aPlayer::GetPlayer() && aRanger::PendingPlayerFollowTarget != nullptr) {
                TargetShip = aRanger::PendingPlayerFollowTarget;
            } else {
                TargetShip = pas::checked_cast<aShip::TShip*>(Ship->OrderTarget);
            }
            if (TargetShip->IsOnPlanet()) {
                TargetPosition = TargetShip->CurrentPlanet->GetPosition();
            } else {
                TargetPosition = TargetShip->Position;
            }
            Distance = aMyFunction::PointDistance(Ship->Position, TargetShip->Position);
            AngleOffset = 0.0f;
            Count = 0;
            while (Count < 4) {
                ++Count;
                Radius = pas::real_divide(TargetShip->Graphic->Size.X, 2.0L) + 2.0E+1L;
                if (Distance < 1.0L) {
                    Angle = static_cast<long double>(aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 259, Ship->Seed * TargetShip->Seed))) + AngleOffset;
                    Point.X = TargetPosition.X + System::Sin(Angle) * Radius;
                    Point.Y = TargetPosition.Y - System::Cos(Angle) * Radius;
                } else {
                    Angle = Math::ArcTan2(static_cast<long double>(Ship->Position.X) - TargetPosition.X, -(static_cast<long double>(Ship->Position.Y) - TargetPosition.Y)) + AngleOffset;
                    Point.X = TargetPosition.X + System::Sin(Angle) * Radius;
                    Point.Y = TargetPosition.Y - System::Cos(Angle) * Radius;
                }
                Index = 0;
                while (Index < pas::list_count(Ship->CurrentStar->Ships)) {
                    OtherShip = pas::list_at<aShip::TShip>(Ship->CurrentStar->Ships, Index);
                    if (OtherShip->InNormalSpace() && aMyFunction::PointDistanceSquared(Point, OtherShip->Position) < 9.0E+2L) {
                        break;
                    }
                    ++Index;
                }
                AngleOffset = AngleOffset + 1.5707963L;
                if (Index >= pas::list_count(Ship->CurrentStar->Ships)) {
                    break;
                }
            }
            aGalaxy::Galaxy->CheckIntegrityChecksum(13);
            Ship->OrderDestination = Point;
            Ship->ClearMovementPath();
            Ship->BuildFullPathTo(Ship->OrderDestination);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(14);
        } else if (Ship->Order == aShip::soLand) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(15);
            Ship->ClearMovementPath();
            if (Ship == aPlayer::GetPlayer() && pas::class_cast_if<aPlanet::TPlanet*>(Ship->OrderTarget) != nullptr) {
                Ship->BuildPlanetLandingPath();
                LandingTurns = Ship->GetMovementPathTurnCount();
                Ship->BuildFullPathTo(EC_Struct::AddPointsF(pas::checked_cast<aPlanet::TPlanet*>(Ship->OrderTarget)->GetPosition(), Ship->OrderDestination));
            } else if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Ship->OrderTarget)) {
                Ship->BuildFullPathTo(EC_Struct::AddPointsF(ship->Position, Ship->OrderDestination));
            } else {
                Ship->BuildFullPathTo(EC_Struct::AddPointsF(pas::checked_cast<aPlanet::TPlanet*>(Ship->OrderTarget)->GetPosition(), Ship->OrderDestination));
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(16);
        } else if (Ship->Order == aShip::soJumpHole) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(17);
            Ship->ClearMovementPath();
            Ship->BuildFullPathTo(Ship->OrderDestination);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(18);
        } else if (Ship->Order == aShip::soJump) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(19);
            Ship->ClearMovementPath();
            Ship->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20);
        } else if (Ship != aPlayer::GetPlayer() && Ship->Order == aShip::soMove) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(21);
            Ship->ClearMovementPath();
            Ship->BuildFullPathTo(Ship->OrderDestination);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(22);
        }
        if (Ship->MovementPath->ActiveHead != nullptr) {
            if (Ship == aPlayer::GetPlayer()) {
                PathImages = pas::checked_cast<GI_MultiImage::TMultiImageGI*>(GetByName(u"PlayerPath"sv));
            } else {
                PathImages = pas::checked_cast<GI_MultiImage::TMultiImageGI*>(GetByName(u"ShipPath"sv));
            }
            if (pas::list_count(PathImages->Images) < 1) {
                PathImages->AddImage(u"Bm.PI.Path1"_w);
                PathImages->AddImage(u"Bm.PI.Path2"_w);
                PathImages->AddImage(u"Bm.PI.Path3"_w);
                PathImages->AddImage(u"Bm.PI.Path4"_w);
            }
            PathImages->ClearUnits();
            PathLength = 0.0f;
            Node = Ship->MovementPath->ActiveHead;
            while (Node != nullptr) {
                LastNode = (static_cast<void>(Ship->MovementPath), aPath::TSPath::GetFollowingNode(Node, 198));
                if (LastNode == nullptr) {
                    LastNode = Ship->MovementPath->ActiveTail;
                }
                Point = EC_Struct::MakePointF(1.0E+10f, 1.0E+10f);
                Cursor = LastNode;
                while (true) {
                    if (aMyFunction::PointDistanceSquared(Point, Cursor->Position) > 225.0L) {
                        if (Point.X != 1.0E+10L && Ship == aPlayer::GetPlayer()) {
                            PathLength = static_cast<long double>(PathLength) + aMyFunction::PointDistance(Point, Cursor->Position);
                        }
                        Point = Cursor->Position;
                        if (Cursor != Ship->MovementPath->ActiveTail) {
                            UnitImage = PathImages->AddUnit();
                            PathImages->SetUnitPosition(UnitImage, EC_Struct::TruncatePointF(Point));
                            if (Cursor != LastNode) {
                                if (Node == Ship->MovementPath->ActiveHead) {
                                    UnitImage->ImageIndex = 0;
                                } else {
                                    UnitImage->ImageIndex = 1;
                                }
                            } else if (Node == Ship->MovementPath->ActiveHead) {
                                UnitImage->ImageIndex = 2;
                            } else {
                                UnitImage->ImageIndex = 2;
                            }
                        }
                    }
                    if (Cursor == Node) {
                        break;
                    }
                    Cursor = Cursor->Prev;
                }
                Node = LastNode->Next;
            }
            if (Ship->Order == aShip::soLand && !(Ship == aPlayer::GetPlayer() && aRanger::PendingPlayerFollowTarget != nullptr)) {
                if (aShip::TShip* ship_2 = pas::class_cast_if<aShip::TShip*>(Ship->OrderTarget)) {
                    EndPosition = EC_Struct::TruncatePointF(EC_Struct::AddPointsF(ship_2->Position, Ship->OrderDestination));
                } else {
                    EndPosition = EC_Struct::TruncatePointF(EC_Struct::AddPointsF(pas::checked_cast<aPlanet::TPlanet*>(Ship->OrderTarget)->GetPosition(), Ship->OrderDestination));
                }
            } else {
                EndPosition = EC_Struct::TruncatePointF(Ship->OrderDestination);
            }
            EndImage = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
            if (!DelayEndImage) {
                if (aRanger::PendingPlayerFollowTarget != nullptr && Ship == aPlayer::GetPlayer()) {
                    EndImage->SetImagePath(u"Bm.PI.PathEndAutoBattle"_wref.get());
                } else if (Ship->Order == aShip::soFollowShip) {
                    if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Ship); kling != nullptr && kling->ShouldKamikaze()) {
                        EndImage->SetImagePath(u"Bm.PI.PathEndKamikaze"_wref.get());
                    } else if (Ship->GetFollowMode() == aShip::fmKamikaze) {
                        EndImage->SetImagePath(u"Bm.PI.PathEndKamikaze"_wref.get());
                    } else if (Ship->GetFollowMode() == aShip::fmFollowNear) {
                        EndImage->SetImagePath(u"Bm.PI.PathEndFollowNear"_wref.get());
                    } else if (Ship->GetFollowMode() == aShip::fmMinWeaponRange) {
                        EndImage->SetImagePath(u"Bm.PI.PathEndFollowMin"_wref.get());
                    } else {
                        EndImage->SetImagePath(u"Bm.PI.PathEndFollowMax"_wref.get());
                    }
                } else if (Ship->Order == aShip::soLand) {
                    EndImage->SetImagePath(u"Bm.PI.PathEndLanding"_wref.get());
                } else if (Ship->Order == aShip::soJumpHole) {
                    EndImage->SetImagePath(u"Bm.PI.PathEndJumpHole"_wref.get());
                } else {
                    EndImage->SetImagePath(u"Bm.PI.PathEndMove"_wref.get());
                }
            } else if (aRanger::PendingPlayerFollowTarget != nullptr && Ship == aPlayer::GetPlayer()) {
                EndImage->SetImagePath(InitialImagePath);
                EndImage->HelpText = u"Bm.PI.PathEndAutoBattle"_w;
                PlayerPathTimer = ScheduleCallbackTimer(WindowsSdk::GetDoubleClickTime() + 50, 999, pas::bind_method<&TfStarMap::UpdatePathEndImage>(this), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(EndImage)));
            } else if (Ship->Order == aShip::soFollowShip) {
                if (Ship->GetFollowMode() == aShip::fmFollowNear) {
                    EndImage->HelpText = u"Bm.PI.PathEndFollowNear"_w;
                } else if (Ship->GetFollowMode() == aShip::fmMinWeaponRange) {
                    EndImage->HelpText = u"Bm.PI.PathEndFollowMin"_w;
                } else {
                    EndImage->HelpText = u"Bm.PI.PathEndFollowMax"_w;
                }
                EndImage->SetImagePath(InitialImagePath);
                PlayerPathTimer = ScheduleCallbackTimer(WindowsSdk::GetDoubleClickTime() + 50, 999, pas::bind_method<&TfStarMap::UpdatePathEndImage>(this), static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(EndImage)));
            } else if (Ship->Order == aShip::soLand) {
                EndImage->SetImagePath(u"Bm.PI.PathEndLanding"_wref.get());
            } else if (Ship->Order == aShip::soJumpHole) {
                EndImage->SetImagePath(u"Bm.PI.PathEndJumpHole"_wref.get());
            } else {
                EndImage->SetImagePath(u"Bm.PI.PathEndMove"_wref.get());
            }
            EndImage->SequenceIndex = 0;
            EndImage->UpdateAutoGeometry();
            if (Ship == aPlayer::GetPlayer()) {
                EndImage->SetDepth(GlobalsV::UnitPathEndDepth);
            } else {
                EndImage->SetDepth(GlobalsV::ShipPathEndDepth);
            }
            EndImage->SetPosition(EndPosition);
            EndImage->SetPositionModeW(true);
            ImageSize = EndImage->GetContentSize();
            EndImage->SetOrigin(EC_Struct::HalfPoint(ImageSize));
            EndImage->SetSize(ImageSize);
            EndImage->RestartPlayback();
            LabelGI = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
            if (Ship == aPlayer::GetPlayer()) {
                LabelGI->SetDepth(GlobalsV::UnitPathEndDepth);
            } else {
                LabelGI->SetDepth(GlobalsV::ShipPathEndDepth);
            }
            LabelGI->SetFontName(GlobalsV::NormalFontName);
            LabelGI->SetSize(ClassesImports::Point(120, 20));
            LabelGI->SetTextAlignX(GI_Main::taxLeft);
            LabelGI->SetTextAlignY(GI_Main::tayCenterEx);
            if (LandingTurns < 0) {
                if (GlobalsV::ViewPathLength && Ship == aPlayer::GetPlayer() && Ship->Order == aShip::soMove) {
                    LabelGI->SetText(static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Ship->GetMovementPathTurnCount()), " (", SysUtils::Int64ToStr(System::Round(PathLength)), ")"})));
                } else {
                    LabelGI->SetText(pas::wide_int_to_str(Ship->GetMovementPathTurnCount()));
                }
            } else {
                LabelGI->SetText(pas::wide_int_to_str(LandingTurns));
            }
            LabelGI->SetPositionModeW(true);
            LabelGI->SetPosition(EC_Struct::AddPoints(EndPosition, ClassesImports::Point(20, -32)));
            if (Ship != aPlayer::GetPlayer() || aPlayer::GetPlayer() == CursorObject) {
                Count = Ship->MovementPath->NodeCount;
                Positions = static_cast<EC_Struct::PPointF>(EC_Mem::AllocEC(Count * static_cast<std::int32_t>(sizeof(EC_Struct::TPointF))));
                WritePosition = static_cast<WindowsSdk::PSingle>(static_cast<void*>(Positions));
                Cursor = Ship->MovementPath->ActiveHead;
                while (Cursor != nullptr) {
                    pas::store_unaligned<float>(WritePosition, Cursor->Position.X);
                    WritePosition = static_cast<WindowsSdk::PSingle>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(WritePosition) + static_cast<std::int32_t>(sizeof(float))));
                    pas::store_unaligned<float>(WritePosition, Cursor->Position.Y);
                    WritePosition = static_cast<WindowsSdk::PSingle>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(WritePosition) + static_cast<std::int32_t>(sizeof(float))));
                    Cursor = Cursor->Next;
                }
                Globals::SpaceProcess->Space->SetPath(Positions, Count);
                Globals::SpaceProcess->Space->DrawMinimap();
                EC_Mem::FreeEC(Positions);
                MinimapPathKind = smpShip;
            }
        }
    }

    void TfStarMap::UpdatePathEndImage(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (PlayerPathTimer != nullptr) {
            CancelCallbackTimer(PlayerPathTimer);
            PlayerPathTimer = nullptr;
        }
        reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->SetImagePath(reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->HelpText);
        reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->SequenceIndex = 0;
        reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->UpdateAutoGeometry();
        reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->SetPositionModeW(true);
        {
            WindowsSdk::TPoint halfPoint = EC_Struct::HalfPoint(reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->GetContentSize());
            GI_MessageLoop::TObjectGI* userData = reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)));
            userData->SetOrigin(halfPoint);
        }
        {
            WindowsSdk::TPoint contentSize = reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->GetContentSize();
            GI_GAI::TgaiGI* userData_2 = reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)));
            userData_2->SetSize(contentSize);
        }
        reinterpret_cast<GI_GAI::TgaiGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(UserData)))->RestartPlayback();
    }

    void TfStarMap::ClearPathOverlay(std::uint8_t PlayerPath) {
        GI_MessageLoop::TObjectGI* NextControl{};
        GI_MessageLoop::TObjectGI* Control{};
        if (PlayerPath && PlayerPathTimer != nullptr) {
            CancelCallbackTimer(PlayerPathTimer);
            PlayerPathTimer = nullptr;
        }
        {
            GI_Panel::TPanelGI* cpp_with = MapControls;
            NextControl = cpp_with->FirstChild;
        }
        while (NextControl != nullptr) {
            Control = NextControl;
            NextControl = NextControl->NextSibling;
            if (PlayerPath && Control->Depth == GlobalsV::UnitPathEndDepth || static_cast<std::uint8_t>(PlayerPath ^ 1) && Control->Depth == GlobalsV::ShipPathEndDepth) {
                Control->SetActive(false);
                pas::free(Control);
            }
        }
        if (PlayerPath) {
            GI_MultiImage::TMultiImageGI* PlayerPath_2 = pas::checked_cast<GI_MultiImage::TMultiImageGI*>(GetByName(u"PlayerPath"sv));
            PlayerPath_2->ClearUnits();
        } else {
            GI_MultiImage::TMultiImageGI* ShipPath = pas::checked_cast<GI_MultiImage::TMultiImageGI*>(GetByName(u"ShipPath"sv));
            ShipPath->ClearUnits();
        }
        if (MinimapPathKind == smpShip) {
            Globals::SpaceProcess->Space->ClearPath();
            Globals::SpaceProcess->Space->DrawMinimap();
            MinimapPathKind = smpNone;
        }
    }

    void TfStarMap::ShowAsteroidPath(aAsteroid::TAsteroid* Asteroid) {
        float NextX{};
        float NextY{};
        std::int32_t I{};
        GI_Image::TImageGI* Image{};
        if (PathAsteroid == Asteroid) {
            return;
        }
        ClearAsteroidPath();
        PathAsteroid = Asteroid;
        std::int32_t Count = 800;
        void* Cursor = EC_Mem::AllocEC(Count * static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        void* Positions = Cursor;
        Asteroid->WritePredictedPositions(static_cast<EC_Struct::PPointF>(Cursor), Count);
        float X = EC_Mem::ReadSingleEC(Cursor);
        Cursor = EC_Mem::AddPointerOffset(Cursor, static_cast<std::int32_t>(sizeof(float)));
        float Y = EC_Mem::ReadSingleEC(Cursor);
        Cursor = EC_Mem::AddPointerOffset(Cursor, static_cast<std::int32_t>(sizeof(float)));
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count - 1); cpp_range.next(I); ) {
            NextX = EC_Mem::ReadSingleEC(Cursor);
            Cursor = EC_Mem::AddPointerOffset(Cursor, static_cast<std::int32_t>(sizeof(float)));
            NextY = EC_Mem::ReadSingleEC(Cursor);
            Cursor = EC_Mem::AddPointerOffset(Cursor, static_cast<std::int32_t>(sizeof(float)));
            if (pas::sqr(static_cast<long double>(X) - NextX) + pas::sqr(static_cast<long double>(Y) - NextY) > 144.0L) {
                X = NextX;
                Y = NextY;
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapControls);
                Image->SetDepth(GlobalsV::UnitPathDepth);
                {
                    std::int32_t round = System::Round(NextY);
                    std::int32_t round_2 = System::Round(NextX);
                    Image->SetPosition(ClassesImports::Point(round_2, round));
                }
                Image->SetPositionModeW(true);
                if (I < 200) {
                    Image->SetImagePath(u"GI,Bm.PI.Path1"_w);
                } else {
                    Image->SetImagePath(u"GI,Bm.PI.Path2"_w);
                }
                Image->SetSize(Image->GetContentSize());
                Image->SetOrigin(EC_Struct::HalfPoint(Image->ClientSize));
                Image->UserValue = 101;
            }
        }
        EC_Mem::FreeEC(Positions);
        Count = 2400;
        Cursor = EC_Mem::AllocEC(Count * static_cast<std::int32_t>(sizeof(EC_Struct::TPointF)));
        Asteroid->WritePredictedPositions(static_cast<EC_Struct::PPointF>(Cursor), Count);
        Globals::SpaceProcess->Space->SetPath(static_cast<EC_Struct::PPointF>(Cursor), Count);
        Globals::SpaceProcess->Space->DrawMinimap();
        EC_Mem::FreeEC(Cursor);
        MinimapPathKind = smpAsteroid;
    }

    void TfStarMap::ClearAsteroidPath() {
        GI_MessageLoop::TObjectGI* NextControl{};
        GI_MessageLoop::TObjectGI* Control{};
        if (PathAsteroid != nullptr) {
            PathAsteroid = nullptr;
            if (MinimapPathKind == smpAsteroid) {
                Globals::SpaceProcess->Space->ClearPath();
                Globals::SpaceProcess->Space->DrawMinimap();
                MinimapPathKind = smpNone;
            }
            NextControl = MapControls->FirstChild;
            while (NextControl != nullptr) {
                Control = NextControl;
                NextControl = NextControl->NextSibling;
                if (Control->UserValue == 101) {
                    Control->SetActive(false);
                    pas::free(Control);
                }
            }
        }
    }

    void TfStarMap::ClearPartnerButtons() {
        PartnerPanel->Parent->SetActive(false);
        SecondaryPartnerPanel->FreeOwnedChildren();
    }

    void TfStarMap::RebuildPartnerButtons() {
        std::int32_t StarIndex{};
        std::int32_t ShipIndex{};
        std::int32_t PlacedCount{};
        std::int32_t PartnerCount{};
        aGalaxy::TStar* Star{};
        aShip::TShip* Ship{};
        pas::WideString Portrait{};
        ClearPartnerButtons();
        if (aPlayer::GetPlayer() != nullptr) {
            PlacedCount = 0;
            PartnerCount = 0;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(StarIndex); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, StarIndex);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(ShipIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, ShipIndex);
                    if (Ship->PartnerShip == aPlayer::GetPlayer()) {
                        ++PartnerCount;
                    }
                }
            }
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_3.next(StarIndex); ) {
                Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, StarIndex);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_4.next(ShipIndex); ) {
                    Ship = pas::list_at<aShip::TShip>(Star->Ships, ShipIndex);
                    if (Ship->PartnerShip == aPlayer::GetPlayer()) {
                        GI_GraphButton::TGraphButtonGI* cpp_with = pas::construct_call<GI_GraphButton::TGraphButtonGI>(GI_GraphButton::TGraphButtonGI_Create, SecondaryPartnerPanel);
                        cpp_with->SetKind(GI_GraphButton::gbkDisable);
                        Portrait = pas::WideString();
                        if (Ship->Graphic != nullptr && pas::class_cast_if<SE_Ship2::TShip2SE*>(Ship->Graphic) != nullptr && static_cast<SE_Ship2::TShip2SE*>(Ship->Graphic)->PanelPartnerImage != u"") {
                            Portrait = static_cast<SE_Ship2::TShip2SE*>(Ship->Graphic)->PanelPartnerImage;
                        } else if (Ship->Graphic != nullptr && pas::class_cast_if<SE_Ruins::TRuinsSE*>(Ship->Graphic) != nullptr && static_cast<SE_Ruins::TRuinsSE*>(Ship->Graphic)->PanelPartnerImage != u"") {
                            Portrait = static_cast<SE_Ruins::TRuinsSE*>(Ship->Graphic)->PanelPartnerImage;
                        } else if (Ship->IsFemaleHumanPilot() && Ship->TypeId == aGalaxyStruct::stRanger && Ship->GetHull()->HullType == aGalaxyStruct::htSpecial && Ship->GetHull()->GetSpecialKindGraph() == u"J") {
                            Portrait = u"Female"_w;
                        } else if (Ship->TypeId == aGalaxyStruct::stRanger && Ship->UsesVeteranHumanRangerAppearance()) {
                            Portrait = u"PeopleO"_w;
                        } else if (aPirate::TPirate* pirate = pas::class_cast_if<aPirate::TPirate*>(Ship)) {
                            Portrait = pas::concat_wide({Portrait, u"P"});
                            if (Ship->OwnerId == aGalaxyStruct::oiPirate && pirate->PirateType != 0) {
                                Portrait = pas::concat_wide({aConst::OwnerInfo[aConst::RaceToOwner(Ship->PilotRace)].InternalName, Portrait, u"C"});
                            } else {
                                Portrait = pas::concat_wide({aConst::OwnerInfo[aConst::RaceToOwner(Ship->PilotRace)].InternalName, Portrait});
                            }
                        } else {
                            Portrait = aConst::OwnerInfo[aConst::RaceToOwner(Ship->PilotRace)].InternalName;
                        }
                        cpp_with->SetImageNormalPath(pas::concat_wide({u"GI,Bm.PanelSpace2.", GR_Main::GiResourceSuffix(), Portrait, u"N"}));
                        cpp_with->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.PanelSpace2.", GR_Main::GiResourceSuffix(), Portrait, u"A"}));
                        cpp_with->SetImageDownPath(pas::concat_wide({u"GI,Bm.PanelSpace2.", GR_Main::GiResourceSuffix(), Portrait, u"D"}));
                        cpp_with->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.PanelSpace2.", GR_Main::GiResourceSuffix(), Portrait, u"H"}));
                        cpp_with->EnterSound = u"Sound.ButtonEnter"_w;
                        cpp_with->LeaveSound = u"Sound.ButtonLeave"_w;
                        cpp_with->ClickSound = u"Sound.ButtonClick"_w;
                        cpp_with->UserValue = Ship->Id;
                        cpp_with->HitKind = GI_GraphButton::gbhRect;
                        cpp_with->SetSize(cpp_with->GetMaxStateImageSize());
                        if (PartnerCount <= 5) {
                            cpp_with->SetPosition(ClassesImports::Point(GR_Main::GiScalePixelsEx(23, 20) + PlacedCount * GR_Main::GiScalePixels(25) - cpp_with->ClientSize.X / 2, GR_Main::GiScalePixels(20) - cpp_with->ClientSize.Y / 2));
                        } else {
                            cpp_with->SetPosition(ClassesImports::Point(GR_Main::GiScalePixelsEx(23, 20) + PlacedCount * GR_Main::GiScalePixels(21) - 5 - cpp_with->ClientSize.X / 2, GR_Main::GiScalePixels(20) - cpp_with->ClientSize.Y / 2));
                        }
                        cpp_with->UpdateStateImagePlacement();
                        cpp_with->UpdateStateVisuals();
                        cpp_with->DownCallback = pas::bind_method<&TfStarMap::PartnerClicked>(this);
                        cpp_with->RightButtonDownCallback = pas::bind_method<&TfStarMap::PartnerRightButtonDown>(this);
                        cpp_with->MouseEnterCallback = pas::bind_method<&TfStarMap::PartnerMouseEnter>(this);
                        cpp_with->MouseLeaveCallback = pas::bind_method<&TfStarMap::PartnerMouseLeave>(this);
                        cpp_with->SetDisabled(Ship->InHyperspace || Ship->CurrentStar != aPlayer::GetPlayer()->CurrentStar);
                        cpp_with->HelpCallback = pas::bind_method<&fPanelMain::TfPanelMain::ShowControlHelp>(MainPanel);
                        cpp_with->HelpText = Ship->GetFullName(u" "_wref.get());
                        ++PlacedCount;
                        PartnerPanel->Parent->SetActive(PlacedCount > 0);
                        PartnerPanel->SetSize(ClassesImports::Point(std::min<std::int32_t>(GR_Main::GiScalePixelsEx(12, 10) + PlacedCount * GR_Main::GiScalePixels(25) + GR_Main::GiScalePixelsEx(22, 18), PartnerPanel->Parent->ClientSize.X - 10), PartnerPanel->ClientSize.Y));
                        PartnerPanel->SetPosition(ClassesImports::Point(PartnerPanel->Parent->ClientSize.X - PartnerPanel->ClientSize.X, 0));
                        {
                            GI_MessageLoop::TObjectGI* MapPartnerBG2 = GetByName(u"MapPartnerBG2"sv);
                            MapPartnerBG2->SetPosition(ClassesImports::Point(PartnerPanel->ClientSize.X - MapPartnerBG2->ClientSize.X, 0));
                        }
                        {
                            GI_MessageLoop::TObjectGI* MapPartnerDuty = GetByName(u"MapPartnerDuty"sv);
                            MapPartnerDuty->SetPosition(ClassesImports::Point(PartnerPanel->LocalPosition.X - 5, MapPartnerDuty->LocalPosition.Y));
                        }
                    }
                }
            }
        }
    }

    void TfStarMap::PartnerClicked(GI_MessageLoop::TObjectGI* Sender) {
        aShip::TShip* Ship{};
        aEFilm::TEFilmObj* FilmObject{};
        pas::Object* Instance{};
        if (Mode == smmTurnFilm) {
            FilmObject = Globals::SecondaryFilm->FindObjectById(u"Ship2"sv, Sender->UserValue);
            if (FilmObject == nullptr || FilmObject->SceneObject == nullptr) {
                return;
            }
            SetMapCenterManually(EC_Struct::TruncatePointF(FilmObject->SceneObject->Position));
        } else {
            Instance = static_cast<pas::Object*>(aGalaxy::Galaxy->IdToShip(Sender->UserValue, false));
            if (Instance == nullptr) {
                return;
            }
            Ship = pas::checked_cast<aShip::TShip*>(Instance);
            if (aPlayer::GetPlayer()->CurrentStar != Ship->CurrentStar || Ship->InHyperspace) {
                return;
            }
            if (Ship->InNormalSpace()) {
                SetMapCenterManually(EC_Struct::TruncatePointF(Ship->Position));
            } else if (Ship->IsOnPlanet()) {
                SetMapCenterManually(EC_Struct::TruncatePointF(Ship->CurrentPlanet->GetPosition()));
            } else if (Ship->IsDockedToShip()) {
                SetMapCenterManually(EC_Struct::TruncatePointF(Ship->DockedTo->Position));
            } else {
                return;
            }
        }
        {
            EC_Struct::TPointF pointToPointF = EC_Struct::PointToPointF(GetMapCenter());
            pas::WideString cpp_arg = pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"});
            AddMapAnimation(pointToPointF, std::move(cpp_arg), 0);
        }
        {
            EC_Struct::TPointF pointToPointF_2 = EC_Struct::PointToPointF(GetMapCenter());
            pas::WideString cpp_arg_2 = pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"});
            AddMapAnimation(pointToPointF_2, std::move(cpp_arg_2), 200);
        }
        {
            EC_Struct::TPointF pointToPointF_3 = EC_Struct::PointToPointF(GetMapCenter());
            pas::WideString cpp_arg_3 = pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"});
            AddMapAnimation(pointToPointF_3, std::move(cpp_arg_3), 400);
        }
    }

    void TfStarMap::PartnerRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aShip::TShip* Ship{};
        pas::Object* Obj{};
        if (Mode == smmOrders) {
            Obj = static_cast<pas::Object*>(aGalaxy::Galaxy->IdToShip(Sender->UserValue, false));
            if (Obj != nullptr) {
                Ship = pas::checked_cast<aShip::TShip*>(Obj);
                if (aPlayer::GetPlayer()->CurrentStar == Ship->CurrentStar && Ship->InNormalSpace()) {
                    pas::Extended cpp_left = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                    if (cpp_left <= aPlayer::GetPlayer()->GetRadarRange()) {
                        Globals::TalkShip = Ship;
                        Globals::TalkScripted = false;
                        Globals::ScriptDialogIndex = -1;
                        aGalaxy::Galaxy->CheckIntegrityChecksum(23);
                        RunTalkDialogs();
                        aGalaxy::Galaxy->PrimeIntegrityChecksum(24);
                        GI_Main::BreakUiMessage();
                    }
                }
            }
        }
    }

    void TfStarMap::PartnerMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        pas::Object* Obj{};
        pas::Object* Instance{};
        if (Mode == smmOrders) {
            Instance = static_cast<pas::Object*>(aGalaxy::Galaxy->IdToShip(Sender->UserValue, false));
            if (Instance != nullptr) {
                Obj = pas::checked_cast<aShip::TShip*>(Instance);
                ShowObjectInfo(Obj);
            }
        }
    }

    void TfStarMap::PartnerMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        ShowObjectInfo(nullptr);
    }

    void TfStarMap::AddMapAnimation(EC_Struct::TPointF Position, pas::WideString ImagePath, std::int32_t DelayMs) {
        GI_GAI::TgaiGI* Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MapControls);
        Animation->SetDepthByName(u"IAnim"_wref.get());
        Animation->SetPosition(EC_Struct::TruncatePointF(Position));
        Animation->SetPositionModeW(true);
        Animation->SetImagePath(ImagePath);
        Animation->SetSize(Animation->GetContentSize());
        Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
        Animation->UserValue = 102;
        Animation->SequenceIndex = 0;
        Animation->UpdateAutoGeometry();
        if (DelayMs >= 0) {
            Animation->SetFrameDelay(0, DelayMs);
        }
        Animation->CycleCompleteCallback = pas::bind_static_method<&TfStarMap::MapAnimationFinished>(this);
        Animation->RestartPlayback();
    }

    void TfStarMap::ClearMapAnimations() {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TObjectGI* NextControl = MapControls->FirstChild;
        while (NextControl != nullptr) {
            Control = NextControl;
            NextControl = NextControl->NextSibling;
            if (Control->UserValue == 102) {
                Control->SetActive(false);
                pas::free(Control);
            }
        }
    }

    void TfStarMap::MapAnimationFinished(GI_MessageLoop::TObjectGI* Sender) {
        Sender->SetActive(false);
        pas::free(Sender);
    }

    // Returns a borrowed game object or nil; updates HitObjectPosition and HitObjectSize.
    pas::Object* TfStarMap::FindObjectAtCursor() {
        std::int32_t Index{};
        aShip::TShip* Ship{};
        aItem::TItem* Item{};
        aPlanet::TPlanet* Planet{};
        aAsteroid::TAsteroid* Asteroid{};
        WindowsSdk::TPoint Point{};
        aGalaxy::THole* Hole{};
        aMissile::TMissile* Missile{};
        if (Mode != smmOrders) {
            return nullptr;
        }
        HitObjectPosition = ClassesImports::Point(0, 0);
        HitObjectSize = ClassesImports::Point(0, 0);
        if (MainPanel->BackgroundImage->HitTestPixel(GetCursorPoint())) {
            return nullptr;
        }
        Point = GetCursorPoint();
        if (IsMapPointBlocked(MapControls, Point)) {
            return nullptr;
        }
        Point = MapControls->ToLocalPoint(GetCursorPoint());
        std::int32_t Count = pas::list_count(aGalaxy::PlayerStar->Missiles);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Missile = pas::list_at<aMissile::TMissile>(aGalaxy::PlayerStar->Missiles, Index);
            if (Missile != nullptr && Missile->GetGraphObject() != nullptr && Missile->GetGraphObject()->HitTestCursor()) {
                {
                    std::int64_t cpp_left_2 = System::Round(Missile->GetGraphObject()->Position.Y);
                    std::int32_t cpp_arg = cpp_left_2 - GetMapCenter().Y;
                    std::int64_t cpp_left = System::Round(Missile->GetGraphObject()->Position.X);
                    std::int32_t cpp_arg_2 = cpp_left - GetMapCenter().X;
                    HitObjectPosition = ClassesImports::Point(cpp_arg_2, cpp_arg);
                }
                HitObjectSize = Missile->GetGraphObject()->Size;
                return Missile;
            }
        }
        Count = pas::list_count(aGalaxy::PlayerStar->Ships);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Ship = pas::list_at<aShip::TShip>(aGalaxy::PlayerStar->Ships, Index);
            if (!(pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr) && Ship != nullptr && Ship->Graphic != nullptr && Ship->Graphic->HitTestCursor()) {
                {
                    std::int64_t cpp_left_4 = System::Round(Ship->Graphic->Position.Y);
                    std::int32_t cpp_arg_3 = cpp_left_4 - GetMapCenter().Y;
                    std::int64_t cpp_left_3 = System::Round(Ship->Graphic->Position.X);
                    std::int32_t cpp_arg_4 = cpp_left_3 - GetMapCenter().X;
                    HitObjectPosition = ClassesImports::Point(cpp_arg_4, cpp_arg_3);
                }
                HitObjectSize = Ship->Graphic->Size;
                return Ship;
            }
        }
        Count = pas::list_count(aGalaxy::PlayerStar->Items);
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_3.next(Index); ) {
            Item = pas::list_at<aItem::TItem>(aGalaxy::PlayerStar->Items, Index);
            if (Item != nullptr && Item->GetGraphObject() != nullptr && Item->GetGraphObject()->HitTestCursor()) {
                {
                    std::int64_t cpp_left_6 = System::Round(Item->GetGraphObject()->Position.Y);
                    std::int32_t cpp_arg_5 = cpp_left_6 - GetMapCenter().Y;
                    std::int64_t cpp_left_5 = System::Round(Item->GetGraphObject()->Position.X);
                    std::int32_t cpp_arg_6 = cpp_left_5 - GetMapCenter().X;
                    HitObjectPosition = ClassesImports::Point(cpp_arg_6, cpp_arg_5);
                }
                HitObjectSize = ClassesImports::Point(38, 38);
                return Item;
            }
        }
        Count = pas::list_count(aGalaxy::PlayerStar->Ships);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(Index); ) {
            Ship = pas::list_at<aShip::TShip>(aGalaxy::PlayerStar->Ships, Index);
            if (pas::class_cast_if<aRuins::TRuins*>(Ship) != nullptr && Ship != nullptr && Ship->Graphic != nullptr && Ship->Graphic->HitTestCursor()) {
                {
                    std::int64_t cpp_left_8 = System::Round(Ship->Graphic->Position.Y);
                    std::int32_t cpp_arg_7 = cpp_left_8 - GetMapCenter().Y;
                    std::int64_t cpp_left_7 = System::Round(Ship->Graphic->Position.X);
                    std::int32_t cpp_arg_8 = cpp_left_7 - GetMapCenter().X;
                    HitObjectPosition = ClassesImports::Point(cpp_arg_8, cpp_arg_7);
                }
                HitObjectSize = Ship->Graphic->Size;
                return Ship;
            }
        }
        Count = pas::list_count(aGalaxy::PlayerStar->Asteroids);
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_5.next(Index); ) {
            Asteroid = pas::list_at<aAsteroid::TAsteroid>(aGalaxy::PlayerStar->Asteroids, Index);
            if (Asteroid != nullptr && Asteroid->GraphObject != nullptr && Asteroid->GraphObject->HitTestCursor()) {
                {
                    std::int64_t cpp_left_10 = System::Round(Asteroid->GraphObject->Position.Y);
                    std::int32_t cpp_arg_9 = cpp_left_10 - GetMapCenter().Y;
                    std::int64_t cpp_left_9 = System::Round(Asteroid->GraphObject->Position.X);
                    std::int32_t cpp_arg_10 = cpp_left_9 - GetMapCenter().X;
                    HitObjectPosition = ClassesImports::Point(cpp_arg_10, cpp_arg_9);
                }
                HitObjectSize = Asteroid->GraphObject->Size;
                return Asteroid;
            }
        }
        Count = pas::list_count(aGalaxy::PlayerStar->Planets);
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_6.next(Index); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(aGalaxy::PlayerStar->Planets, Index);
            if (Planet->Graphic->IsRuins) {
                if (Planet->Graphic->HitTestCursor()) {
                    {
                        std::int64_t cpp_left_12 = System::Round(Planet->Graphic->Position.Y);
                        std::int32_t cpp_arg_11 = cpp_left_12 - GetMapCenter().Y;
                        std::int64_t cpp_left_11 = System::Round(Planet->Graphic->Position.X);
                        std::int32_t cpp_arg_12 = cpp_left_11 - GetMapCenter().X;
                        HitObjectPosition = ClassesImports::Point(cpp_arg_12, cpp_arg_11);
                    }
                    HitObjectSize = Planet->Graphic->Size;
                    return Planet;
                }
            } else {
                pas::Extended cpp_left_13 = pas::sqr(static_cast<long double>(Point.X) - Planet->GetPosition().X);
                if (cpp_left_13 + pas::sqr(static_cast<long double>(Point.Y) - Planet->GetPosition().Y) < pas::sqr(Planet->GraphicRadius)) {
                    {
                        std::int64_t cpp_left_15 = System::Round(Planet->Graphic->Position.Y);
                        std::int32_t cpp_arg_13 = cpp_left_15 - GetMapCenter().Y;
                        std::int64_t cpp_left_14 = System::Round(Planet->Graphic->Position.X);
                        std::int32_t cpp_arg_14 = cpp_left_14 - GetMapCenter().X;
                        HitObjectPosition = ClassesImports::Point(cpp_arg_14, cpp_arg_13);
                    }
                    HitObjectSize = Planet->Graphic->Size;
                    return Planet;
                }
            }
        }
        if (Point.X * Point.X + Point.Y * Point.Y < pas::sqr(aGalaxy::PlayerStar->Radius)) {
            {
                std::int64_t cpp_left_17 = System::Round(aGalaxy::PlayerStar->Graphic->Position.Y);
                std::int32_t cpp_arg_15 = cpp_left_17 - GetMapCenter().Y;
                std::int64_t cpp_left_16 = System::Round(aGalaxy::PlayerStar->Graphic->Position.X);
                std::int32_t cpp_arg_16 = cpp_left_16 - GetMapCenter().X;
                HitObjectPosition = ClassesImports::Point(cpp_arg_16, cpp_arg_15);
            }
            HitObjectSize = ClassesImports::Point(300, 300);
            return aGalaxy::PlayerStar;
        }
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Holes) - 1); cpp_range_7.next(Index); ) {
            Hole = pas::list_at<aGalaxy::THole>(aGalaxy::Galaxy->Holes, Index);
            if ((Hole->Star1 == aGalaxy::PlayerStar || Hole->Star2 == aGalaxy::PlayerStar) && Hole->Graphic != nullptr) {
                if (static_cast<long double>(aMyFunction::PointDistanceSquared(Hole->Graphic->Position, EC_Struct::PointToPointF(Point))) < pas::sqr(reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->HitRadius)) {
                    {
                        std::int64_t cpp_left_19 = System::Round(Hole->Graphic->Position.Y);
                        std::int32_t cpp_arg_17 = cpp_left_19 - GetMapCenter().Y;
                        std::int64_t cpp_left_18 = System::Round(Hole->Graphic->Position.X);
                        std::int32_t cpp_arg_18 = cpp_left_18 - GetMapCenter().X;
                        HitObjectPosition = ClassesImports::Point(cpp_arg_18, cpp_arg_17);
                    }
                    HitObjectSize = ClassesImports::Point(120, 120);
                    return Hole;
                }
            }
        }
        return nullptr;
    }

    void TfStarMap::QueueInterfaceImages() {
        pas::List* Loads{};
        if (!Globals::CacheLoader->IsRunning()) {
            Loads = pas::make_object<pas::List>();
            fLoad::QueueHyperspaceLoadingAssets(Loads, RootUiObject);
            if (pas::list_count(Loads) > 0) {
                Globals::CacheLoader->SetPendingLoads(Loads, true);
            } else {
                pas::free(Loads);
            }
        }
    }

    void TfStarMap::ShowLargeHelp(const pas::WideString& Text) {
        LargeHelpText = Text;
        LargeHelpBuffer->SetActive(true);
        if (LargeHelpTimer != nullptr) {
            CancelCallbackTimer(LargeHelpTimer);
            LargeHelpTimer = nullptr;
        }
        LargeHelpTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfStarMap::AnimateLargeHelp>(this), 0);
        LargeHelpProgress = 0.0f;
        AnimateLargeHelp(nullptr, 0);
    }

    void TfStarMap::HideLargeHelp() {
        if (LargeHelpTimer != nullptr) {
            CancelCallbackTimer(LargeHelpTimer);
            LargeHelpTimer = nullptr;
        }
        LargeHelpBuffer->Invalidate();
        LargeHelpBuffer->SetActive(false);
    }

    void TfStarMap::AnimateLargeHelp(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        float Intensity{};
        std::uint32_t Color{};
        std::uint32_t Offset{};
        WindowsSdk::TRect cpp_with{};
        LargeHelpProgress = LargeHelpProgress + 0.01L;
        if (LargeHelpProgress >= 1.0L) {
            HideLargeHelp();
        } else {
            Intensity = 1.0L - (System::Cos(SystemImports::Pi * LargeHelpProgress * 2.0L * 2.0L) * 0.5L + 0.5L);
            cpp_with = GI_Label::MeasureLabelTextBounds(LargeHelpText, GlobalsV::NormalFontName);
            LargeHelpBuffer->SourceHasPerPixelAlpha = true;
            {
                std::uint32_t cpp_left_2 = 0xff000000u | static_cast<std::uint32_t>(System::Round(Intensity * 224.0L)) << 16;
                std::uint32_t cpp_left = cpp_left_2 | static_cast<std::uint32_t>(System::Round(Intensity * 225.0L)) << 8;
                Color = cpp_left | static_cast<std::uint32_t>(System::Round(Intensity * 183.0L));
            }
            GI_Label::RenderLabelTextToBuffer(LargeHelpBuffer->GraphBuf, cpp_with.Right - cpp_with.Left + 1, 0, 0, LargeHelpText, GlobalsV::NormalFontName, Color, 0u, 0u);
            {
                GI_GraphBuf::TGraphBufGI* cpp_with_2 = LargeHelpBuffer;
                Offset = pas::shr(cpp_with_2->GraphBuf->Width, 1) - cpp_with_2->ClientSize.X / 2;
                cpp_with_2->SetSize(ClassesImports::Point(cpp_with_2->GraphBuf->Width, cpp_with_2->GraphBuf->Height));
                cpp_with_2->SetPosition(ClassesImports::Point(cpp_with_2->LocalPosition.X - Offset, cpp_with_2->LocalPosition.Y));
                cpp_with_2->SetImageKindX(GI_Main::ikxCenter);
                cpp_with_2->SetImageKindY(GI_Main::ikyCenter);
                cpp_with_2->Invalidate();
            }
        }
    }

    void TfStarMap::RedrawMap() {
        UpdateRectsEnabled = true;
        MapControls->InvalidateChildren(false);
        CursorControl->Invalidate();
        UpdateRectsEnabled = false;
        InvalidateTransientControl();
    }

    void TfStarMap::DrawFrame() {
        GR_Rect::TRectGR* RectNode{};
        std::int32_t Stage = 0;
        try {
            if (Globals::MinimapFrameCounter % 16 == 0) {
                Stage = 1;
                if (Mode == smmTurnFilm) {
                    Globals::SpaceProcess->Space->DrawMinimap();
                }
            }
            ++Globals::MinimapFrameCounter;
            Stage = 2;
            RedrawMap();
            Stage = 3;
            StarField->UpdateBackgroundBounds();
            Stage = 4;
            if (GlobalsV::SkipSavedPixelRestore || GR_Main::FullFrameRedrawRequested) {
                UpdateRects->Clear();
                UpdateRectsEnabled = true;
                InvalidateViewport();
                UpdateRectsEnabled = false;
            }
            GR_Main::FullFrameRedrawRequested = false;
            Stage = 5;
            GI_MessageLoop::TMessageLoopGI_RestoreSavedPixels16(this);
            Stage = 6;
            RestoreSavedLines();
            Stage = 7;
            ErasePreviousFrame();
            Stage = 8;
            RectNode = UpdateRects->FirstRect;
            while (RectNode != nullptr) {
                StarField->DrawBackground(RectNode->Bounds);
                RectNode = RectNode->Next;
            }
            Stage = 9;
            if (!GlobalsV::HardwareRenderingEnabled) {
                PrepareFrameDraw();
            }
            Stage = 10;
            DrawQueuedControlRects();
            Stage = 11;
            if (static_cast<std::uint8_t>(Globals::ShipScreen->ReopenRequested ^ 1) && Globals::TalkScreen->ModalTransition == fTalk::tmtNone && static_cast<std::uint8_t>(Globals::GoodsShopScreen->ReopenRequested ^ 1)) {
                if (!GR_Main::BeginFramePresentation()) {
                    GlobalsV::RequestedScreenId = GlobalsV::screenNone;
                    GlobalsV::PostLoadScreenId = GlobalsV::FormToId(this);
                    RequestClose(1);
                    return;
                }
                Stage = 12;
                FinishQueuedDraw();
                Stage = 13;
                CommitFrameDraw();
                Stage = 14;
                ResetSavedLineCount();
                Stage = 15;
                ResetSecondaryPixelCount();
                Stage = 16;
                GR_Main::EndFramePresentation();
            }
            Stage = 17;
            RedrawMap();
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"TfStarMap.Draw2, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfStarMap::RunTalkDialogs() {
        std::int32_t Index{};
        PrepareTalkDisplay();
        GlobalsV::TalkReturnScreenId = GlobalsV::FormToId(this);
        GlobalsV::RequestedScreenId = GlobalsV::screenTalk;
        if (aPlayer::GetPlayer() != Globals::TalkShip) {
            GR_Main::SoundManager->PlaySound(u"Sound.Talk"_wref.get());
        }
        ScannerSelectionActive = false;
        TalkSelectionActive = false;
        HideActionRanges();
        SetCursorActive(false);
        for (Index = 0; Index <= 2; ++Index) {
            GR_Main::PresentScreenBuffer();
            Present();
        }
        if (GR_Main::AuxRenderBuffer != nullptr) {
            GR_Main::AuxRenderBuffer->Clear();
        }
        GR_Main::CaptureScreenBackground(false, 0);
        SetCursorActive(true);
        std::int32_t Trading = 0;
        Globals::TalkScreen->PlayTransitionSounds = true;
        while (true) {
            if (Trading == 0) {
                fTalk::RunTalk(this);
                if (Globals::TalkScreen->ModalTransition == fTalk::tmtTrade) {
                    Trading = 1;
                }
            } else {
                fGoodsShop2::RunGoodsShop(this);
                Globals::TalkScreen->ReturnedFromTrade = true;
                if (!Globals::GoodsShopScreen->ReopenRequested) {
                    Trading = 0;
                }
            }
            MainPanel->RefreshMoneyAndCargo();
            if (Globals::TalkScreen->ModalTransition == fTalk::tmtNone && static_cast<std::uint8_t>(Globals::GoodsShopScreen->ReopenRequested ^ 1)) {
                MainPanel->RebuildMessageButtons(false);
            } else if (Trading == 0) {
                MainPanel->RebuildMessageButtons(false);
            } else {
                MainPanel->ShowHelpText(pas::WideString(), false);
                MainPanel->ClearMessageButtons();
            }
            ScannerSelectionActive = false;
            TalkSelectionActive = false;
            HideActionRanges();
            SetCursorActive(false);
            Present();
            SetCursorActive(true);
            if (Globals::TalkScreen->ModalTransition == fTalk::tmtNone && static_cast<std::uint8_t>(Globals::GoodsShopScreen->ReopenRequested ^ 1)) {
                break;
            }
            SetCursorActive(false);
            GR_Main::CaptureScreenBackground(Trading != 0, 1);
            SetCursorActive(true);
        }
        if (Globals::SpaceProcess != nullptr && Globals::SpaceProcess->Space != nullptr) {
            Globals::SpaceProcess->BindMinimap(GetByName(u"MapPanel"sv));
            if (Mode == smmOrders) {
                Globals::SpaceProcess->Space->ScrollChangedCallback = pas::bind_method<&TfStarMap::MapScrollChanged>(this);
            } else {
                Globals::SpaceProcess->Space->ScrollChangedCallback = nullptr;
            }
            if (Mode == smmOrders) {
                RebuildPartnerButtons();
                RebuildTargetMarkers();
            }
        }
        RestorePendingSceneObjects();
        ResumeMode = smrNormal;
        if (GlobalsV::GameEndReason == GlobalsV::gerTerronConversion) {
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            GR_DX::ReleaseAllTextureSurfaces();
            RequestClose(1);
        }
    }

    void TfStarMap::GalaxyClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aGalaxy::Galaxy->SpecialSimulationMode == 0) {
            MainPanel->NavigationLocked = true;
            if (!Globals::ShipScreen->ReopenRequested) {
                HideLargeHelp();
                GetByName(u"PM_WinMsg"sv)->SetActive(false);
                SetCursorActive(false);
                Present();
                GR_Main::CaptureScreenBackground(true, 0);
                SetCursorActive(true);
                Globals::GalaxyScreen->ViewMode = 2;
                GlobalsV::GalaxyReturnScreenId = GlobalsV::FormToId(this);
                GlobalsV::RequestedScreenId = GlobalsV::screenGalaxy;
                RequestClose(1);
            }
        }
    }

    void TfStarMap::ShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (aGalaxy::Galaxy->SpecialSimulationMode == 0) {
            MainPanel->NavigationLocked = true;
            if (!Globals::ShipScreen->ReopenRequested) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(13131);
                HideLargeHelp();
                ShowObjectInfo(nullptr);
                ClearPathOverlay(true);
                BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                SetCursorActive(false);
                Present();
                GR_Main::CaptureScreenBackground(true, 0);
                SetCursorActive(true);
                Globals::ShipScreen->ShipToInspect = ShipToInspect;
                if (ShipToInspect == nullptr) {
                    fShip2::PlayerHoldShip = aPlayer::GetPlayer();
                }
                Globals::ShipScreen->PlayTransitionSounds = true;
                GlobalsV::ShipReturnScreenId = GlobalsV::FormToId(this);
                aGalaxy::TGalaxy::ClearIntegrityStatus();
                GlobalsV::RequestedScreenId = GlobalsV::screenShip;
                RequestClose(1);
            }
        }
    }

    void TfStarMap::StartOrderMode() {
        std::int32_t Index{};
        aGalaxy::TGalaxy::ClearIntegrityStatus();
        ScrollLeftHeld = false;
        ScrollRightHeld = false;
        ScrollUpHeld = false;
        ScrollDownHeld = false;
        Globals::MinimapFrameCounter = 0;
        ScannerSelectionActive = false;
        TalkSelectionActive = false;
        for (Index = 0; Index <= 4; ++Index) {
            SelectedWeapons[Index] = false;
        }
        InterceptorSelectionActive = false;
        CustomSelectionActive = false;
        DisplayedObject = nullptr;
        CursorObject = nullptr;
        UpdateRectsEnabled = true;
        MapControls->Invalidate();
        UpdateRectsEnabled = false;
        aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
        CenterShipButton->DownCallback = pas::bind_method<&TfStarMap::CenterShipClicked>(this);
        CenterShipButton->MouseEnterCallback = pas::bind_method<&TfStarMap::CenterShipMouseEnter>(this);
        CenterShipButton->MouseLeaveCallback = pas::bind_method<&TfStarMap::CenterShipMouseLeave>(this);
        if (SpaceEffectsTimer != nullptr) {
            CancelCallbackTimer(SpaceEffectsTimer);
            SpaceEffectsTimer = nullptr;
        }
        SpaceEffectsTimer = ScheduleCallbackTimer(18, 18, pas::bind_static_method<&TfStarMap::AdvanceSpaceEffects>(this), 0);
        InfoWindow->SetActive(false);
        ItemInfoWindow->SetActive(false);
        ShipInfoPanel->SetActive(false);
        PlanetInfoPanel->SetActive(false);
        StarInfoWindow->SetActive(false);
        StandardInfoPanel->SetActive(false);
        Globals::SpaceProcess->Space->ScrollChangedCallback = pas::bind_method<&TfStarMap::MapScrollChanged>(this);
        MainPanel->Show();
        WeaponPanel->SetActive(Globals::StarMapWeaponPanelOpen);
        MapControls->LeftButtonDownCallback = pas::bind_method<&TfStarMap::MapLeftButtonDown>(this);
        ConfigureMiddleButtonAction();
        MapControls->RightButtonDownCallback = pas::bind_method<&TfStarMap::MapRightButtonDown>(this);
        MapControls->RightButtonDoubleClickCallback = nullptr;
        MapControls->MouseMoveCallback = pas::bind_method<&TfStarMap::MapMouseMove>(this);
        ContentPanel->KeyDownCallback = pas::bind_method<&TfStarMap::OrderKeyDown>(this);
        ContentPanel->KeyUpCallback = pas::bind_method<&TfStarMap::OrderKeyUp>(this);
        Globals::SpaceProcess->Space->DrawMinimap();
        RebuildTargetMarkers();
        RefreshActionRanges();
        RefreshWeaponButtons();
        MainPanel->RebuildMessageButtons(false);
        Mode = smmOrders;
        aGalaxy::Galaxy->PrimeIntegrityChecksum(25);
        BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
        RebuildPartnerButtons();
        MainPanel->SetDateRange(aGalaxy::Galaxy->CurrentTurn, aGalaxy::Galaxy->CurrentTurn);
        MainPanel->RefreshDate();
        if (Globals::StarMapWeaponPanelOpen) {
            WeaponPanelProgress = 1.0f;
        } else {
            WeaponPanelProgress = 0.0f;
        }
        if (WeaponPanelTimer != nullptr) {
            CancelCallbackTimer(WeaponPanelTimer);
            WeaponPanelTimer = nullptr;
        }
        UpdateWeaponPanelPosition();
        if (AnimateSpacePanelOnResume) {
            if (SpacePanelTimer != nullptr) {
                CancelCallbackTimer(SpacePanelTimer);
                SpacePanelTimer = nullptr;
            }
            SpacePanelProgress = 0.0f;
            AnimateSpacePanel(1);
        } else {
            if (SpacePanelTimer != nullptr) {
                CancelCallbackTimer(SpacePanelTimer);
                SpacePanelTimer = nullptr;
            }
            SpacePanelProgress = 1.0f;
            UpdateSpacePanelPosition();
        }
        AnimateSpacePanelOnResume = false;
        aGalaxy::Galaxy->CheckIntegrityChecksum(311);
        UpdateTerronTransformation();
        aGalaxy::Galaxy->PrimeIntegrityChecksum(310);
        TalkButton->SetDisabled(static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetRadar()) ^ 1));
        ScannerButton->SetDisabled((static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetRadar()) ^ 1) || static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetScanner()) ^ 1)) && aGalaxy::Galaxy->UltraScanModEnabled == 0);
        if (CenterShipButton->IsHovered()) {
            CenterShipMouseLeave(nullptr);
            CenterShipMouseEnter(nullptr);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"sv))->SetHovered(false);
        GR_Main::PostMouseMoveMessage();
        if (GlobalsV::GameEndReason == GlobalsV::gerTerronConversion) {
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            GR_DX::ReleaseAllTextureSurfaces();
            RequestClose(1);
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(105);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(106);
    }

    void TfStarMap::StopOrderMode() {
        std::int32_t Index{};
        aGalaxy::Galaxy->CheckIntegrityChecksum(26);
        if (DeferredEndTurnTimer != nullptr) {
            CancelCallbackTimer(DeferredEndTurnTimer);
            DeferredEndTurnTimer = nullptr;
        }
        if (PlayerPathTimer != nullptr) {
            CancelCallbackTimer(PlayerPathTimer);
            PlayerPathTimer = nullptr;
        }
        Mode = smmInactive;
        if (WeaponPanelTimer != nullptr) {
            CancelCallbackTimer(WeaponPanelTimer);
            WeaponPanelTimer = nullptr;
        }
        AnimateSpacePanel(-1);
        CenterShipButton->DownCallback = nullptr;
        CursorObject = nullptr;
        ScannerSelectionActive = false;
        TalkSelectionActive = false;
        InterceptorSelectionActive = false;
        CustomSelectionActive = false;
        for (Index = 0; Index <= 4; ++Index) {
            SelectedWeapons[Index] = false;
        }
        RefreshActionRanges();
        HideActionRanges();
        MainPanel->ClearMessageButtons();
        HideOrderInterface();
        Globals::SpaceProcess->Space->ScrollChangedCallback = nullptr;
        MapControls->LeftButtonDownCallback = nullptr;
        MapControls->LeftButtonDoubleClickCallback = nullptr;
        MapControls->RightButtonDownCallback = nullptr;
        MapControls->MouseMoveCallback = nullptr;
        ContentPanel->KeyDownCallback = nullptr;
        ContentPanel->KeyUpCallback = nullptr;
        if (SpaceEffectsTimer != nullptr) {
            CancelCallbackTimer(SpaceEffectsTimer);
            SpaceEffectsTimer = nullptr;
        }
    }

    void TfStarMap::HideOrderInterface() {
        ClearAsteroidPath();
        ClearTargetMarkers();
        ClearMapAnimations();
        ClearPathOverlay(true);
        ClearPathOverlay(false);
        MainPanel->ClearMessageButtons();
        InfoWindow->SetActive(false);
        ItemInfoWindow->SetActive(false);
        ShipInfoPanel->SetActive(false);
        PlanetInfoPanel->SetActive(false);
        StarInfoWindow->SetActive(false);
        StandardInfoPanel->SetActive(false);
        MainPanel->Hide();
    }

    void TfStarMap::ConfigureMiddleButtonAction() {
        if (GlobalsV::ActionDoubleClick) {
            MapControls->LeftButtonDoubleClickCallback = pas::bind_method<&TfStarMap::MapMiddleButtonDown>(this);
            return;
        }
        MapControls->LeftButtonDoubleClickCallback = pas::bind_method<&TfStarMap::MapLeftButtonDown>(this);
    }

    void TfStarMap::ScrollMap(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        WindowsSdk::TPoint Point{};
        WindowsSdk::TPoint OldPoint{};
        OldPoint = GetMapCenter();
        Point = OldPoint;
        if (ScrollLeftHeld) {
            Point.X -= GlobalsV::ScrollStep;
        }
        if (ScrollRightHeld) {
            Point.X += GlobalsV::ScrollStep;
        }
        if (ScrollUpHeld) {
            Point.Y -= GlobalsV::ScrollStep;
        }
        if (ScrollDownHeld) {
            Point.Y += GlobalsV::ScrollStep;
        }
        std::int16_t X = GetCursorPoint().X;
        std::int16_t Y = GetCursorPoint().Y;
        {
            WindowsSdk::HWND cpp_left = WindowsSdk::GetForegroundWindow();
            if (cpp_left == GR_Main::MainWindowHandle) {
                if (X >= -30 && X <= GR_Main::GameScreenWidth + 30) {
                    if (X < GlobalsV::ScrollSense) {
                        Point.X -= GlobalsV::ScrollStep;
                    }
                    if (X > GR_Main::GameScreenWidth - GlobalsV::ScrollSense - 1) {
                        Point.X += GlobalsV::ScrollStep;
                    }
                }
                if (Y >= -30 && Y <= GR_Main::GameScreenHeight + 30) {
                    if (Y < GlobalsV::ScrollSense) {
                        Point.Y -= GlobalsV::ScrollStep;
                    }
                    if (Y > GR_Main::GameScreenHeight - GlobalsV::ScrollSense - 1) {
                        Point.Y += GlobalsV::ScrollStep;
                    }
                }
                if (OldPoint.X != Point.X || OldPoint.Y != Point.Y) {
                    SetMapCenterManually(Point);
                }
            }
        }
    }

    void TfStarMap::AdvanceSpaceEffects(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (Globals::TrailingFilmEffects != nullptr) {
            Globals::TrailingFilmEffects->AdvanceEffects();
            if (Globals::TrailingFilmEffects->FirstEntry == nullptr) {
                pas::free(Globals::TrailingFilmEffects);
                Globals::TrailingFilmEffects = nullptr;
            }
        }
        Globals::SpaceProcess->Space->AdvanceTimers();
        Globals::SpaceProcess->Space->AdvanceObjects();
    }

    std::uint8_t TfStarMap::IsMapPointBlocked(GI_MessageLoop::TObjectGI* Sender, WindowsSdk::TPoint Point) {
        std::uint8_t Result = true;
        if (Sender != nullptr && Sender->IsOccludedAtPoint(Point)) {
            return Result;
        }
        if (MainPanel->BackgroundImage->HitTestPixel(Point)) {
            return Result;
        }
        if (WeaponPanel->Active && WeaponBackgroundImage->HitTestPixel(Point)) {
            return Result;
        }
        if (SpaceBackgroundImage->HitTestPixel(Point)) {
            return Result;
        }
        if (WeaponPanel->Active && WeaponButtons[0]->HitTest(Point)) {
            return Result;
        }
        if (WeaponPanel->Active && WeaponButtons[1]->HitTest(Point)) {
            return Result;
        }
        if (WeaponPanel->Active && WeaponButtons[2]->HitTest(Point)) {
            return Result;
        }
        if (WeaponPanel->Active && WeaponButtons[3]->HitTest(Point)) {
            return Result;
        }
        if (WeaponPanel->Active && WeaponButtons[4]->HitTest(Point)) {
            return Result;
        }
        if (WeaponPanel->Active && AllWeaponsButton->HitTest(Point)) {
            return Result;
        }
        if (HideSpacePanelButton->HitTest(Point)) {
            return Result;
        }
        if (ShowSpacePanelButton->HitTest(Point)) {
            return Result;
        }
        if (ScannerButton->HitTest(Point)) {
            return Result;
        }
        if (TalkButton->HitTest(Point)) {
            return Result;
        }
        if (TurnFilmButton->HitTest(Point)) {
            return Result;
        }
        if (MainPanel->ShipButton->HitTest(Point)) {
            return Result;
        }
        if (MainPanel->GalaxyButton->HitTest(Point)) {
            return Result;
        }
        if (MainPanel->QuestButton->HitTest(Point)) {
            return Result;
        }
        if (MainPanel->EndTurnButton->HitTest(Point)) {
            return Result;
        }
        if (MainPanel->MenuButton->HitTest(Point)) {
            return Result;
        }
        return false;
    }

    void TfStarMap::MapLeftButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        EC_Struct::TPointF Destination{};
        aShip::TShip* Ship{};
        pas::Object* Location{};
        aGalaxy::THole* Hole{};
        std::int32_t Index{};
        std::int32_t Mode{};
        std::int32_t ActionResult{};
        EC_Struct::TPointF TargetPosition{};
        aShip::PCustomShipInfo Info{};
        aItem::TWeapon* Weapon{};
        aPath::PSPathNode Node{};
        aPath::PSPathNode FollowingNode{};
        std::uint8_t AnimatePath{};
        pas::WideString Response{};
        pas::WideString InitialImage{};
        if (MainPanel->NavigationLocked) {
            return;
        }
        if (Globals::ShipScreen->ReopenRequested) {
            return;
        }
        HideLargeHelp();
        if (IsMapPointBlocked(Sender, Point)) {
            return;
        }
        CursorObject = FindObjectAtCursor();
        std::uint8_t ScanUnresolved = ScannerSelectionActive;
        std::uint8_t TalkUnresolved = TalkSelectionActive;
        std::uint8_t OutOfRange = false;
        std::uint8_t HadWeapons = false;
        std::int32_t SelectedCount = 0;
        for (Index = 0; Index <= 4; ++Index) {
            if (SelectedWeapons[Index]) {
                ++SelectedCount;
                HadWeapons = true;
            }
        }
        GI_Panel::TPanelGI* MapControl = MapControls;
        Destination = EC_Struct::PointToPointF(MapControl->ToLocalPoint(Point));
        if (CustomSelectionActive && CursorObject != nullptr && !(pas::class_cast_if<aGalaxy::THole*>(CursorObject) != nullptr)) {
            TargetPosition.X = 0.0f;
            TargetPosition.Y = 0.0f;
            if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(CursorObject)) {
                TargetPosition = ship->Position;
            } else if (aMissile::TMissile* missile = pas::class_cast_if<aMissile::TMissile*>(CursorObject)) {
                TargetPosition = missile->Position;
            } else if (aAsteroid::TAsteroid* asteroid = pas::class_cast_if<aAsteroid::TAsteroid*>(CursorObject)) {
                TargetPosition = asteroid->Position;
            } else if (aItem::TItem* item = pas::class_cast_if<aItem::TItem*>(CursorObject)) {
                TargetPosition = item->Position;
            } else if (aPlanet::TPlanet* planet = pas::class_cast_if<aPlanet::TPlanet*>(CursorObject)) {
                TargetPosition = planet->GetPosition();
            } else if (aGalaxy::THole* hole = pas::class_cast_if<aGalaxy::THole*>(CursorObject)) {
                // Kept by the native routine despite the outer hole exclusion.
                if (hole->Star1 == aGalaxy::PlayerStar) {
                    TargetPosition = hole->Position1;
                } else {
                    TargetPosition = hole->Position2;
                }
            }
            if (static_cast<long double>(aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, TargetPosition)) > CustomSelectionRadius * CustomSelectionRadius) {
                ShowLargeHelp(CustomSelectionOutOfRangeText);
            } else {
                ActionResult = 0;
                if (CustomSelectionItem != nullptr) {
                    if (CustomSelectionItem->ScriptItem != nullptr) {
                        ActionResult = reinterpret_cast<aScript::TScriptItem*>(CustomSelectionItem->ScriptItem)->RunActionCode(aGalaxyStruct::satOnCustomTargetting, aPlayer::GetPlayer(), CursorObject, nullptr, ActionResult);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(CustomSelectionItem) != nullptr) {
                        ActionResult = aScript::RunItemConfigActionCode(CustomSelectionItem, aGalaxyStruct::satOnCustomTargetting, aPlayer::GetPlayer(), CursorObject, nullptr, ActionResult);
                    }
                } else {
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CustomShipInfos) - 1); cpp_range.next(Index); ) {
                        Info = pas::list_at<aShip::TCustomShipInfo>(aPlayer::GetPlayer()->CustomShipInfos, Index);
                        if (static_cast<std::uint8_t>(Info->DeleteQueued ^ 1) && Info->TypeName == CustomSelectionInfoName) {
                            ActionResult = aScript::RunCustomShipInfoActionCode(Info, aGalaxyStruct::satOnCustomTargetting, aPlayer::GetPlayer(), CursorObject, nullptr, ActionResult);
                            break;
                        }
                    }
                }
                if (ActionResult > 0) {
                    ShowLargeHelp(CustomSelectionSuccessText);
                } else if (ActionResult == 0) {
                    ShowLargeHelp(CustomSelectionFailureText);
                }
            }
            CustomSelectionActive = false;
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
            return;
        }
        if (!(TalkSelectionActive && pas::class_cast_if<aGalaxy::TStar*>(CursorObject) != nullptr && aKling::TerronShip != nullptr && aPlayer::GetPlayer()->CurrentStar == aKling::TerronShip->CurrentStar && aGalaxy::Galaxy->TerronToStarTurn >= aGalaxyStruct::TerronTransformationFlag)) {
            if ((ScannerSelectionActive || TalkSelectionActive) && !(pas::class_cast_if<aShip::TShip*>(CursorObject) != nullptr) || pas::class_cast_if<aShip::TShip*>(CursorObject) != nullptr && (ScannerSelectionActive && (static_cast<aShip::TShip*>(CursorObject)->NoScan || aPlayer::GetPlayer()->ScanLocked) || TalkSelectionActive && (static_cast<aShip::TShip*>(CursorObject)->NoTalk || aPlayer::GetPlayer()->TalkLocked) || (HadWeapons || InterceptorSelectionActive) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CanSelectShipTarget(static_cast<aShip::TShip*>(CursorObject)) ^ 1))) {
                if (ScannerSelectionActive) {
                    const pas::WideString& lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"Help.ScanImpossible"_wref.get());
                    TfStarMap* self = this;
                    self->ShowLargeHelp(lookupLocalizedTextByKey);
                } else if (TalkSelectionActive) {
                    const pas::WideString& lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"Help.TalkImpossible"_wref.get());
                    TfStarMap* self_2 = this;
                    self_2->ShowLargeHelp(lookupLocalizedTextByKey_2);
                } else if (HadWeapons) {
                    const pas::WideString& lookupLocalizedTextByKey_3 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotCancel"_wref.get());
                    TfStarMap* self_3 = this;
                    self_3->ShowLargeHelp(lookupLocalizedTextByKey_3);
                }
                ScannerSelectionActive = false;
                TalkSelectionActive = false;
                RefreshActionRanges();
                RefreshWeaponButtons();
                UpdateActionCursor(false);
                return;
            }
        }
        if (aPlayer::GetPlayer()->CalculateSpeed() <= 0 && static_cast<std::uint8_t>(HadWeapons ^ 1) && static_cast<std::uint8_t>(ScannerSelectionActive ^ 1) && static_cast<std::uint8_t>(TalkSelectionActive ^ 1) && static_cast<std::uint8_t>(InterceptorSelectionActive ^ 1) && !(pas::class_cast_if<aItem::TItem*>(CursorObject) != nullptr)) {
            SetCursorActive(false);
            DrawFrame();
            SetCursorActive(true);
            {
                const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"Help.Speed0"_wref.get());
                GI_MessageLoop::TMessageLoopGI* self_4 = this;
                GI_MessageBox::ShowMessageBoxGI(self_4, localizedColorText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
            }
            GR_Main::FullFrameRedrawRequested = true;
            DrawFrame();
            return;
        }
        if (CursorObject != nullptr) {
            if (aPlanet::TPlanet* planet_2 = pas::class_cast_if<aPlanet::TPlanet*>(CursorObject); planet_2 != nullptr && planet_2->NoLanding) {
                CursorObject = nullptr;
            } else if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(CursorObject)) {
                if (HadWeapons || InterceptorSelectionActive) {
                    if (!aPlayer::GetPlayer()->CanSelectShipTarget(ruins)) {
                        CursorObject = nullptr;
                    }
                }
                if (static_cast<std::uint8_t>(HadWeapons ^ 1) && static_cast<std::uint8_t>(InterceptorSelectionActive ^ 1)) {
                    if (!static_cast<aRuins::TRuins*>(CursorObject)->CheckDockingPermission(aPlayer::GetPlayer(), Response)) {
                        ShowLargeHelp(Response);
                        CursorObject = nullptr;
                    }
                }
            } else if (aGalaxy::THole* hole_2 = pas::class_cast_if<aGalaxy::THole*>(CursorObject)) {
                if (aPlayer::GetPlayer()->NoJump || hole_2->ArcadeMapName == u"NoEntry") {
                    CursorObject = nullptr;
                }
            }
        }
        if (CursorObject == nullptr) {
            ClearPathOverlay(true);
            aRanger::PendingPlayerFollowTarget = nullptr;
            aGalaxy::Galaxy->CheckIntegrityChecksum(27);
            aPlayer::GetPlayer()->OrderMove(Destination, false);
            aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
            if (aPlayer::GetPlayer()->MovementPath != nullptr && aPlayer::GetPlayer()->MovementPath->ActiveHead != nullptr) {
                Node = aPlayer::GetPlayer()->MovementPath->ActiveHead;
                while (Node != nullptr) {
                    FollowingNode = (static_cast<void>(aPlayer::GetPlayer()->MovementPath), aPath::TSPath::GetFollowingNode(Node, 198));
                    if (FollowingNode == nullptr) {
                        break;
                    }
                    if (FollowingNode->Next == nullptr) {
                        break;
                    }
                    Node = FollowingNode->Next;
                }
                if (aMyFunction::PointDistanceSquared(Node->Position, aPlayer::GetPlayer()->MovementPath->ActiveTail->Position) < 4.9E+3L) {
                    FollowingNode = Node;
                    while (FollowingNode != nullptr && aMyFunction::PointDistanceSquared(FollowingNode->Position, Node->Position) < 25.0L) {
                        FollowingNode = FollowingNode->Prev;
                    }
                    if (FollowingNode != nullptr) {
                        aPlayer::GetPlayer()->OrderMove(FollowingNode->Position, false);
                        aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                    }
                }
            } else {
                aPlayer::GetPlayer()->OrderNone(false);
            }
            BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
            if (InterceptorSelectionActive) {
                if (aPlayer::GetPlayer()->GetHull()->InterceptorTarget != nullptr) {
                    aPlayer::GetPlayer()->GetHull()->InterceptorTarget = nullptr;
                    RebuildTargetMarkers();
                }
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(28);
        } else if (aPlanet::TPlanet* planet_3 = pas::class_cast_if<aPlanet::TPlanet*>(CursorObject)) {
            Location = planet_3;
            ClearPathOverlay(true);
            aGalaxy::Galaxy->CheckIntegrityChecksum(33);
            aRanger::PendingPlayerFollowTarget = nullptr;
            if (!(aPlayer::GetPlayer()->Order == aShip::soLand && aPlayer::GetPlayer()->OrderTarget == Location || pas::checked_cast<aPlanet::TPlanet*>(CursorObject)->OwnerId == aGalaxyStruct::oiDominator || aPlayer::GetPlayer()->CurrentStar->Status.CustomFaction != u"" && static_cast<aPlanet::TPlanet*>(CursorObject)->OwnerId != aGalaxyStruct::oiUninhabited)) {
                aPlayer::GetPlayer()->OrderLanding(Location, false);
                aPlayer::GetPlayer()->OrderDestination = EC_Struct::SubtractPointsF(Destination, reinterpret_cast<aPlanet::TPlanet*>(Location)->GetPosition());
                {
                    const pas::WideString& lookupLocalizedTextByKey_4 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveLanding"_wref.get());
                    TfStarMap* self_5 = this;
                    self_5->ShowLargeHelp(lookupLocalizedTextByKey_4);
                }
            } else {
                aPlayer::GetPlayer()->OrderMove(Destination, false);
                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(34);
            BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
        } else if (aGalaxy::THole* hole_3 = pas::class_cast_if<aGalaxy::THole*>(CursorObject)) {
            Hole = hole_3;
            ClearPathOverlay(true);
            aGalaxy::Galaxy->CheckIntegrityChecksum(35);
            aRanger::PendingPlayerFollowTarget = nullptr;
            if (!(aPlayer::GetPlayer()->Order == aShip::soJumpHole && aPlayer::GetPlayer()->OrderTarget == Hole)) {
                aPlayer::GetPlayer()->OrderJumpHole(Hole, false);
                aPlayer::GetPlayer()->OrderDestination = Destination;
                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
            } else {
                aPlayer::GetPlayer()->OrderMove(Destination, false);
                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(36);
            BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
        } else if (pas::class_cast_if<aRuins::TRuins*>(CursorObject) != nullptr || aKling::TerronShip == CursorObject && aGalaxy::Galaxy->TerronLandingLockTurn > 0 && static_cast<std::uint8_t>(TalkSelectionActive ^ 1)) {
            Ship = pas::checked_cast<aShip::TShip*>(CursorObject);
            ClearPathOverlay(true);
            aGalaxy::Galaxy->CheckIntegrityChecksum(37);
            aRanger::PendingPlayerFollowTarget = nullptr;
            if (HadWeapons && aPlayer::GetPlayer() != CursorObject) {
                Mode = 0;
                aGalaxy::Galaxy->CheckIntegrityChecksum(41);
                for (Index = 0; Index <= 4; ++Index) {
                    if (SelectedWeapons[Index]) {
                        Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                        {
                            pas::Extended cpp_right = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                            if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, pas::checked_cast<aShip::TShip*>(CursorObject)->Position) <= cpp_right) {
                                SelectedWeapons[Index] = false;
                                Weapon->Target = CursorObject;
                                Mode = 1;
                            } else if (Mode == 0) {
                                Mode = 2;
                                OutOfRange = true;
                            }
                        }
                    }
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(42);
                if (Mode == 1) {
                    const pas::WideString& lookupLocalizedTextByKey_5 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotSet"_wref.get());
                    TfStarMap* self_6 = this;
                    self_6->ShowLargeHelp(lookupLocalizedTextByKey_5);
                } else if (Mode == 2) {
                    const pas::WideString& lookupLocalizedTextByKey_6 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotOutRange"_wref.get());
                    TfStarMap* self_7 = this;
                    self_7->ShowLargeHelp(lookupLocalizedTextByKey_6);
                }
                RebuildTargetMarkers();
            } else if (InterceptorSelectionActive && aPlayer::GetPlayer() != CursorObject) {
                if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, pas::checked_cast<aShip::TShip*>(CursorObject)->Position) <= 1.0E+3L && pas::checked_cast<aRuins::TRuins*>(CursorObject)->InterceptorPassesRemaining == 0) {
                    aPlayer::GetPlayer()->GetHull()->InterceptorTarget = static_cast<aRuins::TRuins*>(CursorObject);
                    {
                        const pas::WideString& lookupLocalizedTextByKey_7 = GR_Main::LookupLocalizedTextByKey(u"Help.InterceptorsSet"_wref.get());
                        TfStarMap* self_8 = this;
                        self_8->ShowLargeHelp(lookupLocalizedTextByKey_7);
                    }
                } else if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, static_cast<aShip::TShip*>(CursorObject)->Position) > 1.0E+3L) {
                    const pas::WideString& lookupLocalizedTextByKey_8 = GR_Main::LookupLocalizedTextByKey(u"Help.InterceptorsOutRange"_wref.get());
                    TfStarMap* self_9 = this;
                    self_9->ShowLargeHelp(lookupLocalizedTextByKey_8);
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(42);
                RebuildTargetMarkers();
            } else if (!(aPlayer::GetPlayer()->Order == aShip::soLand && aPlayer::GetPlayer()->OrderTarget == Ship || static_cast<std::uint8_t>(aShip::TShip_GetRelationLevelToShip(Ship, aPlayer::GetPlayer())) <= 0 && (Ship != aKling::TerronShip || aGalaxy::Galaxy->TerronLandingLockTurn <= 0))) {
                aPlayer::GetPlayer()->OrderLanding(Ship, false);
                aPlayer::GetPlayer()->OrderDestination = EC_Struct::SubtractPointsF(Destination, Ship->Position);
                {
                    const pas::WideString& lookupLocalizedTextByKey_9 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveLanding"_wref.get());
                    TfStarMap* self_10 = this;
                    self_10->ShowLargeHelp(lookupLocalizedTextByKey_9);
                }
            } else {
                aPlayer::GetPlayer()->OrderMove(Destination, false);
                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(38);
            BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
        } else if (aShip::TShip* ship_2 = pas::class_cast_if<aShip::TShip*>(CursorObject)) {
            Ship = ship_2;
            if (ScannerSelectionActive) {
                if (([&] {
                    pas::Extended cpp_left = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                    return cpp_left <= aPlayer::GetPlayer()->GetRadarRange();
                }()) && aPlayer::GetPlayer() != CursorObject && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanScanShip(Ship)) && (aGalaxy::Galaxy->UltraScanModEnabled != 0 || aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship) || Ship->TypeId == aGalaxyStruct::stTranclucator)) {
                    if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(Ship); tranclucator != nullptr && tranclucator->OwnerShip == aPlayer::GetPlayer()) {
                        ShipToInspect = Ship;
                        ShipClicked(nullptr);
                    } else {
                        HideLargeHelp();
                        ScannerSelectionActive = false;
                        TalkSelectionActive = false;
                        HideActionRanges();
                        ShowObjectInfo(nullptr);
                        GR_Main::SoundManager->PlaySound(u"Sound.Scan"_wref.get());
                        SetCursorActive(false);
                        Present();
                        GR_Main::CaptureScreenBackground(true, 0);
                        SetCursorActive(true);
                        ScanUnresolved = false;
                        Globals::ScannerTarget = CursorObject;
                        GlobalsV::ScannerReturnScreenId = GlobalsV::FormToId(this);
                        GlobalsV::RequestedScreenId = GlobalsV::screenScanner;
                        RequestClose(1);
                    }
                } else if (aPlayer::GetPlayer() == CursorObject) {
                    ScanUnresolved = false;
                    ShipToInspect = aPlayer::GetPlayer();
                    ShipClicked(nullptr);
                } else {
                    pas::Extended cpp_left_2 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, pas::checked_cast<aShip::TShip*>(CursorObject)->Position);
                    if (cpp_left_2 > aPlayer::GetPlayer()->GetRadarRange()) {
                        ScanUnresolved = false;
                        {
                            const pas::WideString& lookupLocalizedTextByKey_10 = GR_Main::LookupLocalizedTextByKey(u"Help.ScanOutRange"_wref.get());
                            TfStarMap* self_11 = this;
                            self_11->ShowLargeHelp(lookupLocalizedTextByKey_10);
                        }
                    } else if (aPlayer::GetPlayer() != CursorObject && !(pas::class_cast_if<aRuins::TRuins*>(CursorObject) != nullptr)) {
                        if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->CanResolveObjectWithScanner(CursorObject) ^ 1) && static_cast<aShip::TShip*>(CursorObject)->TypeId != aGalaxyStruct::stTranclucator) {
                            ScanUnresolved = false;
                            {
                                const pas::WideString& lookupLocalizedTextByKey_11 = GR_Main::LookupLocalizedTextByKey(u"Help.ScanPowerLow"_wref.get());
                                TfStarMap* self_12 = this;
                                self_12->ShowLargeHelp(lookupLocalizedTextByKey_11);
                            }
                        }
                    }
                }
            } else if (TalkSelectionActive) {
                if (([&] {
                    pas::Extended cpp_left_3 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                    return cpp_left_3 <= aPlayer::GetPlayer()->GetRadarRange();
                }()) && aPlayer::GetPlayer() != CursorObject && !(pas::class_cast_if<aKling::TKling*>(CursorObject) != nullptr && static_cast<aKling::TKling*>(CursorObject)->IsProgramActive(aGalaxyStruct::prgDisconnection))) {
                    HideLargeHelp();
                    TalkUnresolved = false;
                    aScript::CurrentScript = nullptr;
                    Globals::ScriptDialogIndex = -1;
                    Globals::TalkShip = pas::checked_cast<aShip::TShip*>(CursorObject);
                    Globals::TalkScripted = false;
                    aGalaxy::Galaxy->CheckIntegrityChecksum(39);
                    RunTalkDialogs();
                    ClearPathOverlay(true);
                    aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(40);
                    BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                } else {
                    pas::Extended cpp_left_4 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                    if (cpp_left_4 > aPlayer::GetPlayer()->GetRadarRange()) {
                        TalkUnresolved = false;
                        {
                            const pas::WideString& lookupLocalizedTextByKey_12 = GR_Main::LookupLocalizedTextByKey(u"Help.TalkOutRange"_wref.get());
                            TfStarMap* self_13 = this;
                            self_13->ShowLargeHelp(lookupLocalizedTextByKey_12);
                        }
                    }
                }
            } else if (InterceptorSelectionActive && aPlayer::GetPlayer() != CursorObject) {
                if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position) <= 1.0E+3L && Ship->InterceptorPassesRemaining == 0) {
                    aPlayer::GetPlayer()->GetHull()->InterceptorTarget = Ship;
                    {
                        const pas::WideString& lookupLocalizedTextByKey_13 = GR_Main::LookupLocalizedTextByKey(u"Help.InterceptorsSet"_wref.get());
                        TfStarMap* self_14 = this;
                        self_14->ShowLargeHelp(lookupLocalizedTextByKey_13);
                    }
                } else if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position) > 1.0E+3L) {
                    const pas::WideString& lookupLocalizedTextByKey_14 = GR_Main::LookupLocalizedTextByKey(u"Help.InterceptorsOutRange"_wref.get());
                    TfStarMap* self_15 = this;
                    self_15->ShowLargeHelp(lookupLocalizedTextByKey_14);
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(42);
                RebuildTargetMarkers();
            } else if (HadWeapons && aPlayer::GetPlayer() != CursorObject) {
                Mode = 0;
                aGalaxy::Galaxy->CheckIntegrityChecksum(41);
                for (Index = 0; Index <= 4; ++Index) {
                    if (SelectedWeapons[Index]) {
                        Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                        {
                            pas::Extended cpp_right_2 = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                            if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, pas::checked_cast<aShip::TShip*>(CursorObject)->Position) <= cpp_right_2) {
                                SelectedWeapons[Index] = false;
                                Weapon->Target = CursorObject;
                                Mode = 1;
                            } else if (Mode == 0) {
                                Mode = 2;
                                OutOfRange = true;
                            }
                        }
                    }
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(42);
                if (Mode == 1) {
                    const pas::WideString& lookupLocalizedTextByKey_15 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotSet"_wref.get());
                    TfStarMap* self_16 = this;
                    self_16->ShowLargeHelp(lookupLocalizedTextByKey_15);
                } else if (Mode == 2) {
                    const pas::WideString& lookupLocalizedTextByKey_16 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotOutRange"_wref.get());
                    TfStarMap* self_17 = this;
                    self_17->ShowLargeHelp(lookupLocalizedTextByKey_16);
                }
                RebuildTargetMarkers();
            } else if ((WindowsSdk::GetAsyncKeyState(WindowsSdk::VK_CONTROL) & 0x00008000) == 0x00008000 && aPlayer::GetPlayer() != CursorObject) {
                ClearPathOverlay(true);
                aGalaxy::Galaxy->CheckIntegrityChecksum(43);
                aPlayer::GetPlayer()->OrderNone(false);
                if (aPlayer::GetPlayer()->CanSelectShipTarget(pas::checked_cast<aShip::TShip*>(CursorObject))) {
                    aRanger::PendingPlayerFollowTarget = static_cast<aShip::TShip*>(CursorObject);
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(44);
                BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                {
                    const pas::WideString& lookupLocalizedTextByKey_17 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveAuto"_wref.get());
                    TfStarMap* self_18 = this;
                    self_18->ShowLargeHelp(lookupLocalizedTextByKey_17);
                }
            } else if (aPlayer::GetPlayer() == Ship) {
                ClearPathOverlay(true);
                aGalaxy::Galaxy->CheckIntegrityChecksum(45);
                aRanger::PendingPlayerFollowTarget = nullptr;
                aPlayer::GetPlayer()->OrderNone(false);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(46);
            } else {
                Mode = 0;
                if (aRanger::PendingPlayerFollowTarget == Ship) {
                    Mode = 1;
                } else if (aPlayer::GetPlayer()->Order == aShip::soFollowShip && aPlayer::GetPlayer()->OrderTarget == Ship) {
                    if (static_cast<aShip::TFollowMode>(static_cast<std::uint8_t>(aPlayer::GetPlayer()->OrderStateData)) == aShip::fmMinWeaponRange) {
                        Mode = 3;
                    } else {
                        Mode = 2;
                    }
                }
                if (GlobalsV::RightClickOnShip != 2 || Mode == 0) {
                    ClearPathOverlay(true);
                    AnimatePath = false;
                    if (Mode == 0) {
                        SuppressMiddleFollowCycle = true;
                        Mode = GlobalsV::DefaultOrder;
                        if (Mode < 0 || Mode > 3) {
                            Mode = 0;
                        }
                        if (GlobalsV::DefaultOrder == 0) {
                            if (static_cast<std::uint8_t>(aShip::TShip_GetRelationLevelToShip(Ship, aPlayer::GetPlayer())) <= 0) {
                                Mode = 1;
                            } else {
                                Mode = 2;
                            }
                        }
                    } else {
                        SuppressMiddleFollowCycle = false;
                        AnimatePath = GlobalsV::RightClickOnShip != 2 && GlobalsV::ActionDoubleClick;
                        if (AnimatePath) {
                            if (Mode == 1) {
                                InitialImage = u"Bm.PI.PathEndAutoBattle"_w;
                            } else if (Mode == 2) {
                                InitialImage = u"Bm.PI.PathEndFollowNear"_w;
                            } else if (Mode == 3) {
                                InitialImage = u"Bm.PI.PathEndFollowMin"_w;
                            } else {
                                InitialImage = u"Bm.PI.PathEndFollowMax"_w;
                            }
                        }
                        ++Mode;
                        if (Mode > 3) {
                            Mode = 1;
                        }
                    }
                    aGalaxy::Galaxy->CheckIntegrityChecksum(47);
                    if (Mode == 1) {
                        if (!aPlayer::GetPlayer()->CanSelectShipTarget(pas::checked_cast<aShip::TShip*>(CursorObject))) {
                            Mode = 2;
                        }
                    }
                    if (Mode == 1) {
                        aPlayer::GetPlayer()->OrderNone(false);
                        aRanger::PendingPlayerFollowTarget = pas::checked_cast<aShip::TShip*>(CursorObject);
                        {
                            const pas::WideString& lookupLocalizedTextByKey_18 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveAuto"_wref.get());
                            TfStarMap* self_19 = this;
                            self_19->ShowLargeHelp(lookupLocalizedTextByKey_18);
                        }
                    } else if (Mode == 2) {
                        aRanger::PendingPlayerFollowTarget = nullptr;
                        aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmFollowNear, false);
                        {
                            const pas::WideString& lookupLocalizedTextByKey_19 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveNear"_wref.get());
                            TfStarMap* self_20 = this;
                            self_20->ShowLargeHelp(lookupLocalizedTextByKey_19);
                        }
                    } else if (Mode == 3) {
                        aRanger::PendingPlayerFollowTarget = nullptr;
                        aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmMinWeaponRange, false);
                        {
                            const pas::WideString& lookupLocalizedTextByKey_20 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveShot"_wref.get());
                            TfStarMap* self_21 = this;
                            self_21->ShowLargeHelp(lookupLocalizedTextByKey_20);
                        }
                    }
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(48);
                    if (AnimatePath) {
                        BuildShipPathOverlay(aPlayer::GetPlayer(), true, InitialImage);
                    } else {
                        BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                    }
                }
            }
        } else if (pas::class_cast_if<aGalaxy::TStar*>(CursorObject) != nullptr) {
            if (TalkSelectionActive) {
                pas::Extended cpp_left_5 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, aKling::TerronShip->Position);
                if (cpp_left_5 <= aPlayer::GetPlayer()->GetRadarRange()) {
                    Globals::TalkShip = aKling::TerronShip;
                    Globals::TalkScripted = false;
                    Globals::ScriptDialogIndex = -1;
                    aGalaxy::Galaxy->CheckIntegrityChecksum(49);
                    RunTalkDialogs();
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(50);
                }
            } else {
                ClearPathOverlay(true);
                aGalaxy::Galaxy->CheckIntegrityChecksum(51);
                aPlayer::GetPlayer()->OrderMove(Destination, false);
                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(52);
                BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
            }
        } else if (aItem::TItem* item_2 = pas::class_cast_if<aItem::TItem*>(CursorObject)) {
            if (HadWeapons) {
                Mode = 0;
                aGalaxy::Galaxy->CheckIntegrityChecksum(53);
                for (Index = 0; Index <= 4; ++Index) {
                    if (SelectedWeapons[Index]) {
                        Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                        {
                            pas::Extended cpp_right_3 = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                            if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, pas::checked_cast<aItem::TItem*>(CursorObject)->Position) <= cpp_right_3) {
                                SelectedWeapons[Index] = false;
                                Weapon->Target = CursorObject;
                                Mode = 1;
                            } else if (Mode == 0) {
                                Mode = 2;
                                OutOfRange = true;
                            }
                        }
                    }
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(54);
                if (Mode == 1) {
                    const pas::WideString& lookupLocalizedTextByKey_21 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotSet"_wref.get());
                    TfStarMap* self_22 = this;
                    self_22->ShowLargeHelp(lookupLocalizedTextByKey_21);
                } else if (Mode == 2) {
                    const pas::WideString& lookupLocalizedTextByKey_22 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotOutRange"_wref.get());
                    TfStarMap* self_23 = this;
                    self_23->ShowLargeHelp(lookupLocalizedTextByKey_22);
                }
                RebuildTargetMarkers();
            } else if (aItem::CanCargoHookHandleItem(item_2, aPlayer::GetPlayer())) {
                if (!aPlayer::GetPlayer()->IsRecentlyDroppedItem(item_2)) {
                    aGalaxy::Galaxy->CheckIntegrityChecksum(55);
                    if (aPlayer::GetPlayer()->HasPickupTarget(reinterpret_cast<aItem::TItem*>(CursorObject))) {
                        aPlayer::GetPlayer()->RemovePickupTarget(reinterpret_cast<aItem::TItem*>(CursorObject));
                    } else {
                        aPlayer::GetPlayer()->AddPickupTarget(reinterpret_cast<aItem::TItem*>(CursorObject), false);
                    }
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(56);
                    RebuildTargetMarkers();
                }
            }
        } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(CursorObject) != nullptr) {
            if (HadWeapons) {
                Mode = 0;
                aGalaxy::Galaxy->CheckIntegrityChecksum(57);
                for (Index = 0; Index <= 4; ++Index) {
                    if (SelectedWeapons[Index]) {
                        Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                        {
                            pas::Extended cpp_right_4 = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                            if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, pas::checked_cast<aAsteroid::TAsteroid*>(CursorObject)->Position) <= cpp_right_4) {
                                SelectedWeapons[Index] = false;
                                Weapon->Target = CursorObject;
                                Mode = 1;
                            } else if (Mode == 0) {
                                Mode = 2;
                                OutOfRange = true;
                            }
                        }
                    }
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(58);
                if (Mode == 1) {
                    const pas::WideString& lookupLocalizedTextByKey_23 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotSet"_wref.get());
                    TfStarMap* self_24 = this;
                    self_24->ShowLargeHelp(lookupLocalizedTextByKey_23);
                } else if (Mode == 2) {
                    const pas::WideString& lookupLocalizedTextByKey_24 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotOutRange"_wref.get());
                    TfStarMap* self_25 = this;
                    self_25->ShowLargeHelp(lookupLocalizedTextByKey_24);
                }
                RebuildTargetMarkers();
            }
        } else if (pas::class_cast_if<aMissile::TMissile*>(CursorObject) != nullptr) {
            if (HadWeapons) {
                Mode = 0;
                aGalaxy::Galaxy->CheckIntegrityChecksum(59);
                for (Index = 0; Index <= 4; ++Index) {
                    if (SelectedWeapons[Index]) {
                        Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                        {
                            pas::Extended cpp_right_5 = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                            if (aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, pas::checked_cast<aMissile::TMissile*>(CursorObject)->Position) <= cpp_right_5) {
                                SelectedWeapons[Index] = false;
                                Weapon->Target = CursorObject;
                                Mode = 1;
                            } else if (Mode == 0) {
                                Mode = 2;
                                OutOfRange = true;
                            }
                        }
                    }
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(60);
                if (Mode == 1) {
                    const pas::WideString& lookupLocalizedTextByKey_25 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotSet"_wref.get());
                    TfStarMap* self_26 = this;
                    self_26->ShowLargeHelp(lookupLocalizedTextByKey_25);
                } else if (Mode == 2) {
                    const pas::WideString& lookupLocalizedTextByKey_26 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotOutRange"_wref.get());
                    TfStarMap* self_27 = this;
                    self_27->ShowLargeHelp(lookupLocalizedTextByKey_26);
                }
                RebuildTargetMarkers();
            }
        }
        ScannerSelectionActive = false;
        TalkSelectionActive = false;
        InterceptorSelectionActive = false;
        CustomSelectionActive = false;
        for (Index = 0; Index <= 4; ++Index) {
            if (SelectedWeapons[Index]) {
                --SelectedCount;
            }
            SelectedWeapons[Index] = false;
        }
        if (HadWeapons && SelectedCount <= 0 && static_cast<std::uint8_t>(OutOfRange ^ 1)) {
            const pas::WideString& lookupLocalizedTextByKey_27 = GR_Main::LookupLocalizedTextByKey(u"Help.ShotCancel"_wref.get());
            TfStarMap* self_28 = this;
            self_28->ShowLargeHelp(lookupLocalizedTextByKey_27);
        }
        if (ScanUnresolved) {
            const pas::WideString& lookupLocalizedTextByKey_28 = GR_Main::LookupLocalizedTextByKey(u"Help.ScanImpossible"_wref.get());
            TfStarMap* self_29 = this;
            self_29->ShowLargeHelp(lookupLocalizedTextByKey_28);
        }
        if (TalkUnresolved) {
            const pas::WideString& lookupLocalizedTextByKey_29 = GR_Main::LookupLocalizedTextByKey(u"Help.TalkImpossible"_wref.get());
            TfStarMap* self_30 = this;
            self_30->ShowLargeHelp(lookupLocalizedTextByKey_29);
        }
        RefreshActionRanges();
        RefreshWeaponButtons();
        UpdateActionCursor(false);
    }

    void TfStarMap::MapMiddleButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        EC_Struct::TPointF Vector{};
        float Scale{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        aGalaxy::THole* Hole{};
        EC_Struct::TPointF Destination{};
        std::int32_t FollowMode{};
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(Globals::ShipScreen->ReopenRequested ^ 1) && static_cast<std::uint8_t>(IsMapPointBlocked(Sender, Point) ^ 1)) {
            Destination = EC_Struct::PointToPointF(MapControls->ToLocalPoint(Point));
            if (aGalaxy::THole* hole = pas::class_cast_if<aGalaxy::THole*>(CursorObject); hole != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->NoJump ^ 1) && hole->ArcadeMapName != u"NoEntry") {
                Hole = hole;
                ClearPathOverlay(true);
                aGalaxy::Galaxy->CheckIntegrityChecksum(61);
                aRanger::PendingPlayerFollowTarget = nullptr;
                aPlayer::GetPlayer()->OrderJumpHole(Hole, false);
                aPlayer::GetPlayer()->OrderDestination = Destination;
                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(62);
            } else if (CursorObject != nullptr && (pas::class_cast_if<aRuins::TRuins*>(CursorObject) != nullptr && static_cast<aRuins::TRuins*>(CursorObject)->virtual_TShip_CanDock(aPlayer::GetPlayer()) || CursorObject == aKling::TerronShip && aGalaxy::Galaxy->TerronLandingLockTurn > 0)) {
                Ship = pas::checked_cast<aShip::TShip*>(CursorObject);
                ClearPathOverlay(true);
                aGalaxy::Galaxy->CheckIntegrityChecksum(63);
                aRanger::PendingPlayerFollowTarget = nullptr;
                aPlayer::GetPlayer()->OrderLanding(Ship, false);
                aPlayer::GetPlayer()->OrderDestination = EC_Struct::SubtractPointsF(Destination, Ship->Position);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(64);
            } else if (CursorObject != nullptr && pas::class_cast_if<aShip::TShip*>(CursorObject) != nullptr && aPlayer::GetPlayer() != CursorObject && GlobalsV::RightClickOnShip != 2) {
                ClearPathOverlay(true);
                Ship = pas::checked_cast<aShip::TShip*>(CursorObject);
                FollowMode = 0;
                if (aRanger::PendingPlayerFollowTarget == Ship) {
                    FollowMode = 1;
                } else if (aPlayer::GetPlayer()->Order == aShip::soFollowShip && aPlayer::GetPlayer()->OrderTarget == Ship) {
                    if (static_cast<aShip::TFollowMode>(static_cast<std::uint8_t>(aPlayer::GetPlayer()->OrderStateData)) == aShip::fmMinWeaponRange) {
                        FollowMode = 3;
                    } else {
                        FollowMode = 2;
                    }
                }
                if (FollowMode == 0) {
                    FollowMode = GlobalsV::DefaultOrder;
                    if (FollowMode < 0 || FollowMode > 3) {
                        FollowMode = 0;
                    }
                    if (GlobalsV::DefaultOrder == 0) {
                        if (static_cast<std::uint8_t>(aShip::TShip_GetRelationLevelToShip(Ship, aPlayer::GetPlayer())) <= 0) {
                            FollowMode = 1;
                        } else {
                            FollowMode = 2;
                        }
                    }
                } else if (!SuppressMiddleFollowCycle) {
                    --FollowMode;
                    if (FollowMode < 1) {
                        FollowMode = 3;
                    }
                }
                aGalaxy::Galaxy->CheckIntegrityChecksum(65);
                if (FollowMode == 1) {
                    if (!aPlayer::GetPlayer()->CanSelectShipTarget(pas::checked_cast<aShip::TShip*>(CursorObject))) {
                        FollowMode = 2;
                    }
                }
                if (FollowMode == 1) {
                    aPlayer::GetPlayer()->OrderNone(false);
                    aRanger::PendingPlayerFollowTarget = pas::checked_cast<aShip::TShip*>(CursorObject);
                    ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.MoveAuto"_wref.get()));
                } else if (FollowMode == 2) {
                    aRanger::PendingPlayerFollowTarget = nullptr;
                    aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmFollowNear, false);
                    ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.MoveNear"_wref.get()));
                } else if (FollowMode == 3) {
                    aRanger::PendingPlayerFollowTarget = nullptr;
                    aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmMinWeaponRange, false);
                    ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.MoveShot"_wref.get()));
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(66);
            } else if (CursorObject != nullptr && pas::class_cast_if<aPlanet::TPlanet*>(CursorObject) != nullptr && static_cast<std::uint8_t>(static_cast<aPlanet::TPlanet*>(CursorObject)->NoLanding ^ 1)) {
                Planet = static_cast<aPlanet::TPlanet*>(CursorObject);
                ClearPathOverlay(true);
                aGalaxy::Galaxy->CheckIntegrityChecksum(67);
                aRanger::PendingPlayerFollowTarget = nullptr;
                if (pas::checked_cast<aPlanet::TPlanet*>(CursorObject)->OwnerId != aGalaxyStruct::oiDominator && (aPlayer::GetPlayer()->CurrentStar->Status.CustomFaction == u"" || static_cast<aPlanet::TPlanet*>(CursorObject)->OwnerId == aGalaxyStruct::oiUninhabited)) {
                    aPlayer::GetPlayer()->OrderLanding(Planet, false);
                    aPlayer::GetPlayer()->OrderDestination = EC_Struct::SubtractPointsF(Destination, Planet->GetPosition());
                } else {
                    aPlayer::GetPlayer()->OrderMove(Destination, false);
                    aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(68);
            } else if (CursorObject != nullptr && pas::class_cast_if<aItem::TItem*>(CursorObject) != nullptr) {
                Vector.X = static_cast<long double>(aPlayer::GetPlayer()->Position.X) - static_cast<aItem::TItem*>(CursorObject)->Position.X;
                Vector.Y = static_cast<long double>(aPlayer::GetPlayer()->Position.Y) - static_cast<aItem::TItem*>(CursorObject)->Position.Y;
                {
                    pas::Extended cpp_left = pas::real_divide(aPlayer::GetPlayer()->GetCargoHookRange(), 4.0L);
                    Scale = cpp_left * pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Vector.X) * Vector.X + static_cast<long double>(Vector.Y) * Vector.Y));
                }
                Vector.X = System::Round(static_cast<aItem::TItem*>(CursorObject)->Position.X + static_cast<long double>(Scale) * Vector.X);
                Vector.Y = System::Round(static_cast<aItem::TItem*>(CursorObject)->Position.Y + static_cast<long double>(Scale) * Vector.Y);
                aGalaxy::Galaxy->CheckIntegrityChecksum(69);
                aRanger::PendingPlayerFollowTarget = nullptr;
                aPlayer::GetPlayer()->OrderMove(Vector, false);
                if (aItem::CanCargoHookHandleItem(reinterpret_cast<aItem::TItem*>(CursorObject), aPlayer::GetPlayer()) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->IsRecentlyDroppedItem(reinterpret_cast<aItem::TItem*>(CursorObject)) ^ 1)) {
                    if (aPlayer::GetPlayer()->HasPickupTarget(reinterpret_cast<aItem::TItem*>(CursorObject))) {
                        aPlayer::GetPlayer()->RemovePickupTarget(reinterpret_cast<aItem::TItem*>(CursorObject));
                    }
                    aPlayer::GetPlayer()->AddPickupTarget(reinterpret_cast<aItem::TItem*>(CursorObject), true);
                    RebuildTargetMarkers();
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(70);
            }
            if (Mode == smmOrders) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(71);
                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(72);
                BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                if (DeferredEndTurnTimer != nullptr) {
                    CancelCallbackTimer(DeferredEndTurnTimer);
                    DeferredEndTurnTimer = nullptr;
                }
                DeferredEndTurnTimer = ScheduleCallbackTimer(200, 200, pas::bind_method<&TfStarMap::DeferredEndTurn>(this), 0);
            }
        }
    }

    void TfStarMap::MapRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        std::int32_t Index{};
        std::int32_t FollowMode{};
        aShip::TShip* Ship{};
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(Globals::ShipScreen->ReopenRequested ^ 1)) {
            HideLargeHelp();
            ScannerSelectionActive = false;
            TalkSelectionActive = false;
            InterceptorSelectionActive = false;
            CustomSelectionActive = false;
            for (Index = 0; Index <= 4; ++Index) {
                SelectedWeapons[Index] = false;
            }
            CursorObject = FindObjectAtCursor();
            if (CursorObject == aPlayer::GetPlayer()) {
                ShipToInspect = aPlayer::GetPlayer();
                ShipClicked(nullptr);
            } else if (aTranclucator::TTranclucator* tranclucator = pas::class_cast_if<aTranclucator::TTranclucator*>(CursorObject); tranclucator != nullptr && tranclucator->OwnerShip == aPlayer::GetPlayer()) {
                ShipToInspect = static_cast<aShip::TShip*>(tranclucator);
                ShipClicked(nullptr);
            } else if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(CursorObject); ship != nullptr && !(pas::class_cast_if<aRuins::TRuins*>(CursorObject) != nullptr)) {
                Ship = ship;
                if (Ship != aPlayer::GetPlayer() && static_cast<std::uint8_t>(ScannerSelectionActive ^ 1) && static_cast<std::uint8_t>(TalkSelectionActive ^ 1)) {
                    if (GlobalsV::RightClickOnShip == 0 && (aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetScanner()) && aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetRadar()) || aGalaxy::Galaxy->UltraScanModEnabled == 1)) {
                        if (([&] {
                            pas::Extended cpp_left = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                            return cpp_left <= aPlayer::GetPlayer()->GetRadarRange();
                        }()) && Ship != aPlayer::GetPlayer() && (static_cast<void>(aPlayer::GetPlayer()), aPlayer::TPlayer::CanScanShip(Ship)) && (aGalaxy::Galaxy->UltraScanModEnabled != 0 || aPlayer::GetPlayer()->CanResolveObjectWithScanner(Ship) || Ship->TypeId == aGalaxyStruct::stTranclucator)) {
                            if (Ship->NoScan || aPlayer::GetPlayer()->ScanLocked) {
                                const pas::WideString& lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"Help.ScanImpossible"_wref.get());
                                TfStarMap* self = this;
                                self->ShowLargeHelp(lookupLocalizedTextByKey);
                            } else {
                                HideLargeHelp();
                                ScannerSelectionActive = false;
                                TalkSelectionActive = false;
                                HideActionRanges();
                                ShowObjectInfo(nullptr);
                                GR_Main::SoundManager->PlaySound(u"Sound.Scan"_wref.get());
                                SetCursorActive(false);
                                Present();
                                GR_Main::CaptureScreenBackground(true, 0);
                                SetCursorActive(true);
                                Globals::ScannerTarget = CursorObject;
                                GlobalsV::ScannerReturnScreenId = GlobalsV::FormToId(this);
                                GlobalsV::RequestedScreenId = GlobalsV::screenScanner;
                                RequestClose(1);
                            }
                        } else if (CursorObject == aPlayer::GetPlayer()) {
                            // Kept by the native routine even after the earlier player-object branch.
                            ShipToInspect = aPlayer::GetPlayer();
                            ShipClicked(nullptr);
                        } else {
                            pas::Extended cpp_left_2 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                            if (cpp_left_2 > aPlayer::GetPlayer()->GetRadarRange()) {
                                const pas::WideString& lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"Help.ScanOutRange"_wref.get());
                                TfStarMap* self_2 = this;
                                self_2->ShowLargeHelp(lookupLocalizedTextByKey_2);
                            } else if (CursorObject != aPlayer::GetPlayer() && !(pas::class_cast_if<aRuins::TRuins*>(CursorObject) != nullptr) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->CanResolveObjectWithScanner(CursorObject) ^ 1) && Ship->TypeId != aGalaxyStruct::stTranclucator) {
                                const pas::WideString& lookupLocalizedTextByKey_3 = GR_Main::LookupLocalizedTextByKey(u"Help.ScanPowerLow"_wref.get());
                                TfStarMap* self_3 = this;
                                self_3->ShowLargeHelp(lookupLocalizedTextByKey_3);
                            }
                        }
                    } else if (GlobalsV::RightClickOnShip == 1 && aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetRadar())) {
                        if (([&] {
                            pas::Extended cpp_left_3 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                            return cpp_left_3 <= aPlayer::GetPlayer()->GetRadarRange();
                        }()) && CursorObject != aPlayer::GetPlayer() && !(pas::class_cast_if<aKling::TKling*>(CursorObject) != nullptr && static_cast<aKling::TKling*>(CursorObject)->IsProgramActive(aGalaxyStruct::prgDisconnection))) {
                            if (Ship->NoTalk || aPlayer::GetPlayer()->TalkLocked) {
                                const pas::WideString& lookupLocalizedTextByKey_4 = GR_Main::LookupLocalizedTextByKey(u"Help.TalkImpossible"_wref.get());
                                TfStarMap* self_4 = this;
                                self_4->ShowLargeHelp(lookupLocalizedTextByKey_4);
                            } else {
                                HideLargeHelp();
                                Globals::TalkShip = Ship;
                                Globals::TalkScripted = false;
                                Globals::ScriptDialogIndex = -1;
                                aGalaxy::Galaxy->CheckIntegrityChecksum(73);
                                RunTalkDialogs();
                                ClearPathOverlay(true);
                                aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                                aGalaxy::Galaxy->PrimeIntegrityChecksum(74);
                                BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                                GI_Main::BreakUiMessage();
                            }
                        } else {
                            pas::Extended cpp_left_4 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                            if (cpp_left_4 > aPlayer::GetPlayer()->GetRadarRange()) {
                                const pas::WideString& lookupLocalizedTextByKey_5 = GR_Main::LookupLocalizedTextByKey(u"Help.TalkOutRange"_wref.get());
                                TfStarMap* self_5 = this;
                                self_5->ShowLargeHelp(lookupLocalizedTextByKey_5);
                            }
                        }
                    } else if (GlobalsV::RightClickOnShip == 2) {
                        ClearPathOverlay(true);
                        FollowMode = 0;
                        if (aRanger::PendingPlayerFollowTarget == Ship) {
                            FollowMode = 1;
                        } else if (aPlayer::GetPlayer()->Order == aShip::soFollowShip && aPlayer::GetPlayer()->OrderTarget == Ship) {
                            if (static_cast<aShip::TFollowMode>(static_cast<std::uint8_t>(aPlayer::GetPlayer()->OrderStateData)) == aShip::fmMinWeaponRange && aPlayer::GetPlayer()->CanSelectShipTarget(Ship)) {
                                FollowMode = 3;
                            } else {
                                FollowMode = 2;
                            }
                        }
                        if (FollowMode == 0) {
                            FollowMode = GlobalsV::DefaultOrder;
                            if (FollowMode < 0 || FollowMode > 3) {
                                FollowMode = 0;
                            }
                            if (GlobalsV::DefaultOrder == 0) {
                                if (static_cast<std::uint8_t>(aShip::TShip_GetRelationLevelToShip(Ship, aPlayer::GetPlayer())) <= 0) {
                                    FollowMode = 1;
                                } else {
                                    FollowMode = 2;
                                }
                            }
                        } else {
                            ++FollowMode;
                            if (FollowMode > 3) {
                                FollowMode = 1;
                            }
                        }
                        if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->CanSelectShipTarget(Ship) ^ 1) && FollowMode == 3) {
                            FollowMode = 1;
                        }
                        aGalaxy::Galaxy->CheckIntegrityChecksum(75);
                        if (FollowMode == 1) {
                            if (!aPlayer::GetPlayer()->CanSelectShipTarget(pas::checked_cast<aShip::TShip*>(CursorObject))) {
                                FollowMode = 2;
                            }
                        }
                        if (FollowMode == 1) {
                            aPlayer::GetPlayer()->OrderNone(false);
                            aRanger::PendingPlayerFollowTarget = pas::checked_cast<aShip::TShip*>(CursorObject);
                            {
                                const pas::WideString& lookupLocalizedTextByKey_6 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveAuto"_wref.get());
                                TfStarMap* self_6 = this;
                                self_6->ShowLargeHelp(lookupLocalizedTextByKey_6);
                            }
                        } else if (FollowMode == 2) {
                            aRanger::PendingPlayerFollowTarget = nullptr;
                            aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmFollowNear, false);
                            {
                                const pas::WideString& lookupLocalizedTextByKey_7 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveNear"_wref.get());
                                TfStarMap* self_7 = this;
                                self_7->ShowLargeHelp(lookupLocalizedTextByKey_7);
                            }
                        } else if (FollowMode == 3) {
                            aRanger::PendingPlayerFollowTarget = nullptr;
                            aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmMinWeaponRange, false);
                            {
                                const pas::WideString& lookupLocalizedTextByKey_8 = GR_Main::LookupLocalizedTextByKey(u"Help.MoveShot"_wref.get());
                                TfStarMap* self_8 = this;
                                self_8->ShowLargeHelp(lookupLocalizedTextByKey_8);
                            }
                        }
                        aGalaxy::Galaxy->PrimeIntegrityChecksum(76);
                        BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                    }
                }
            } else if (aPlanet::TPlanet* planet = pas::class_cast_if<aPlanet::TPlanet*>(CursorObject); planet != nullptr && pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(planet->OwnerId) && planet->CurrentStar->Status.CustomFaction == u"" && ([&] {
                pas::Extended cpp_left_5 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, planet->GetPosition());
                return cpp_left_5 <= aPlayer::GetPlayer()->GetRadarRange();
            }()) || pas::class_cast_if<aRuins::TRuins*>(CursorObject) != nullptr && static_cast<aRuins::TRuins*>(CursorObject)->virtual_TShip_CanDock(aPlayer::GetPlayer()) && static_cast<std::uint8_t>(pas::checked_cast<aRuins::TRuins*>(CursorObject)->NoTalk ^ 1) && ([&] {
                pas::Extended cpp_left_6 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, static_cast<aRuins::TRuins*>(CursorObject)->Position);
                return cpp_left_6 <= aPlayer::GetPlayer()->GetRadarRange();
            }())) {
                {
                    const pas::WideString& priceSnapshotKey = TfStarMap::GetPriceSnapshotKey(CursorObject);
                    const pas::WideString& buildPriceText = fGoodsShop2::TfGoodsShop2::BuildPriceText(CursorObject);
                    std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                    Globals::AddOrUpdatePlayerBubble(Globals::pmUserNote, currentTurn, buildPriceText, priceSnapshotKey);
                }
                GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
                MainPanel->RebuildMessageButtons(false);
            }
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
        }
    }

    void TfStarMap::MapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(Globals::ShipScreen->ReopenRequested ^ 1)) {
            RefreshActionRanges();
            if (TypesImports::PtInRect(GR_Main::ScrollInteriorRect, Point)) {
                UpdateActionCursor(false);
            }
            if (!Sender->IsOccludedAtPoint(Point)) {
                if ((KeyState & WindowsSdk::MK_RBUTTON) == WindowsSdk::MK_RBUTTON) {
                    ShowObjectInfo(FindObjectAtCursor());
                } else if (!TypesImports::PtInRect(GR_Main::ScrollInteriorRect, Point)) {
                    if (!IsCursorImageSelected(u"Scroll"sv)) {
                        SetCursorByName(u"Scroll"_wref.get());
                    }
                    ShowObjectInfo(nullptr);
                } else if (static_cast<std::uint8_t>(ScrollLeftHeld ^ 1) && static_cast<std::uint8_t>(ScrollRightHeld ^ 1) && static_cast<std::uint8_t>(ScrollUpHeld ^ 1) && static_cast<std::uint8_t>(ScrollDownHeld ^ 1)) {
                    ShowObjectInfo(FindObjectAtCursor());
                } else if (!IsCursorImageSelected(u"Main"sv)) {
                    SetCursorByName(u"Main"_wref.get());
                }
            }
        }
    }

    void TfStarMap::OrderKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        std::int32_t Index{};
        aItem::TWeapon* Weapon{};
        std::uint8_t CanAfterburn{};
        aScript::TScriptShip* Binding{};
        if (static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(Globals::ShipScreen->ReopenRequested ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            DisplayedObject = nullptr;
            if (Key == 'I') {
                ScannerClicked(nullptr);
            } else if (Key == 'T') {
                TalkClicked(nullptr);
            } else if (Key >= '1' && Key < '6') {
                Index = Key - '1';
                aGalaxy::Galaxy->CheckIntegrityChecksum(77);
                TalkSelectionActive = false;
                ScannerSelectionActive = false;
                InterceptorSelectionActive = false;
                CustomSelectionActive = false;
                Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                if (static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Weapon) ^ 1) || pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Ammo <= 0) {
                    SelectedWeapons[Index] = false;
                } else {
                    Weapon->Target = nullptr;
                    SelectedWeapons[Index] = static_cast<std::uint8_t>(SelectedWeapons[Index] ^ 1);
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(78);
                RebuildTargetMarkers();
                RefreshActionRanges();
                RefreshWeaponButtons();
                UpdateActionCursor(false);
            } else if (Key == WindowsSdk::VK_OEM_3) {
                TalkSelectionActive = false;
                ScannerSelectionActive = false;
                InterceptorSelectionActive = false;
                CustomSelectionActive = false;
                SelectAllUsableWeapons();
                RebuildTargetMarkers();
                RefreshActionRanges();
                RefreshWeaponButtons();
                UpdateActionCursor(false);
            } else if (Key == 'B') {
                if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull()->CapitalShip > 0) {
                    aGalaxy::Galaxy->CheckIntegrityChecksum(401);
                    aPlayer::GetPlayer()->EnterRuinsMode(0);
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(402);
                }
            } else if (Key == 'N') {
                if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull()->InterceptorsEnabled) {
                    SelectInterceptorTarget();
                }
            } else if (Key == WindowsSdk::VK_OEM_PLUS || Key == WindowsSdk::VK_ADD) {
                SelectUntargetedWeapons();
                RebuildTargetMarkers();
                RefreshActionRanges();
                RefreshWeaponButtons();
                UpdateActionCursor(false);
            } else if (Key == WindowsSdk::VK_SUBTRACT) {
                aGalaxy::Galaxy->CheckIntegrityChecksum(79);
                for (Index = 0; Index <= 4; ++Index) {
                    SelectedWeapons[Index] = false;
                    Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                    if (Weapon != nullptr) {
                        Weapon->Target = nullptr;
                        ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.ShotCancel"_wref.get()));
                    }
                }
                aGalaxy::Galaxy->PrimeIntegrityChecksum(80);
                RebuildTargetMarkers();
                RefreshActionRanges();
                RefreshWeaponButtons();
                UpdateActionCursor(false);
            } else if (Key == 'S') {
                if (aGalaxy::Galaxy->SpecialSimulationMode == 0) {
                    ShipToInspect = aPlayer::GetPlayer();
                    ShipClicked(nullptr);
                }
            } else if (Key == 'M') {
                if (aGalaxy::Galaxy->SpecialSimulationMode == 0) {
                    GalaxyClicked(nullptr);
                }
            } else if (Key == 'R') {
                MainPanel->QuestClicked(nullptr);
            } else if (Key == WindowsSdk::VK_ESCAPE) {
                MainPanel->MenuClicked(nullptr);
            } else if (Key == 'H') {
                OpenFilmHistoryClicked(nullptr);
            } else if (Key == 'W') {
                ToggleWeaponPanelClicked(nullptr);
            } else if (Key == 'F' && Mode == smmOrders) {
                CanAfterburn = aPlayer::GetPlayer()->GetSlotCount(aConst::sskAfterburner) > 0 && aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetEngine()) && aPlayer::GetPlayer()->InNormalSpace();
                if (static_cast<std::uint8_t>(aPlayer::GetPlayer()->AfterburnerActive ^ 1) && CanAfterburn) {
                    GR_Main::SoundManager->PlaySound(u"Sound.ForsageOn"_wref.get());
                    ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.ForsageOn"_wref.get()));
                    ClearPathOverlay(true);
                    aGalaxy::Galaxy->CheckIntegrityChecksum(81);
                    aPlayer::GetPlayer()->AfterburnerActive = true;
                    aPlayer::GetPlayer()->RefreshDerivedStats(true);
                    aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                    if (aPlayer::GetPlayer()->ScriptShipBindings != nullptr) {
                        Index = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
                        while (Index >= 0) {
                            if (Index >= pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings)) {
                                Index = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
                            } else {
                                Binding = pas::list_at<aScript::TScriptShip>(aPlayer::GetPlayer()->ScriptShipBindings, Index);
                                if (Binding->Script != nullptr) {
                                    aScript::TScript_RunShipState(Binding->Script, Binding);
                                }
                                --Index;
                            }
                        }
                    }
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(82);
                    BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                } else if (aPlayer::GetPlayer()->AfterburnerActive) {
                    GR_Main::SoundManager->PlaySound(u"Sound.ForsageOff"_wref.get());
                    ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.ForsageOff"_wref.get()));
                    ClearPathOverlay(true);
                    aGalaxy::Galaxy->CheckIntegrityChecksum(83);
                    aPlayer::GetPlayer()->AfterburnerActive = false;
                    aPlayer::GetPlayer()->RefreshDerivedStats(true);
                    aPlayer::GetPlayer()->BuildOrderMovementPath(aGalaxyStruct::FullPathNodeLimit);
                    if (aPlayer::GetPlayer()->ScriptShipBindings != nullptr) {
                        Index = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
                        while (Index >= 0) {
                            if (Index >= pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings)) {
                                Index = pas::list_count(aPlayer::GetPlayer()->ScriptShipBindings) - 1;
                            } else {
                                Binding = pas::list_at<aScript::TScriptShip>(aPlayer::GetPlayer()->ScriptShipBindings, Index);
                                if (Binding->Script != nullptr) {
                                    aScript::TScript_RunShipState(Binding->Script, Binding);
                                }
                                --Index;
                            }
                        }
                    }
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(84);
                    BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
                }
            }
        }
    }

    void TfStarMap::OrderKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        // The native body retains these guard reads despite having no guarded action.
        static_cast<void>(static_cast<std::uint8_t>(MainPanel->NavigationLocked ^ 1) && static_cast<std::uint8_t>(Globals::ShipScreen->ReopenRequested ^ 1));
    }

    void TfStarMap::SelectAllUsableWeapons() {
        std::int32_t Index{};
        aItem::TWeapon* Weapon{};
        aGalaxy::Galaxy->CheckIntegrityChecksum(85);
        for (Index = 0; Index <= 4; ++Index) {
            Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
            if (static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Weapon) ^ 1) || pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Ammo <= 0) {
                SelectedWeapons[Index] = false;
            } else {
                Weapon->Target = nullptr;
                SelectedWeapons[Index] = true;
            }
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(86);
    }

    void TfStarMap::SelectUntargetedWeapons() {
        std::int32_t Index{};
        aItem::TWeapon* Weapon{};
        for (Index = 0; Index <= 4; ++Index) {
            Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
            if (static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Weapon) ^ 1) || pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Ammo <= 0) {
                SelectedWeapons[Index] = false;
            } else if (Weapon->Target == nullptr) {
                SelectedWeapons[Index] = true;
            }
        }
    }

    // Nil hides the object panels. Accepts game objects, not scene objects.
    void TfStarMap::ShowObjectInfo(pas::Object* Obj) {
        static const pas::Set<0, 255> WearableItemTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        GI_Panel::TPanelGI* Panel{};
        pas::List* Objects{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t RowHeight{};
        std::int32_t RowX{};
        std::uint32_t IconInset{};
        std::int32_t NameWidth{};
        std::int32_t DetailWidth{};
        std::int32_t StatusCount{};
        float Distance{};
        aGalaxyStruct::TOwnerId OwnerId{};
        pas::WideString ImagePath{};
        pas::WideString Text{};
        pas::WideString ColorTag{};
        GI_MessageLoop::TObjectGI* Child{};
        GI_Label::TLabelGI* DamageName{};
        GI_Label::TLabelGI* DamageValue{};
        aItem::TItem* ItemObject{};
        GI_MessageLoop::TObjectGI* ActivePanel{};
        std::int32_t BarWidth{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        aGalaxy::TCustomSystemInfo* CustomInfo{};
        pas::WideString Images{};
        if (pas::class_cast_if<aGalaxy::TStar*>(Obj) != nullptr && aKling::TerronShip != nullptr && aKling::TerronShip->CurrentStar == Obj && aGalaxy::Galaxy->TerronToStarTurn >= aGalaxyStruct::TerronTransformationFlag) {
            Obj = aKling::TerronShip;
        }
        if (aKling::TerronShip != nullptr && Obj == aKling::TerronShip && aGalaxy::Galaxy->TerronToStarTurn >= aGalaxyStruct::TerronTransformationFlag) {
            std::int64_t cpp_left_2 = System::Round(aKling::TerronShip->CurrentStar->Graphic->Position.Y);
            std::int32_t cpp_arg = cpp_left_2 - GetMapCenter().Y;
            std::int64_t cpp_left = System::Round(aKling::TerronShip->CurrentStar->Graphic->Position.X);
            std::int32_t cpp_arg_2 = cpp_left - GetMapCenter().X;
            HitObjectPosition = ClassesImports::Point(cpp_arg_2, cpp_arg);
        }
        if (Obj != nullptr && pas::class_cast_if<aAsteroid::TAsteroid*>(Obj) != nullptr) {
            ShowAsteroidPath(static_cast<aAsteroid::TAsteroid*>(Obj));
        } else {
            ClearAsteroidPath();
        }
        if (Obj == nullptr || DisplayedObject != Obj) {
            if (Obj != nullptr && pas::class_cast_if<aShip::TShip*>(Obj) != nullptr) {
                static_cast<aShip::TShip*>(Obj)->ScriptItemsAct(aGalaxyStruct::satOnShowingShipInfo, nullptr, nullptr, 0);
            }
            if (Obj != nullptr && pas::class_cast_if<aGalaxy::TStar*>(Obj) != nullptr && aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->ScriptItemsAct(aGalaxyStruct::satOnShowingStarInfo, Obj, nullptr, 0);
            }
            if (Obj == nullptr) {
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(false);
                ClearAsteroidPath();
                ClearPathOverlay(false);
                DisplayedObject = nullptr;
            } else if (aPlayer::GetPlayer() == nullptr || pas::class_cast_if<aItem::TItem*>(Obj) != nullptr && ([&] {
                pas::Extended cpp_left_3 = aMyFunction::PointDistance(static_cast<aItem::TItem*>(Obj)->Position, aPlayer::GetPlayer()->Position);
                return cpp_left_3 > aPlayer::GetPlayer()->GetRadarRange();
            }()) || pas::class_cast_if<aShip::TShip*>(Obj) != nullptr && (aPlayer::GetPlayer()->CurrentStar != static_cast<aShip::TShip*>(Obj)->CurrentStar || static_cast<aShip::TShip*>(Obj)->InHyperspace && Obj != aKling::TerronShip || ([&] {
                pas::Extended cpp_left_4 = aMyFunction::PointDistance(static_cast<aShip::TShip*>(Obj)->Position, aPlayer::GetPlayer()->Position);
                return cpp_left_4 > aPlayer::GetPlayer()->GetRadarRange();
            }())) || pas::class_cast_if<aAsteroid::TAsteroid*>(Obj) != nullptr && ([&] {
                pas::Extended cpp_left_5 = aMyFunction::PointDistance(static_cast<aAsteroid::TAsteroid*>(Obj)->Position, aPlayer::GetPlayer()->Position);
                return cpp_left_5 > aPlayer::GetPlayer()->GetRadarRange();
            }()) || pas::class_cast_if<aMissile::TMissile*>(Obj) != nullptr && ([&] {
                pas::Extended cpp_left_6 = aMyFunction::PointDistance(static_cast<aMissile::TMissile*>(Obj)->Position, aPlayer::GetPlayer()->Position);
                return cpp_left_6 > aPlayer::GetPlayer()->GetRadarRange();
            }()) || pas::class_cast_if<aGalaxy::THole*>(Obj) != nullptr || pas::class_cast_if<aPlanet::TPlanet*>(Obj) != nullptr) {
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(true);
                if (aItem::TItem* item = pas::class_cast_if<aItem::TItem*>(Obj)) {
                    if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
                        ItemObject = item;
                        if (ItemObject->ScriptItem != nullptr) {
                            reinterpret_cast<aScript::TScriptItem*>(ItemObject->ScriptItem)->RunActionCode(aGalaxyStruct::satOnShowingItemInfo, nullptr, aGalaxy::PlayerStar, nullptr, 0);
                        }
                        if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(ItemObject) != nullptr) {
                            aScript::RunItemConfigActionCode(ItemObject, aGalaxyStruct::satOnShowingItemInfo, nullptr, aGalaxy::PlayerStar, nullptr, 0);
                        }
                    }
                    GetByName(u"InfoStdGB"sv)->SetActive(false);
                    {
                        GI_Image::TImageGI* InfoStdImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoStdImage"sv));
                        InfoStdImage->SetActive(true);
                        if (aItem::TGoods* goods = pas::class_cast_if<aItem::TGoods*>(Obj)) {
                            InfoStdImage->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aItem::TItem*>(goods)->ItemType)}));
                        } else {
                            InfoStdImage->SetImagePath(pas::concat_wide({u"GI,", reinterpret_cast<aItem::TItem*>(Obj)->GetBitmapResourceName(), u"s"}));
                        }
                        InfoStdImage->SetImageKindX(GI_Main::ikxCenter);
                        InfoStdImage->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter = InfoStdImage->GetVisualCenter();
                            Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                            InfoStdImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                        }
                    }
                    if (aItem::TGoods* goods_2 = pas::class_cast_if<aItem::TGoods*>(Obj)) {
                        {
                            const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[static_cast<aItem::TItem*>(goods_2)->ItemType].DisplayName), pas::view(aMyFunction::InfoNameColorTag));
                            GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            cpp_arg_3->SetText(wrapTextInColor);
                        }
                        {
                            const pas::WideString& localizedText = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text.", SysUtils::IntToStr(reinterpret_cast<aItem::TItem*>(Obj)->ItemType + 1)})));
                            GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                            cpp_arg_4->SetText(localizedText);
                        }
                    } else {
                        {
                            const pas::WideString& wrapTextInColor_2 = ([&] {
                                pas::WideString localizedText_2 = aConst::LocalizedText(u"FormInfo.ContainerName"_wref.get());
                                pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                                return aMyFunction::WrapTextInColor(pas::view(std::move(localizedText_2)), pas::view(std::move(infoNameColorTag)));
                            }());
                            GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            cpp_arg_5->SetText(wrapTextInColor_2);
                        }
                        {
                            const pas::WideString& localizedText_3 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                            GI_Label::TLabelGI* cpp_arg_6 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                            cpp_arg_6->SetText(localizedText_3);
                        }
                    }
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemSize"sv))->SetText(u"???"_wref.get());
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemPrice"sv))->SetText(u"???"_wref.get());
                    {
                        GI_Label::TLabelGI* cpp_arg_7 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_8 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_9 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_9, cpp_arg_8, cpp_arg_7, true, true, 0);
                    }
                } else if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Obj)) {
                    if (pas::class_cast_if<SE_Ship2::TShip2SE*>(ship->Graphic) != nullptr) {
                        GetByName(u"InfoStdImage"sv)->SetActive(false);
                        {
                            GI_GraphBuf::TGraphBufGI* InfoStdGB = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                            ImagePath = pas::checked_cast<aShip::TShip*>(Obj)->GetShipPortraitImagePath();
                            InfoStdGB->SetActive(ImagePath != u"");
                            if (InfoStdGB->Active) {
                                InfoStdGB->SourceHasPerPixelAlpha = true;
                                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 1, u","sv), InfoStdGB->GraphBuf);
                                if (InfoStdGB->ClientSize.X < InfoStdGB->GraphBuf->Width || InfoStdGB->ClientSize.Y < InfoStdGB->GraphBuf->Height) {
                                    if (static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Height)) {
                                        InfoStdGB->GraphBuf->RescaleRgba(InfoStdGB->ClientSize.X, System::Round(pas::real_divide(InfoStdGB->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Height)), 5);
                                    } else {
                                        InfoStdGB->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Width)), InfoStdGB->ClientSize.Y, 5);
                                    }
                                }
                                InfoStdGB->SetImageKindX(GI_Main::ikxCenter);
                                InfoStdGB->SetImageKindY(GI_Main::ikyCenter);
                                {
                                    Types::TPoint visualCenter_2 = InfoStdGB->GetVisualCenter();
                                    Types::TPoint itemImageCenter_2 = Globals::ShipScreen->ItemImageCenter;
                                    InfoStdGB->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_2, visualCenter_2));
                                }
                            }
                        }
                    } else {
                        GetByName(u"InfoStdImage"sv)->SetActive(false);
                        {
                            GI_GraphBuf::TGraphBufGI* InfoStdGB_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                            InfoStdGB_2->SetActive(true);
                            InfoStdGB_2->SourceHasPerPixelAlpha = true;
                            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::checked_cast<aShip::TShip*>(Obj)->Graphic)->StaticImagePath), 1, u","sv), InfoStdGB_2->GraphBuf);
                            if (InfoStdGB_2->ClientSize.X < InfoStdGB_2->GraphBuf->Width || InfoStdGB_2->ClientSize.Y < InfoStdGB_2->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Height)) {
                                    InfoStdGB_2->GraphBuf->RescaleRgba(InfoStdGB_2->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_2->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Height)), 5);
                                } else {
                                    InfoStdGB_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_2->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Width)), InfoStdGB_2->ClientSize.Y, 5);
                                }
                            }
                            {
                                Types::TPoint visualCenter_3 = InfoStdGB_2->GetVisualCenter();
                                Types::TPoint itemImageCenter_3 = Globals::ShipScreen->ItemImageCenter;
                                InfoStdGB_2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_3, visualCenter_3));
                            }
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor_3 = ([&] {
                            pas::WideString fullName = pas::checked_cast<aShip::TShip*>(Obj)->GetFullName(u" "_wref.get());
                            pas::WideString infoNameColorTag_2 = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(fullName)), pas::view(std::move(infoNameColorTag_2)));
                        }());
                        GI_Label::TLabelGI* cpp_arg_10 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        cpp_arg_10->SetText(wrapTextInColor_3);
                    }
                    if (aShip::TShip* ship_2 = pas::class_cast_if<aShip::TShip*>(Obj)) {
                        if (ship_2->PartnerShip == aPlayer::GetPlayer()) {
                            const pas::WideString& cpp_arg_11 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv))->GetText(), u"\r\n", ([&] {
                                pas::WideString lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"FormInfo.Partner"_wref.get());
                                pas::WideString textHighlightColorTag = aMyFunction::TextHighlightColorTag;
                                return aMyFunction::WrapTextInColor(pas::view(std::move(lookupLocalizedTextByKey)), pas::view(std::move(textHighlightColorTag)));
                            }())});
                            GI_Label::TLabelGI* cpp_arg_12 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            cpp_arg_12->SetText(cpp_arg_11);
                        }
                    }
                    {
                        const pas::WideString& localizedText_4 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                        GI_Label::TLabelGI* cpp_arg_13 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_13->SetText(localizedText_4);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_14 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_15 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_16 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_16, cpp_arg_15, cpp_arg_14, true, true, 0);
                    }
                } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(Obj) != nullptr) {
                    GetByName(u"InfoStdImage"sv)->SetActive(false);
                    {
                        GI_GraphBuf::TGraphBufGI* InfoStdGB_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                        InfoStdGB_3->SetActive(true);
                        InfoStdGB_3->SourceHasPerPixelAlpha = true;
                        GI_GAI::LoadGaiFrameToGraphBuf(reinterpret_cast<SE_Asteroid::TAsteroidSE*>(reinterpret_cast<aAsteroid::TAsteroid*>(Obj)->GraphObject)->ImagePath, InfoStdGB_3->GraphBuf, reinterpret_cast<aAsteroid::TAsteroid*>(Obj)->Id);
                        if (InfoStdGB_3->ClientSize.X < InfoStdGB_3->GraphBuf->Width || InfoStdGB_3->ClientSize.Y < InfoStdGB_3->GraphBuf->Height) {
                            if (static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Height)) {
                                InfoStdGB_3->GraphBuf->RescaleRgba(InfoStdGB_3->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_3->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Height)), 5);
                            } else {
                                InfoStdGB_3->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_3->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Width)), InfoStdGB_3->ClientSize.Y, 5);
                            }
                        }
                        {
                            Types::TPoint visualCenter_4 = InfoStdGB_3->GetVisualCenter();
                            Types::TPoint itemImageCenter_4 = Globals::ShipScreen->ItemImageCenter;
                            InfoStdGB_3->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_4, visualCenter_4));
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor_4 = ([&] {
                            pas::WideString displayName = pas::checked_cast<aAsteroid::TAsteroid*>(Obj)->GetDisplayName();
                            pas::WideString infoNameColorTag_3 = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(displayName)), pas::view(std::move(infoNameColorTag_3)));
                        }());
                        GI_Label::TLabelGI* cpp_arg_17 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        cpp_arg_17->SetText(wrapTextInColor_4);
                    }
                    {
                        const pas::WideString& localizedText_5 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                        GI_Label::TLabelGI* cpp_arg_18 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_18->SetText(localizedText_5);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_19 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_20 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_21 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_21, cpp_arg_20, cpp_arg_19, true, true, 0);
                    }
                } else if (pas::class_cast_if<aMissile::TMissile*>(Obj) != nullptr) {
                    GetByName(u"InfoStdImage"sv)->SetActive(false);
                    {
                        GI_GraphBuf::TGraphBufGI* InfoStdGB_4 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                        InfoStdGB_4->SetActive(true);
                        InfoStdGB_4->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.Missile.w", pas::checked_cast<aMissile::TMissile*>(Obj)->GetGraphSuffix(), u"_", GR_Main::GiResourceSuffix(), u"i"}), InfoStdGB_4->GraphBuf);
                        {
                            Types::TPoint visualCenter_5 = InfoStdGB_4->GetVisualCenter();
                            Types::TPoint itemImageCenter_5 = Globals::ShipScreen->ItemImageCenter;
                            InfoStdGB_4->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_5, visualCenter_5));
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor_5 = ([&] {
                            pas::WideString displayName_2 = pas::checked_cast<aMissile::TMissile*>(Obj)->GetDisplayName();
                            pas::WideString infoNameColorTag_4 = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(displayName_2)), pas::view(std::move(infoNameColorTag_4)));
                        }());
                        GI_Label::TLabelGI* cpp_arg_22 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        cpp_arg_22->SetText(wrapTextInColor_5);
                    }
                    {
                        const pas::WideString& localizedText_6 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                        GI_Label::TLabelGI* cpp_arg_23 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_23->SetText(localizedText_6);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_24 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_25 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_26 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_26, cpp_arg_25, cpp_arg_24, true, true, 0);
                    }
                } else if (pas::class_cast_if<aGalaxy::THole*>(Obj) != nullptr) {
                    GetByName(u"InfoStdImage"sv)->SetActive(false);
                    {
                        GI_GraphBuf::TGraphBufGI* InfoStdGB_5 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                        InfoStdGB_5->SetActive(true);
                        InfoStdGB_5->SourceHasPerPixelAlpha = true;
                        GI_GAI::LoadGaiFrameToGraphBuf(reinterpret_cast<SE_Hole::THoleSE*>(reinterpret_cast<aGalaxy::THole*>(Obj)->Graphic)->ImagePath, InfoStdGB_5->GraphBuf, 32u);
                        if (InfoStdGB_5->ClientSize.X < InfoStdGB_5->GraphBuf->Width || InfoStdGB_5->ClientSize.Y < InfoStdGB_5->GraphBuf->Height) {
                            if (static_cast<std::uint32_t>(InfoStdGB_5->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_5->GraphBuf->Height)) {
                                InfoStdGB_5->GraphBuf->RescaleRgba(InfoStdGB_5->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_5->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_5->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_5->GraphBuf->Height)), 5);
                            } else {
                                InfoStdGB_5->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_5->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_5->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_5->GraphBuf->Width)), InfoStdGB_5->ClientSize.Y, 5);
                            }
                        }
                        {
                            Types::TPoint visualCenter_6 = InfoStdGB_5->GetVisualCenter();
                            Types::TPoint itemImageCenter_6 = Globals::ShipScreen->ItemImageCenter;
                            InfoStdGB_5->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_6, visualCenter_6));
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor_6 = ([&] {
                            pas::WideString localizedText_7 = aConst::LocalizedText(reinterpret_cast<SE_Hole::THoleSE*>(reinterpret_cast<aGalaxy::THole*>(Obj)->Graphic)->NameTextPath);
                            pas::WideString infoNameColorTag_5 = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(localizedText_7)), pas::view(std::move(infoNameColorTag_5)));
                        }());
                        GI_Label::TLabelGI* cpp_arg_27 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        cpp_arg_27->SetText(wrapTextInColor_6);
                    }
                    {
                        const pas::WideString& localizedText_8 = aConst::LocalizedText(reinterpret_cast<SE_Hole::THoleSE*>(reinterpret_cast<aGalaxy::THole*>(Obj)->Graphic)->InfoTextPath);
                        GI_Label::TLabelGI* cpp_arg_28 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_28->SetText(localizedText_8);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_29 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_30 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_31 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_31, cpp_arg_30, cpp_arg_29, true, true, 0);
                    }
                } else if (aPlanet::TPlanet* planet = pas::class_cast_if<aPlanet::TPlanet*>(Obj)) {
                    if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(planet->OwnerId) && static_cast<std::uint8_t>(planet->IsMainPiratePlanet ^ 1) && planet->CurrentStar->Status.CustomFaction == u"") {
                        if (DisplayedObject == Obj) {
                            return;
                        }
                        PlanetInfoPanel->SetActive(true);
                        StandardInfoPanel->SetActive(false);
                        {
                            const pas::WideString& wrapTextInColor_7 = aMyFunction::WrapTextInColor(pas::view(pas::checked_cast<aPlanet::TPlanet*>(Obj)->Name), pas::view(aMyFunction::InfoNameColorTag));
                            GI_Label::TLabelGI* cpp_arg_32 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetName"sv));
                            cpp_arg_32->SetText(wrapTextInColor_7);
                        }
                        if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(reinterpret_cast<aPlanet::TPlanet*>(Obj)->OwnerId) && reinterpret_cast<aPlanet::TPlanet*>(Obj)->CurrentStar->Status.CustomFaction == u"") {
                            GI_Image::TImageGI* InfoPlanetEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoPlanetEmRace"sv));
                            InfoPlanetEmRace->SetImagePath(aConst::GetFactionEmblemPath(pas::checked_cast<aPlanet::TPlanet*>(Obj)->GetFactionResourceName()));
                            InfoPlanetEmRace->SetImageKindX(GI_Main::ikxCenter);
                            InfoPlanetEmRace->SetImageKindY(GI_Main::ikyCenter);
                            InfoPlanetEmRace->SetActive(true);
                        } else {
                            GetByName(u"InfoPlanetEmRace"sv)->SetActive(false);
                        }
                        {
                            GI_GraphBuf::TGraphBufGI* InfoPlanetImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoPlanetImage"sv));
                            InfoPlanetImage->SourceHasPerPixelAlpha = true;
                            pas::checked_cast<aPlanet::TPlanet*>(Obj)->Graphic->RenderToBuffer(this, InfoPlanetImage->GraphBuf, false);
                            if (InfoPlanetImage->ClientSize.X < InfoPlanetImage->GraphBuf->Width || InfoPlanetImage->ClientSize.Y < InfoPlanetImage->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)) {
                                    InfoPlanetImage->GraphBuf->RescaleRgba(InfoPlanetImage->ClientSize.X, System::Round(pas::real_divide(InfoPlanetImage->ClientSize.X, static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)), 5);
                                } else {
                                    InfoPlanetImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoPlanetImage->ClientSize.Y, static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width)), InfoPlanetImage->ClientSize.Y, 5);
                                }
                            }
                        }
                        if (pas::checked_cast<aPlanet::TPlanet*>(Obj)->IsMainPiratePlanet) {
                            GI_Label::TLabelGI* cpp_arg_33 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetOwner"sv));
                            const pas::WideString& displayName_3 = aConst::OwnerInfo[static_cast<aPlanet::TPlanet*>(Obj)->OwnerId].DisplayName;
                            cpp_arg_33->SetText(displayName_3);
                        } else {
                            const pas::WideString& nativeRaceName = static_cast<aPlanet::TPlanet*>(Obj)->GetNativeRaceName();
                            GI_Label::TLabelGI* cpp_arg_34 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetOwner"sv));
                            cpp_arg_34->SetText(nativeRaceName);
                        }
                        {
                            const pas::WideString& int64ToStr = pas::wide_int64_to_str(System::Round(pas::real_divide(pas::checked_cast<aPlanet::TPlanet*>(Obj)->Population, 1.0E+3L)));
                            GI_Label::TLabelGI* cpp_arg_35 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetPop"sv));
                            cpp_arg_35->SetText(int64ToStr);
                        }
                        {
                            GI_Label::TLabelGI* cpp_arg_36 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetEco"sv));
                            const pas::WideString& displayName_4 = aConst::PlanetEconomyInfo[pas::checked_cast<aPlanet::TPlanet*>(Obj)->Economy].DisplayName;
                            cpp_arg_36->SetText(displayName_4);
                        }
                        {
                            const pas::WideString& governmentName = pas::checked_cast<aPlanet::TPlanet*>(Obj)->GetGovernmentName();
                            GI_Label::TLabelGI* cpp_arg_37 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetGov"sv));
                            cpp_arg_37->SetText(governmentName);
                        }
                        {
                            const pas::WideString& relationLevelTextToShip = pas::checked_cast<aPlanet::TPlanet*>(Obj)->GetRelationLevelTextToShip(aPlayer::GetPlayer());
                            GI_Label::TLabelGI* cpp_arg_38 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetRel"sv));
                            cpp_arg_38->SetText(relationLevelTextToShip);
                        }
                        {
                            GI_Label::TLabelGI* cpp_arg_39 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPOwner"sv));
                            GI_Label::TLabelGI* cpp_arg_40 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetOwner"sv));
                            GI_Label::TLabelGI* cpp_arg_41 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPPop"sv));
                            GI_Label::TLabelGI* cpp_arg_42 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetPop"sv));
                            GI_Label::TLabelGI* cpp_arg_43 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPEco"sv));
                            GI_Label::TLabelGI* cpp_arg_44 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetEco"sv));
                            GI_Label::TLabelGI* cpp_arg_45 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPGov"sv));
                            GI_Label::TLabelGI* cpp_arg_46 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetGov"sv));
                            GI_Label::TLabelGI* cpp_arg_47 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPRel"sv));
                            GI_Label::TLabelGI* cpp_arg_48 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetRel"sv));
                            GI_MessageLoop::TObjectGI* byName = GetByName(u"InfoPlanetEmRace"sv);
                            GI_Label::TLabelGI* cpp_arg_49 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetName"sv));
                            GI_Window::TWindowGI* cpp_arg_50 = pas::checked_cast<GI_Window::TWindowGI*>(PlanetInfoPanel);
                            Globals::ShipScreen->LayoutObjectInfo(cpp_arg_50, cpp_arg_49, cpp_arg_39, cpp_arg_40, cpp_arg_41, cpp_arg_42, cpp_arg_43, cpp_arg_44, cpp_arg_45, cpp_arg_46, cpp_arg_47, cpp_arg_48, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, byName, true, 0);
                        }
                    } else {
                        GetByName(u"InfoStdImage"sv)->SetActive(false);
                        {
                            GI_GraphBuf::TGraphBufGI* InfoStdGB_6 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                            InfoStdGB_6->SetActive(true);
                            InfoStdGB_6->SourceHasPerPixelAlpha = true;
                            pas::checked_cast<aPlanet::TPlanet*>(Obj)->Graphic->RenderToBuffer(this, InfoStdGB_6->GraphBuf, false);
                            if (InfoStdGB_6->ClientSize.X < InfoStdGB_6->GraphBuf->Width || InfoStdGB_6->ClientSize.Y < InfoStdGB_6->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Height)) {
                                    InfoStdGB_6->GraphBuf->RescaleRgba(InfoStdGB_6->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_6->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Height)), 5);
                                } else {
                                    InfoStdGB_6->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_6->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Width)), InfoStdGB_6->ClientSize.Y, 5);
                                }
                            }
                            {
                                Types::TPoint visualCenter_7 = InfoStdGB_6->GetVisualCenter();
                                Types::TPoint itemImageCenter_7 = Globals::ShipScreen->ItemImageCenter;
                                InfoStdGB_6->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_7, visualCenter_7));
                            }
                        }
                        {
                            const pas::WideString& wrapTextInColor_8 = aMyFunction::WrapTextInColor(pas::view(pas::checked_cast<aPlanet::TPlanet*>(Obj)->Name), pas::view(aMyFunction::InfoNameColorTag));
                            GI_Label::TLabelGI* cpp_arg_51 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            cpp_arg_51->SetText(wrapTextInColor_8);
                        }
                        {
                            const pas::WideString& infoText = pas::checked_cast<aPlanet::TPlanet*>(Obj)->GetInfoText(false);
                            GI_Label::TLabelGI* cpp_arg_52 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                            cpp_arg_52->SetText(infoText);
                        }
                        {
                            GI_Label::TLabelGI* cpp_arg_53 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                            GI_Label::TLabelGI* cpp_arg_54 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            GI_Window::TWindowGI* cpp_arg_55 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                            fShip2::TfShip2::LayoutItemInfo(cpp_arg_55, cpp_arg_54, cpp_arg_53, true, true, 0);
                        }
                    }
                }
                DisplayedObject = Obj;
            } else if (pas::class_cast_if<aItem::TItem*>(Obj) != nullptr) {
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(true);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(false);
                if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
                    ItemObject = pas::checked_cast<aItem::TItem*>(Obj);
                    if (ItemObject->ScriptItem != nullptr) {
                        reinterpret_cast<aScript::TScriptItem*>(ItemObject->ScriptItem)->RunActionCode(aGalaxyStruct::satOnShowingItemInfo, nullptr, aGalaxy::PlayerStar, nullptr, 0);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(ItemObject) != nullptr) {
                        aScript::RunItemConfigActionCode(ItemObject, aGalaxyStruct::satOnShowingItemInfo, nullptr, aGalaxy::PlayerStar, nullptr, 0);
                    }
                }
                {
                    GI_Image::TImageGI* InfoItemImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoItemImage"sv));
                    if (aItem::TGoods* goods_3 = pas::class_cast_if<aItem::TGoods*>(Obj)) {
                        InfoItemImage->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(static_cast<aItem::TItem*>(goods_3)->ItemType)}));
                    } else {
                        InfoItemImage->SetImagePath(pas::concat_wide({u"GI,", reinterpret_cast<aItem::TItem*>(Obj)->GetBitmapResourceName(), u"s"}));
                    }
                    InfoItemImage->SetImageKindX(GI_Main::ikxCenter);
                    InfoItemImage->SetImageKindY(GI_Main::ikyCenter);
                    {
                        Types::TPoint visualCenter_8 = InfoItemImage->GetVisualCenter();
                        Types::TPoint itemImageCenter_8 = Globals::ShipScreen->ItemImageCenter;
                        InfoItemImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_8, visualCenter_8));
                    }
                }
                if (aItem::TGoods* goods_4 = pas::class_cast_if<aItem::TGoods*>(Obj)) {
                    {
                        const pas::WideString& wrapTextInColor_9 = aMyFunction::WrapTextInColor(pas::view(aConst::GoodsMarket[static_cast<aItem::TItem*>(goods_4)->ItemType].DisplayName), pas::view(aMyFunction::InfoNameColorTag));
                        GI_Label::TLabelGI* cpp_arg_56 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv));
                        cpp_arg_56->SetText(wrapTextInColor_9);
                    }
                    {
                        const pas::WideString& localizedText_9 = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text.", SysUtils::IntToStr(reinterpret_cast<aItem::TItem*>(Obj)->ItemType + 1)})));
                        GI_Label::TLabelGI* cpp_arg_57 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"sv));
                        cpp_arg_57->SetText(localizedText_9);
                    }
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv))->SetText(u""_wref.get());
                    {
                        const pas::WideString& wrapTextInColor_10 = ([&] {
                            pas::WideString displayName_5 = reinterpret_cast<aItem::TItem*>(Obj)->GetDisplayName();
                            pas::WideString infoNameColorTag_6 = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(displayName_5)), pas::view(std::move(infoNameColorTag_6)));
                        }());
                        GI_Label::TLabelGI* cpp_arg_58 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv));
                        cpp_arg_58->SetText(wrapTextInColor_10);
                    }
                    {
                        const pas::WideString& infoText_2 = reinterpret_cast<aItem::TItem*>(Obj)->virtual_TItem_GetInfoText(aMyFunction::TextHighlightColorTag, nullptr);
                        GI_Label::TLabelGI* cpp_arg_59 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"sv));
                        cpp_arg_59->SetText(infoText_2);
                    }
                }
                {
                    const pas::WideString& intToStr = pas::wide_int_to_str(reinterpret_cast<aItem::TItem*>(Obj)->Weight);
                    GI_Label::TLabelGI* cpp_arg_60 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemSize"sv));
                    cpp_arg_60->SetText(intToStr);
                }
                {
                    const pas::WideString& intToStr_2 = pas::wide_int_to_str(reinterpret_cast<aItem::TItem*>(Obj)->Cost);
                    GI_Label::TLabelGI* cpp_arg_61 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemPrice"sv));
                    cpp_arg_61->SetText(intToStr_2);
                }
                {
                    GI_Image::TImageGI* InfoItemEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoItemEmRace"sv));
                    InfoItemEmRace->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(pas::checked_cast<aItem::TItem*>(Obj))));
                    InfoItemEmRace->SetImageKindX(GI_Main::ikxCenter);
                    InfoItemEmRace->SetImageKindY(GI_Main::ikyCenter);
                }
                if (!(pas::contains(WearableItemTypes, static_cast<std::uint8_t>(pas::checked_cast<aItem::TItem*>(Obj)->ItemType)) || static_cast<aItem::TItem*>(Obj)->ItemType == aConst::t_Hull)) {
                    {
                        GI_Image::TImageGI* InfoDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"sv));
                        InfoDurable->Parent->Parent->SetActive(false);
                    }
                    MinimumWidth = 0;
                } else {
                    if (aItem::THull* hull = pas::class_cast_if<aItem::THull*>(Obj)) {
                        float real_max = pas::real_max<float>(0.1f, hull->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
                        BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(static_cast<aItem::TItem*>(hull)->Weight, aConst::HullBaseSize), real_max)) * 64.0L);
                    } else {
                        BarWidth = System::Round(pas::real_divide(64.0L, pas::real_max<float>(0.1f, pas::checked_cast<aItem::TEquipment*>(Obj)->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})))));
                    }
                    BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
                    {
                        GI_Image::TImageGI* InfoDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableLeft"sv));
                        CapWidth = InfoDurableLeft->GetContentSize().X;
                        MinimumWidth = CapWidth * 2 + BarWidth + InfoDurableLeft->LocalPosition.X + InfoDurableLeft->Parent->LocalPosition.X + InfoDurableLeft->Parent->Parent->LocalPosition.X * 2;
                    }
                    {
                        GI_Image::TImageGI* InfoDurable_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"sv));
                        InfoDurable_2->Parent->Parent->SetActive(true);
                        InfoDurable_2->Parent->Parent->SetSize(ClassesImports::Point(CapWidth * 2 + BarWidth, InfoDurable_2->Parent->Parent->ClientSize.Y));
                        InfoDurable_2->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoDurable_2->Parent->Parent->ClientSize.Y));
                        if (pas::checked_cast<aItem::TItem*>(Obj)->ItemType == aConst::t_Hull) {
                            pas::Extended cpp_left_8 = pas::checked_cast<aItem::THull*>(Obj)->HullPoints;
                            std::int64_t cpp_left_7 = System::Round(pas::real_divide(cpp_left_8, pas::checked_cast<aItem::THull*>(Obj)->Weight) * BarWidth);
                            std::int32_t cpp_arg_62 = cpp_left_7 - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_62, y));
                        } else {
                            std::int64_t cpp_left_9 = System::Round(pas::real_divide(pas::checked_cast<aItem::TEquipment*>(Obj)->ConditionPercent, 1.0E+2L) * BarWidth);
                            std::int32_t cpp_arg_63 = cpp_left_9 - (InfoDurable_2->GetContentSize().X - 5);
                            std::int32_t y_2 = InfoDurable_2->LocalPosition.Y;
                            InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_63, y_2));
                        }
                    }
                    {
                        GI_Image::TImageGI* InfoDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableRight"sv));
                        {
                            std::int32_t cpp_arg_64 = BarWidth + CapWidth - InfoDurableRight->GetContentSize().X;
                            std::int32_t y_3 = InfoDurableRight->LocalPosition.Y;
                            InfoDurableRight->SetPosition(ClassesImports::Point(cpp_arg_64, y_3));
                        }
                        InfoDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoDurableRight->Parent->LocalPosition.Y));
                        InfoDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableRight->Parent->ClientSize.Y));
                    }
                    {
                        GI_Image::TImageGI* InfoDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableBack"sv));
                        {
                            std::int32_t cpp_arg_65 = BarWidth + 1 - InfoDurableBack->GetContentSize().X;
                            std::int32_t y_4 = InfoDurableBack->LocalPosition.Y;
                            InfoDurableBack->SetPosition(ClassesImports::Point(cpp_arg_65, y_4));
                        }
                        InfoDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableBack->Parent->ClientSize.Y));
                    }
                }
                {
                    GI_Label::TLabelGI* cpp_arg_66 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"sv));
                    GI_Label::TLabelGI* cpp_arg_67 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv));
                    fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, cpp_arg_67, cpp_arg_66, true, true, MinimumWidth);
                }
                GetByName(u"InfoItemSize"sv)->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
                GetByName(u"InfoItemPrice"sv)->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
                GetByName(u"InfoItemEmRace"sv)->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
                {
                    GI_Label::TLabelGI* cpp_arg_68 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"sv));
                    GI_Label::TLabelGI* cpp_arg_69 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv));
                    fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, cpp_arg_69, cpp_arg_68, true, true, 0);
                }
                DisplayedObject = Obj;
            } else if (pas::class_cast_if<aShip::TShip*>(Obj) != nullptr) {
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(true);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(false);
                if (aPlayer::GetPlayer() != Obj) {
                    {
                        const pas::WideString& wrapTextInColor_11 = ([&] {
                            pas::WideString fullName_2 = pas::checked_cast<aShip::TShip*>(Obj)->GetFullName(u" "_wref.get());
                            pas::WideString infoNameColorTag_7 = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(fullName_2)), pas::view(std::move(infoNameColorTag_7)));
                        }());
                        GI_Label::TLabelGI* cpp_arg_70 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv));
                        cpp_arg_70->SetText(wrapTextInColor_11);
                    }
                    if (aShip::TShip* ship_3 = pas::class_cast_if<aShip::TShip*>(Obj)) {
                        if (ship_3->PartnerShip == aPlayer::GetPlayer()) {
                            const pas::WideString& cpp_arg_71 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv))->GetText(), u"\r\n", ([&] {
                                pas::WideString lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"FormInfo.Partner"_wref.get());
                                pas::WideString textHighlightColorTag_2 = aMyFunction::TextHighlightColorTag;
                                return aMyFunction::WrapTextInColor(pas::view(std::move(lookupLocalizedTextByKey_2)), pas::view(std::move(textHighlightColorTag_2)));
                            }())});
                            GI_Label::TLabelGI* cpp_arg_72 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv));
                            cpp_arg_72->SetText(cpp_arg_71);
                        }
                    }
                    if (aKling::TKling* kling = pas::class_cast_if<aKling::TKling*>(Obj); kling != nullptr && kling->ActiveProgramAppliedTurn > 0 && pas::in_range(kling->ActiveProgramId, static_cast<std::int32_t>(aGalaxyStruct::prgShipwreck), static_cast<std::int32_t>(aGalaxyStruct::prgDisconnection))) {
                        const pas::WideString& cpp_arg_73 = pas::concat_wide({pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv))->GetText(), u"\r\n", ([&] {
                            pas::WideString localizedText_10 = aConst::LocalizedText(pas::concat_wide({u"Programms.", aConst::ProgramNames[pas::checked_cast<aKling::TKling*>(Obj)->ActiveProgramId], u".AddToShipInfo"}));
                            pas::WideString redColorTag = aMyFunction::RedColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(localizedText_10)), pas::view(std::move(redColorTag)));
                        }())});
                        GI_Label::TLabelGI* cpp_arg_74 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv));
                        cpp_arg_74->SetText(cpp_arg_73);
                    }
                } else {
                    const pas::WideString& wrapTextInColor_12 = ([&] {
                        pas::WideString fullName_3 = pas::checked_cast<aShip::TShip*>(Obj)->GetFullName(u" "_wref.get());
                        pas::WideString infoNameColorTag_8 = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(fullName_3)), pas::view(std::move(infoNameColorTag_8)));
                    }());
                    GI_Label::TLabelGI* cpp_arg_75 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv));
                    cpp_arg_75->SetText(wrapTextInColor_12);
                }
                if (reinterpret_cast<aShip::TShip*>(Obj)->GetFactionNameKey() != u"None") {
                    GI_Image::TImageGI* InfoShipEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipEmRace"sv));
                    InfoShipEmRace->SetImagePath(aConst::GetFactionEmblemPath(pas::checked_cast<aShip::TShip*>(Obj)->GetFactionNameKey()));
                    InfoShipEmRace->SetImageKindX(GI_Main::ikxCenter);
                    InfoShipEmRace->SetImageKindY(GI_Main::ikyCenter);
                    InfoShipEmRace->SetActive(true);
                } else {
                    GetByName(u"InfoShipEmRace"sv)->SetActive(false);
                }
                if (pas::class_cast_if<SE_Ship2::TShip2SE*>(pas::checked_cast<aShip::TShip*>(Obj)->Graphic) != nullptr) {
                    GI_GraphBuf::TGraphBufGI* InfoShipImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoShipImage2"sv));
                    ImagePath = static_cast<aShip::TShip*>(Obj)->GetShipPortraitImagePath();
                    InfoShipImage2->SetActive(ImagePath != u"");
                    if (InfoShipImage2->Active) {
                        InfoShipImage2->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 1, u","sv), InfoShipImage2->GraphBuf);
                        if (InfoShipImage2->ClientSize.X < InfoShipImage2->GraphBuf->Width || InfoShipImage2->ClientSize.Y < InfoShipImage2->GraphBuf->Height) {
                            if (static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)) {
                                InfoShipImage2->GraphBuf->RescaleRgba(InfoShipImage2->ClientSize.X, System::Round(pas::real_divide(InfoShipImage2->ClientSize.X, static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)), 5);
                            } else {
                                InfoShipImage2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoShipImage2->ClientSize.Y, static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width)), InfoShipImage2->ClientSize.Y, 5);
                            }
                        }
                        InfoShipImage2->SetImageKindX(GI_Main::ikxCenter);
                        InfoShipImage2->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter_9 = InfoShipImage2->GetVisualCenter();
                            Types::TPoint itemImageCenter_9 = Globals::ShipScreen->ItemImageCenter;
                            InfoShipImage2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_9, visualCenter_9));
                        }
                    }
                } else {
                    GI_GraphBuf::TGraphBufGI* InfoShipImage2_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoShipImage2"sv));
                    InfoShipImage2_2->SetActive(true);
                    InfoShipImage2_2->SourceHasPerPixelAlpha = true;
                    if (Obj == aKling::TerronShip && aGalaxy::Galaxy->TerronToStarTurn >= aGalaxyStruct::TerronTransformationFlag) {
                        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(reinterpret_cast<SE_Star::TStarSE*>(aKling::TerronShip->CurrentStar->Graphic)->StaticImagePath), 1, u","sv), InfoShipImage2_2->GraphBuf);
                    } else {
                        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::checked_cast<aShip::TShip*>(Obj)->Graphic)->StaticImagePath), 1, u","sv), InfoShipImage2_2->GraphBuf);
                    }
                    if (InfoShipImage2_2->ClientSize.X < InfoShipImage2_2->GraphBuf->Width || InfoShipImage2_2->ClientSize.Y < InfoShipImage2_2->GraphBuf->Height) {
                        if (static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)) {
                            InfoShipImage2_2->GraphBuf->RescaleRgba(InfoShipImage2_2->ClientSize.X, System::Round(pas::real_divide(InfoShipImage2_2->ClientSize.X, static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)), 5);
                        } else {
                            InfoShipImage2_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoShipImage2_2->ClientSize.Y, static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width)), InfoShipImage2_2->ClientSize.Y, 5);
                        }
                    }
                    {
                        Types::TPoint visualCenter_10 = InfoShipImage2_2->GetVisualCenter();
                        Types::TPoint itemImageCenter_10 = Globals::ShipScreen->ItemImageCenter;
                        InfoShipImage2_2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_10, visualCenter_10));
                    }
                }
                if (pas::class_cast_if<aRuins::TRuins*>(Obj) != nullptr) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"sv))->SetActive(false);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv))->SetActive(false);
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"sv))->SetActive(true);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv))->SetActive(true);
                    if (pas::class_cast_if<aRanger::TRanger*>(Obj) != nullptr) {
                        const pas::WideString& characterName = pas::checked_cast<aRanger::TRanger*>(Obj)->GetCharacterName();
                        GI_Label::TLabelGI* cpp_arg_76 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv));
                        cpp_arg_76->SetText(characterName);
                    } else {
                        const pas::WideString& localizedTypeName = pas::checked_cast<aShip::TShip*>(Obj)->GetLocalizedTypeName();
                        GI_Label::TLabelGI* cpp_arg_77 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv));
                        cpp_arg_77->SetText(localizedTypeName);
                    }
                }
                {
                    const pas::WideString& intToStr_3 = pas::wide_int_to_str(pas::checked_cast<aShip::TShip*>(Obj)->CalculateSpeed());
                    GI_Label::TLabelGI* cpp_arg_78 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSpeed"sv));
                    cpp_arg_78->SetText(intToStr_3);
                }
                {
                    const pas::WideString& wrapTextInColor_13 = aMyFunction::WrapTextInColor(u"???"sv, u""sv);
                    GI_Label::TLabelGI* cpp_arg_79 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"sv));
                    cpp_arg_79->SetText(wrapTextInColor_13);
                }
                {
                    pas::Extended cpp_left_10 = pas::checked_cast<aShip::TShip*>(Obj)->GetHull()->HullPoints;
                    if (cpp_left_10 <= pas::real_divide(pas::checked_cast<aShip::TShip*>(Obj)->GetHull()->Weight, 2.0L)) {
                        ColorTag = aMyFunction::OrangeColorTag;
                    } else {
                        ColorTag = pas::WideString();
                    }
                }
                if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Obj) || aPlayer::GetPlayer() == Obj || pas::checked_cast<aShip::TShip*>(Obj)->PartnerShip == aPlayer::GetPlayer() || static_cast<aShip::TShip*>(Obj)->TypeId == aGalaxyStruct::stTranclucator) {
                    Text = pas::concat_wide({aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(pas::checked_cast<aShip::TShip*>(Obj)->GetHull()->HullPoints)), pas::view(ColorTag)), u"/", pas::wide_int_to_str(pas::checked_cast<aShip::TShip*>(Obj)->GetHull()->Weight)});
                    if (aPlayer::GetPlayer()->HasScannerArtefact(pas::checked_cast<aShip::TShip*>(Obj))) {
                        {
                            const pas::WideString& weaponDamageSummary = aShip::TShip_GetWeaponDamageSummary(static_cast<aShip::TShip*>(Obj));
                            GI_Label::TLabelGI* cpp_arg_80 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"sv));
                            cpp_arg_80->SetText(weaponDamageSummary);
                        }
                        Text = pas::concat_wide({Text, u" + ", aMyFunction::WrapTextInColor(pas::view(aShip::TShip_GetRepairPointsSummary(pas::checked_cast<aShip::TShip*>(Obj))), u""sv)});
                    }
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"sv))->SetText(Text);
                } else {
                    const pas::WideString& wrapTextInColor_14 = aMyFunction::WrapTextInColor(u"???"sv, pas::view(ColorTag));
                    GI_Label::TLabelGI* cpp_arg_81 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"sv));
                    cpp_arg_81->SetText(wrapTextInColor_14);
                }
                Text = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(pas::checked_cast<aShip::TShip*>(Obj)->GetDefensePercent()), "%"}));
                if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Obj) || aPlayer::GetPlayer() == Obj || pas::checked_cast<aShip::TShip*>(Obj)->PartnerShip == aPlayer::GetPlayer() || static_cast<aShip::TShip*>(Obj)->TypeId == aGalaxyStruct::stTranclucator) {
                    Text = pas::concat_wide({Text, u" + ", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(pas::checked_cast<aShip::TShip*>(Obj)->GetArmor())), u""sv)});
                    if (aPlayer::GetPlayer()->HasScannerArtefact(pas::checked_cast<aShip::TShip*>(Obj))) {
                        Text = pas::concat_wide({static_cast<aShip::TShip*>(Obj)->GetManeuverabilitySummary(), Text});
                    }
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDef"sv))->SetText(Text);
                {
                    const pas::WideString& relationLevelTextToShip_2 = aShip::TShip_GetRelationLevelTextToShip(pas::checked_cast<aShip::TShip*>(Obj), aPlayer::GetPlayer());
                    GI_Label::TLabelGI* cpp_arg_82 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipRel"sv));
                    cpp_arg_82->SetText(relationLevelTextToShip_2);
                }
                if (aPlayer::GetPlayer() != Obj && !(pas::class_cast_if<aRuins::TRuins*>(Obj) != nullptr) && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 && aPlayer::GetPlayer()->CanResolveObjectWithScanner(Obj)) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"sv))->SetActive(true);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv))->SetActive(true);
                    {
                        const pas::WideString& cpp_arg_83 = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(aShip::TShip_GetWinChancePercent(aPlayer::GetPlayer(), pas::checked_cast<aShip::TShip*>(Obj))), "%"}));
                        GI_Label::TLabelGI* cpp_arg_84 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv));
                        cpp_arg_84->SetText(cpp_arg_83);
                    }
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"sv))->SetActive(false);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv))->SetActive(false);
                }
                {
                    float real_max_3 = pas::real_max<float>(0.1f, pas::checked_cast<aShip::TShip*>(Obj)->GetHull()->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
                    BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(static_cast<aShip::TShip*>(Obj)->GetHull()->Weight, aConst::HullBaseSize), real_max_3)) * 64.0L);
                }
                BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
                {
                    GI_Image::TImageGI* InfoShipDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableLeft"sv));
                    CapWidth = InfoShipDurableLeft->GetContentSize().X;
                    MinimumWidth = CapWidth * 2 + BarWidth + InfoShipDurableLeft->LocalPosition.X + InfoShipDurableLeft->Parent->LocalPosition.X + InfoShipDurableLeft->Parent->Parent->LocalPosition.X * 2;
                }
                {
                    GI_Image::TImageGI* InfoShipDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurable"sv));
                    if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Obj) || aPlayer::GetPlayer() == Obj || pas::checked_cast<aShip::TShip*>(Obj)->PartnerShip == aPlayer::GetPlayer() || static_cast<aShip::TShip*>(Obj)->TypeId == aGalaxyStruct::stTranclucator) {
                        pas::Extended cpp_left_12 = pas::checked_cast<aShip::TShip*>(Obj)->GetHull()->HullPoints;
                        std::int64_t cpp_left_11 = System::Round(pas::real_divide(cpp_left_12, pas::checked_cast<aShip::TShip*>(Obj)->GetHull()->Weight) * BarWidth);
                        std::int32_t cpp_arg_85 = cpp_left_11 - (InfoShipDurable->GetContentSize().X - 5);
                        std::int32_t y_5 = InfoShipDurable->LocalPosition.Y;
                        InfoShipDurable->SetPosition(ClassesImports::Point(cpp_arg_85, y_5));
                    } else {
                        MinimumWidth = MinimumWidth - BarWidth + 64;
                        BarWidth = 64;
                        {
                            std::int32_t cpp_arg_86 = BarWidth - (InfoShipDurable->GetContentSize().X - 5);
                            std::int32_t y_6 = InfoShipDurable->LocalPosition.Y;
                            InfoShipDurable->SetPosition(ClassesImports::Point(cpp_arg_86, y_6));
                        }
                    }
                    InfoShipDurable->Parent->Parent->SetActive(true);
                    InfoShipDurable->Parent->Parent->SetSize(ClassesImports::Point(CapWidth * 2 + BarWidth, InfoShipDurable->Parent->Parent->ClientSize.Y));
                    InfoShipDurable->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoShipDurable->Parent->Parent->ClientSize.Y));
                }
                {
                    GI_Image::TImageGI* InfoShipDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableRight"sv));
                    {
                        std::int32_t cpp_arg_87 = BarWidth + CapWidth - InfoShipDurableRight->GetContentSize().X;
                        std::int32_t y_7 = InfoShipDurableRight->LocalPosition.Y;
                        InfoShipDurableRight->SetPosition(ClassesImports::Point(cpp_arg_87, y_7));
                    }
                    InfoShipDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoShipDurableRight->Parent->LocalPosition.Y));
                    InfoShipDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoShipDurableRight->Parent->ClientSize.Y));
                }
                {
                    GI_Image::TImageGI* InfoShipDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableBack"sv));
                    {
                        std::int32_t cpp_arg_88 = BarWidth + 1 - InfoShipDurableBack->GetContentSize().X;
                        std::int32_t y_8 = InfoShipDurableBack->LocalPosition.Y;
                        InfoShipDurableBack->SetPosition(ClassesImports::Point(cpp_arg_88, y_8));
                    }
                    InfoShipDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoShipDurableBack->Parent->ClientSize.Y));
                }
                DamageName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISDamage"sv));
                DamageValue = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"sv));
                if (aPlayer::GetPlayer()->HasScannerArtefact(pas::checked_cast<aShip::TShip*>(Obj))) {
                    DamageName->SetActive(true);
                    DamageValue->SetActive(true);
                } else {
                    DamageName->SetActive(false);
                    DamageValue->SetActive(false);
                    DamageName = nullptr;
                    DamageValue = nullptr;
                }
                Text = pas::checked_cast<aShip::TShip*>(Obj)->GetCombatStatusDescription(StatusCount, false);
                if (StatusCount > 0) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISEffects"sv))->SetActive(true);
                    {
                        GI_Label::TLabelGI* InfoShipEffects = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipEffects"sv));
                        if (aPlayer::GetPlayer()->HasScannerArtefact(pas::checked_cast<aShip::TShip*>(Obj))) {
                            InfoShipEffects->SetText(pas::checked_cast<aShip::TShip*>(Obj)->GetCombatStatusDescription(StatusCount, true));
                        } else {
                            InfoShipEffects->SetText(Text);
                        }
                        {
                            std::int32_t cpp_right = InfoShipEffects->GetLineHeight();
                            std::int32_t cpp_arg_89 = StatusCount * cpp_right + 2;
                            std::int32_t x = InfoShipEffects->ClientSize.X;
                            InfoShipEffects->SetSize(ClassesImports::Point(x, cpp_arg_89));
                        }
                        InfoShipEffects->SetActive(true);
                    }
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISEffects"sv))->SetActive(false);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipEffects"sv))->SetActive(false);
                }
                {
                    GI_Label::TLabelGI* cpp_arg_90 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"sv));
                    GI_Label::TLabelGI* cpp_arg_91 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv));
                    GI_Label::TLabelGI* cpp_arg_92 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISSpeed"sv));
                    GI_Label::TLabelGI* cpp_arg_93 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSpeed"sv));
                    GI_Label::TLabelGI* cpp_arg_94 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISSize"sv));
                    GI_Label::TLabelGI* cpp_arg_95 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"sv));
                    GI_Label::TLabelGI* cpp_arg_96 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISDef"sv));
                    GI_Label::TLabelGI* cpp_arg_97 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDef"sv));
                    GI_Label::TLabelGI* cpp_arg_98 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISRel"sv));
                    GI_Label::TLabelGI* cpp_arg_99 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipRel"sv));
                    GI_Label::TLabelGI* cpp_arg_100 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"sv));
                    GI_Label::TLabelGI* cpp_arg_101 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv));
                    GI_Label::TLabelGI* cpp_arg_102 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISEffects"sv));
                    GI_Label::TLabelGI* cpp_arg_103 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipEffects"sv));
                    GI_MessageLoop::TObjectGI* byName_2 = GetByName(u"InfoShipEmRace"sv);
                    GI_Label::TLabelGI* cpp_arg_104 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv));
                    GI_Window::TWindowGI* cpp_arg_105 = pas::checked_cast<GI_Window::TWindowGI*>(ShipInfoPanel);
                    Globals::ShipScreen->LayoutObjectInfo(cpp_arg_105, cpp_arg_104, cpp_arg_90, cpp_arg_91, cpp_arg_92, cpp_arg_93, cpp_arg_94, cpp_arg_95, cpp_arg_96, cpp_arg_97, DamageName, DamageValue, cpp_arg_98, cpp_arg_99, cpp_arg_100, cpp_arg_101, cpp_arg_102, cpp_arg_103, byName_2, true, MinimumWidth);
                }
                DisplayedObject = Obj;
                if (aShip::TShip* ship_4 = pas::class_cast_if<aShip::TShip*>(Obj)) {
                    BuildShipPathOverlay(ship_4, false, pas::WideString());
                } else {
                    ClearPathOverlay(false);
                }
            } else if (pas::class_cast_if<aGalaxy::TStar*>(Obj) != nullptr) {
                ClearPathOverlay(false);
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(true);
                StandardInfoPanel->SetActive(false);
                {
                    GI_GraphBuf::TGraphBufGI* InfoStarImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStarImage"sv));
                    InfoStarImage->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(reinterpret_cast<SE_Star::TStarSE*>(pas::checked_cast<aGalaxy::TStar*>(Obj)->Graphic)->StaticImagePath), 1, u","sv), InfoStarImage->GraphBuf);
                    if (InfoStarImage->ClientSize.X < InfoStarImage->GraphBuf->Width || InfoStarImage->ClientSize.Y < InfoStarImage->GraphBuf->Height) {
                        if (static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) {
                            InfoStarImage->GraphBuf->RescaleRgba(InfoStarImage->ClientSize.X, System::Round(pas::real_divide(InfoStarImage->ClientSize.X, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)), 5);
                        } else {
                            InfoStarImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStarImage->ClientSize.Y, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)), InfoStarImage->ClientSize.Y, 5);
                        }
                    }
                    InfoStarImage->SetImageKindX(GI_Main::ikxCenter);
                    InfoStarImage->SetImageKindY(GI_Main::ikyCenter);
                }
                Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoStarPanel"sv));
                Panel->FreeOwnedChildren();
                Panel->SetSize(ClassesImports::Point(StarInfoWindow->ClientSize.X - StarInfoWindow->WorkSubRect.Left - StarInfoWindow->WorkSubRect.Right, Panel->ClientSize.Y));
                Objects = pas::make_object<pas::List>();
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Planets) - 1); cpp_range.next(I); ) {
                    pas::list_add(Objects, pas::list_get(aPlayer::GetPlayer()->CurrentStar->Planets, I));
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range_2.next(I); ) {
                    if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(aPlayer::GetPlayer()->CurrentStar->Ships, I)) != nullptr) {
                        if (pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(aPlayer::GetPlayer()->CurrentStar->Ships, I))->InNormalSpace()) {
                            if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(aPlayer::GetPlayer()->CurrentStar->Ships, I))->Graphic) != nullptr || pas::class_cast_if<SE_Ship2::TShip2SE*>(pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(aPlayer::GetPlayer()->CurrentStar->Ships, I))->Graphic) != nullptr && pas::checked_cast<SE_Ship2::TShip2SE*>(pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(aPlayer::GetPlayer()->CurrentStar->Ships, I))->Graphic)->AlternateImagePath != u"") {
                                Distance = aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, I)->Position, EC_Struct::MakePointF(0.0f, 0.0f));
                                J = 0;
                                while (J < pas::list_count(Objects)) {
                                    if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, J)) != nullptr) {
                                        if (aMyFunction::PointDistanceSquared(pas::list_at<aPlanet::TPlanet>(Objects, J)->GetPosition(), EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                            break;
                                        }
                                    } else if (aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(Objects, J)->Position, EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                        break;
                                    }
                                    ++J;
                                }
                                pas::list_insert(Objects, J, pas::list_get(aPlayer::GetPlayer()->CurrentStar->Ships, I));
                            }
                        }
                    }
                }
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->CustomSystemInfos) - 1); cpp_range_3.next(I); ) {
                    CustomInfo = pas::list_at<aGalaxy::TCustomSystemInfo>(aPlayer::GetPlayer()->CurrentStar->CustomSystemInfos, I);
                    Distance = pas::sqr(CustomInfo->Distance);
                    J = 0;
                    while (J < pas::list_count(Objects)) {
                        if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, J)) != nullptr) {
                            if (aMyFunction::PointDistanceSquared(pas::list_at<aPlanet::TPlanet>(Objects, J)->GetPosition(), EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                break;
                            }
                        } else if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, J)) != nullptr) {
                            if (aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(Objects, J)->Position, EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                break;
                            }
                        } else if (static_cast<long double>(pas::sqr(pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, J)->Distance)) > Distance) {
                            break;
                        }
                        ++J;
                    }
                    pas::list_insert(Objects, J, reinterpret_cast<void*>(CustomInfo));
                }
                RowHeight = GR_Main::GiScalePixels(20);
                NameWidth = GR_Main::GiScalePixels(100);
                DetailWidth = GR_Main::GiScalePixels(100);
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range_4.next(I); ) {
                    GI_Label::TLabelGI* cpp_with_26 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                    cpp_with_26->SetFontName(GlobalsV::NormalFontName);
                    cpp_with_26->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
                    cpp_with_26->SetSize(ClassesImports::Point(1, RowHeight));
                    cpp_with_26->SetPosition(ClassesImports::Point(0, RowHeight * I));
                    cpp_with_26->SetWordWrapEnabled(false);
                    cpp_with_26->SetTextAlignX(GI_Main::taxAuto);
                    cpp_with_26->SetTextAlignY(GI_Main::tayCenterEx);
                    if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        cpp_with_26->SetText(pas::list_at<aPlanet::TPlanet>(Objects, I)->Name);
                    } else if (pas::class_cast_if<aShip::TShip*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        cpp_with_26->SetText(pas::list_at<aShip::TShip>(Objects, I)->Name);
                    } else {
                        cpp_with_26->SetText(pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I)->Name);
                    }
                    NameWidth = std::max<std::int32_t>(NameWidth, cpp_with_26->ClientSize.X);
                }
                Child = Panel->FirstChild;
                while (Child != nullptr) {
                    if (GI_Label::TLabelGI* labelGI = pas::class_cast_if<GI_Label::TLabelGI*>(Child)) {
                        GI_Label::TLabelGI* cpp_with_27 = labelGI;
                        cpp_with_27->SetTextAlignX(GI_Main::taxRight);
                        cpp_with_27->SetSize(ClassesImports::Point(NameWidth, RowHeight));
                    }
                    Child = Child->NextSibling;
                }
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range_5.next(I); ) {
                    {
                        GI_GraphBuf::TGraphBufGI* cpp_with_28 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Panel, false);
                        IconInset = 0u;
                        if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                            if (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->Radius < 70) {
                                IconInset = 4u;
                            } else if (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->Radius < 80) {
                                IconInset = 3u;
                            } else if (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->Radius < 90) {
                                IconInset = 2u;
                            } else if (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->Radius < 100) {
                                IconInset = 1u;
                            } else {
                                IconInset = 0u;
                            }
                        }
                        cpp_with_28->SourceHasPerPixelAlpha = true;
                        cpp_with_28->SetPosition(ClassesImports::Point(NameWidth + 5 + 1 + (IconInset >> 1), RowHeight * I + 1 + (IconInset >> 1)));
                        cpp_with_28->SetSize(ClassesImports::Point(RowHeight - 2 - IconInset, RowHeight - 2 - IconInset));
                        if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                            pas::list_at<aPlanet::TPlanet>(Objects, I)->Graphic->RenderToBuffer(this, cpp_with_28->GraphBuf, true);
                            if (cpp_with_28->ClientSize.X < cpp_with_28->GraphBuf->Width || cpp_with_28->ClientSize.Y < cpp_with_28->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) {
                                    cpp_with_28->GraphBuf->RescaleRgba(cpp_with_28->ClientSize.X, System::Round(pas::real_divide(cpp_with_28->ClientSize.X, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_28->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_28->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)), cpp_with_28->ClientSize.Y, 5);
                                }
                            }
                        } else if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                            if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(pas::list_at<aShip::TShip>(Objects, I)->Graphic) != nullptr) {
                                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::list_at<aShip::TShip>(Objects, I)->Graphic)->StaticImagePath), 1, u","sv), cpp_with_28->GraphBuf);
                            } else {
                                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::checked_cast<SE_Ship2::TShip2SE*>(pas::list_at<aShip::TShip>(Objects, I)->Graphic)->AlternateImagePath), 1, u","sv), cpp_with_28->GraphBuf);
                            }
                            if (cpp_with_28->ClientSize.X < cpp_with_28->GraphBuf->Width || cpp_with_28->ClientSize.Y < cpp_with_28->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) {
                                    cpp_with_28->GraphBuf->RescaleRgba(cpp_with_28->ClientSize.X, System::Round(pas::real_divide(cpp_with_28->ClientSize.X, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_28->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_28->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)), cpp_with_28->ClientSize.Y, 5);
                                }
                            }
                        } else if (pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I)->Icon != u"") {
                            GI_GI::LoadGiByPathIntoGraphBuf(pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I)->Icon, cpp_with_28->GraphBuf);
                            if (static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) {
                                cpp_with_28->GraphBuf->RescaleRgba(cpp_with_28->ClientSize.X, System::Round(pas::real_divide(cpp_with_28->ClientSize.X, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)), 5);
                            } else {
                                cpp_with_28->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_28->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)), cpp_with_28->ClientSize.Y, 5);
                            }
                        }
                        cpp_with_28->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_28->SetImageKindY(GI_Main::ikyCenter);
                    }
                    if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        OwnerId = pas::list_at<aPlanet::TPlanet>(Objects, I)->OwnerId;
                    } else if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        OwnerId = pas::list_at<aShip::TShip>(Objects, I)->OwnerId;
                    } else {
                        OwnerId = aGalaxyStruct::oiUninhabited;
                    }
                    if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        GI_Label::TLabelGI* cpp_with_29 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                        if (GR_Main::GiResourceVariant() == 2) {
                            cpp_with_29->SetFontName(GlobalsV::MiniFontName);
                        } else {
                            cpp_with_29->SetFontName(GlobalsV::SmallFontName);
                        }
                        cpp_with_29->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                        cpp_with_29->SetSize(ClassesImports::Point(1, RowHeight));
                        cpp_with_29->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                        cpp_with_29->SetWordWrapEnabled(false);
                        cpp_with_29->SetTextAlignX(GI_Main::taxAuto);
                        cpp_with_29->SetTextAlignY(GI_Main::tayCenterEx);
                        cpp_with_29->SetText(EC_Str::LowerCaseWideString(pas::list_at<aShip::TShip>(Objects, I)->GetLocalizedTypeName()));
                        DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_29->ClientSize.X + GR_Main::GiScalePixels(35));
                    } else if (pas::class_cast_if<aGalaxy::TCustomSystemInfo*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        CustomInfo = pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I);
                        if (EC_Str::CountDelimitedPartsW(pas::view(CustomInfo->Info), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Info), 0, u":"sv) == u"Image") {
                            Images = EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Info), 1, u":"sv);
                            RowX = NameWidth + 5 + RowHeight + 5 + 1;
                            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Images), u","sv) - 1); cpp_range_6.next(J); ) {
                                GI_Image::TImageGI* cpp_with_30 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                                cpp_with_30->SetImagePath(pas::concat_wide({u"GI,", EC_Str::ExtractDelimitedPartW(pas::view(Images), J, u","sv)}));
                                cpp_with_30->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                                cpp_with_30->SetPosition(ClassesImports::Point(RowX, RowHeight * I + 1));
                                RowX = RowX + RowHeight + 2;
                            }
                        } else if (EC_Str::CountDelimitedPartsW(pas::view(CustomInfo->Info), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Info), 0, u":"sv) == u"RGBA") {
                            Images = EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Info), 1, u":"sv);
                            RowX = NameWidth + 5 + RowHeight + 5 + 1;
                            for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Images), u","sv) - 1); cpp_range_7.next(J); ) {
                                GI_GraphBuf::TGraphBufGI* cpp_with_31 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Panel, false);
                                cpp_with_31->SourceHasPerPixelAlpha = true;
                                cpp_with_31->LoadBitmapPathAsRgba(pas::concat_wide({EC_Str::ExtractDelimitedPartW(pas::view(Images), J, u","sv), EC_CacheBitmap::RgbaImagePathSuffix}));
                                cpp_with_31->SetPosition(ClassesImports::Point(RowX, RowHeight * I + 1));
                                cpp_with_31->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                                if (cpp_with_31->ClientSize.X < cpp_with_31->GraphBuf->Width || cpp_with_31->ClientSize.Y < cpp_with_31->GraphBuf->Height) {
                                    if (static_cast<std::uint32_t>(cpp_with_31->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_31->GraphBuf->Height)) {
                                        cpp_with_31->GraphBuf->RescaleRgba(cpp_with_31->ClientSize.X, System::Round(pas::real_divide(cpp_with_31->ClientSize.X, static_cast<std::uint32_t>(cpp_with_31->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_31->GraphBuf->Height)), 5);
                                    } else {
                                        cpp_with_31->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_31->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_31->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_31->GraphBuf->Width)), cpp_with_31->ClientSize.Y, 5);
                                    }
                                }
                                cpp_with_31->SetImageKindX(GI_Main::ikxCenter);
                                cpp_with_31->SetImageKindY(GI_Main::ikyCenter);
                                RowX = RowX + RowHeight + 2;
                            }
                        } else {
                            GI_Label::TLabelGI* cpp_with_32 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                            if (GR_Main::GiResourceVariant() == 2) {
                                cpp_with_32->SetFontName(GlobalsV::MiniFontName);
                            } else {
                                cpp_with_32->SetFontName(GlobalsV::SmallFontName);
                            }
                            cpp_with_32->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                            cpp_with_32->SetSize(ClassesImports::Point(1, RowHeight));
                            cpp_with_32->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                            cpp_with_32->SetWordWrapEnabled(false);
                            cpp_with_32->SetTextAlignX(GI_Main::taxAuto);
                            cpp_with_32->SetTextAlignY(GI_Main::tayCenterEx);
                            cpp_with_32->SetText(CustomInfo->Info);
                            DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_32->ClientSize.X + GR_Main::GiScalePixels(35));
                        }
                    } else if (OwnerId != aGalaxyStruct::oiUninhabited) {
                        if (!(pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->IsMainPiratePlanet ^ 1)) {
                            GI_GraphBuf::TGraphBufGI* cpp_with_33 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Panel, false);
                            cpp_with_33->SourceHasPerPixelAlpha = true;
                            cpp_with_33->LoadBitmapPathAsRgba(pas::concat_wide_reverse({EC_CacheBitmap::RgbaImagePathSuffix, EC_Str::ExtractDelimitedPartW(pas::view(aConst::GetFactionEmblemPath(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->GetFactionResourceName())), 1, u","sv)}));
                            cpp_with_33->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I + 1));
                            cpp_with_33->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                            if (cpp_with_33->ClientSize.X < cpp_with_33->GraphBuf->Width || cpp_with_33->ClientSize.Y < cpp_with_33->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Height)) {
                                    cpp_with_33->GraphBuf->RescaleRgba(cpp_with_33->ClientSize.X, System::Round(pas::real_divide(cpp_with_33->ClientSize.X, static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_33->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_33->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Width)), cpp_with_33->ClientSize.Y, 5);
                                }
                            }
                            cpp_with_33->SetImageKindX(GI_Main::ikxCenter);
                            cpp_with_33->SetImageKindY(GI_Main::ikyCenter);
                        }
                    }
                    if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr && pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->OwnerId) && static_cast<std::uint8_t>(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->IsMainPiratePlanet ^ 1) && pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->CurrentStar->Status.CustomFaction == u"") {
                        RowX = NameWidth + 5 + RowHeight + 5 + 1;
                        {
                            GI_Image::TImageGI* cpp_with_34 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                            switch (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->GetRelationLevelToShip(aPlayer::GetPlayer())) {
                                case aGalaxyStruct::rlHostile: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face4"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlBad: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face3"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlNormal: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face2"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlGood: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face1"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlExcellent: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face0"_w);
                                    break;
                                }
                                default: cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face2"_w); break;
                            }
                            cpp_with_34->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                            cpp_with_34->SetPosition(ClassesImports::Point(RowX + RowHeight + 2, RowHeight * I + 1));
                        }
                        RowX = RowX + RowHeight + 2;
                        if (pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peIndustrial>(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->Economy)) {
                            GI_Image::TImageGI* cpp_with_35 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                            switch (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->Economy) {
                                case aGalaxyStruct::peAgricultural: {
                                    cpp_with_35->SetImagePath(u"GI,Bm.FormGalaxy.EconAgrar"_w);
                                    break;
                                }
                                case aGalaxyStruct::peIndustrial: {
                                    cpp_with_35->SetImagePath(u"GI,Bm.FormGalaxy.EconIndustr"_w);
                                    break;
                                }
                            }
                            cpp_with_35->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                            cpp_with_35->SetPosition(ClassesImports::Point(RowX + RowHeight, RowHeight * I + 1));
                        }
                    } else if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr && pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->IsMainPiratePlanet) {
                        GI_Label::TLabelGI* cpp_with_36 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                        if (GR_Main::GiResourceVariant() == 2) {
                            cpp_with_36->SetFontName(GlobalsV::MiniFontName);
                        } else {
                            cpp_with_36->SetFontName(GlobalsV::SmallFontName);
                        }
                        cpp_with_36->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                        cpp_with_36->SetSize(ClassesImports::Point(1, RowHeight));
                        cpp_with_36->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                        cpp_with_36->SetWordWrapEnabled(false);
                        cpp_with_36->SetTextAlignX(GI_Main::taxAuto);
                        cpp_with_36->SetTextAlignY(GI_Main::tayCenterEx);
                        cpp_with_36->SetText(EC_Str::LowerCaseWideString(aConst::LocalizedText(u"ShipType.TypeName.PB"_wref.get())));
                        DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_36->ClientSize.X + GR_Main::GiScalePixels(35));
                    } else if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        if (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->OwnerId == aGalaxyStruct::oiUninhabited) {
                            if (pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->GetUnexploredSurfaceTileCount() == 0) {
                                GI_Label::TLabelGI* cpp_with_37 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                                cpp_with_37->SetFontName(GlobalsV::MiniFontName);
                                cpp_with_37->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(140, 140, 140));
                                cpp_with_37->SetSize(ClassesImports::Point(1, RowHeight));
                                cpp_with_37->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                                cpp_with_37->SetWordWrapEnabled(false);
                                cpp_with_37->SetTextAlignX(GI_Main::taxAuto);
                                cpp_with_37->SetTextAlignY(GI_Main::tayCenterEx);
                                cpp_with_37->SetText(EC_Str::LowerCaseWideString(aConst::LocalizedText(u"Planet.NotCivil.AllExplore"_wref.get())));
                                DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_37->ClientSize.X + GR_Main::GiScalePixels(35));
                            }
                        }
                    }
                }
                Panel->SetSize(ClassesImports::Point(NameWidth + DetailWidth, RowHeight * pas::list_count(Objects)));
                Panel->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&StarInfoWindow->WorkSubRect, 0)));
                StarInfoWindow->SetSize(ClassesImports::Point(Panel->ClientSize.X + StarInfoWindow->WorkSubRect.Left + StarInfoWindow->WorkSubRect.Right, StarInfoWindow->WorkSubRect.Top + StarInfoWindow->WorkSubRect.Bottom + RowHeight * pas::list_count(Objects)));
                StarInfoWindow->UpdateAutoGeometry();
                {
                    GI_Label::TLabelGI* InfoStarName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStarName"sv));
                    InfoStarName->SetText(aMyFunction::WrapTextInColor(pas::view(pas::checked_cast<aGalaxy::TStar*>(Obj)->Name), pas::view(aMyFunction::InfoNameColorTag)));
                    InfoStarName->SetSize(ClassesImports::Point(StarInfoWindow->ClientSize.X - StarInfoWindow->WorkSubRect.Right - InfoStarName->LocalPosition.X - 15, InfoStarName->ClientSize.Y));
                }
                pas::free(Objects);
                DisplayedObject = Obj;
            } else if (pas::class_cast_if<aMissile::TMissile*>(Obj) != nullptr) {
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(true);
                GetByName(u"InfoStdImage"sv)->SetActive(false);
                {
                    GI_GraphBuf::TGraphBufGI* InfoStdGB_7 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                    InfoStdGB_7->SetActive(true);
                    InfoStdGB_7->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.Missile.w", pas::checked_cast<aMissile::TMissile*>(Obj)->GetGraphSuffix(), u"_", GR_Main::GiResourceSuffix(), u"i"}), InfoStdGB_7->GraphBuf);
                    {
                        Types::TPoint visualCenter_11 = InfoStdGB_7->GetVisualCenter();
                        Types::TPoint itemImageCenter_11 = Globals::ShipScreen->ItemImageCenter;
                        InfoStdGB_7->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_11, visualCenter_11));
                    }
                }
                {
                    const pas::WideString& wrapTextInColor_15 = ([&] {
                        pas::WideString displayName_6 = pas::checked_cast<aMissile::TMissile*>(Obj)->GetDisplayName();
                        pas::WideString infoNameColorTag_9 = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(displayName_6)), pas::view(std::move(infoNameColorTag_9)));
                    }());
                    GI_Label::TLabelGI* cpp_arg_106 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    cpp_arg_106->SetText(wrapTextInColor_15);
                }
                {
                    const pas::WideString& infoText_3 = pas::checked_cast<aMissile::TMissile*>(Obj)->GetInfoText();
                    GI_Label::TLabelGI* cpp_arg_107 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                    cpp_arg_107->SetText(infoText_3);
                }
                {
                    GI_Label::TLabelGI* cpp_arg_108 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                    GI_Label::TLabelGI* cpp_arg_109 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    GI_Window::TWindowGI* cpp_arg_110 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                    fShip2::TfShip2::LayoutItemInfo(cpp_arg_110, cpp_arg_109, cpp_arg_108, true, true, 0);
                }
                DisplayedObject = Obj;
            } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(Obj) != nullptr) {
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(true);
                GetByName(u"InfoStdImage"sv)->SetActive(false);
                {
                    GI_GraphBuf::TGraphBufGI* InfoStdGB_8 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                    InfoStdGB_8->SetActive(true);
                    InfoStdGB_8->SourceHasPerPixelAlpha = true;
                    GI_GAI::LoadGaiFrameToGraphBuf(reinterpret_cast<SE_Asteroid::TAsteroidSE*>(reinterpret_cast<aAsteroid::TAsteroid*>(Obj)->GraphObject)->ImagePath, InfoStdGB_8->GraphBuf, reinterpret_cast<aAsteroid::TAsteroid*>(Obj)->Id);
                    if (InfoStdGB_8->ClientSize.X < InfoStdGB_8->GraphBuf->Width || InfoStdGB_8->ClientSize.Y < InfoStdGB_8->GraphBuf->Height) {
                        if (static_cast<std::uint32_t>(InfoStdGB_8->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_8->GraphBuf->Height)) {
                            InfoStdGB_8->GraphBuf->RescaleRgba(InfoStdGB_8->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_8->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_8->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_8->GraphBuf->Height)), 5);
                        } else {
                            InfoStdGB_8->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_8->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_8->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_8->GraphBuf->Width)), InfoStdGB_8->ClientSize.Y, 5);
                        }
                    }
                    {
                        Types::TPoint visualCenter_12 = InfoStdGB_8->GetVisualCenter();
                        Types::TPoint itemImageCenter_12 = Globals::ShipScreen->ItemImageCenter;
                        InfoStdGB_8->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_12, visualCenter_12));
                    }
                }
                {
                    const pas::WideString& wrapTextInColor_16 = ([&] {
                        pas::WideString displayName_7 = pas::checked_cast<aAsteroid::TAsteroid*>(Obj)->GetDisplayName();
                        pas::WideString infoNameColorTag_10 = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(pas::view(std::move(displayName_7)), pas::view(std::move(infoNameColorTag_10)));
                    }());
                    GI_Label::TLabelGI* cpp_arg_111 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    cpp_arg_111->SetText(wrapTextInColor_16);
                }
                {
                    const pas::WideString& infoText_4 = pas::checked_cast<aAsteroid::TAsteroid*>(Obj)->GetInfoText();
                    GI_Label::TLabelGI* cpp_arg_112 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                    cpp_arg_112->SetText(infoText_4);
                }
                {
                    GI_Label::TLabelGI* cpp_arg_113 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                    GI_Label::TLabelGI* cpp_arg_114 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    GI_Window::TWindowGI* cpp_arg_115 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                    fShip2::TfShip2::LayoutItemInfo(cpp_arg_115, cpp_arg_114, cpp_arg_113, true, true, 0);
                }
                DisplayedObject = Obj;
            } else {
                InfoWindow->SetActive(true);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(false);
                {
                    const pas::WideString& objectInfoText = aPlayer::GetPlayer()->GetObjectInfoText(Obj);
                    GI_Label::TLabelGI* infoTextLabel = InfoTextLabel;
                    infoTextLabel->SetText(objectInfoText);
                }
                InfoWindow->SetSize(ClassesImports::Point(InfoTextLabel->ClientSize.X + InfoWindow->WorkSubRect.Left + InfoWindow->WorkSubRect.Right, InfoTextLabel->ClientSize.Y + InfoWindow->WorkSubRect.Top + InfoWindow->WorkSubRect.Bottom));
                InfoWindow->UpdateAutoGeometry();
                InfoTextLabel->SetPosition(ClassesImports::Point(InfoWindow->WorkSubRect.Left, InfoWindow->WorkSubRect.Top));
                DisplayedObject = Obj;
                if (aAsteroid::TAsteroid* asteroid = pas::class_cast_if<aAsteroid::TAsteroid*>(Obj)) {
                    ShowAsteroidPath(asteroid);
                } else {
                    ClearAsteroidPath();
                }
                if (aShip::TShip* ship_5 = pas::class_cast_if<aShip::TShip*>(Obj)) {
                    BuildShipPathOverlay(ship_5, false, pas::WideString());
                } else {
                    ClearPathOverlay(false);
                }
            }
        }
        if (InfoWindow->Active) {
            ActivePanel = InfoWindow;
        } else if (ItemInfoWindow->Active) {
            ActivePanel = ItemInfoWindow;
        } else if (ShipInfoPanel->Active) {
            ActivePanel = ShipInfoPanel;
        } else if (PlanetInfoPanel->Active) {
            ActivePanel = PlanetInfoPanel;
        } else if (StarInfoWindow->Active) {
            ActivePanel = StarInfoWindow;
        } else if (StandardInfoPanel->Active) {
            ActivePanel = StandardInfoPanel;
        } else {
            return;
        }
        if (GlobalsV::DynamicTipsPos) {
            HitObjectPosition.X += static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2;
            HitObjectPosition.Y += static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2;
            I = static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 3;
            if (HitObjectPosition.X <= I) {
                HitObjectPosition.X += HitObjectSize.X / 2;
            } else if (HitObjectPosition.X >= 2 * I) {
                HitObjectPosition.X = HitObjectPosition.X - HitObjectSize.X / 2 - ActivePanel->ClientSize.X;
            } else {
                HitObjectPosition.X -= ActivePanel->ClientSize.X / 2;
                I = 0;
            }
            if (I == 0) {
                if (HitObjectPosition.Y < static_cast<std::int32_t>(static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2)) {
                    HitObjectPosition.Y += HitObjectSize.Y / 2;
                    if (ActivePanel->ClientSize.Y + HitObjectPosition.Y + 10 > GR_Main::GameScreenHeight) {
                        HitObjectPosition.Y = HitObjectPosition.Y - HitObjectSize.Y - ActivePanel->ClientSize.Y;
                    }
                    if (HitObjectPosition.Y < 10) {
                        HitObjectPosition.Y = 10;
                    }
                } else {
                    HitObjectPosition.Y = HitObjectPosition.Y - HitObjectSize.Y / 2 - ActivePanel->ClientSize.Y;
                }
            } else {
                I = static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 3;
                if (HitObjectPosition.Y <= I) {
                    HitObjectPosition.Y += HitObjectSize.Y / 2;
                } else if (HitObjectPosition.Y >= 2 * I) {
                    HitObjectPosition.Y = HitObjectPosition.Y - HitObjectSize.Y / 2 - ActivePanel->ClientSize.Y;
                } else {
                    HitObjectPosition.Y -= ActivePanel->ClientSize.Y / 2;
                }
            }
            HitObjectPosition.X -= static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2;
            HitObjectPosition.Y -= static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2;
            ActivePanel->SetPosition(HitObjectPosition);
        } else {
            ActivePanel->SetPosition(ClassesImports::Point(10 - static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2, 10 - static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2));
        }
    }

    void TfStarMap::MapScrollChanged() {
        RefreshActionRanges();
        Globals::SpaceProcess->Space->DrawMinimap();
    }

    pas::WideString TfStarMap::GetPriceSnapshotKey(pas::Object* Obj) {
        pas::WideString Result{};
        if (aPlanet::TPlanet* planet = pas::class_cast_if<aPlanet::TPlanet*>(Obj)) {
            Result = planet->Name;
        } else if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Obj)) {
            Result = ruins->Name;
        } else if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Obj)) {
            Result = ship->Name;
        } else {
            Result = pas::WideString();
        }
        return pas::concat_wide({u"GOODS", Result, aGalaxy::Galaxy->FormatTurnDate(-1)});
    }

    void TfStarMap::SaveVisiblePriceSnapshots() {
        std::int32_t Index{};
        aPlanet::TPlanet* Planet{};
        aShip::TShip* Ship{};
        std::uint8_t Added = false;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Planets) - 1); cpp_range.next(Index); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(aPlayer::GetPlayer()->CurrentStar->Planets, Index);
            if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId)) {
                pas::Extended cpp_left = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Planet->GetPosition());
                if (cpp_left <= aPlayer::GetPlayer()->GetRadarRange()) {
                    {
                        const pas::WideString& priceSnapshotKey = TfStarMap::GetPriceSnapshotKey(Planet);
                        const pas::WideString& buildPriceText = fGoodsShop2::TfGoodsShop2::BuildPriceText(Planet);
                        std::int32_t currentTurn = aGalaxy::Galaxy->CurrentTurn;
                        Globals::AddOrUpdatePlayerBubble(Globals::pmUserNote, currentTurn, buildPriceText, priceSnapshotKey);
                    }
                    Added = true;
                }
            }
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CurrentStar->Ships) - 1); cpp_range_2.next(Index); ) {
            Ship = pas::list_at<aShip::TShip>(aPlayer::GetPlayer()->CurrentStar->Ships, Index);
            if (aRuins::TRuins* ruins = pas::class_cast_if<aRuins::TRuins*>(Ship)) {
                if (ruins->virtual_TShip_CanDock(aPlayer::GetPlayer()) && static_cast<std::uint8_t>(Ship->NoTalk ^ 1)) {
                    pas::Extended cpp_left_2 = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                    if (cpp_left_2 <= aPlayer::GetPlayer()->GetRadarRange()) {
                        {
                            const pas::WideString& priceSnapshotKey_2 = TfStarMap::GetPriceSnapshotKey(Ship);
                            const pas::WideString& buildPriceText_2 = fGoodsShop2::TfGoodsShop2::BuildPriceText(Ship);
                            std::int32_t currentTurn_2 = aGalaxy::Galaxy->CurrentTurn;
                            Globals::AddOrUpdatePlayerBubble(Globals::pmUserNote, currentTurn_2, buildPriceText_2, priceSnapshotKey_2);
                        }
                        Added = true;
                    }
                }
            }
        }
        if (Added) {
            GR_Main::SoundManager->PlaySound(u"Sound.UserMsgAdd"_wref.get());
            MainPanel->RebuildMessageButtons(false);
        }
    }

    void TfStarMap::CenterOnShip(aShip::TShip* Ship) {
        SetMapCenterManually(EC_Struct::TruncatePointF(Ship->Position));
        AddMapAnimation(Ship->Position, pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 0);
        AddMapAnimation(Ship->Position, pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 200);
        AddMapAnimation(Ship->Position, pas::concat_wide({u"Bm.SI.", GR_Main::GiResourceSuffix(), u"Ring"}), 400);
    }

    // Selection 1 chooses the nearest TKling; 2 chooses the farthest.
    void TfStarMap::CenterOnDominator(std::int32_t Selection) {
        std::int32_t Index{};
        aShip::TShip* Ship{};
        double Distance{};
        double FarthestDistance = 0.0;
        double NearestDistance = 1.0E+20;
        aShip::TShip* NearestShip = nullptr;
        aShip::TShip* FarthestShip = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::PlayerStar->Ships) - 1); cpp_range.next(Index); ) {
            Ship = pas::list_at<aShip::TShip>(aGalaxy::PlayerStar->Ships, Index);
            if (static_cast<std::uint8_t>(Ship->IsOutsideStarSpace() ^ 1) && pas::class_cast_if<aKling::TKling*>(Ship) != nullptr) {
                Distance = aMyFunction::PointDistance(aPlayer::GetPlayer()->Position, Ship->Position);
                if (Distance > FarthestDistance) {
                    FarthestDistance = Distance;
                    FarthestShip = Ship;
                }
                if (Distance < NearestDistance) {
                    NearestDistance = Distance;
                    NearestShip = Ship;
                }
            }
        }
        if (Selection == 1 && NearestShip != nullptr) {
            CenterOnShip(NearestShip);
        } else if (Selection == 2) {
            if (FarthestShip != nullptr) {
                CenterOnShip(FarthestShip);
            }
        }
    }

    void TfStarMap::CenterShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        CenterOnShip(aPlayer::GetPlayer());
    }

    void TfStarMap::CenterShipMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        // The native guard tests job 5 as well as the declared preparation job 3.
        if (ThreadCalc::IsTurnCalculationRunning() && (Globals::TurnCalculationThread->Job == ThreadCalc::tcjPreparePlayerStar || static_cast<std::int32_t>(Globals::TurnCalculationThread->Job) == 5)) {
            return;
        }
        if (aPlayer::GetPlayer() != nullptr) {
            ShowObjectInfo(aPlayer::GetPlayer());
        }
    }

    void TfStarMap::CenterShipMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        ShowObjectInfo(nullptr);
    }

    void TfStarMap::AllWeaponsClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Mode == smmOrders) {
            TalkSelectionActive = false;
            ScannerSelectionActive = false;
            InterceptorSelectionActive = false;
            CustomSelectionActive = false;
            SelectAllUsableWeapons();
            RebuildTargetMarkers();
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
        }
    }

    void TfStarMap::ScannerClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        if (Mode == smmOrders) {
            TalkSelectionActive = false;
            for (Index = 0; Index <= 4; ++Index) {
                SelectedWeapons[Index] = false;
            }
            InterceptorSelectionActive = false;
            CustomSelectionActive = false;
            if ((static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetScanner()) ^ 1) || static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetRadar()) ^ 1)) && aGalaxy::Galaxy->UltraScanModEnabled == 0) {
                ScannerSelectionActive = false;
            } else {
                ScannerSelectionActive = static_cast<std::uint8_t>(ScannerSelectionActive ^ 1);
            }
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
        }
    }

    void TfStarMap::TalkClicked(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        if (aGalaxy::Galaxy->SpecialSimulationMode != 0) {
            return;
        }
        if (Mode == smmOrders) {
            ScannerSelectionActive = false;
            for (Index = 0; Index <= 4; ++Index) {
                SelectedWeapons[Index] = false;
            }
            InterceptorSelectionActive = false;
            CustomSelectionActive = false;
            if (!aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetRadar())) {
                TalkSelectionActive = false;
            } else {
                TalkSelectionActive = static_cast<std::uint8_t>(TalkSelectionActive ^ 1);
            }
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
        }
    }

    void TfStarMap::SelectInterceptorTarget() {
        std::int32_t Index{};
        if (Mode == smmOrders && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull()->InterceptorsEnabled && aPlayer::GetPlayer()->GetHull()->Energy >= aPlayer::GetPlayer()->GetInterceptorEnergyCost()) {
            TalkSelectionActive = false;
            ScannerSelectionActive = false;
            for (Index = 0; Index <= 4; ++Index) {
                SelectedWeapons[Index] = false;
            }
            CustomSelectionActive = false;
            InterceptorSelectionActive = true;
            RebuildTargetMarkers();
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
        }
    }

    void TfStarMap::BeginCustomSelection() {
        std::int32_t Index{};
        if (Mode == smmOrders && aPlayer::GetPlayer() != nullptr) {
            TalkSelectionActive = false;
            ScannerSelectionActive = false;
            for (Index = 0; Index <= 4; ++Index) {
                SelectedWeapons[Index] = false;
            }
            InterceptorSelectionActive = false;
            CustomSelectionActive = true;
            RebuildTargetMarkers();
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
        }
    }

    void TfStarMap::ToggleWeaponPanelClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (Mode == smmOrders) {
            Globals::StarMapWeaponPanelOpen = static_cast<std::uint8_t>(Globals::StarMapWeaponPanelOpen ^ 1);
            if (Globals::StarMapWeaponPanelOpen) {
                AnimateWeaponPanel(1);
            } else {
                AnimateWeaponPanel(-1);
            }
        }
    }

    void TfStarMap::WeaponButtonDown(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
        GI_Image::TImageGI* Image = WeaponImages[Index];
        Image->SetPosition(ClassesImports::Point(Image->LocalPosition.X, 0));
    }

    void TfStarMap::WeaponButtonUp(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        aItem::TWeapon* Weapon{};
        if (Mode == smmOrders) {
            DisplayedObject = nullptr;
            Index = EC_Str::ExtractDigitsToIntW(pas::view(Sender->ControlName));
            aGalaxy::Galaxy->CheckIntegrityChecksum(87);
            Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
            TalkSelectionActive = false;
            ScannerSelectionActive = false;
            InterceptorSelectionActive = false;
            CustomSelectionActive = false;
            if (static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Weapon) ^ 1) || pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket)) && Weapon->Ammo <= 0) {
                SelectedWeapons[Index] = false;
            } else if (Weapon->Target != nullptr) {
                Weapon->Target = nullptr;
                SelectedWeapons[Index] = false;
            } else {
                SelectedWeapons[Index] = static_cast<std::uint8_t>(SelectedWeapons[Index] ^ 1);
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(88);
            RebuildTargetMarkers();
            RefreshActionRanges();
            RefreshWeaponButtons();
            UpdateActionCursor(false);
        }
    }

    void TfStarMap::RefreshWeaponButtons() {
        std::int32_t Slot{};
        aItem::TWeapon* Weapon{};
        GI_GraphButton::TGraphButtonGI* Button{};
        GI_Image::TImageGI* Image{};
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 4); cpp_range.next(Slot); ) {
            Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Slot));
            Button = WeaponButtons[Slot];
            Button->SetDisabled(aPlayer::GetPlayer()->GetSlotCount(aConst::sskWeapon) <= Slot || static_cast<std::uint8_t>(aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Weapon) ^ 1));
            if (aPlayer::GetPlayer()->GetSlotCount(aConst::sskWeapon) <= Slot) {
                Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Disabled"}));
            } else if (Weapon == nullptr) {
                Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Empty"}));
            } else if (!aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Weapon)) {
                Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Red"}));
            }
            if (SelectedWeapons[Slot]) {
                Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Down"}));
                Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Down"}));
                Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Normal"}));
            } else if (SelectedWeapons[Slot] || Weapon != nullptr && Weapon->Target != nullptr) {
                Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Select"}));
                Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Select"}));
                Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Down"}));
            } else {
                Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Normal"}));
                Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Active"}));
                Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.PanelMain2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Slot + 1), u"Down"}));
            }
            Button->DownCallback = pas::bind_method<&TfStarMap::WeaponButtonDown>(this);
            Button->UpCallback = pas::bind_method<&TfStarMap::WeaponButtonUp>(this);
            Image = WeaponImages[Slot];
            if (Weapon == nullptr) {
                Image->SetActive(false);
            } else {
                Image->SetActive(true);
                Image->SetImagePath(pas::concat_wide({u"GI,", Weapon->GetBitmapResourceName(), u"s"}));
            }
            Image->SetImageKindX(GI_Main::ikxCenter);
            Image->SetImageKindY(GI_Main::ikyCenter);
            if (SelectedWeapons[Slot]) {
                Image->SetPosition(ClassesImports::Point(Image->LocalPosition.X, 0));
            } else {
                Image->SetPosition(ClassesImports::Point(Image->LocalPosition.X, -2));
            }
            if (Weapon != nullptr) {
                WeaponButtons[Slot]->HelpText = pas::concat_wide({Weapon->GetDisplayName(), u" (", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Slot + 1)), pas::view(aMyFunction::TextHighlightColorTag)), u")"});
            }
        }
    }

    void TfStarMap::RefreshActionRanges() {
        WindowsSdk::TPoint Point{};
        WindowsSdk::TPoint MainCenter{};
        WindowsSdk::TPoint InnerCenter{};
        EC_Struct::TPointF RadarCenter{};
        std::int32_t Slot{};
        std::int32_t MinRange{};
        std::int32_t MaxRange{};
        aItem::TWeapon* Weapon{};
        GI_SpaceCircle::TSpaceCircleGI* ExtraCircle{};
        std::int32_t RangeValue{};
        std::int32_t RangeMaximum{};
        pas::Array<std::int32_t, 0, 2> Reserved{};
        CursorObject = FindObjectAtCursor();
        // These three assignments are retained from the native routine; their values are unused.
        Reserved[0] = 1;
        Reserved[1] = 1;
        Reserved[2] = 1;
        std::uint8_t AnyWeapon = false;
        for (Slot = 0; Slot <= 4; ++Slot) {
            if (SelectedWeapons[Slot]) {
                AnyWeapon = true;
                break;
            }
        }
        RadarCenter = aPlayer::GetPlayer()->Position;
        std::int32_t RadarRadius = aPlayer::GetPlayer()->GetRadarRange();
        std::uint32_t RadarColor = 0u;
        InnerCenter = EC_Struct::TruncatePointF(aPlayer::GetPlayer()->Position);
        std::int32_t InnerRadius = 0;
        std::uint32_t InnerColor = 0u;
        std::uint8_t InnerVisible = false;
        MainCenter = EC_Struct::TruncatePointF(aPlayer::GetPlayer()->Position);
        std::int32_t MainRadius = aPlayer::GetPlayer()->GetRadarRange();
        std::uint32_t MainColor = 0u;
        std::uint8_t MainVisible = false;
        if (ScannerSelectionActive) {
            MainColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 0);
            MainVisible = true;
            RadarColor = MainColor;
        } else if (TalkSelectionActive) {
            MainColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 255);
            MainVisible = true;
            RadarColor = MainColor;
        } else if (InterceptorSelectionActive) {
            MainColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 100, 0);
            MainVisible = true;
            MainRadius = 1000;
            RadarRadius = 1000;
            RadarColor = MainColor;
        } else if (CustomSelectionActive) {
            MainColor = CustomSelectionColor;
            MainVisible = true;
            MainRadius = CustomSelectionRadius;
            RadarRadius = CustomSelectionRadius;
            RadarColor = MainColor;
        } else if (AnyWeapon) {
            MinRange = 999999999;
            MaxRange = -999999999;
            for (Slot = 0; Slot <= 4; ++Slot) {
                if (SelectedWeapons[Slot]) {
                    Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Slot));
                    if (aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon) < MinRange) {
                        MinRange = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                    }
                    if (aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon) > MaxRange) {
                        MaxRange = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                    }
                }
            }
            MainColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
            MainRadius = MaxRange;
            MainVisible = true;
            if (MaxRange != MinRange) {
                InnerRadius = MinRange;
                InnerColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0);
                InnerVisible = true;
            }
            RadarColor = MainColor;
            RadarRadius = MainRadius;
        } else if (CursorObject == nullptr) {
            MainVisible = false;
            RadarColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
        } else if (pas::class_cast_if<aItem::TItem*>(CursorObject) != nullptr) {
            InnerRadius = aPlayer::GetPlayer()->GetRadarRange();
            InnerColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
            InnerVisible = true;
            if (aPlayer::GetPlayer()->GetCargoHook() != nullptr) {
                MainColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 255);
                MainRadius = aPlayer::GetPlayer()->GetCargoHookRange();
                MainVisible = true;
                RadarColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
            }
        } else if (CursorObject != nullptr) {
            MainColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
            MainVisible = true;
            RadarColor = MainColor;
        }
        GI_SpaceCircle::TSpaceCircleGI* MainCircle = ActionColorCircle;
        if (MainVisible) {
            MainCircle->UpdateAbsolutePosition();
        }
        Point = MainCircle->ToLocalPoint(MapControls->ToAbsolutePoint(MainCenter));
        if (MainCircle->Center.X != Point.X || MainCircle->Center.Y != Point.Y) {
            MainCircle->SetCenter(Point);
        }
        if (MainCircle->Radius != MainRadius) {
            MainCircle->SetRadius(MainRadius);
        }
        if (MainCircle->Color != MainColor) {
            MainCircle->Color = MainColor;
        }
        if (MainCircle->Active != MainVisible) {
            MainCircle->SetActive(MainVisible);
        }
        GI_SpaceCircle::TSpaceCircleGI* InnerCircle = WeaponColorCircles[0];
        if (InnerVisible) {
            InnerCircle->UpdateAbsolutePosition();
        }
        Point = InnerCircle->ToLocalPoint(MapControls->ToAbsolutePoint(InnerCenter));
        if (InnerCircle->Center.X != Point.X || InnerCircle->Center.Y != Point.Y) {
            InnerCircle->SetCenter(Point);
        }
        if (InnerCircle->Radius != InnerRadius) {
            InnerCircle->SetRadius(InnerRadius);
        }
        if (InnerCircle->Color != InnerColor) {
            InnerCircle->Color = InnerColor;
        }
        if (InnerCircle->Active != InnerVisible) {
            InnerCircle->SetActive(InnerVisible);
        }
        std::int32_t DirectRange = 0;
        if (AnyWeapon) {
            for (Slot = 0; Slot <= 4; ++Slot) {
                if (SelectedWeapons[Slot]) {
                    Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Slot));
                    if (!pas::in_range(Weapon->GetWeaponInfo()->ShotType, static_cast<std::int32_t>(aGalaxyStruct::wstTorpedo), static_cast<std::int32_t>(aGalaxyStruct::wstRocket))) {
                        RangeValue = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                        if (DirectRange > RangeValue) {
                            RangeMaximum = DirectRange;
                        } else {
                            RangeMaximum = RangeValue;
                        }
                        DirectRange = RangeMaximum;
                    }
                }
            }
        }
        std::int32_t ExtraIndex = -1;
        if (DirectRange != 0 && DirectRange != MainRadius && (static_cast<std::uint8_t>(InnerVisible ^ 1) || InnerRadius != DirectRange)) {
            ExtraIndex = 0;
        }
        for (Slot = 0; Slot <= 2; ++Slot) {
            ExtraCircle = WeaponColorCircles[Slot + 1];
            if (Slot == ExtraIndex) {
                ExtraCircle->UpdateAbsolutePosition();
            }
            Point = ExtraCircle->ToLocalPoint(MapControls->ToAbsolutePoint(MainCenter));
            if (ExtraCircle->Center.X != Point.X || ExtraCircle->Center.Y != Point.Y) {
                ExtraCircle->SetCenter(Point);
            }
            if (ExtraCircle->Radius != DirectRange) {
                ExtraCircle->SetRadius(DirectRange);
            }
            if (ExtraCircle->Color != MainColor) {
                ExtraCircle->Color = MainColor;
            }
            if ((Slot == ExtraIndex) != ExtraCircle->Active) {
                ExtraCircle->SetActive(Slot == ExtraIndex);
            }
        }
        GI_Circle::TCircleGI* SimpleCircle = ActionCircle;
        if (!GlobalsV::CircleAction) {
            SimpleCircle->SetActive(false);
        } else {
            Point = SimpleCircle->ToLocalPoint(MapControls->ToAbsolutePoint(MainCenter));
            SimpleCircle->UpdateAbsolutePosition();
            if (SimpleCircle->Center.X != Point.X || SimpleCircle->Center.Y != Point.Y) {
                SimpleCircle->SetCenter(Point);
            }
            if (SimpleCircle->Radius != MainRadius) {
                SimpleCircle->SetRadius(MainRadius);
            }
            if (SimpleCircle->Active != MainVisible) {
                SimpleCircle->SetActive(MainVisible);
            }
        }
        std::uint8_t RadarChanged = false;
        if (Globals::SpaceProcess->RadarCenter.X != RadarCenter.X || Globals::SpaceProcess->RadarCenter.Y != RadarCenter.Y) {
            Globals::SpaceProcess->RadarCenter = RadarCenter;
            RadarChanged = true;
        }
        if (Globals::SpaceProcess->ActionRange != RadarRadius) {
            Globals::SpaceProcess->ActionRange = RadarRadius;
            RadarChanged = true;
        }
        if (Globals::SpaceProcess->ActionColor != RadarColor) {
            Globals::SpaceProcess->ActionColor = RadarColor;
            RadarChanged = true;
        }
        if (RadarChanged) {
            Globals::SpaceProcess->Space->DrawMinimap();
        }
    }

    void TfStarMap::HideActionRanges() {
        {
            GI_SpaceCircle::TSpaceCircleGI* cpp_with = ActionColorCircle;
            cpp_with->SetActive(false);
        }
        {
            GI_SpaceCircle::TSpaceCircleGI* cpp_with_2 = WeaponColorCircles[0];
            cpp_with_2->SetActive(false);
        }
        {
            GI_SpaceCircle::TSpaceCircleGI* cpp_with_3 = WeaponColorCircles[1];
            cpp_with_3->SetActive(false);
        }
        {
            GI_SpaceCircle::TSpaceCircleGI* cpp_with_4 = WeaponColorCircles[2];
            cpp_with_4->SetActive(false);
        }
        {
            GI_SpaceCircle::TSpaceCircleGI* cpp_with_5 = WeaponColorCircles[3];
            cpp_with_5->SetActive(false);
        }
        {
            GI_Circle::TCircleGI* cpp_with_6 = ActionCircle;
            cpp_with_6->SetActive(false);
        }
    }

    void TfStarMap::RebuildTargetMarkers() {
        static const pas::Set<0, 255> TargetDamageFlags = pas::constant_set<pas::Set<0, 255>>({{aGalaxyStruct::dkScanBonus, aGalaxyStruct::dkDroidBlock}});
        static const pas::Set<0, 255> NoDamageFlags = pas::constant_set<pas::Set<0, 255>>({});
        std::int32_t J{};
        std::int32_t I{};
        aItem::TWeapon* Weapon{};
        EC_Struct::TPointF Point{};
        GI_Image::TImageGI* Image{};
        aItem::TItem* Item{};
        aShip::TShip* Target{};
        EC_Struct::TPointF BadgePoint{};
        ClearTargetMarkers();
        {
            const std::int32_t cpp_last = static_cast<std::int32_t>(aPlayer::GetPlayer()->WeaponCount);
            if (1 <= cpp_last) {
                for (I = 1; I <= cpp_last; ++I) {
                    Weapon = aPlayer::GetPlayer()->Weapons[I];
                    if (Weapon->Target != nullptr) {
                        if (pas::class_cast_if<aItem::TItem*>(Weapon->Target) != nullptr || pas::class_cast_if<aAsteroid::TAsteroid*>(Weapon->Target) != nullptr || pas::class_cast_if<aMissile::TMissile*>(Weapon->Target) != nullptr || pas::class_cast_if<aShip::TShip*>(Weapon->Target) != nullptr && static_cast<aShip::TShip*>(Weapon->Target)->InNormalSpace()) {
                            if (aItem::TItem* item = pas::class_cast_if<aItem::TItem*>(Weapon->Target)) {
                                Point = item->Position;
                            } else if (aAsteroid::TAsteroid* asteroid = pas::class_cast_if<aAsteroid::TAsteroid*>(Weapon->Target)) {
                                Point = asteroid->Position;
                            } else if (aMissile::TMissile* missile = pas::class_cast_if<aMissile::TMissile*>(Weapon->Target)) {
                                Point = missile->Position;
                            } else if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Weapon->Target)) {
                                Point = ship->Position;
                            }
                            Point = EC_Struct::AddPointsF(Point, EC_Struct::MakePointF(-4.0E+1f, -4.0E+1f));
                            for (auto cpp_range = pas::for_to<std::int32_t>(1, I - 1); cpp_range.next(J); ) {
                                if (aPlayer::GetPlayer()->Weapons[J]->Target == Weapon->Target) {
                                    Point.X = Point.X + 32.0L;
                                }
                            }
                            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapControls);
                            Image->SetImagePath(pas::concat_wide({u"GI,", Weapon->GetBitmapResourceName(), u"s"}));
                            Image->SetSize(Image->GetContentSize());
                            Image->SetOrigin(EC_Struct::HalfPoint(Image->ClientSize));
                            Image->SetPosition(EC_Struct::TruncatePointF(Point));
                            Image->SetDepthByName(u"Weapon"_wref.get());
                            Image->SetPositionModeW(true);
                            Image->UserValue = 100;
                            Image->UserIndex = I;
                            Image->MouseBlocking = true;
                            if (pas::class_cast_if<aShip::TShip*>(Weapon->Target) != nullptr) {
                                if (aPlayer::GetPlayer()->CanResolveObjectWithScanner(Weapon->Target) && Weapon->GetDamageFlags() * static_cast<aGalaxyStruct::TDamageFlagSet>(TargetDamageFlags) != static_cast<aGalaxyStruct::TDamageFlagSet>(NoDamageFlags)) {
                                    Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapControls);
                                    Image->SetImagePath(u"GI,Bm.Items.WeaponTarget"_w);
                                    Image->SetSize(Image->GetContentSize());
                                    Image->SetOrigin(EC_Struct::HalfPoint(Image->ClientSize));
                                    BadgePoint.X = Point.X - 1.0E+1L;
                                    BadgePoint.Y = Point.Y + 1.0E+1L;
                                    Image->SetPosition(EC_Struct::TruncatePointF(BadgePoint));
                                    Image->SetDepthByName(u"Hit"_wref.get());
                                    Image->SetPositionModeW(true);
                                    Image->UserValue = 100;
                                    Image->UserIndex = I;
                                    Image->MouseBlocking = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (aPlayer::GetPlayer()->GetHull()->InterceptorsEnabled) {
            Target = static_cast<aShip::TShip*>(aPlayer::GetPlayer()->GetHull()->InterceptorTarget);
            if (Target == nullptr) {
                Target = aPlayer::GetPlayer()->SelectInterceptorTarget();
            }
            if (Target != nullptr) {
                Point = Target->Position;
                Point = EC_Struct::AddPointsF(Point, EC_Struct::MakePointF(-4.0E+1f, -4.0E+1f));
                {
                    const std::int32_t cpp_last_2 = static_cast<std::int32_t>(aPlayer::GetPlayer()->WeaponCount);
                    if (1 <= cpp_last_2) {
                        for (J = 1; J <= cpp_last_2; ++J) {
                            if (aPlayer::GetPlayer()->Weapons[J]->Target == Target) {
                                Point.X = Point.X + 32.0L;
                            }
                        }
                    }
                }
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapControls);
                Image->SetImagePath(u"GI,Bm.Items.2Interceptors_s"_w);
                Image->SetSize(Image->GetContentSize());
                Image->SetOrigin(EC_Struct::HalfPoint(Image->ClientSize));
                Image->SetPosition(EC_Struct::TruncatePointF(Point));
                Image->SetDepthByName(u"Weapon"_wref.get());
                Image->SetPositionModeW(true);
                Image->UserValue = 100;
                Image->UserIndex = 0;
                Image->MouseBlocking = true;
            }
        }
        if (aPlayer::GetPlayer()->PickupTargets != nullptr) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->PickupTargets) - 1); cpp_range_2.next(I); ) {
                Item = pas::list_at<aItem::TItem>(aPlayer::GetPlayer()->PickupTargets, I);
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapControls);
                Image->SetImagePath(u"GAI,Bm.PI.ItemTakeAnim"_w);
                Image->SetSize(Image->GetContentSize());
                Image->SetOrigin(EC_Struct::HalfPoint(Image->ClientSize));
                Image->SetPosition(EC_Struct::TruncatePointF(EC_Struct::AddPointsF(Item->Position, EC_Struct::MakePointF(-2.0E+1f, 2.0E+1f))));
                Image->SetDepthByName(u"Weapon"_wref.get());
                Image->SetPositionModeW(true);
                Image->UserValue = 100;
                Image->MouseBlocking = true;
                Image->RestartPlayback();
            }
        }
    }

    void TfStarMap::ClearTargetMarkers() {
        GI_MessageLoop::TObjectGI* Control{};
        GI_MessageLoop::TObjectGI* NextControl = MapControls->FirstChild;
        while (NextControl != nullptr) {
            Control = NextControl;
            NextControl = NextControl->NextSibling;
            if (Control->UserValue == 100) {
                Control->SetActive(false);
                pas::free(Control);
            }
        }
    }

    void TfStarMap::UpdateActionCursor(std::uint8_t CanTake) {
        EC_Struct::TPointF Point{};
        pas::Object* Obj{};
        std::int32_t Index{};
        std::int32_t Range{};
        std::int32_t ActionResult{};
        aShip::PCustomShipInfo Info{};
        aItem::TWeapon* Weapon{};
        std::uint8_t AnyWeapon = false;
        for (Index = 0; Index <= 4; ++Index) {
            if (SelectedWeapons[Index]) {
                AnyWeapon = true;
                break;
            }
        }
        if (ScannerSelectionActive) {
            Obj = FindObjectAtCursor();
            if (aShip::TShip* ship = pas::class_cast_if<aShip::TShip*>(Obj); ship != nullptr && !(pas::class_cast_if<aRuins::TRuins*>(Obj) != nullptr) && !(pas::class_cast_if<aKling::TKling*>(Obj) != nullptr)) {
                pas::Extended cpp_left = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (cpp_left >= aMyFunction::PointDistanceSquared(ship->Position, aPlayer::GetPlayer()->Position)) {
                    if (!IsCursorImageSelected(u"ScanFull"sv)) {
                        SetCursorByName(u"ScanFull"_wref.get());
                    }
                } else if (!IsCursorImageSelected(u"ScanSmall"sv)) {
                    SetCursorByName(u"ScanSmall"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"ScanSmall"sv)) {
                SetCursorByName(u"ScanSmall"_wref.get());
            }
        } else if (TalkSelectionActive) {
            Obj = FindObjectAtCursor();
            if (aShip::TShip* ship_2 = pas::class_cast_if<aShip::TShip*>(Obj); ship_2 != nullptr && !(pas::class_cast_if<aRuins::TRuins*>(Obj) != nullptr)) {
                pas::Extended cpp_left_2 = pas::sqr(aPlayer::GetPlayer()->GetRadarRange());
                if (cpp_left_2 >= aMyFunction::PointDistanceSquared(ship_2->Position, aPlayer::GetPlayer()->Position)) {
                    if (!IsCursorImageSelected(u"TalkFull"sv)) {
                        SetCursorByName(u"TalkFull"_wref.get());
                    }
                } else if (!IsCursorImageSelected(u"TalkSmall"sv)) {
                    SetCursorByName(u"TalkSmall"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"TalkSmall"sv)) {
                SetCursorByName(u"TalkSmall"_wref.get());
            }
        } else if (InterceptorSelectionActive) {
            Obj = FindObjectAtCursor();
            if (aShip::TShip* ship_3 = pas::class_cast_if<aShip::TShip*>(Obj)) {
                Point = ship_3->Position;
                if (aMyFunction::PointDistanceSquared(Point, aPlayer::GetPlayer()->Position) <= pas::constant(static_cast<long double>(aGalaxyStruct::InterceptorTargetRangeSquared)) && ship_3->InterceptorPassesRemaining == 0) {
                    if (!IsCursorImageSelected(u"InterceptorsFull"sv)) {
                        SetCursorByName(u"InterceptorsFull"_wref.get());
                    }
                } else if (!IsCursorImageSelected(u"InterceptorsSmall"sv)) {
                    SetCursorByName(u"InterceptorsSmall"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"InterceptorsSmall"sv)) {
                SetCursorByName(u"InterceptorsSmall"_wref.get());
            }
        } else if (CustomSelectionActive) {
            Obj = FindObjectAtCursor();
            Point.X = 0.0f;
            Point.Y = 0.0f;
            // Native checks the cached CursorObject class but reads the current hit object's position.
            if (pas::class_cast_if<aShip::TShip*>(CursorObject) != nullptr) {
                Point = reinterpret_cast<aShip::TShip*>(Obj)->Position;
            } else if (pas::class_cast_if<aMissile::TMissile*>(CursorObject) != nullptr) {
                Point = reinterpret_cast<aMissile::TMissile*>(Obj)->Position;
            } else if (pas::class_cast_if<aAsteroid::TAsteroid*>(CursorObject) != nullptr) {
                Point = reinterpret_cast<aAsteroid::TAsteroid*>(Obj)->Position;
            } else if (pas::class_cast_if<aItem::TItem*>(CursorObject) != nullptr) {
                Point = reinterpret_cast<aItem::TItem*>(Obj)->Position;
            } else if (pas::class_cast_if<aPlanet::TPlanet*>(CursorObject) != nullptr) {
                Point = reinterpret_cast<aPlanet::TPlanet*>(Obj)->GetPosition();
            } else if (pas::class_cast_if<aGalaxy::THole*>(CursorObject) != nullptr) {
                if (reinterpret_cast<aGalaxy::THole*>(Obj)->Star1 == aGalaxy::PlayerStar) {
                    Point = reinterpret_cast<aGalaxy::THole*>(Obj)->Position1;
                } else {
                    Point = reinterpret_cast<aGalaxy::THole*>(Obj)->Position2;
                }
            }
            if (static_cast<long double>(aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, Point)) > CustomSelectionRadius * CustomSelectionRadius) {
                if (!IsCursorImageSelected(pas::view(CustomSelectionDeniedCursor))) {
                    SetCursorByName(CustomSelectionDeniedCursor);
                }
            } else {
                ActionResult = 0;
                if (CustomSelectionItem != nullptr) {
                    if (CustomSelectionItem->ScriptItem != nullptr) {
                        ActionResult = reinterpret_cast<aScript::TScriptItem*>(CustomSelectionItem->ScriptItem)->RunActionCode(aGalaxyStruct::satOnCustomTargettingCheck, aPlayer::GetPlayer(), Obj, nullptr, ActionResult);
                    }
                    if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(CustomSelectionItem) != nullptr) {
                        ActionResult = aScript::RunItemConfigActionCode(CustomSelectionItem, aGalaxyStruct::satOnCustomTargettingCheck, aPlayer::GetPlayer(), Obj, nullptr, ActionResult);
                    }
                } else {
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aPlayer::GetPlayer()->CustomShipInfos) - 1); cpp_range.next(Index); ) {
                        Info = pas::list_at<aShip::TCustomShipInfo>(aPlayer::GetPlayer()->CustomShipInfos, Index);
                        if (static_cast<std::uint8_t>(Info->DeleteQueued ^ 1) && Info->TypeName == CustomSelectionInfoName) {
                            ActionResult = aScript::RunCustomShipInfoActionCode(Info, aGalaxyStruct::satOnCustomTargettingCheck, aPlayer::GetPlayer(), Obj, nullptr, ActionResult);
                            break;
                        }
                    }
                }
                if (ActionResult > 0) {
                    if (!IsCursorImageSelected(pas::view(CustomSelectionAllowedCursor))) {
                        SetCursorByName(CustomSelectionAllowedCursor);
                    }
                } else if (!IsCursorImageSelected(pas::view(CustomSelectionDeniedCursor))) {
                    SetCursorByName(CustomSelectionDeniedCursor);
                }
            }
        } else if (AnyWeapon) {
            Obj = FindObjectAtCursor();
            if (pas::class_cast_if<aShip::TShip*>(Obj) != nullptr || pas::class_cast_if<aItem::TItem*>(Obj) != nullptr || pas::class_cast_if<aAsteroid::TAsteroid*>(Obj) != nullptr || pas::class_cast_if<aMissile::TMissile*>(Obj) != nullptr) {
                Range = -999999999;
                for (Index = 0; Index <= 4; ++Index) {
                    if (SelectedWeapons[Index]) {
                        Weapon = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::WeaponCategoryItemType, Index));
                        if (aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon) > Range) {
                            Range = aShip::TShip_GetWeaponActionRange(aPlayer::GetPlayer(), Weapon);
                        }
                    }
                }
                if (aShip::TShip* ship_4 = pas::class_cast_if<aShip::TShip*>(Obj)) {
                    Point = ship_4->Position;
                } else if (aItem::TItem* item = pas::class_cast_if<aItem::TItem*>(Obj)) {
                    Point = item->Position;
                } else if (aAsteroid::TAsteroid* asteroid = pas::class_cast_if<aAsteroid::TAsteroid*>(Obj)) {
                    Point = asteroid->Position;
                } else if (aMissile::TMissile* missile = pas::class_cast_if<aMissile::TMissile*>(Obj)) {
                    Point = missile->Position;
                }
                if (static_cast<long double>(aMyFunction::PointDistanceSquared(Point, aPlayer::GetPlayer()->Position)) <= pas::sqr(Range)) {
                    if (!IsCursorImageSelected(u"FireFull"sv)) {
                        SetCursorByName(u"FireFull"_wref.get());
                    }
                } else if (!IsCursorImageSelected(u"FireSmall"sv)) {
                    SetCursorByName(u"FireSmall"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"FireSmall"sv)) {
                SetCursorByName(u"FireSmall"_wref.get());
            }
        } else {
            Obj = FindObjectAtCursor();
            if (aItem::TItem* item_2 = pas::class_cast_if<aItem::TItem*>(Obj); item_2 != nullptr && aItem::CanCargoHookHandleItem(item_2, aPlayer::GetPlayer()) && static_cast<std::uint8_t>(aPlayer::GetPlayer()->IsRecentlyDroppedItem(item_2) ^ 1)) {
                if (!IsCursorImageSelected(u"Take"sv)) {
                    SetCursorByName(u"Take"_wref.get());
                }
            } else if (MapControls->Dragging) {
                if (!IsCursorImageSelected(u"Scroll"sv)) {
                    SetCursorByName(u"Scroll"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"Main"sv)) {
                SetCursorByName(u"Main"_wref.get());
            }
        }
    }

    void TfStarMap::UpdateWeaponPanelPosition() {
        if (WeaponPanelProgress < 0.0L) {
            WeaponPanelProgress = 0.0f;
        } else if (WeaponPanelProgress > 1.0L) {
            WeaponPanelProgress = 1.0f;
        }
        WeaponPanel->SetActive(WeaponPanelProgress > 0.0L);
        float Progress = PanelSlideCurve[System::Round(15.0L * WeaponPanelProgress)];
        WeaponPanel->SetPosition(ClassesImports::Point(WeaponPanel->LocalPosition.X, WeaponPanelRestTop + WeaponPanel->ClientSize.Y - System::Round(static_cast<long double>(WeaponPanel->ClientSize.Y) * Progress)));
        ShowSpacePanelButton->SetActive(static_cast<std::uint8_t>(Globals::StarMapWeaponPanelOpen ^ 1));
        HideSpacePanelButton->SetActive(Globals::StarMapWeaponPanelOpen);
    }

    // Stores the animation direction; native toggle passes -1 to hide and +1 to show.
    void TfStarMap::AnimateWeaponPanel(std::int32_t Target) {
        WeaponPanelTarget = Target;
        if (WeaponPanelTimer != nullptr) {
            CancelCallbackTimer(WeaponPanelTimer);
            WeaponPanelTimer = nullptr;
        }
        WeaponPanelTimer = ScheduleCallbackTimer(30, 30, pas::bind_method<&TfStarMap::AdvanceWeaponPanel>(this), 0);
        UpdateWeaponPanelPosition();
    }

    void TfStarMap::AdvanceWeaponPanel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (WeaponPanelTarget <= 0.0L) {
            WeaponPanelProgress = WeaponPanelProgress - 0.07L;
            if (WeaponPanelProgress <= 0.0L) {
                WeaponPanelProgress = 0.0f;
                if (WeaponPanelTimer != nullptr) {
                    CancelCallbackTimer(WeaponPanelTimer);
                    WeaponPanelTimer = nullptr;
                }
            }
        } else if (WeaponPanelTarget >= 1.0L) {
            WeaponPanelProgress = WeaponPanelProgress + 0.07L;
            if (WeaponPanelProgress >= 1.0L) {
                WeaponPanelProgress = 1.0f;
                if (WeaponPanelTimer != nullptr) {
                    CancelCallbackTimer(WeaponPanelTimer);
                    WeaponPanelTimer = nullptr;
                }
            }
        }
        UpdateWeaponPanelPosition();
    }

    void TfStarMap::UpdateSpacePanelPosition() {
        if (SpacePanelProgress < 0.0L) {
            SpacePanelProgress = 0.0f;
        } else if (SpacePanelProgress > 1.0L) {
            SpacePanelProgress = 1.0f;
        }
        SpacePanel->SetActive(SpacePanelProgress > 0.0L);
        float Progress = PanelSlideCurve[System::Round(15.0L * SpacePanelProgress)];
        SpacePanel->SetPosition(ClassesImports::Point(SpacePanel->LocalPosition.X, SpacePanelRestTop + SpacePanel->ClientSize.Y - System::Round(static_cast<long double>(SpacePanel->ClientSize.Y) * Progress)));
    }

    // Stores the signed animation direction.
    void TfStarMap::AnimateSpacePanel(std::int32_t Target) {
        SpacePanelTarget = Target;
        if (SpacePanelTimer != nullptr) {
            CancelCallbackTimer(SpacePanelTimer);
            SpacePanelTimer = nullptr;
        }
        SpacePanelTimer = ScheduleCallbackTimer(30, 30, pas::bind_method<&TfStarMap::AdvanceSpacePanel>(this), 0);
        UpdateSpacePanelPosition();
    }

    void TfStarMap::AdvanceSpacePanel(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (SpacePanelTarget <= 0.0L) {
            SpacePanelProgress = SpacePanelProgress - 0.07L;
            if (SpacePanelProgress <= 0.0L) {
                SpacePanelProgress = 0.0f;
                if (SpacePanelTimer != nullptr) {
                    CancelCallbackTimer(SpacePanelTimer);
                    SpacePanelTimer = nullptr;
                }
            }
        } else if (SpacePanelTarget >= 1.0L) {
            SpacePanelProgress = SpacePanelProgress + 0.07L;
            if (SpacePanelProgress >= 1.0L) {
                SpacePanelProgress = 1.0f;
                if (SpacePanelTimer != nullptr) {
                    CancelCallbackTimer(SpacePanelTimer);
                    SpacePanelTimer = nullptr;
                }
            }
        }
        UpdateSpacePanelPosition();
    }

    void TfStarMap::ProcessMouseWheel(std::uint32_t KeyState, WindowsSdk::TPoint Point, std::int32_t Delta) {
        std::int32_t FollowMode{};
        aShip::TShip* Ship{};
        if (Mode == smmOrders && (aPlayer::GetPlayer()->Order == aShip::soFollowShip || aRanger::PendingPlayerFollowTarget != nullptr)) {
            ClearPathOverlay(true);
            if (aRanger::PendingPlayerFollowTarget != nullptr) {
                Ship = aRanger::PendingPlayerFollowTarget;
                FollowMode = 1;
            } else if (static_cast<aShip::TFollowMode>(static_cast<std::uint8_t>(aPlayer::GetPlayer()->OrderStateData)) == aShip::fmMinWeaponRange) {
                Ship = pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget);
                FollowMode = 3;
            } else {
                Ship = pas::checked_cast<aShip::TShip*>(aPlayer::GetPlayer()->OrderTarget);
                FollowMode = 2;
            }
            if (Delta == WindowsSdk::WHEEL_DELTA) {
                ++FollowMode;
                if (FollowMode > 3) {
                    FollowMode = 1;
                }
            } else if (Delta == -WindowsSdk::WHEEL_DELTA) {
                --FollowMode;
                if (FollowMode < 1) {
                    FollowMode = 3;
                }
            }
            aGalaxy::Galaxy->CheckIntegrityChecksum(89);
            if (FollowMode == 1) {
                if (!aPlayer::GetPlayer()->CanSelectShipTarget(Ship)) {
                    FollowMode = 2;
                }
            }
            if (FollowMode == 1) {
                aPlayer::GetPlayer()->OrderNone(false);
                aRanger::PendingPlayerFollowTarget = Ship;
                ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.MoveAuto"_wref.get()));
            } else if (FollowMode == 2) {
                aRanger::PendingPlayerFollowTarget = nullptr;
                aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmFollowNear, false);
                ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.MoveNear"_wref.get()));
            } else if (FollowMode == 3) {
                aRanger::PendingPlayerFollowTarget = nullptr;
                aPlayer::GetPlayer()->OrderFollowShip(Ship, aShip::fmMinWeaponRange, false);
                ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.MoveShot"_wref.get()));
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(90);
            BuildShipPathOverlay(aPlayer::GetPlayer(), false, pas::WideString());
        }
    }

    void TfStarMap::StartTurnFilm() {
        DisplayedFilmObject = nullptr;
        MapControls->MouseMoveCallback = pas::bind_method<&TfStarMap::FilmMouseMove>(this);
        ScrollLeftHeld = false;
        ScrollRightHeld = false;
        ScrollUpHeld = false;
        ScrollDownHeld = false;
        FilmStepIndex = -1;
        TrailingEffectSteps = 0;
        BreakRequested = BreakOnNextFilm;
        Flag19F = true;
        MainPanel->Show();
        MainPanel->DisableNavigationButtons();
        MainPanel->RebuildMessageButtons(false);
        Globals::SwapTurnFilms();
        Globals::SpaceProcess->RadarRange = Globals::SecondaryFilm->RadarRange;
        Globals::SpaceProcess->ActionRange = Globals::SecondaryFilm->RadarRange;
        Globals::SpaceProcess->ActionColor = GR_Main::CurrentPixelFormat->PackRgbBytes(0, 255, 0);
        MainPanel->SetDateRange(Globals::SecondaryFilm->Turn, Globals::SecondaryFilm->Turn + 1);
        if (Globals::SecondaryFilm->PlayerCombatRecorded && static_cast<std::uint8_t>(BattleMusicSelected ^ 1) && static_cast<std::uint8_t>(GlobalsV::DoNotChangeMusicInBattle ^ 1)) {
            if (!GlobalsV::MusicInSpaceEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else {
                GR_Main::MusicManager->PlayCategory(u"Battle"_wref.get());
            }
        }
        CenterShipButton->DownCallback = pas::bind_method<&TfStarMap::CenterFilmShipClicked>(this);
        CenterShipButton->MouseEnterCallback = pas::bind_method<&TfStarMap::CenterFilmShipMouseEnter>(this);
        CenterShipButton->MouseLeaveCallback = pas::bind_method<&TfStarMap::CenterFilmShipMouseLeave>(this);
        {
            GI_GraphButton::TGraphButtonGI* PM_Break = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Break"sv));
            PM_Break->SetActive(true);
            PM_Break->SetDisabled(false);
            PM_Break->UpCallback = pas::bind_method<&TfStarMap::BreakTurnClicked>(this);
        }
        ContinueTurnCalculation = false;
        FilmProgressTimer = ScheduleCallbackTimer(50, 50, pas::bind_method<&TfStarMap::UpdateTurnCalculation>(this), 0);
        NextFilmCommand = Globals::SecondaryFilm->FirstCommand;
        Mode = smmTurnFilm;
        FilmFrameIntervalMs = fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->InitialActivity);
        if (Globals::SecondaryFilm->InitialActivity == Globals::SecondaryFilm->FinalActivity) {
            FilmFrameIntervalDelta = 0.0f;
        } else {
            std::int32_t cpp_left = fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->FinalActivity);
            FilmFrameIntervalDelta = pas::real_divide(cpp_left - fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->InitialActivity), 1.8E+2L);
        }
        if (FilmFrameTimer != nullptr) {
            CancelCallbackTimer(FilmFrameTimer);
            FilmFrameTimer = nullptr;
        }
        {
            std::int32_t round = System::Round(FilmFrameIntervalMs);
            std::int32_t round_2 = System::Round(FilmFrameIntervalMs);
            FilmFrameTimer = ScheduleCallbackTimer(round_2, round, pas::bind_method<&TfStarMap::AdvanceFilmFrame>(this), 0);
        }
        FilmCameraTargetUntilStep = 0;
        FilmCameraEventIndex = 0;
        FilmCameraTargetKind = 0;
        FilmCameraPosition = EC_Struct::PointToPointF(GetMapCenter());
        FilmCameraTarget = FilmCameraPosition;
        ReservedFilmState1E4 = 0;
        FilmCameraMoving = false;
        AdvanceFilmFrame(nullptr, 0);
        ReservedFilmState1A0 = 0;
        if (!IsCursorImageSelected(u"Main"sv)) {
            SetCursorByName(u"Main"_wref.get());
        }
        if (CenterShipButton->IsHovered()) {
            CenterFilmShipMouseEnter(nullptr);
        }
    }

    void TfStarMap::StopTurnFilm(std::uint8_t StopTurnProcessing) {
        DisplayedFilmObject = nullptr;
        Mode = smmInactive;
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Break"sv))->SetDisabled(true);
        if (StopTurnProcessing && ContinueTurnCalculation) {
            aCalc::WaitForTurnCalculationUI();
        }
        if (FilmProgressTimer != nullptr) {
            CancelCallbackTimer(FilmProgressTimer);
            FilmProgressTimer = nullptr;
        }
        if (FilmFrameTimer != nullptr) {
            CancelCallbackTimer(FilmFrameTimer);
            FilmFrameTimer = nullptr;
        }
        CenterShipButton->DownCallback = nullptr;
        MapControls->MouseMoveCallback = nullptr;
        MainPanel->ClearMessageButtons();
        MainPanel->Hide();
    }

    void TfStarMap::RestartTurnFilm() {
        DisplayedFilmObject = nullptr;
        MainPanel->RebuildMessageButtons(false);
        RebuildPartnerButtons();
        FilmStepIndex = -1;
        if (FilmProgressTimer != nullptr) {
            CancelCallbackTimer(FilmProgressTimer);
            FilmProgressTimer = nullptr;
        }
        FilmProgressTimer = ScheduleCallbackTimer(50, 50, pas::bind_method<&TfStarMap::UpdateTurnCalculation>(this), 0);
        ContinueTurnCalculation = false;
        BreakRequested = BreakOnNextFilm;
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Break"sv))->UpCallback = pas::bind_method<&TfStarMap::BreakTurnClicked>(this);
        Globals::SwapTurnFilms();
        MainPanel->SetDateRange(Globals::SecondaryFilm->Turn, Globals::SecondaryFilm->Turn + 1);
        NextFilmCommand = Globals::SecondaryFilm->FirstCommand;
        FilmFrameIntervalMs = fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->InitialActivity);
        if (Globals::SecondaryFilm->InitialActivity == Globals::SecondaryFilm->FinalActivity) {
            FilmFrameIntervalDelta = 0.0f;
        } else {
            std::int32_t cpp_left = fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->FinalActivity);
            FilmFrameIntervalDelta = pas::real_divide(cpp_left - fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->InitialActivity), 1.8E+2L);
        }
        if (FilmFrameTimer != nullptr) {
            CancelCallbackTimer(FilmFrameTimer);
            FilmFrameTimer = nullptr;
        }
        {
            std::int32_t round = System::Round(FilmFrameIntervalMs);
            std::int32_t round_2 = System::Round(FilmFrameIntervalMs);
            FilmFrameTimer = ScheduleCallbackTimer(round_2, round, pas::bind_method<&TfStarMap::AdvanceFilmFrame>(this), 0);
        }
        FilmCameraTargetUntilStep = 0;
        FilmCameraEventIndex = 0;
        FilmCameraTargetKind = 0;
        FilmCameraPosition = EC_Struct::PointToPointF(GetMapCenter());
        FilmCameraTarget = FilmCameraPosition;
        ReservedFilmState1E4 = 0;
        FilmCameraMoving = false;
        AdvanceFilmFrame(nullptr, 0);
        if (CenterShipButton->IsHovered()) {
            CenterFilmShipMouseEnter(nullptr);
        }
    }

    void TfStarMap::ProcessTurnFilm() {
        std::uint32_t WaitResult{};
        pas::Array<WindowsImports::THandle, 0, 1> Events{};
        void* EventList{};
        std::int32_t Index{};
        aScript::PScriptABRequest Request{};
        std::int32_t Stage = 0;
        try {
            if (Globals::TrailingFilmEffects != nullptr) {
                Stage = 1;
                Globals::TrailingFilmEffects->AdvanceEffects();
                if (Globals::TrailingFilmEffects->FirstEntry == nullptr) {
                    pas::free(Globals::TrailingFilmEffects);
                    Globals::TrailingFilmEffects = nullptr;
                }
            }
            Stage = 2;
            Globals::SpaceProcess->Space->AdvanceTimers();
            if (TrailingEffectSteps <= 0 && NextFilmCommand != nullptr) {
                Stage = 3;
                if (NextFilmCommand == nullptr) {
                    GR_Main::RaiseWideMessage(u"film step"_wref.get());
                } else if (NextFilmCommand->Kind == aEFilm::efcBeginTrailingEffects) {
                    Stage = 4;
                    if (Globals::TrailingFilmEffects == nullptr) {
                        Globals::TrailingFilmEffects = pas::construct_call<aEFilmEnd::TEFilmEnd>(aEFilmEnd::TEFilmEnd_Create);
                    }
                    Globals::TrailingFilmEffects->TakeTrailingEffects(Globals::SecondaryFilm);
                    ++FilmStepIndex;
                    NextFilmCommand = NextFilmCommand->Next;
                } else {
                    Stage = 5;
                    ++FilmStepIndex;
                    while (NextFilmCommand != nullptr) {
                        if (NextFilmCommand->Kind == aEFilm::efcBeginTrailingEffects) {
                            break;
                        }
                        if (!(NextFilmCommand->Kind == aEFilm::efcAttachObject && (aGalaxy::Galaxy->TerronToStarTurn & aGalaxyStruct::TerronTransformationFlag) != 0 && NextFilmCommand->Obj->GraphKey == u"Ruins.Terron")) {
                            if (NextFilmCommand->StepIndex > FilmStepIndex) {
                                break;
                            }
                            Stage = 6;
                            Globals::SecondaryFilm->ExecuteCommand(Globals::SpaceProcess, NextFilmCommand, false);
                        }
                        NextFilmCommand = NextFilmCommand->Next;
                    }
                }
            }
            Stage = 7;
            MainPanel->DateSlideProgress = pas::real_divide(FilmStepIndex, 2.0E+2L);
            if (MainPanel->DateSlideProgress > 1.0L) {
                MainPanel->DateSlideProgress = 1.0f;
            }
            MainPanel->RefreshDate();
            UpdateFilmCamera();
            Stage = 8;
            if (TrailingEffectSteps > 0) {
                Stage = 9;
                --TrailingEffectSteps;
                if (TrailingEffectSteps <= 0) {
                    Stage = 10;
                    StopTurnFilm(true);
                    GlobalsV::GameEndReason = GlobalsV::gerDefault;
                    GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
                    GR_DX::ReleaseAllTextureSurfaces();
                    RequestClose(1);
                }
            } else if (NextFilmCommand == nullptr) {
                Stage = 11;
                if (aPlayer::GetPlayer() == nullptr && Globals::SecondaryFilm->Turn >= aGalaxy::Galaxy->CurrentTurn) {
                    TrailingEffectSteps = 200;
                } else if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->InHyperspace && static_cast<std::uint8_t>(aPlayer::GetPlayer()->Graphic->IsAttachedToSpace() ^ 1)) {
                    Stage = 12;
                    StopTurnFilm(true);
                    if (aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
                        Stage = 13;
                        if (pas::class_cast_if<aGalaxy::THole*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
                            Globals::ArcadeBattleScreen->SelectedMapName = pas::checked_cast<aGalaxy::THole*>(aPlayer::GetPlayer()->OrderTarget)->ArcadeMapName;
                        } else {
                            Globals::ArcadeBattleScreen->SelectedMapName = pas::WideString();
                        }
                        if (pas::pos(u"SkipAB", Globals::ArcadeBattleScreen->SelectedMapName) != 1) {
                            GlobalsV::RequestedScreenId = GlobalsV::screenArcadeBattle;
                        } else {
                            GlobalsV::RequestedScreenId = GlobalsV::screenJump;
                            Globals::ArcadeBattleScreen->SelectedMapName = pas::WideString();
                        }
                        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Scripts) - 1); cpp_range.next(Index); ) {
                            aScript::TScript_RunTurnCode(pas::list_at<aScript::TScript>(aGalaxy::Galaxy->Scripts, Index));
                        }
                        if (pas::list_count(aScript::QueuedArcadeBattles) > 0) {
                            Stage = 14;
                            Request = pas::list_at<aScript::TScriptABRequest>(aScript::QueuedArcadeBattles, 0);
                            GlobalsV::RequestedScreenId = GlobalsV::screenArcadeBattle;
                            Globals::ArcadeBattleScreen->SelectedMapName = Request->MapName;
                            aScript::ScriptArcadeReturnScreenId = GlobalsV::CurrentScreenId;
                        }
                    } else {
                        Stage = 15;
                        if (aGalaxy::Galaxy != nullptr && aGalaxy::Galaxy->IsOldHyperspaceEnabled() && aPlayer::GetPlayer()->Order != aShip::soTeleport) {
                            GlobalsV::RequestedScreenId = GlobalsV::screenArcadeBattle;
                        } else {
                            GlobalsV::RequestedScreenId = GlobalsV::screenJump;
                        }
                    }
                    Stage = 16;
                    if (aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
                        LoadPanel->SelectBackgroundStyle(2);
                    } else {
                        LoadPanel->SelectBackgroundStyle(1);
                    }
                    LoadPanel->RefreshBackgroundImages();
                    LoadPanel->StartClosingShutters();
                    GR_DX::ReleaseAllTextureSurfaces();
                } else if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && aPlayer::GetPlayer()->IsOnPlanet() && static_cast<std::uint8_t>(aPlayer::GetPlayer()->Graphic->IsAttachedToSpace() ^ 1)) {
                    Stage = 17;
                    StopTurnFilm(true);
                    if (static_cast<std::uint8_t>(aCalc::IsTurnCalculationRunningUI() ^ 1) && aCalc::TurnCalculationPhase == ThreadCalc::tcpPlayerStarFinished) {
                        aCalc::QueueGalaxyTurnCalculation();
                    }
                    if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == aGalaxyStruct::oiUninhabited) {
                        if (aPlayer::GetPlayer()->GetEngine() != nullptr) {
                            double cpp_arg = aMyFunction::NextRandomUnitFloat(aPlayer::GetPlayer()->RandomState) * 15.0L;
                            aItem::TEquipment* engine = aPlayer::GetPlayer()->GetEngine();
                            aShip::TShip* player = aPlayer::GetPlayer();
                            aShip::TShip_ApplyItemDegradation(player, engine, aShip::idkUse, cpp_arg);
                        }
                        GlobalsV::RequestedScreenId = GlobalsV::screenPlanetNO;
                    } else {
                        GlobalsV::RequestedScreenId = GlobalsV::screenPlanet;
                    }
                    LoadPanel->SelectBackgroundStyle(0);
                    LoadPanel->RefreshBackgroundImages();
                    LoadPanel->StartClosingShutters();
                    GR_DX::ReleaseAllTextureSurfaces();
                } else if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aRanger::PlayerAutomaticControl ^ 1) && aPlayer::GetPlayer()->IsDockedToShip() && static_cast<std::uint8_t>(aPlayer::GetPlayer()->Graphic->IsAttachedToSpace() ^ 1)) {
                    Stage = 18;
                    StopTurnFilm(true);
                    if (static_cast<std::uint8_t>(aCalc::IsTurnCalculationRunningUI() ^ 1) && aCalc::TurnCalculationPhase == ThreadCalc::tcpPlayerStarFinished) {
                        aCalc::QueueGalaxyTurnCalculation();
                    }
                    if (aPlayer::GetPlayer()->DockedTo == aKling::TerronShip) {
                        Stage = 19;
                        PlanetBattleMapId = -1;
                        {
                            const std::int32_t cpp_last = Globals::RobotMapDefinitions.length() - 1;
                            if (0 <= cpp_last) {
                                for (Index = 0; Index <= cpp_last; ++Index) {
                                    if (Globals::RobotMapDefinitions[Index].Terron) {
                                        PlanetBattleMapId = Globals::RobotMapDefinitions[Index].Id;
                                        break;
                                    }
                                }
                            }
                        }
                        if (PlanetBattleMapId < 0) {
                            GR_Main::RaiseWideMessage(u"terron map"_wref.get());
                        }
                        PlanetBattleState = 1;
                        GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
                        LoadPanel->SelectBackgroundStyle(3);
                        LoadPanel->RefreshBackgroundImages();
                        LoadPanel->StartClosingShutters();
                    } else {
                        Stage = 20;
                        GlobalsV::RequestedScreenId = GlobalsV::screenRuinsTalk;
                        LoadPanel->SelectBackgroundStyle(0);
                        LoadPanel->RefreshBackgroundImages();
                        LoadPanel->StartClosingShutters();
                        GR_DX::ReleaseAllTextureSurfaces();
                    }
                } else {
                    Stage = 21;
                    if (static_cast<std::uint8_t>(ContinueTurnCalculation ^ 1) && aGalaxy::ShouldContinuePlayerTravel()) {
                        if (static_cast<std::uint8_t>(BreakRequested ^ 1) || aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->IsOnPlanet()) {
                            aCalc::WaitForTurnCalculationUI();
                            UpdateTurnCalculation(nullptr, 0);
                        }
                    }
                    if (ContinueTurnCalculation) {
                        Stage = 22;
                        if (aCalc::IsTurnCalculationRunningUI()) {
                            Events[0] = Globals::TurnCalculationThread->IdleEvent;
                            Events[1] = Globals::TalkRequestEvent;
                            EventList = &Events;
                            WaitResult = WindowsSdk::WaitForMultipleObjects(2u, static_cast<WindowsSdk::PWOHandleArray>(EventList), 0, WindowsSdk::INFINITE);
                        } else {
                            WaitResult = WindowsSdk::WAIT_OBJECT_0;
                        }
                        Stage = 23;
                        if (WaitResult == WindowsSdk::WAIT_FAILED) {
                            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error GetLastError()=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
                        } else if (WaitResult == WindowsSdk::WAIT_OBJECT_0) {
                            Stage = 24;
                            QueueInterfaceImages();
                            if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1)) {
                                UpdateTerronTransformation();
                                aCalc::QueueGalaxyTurnCalculation();
                            }
                            if (BreakRequested) {
                                RestartTurnFilm();
                                BreakRequested = true;
                            } else {
                                RestartTurnFilm();
                            }
                        } else if (WaitResult == WindowsSdk::WAIT_OBJECT_0 + 1) {
                            Stage = 25;
                            StopTurnFilm(false);
                            RunTalkDialogs();
                            WaitForTurnOrTalk();
                        }
                    } else {
                        Stage = 26;
                        StopTurnFilm(true);
                        QueueInterfaceImages();
                        if (aCalc::IsTurnCalculationRunningUI()) {
                            aCalc::WaitForTurnCalculationUI();
                        }
                        Stage = 27;
                        aCalc::QueuePlayerStarPreparation();
                        Stage = 28;
                        aScript::StartScriptRequestThread();
                        Events[0] = Globals::TurnCalculationThread->IdleEvent;
                        Events[1] = Globals::TalkRequestEvent;
                        EventList = &Events;
                        WaitResult = WindowsSdk::WaitForMultipleObjects(2u, static_cast<WindowsSdk::PWOHandleArray>(EventList), 0, WindowsSdk::INFINITE);
                        if (Globals::TurnCalculationThread->IdleEvent == 0 || WaitResult == WindowsSdk::WAIT_OBJECT_0) {
                            AnimateSpacePanelOnResume = true;
                            StartOrderMode();
                        } else if (WaitResult == WindowsSdk::WAIT_FAILED) {
                            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error GetLastError()=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
                        } else if (WaitResult == WindowsSdk::WAIT_OBJECT_0 + 1) {
                            Stage = 29;
                            RunTalkDialogs();
                            Stage = 30;
                            WaitForTurnOrTalk();
                        }
                    }
                }
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error in TfStarMap.FilmStep, label = ", SysUtils::IntToStr(Stage)})));
            } else {
                throw;
            }
        }
    }

    void TfStarMap::AdvanceFilmFrame(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        ProcessTurnFilm();
        if (FilmFrameIntervalDelta != 0.0L) {
            FilmFrameIntervalMs = static_cast<long double>(FilmFrameIntervalMs) + FilmFrameIntervalDelta;
            if (FilmFrameIntervalDelta < 0.0L) {
                if (static_cast<long double>(FilmFrameIntervalMs) < fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->FinalActivity)) {
                    FilmFrameIntervalMs = fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->FinalActivity);
                    FilmFrameIntervalDelta = 0.0f;
                }
            } else if (static_cast<long double>(FilmFrameIntervalMs) > fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->FinalActivity)) {
                FilmFrameIntervalMs = fStarMap::GetTurnFilmFrameInterval(Globals::SecondaryFilm->FinalActivity);
                FilmFrameIntervalDelta = 0.0f;
            }
            if (FilmFrameTimer != nullptr) {
                CancelCallbackTimer(FilmFrameTimer);
                FilmFrameTimer = nullptr;
            }
            {
                std::int32_t round = System::Round(FilmFrameIntervalMs);
                std::int32_t round_2 = System::Round(FilmFrameIntervalMs);
                FilmFrameTimer = ScheduleCallbackTimer(round_2, round, pas::bind_method<&TfStarMap::AdvanceFilmFrame>(this), 0);
            }
        }
    }

    void TfStarMap::UpdateTurnCalculation(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (BreakRequested && aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->IsOnPlanet() ^ 1)) {
            ContinueTurnCalculation = false;
        } else if (static_cast<std::uint8_t>(aCalc::IsTurnCalculationRunningUI() ^ 1) && (GlobalsV::BeginCalcNextTurn < 1.0L && FilmStepIndex > System::Round(GlobalsV::BeginCalcNextTurn * 2.0E+2L) || GlobalsV::BeginCalcNextTurn >= 1.0L && FilmStepIndex > System::Round(ThreadCalc::AdaptiveBeginCalcNextTurn * 2.0E+2L))) {
            ContinueTurnCalculation = aGalaxy::ShouldContinuePlayerTravel();
            if (ContinueTurnCalculation && aCalc::TurnCalculationPhase == ThreadCalc::tcpGalaxyFinished) {
                MainPanel->TryAutoTurnSave();
                aCalc::QueuePlayerStarTurnCalculation();
            }
            if (FilmProgressTimer != nullptr) {
                CancelCallbackTimer(FilmProgressTimer);
                FilmProgressTimer = nullptr;
            }
        }
    }

    void TfStarMap::BreakTurnClicked(GI_MessageLoop::TObjectGI* Sender) {
        ShowLargeHelp(GR_Main::LookupLocalizedTextByKey(u"Help.MoveBreak"_wref.get()));
        BreakRequested = true;
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Break"sv))->SetDisabled(true);
    }

    void TfStarMap::UpdateFilmCamera() {
        std::int32_t BestPriority{};
        std::int32_t BestIndex{};
        WindowsSdk::TPoint Center{};
        WindowsSdk::TPoint HalfSize{};
        EC_Struct::TPointF Vector{};
        EC_Struct::TPointF TopLeft{};
        EC_Struct::TPointF BottomRight{};
        float Distance{};
        float Speed{};
        WindowsSdk::TRect ViewRect{};
        if (FilmStepIndex > 200) {
            return;
        }
        if (FilmStepIndex == 0 && static_cast<std::uint8_t>(Globals::SecondaryFilm->ForceCameraMovement ^ 1)) {
            HalfSize.X = pas::shr(GR_Main::GameScreenWidth, 2);
            HalfSize.Y = pas::shr(GR_Main::GameScreenHeight, 2);
            FilmCameraMoving = static_cast<long double>(FilmCameraPosition.X) - HalfSize.X > Globals::SecondaryFilm->CameraAnchor.X || static_cast<long double>(FilmCameraPosition.X) + HalfSize.X <= Globals::SecondaryFilm->CameraAnchor.X || static_cast<long double>(FilmCameraPosition.Y) - HalfSize.Y > Globals::SecondaryFilm->CameraAnchor.Y || static_cast<long double>(FilmCameraPosition.Y) + HalfSize.Y <= Globals::SecondaryFilm->CameraAnchor.Y;
        }
        if (FilmCameraMoving || Globals::SecondaryFilm->ForceCameraMovement || Globals::SecondaryFilm->CameraEventCount != 0) {
            if (FilmCameraTargetUntilStep <= FilmStepIndex) {
                BestPriority = -1;
                BestIndex = -1;
                while (FilmCameraEventIndex < Globals::SecondaryFilm->CameraEventCount) {
                    if (Globals::SecondaryFilm->CameraEvents[FilmCameraEventIndex].StepIndex >= FilmStepIndex + FilmCameraLookAheadSteps) {
                        break;
                    }
                    if (Globals::SecondaryFilm->CameraEvents[FilmCameraEventIndex].Priority > BestPriority) {
                        BestPriority = Globals::SecondaryFilm->CameraEvents[FilmCameraEventIndex].Priority;
                        BestIndex = FilmCameraEventIndex;
                    }
                    ++FilmCameraEventIndex;
                }
                if (BestIndex >= 0) {
                    FilmCameraTargetUntilStep = Globals::SecondaryFilm->CameraEvents[BestIndex].StepIndex + FilmCameraLookAheadSteps;
                    HalfSize.X = pas::shr(GR_Main::GameScreenWidth, 1) - GR_Main::GiScalePixels(100);
                    HalfSize.Y = pas::shr(GR_Main::GameScreenHeight, 1) - GR_Main::GiScalePixels(100);
                    ViewRect.Left = System::Round(static_cast<long double>(FilmCameraPosition.X) - HalfSize.X);
                    ViewRect.Top = System::Round(static_cast<long double>(FilmCameraPosition.Y) - HalfSize.Y);
                    ViewRect.Right = System::Round(static_cast<long double>(FilmCameraPosition.X) + HalfSize.X);
                    ViewRect.Bottom = System::Round(static_cast<long double>(FilmCameraPosition.Y) + HalfSize.Y);
                    {
                        aEFilm::TEFilmCameraEvent& cpp_with = Globals::SecondaryFilm->CameraEvents[BestIndex];
                        if (static_cast<long double>(ViewRect.Left) > cpp_with.StartPosition.X || static_cast<long double>(ViewRect.Right) <= cpp_with.StartPosition.X || static_cast<long double>(ViewRect.Top) > cpp_with.StartPosition.Y || static_cast<long double>(ViewRect.Bottom) <= cpp_with.StartPosition.Y || static_cast<long double>(ViewRect.Left) > cpp_with.EndPosition.X || static_cast<long double>(ViewRect.Right) <= cpp_with.EndPosition.X || static_cast<long double>(ViewRect.Top) > cpp_with.EndPosition.Y || static_cast<long double>(ViewRect.Bottom) <= cpp_with.EndPosition.Y) {
                            Vector.X = static_cast<long double>(cpp_with.EndPosition.X) - cpp_with.StartPosition.X;
                            Vector.Y = static_cast<long double>(cpp_with.EndPosition.Y) - cpp_with.StartPosition.Y;
                            Distance = System::Sqrt(pas::sqr(static_cast<pas::Extended>(Vector.X)) + pas::sqr(static_cast<pas::Extended>(Vector.Y)));
                            if (Distance == 0.0L) {
                                FilmCameraTarget = cpp_with.StartPosition;
                            } else {
                                Vector.X = pas::real_divide(Vector.X, Distance);
                                Vector.Y = pas::real_divide(Vector.Y, Distance);
                                Distance = pas::real_min<float>(Distance, static_cast<float>(GR_Main::GiScalePixels(600)));
                                FilmCameraTarget.X = cpp_with.StartPosition.X + static_cast<long double>(Vector.X) * Distance * 0.5L;
                                FilmCameraTarget.Y = cpp_with.StartPosition.Y + static_cast<long double>(Vector.Y) * Distance * 0.5L;
                            }
                        }
                    }
                    FilmCameraTargetKind = 1;
                } else {
                    FilmCameraTargetKind = 0;
                    FilmCameraTargetUntilStep = FilmStepIndex + FilmCameraLookAheadSteps;
                }
            }
            if (FilmCameraTargetKind == 0) {
                if (Globals::SpaceProcess->RadarCenter.X != Globals::SecondaryFilm->CameraAnchor.X || Globals::SpaceProcess->RadarCenter.Y != Globals::SecondaryFilm->CameraAnchor.Y) {
                    Vector.X = pas::shr(GR_Main::GameScreenWidth, 1) - GR_Main::GiScalePixels(150);
                    Vector.Y = pas::shr(GR_Main::GameScreenHeight, 1) - GR_Main::GiScalePixels(150);
                    TopLeft = EC_Struct::SubtractPointsF(Globals::SpaceProcess->RadarCenter, Vector);
                    BottomRight = EC_Struct::AddPointsF(Globals::SpaceProcess->RadarCenter, Vector);
                    if (aMyFunction::SegmentIntersectsRectEdges(Globals::SpaceProcess->RadarCenter, Globals::SecondaryFilm->CameraAnchor, TopLeft, BottomRight, Vector)) {
                        FilmCameraTarget = Vector;
                    } else {
                        FilmCameraTarget = Globals::SecondaryFilm->CameraAnchor;
                    }
                } else {
                    FilmCameraTarget = Globals::SpaceProcess->RadarCenter;
                }
            }
            if (FilmStepIndex != 0 && Globals::FilmCameraFollow) {
                Center = GetMapCenter();
                if (Center.X != System::Round(static_cast<long double>(FilmCameraPosition.X) - FilmCameraShakeOffset.X) || Center.Y != System::Round(static_cast<long double>(FilmCameraPosition.Y) - FilmCameraShakeOffset.Y)) {
                    FilmCameraPosition.X = static_cast<long double>(Center.X) - FilmCameraShakeOffset.X;
                    FilmCameraPosition.Y = static_cast<long double>(Center.Y) - FilmCameraShakeOffset.Y;
                }
                Vector.X = static_cast<long double>(FilmCameraTarget.X) - FilmCameraPosition.X;
                Vector.Y = static_cast<long double>(FilmCameraTarget.Y) - FilmCameraPosition.Y;
                Distance = System::Sqrt(pas::sqr(static_cast<pas::Extended>(Vector.X)) + pas::sqr(static_cast<pas::Extended>(Vector.Y)));
                if (Distance <= 2.0L) {
                    FilmCameraMoving = false;
                    FilmCameraPosition = FilmCameraTarget;
                } else {
                    Vector.X = pas::real_divide(Vector.X, Distance);
                    Vector.Y = pas::real_divide(Vector.Y, Distance);
                    Speed = pas::real_max<pas::Extended>(1.0L, Distance * 0.1L);
                    if (Speed > FilmCameraSpeed) {
                        Speed = pas::real_min<pas::Extended>(static_cast<pas::Extended>(Speed), FilmCameraSpeed * 1.1L);
                    } else if (Speed < FilmCameraSpeed) {
                        Speed = pas::real_max<pas::Extended>(static_cast<pas::Extended>(Speed), FilmCameraSpeed * 0.9L);
                    }
                    FilmCameraSpeed = Speed;
                    FilmCameraPosition.X = FilmCameraPosition.X + static_cast<long double>(Vector.X) * Speed;
                    FilmCameraPosition.Y = FilmCameraPosition.Y + static_cast<long double>(Vector.Y) * Speed;
                }
                FilmCameraShakeOffset.X = System::Sin(FilmCameraShakeAngle) * GR_Main::GiScalePixels(30);
                FilmCameraShakeOffset.Y = -System::Cos(FilmCameraShakeAngle) * GR_Main::GiScalePixels(30);
                FilmCameraShakeAngle = FilmCameraShakeAngle + pas::constant(SystemImports::Pi / 256.0L);
                {
                    std::int32_t round = System::Round(static_cast<long double>(FilmCameraPosition.Y) + FilmCameraShakeOffset.Y);
                    std::int32_t round_2 = System::Round(static_cast<long double>(FilmCameraPosition.X) + FilmCameraShakeOffset.X);
                    SetMapCenter(ClassesImports::Point(round_2, round));
                }
            }
        }
    }

    void TfStarMap::CenterFilmShipClicked(GI_MessageLoop::TObjectGI* Sender) {
        Globals::FilmCameraFollow = true;
        FilmCameraMoving = true;
    }

    void TfStarMap::CenterFilmShipMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        if (ThreadCalc::IsTurnCalculationRunning() && (Globals::TurnCalculationThread->Job == ThreadCalc::tcjPreparePlayerStar || static_cast<std::int32_t>(Globals::TurnCalculationThread->Job) == 5)) {
            return;
        }
        if (aPlayer::GetPlayer() != nullptr) {
            ShowFilmObjectInfo(aPlayer::GetPlayer()->Graphic, aPlayer::GetPlayer()->Id);
        }
    }

    void TfStarMap::CenterFilmShipMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        ShowFilmObjectInfo(nullptr, 0u);
    }

    void TfStarMap::FilmMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        SE_Space::TObjectSE* Obj{};
        std::uint32_t ObjectId{};
        if (Sender->IsOccludedAtPoint(Point) || (KeyState & WindowsSdk::MK_RBUTTON) == WindowsSdk::MK_RBUTTON) {
            return;
        }
        if (!TypesImports::PtInRect(GR_Main::ScrollInteriorRect, Point)) {
            ShowFilmObjectInfo(nullptr, 0u);
        } else if (static_cast<std::uint8_t>(ScrollLeftHeld ^ 1) && static_cast<std::uint8_t>(ScrollRightHeld ^ 1) && static_cast<std::uint8_t>(ScrollUpHeld ^ 1) && static_cast<std::uint8_t>(ScrollDownHeld ^ 1)) {
            Obj = FindFilmObjectAtCursor(ObjectId);
            ShowFilmObjectInfo(Obj, ObjectId);
        }
    }

    // Returns a borrowed scene object; sets ObjectId to zero on failure.
    SE_Space::TObjectSE* TfStarMap::FindFilmObjectAtCursor(std::uint32_t& ObjectId) {
        SE_Space::TObjectSE* Result{};
        WindowsSdk::TPoint Point{};
        if (MainPanel->BackgroundImage->HitTestPixel(GetCursorPoint())) {
            Result = nullptr;
            ObjectId = 0u;
            return Result;
        }
        Point = MapControls->ToLocalPoint(GetCursorPoint());
        aEFilm::TEFilmObj* Obj = Globals::SecondaryFilm->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Missile::TMissileSE*>(Obj->SceneObject) != nullptr && Obj->SceneObject->HitTestCursor()) {
                Result = Obj->SceneObject;
                ObjectId = Obj->ObjectId;
                return Result;
            }
            Obj = Obj->Next;
        }
        Obj = Globals::SecondaryFilm->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj->SceneObject) != nullptr && Obj->SceneObject->HitTestCursor()) {
                Result = Obj->SceneObject;
                ObjectId = Obj->ObjectId;
                return Result;
            }
            Obj = Obj->Next;
        }
        Obj = Globals::SecondaryFilm->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Container::TContainerSE*>(Obj->SceneObject) != nullptr && Obj->SceneObject->HitTestCursor()) {
                Result = Obj->SceneObject;
                ObjectId = Obj->ObjectId;
                return Result;
            }
            Obj = Obj->Next;
        }
        Obj = Globals::SecondaryFilm->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj->SceneObject) != nullptr && Obj->SceneObject->HitTestCursor()) {
                Result = Obj->SceneObject;
                ObjectId = Obj->ObjectId;
                return Result;
            }
            Obj = Obj->Next;
        }
        Obj = Globals::SecondaryFilm->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Asteroid::TAsteroidSE*>(Obj->SceneObject) != nullptr && Obj->SceneObject->HitTestCursor()) {
                Result = Obj->SceneObject;
                ObjectId = Obj->ObjectId;
                return Result;
            }
            Obj = Obj->Next;
        }
        Obj = Globals::SecondaryFilm->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj->SceneObject) != nullptr) {
                if (static_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->IsRuins) {
                    if (Obj->SceneObject->HitTestCursor()) {
                        Result = Obj->SceneObject;
                        ObjectId = Obj->ObjectId;
                        return Result;
                    }
                } else if (pas::sqr(static_cast<long double>(Point.X) - Obj->SceneObject->Position.X) + pas::sqr(static_cast<long double>(Point.Y) - Obj->SceneObject->Position.Y) < pas::sqr(static_cast<SE_Planet::TPlanetSE*>(Obj->SceneObject)->Radius)) {
                    Result = Obj->SceneObject;
                    ObjectId = Obj->ObjectId;
                    return Result;
                }
            }
            Obj = Obj->Next;
        }
        if (pas::sqr(pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo)->StarRadius) > Point.X * Point.X + Point.Y * Point.Y) {
            Obj = Globals::SecondaryFilm->FirstObject;
            while (Obj != nullptr) {
                if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Star::TStarSE*>(Obj->SceneObject) != nullptr) {
                    Result = Obj->SceneObject;
                    ObjectId = Obj->ObjectId;
                    return Result;
                }
                Obj = Obj->Next;
            }
        }
        Obj = Globals::SecondaryFilm->FirstObject;
        while (Obj != nullptr) {
            if (Obj->SceneObject != nullptr && pas::class_cast_if<SE_Hole::THoleSE*>(Obj->SceneObject) != nullptr) {
                if (static_cast<long double>(aMyFunction::PointDistanceSquared(Obj->SceneObject->Position, EC_Struct::PointToPointF(Point))) < pas::sqr(static_cast<SE_Hole::THoleSE*>(Obj->SceneObject)->HitRadius)) {
                    Result = Obj->SceneObject;
                    ObjectId = Obj->ObjectId;
                    return Result;
                }
            }
            Obj = Obj->Next;
        }
        Result = nullptr;
        ObjectId = 0u;
        return Result;
    }

    // Nil hides the object panels. ObjectId resolves recorded information in the current film.
    void TfStarMap::ShowFilmObjectInfo(SE_Space::TObjectSE* Obj, std::uint32_t ObjectId) {
        static const pas::Set<0, 255> WearableItemTypes = pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}});
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t RowHeight{};
        std::int32_t RowX{};
        std::uint32_t IconInset{};
        std::int32_t NameWidth{};
        std::int32_t DetailWidth{};
        std::int32_t StatusCount{};
        aEObjInfo::PEPlanetInfo Planet{};
        aEObjInfo::PEPlanetInfo ListedPlanet{};
        aEObjInfo::PEShipInfo Ship{};
        aEObjInfo::PEItemInfo Item{};
        aEObjInfo::PEAsteroidInfo Asteroid{};
        aEObjInfo::PEMissileInfo Missile{};
        pas::WideString Text{};
        GI_Panel::TPanelGI* Panel{};
        pas::List* Objects{};
        pas::List* Records{};
        float Distance{};
        aGalaxyStruct::TOwnerId OwnerId{};
        aEObjInfo::TEObjInfo* Snapshot{};
        aEFilm::TEFilmObj* FilmObject{};
        pas::WideString ImagePath{};
        pas::WideString ColorTag{};
        GI_MessageLoop::TObjectGI* Child{};
        GI_Label::TLabelGI* DamageName{};
        GI_Label::TLabelGI* DamageValue{};
        std::uint8_t IsCivilized{};
        GI_MessageLoop::TObjectGI* ActivePanel{};
        std::int32_t BarWidth{};
        std::int32_t CapWidth{};
        std::int32_t MinimumWidth{};
        aEObjInfo::PECustomSystemInfo CustomInfo{};
        pas::WideString Images{};
        Item = nullptr;
        Asteroid = nullptr;
        Planet = nullptr;
        Ship = nullptr;
        Missile = nullptr;
        if (pas::class_cast_if<SE_Star::TStarSE*>(Obj) != nullptr && aKling::TerronShip != nullptr && aKling::TerronShip->CurrentStar->Id == ObjectId && aGalaxy::Galaxy->TerronToStarTurn >= aGalaxyStruct::TerronTransformationFlag) {
            ShowObjectInfo(aKling::TerronShip);
            return;
        }
        Snapshot = pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo);
        if (Obj != nullptr) {
            if (pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj) != nullptr) {
                Planet = pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo)->FindPlanet(ObjectId);
            }
            if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr || pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj) != nullptr) {
                Ship = pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo)->FindShip(ObjectId);
            }
            if (pas::class_cast_if<SE_Container::TContainerSE*>(Obj) != nullptr) {
                Item = pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo)->FindItem(ObjectId);
            }
            if (pas::class_cast_if<SE_Asteroid::TAsteroidSE*>(Obj) != nullptr) {
                Asteroid = pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo)->FindAsteroid(ObjectId);
            }
            if (pas::class_cast_if<SE_Missile::TMissileSE*>(Obj) != nullptr) {
                Missile = pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo)->FindMissile(ObjectId);
            }
        }
        if (Obj == nullptr || pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj) != nullptr && Planet == nullptr || pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr && Ship == nullptr || pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj) != nullptr && Ship == nullptr || pas::class_cast_if<SE_Container::TContainerSE*>(Obj) != nullptr && Item == nullptr || pas::class_cast_if<SE_Asteroid::TAsteroidSE*>(Obj) != nullptr && Asteroid == nullptr || pas::class_cast_if<SE_Missile::TMissileSE*>(Obj) != nullptr && Missile == nullptr) {
            InfoWindow->SetActive(false);
            ItemInfoWindow->SetActive(false);
            ShipInfoPanel->SetActive(false);
            PlanetInfoPanel->SetActive(false);
            StarInfoWindow->SetActive(false);
            StandardInfoPanel->SetActive(false);
            DisplayedFilmObject = nullptr;
            DisplayedObject = nullptr;
            return;
        }
        if (pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj) != nullptr || pas::class_cast_if<SE_Hole::THoleSE*>(Obj) != nullptr || pas::class_cast_if<SE_Container::TContainerSE*>(Obj) != nullptr && static_cast<long double>(aMyFunction::PointDistance(Obj->Position, Globals::SpaceProcess->RadarCenter)) > Globals::SecondaryFilm->RadarRange || (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr || pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj) != nullptr) && static_cast<long double>(aMyFunction::PointDistance(Obj->Position, Globals::SpaceProcess->RadarCenter)) > Globals::SecondaryFilm->RadarRange || pas::class_cast_if<SE_Asteroid::TAsteroidSE*>(Obj) != nullptr && static_cast<long double>(aMyFunction::PointDistance(Obj->Position, Globals::SpaceProcess->RadarCenter)) > Globals::SecondaryFilm->RadarRange || pas::class_cast_if<SE_Missile::TMissileSE*>(Obj) != nullptr && static_cast<long double>(aMyFunction::PointDistance(Obj->Position, Globals::SpaceProcess->RadarCenter)) > Globals::SecondaryFilm->RadarRange) {
            {
                std::int64_t cpp_left_2 = System::Round(Obj->Position.Y);
                std::int32_t cpp_arg = cpp_left_2 - GetMapCenter().Y;
                std::int64_t cpp_left = System::Round(Obj->Position.X);
                std::int32_t cpp_arg_2 = cpp_left - GetMapCenter().X;
                HitObjectPosition = ClassesImports::Point(cpp_arg_2, cpp_arg);
            }
            HitObjectSize = Obj->Size;
            if (DisplayedFilmObject != Obj) {
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(true);
                if (pas::class_cast_if<SE_Container::TContainerSE*>(Obj) != nullptr) {
                    GetByName(u"InfoStdGB"sv)->SetActive(false);
                    {
                        GI_Image::TImageGI* InfoStdImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoStdImage"sv));
                        InfoStdImage->SetActive(true);
                        InfoStdImage->SetImagePath(Item->ImagePath);
                        InfoStdImage->SetImageKindX(GI_Main::ikxCenter);
                        InfoStdImage->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter = InfoStdImage->GetVisualCenter();
                            Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                            InfoStdImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                        }
                    }
                    if (pas::in_range(Item->ItemType, static_cast<std::int32_t>(aConst::t_Food), static_cast<std::int32_t>(aConst::t_Narcotics))) {
                        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv))->SetText(Item->Name);
                        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv))->SetText(Item->InfoText);
                    } else {
                        {
                            const pas::WideString& wrapTextInColor = ([&] {
                                pas::WideString localizedText = aConst::LocalizedText(u"FormInfo.ContainerName"_wref.get());
                                pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                                return aMyFunction::WrapTextInColor(pas::view(std::move(localizedText)), pas::view(std::move(infoNameColorTag)));
                            }());
                            GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            cpp_arg_3->SetText(wrapTextInColor);
                        }
                        {
                            const pas::WideString& localizedText_2 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                            GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                            cpp_arg_4->SetText(localizedText_2);
                        }
                    }
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemSize"sv))->SetText(u"???"_wref.get());
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemPrice"sv))->SetText(u"???"_wref.get());
                    {
                        GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_6 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_7 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_7, cpp_arg_6, cpp_arg_5, true, true, 0);
                    }
                } else if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr || pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj) != nullptr) {
                    if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr) {
                        GetByName(u"InfoStdImage"sv)->SetActive(false);
                        {
                            GI_GraphBuf::TGraphBufGI* InfoStdGB = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                            ImagePath = Ship->PortraitImage;
                            InfoStdGB->SetActive(ImagePath != u"");
                            if (InfoStdGB->Active) {
                                InfoStdGB->SourceHasPerPixelAlpha = true;
                                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 1, u","sv), InfoStdGB->GraphBuf);
                                if (InfoStdGB->ClientSize.X < InfoStdGB->GraphBuf->Width || InfoStdGB->ClientSize.Y < InfoStdGB->GraphBuf->Height) {
                                    if (static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Height)) {
                                        InfoStdGB->GraphBuf->RescaleRgba(InfoStdGB->ClientSize.X, System::Round(pas::real_divide(InfoStdGB->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Height)), 5);
                                    } else {
                                        InfoStdGB->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB->GraphBuf->Width)), InfoStdGB->ClientSize.Y, 5);
                                    }
                                }
                                InfoStdGB->SetImageKindX(GI_Main::ikxCenter);
                                InfoStdGB->SetImageKindY(GI_Main::ikyCenter);
                                {
                                    Types::TPoint visualCenter_2 = InfoStdGB->GetVisualCenter();
                                    Types::TPoint itemImageCenter_2 = Globals::ShipScreen->ItemImageCenter;
                                    InfoStdGB->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_2, visualCenter_2));
                                }
                            }
                        }
                    } else {
                        GetByName(u"InfoStdImage"sv)->SetActive(false);
                        {
                            GI_GraphBuf::TGraphBufGI* InfoStdGB_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                            InfoStdGB_2->SetActive(true);
                            InfoStdGB_2->SourceHasPerPixelAlpha = true;
                            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::checked_cast<SE_Ruins::TRuinsSE*>(Obj)->StaticImagePath), 1, u","sv), InfoStdGB_2->GraphBuf);
                            if (InfoStdGB_2->ClientSize.X < InfoStdGB_2->GraphBuf->Width || InfoStdGB_2->ClientSize.Y < InfoStdGB_2->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Height)) {
                                    InfoStdGB_2->GraphBuf->RescaleRgba(InfoStdGB_2->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_2->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Height)), 5);
                                } else {
                                    InfoStdGB_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_2->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_2->GraphBuf->Width)), InfoStdGB_2->ClientSize.Y, 5);
                                }
                            }
                            {
                                Types::TPoint visualCenter_3 = InfoStdGB_2->GetVisualCenter();
                                Types::TPoint itemImageCenter_3 = Globals::ShipScreen->ItemImageCenter;
                                InfoStdGB_2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_3, visualCenter_3));
                            }
                        }
                    }
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv))->SetText(Ship->FullName);
                    {
                        const pas::WideString& localizedText_3 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                        GI_Label::TLabelGI* cpp_arg_8 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_8->SetText(localizedText_3);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_9 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_10 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_11 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_11, cpp_arg_10, cpp_arg_9, true, true, 0);
                    }
                } else if (pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj) != nullptr) {
                    IsCivilized = pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId) && (aPlanet::MainPiratePlanet == nullptr || Planet->Id != aPlanet::MainPiratePlanet->Id);
                    if (IsCivilized) {
                        if (Planet->OwnerId == aGalaxyStruct::oiPirate) {
                            IsCivilized = Planet->Faction == pas::concat_wide({aConst::OwnerInfo[aGalaxyStruct::oiPirate].InternalName, aConst::RaceToSys(Planet->RaceId)});
                        } else {
                            IsCivilized = Planet->Faction == aConst::OwnerInfo[Planet->OwnerId].InternalName;
                        }
                    }
                    if (IsCivilized) {
                        InfoWindow->SetActive(false);
                        ItemInfoWindow->SetActive(false);
                        ShipInfoPanel->SetActive(false);
                        PlanetInfoPanel->SetActive(true);
                        StarInfoWindow->SetActive(false);
                        StandardInfoPanel->SetActive(false);
                        {
                            const pas::WideString& wrapTextInColor_2 = aMyFunction::WrapTextInColor(pas::view(Planet->Name), pas::view(aMyFunction::InfoNameColorTag));
                            GI_Label::TLabelGI* cpp_arg_12 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetName"sv));
                            cpp_arg_12->SetText(wrapTextInColor_2);
                        }
                        if (pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(Planet->OwnerId)) {
                            GI_Image::TImageGI* InfoPlanetEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoPlanetEmRace"sv));
                            InfoPlanetEmRace->SetImagePath(aConst::GetFactionEmblemPath(Planet->Faction));
                            InfoPlanetEmRace->SetImageKindX(GI_Main::ikxCenter);
                            InfoPlanetEmRace->SetImageKindY(GI_Main::ikyCenter);
                            InfoPlanetEmRace->SetActive(true);
                        } else {
                            GetByName(u"InfoPlanetEmRace"sv)->SetActive(false);
                        }
                        {
                            GI_GraphBuf::TGraphBufGI* InfoPlanetImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoPlanetImage"sv));
                            InfoPlanetImage->SourceHasPerPixelAlpha = true;
                            pas::checked_cast<SE_Planet::TPlanetSE*>(Obj)->RenderToBuffer(this, InfoPlanetImage->GraphBuf, false);
                            if (InfoPlanetImage->ClientSize.X < InfoPlanetImage->GraphBuf->Width || InfoPlanetImage->ClientSize.Y < InfoPlanetImage->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)) {
                                    InfoPlanetImage->GraphBuf->RescaleRgba(InfoPlanetImage->ClientSize.X, System::Round(pas::real_divide(InfoPlanetImage->ClientSize.X, static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)), 5);
                                } else {
                                    InfoPlanetImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoPlanetImage->ClientSize.Y, static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width)), InfoPlanetImage->ClientSize.Y, 5);
                                }
                            }
                        }
                        if (aPlanet::MainPiratePlanet != nullptr && Planet->Id == aPlanet::MainPiratePlanet->Id) {
                            pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetOwner"sv))->SetText(aConst::OwnerInfo[Planet->OwnerId].DisplayName);
                        } else {
                            GI_Label::TLabelGI* cpp_arg_13 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetOwner"sv));
                            const pas::WideString& displayName = aConst::OwnerInfo[aConst::RaceToOwner(Planet->RaceId)].DisplayName;
                            cpp_arg_13->SetText(displayName);
                        }
                        {
                            const pas::WideString& int64ToStr = pas::wide_int64_to_str(System::Round(pas::real_divide(Planet->Population, 1.0E+3L)));
                            GI_Label::TLabelGI* cpp_arg_14 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetPop"sv));
                            cpp_arg_14->SetText(int64ToStr);
                        }
                        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetEco"sv))->SetText(aConst::PlanetEconomyInfo[Planet->Economy].DisplayName);
                        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetGov"sv))->SetText(aConst::PlanetGovernmentMarket[Planet->Government].DisplayName);
                        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetRel"sv))->SetText(aConst::RelationInfo[Planet->Relation].DisplayName);
                        {
                            GI_Label::TLabelGI* cpp_arg_15 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPOwner"sv));
                            GI_Label::TLabelGI* cpp_arg_16 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetOwner"sv));
                            GI_Label::TLabelGI* cpp_arg_17 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPPop"sv));
                            GI_Label::TLabelGI* cpp_arg_18 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetPop"sv));
                            GI_Label::TLabelGI* cpp_arg_19 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPEco"sv));
                            GI_Label::TLabelGI* cpp_arg_20 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetEco"sv));
                            GI_Label::TLabelGI* cpp_arg_21 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPGov"sv));
                            GI_Label::TLabelGI* cpp_arg_22 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetGov"sv));
                            GI_Label::TLabelGI* cpp_arg_23 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"IPRel"sv));
                            GI_Label::TLabelGI* cpp_arg_24 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetRel"sv));
                            GI_MessageLoop::TObjectGI* byName = GetByName(u"InfoPlanetEmRace"sv);
                            GI_Label::TLabelGI* cpp_arg_25 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPlanetName"sv));
                            GI_Window::TWindowGI* cpp_arg_26 = pas::checked_cast<GI_Window::TWindowGI*>(PlanetInfoPanel);
                            Globals::ShipScreen->LayoutObjectInfo(cpp_arg_26, cpp_arg_25, cpp_arg_15, cpp_arg_16, cpp_arg_17, cpp_arg_18, cpp_arg_19, cpp_arg_20, cpp_arg_21, cpp_arg_22, cpp_arg_23, cpp_arg_24, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, byName, true, 0);
                        }
                    } else {
                        GetByName(u"InfoStdImage"sv)->SetActive(false);
                        {
                            GI_GraphBuf::TGraphBufGI* InfoStdGB_3 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                            InfoStdGB_3->SetActive(true);
                            InfoStdGB_3->SourceHasPerPixelAlpha = true;
                            pas::checked_cast<SE_Planet::TPlanetSE*>(Obj)->RenderToBuffer(this, InfoStdGB_3->GraphBuf, false);
                            if (InfoStdGB_3->ClientSize.X < InfoStdGB_3->GraphBuf->Width || InfoStdGB_3->ClientSize.Y < InfoStdGB_3->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Height)) {
                                    InfoStdGB_3->GraphBuf->RescaleRgba(InfoStdGB_3->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_3->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Height)), 5);
                                } else {
                                    InfoStdGB_3->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_3->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_3->GraphBuf->Width)), InfoStdGB_3->ClientSize.Y, 5);
                                }
                            }
                            {
                                Types::TPoint visualCenter_4 = InfoStdGB_3->GetVisualCenter();
                                Types::TPoint itemImageCenter_4 = Globals::ShipScreen->ItemImageCenter;
                                InfoStdGB_3->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_4, visualCenter_4));
                            }
                        }
                        {
                            const pas::WideString& wrapTextInColor_3 = aMyFunction::WrapTextInColor(pas::view(Planet->Name), pas::view(aMyFunction::InfoNameColorTag));
                            GI_Label::TLabelGI* cpp_arg_27 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            cpp_arg_27->SetText(wrapTextInColor_3);
                        }
                        if (Planet->OwnerId == aGalaxyStruct::oiUninhabited) {
                            Text = aConst::LocalizedText(u"Planet.NotCivil.Info.TextAboutPlanet"_wref.get());
                            if (Planet->UnexploredWater > 0) {
                                aMyFunction::ReplaceTextToken(Text, u"<Water>"_w, pas::wide_int_to_str(Planet->UnexploredWater), aMyFunction::TextHighlightColorTag);
                            } else {
                                aMyFunction::ReplaceTextToken(Text, u"<Water>"_w, u"-"_w, pas::WideString());
                            }
                            if (Planet->UnexploredLand > 0) {
                                aMyFunction::ReplaceTextToken(Text, u"<Land>"_w, pas::wide_int_to_str(Planet->UnexploredLand), aMyFunction::TextHighlightColorTag);
                            } else {
                                aMyFunction::ReplaceTextToken(Text, u"<Land>"_w, u"-"_w, pas::WideString());
                            }
                            if (Planet->UnexploredHills > 0) {
                                aMyFunction::ReplaceTextToken(Text, u"<Hill>"_w, pas::wide_int_to_str(Planet->UnexploredHills), aMyFunction::TextHighlightColorTag);
                            } else {
                                aMyFunction::ReplaceTextToken(Text, u"<Hill>"_w, u"-"_w, pas::WideString());
                            }
                            if (aPlayer::GetPlayer() != nullptr) {
                                if (aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0) {
                                    Text = pas::concat_wide({Text, u"\r\n", Planet->TreasureHint});
                                }
                            }
                        } else if (aPlanet::MainPiratePlanet != nullptr && Planet->Id == aPlanet::MainPiratePlanet->Id) {
                            if (Planet->OwnerId == aGalaxyStruct::oiPirate) {
                                Text = aConst::LocalizedText(u"Planet.MainPiratePlanet.Info.TextAboutPlanet"_wref.get());
                            } else {
                                Text = aConst::LocalizedText(u"Planet.MainPiratePlanet.Info.TextAboutPlanetAlt"_wref.get());
                            }
                        } else {
                            IsCivilized = Planet->OwnerId == aGalaxyStruct::oiDominator;
                            if (IsCivilized) {
                                IsCivilized = Planet->Faction == aConst::DominatorSeriesNames[aGalaxyStruct::dsBlazer] || Planet->Faction == aConst::DominatorSeriesNames[aGalaxyStruct::dsTerron] || Planet->Faction == aConst::DominatorSeriesNames[aGalaxyStruct::dsKeller];
                            }
                            if (IsCivilized) {
                                Text = aConst::LocalizedText(u"Planet.Kling.Info.TextAboutPlanet"_wref.get());
                            } else {
                                Text = aConst::LocalizedText(pas::concat_wide({u"Planet.", Planet->Faction, u".Info.TextAboutPlanet"}));
                            }
                            aMyFunction::ReplaceTextToken(Text, u"<Race>"_w, aConst::OwnerInfo[aConst::RaceToOwner(Planet->RaceId)].DisplayName, aMyFunction::TextHighlightColorTag);
                        }
                        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv))->SetText(Text);
                        {
                            GI_Label::TLabelGI* cpp_arg_28 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                            GI_Label::TLabelGI* cpp_arg_29 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                            GI_Window::TWindowGI* cpp_arg_30 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                            fShip2::TfShip2::LayoutItemInfo(cpp_arg_30, cpp_arg_29, cpp_arg_28, true, true, 0);
                        }
                    }
                } else if (pas::class_cast_if<SE_Asteroid::TAsteroidSE*>(Obj) != nullptr) {
                    GetByName(u"InfoStdImage"sv)->SetActive(false);
                    {
                        GI_GraphBuf::TGraphBufGI* InfoStdGB_4 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                        InfoStdGB_4->SetActive(true);
                        InfoStdGB_4->SourceHasPerPixelAlpha = true;
                        GI_GAI::LoadGaiFrameToGraphBuf(pas::checked_cast<SE_Asteroid::TAsteroidSE*>(Obj)->ImagePath, InfoStdGB_4->GraphBuf, ObjectId);
                        if (InfoStdGB_4->ClientSize.X < InfoStdGB_4->GraphBuf->Width || InfoStdGB_4->ClientSize.Y < InfoStdGB_4->GraphBuf->Height) {
                            if (static_cast<std::uint32_t>(InfoStdGB_4->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_4->GraphBuf->Height)) {
                                InfoStdGB_4->GraphBuf->RescaleRgba(InfoStdGB_4->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_4->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_4->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_4->GraphBuf->Height)), 5);
                            } else {
                                InfoStdGB_4->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_4->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_4->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_4->GraphBuf->Width)), InfoStdGB_4->ClientSize.Y, 5);
                            }
                        }
                        {
                            Types::TPoint visualCenter_5 = InfoStdGB_4->GetVisualCenter();
                            Types::TPoint itemImageCenter_5 = Globals::ShipScreen->ItemImageCenter;
                            InfoStdGB_4->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_5, visualCenter_5));
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor_4 = aMyFunction::WrapTextInColor(pas::view(Asteroid->Name), pas::view(aMyFunction::InfoNameColorTag));
                        GI_Label::TLabelGI* cpp_arg_31 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        cpp_arg_31->SetText(wrapTextInColor_4);
                    }
                    {
                        const pas::WideString& localizedText_4 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                        GI_Label::TLabelGI* cpp_arg_32 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_32->SetText(localizedText_4);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_33 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_34 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_35 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_35, cpp_arg_34, cpp_arg_33, true, true, 0);
                    }
                } else if (pas::class_cast_if<SE_Missile::TMissileSE*>(Obj) != nullptr) {
                    GetByName(u"InfoStdImage"sv)->SetActive(false);
                    {
                        GI_GraphBuf::TGraphBufGI* InfoStdGB_5 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                        InfoStdGB_5->SetActive(true);
                        InfoStdGB_5->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.", Obj->GraphKey, u"_", GR_Main::GiResourceSuffix(), u"i"}), InfoStdGB_5->GraphBuf);
                        {
                            Types::TPoint visualCenter_6 = InfoStdGB_5->GetVisualCenter();
                            Types::TPoint itemImageCenter_6 = Globals::ShipScreen->ItemImageCenter;
                            InfoStdGB_5->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_6, visualCenter_6));
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor_5 = aMyFunction::WrapTextInColor(pas::view(Missile->Name), pas::view(aMyFunction::InfoNameColorTag));
                        GI_Label::TLabelGI* cpp_arg_36 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        cpp_arg_36->SetText(wrapTextInColor_5);
                    }
                    {
                        const pas::WideString& localizedText_5 = aConst::LocalizedText(u"FormInfo.ObjOutOfRange"_wref.get());
                        GI_Label::TLabelGI* cpp_arg_37 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_37->SetText(localizedText_5);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_38 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_39 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_40 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_40, cpp_arg_39, cpp_arg_38, true, true, 0);
                    }
                } else if (pas::class_cast_if<SE_Hole::THoleSE*>(Obj) != nullptr) {
                    GetByName(u"InfoStdImage"sv)->SetActive(false);
                    {
                        GI_GraphBuf::TGraphBufGI* InfoStdGB_6 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                        InfoStdGB_6->SetActive(true);
                        InfoStdGB_6->SourceHasPerPixelAlpha = true;
                        GI_GAI::LoadGaiFrameToGraphBuf(pas::checked_cast<SE_Hole::THoleSE*>(Obj)->ImagePath, InfoStdGB_6->GraphBuf, 32u);
                        if (InfoStdGB_6->ClientSize.X < InfoStdGB_6->GraphBuf->Width || InfoStdGB_6->ClientSize.Y < InfoStdGB_6->GraphBuf->Height) {
                            if (static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Height)) {
                                InfoStdGB_6->GraphBuf->RescaleRgba(InfoStdGB_6->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_6->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Height)), 5);
                            } else {
                                InfoStdGB_6->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_6->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_6->GraphBuf->Width)), InfoStdGB_6->ClientSize.Y, 5);
                            }
                        }
                        {
                            Types::TPoint visualCenter_7 = InfoStdGB_6->GetVisualCenter();
                            Types::TPoint itemImageCenter_7 = Globals::ShipScreen->ItemImageCenter;
                            InfoStdGB_6->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_7, visualCenter_7));
                        }
                    }
                    {
                        const pas::WideString& wrapTextInColor_6 = ([&] {
                            pas::WideString localizedText_6 = aConst::LocalizedText(reinterpret_cast<SE_Hole::THoleSE*>(Obj)->NameTextPath);
                            pas::WideString infoNameColorTag_2 = aMyFunction::InfoNameColorTag;
                            return aMyFunction::WrapTextInColor(pas::view(std::move(localizedText_6)), pas::view(std::move(infoNameColorTag_2)));
                        }());
                        GI_Label::TLabelGI* cpp_arg_41 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        cpp_arg_41->SetText(wrapTextInColor_6);
                    }
                    {
                        const pas::WideString& localizedText_7 = aConst::LocalizedText(reinterpret_cast<SE_Hole::THoleSE*>(Obj)->InfoTextPath);
                        GI_Label::TLabelGI* cpp_arg_42 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        cpp_arg_42->SetText(localizedText_7);
                    }
                    {
                        GI_Label::TLabelGI* cpp_arg_43 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                        GI_Label::TLabelGI* cpp_arg_44 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                        GI_Window::TWindowGI* cpp_arg_45 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                        fShip2::TfShip2::LayoutItemInfo(cpp_arg_45, cpp_arg_44, cpp_arg_43, true, true, 0);
                    }
                }
                DisplayedFilmObject = Obj;
            }
        } else if (pas::class_cast_if<SE_Container::TContainerSE*>(Obj) != nullptr) {
            {
                std::int64_t cpp_left_4 = System::Round(Obj->Position.Y);
                std::int32_t cpp_arg_46 = cpp_left_4 - GetMapCenter().Y;
                std::int64_t cpp_left_3 = System::Round(Obj->Position.X);
                std::int32_t cpp_arg_47 = cpp_left_3 - GetMapCenter().X;
                HitObjectPosition = ClassesImports::Point(cpp_arg_47, cpp_arg_46);
            }
            HitObjectSize = Obj->Size;
            if (DisplayedFilmObject != Obj) {
                DisplayedFilmObject = Obj;
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(true);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(false);
                {
                    GI_Image::TImageGI* InfoItemImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoItemImage"sv));
                    InfoItemImage->SetImagePath(Item->ImagePath);
                    InfoItemImage->SetImageKindX(GI_Main::ikxCenter);
                    InfoItemImage->SetImageKindY(GI_Main::ikyCenter);
                    {
                        Types::TPoint visualCenter_8 = InfoItemImage->GetVisualCenter();
                        Types::TPoint itemImageCenter_8 = Globals::ShipScreen->ItemImageCenter;
                        InfoItemImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_8, visualCenter_8));
                    }
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv))->SetText(Item->Name);
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"sv))->SetText(Item->InfoText);
                {
                    const pas::WideString& intToStr = pas::wide_int_to_str(Item->Weight);
                    GI_Label::TLabelGI* cpp_arg_48 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemSize"sv));
                    cpp_arg_48->SetText(intToStr);
                }
                {
                    const pas::WideString& intToStr_2 = pas::wide_int_to_str(Item->Cost);
                    GI_Label::TLabelGI* cpp_arg_49 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemPrice"sv));
                    cpp_arg_49->SetText(intToStr_2);
                }
                {
                    GI_Image::TImageGI* InfoItemEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoItemEmRace"sv));
                    InfoItemEmRace->SetImagePath(aConst::GetFactionEmblemPath(Item->Faction));
                    InfoItemEmRace->SetImageKindX(GI_Main::ikxCenter);
                    InfoItemEmRace->SetImageKindY(GI_Main::ikyCenter);
                }
                if (!(pas::contains(WearableItemTypes, static_cast<std::uint8_t>(Item->ItemType)) || Item->ItemType == aConst::t_Hull)) {
                    {
                        GI_Image::TImageGI* InfoDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"sv));
                        InfoDurable->Parent->Parent->SetActive(false);
                    }
                    MinimumWidth = 0;
                } else {
                    if (Item->ItemType == aConst::t_Hull) {
                        double real_max = pas::real_max<double>(0.1, Item->Fragility);
                        BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(Item->Weight, aConst::HullBaseSize), real_max)) * 64.0L);
                    } else {
                        BarWidth = System::Round(pas::real_divide(64.0L, pas::real_max<double>(0.1, Item->Fragility)));
                    }
                    BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
                    {
                        GI_Image::TImageGI* InfoDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableLeft"sv));
                        CapWidth = InfoDurableLeft->GetContentSize().X;
                        MinimumWidth = CapWidth * 2 + BarWidth + InfoDurableLeft->LocalPosition.X + InfoDurableLeft->Parent->LocalPosition.X + InfoDurableLeft->Parent->Parent->LocalPosition.X * 2;
                    }
                    {
                        GI_Image::TImageGI* InfoDurable_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"sv));
                        InfoDurable_2->Parent->Parent->SetActive(true);
                        InfoDurable_2->Parent->Parent->SetSize(ClassesImports::Point(CapWidth * 2 + BarWidth, InfoDurable_2->Parent->Parent->ClientSize.Y));
                        InfoDurable_2->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoDurable_2->Parent->Parent->ClientSize.Y));
                        InfoDurable_2->SetPosition(([&] {
                            std::int32_t cpp_arg_50 = ([&] {
                                std::int64_t cpp_left_5 = System::Round(BarWidth * pas::real_divide(Item->ConditionPercent, 1.0E+2L));
                                return cpp_left_5 - (InfoDurable_2->GetContentSize().X - 5);
                            }());
                            std::int32_t y = InfoDurable_2->LocalPosition.Y;
                            return ClassesImports::Point(cpp_arg_50, y);
                        }()));
                    }
                    {
                        GI_Image::TImageGI* InfoDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableRight"sv));
                        {
                            std::int32_t cpp_arg_51 = BarWidth + CapWidth - InfoDurableRight->GetContentSize().X;
                            std::int32_t y_2 = InfoDurableRight->LocalPosition.Y;
                            InfoDurableRight->SetPosition(ClassesImports::Point(cpp_arg_51, y_2));
                        }
                        InfoDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoDurableRight->Parent->LocalPosition.Y));
                        InfoDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableRight->Parent->ClientSize.Y));
                    }
                    {
                        GI_Image::TImageGI* InfoDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableBack"sv));
                        {
                            std::int32_t cpp_arg_52 = BarWidth + 1 - InfoDurableBack->GetContentSize().X;
                            std::int32_t y_3 = InfoDurableBack->LocalPosition.Y;
                            InfoDurableBack->SetPosition(ClassesImports::Point(cpp_arg_52, y_3));
                        }
                        InfoDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoDurableBack->Parent->ClientSize.Y));
                    }
                }
                {
                    GI_Label::TLabelGI* cpp_arg_53 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"sv));
                    GI_Label::TLabelGI* cpp_arg_54 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv));
                    fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, cpp_arg_54, cpp_arg_53, true, true, MinimumWidth);
                }
                GetByName(u"InfoItemSize"sv)->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
                GetByName(u"InfoItemPrice"sv)->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
                GetByName(u"InfoItemEmRace"sv)->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
                {
                    GI_Label::TLabelGI* cpp_arg_55 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"sv));
                    GI_Label::TLabelGI* cpp_arg_56 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"sv));
                    fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, cpp_arg_56, cpp_arg_55, true, true, 0);
                }
            }
        } else if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr || pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj) != nullptr) {
            {
                std::int64_t cpp_left_7 = System::Round(Obj->Position.Y);
                std::int32_t cpp_arg_57 = cpp_left_7 - GetMapCenter().Y;
                std::int64_t cpp_left_6 = System::Round(Obj->Position.X);
                std::int32_t cpp_arg_58 = cpp_left_6 - GetMapCenter().X;
                HitObjectPosition = ClassesImports::Point(cpp_arg_58, cpp_arg_57);
            }
            HitObjectSize = Obj->Size;
            if (DisplayedFilmObject != Obj) {
                DisplayedFilmObject = Obj;
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(true);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(false);
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv))->SetText(Ship->FullName);
                if (Ship->Faction != u"None") {
                    GI_Image::TImageGI* InfoShipEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipEmRace"sv));
                    InfoShipEmRace->SetImagePath(aConst::GetFactionEmblemPath(Ship->Faction));
                    InfoShipEmRace->SetImageKindX(GI_Main::ikxCenter);
                    InfoShipEmRace->SetImageKindY(GI_Main::ikyCenter);
                    InfoShipEmRace->SetActive(true);
                } else {
                    GetByName(u"InfoShipEmRace"sv)->SetActive(false);
                }
                if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr) {
                    GI_GraphBuf::TGraphBufGI* InfoShipImage2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoShipImage2"sv));
                    ImagePath = Ship->PortraitImage;
                    InfoShipImage2->SetActive(ImagePath != u"");
                    if (InfoShipImage2->Active) {
                        InfoShipImage2->SourceHasPerPixelAlpha = true;
                        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(ImagePath), 1, u","sv), InfoShipImage2->GraphBuf);
                        if (InfoShipImage2->ClientSize.X < InfoShipImage2->GraphBuf->Width || InfoShipImage2->ClientSize.Y < InfoShipImage2->GraphBuf->Height) {
                            if (static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)) {
                                InfoShipImage2->GraphBuf->RescaleRgba(InfoShipImage2->ClientSize.X, System::Round(pas::real_divide(InfoShipImage2->ClientSize.X, static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)), 5);
                            } else {
                                InfoShipImage2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoShipImage2->ClientSize.Y, static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoShipImage2->GraphBuf->Width)), InfoShipImage2->ClientSize.Y, 5);
                            }
                        }
                        InfoShipImage2->SetImageKindX(GI_Main::ikxCenter);
                        InfoShipImage2->SetImageKindY(GI_Main::ikyCenter);
                        {
                            Types::TPoint visualCenter_9 = InfoShipImage2->GetVisualCenter();
                            Types::TPoint itemImageCenter_9 = Globals::ShipScreen->ItemImageCenter;
                            InfoShipImage2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_9, visualCenter_9));
                        }
                    }
                } else {
                    GI_GraphBuf::TGraphBufGI* InfoShipImage2_2 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoShipImage2"sv));
                    InfoShipImage2_2->SetActive(true);
                    InfoShipImage2_2->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::checked_cast<SE_Ruins::TRuinsSE*>(Obj)->StaticImagePath), 1, u","sv), InfoShipImage2_2->GraphBuf);
                    if (InfoShipImage2_2->ClientSize.X < InfoShipImage2_2->GraphBuf->Width || InfoShipImage2_2->ClientSize.Y < InfoShipImage2_2->GraphBuf->Height) {
                        if (static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)) {
                            InfoShipImage2_2->GraphBuf->RescaleRgba(InfoShipImage2_2->ClientSize.X, System::Round(pas::real_divide(InfoShipImage2_2->ClientSize.X, static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)), 5);
                        } else {
                            InfoShipImage2_2->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoShipImage2_2->ClientSize.Y, static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoShipImage2_2->GraphBuf->Width)), InfoShipImage2_2->ClientSize.Y, 5);
                        }
                    }
                    {
                        Types::TPoint visualCenter_10 = InfoShipImage2_2->GetVisualCenter();
                        Types::TPoint itemImageCenter_10 = Globals::ShipScreen->ItemImageCenter;
                        InfoShipImage2_2->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_10, visualCenter_10));
                    }
                }
                if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj) != nullptr && Ship->OwnerId != aGalaxyStruct::oiDominator) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"sv))->SetActive(false);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv))->SetActive(false);
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"sv))->SetActive(true);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv))->SetActive(true);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv))->SetText(Ship->TypeName);
                }
                {
                    const pas::WideString& intToStr_3 = pas::wide_int_to_str(Ship->Speed);
                    GI_Label::TLabelGI* cpp_arg_59 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSpeed"sv));
                    cpp_arg_59->SetText(intToStr_3);
                }
                if (Ship->HullPoints <= pas::real_divide(Ship->HullCapacity, 2.0L)) {
                    ColorTag = aMyFunction::OrangeColorTag;
                } else {
                    ColorTag = pas::WideString();
                }
                if (Ship->ScannerResolved) {
                    Text = pas::concat_wide({aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Ship->HullPoints)), pas::view(ColorTag)), u"/", pas::wide_int_to_str(Ship->HullCapacity)});
                    if (Ship->RepairPoints >= 0) {
                        Text = pas::concat_wide({Text, u" + ", aMyFunction::WrapTextInColor(pas::view(pas::wide_int_to_str(Ship->RepairPoints)), u""sv)});
                    }
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"sv))->SetText(Text);
                } else {
                    const pas::WideString& wrapTextInColor_7 = aMyFunction::WrapTextInColor(u"???"sv, pas::view(ColorTag));
                    GI_Label::TLabelGI* cpp_arg_60 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"sv));
                    cpp_arg_60->SetText(wrapTextInColor_7);
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDef"sv))->SetText(Ship->DefenseText);
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"sv))->SetText(Ship->DamageText);
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipRel"sv))->SetText(aConst::RelationInfo[Ship->Relation].DisplayName);
                if (Ship->WinChance >= 0) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"sv))->SetActive(true);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv))->SetActive(true);
                    {
                        const pas::WideString& cpp_arg_61 = static_cast<pas::WideString>(pas::concat_ansi({SysUtils::IntToStr(Ship->WinChance), "%"}));
                        GI_Label::TLabelGI* cpp_arg_62 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv));
                        cpp_arg_62->SetText(cpp_arg_61);
                    }
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"sv))->SetActive(false);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv))->SetActive(false);
                }
                {
                    double real_max_3 = pas::real_max<double>(0.1, Ship->HullFragility);
                    BarWidth = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(Ship->HullCapacity, aConst::HullBaseSize), real_max_3)) * 64.0L);
                }
                BarWidth = std::min<std::int32_t>(192, std::max<std::int32_t>(32, BarWidth));
                {
                    GI_Image::TImageGI* InfoShipDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableLeft"sv));
                    CapWidth = InfoShipDurableLeft->GetContentSize().X;
                    MinimumWidth = CapWidth * 2 + BarWidth + InfoShipDurableLeft->LocalPosition.X + InfoShipDurableLeft->Parent->LocalPosition.X + InfoShipDurableLeft->Parent->Parent->LocalPosition.X * 2;
                }
                {
                    GI_Image::TImageGI* InfoShipDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurable"sv));
                    if (Ship->ScannerResolved) {
                        InfoShipDurable->SetPosition(([&] {
                            std::int32_t cpp_arg_63 = ([&] {
                                std::int64_t cpp_left_8 = System::Round(pas::real_divide(Ship->HullPoints, Ship->HullCapacity) * BarWidth);
                                return cpp_left_8 - (InfoShipDurable->GetContentSize().X - 5);
                            }());
                            std::int32_t y_4 = InfoShipDurable->LocalPosition.Y;
                            return ClassesImports::Point(cpp_arg_63, y_4);
                        }()));
                    } else {
                        MinimumWidth = MinimumWidth - BarWidth + 64;
                        BarWidth = 64;
                        {
                            std::int32_t cpp_arg_64 = BarWidth - (InfoShipDurable->GetContentSize().X - 5);
                            std::int32_t y_5 = InfoShipDurable->LocalPosition.Y;
                            InfoShipDurable->SetPosition(ClassesImports::Point(cpp_arg_64, y_5));
                        }
                    }
                    InfoShipDurable->Parent->Parent->SetActive(true);
                    InfoShipDurable->Parent->Parent->SetSize(ClassesImports::Point(CapWidth * 2 + BarWidth, InfoShipDurable->Parent->Parent->ClientSize.Y));
                    InfoShipDurable->Parent->SetSize(ClassesImports::Point(BarWidth + 2, InfoShipDurable->Parent->Parent->ClientSize.Y));
                }
                {
                    GI_Image::TImageGI* InfoShipDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableRight"sv));
                    {
                        std::int32_t cpp_arg_65 = BarWidth + CapWidth - InfoShipDurableRight->GetContentSize().X;
                        std::int32_t y_6 = InfoShipDurableRight->LocalPosition.Y;
                        InfoShipDurableRight->SetPosition(ClassesImports::Point(cpp_arg_65, y_6));
                    }
                    InfoShipDurableRight->Parent->SetPosition(ClassesImports::Point(CapWidth, InfoShipDurableRight->Parent->LocalPosition.Y));
                    InfoShipDurableRight->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoShipDurableRight->Parent->ClientSize.Y));
                }
                {
                    GI_Image::TImageGI* InfoShipDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoShipDurableBack"sv));
                    {
                        std::int32_t cpp_arg_66 = BarWidth + 1 - InfoShipDurableBack->GetContentSize().X;
                        std::int32_t y_7 = InfoShipDurableBack->LocalPosition.Y;
                        InfoShipDurableBack->SetPosition(ClassesImports::Point(cpp_arg_66, y_7));
                    }
                    InfoShipDurableBack->Parent->SetSize(ClassesImports::Point(BarWidth + CapWidth, InfoShipDurableBack->Parent->ClientSize.Y));
                }
                DamageName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISDamage"sv));
                DamageValue = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDamage"sv));
                if (Ship->DamageText.length() > 0) {
                    DamageName->SetActive(true);
                    DamageValue->SetActive(true);
                } else {
                    DamageName->SetActive(false);
                    DamageValue->SetActive(false);
                    DamageName = nullptr;
                    DamageValue = nullptr;
                }
                StatusCount = Ship->CombatStatusCount;
                if (StatusCount > 0) {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISEffects"sv))->SetActive(true);
                    {
                        GI_Label::TLabelGI* InfoShipEffects = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipEffects"sv));
                        InfoShipEffects->SetText(Ship->CombatStatusText);
                        {
                            std::int32_t cpp_arg_67 = StatusCount * InfoShipEffects->GetLineHeight() + 2;
                            std::int32_t x = InfoShipEffects->ClientSize.X;
                            InfoShipEffects->SetSize(ClassesImports::Point(x, cpp_arg_67));
                        }
                        InfoShipEffects->SetActive(true);
                    }
                } else {
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISEffects"sv))->SetActive(false);
                    pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipEffects"sv))->SetActive(false);
                }
                {
                    GI_Label::TLabelGI* cpp_arg_68 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISType"sv));
                    GI_Label::TLabelGI* cpp_arg_69 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipType"sv));
                    GI_Label::TLabelGI* cpp_arg_70 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISSpeed"sv));
                    GI_Label::TLabelGI* cpp_arg_71 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSpeed"sv));
                    GI_Label::TLabelGI* cpp_arg_72 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISSize"sv));
                    GI_Label::TLabelGI* cpp_arg_73 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipSize"sv));
                    GI_Label::TLabelGI* cpp_arg_74 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISDef"sv));
                    GI_Label::TLabelGI* cpp_arg_75 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipDef"sv));
                    GI_Label::TLabelGI* cpp_arg_76 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISRel"sv));
                    GI_Label::TLabelGI* cpp_arg_77 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipRel"sv));
                    GI_Label::TLabelGI* cpp_arg_78 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISWin"sv));
                    GI_Label::TLabelGI* cpp_arg_79 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipWin"sv));
                    GI_Label::TLabelGI* cpp_arg_80 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"ISEffects"sv));
                    GI_Label::TLabelGI* cpp_arg_81 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipEffects"sv));
                    GI_MessageLoop::TObjectGI* byName_2 = GetByName(u"InfoShipEmRace"sv);
                    GI_Label::TLabelGI* cpp_arg_82 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoShipName"sv));
                    GI_Window::TWindowGI* cpp_arg_83 = pas::checked_cast<GI_Window::TWindowGI*>(ShipInfoPanel);
                    Globals::ShipScreen->LayoutObjectInfo(cpp_arg_83, cpp_arg_82, cpp_arg_68, cpp_arg_69, cpp_arg_70, cpp_arg_71, cpp_arg_72, cpp_arg_73, cpp_arg_74, cpp_arg_75, DamageName, DamageValue, cpp_arg_76, cpp_arg_77, cpp_arg_78, cpp_arg_79, cpp_arg_80, cpp_arg_81, byName_2, true, MinimumWidth);
                }
            }
        } else if (pas::class_cast_if<SE_Star::TStarSE*>(Obj) != nullptr) {
            {
                std::int64_t cpp_left_10 = System::Round(Obj->Position.Y);
                std::int32_t cpp_arg_84 = cpp_left_10 - GetMapCenter().Y;
                std::int64_t cpp_left_9 = System::Round(Obj->Position.X);
                std::int32_t cpp_arg_85 = cpp_left_9 - GetMapCenter().X;
                HitObjectPosition = ClassesImports::Point(cpp_arg_85, cpp_arg_84);
            }
            HitObjectSize = Obj->Size;
            if (DisplayedFilmObject != Obj) {
                DisplayedFilmObject = Obj;
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(true);
                StandardInfoPanel->SetActive(false);
                {
                    const pas::WideString& wrapTextInColor_8 = aMyFunction::WrapTextInColor(pas::view(pas::checked_cast<aEObjInfo::TEObjInfo*>(Globals::SecondaryFilm->ObjectInfo)->StarName), pas::view(aMyFunction::InfoNameColorTag));
                    GI_Label::TLabelGI* cpp_arg_86 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStarName"sv));
                    cpp_arg_86->SetText(wrapTextInColor_8);
                }
                {
                    GI_GraphBuf::TGraphBufGI* InfoStarImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStarImage"sv));
                    InfoStarImage->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::checked_cast<SE_Star::TStarSE*>(Obj)->StaticImagePath), 1, u","sv), InfoStarImage->GraphBuf);
                    if (InfoStarImage->ClientSize.X < InfoStarImage->GraphBuf->Width || InfoStarImage->ClientSize.Y < InfoStarImage->GraphBuf->Height) {
                        if (static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) {
                            InfoStarImage->GraphBuf->RescaleRgba(InfoStarImage->ClientSize.X, System::Round(pas::real_divide(InfoStarImage->ClientSize.X, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)), 5);
                        } else {
                            InfoStarImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStarImage->ClientSize.Y, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)), InfoStarImage->ClientSize.Y, 5);
                        }
                    }
                    InfoStarImage->SetImageKindX(GI_Main::ikxCenter);
                    InfoStarImage->SetImageKindY(GI_Main::ikyCenter);
                }
                Panel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoStarPanel"sv));
                Panel->FreeOwnedChildren();
                Objects = pas::make_object<pas::List>();
                Records = pas::make_object<pas::List>();
                FilmObject = Globals::SecondaryFilm->FirstObject;
                while (FilmObject != nullptr) {
                    if (FilmObject->SceneObject != nullptr) {
                        if (pas::class_cast_if<SE_Planet::TPlanetSE*>(FilmObject->SceneObject) != nullptr) {
                            Planet = Snapshot->FindPlanet(FilmObject->ObjectId);
                            if (Planet != nullptr) {
                                pas::list_add(Objects, reinterpret_cast<void*>(FilmObject->SceneObject));
                                pas::list_add(Records, static_cast<void*>(Planet));
                            }
                        }
                    }
                    FilmObject = FilmObject->Next;
                }
                FilmObject = Globals::SecondaryFilm->FirstObject;
                while (FilmObject != nullptr) {
                    if (FilmObject->SceneObject != nullptr) {
                        if (SE_Ruins::TRuinsSE* ruinsSE = pas::class_cast_if<SE_Ruins::TRuinsSE*>(FilmObject->SceneObject); ruinsSE != nullptr && static_cast<std::uint8_t>(ruinsSE->HideOnStarInfo ^ 1) || pas::class_cast_if<SE_Ship2::TShip2SE*>(FilmObject->SceneObject) != nullptr && static_cast<SE_Ship2::TShip2SE*>(FilmObject->SceneObject)->AlternateImagePath != u"") {
                            if (FilmObject->SceneObject->GraphKey != u"Ruins.Blazer" && FilmObject->SceneObject->GraphKey != u"Ruins.Keller" && FilmObject->SceneObject->GraphKey != u"Ruins.Terron" && FilmObject->SceneObject->GraphKey != u"Ruins.FighterSwarm") {
                                Ship = Snapshot->FindShip(FilmObject->ObjectId);
                                if (Ship != nullptr) {
                                    if (!Ship->OutsideNormalSpace) {
                                        Distance = aMyFunction::PointDistanceSquared(FilmObject->SceneObject->Position, EC_Struct::MakePointF(0.0f, 0.0f));
                                        J = 0;
                                        while (J < pas::list_count(Objects)) {
                                            if (aMyFunction::PointDistanceSquared(pas::list_at<SE_Space::TObjectSE>(Objects, J)->Position, EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                                break;
                                            }
                                            ++J;
                                        }
                                        pas::list_insert(Objects, J, reinterpret_cast<void*>(FilmObject->SceneObject));
                                        pas::list_insert(Records, J, static_cast<void*>(Ship));
                                    }
                                }
                            }
                        }
                    }
                    FilmObject = FilmObject->Next;
                }
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Snapshot->CustomSystemInfos.length() - 1); cpp_range.next(I); ) {
                    CustomInfo = &Snapshot->CustomSystemInfos[I];
                    Distance = pas::sqr(CustomInfo->Distance);
                    J = 0;
                    while (J < pas::list_count(Objects)) {
                        if (pas::list_get(Objects, J) != nullptr) {
                            if (aMyFunction::PointDistanceSquared(pas::list_at<SE_Space::TObjectSE>(Objects, J)->Position, EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                break;
                            }
                        }
                        if (pas::list_get(Objects, J) == nullptr) {
                            if (pas::list_at<aEObjInfo::TEOTCustomStarInfo>(Records, J)->Distance > CustomInfo->Distance) {
                                break;
                            }
                        }
                        ++J;
                    }
                    pas::list_insert(Objects, J, nullptr);
                    pas::list_insert(Records, J, static_cast<void*>(CustomInfo));
                }
                RowHeight = GR_Main::GiScalePixels(20);
                NameWidth = GR_Main::GiScalePixels(100);
                DetailWidth = GR_Main::GiScalePixels(100);
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range_2.next(I); ) {
                    GI_Label::TLabelGI* cpp_with_26 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                    cpp_with_26->SetFontName(GlobalsV::NormalFontName);
                    cpp_with_26->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
                    cpp_with_26->SetSize(ClassesImports::Point(1, RowHeight));
                    cpp_with_26->SetPosition(ClassesImports::Point(0, RowHeight * I));
                    cpp_with_26->SetWordWrapEnabled(false);
                    cpp_with_26->SetTextAlignX(GI_Main::taxAuto);
                    cpp_with_26->SetTextAlignY(GI_Main::tayCenterEx);
                    if (pas::list_get(Objects, I) == nullptr) {
                        cpp_with_26->SetText(pas::list_at<aEObjInfo::TEOTCustomStarInfo>(Records, I)->Name);
                    } else if (pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        cpp_with_26->SetText(pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Name);
                    } else {
                        cpp_with_26->SetText(pas::list_at<aEObjInfo::TEOTShip>(Records, I)->Name);
                    }
                    NameWidth = std::max<std::int32_t>(NameWidth, cpp_with_26->ClientSize.X);
                }
                Child = Panel->FirstChild;
                while (Child != nullptr) {
                    if (GI_Label::TLabelGI* labelGI = pas::class_cast_if<GI_Label::TLabelGI*>(Child)) {
                        GI_Label::TLabelGI* cpp_with_27 = labelGI;
                        cpp_with_27->SetTextAlignX(GI_Main::taxRight);
                        cpp_with_27->SetSize(ClassesImports::Point(NameWidth, RowHeight));
                    }
                    Child = Child->NextSibling;
                }
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range_3.next(I); ) {
                    {
                        GI_GraphBuf::TGraphBufGI* cpp_with_28 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Panel, false);
                        IconInset = 0u;
                        if (pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                            if (pas::checked_cast<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I))->Radius < 70) {
                                IconInset = 4u;
                            } else if (pas::checked_cast<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I))->Radius < 80) {
                                IconInset = 3u;
                            } else if (pas::checked_cast<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I))->Radius < 90) {
                                IconInset = 2u;
                            } else if (pas::checked_cast<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I))->Radius < 100) {
                                IconInset = 1u;
                            } else {
                                IconInset = 0u;
                            }
                        }
                        cpp_with_28->SourceHasPerPixelAlpha = true;
                        cpp_with_28->SetPosition(ClassesImports::Point(NameWidth + 5 + 1 + (IconInset >> 1), RowHeight * I + 1 + (IconInset >> 1)));
                        cpp_with_28->SetSize(ClassesImports::Point(RowHeight - 2 - IconInset, RowHeight - 2 - IconInset));
                        if (pas::list_get(Objects, I) == nullptr) {
                            if (pas::list_at<aEObjInfo::TEOTCustomStarInfo>(Records, I)->ImagePath != u"") {
                                GI_GI::LoadGiByPathIntoGraphBuf(pas::list_at<aEObjInfo::TEOTCustomStarInfo>(Records, I)->ImagePath, cpp_with_28->GraphBuf);
                                if (static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) {
                                    cpp_with_28->GraphBuf->RescaleRgba(cpp_with_28->ClientSize.X, System::Round(pas::real_divide(cpp_with_28->ClientSize.X, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_28->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_28->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)), cpp_with_28->ClientSize.Y, 5);
                                }
                            }
                        } else if (pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                            pas::list_at<SE_Planet::TPlanetSE>(Objects, I)->RenderToBuffer(this, cpp_with_28->GraphBuf, true);
                            if (cpp_with_28->ClientSize.X < cpp_with_28->GraphBuf->Width || cpp_with_28->ClientSize.Y < cpp_with_28->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) {
                                    cpp_with_28->GraphBuf->RescaleRgba(cpp_with_28->ClientSize.X, System::Round(pas::real_divide(cpp_with_28->ClientSize.X, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_28->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_28->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)), cpp_with_28->ClientSize.Y, 5);
                                }
                            }
                        } else if (pas::class_cast_if<SE_Ship2::TShip2SE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::list_at<SE_Ship2::TShip2SE>(Objects, I)->AlternateImagePath), 1, u","sv), cpp_with_28->GraphBuf);
                            if (cpp_with_28->ClientSize.X < cpp_with_28->GraphBuf->Width || cpp_with_28->ClientSize.Y < cpp_with_28->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) {
                                    cpp_with_28->GraphBuf->RescaleRgba(cpp_with_28->ClientSize.X, System::Round(pas::real_divide(cpp_with_28->ClientSize.X, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_28->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_28->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)), cpp_with_28->ClientSize.Y, 5);
                                }
                            }
                        } else {
                            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(pas::list_at<SE_Ruins::TRuinsSE>(Objects, I)->StaticImagePath), 1, u","sv), cpp_with_28->GraphBuf);
                            if (cpp_with_28->ClientSize.X < cpp_with_28->GraphBuf->Width || cpp_with_28->ClientSize.Y < cpp_with_28->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) {
                                    cpp_with_28->GraphBuf->RescaleRgba(cpp_with_28->ClientSize.X, System::Round(pas::real_divide(cpp_with_28->ClientSize.X, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_28->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_28->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_28->GraphBuf->Width)), cpp_with_28->ClientSize.Y, 5);
                                }
                            }
                        }
                        cpp_with_28->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_28->SetImageKindY(GI_Main::ikyCenter);
                    }
                    if (pas::list_get(Objects, I) == nullptr) {
                        OwnerId = aGalaxyStruct::oiUninhabited;
                    } else if (pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        OwnerId = pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->OwnerId;
                    } else {
                        OwnerId = pas::list_at<aEObjInfo::TEOTShip>(Records, I)->OwnerId;
                    }
                    if (pas::list_get(Objects, I) == nullptr) {
                        CustomInfo = pas::list_at<aEObjInfo::TEOTCustomStarInfo>(Records, I);
                        if (EC_Str::CountDelimitedPartsW(pas::view(CustomInfo->Text), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Text), 0, u":"sv) == u"Image") {
                            Images = EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Text), 1, u":"sv);
                            RowX = NameWidth + 5 + RowHeight + 5 + 1;
                            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Images), u","sv) - 1); cpp_range_4.next(J); ) {
                                GI_Image::TImageGI* cpp_with_29 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                                cpp_with_29->SetImagePath(pas::concat_wide({u"GI,", EC_Str::ExtractDelimitedPartW(pas::view(Images), J, u","sv)}));
                                cpp_with_29->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                                cpp_with_29->SetPosition(ClassesImports::Point(RowX, RowHeight * I + 1));
                                RowX = RowX + RowHeight + 2;
                            }
                        } else if (EC_Str::CountDelimitedPartsW(pas::view(CustomInfo->Text), u":"sv) > 1 && EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Text), 0, u":"sv) == u"RGBA") {
                            Images = EC_Str::ExtractDelimitedPartW(pas::view(CustomInfo->Text), 1, u":"sv);
                            RowX = NameWidth + 5 + RowHeight + 5 + 1;
                            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(pas::view(Images), u","sv) - 1); cpp_range_5.next(J); ) {
                                GI_GraphBuf::TGraphBufGI* cpp_with_30 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Panel, false);
                                cpp_with_30->SourceHasPerPixelAlpha = true;
                                cpp_with_30->LoadBitmapPathAsRgba(pas::concat_wide({EC_Str::ExtractDelimitedPartW(pas::view(Images), J, u","sv), EC_CacheBitmap::RgbaImagePathSuffix}));
                                cpp_with_30->SetPosition(ClassesImports::Point(RowX, RowHeight * I + 1));
                                cpp_with_30->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                                if (cpp_with_30->ClientSize.X < cpp_with_30->GraphBuf->Width || cpp_with_30->ClientSize.Y < cpp_with_30->GraphBuf->Height) {
                                    if (static_cast<std::uint32_t>(cpp_with_30->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_30->GraphBuf->Height)) {
                                        cpp_with_30->GraphBuf->RescaleRgba(cpp_with_30->ClientSize.X, System::Round(pas::real_divide(cpp_with_30->ClientSize.X, static_cast<std::uint32_t>(cpp_with_30->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_30->GraphBuf->Height)), 5);
                                    } else {
                                        cpp_with_30->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_30->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_30->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_30->GraphBuf->Width)), cpp_with_30->ClientSize.Y, 5);
                                    }
                                }
                                cpp_with_30->SetImageKindX(GI_Main::ikxCenter);
                                cpp_with_30->SetImageKindY(GI_Main::ikyCenter);
                                RowX = RowX + RowHeight + 2;
                            }
                        } else {
                            GI_Label::TLabelGI* cpp_with_31 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                            if (GR_Main::GiResourceVariant() == 2) {
                                cpp_with_31->SetFontName(GlobalsV::MiniFontName);
                            } else {
                                cpp_with_31->SetFontName(GlobalsV::SmallFontName);
                            }
                            cpp_with_31->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                            cpp_with_31->SetSize(ClassesImports::Point(1, RowHeight));
                            cpp_with_31->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                            cpp_with_31->SetWordWrapEnabled(false);
                            cpp_with_31->SetTextAlignX(GI_Main::taxAuto);
                            cpp_with_31->SetTextAlignY(GI_Main::tayCenterEx);
                            cpp_with_31->SetText(CustomInfo->Text);
                            DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_31->ClientSize.X + GR_Main::GiScalePixels(35));
                        }
                    } else if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr || pas::class_cast_if<SE_Ship2::TShip2SE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                        GI_Label::TLabelGI* cpp_with_32 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                        if (GR_Main::GiResourceVariant() == 2) {
                            cpp_with_32->SetFontName(GlobalsV::MiniFontName);
                        } else {
                            cpp_with_32->SetFontName(GlobalsV::SmallFontName);
                        }
                        cpp_with_32->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                        cpp_with_32->SetSize(ClassesImports::Point(1, RowHeight));
                        cpp_with_32->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                        cpp_with_32->SetWordWrapEnabled(false);
                        cpp_with_32->SetTextAlignX(GI_Main::taxAuto);
                        cpp_with_32->SetTextAlignY(GI_Main::tayCenterEx);
                        cpp_with_32->SetText(EC_Str::LowerCaseWideString(pas::list_at<aEObjInfo::TEOTShip>(Records, I)->TypeName));
                        DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_32->ClientSize.X + GR_Main::GiScalePixels(35));
                    } else if (OwnerId != aGalaxyStruct::oiUninhabited) {
                        if (pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr && (aPlanet::MainPiratePlanet == nullptr || pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Id != aPlanet::MainPiratePlanet->Id)) {
                            GI_GraphBuf::TGraphBufGI* cpp_with_33 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Panel, false);
                            cpp_with_33->SourceHasPerPixelAlpha = true;
                            cpp_with_33->LoadBitmapPathAsRgba(pas::concat_wide_reverse({EC_CacheBitmap::RgbaImagePathSuffix, EC_Str::ExtractDelimitedPartW(pas::view(aConst::GetFactionEmblemPath(pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Faction)), 1, u","sv)}));
                            cpp_with_33->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I + 1));
                            cpp_with_33->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                            if (cpp_with_33->ClientSize.X < cpp_with_33->GraphBuf->Width || cpp_with_33->ClientSize.Y < cpp_with_33->GraphBuf->Height) {
                                if (static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Height)) {
                                    cpp_with_33->GraphBuf->RescaleRgba(cpp_with_33->ClientSize.X, System::Round(pas::real_divide(cpp_with_33->ClientSize.X, static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Height)), 5);
                                } else {
                                    cpp_with_33->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_33->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_33->GraphBuf->Width)), cpp_with_33->ClientSize.Y, 5);
                                }
                            }
                            cpp_with_33->SetImageKindX(GI_Main::ikxCenter);
                            cpp_with_33->SetImageKindY(GI_Main::ikyCenter);
                        }
                    }
                    IsCivilized = pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr && pas::in_set<aGalaxyStruct::oiMaloc, aGalaxyStruct::oiGaal, aGalaxyStruct::oiPirate, aGalaxyStruct::oiPirate>(pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->OwnerId) && (aPlanet::MainPiratePlanet == nullptr || pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Id != aPlanet::MainPiratePlanet->Id);
                    if (IsCivilized) {
                        ListedPlanet = pas::list_at<aEObjInfo::TEOTPlanet>(Records, I);
                        if (ListedPlanet->OwnerId == aGalaxyStruct::oiPirate) {
                            IsCivilized = ListedPlanet->Faction == pas::concat_wide({aConst::OwnerInfo[aGalaxyStruct::oiPirate].InternalName, aConst::RaceToSys(ListedPlanet->RaceId)});
                        } else {
                            IsCivilized = ListedPlanet->Faction == aConst::OwnerInfo[ListedPlanet->OwnerId].InternalName;
                        }
                    }
                    if (IsCivilized) {
                        RowX = NameWidth + 5 + RowHeight + 5 + 1;
                        {
                            GI_Image::TImageGI* cpp_with_34 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                            switch (pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Relation) {
                                case aGalaxyStruct::rlHostile: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face4"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlBad: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face3"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlNormal: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face2"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlGood: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face1"_w);
                                    break;
                                }
                                case aGalaxyStruct::rlExcellent: {
                                    cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face0"_w);
                                    break;
                                }
                                default: cpp_with_34->SetImagePath(u"GI,Bm.FormGalaxy2.Face2"_w); break;
                            }
                            cpp_with_34->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                            cpp_with_34->SetPosition(ClassesImports::Point(RowX + RowHeight + 2, RowHeight * I + 1));
                        }
                        RowX = RowX + RowHeight + 2;
                        if (pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peIndustrial>(pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Economy)) {
                            GI_Image::TImageGI* cpp_with_35 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
                            switch (pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Economy) {
                                case aGalaxyStruct::peAgricultural: {
                                    cpp_with_35->SetImagePath(u"GI,Bm.FormGalaxy.EconAgrar"_w);
                                    break;
                                }
                                case aGalaxyStruct::peIndustrial: {
                                    cpp_with_35->SetImagePath(u"GI,Bm.FormGalaxy.EconIndustr"_w);
                                    break;
                                }
                            }
                            cpp_with_35->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                            cpp_with_35->SetPosition(ClassesImports::Point(RowX + RowHeight, RowHeight * I + 1));
                        }
                    } else if (pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr && aPlanet::MainPiratePlanet != nullptr && pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->Id == aPlanet::MainPiratePlanet->Id) {
                        GI_Label::TLabelGI* cpp_with_36 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                        if (GR_Main::GiResourceVariant() == 2) {
                            cpp_with_36->SetFontName(GlobalsV::MiniFontName);
                        } else {
                            cpp_with_36->SetFontName(GlobalsV::SmallFontName);
                        }
                        cpp_with_36->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                        cpp_with_36->SetSize(ClassesImports::Point(1, RowHeight));
                        cpp_with_36->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                        cpp_with_36->SetWordWrapEnabled(false);
                        cpp_with_36->SetTextAlignX(GI_Main::taxAuto);
                        cpp_with_36->SetTextAlignY(GI_Main::tayCenterEx);
                        cpp_with_36->SetText(EC_Str::LowerCaseWideString(aConst::LocalizedText(u"ShipType.TypeName.PB"_wref.get())));
                        DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_36->ClientSize.X + GR_Main::GiScalePixels(35));
                    } else if (pas::class_cast_if<SE_Planet::TPlanetSE*>(pas::list_at<pas::Object>(Objects, I)) != nullptr && pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->OwnerId == aGalaxyStruct::oiUninhabited && pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->UnexploredWater == 0 && pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->UnexploredLand == 0 && pas::list_at<aEObjInfo::TEOTPlanet>(Records, I)->UnexploredHills == 0) {
                        GI_Label::TLabelGI* cpp_with_37 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
                        if (GR_Main::GiResourceVariant() == 2) {
                            cpp_with_37->SetFontName(GlobalsV::MiniFontName);
                        } else {
                            cpp_with_37->SetFontName(GlobalsV::SmallFontName);
                        }
                        cpp_with_37->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(140, 140, 140));
                        cpp_with_37->SetSize(ClassesImports::Point(1, RowHeight));
                        cpp_with_37->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                        cpp_with_37->SetWordWrapEnabled(false);
                        cpp_with_37->SetTextAlignX(GI_Main::taxAuto);
                        cpp_with_37->SetTextAlignY(GI_Main::tayCenterEx);
                        cpp_with_37->SetText(EC_Str::LowerCaseWideString(aConst::LocalizedText(u"Planet.NotCivil.AllExplore"_wref.get())));
                        DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_37->ClientSize.X + GR_Main::GiScalePixels(35));
                    }
                }
                Panel->SetSize(ClassesImports::Point(NameWidth + DetailWidth, RowHeight * pas::list_count(Objects)));
                StarInfoWindow->SetSize(ClassesImports::Point(Panel->ClientSize.X + StarInfoWindow->WorkSubRect.Left + StarInfoWindow->WorkSubRect.Right, StarInfoWindow->WorkSubRect.Top + StarInfoWindow->WorkSubRect.Bottom + RowHeight * pas::list_count(Objects)));
                StarInfoWindow->UpdateAutoGeometry();
                pas::free(Objects);
                pas::free(Records);
            }
        } else if (pas::class_cast_if<SE_Asteroid::TAsteroidSE*>(Obj) != nullptr) {
            {
                std::int64_t cpp_left_12 = System::Round(Obj->Position.Y);
                std::int32_t cpp_arg_87 = cpp_left_12 - GetMapCenter().Y;
                std::int64_t cpp_left_11 = System::Round(Obj->Position.X);
                std::int32_t cpp_arg_88 = cpp_left_11 - GetMapCenter().X;
                HitObjectPosition = ClassesImports::Point(cpp_arg_88, cpp_arg_87);
            }
            HitObjectSize = Obj->Size;
            if (DisplayedFilmObject != Obj) {
                DisplayedFilmObject = Obj;
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(true);
                GetByName(u"InfoStdImage"sv)->SetActive(false);
                {
                    GI_GraphBuf::TGraphBufGI* InfoStdGB_7 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                    InfoStdGB_7->SetActive(true);
                    InfoStdGB_7->SourceHasPerPixelAlpha = true;
                    GI_GAI::LoadGaiFrameToGraphBuf(pas::checked_cast<SE_Asteroid::TAsteroidSE*>(Obj)->ImagePath, InfoStdGB_7->GraphBuf, ObjectId);
                    if (InfoStdGB_7->ClientSize.X < InfoStdGB_7->GraphBuf->Width || InfoStdGB_7->ClientSize.Y < InfoStdGB_7->GraphBuf->Height) {
                        if (static_cast<std::uint32_t>(InfoStdGB_7->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStdGB_7->GraphBuf->Height)) {
                            InfoStdGB_7->GraphBuf->RescaleRgba(InfoStdGB_7->ClientSize.X, System::Round(pas::real_divide(InfoStdGB_7->ClientSize.X, static_cast<std::uint32_t>(InfoStdGB_7->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStdGB_7->GraphBuf->Height)), 5);
                        } else {
                            InfoStdGB_7->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStdGB_7->ClientSize.Y, static_cast<std::uint32_t>(InfoStdGB_7->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStdGB_7->GraphBuf->Width)), InfoStdGB_7->ClientSize.Y, 5);
                        }
                    }
                    {
                        Types::TPoint visualCenter_11 = InfoStdGB_7->GetVisualCenter();
                        Types::TPoint itemImageCenter_11 = Globals::ShipScreen->ItemImageCenter;
                        InfoStdGB_7->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_11, visualCenter_11));
                    }
                }
                {
                    const pas::WideString& wrapTextInColor_9 = aMyFunction::WrapTextInColor(pas::view(Asteroid->Name), pas::view(aMyFunction::InfoNameColorTag));
                    GI_Label::TLabelGI* cpp_arg_89 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    cpp_arg_89->SetText(wrapTextInColor_9);
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv))->SetText(Asteroid->InfoText);
                {
                    GI_Label::TLabelGI* cpp_arg_90 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                    GI_Label::TLabelGI* cpp_arg_91 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    GI_Window::TWindowGI* cpp_arg_92 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                    fShip2::TfShip2::LayoutItemInfo(cpp_arg_92, cpp_arg_91, cpp_arg_90, true, true, 0);
                }
            }
        } else if (pas::class_cast_if<SE_Missile::TMissileSE*>(Obj) != nullptr) {
            {
                std::int64_t cpp_left_14 = System::Round(Obj->Position.Y);
                std::int32_t cpp_arg_93 = cpp_left_14 - GetMapCenter().Y;
                std::int64_t cpp_left_13 = System::Round(Obj->Position.X);
                std::int32_t cpp_arg_94 = cpp_left_13 - GetMapCenter().X;
                HitObjectPosition = ClassesImports::Point(cpp_arg_94, cpp_arg_93);
            }
            HitObjectSize = Obj->Size;
            if (DisplayedFilmObject != Obj) {
                DisplayedFilmObject = Obj;
                InfoWindow->SetActive(false);
                ItemInfoWindow->SetActive(false);
                ShipInfoPanel->SetActive(false);
                PlanetInfoPanel->SetActive(false);
                StarInfoWindow->SetActive(false);
                StandardInfoPanel->SetActive(true);
                GetByName(u"InfoStdImage"sv)->SetActive(false);
                {
                    GI_GraphBuf::TGraphBufGI* InfoStdGB_8 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStdGB"sv));
                    InfoStdGB_8->SetActive(true);
                    InfoStdGB_8->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::concat_wide({u"Bm.", Obj->GraphKey, u"_", GR_Main::GiResourceSuffix(), u"i"}), InfoStdGB_8->GraphBuf);
                    {
                        Types::TPoint visualCenter_12 = InfoStdGB_8->GetVisualCenter();
                        Types::TPoint itemImageCenter_12 = Globals::ShipScreen->ItemImageCenter;
                        InfoStdGB_8->SetPosition(EC_Struct::SubtractPoints(itemImageCenter_12, visualCenter_12));
                    }
                }
                {
                    const pas::WideString& wrapTextInColor_10 = aMyFunction::WrapTextInColor(pas::view(Missile->Name), pas::view(aMyFunction::InfoNameColorTag));
                    GI_Label::TLabelGI* cpp_arg_95 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    cpp_arg_95->SetText(wrapTextInColor_10);
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv))->SetText(Missile->InfoText);
                {
                    GI_Label::TLabelGI* cpp_arg_96 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdText"sv));
                    GI_Label::TLabelGI* cpp_arg_97 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStdName"sv));
                    GI_Window::TWindowGI* cpp_arg_98 = pas::checked_cast<GI_Window::TWindowGI*>(StandardInfoPanel);
                    fShip2::TfShip2::LayoutItemInfo(cpp_arg_98, cpp_arg_97, cpp_arg_96, true, true, 0);
                }
            }
        }
        if (pas::class_cast_if<SE_Container::TContainerSE*>(Obj) != nullptr) {
            HitObjectSize = ClassesImports::Point(38, 38);
        }
        if (pas::class_cast_if<SE_Star::TStarSE*>(Obj) != nullptr) {
            HitObjectSize = ClassesImports::Point(300, 300);
        }
        if (InfoWindow->Active) {
            ActivePanel = InfoWindow;
        } else if (ItemInfoWindow->Active) {
            ActivePanel = ItemInfoWindow;
        } else if (ShipInfoPanel->Active) {
            ActivePanel = ShipInfoPanel;
        } else if (PlanetInfoPanel->Active) {
            ActivePanel = PlanetInfoPanel;
        } else if (StarInfoWindow->Active) {
            ActivePanel = StarInfoWindow;
        } else if (StandardInfoPanel->Active) {
            ActivePanel = StandardInfoPanel;
        } else {
            return;
        }
        if (GlobalsV::DynamicTipsPos) {
            HitObjectPosition.X += static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2;
            HitObjectPosition.Y += static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2;
            I = static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 3;
            if (HitObjectPosition.X <= I) {
                HitObjectPosition.X += HitObjectSize.X / 2;
            } else if (HitObjectPosition.X >= 2 * I) {
                HitObjectPosition.X = HitObjectPosition.X - HitObjectSize.X / 2 - ActivePanel->ClientSize.X;
            } else {
                HitObjectPosition.X -= ActivePanel->ClientSize.X / 2;
                I = 0;
            }
            if (I == 0) {
                if (HitObjectPosition.Y < static_cast<std::int32_t>(static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2)) {
                    HitObjectPosition.Y += HitObjectSize.Y / 2;
                    if (ActivePanel->ClientSize.Y + HitObjectPosition.Y + 10 > GR_Main::GameScreenHeight) {
                        HitObjectPosition.Y = HitObjectPosition.Y - HitObjectSize.Y - ActivePanel->ClientSize.Y;
                    }
                    if (HitObjectPosition.Y < 10) {
                        HitObjectPosition.Y = 10;
                    }
                } else {
                    HitObjectPosition.Y = HitObjectPosition.Y - HitObjectSize.Y / 2 - ActivePanel->ClientSize.Y;
                }
            } else {
                I = static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 3;
                if (HitObjectPosition.Y <= I) {
                    HitObjectPosition.Y += HitObjectSize.Y / 2;
                } else if (HitObjectPosition.Y >= 2 * I) {
                    HitObjectPosition.Y = HitObjectPosition.Y - HitObjectSize.Y / 2 - ActivePanel->ClientSize.Y;
                } else {
                    HitObjectPosition.Y -= ActivePanel->ClientSize.Y / 2;
                }
            }
            HitObjectPosition.X -= static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2;
            HitObjectPosition.Y -= static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2;
            ActivePanel->SetPosition(HitObjectPosition);
        } else {
            ActivePanel->SetPosition(ClassesImports::Point(10 - static_cast<std::uint32_t>(GR_Main::GameScreenWidth) / 2, 10 - static_cast<std::uint32_t>(GR_Main::GameScreenHeight) / 2));
        }
    }

    void TfStarMap::PrepareTalkDisplay() {
        FilmCameraMoving = true;
        MainPanel->Show();
        ShowObjectInfo(nullptr);
        if (Globals::TalkShip != nullptr) {
            CenterMapForTalk(Globals::TalkShip->Position);
        } else if (Globals::TalkPlanet != nullptr && Globals::TalkPlanet->CurrentStar == aPlayer::GetPlayer()->CurrentStar) {
            CenterMapForTalk(aPlayer::GetPlayer()->Position);
        }
        Globals::SpaceProcess->Space->DrawMinimap();
        SetCursorActive(false);
        DrawFrame();
        GR_Main::CaptureScreenBackground(true, 0);
        SetCursorActive(true);
    }

    void TfStarMap::WaitForTurnOrTalk() {
        pas::Array<WindowsImports::THandle, 0, 1> Events{};
        MainPanel->Hide();
        ResumeMode = smrNormal;
        // Native dormant checks precede the actual wait setup.
        if (Globals::TurnCalculationThread->IdleEvent == 0) {
            static_cast<void>(Globals::TurnCalculationThread->IdleEvent == 0);
        }
        WindowsSdk::SetEvent(Globals::TalkCompletedEvent);
        Events[0] = Globals::TurnCalculationThread->IdleEvent;
        Events[1] = Globals::TalkRequestEvent;
        void* EventList = &Events;
        std::uint32_t WaitResult = WindowsSdk::WaitForMultipleObjects(2u, static_cast<WindowsSdk::PWOHandleArray>(EventList), 0, WindowsSdk::INFINITE);
        if (Globals::TurnCalculationThread->IdleEvent == 0 || WaitResult == WindowsSdk::WAIT_OBJECT_0) {
            if (Globals::PlayerStarDayPrepared) {
                AnimateSpacePanelOnResume = true;
                StartOrderMode();
            } else {
                RebuildPartnerButtons();
                if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(aPlayer::GetPlayer()->InHyperspace ^ 1)) {
                    UpdateTerronTransformation();
                    aCalc::QueueGalaxyTurnCalculation();
                }
                StartTurnFilm();
            }
        } else if (WaitResult == WindowsSdk::WAIT_FAILED) {
            pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error GetLastError()=", SysUtils::Int64ToStr(WindowsImports::GetLastError())})));
        } else if (WaitResult == WindowsSdk::WAIT_OBJECT_0 + 1) {
            RunTalkDialogs();
            WaitForTurnOrTalk();
        }
    }

    void TfStarMap::UpdateTerronTransformation() {
        SE_Space::TObjectSE* Obj{};
        if ((aGalaxy::Galaxy->TerronToStarTurn & 0x20000000) == 0 && (aGalaxy::Galaxy->TerronToStarTurn & aGalaxyStruct::TerronTransformationFlag) != 0 && (aGalaxy::Galaxy->TerronToStarTurn & 0x0fffffff) <= aGalaxy::Galaxy->CurrentTurn && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CurrentStar == aKling::TerronShip->CurrentStar) {
            if (!GlobalsV::AnimStar) {
                Obj = Globals::SpaceProcess->Space->FirstObject;
                while (Obj != nullptr) {
                    if (aKling::TerronShip->Graphic == Obj) {
                        Obj->DetachFromSpace();
                        aKling::TerronShip->Order = aShip::soJump;
                        aKling::TerronShip->OrderTarget = aKling::TerronShip->CurrentStar;
                        aKling::TerronShip->InHyperspace = true;
                        aKling::TerronShip->OrderStateData = 2;
                        aKling::TerronShip->Position = EC_Struct::MakePointF(0.0f, 0.0f);
                        break;
                    }
                    Obj = Obj->Next;
                }
                aKling::TerronShip->CurrentStar->Graphic->DetachFromSpace();
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&aKling::TerronShip->CurrentStar->Graphic));
                {
                    SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Star"sv, u"Star.TerronAfter"_wref.get(), ClassesImports::Point(0, 0));
                    pas::Var<SE_Space::TObjectSE*> graphic = pas::Var<SE_Space::TObjectSE*>(&aKling::TerronShip->CurrentStar->Graphic);
                    SE_Space::RetainSpaceObject(graphic, createSpaceObjectByName);
                }
                aKling::TerronShip->CurrentStar->Graphic->AttachToSpace(Globals::SpaceProcess->Space);
            } else {
                Obj = Globals::SpaceProcess->Space->FirstObject;
                while (Obj != nullptr) {
                    if (SE_Star::TStarSE* starSE = pas::class_cast_if<SE_Star::TStarSE*>(Obj)) {
                        if (!pas::assigned(starSE->Animation->CycleCompleteCallback)) {
                            aGalaxy::Galaxy->TerronToStarTurn = aGalaxyStruct::TerronTransformationFlag;
                            starSE->Animation->CycleCompleteCallback = pas::bind_method<&TfStarMap::TerronTransformationStarted>(this);
                            Obj = Globals::SpaceProcess->Space->FirstObject;
                            while (Obj != nullptr) {
                                if (aKling::TerronShip->Graphic == Obj) {
                                    // CreateNormalGraphic uses Ruins.Terron, whose TRuinsSE owns these images.
                                    if (reinterpret_cast<SE_Ruins::TRuinsSE*>(Obj)->Animation != nullptr) {
                                        TerronFadeImage = reinterpret_cast<SE_Ruins::TRuinsSE*>(Obj)->Animation;
                                        reinterpret_cast<SE_Ruins::TRuinsSE*>(Obj)->Animation = nullptr;
                                    } else {
                                        TerronFadeImage = reinterpret_cast<SE_Ruins::TRuinsSE*>(Obj)->StaticImage;
                                        reinterpret_cast<SE_Ruins::TRuinsSE*>(Obj)->StaticImage = nullptr;
                                    }
                                    Obj->DetachFromSpace();
                                    aKling::TerronShip->Order = aShip::soJump;
                                    aKling::TerronShip->OrderTarget = aKling::TerronShip->CurrentStar;
                                    aKling::TerronShip->InHyperspace = true;
                                    aKling::TerronShip->OrderStateData = 2;
                                    aKling::TerronShip->Position = EC_Struct::MakePointF(0.0f, 0.0f);
                                    break;
                                }
                                Obj = Obj->Next;
                            }
                        }
                        break;
                    }
                    Obj = Obj->Next;
                }
            }
        }
    }

    void TfStarMap::TerronTransformationStarted(GI_MessageLoop::TObjectGI* Sender) {
        GI_GAI::TgaiGI* Image = pas::checked_cast<GI_GAI::TgaiGI*>(Sender);
        Image->SetImagePath(u"Bm.Star.Terron_Transform_a"_wref.get());
        Image->SequenceIndex = 0;
        Image->UpdateAutoGeometry();
        Image->RestartPlayback();
        Image->CycleCompleteCallback = pas::bind_static_method<&TfStarMap::TerronTransformationFinished>(this);
        Image->FrameAdvancedCallback = pas::bind_method<&TfStarMap::TerronTransformationFrame>(this);
    }

    void TfStarMap::TerronTransformationFrame(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t Alpha{};
        float Progress{};
        if (aKling::TerronShip != nullptr && pas::checked_cast<GI_GAI::TgaiGI*>(Sender)->SequenceFrame != 0 && TerronFadeImage != nullptr) {
            {
                pas::Extended cpp_left = static_cast<GI_GAI::TgaiGI*>(Sender)->SequenceFrame;
                Progress = pas::real_divide(cpp_left, static_cast<GI_GAI::TgaiGI*>(Sender)->SequenceFrameCount - 1);
            }
            Progress = Progress * 2.0L;
            if (Progress > 1.0L) {
                Progress = 1.0f;
            }
            Alpha = System::Round((1.0L - Progress) * 255.0L);
            if (GI_GAI::TgaiGI* gaiGI = pas::class_cast_if<GI_GAI::TgaiGI*>(TerronFadeImage)) {
                if (gaiGI->Alpha > Alpha) {
                    gaiGI->SetAlpha(Alpha);
                    if (pas::checked_cast<GI_GAI::TgaiGI*>(TerronFadeImage)->Alpha <= 0) {
                        pas::free(TerronFadeImage);
                        TerronFadeImage = nullptr;
                    }
                }
            } else if (pas::checked_cast<GI_Image::TImageGI*>(TerronFadeImage)->GetAlpha() > Alpha) {
                static_cast<GI_Image::TImageGI*>(TerronFadeImage)->SetAlpha(Alpha);
                if (pas::checked_cast<GI_Image::TImageGI*>(TerronFadeImage)->GetAlpha() <= 0) {
                    pas::free(TerronFadeImage);
                    TerronFadeImage = nullptr;
                }
            }
        }
    }

    void TfStarMap::TerronTransformationFinished(GI_MessageLoop::TObjectGI* Sender) {
        GI_GAI::TgaiGI* Animation = pas::checked_cast<GI_GAI::TgaiGI*>(Sender);
        Animation->SetImagePath(u"Bm.Star.TerronAfter_a"_wref.get());
        Animation->SequenceIndex = 0;
        Animation->UpdateAutoGeometry();
        Animation->RestartPlayback();
        Animation->FrameAdvancedCallback = nullptr;
        Animation->CycleCompleteCallback = nullptr;
        aGalaxy::Galaxy->TerronToStarTurn |= 0x20000000;
    }

    void TfStarMap::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (MainPanel->NavigationLocked) {
            return;
        }
        if (Globals::ShipScreen->ReopenRequested) {
            return;
        }
        if (GR_Main::ExitScreenLoop) {
            return;
        }
        if (WindowsSdk::WaitForSingleObject(Globals::ScriptUiRequestEvent, 0u) == WindowsSdk::WAIT_OBJECT_0) {
            return;
        }
        if (!pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            ThreadCalc::WaitForTurnCalculation();
        }
        aGalaxy::Galaxy->CheckIntegrityChecksum(10007);
        aScript::ExecuteGameplayUiCode(Block, Key);
        aGalaxy::Galaxy->PrimeIntegrityChecksum(20007);
    }

    void TfStarMap::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
            return;
        }
        if (GlobalsV::MusicInSpaceEnabled) {
            if (aPlayer::GetPlayer()->GetHull()->CapitalShip == 1 && aMyFunction::RandomIntRange(0, 100) < 20) {
                Globals::StarMapScreen->BattleMusicSelected = true;
                GR_Main::MusicManager->PlayCategory(u"Destroyer"_wref.get());
            } else {
                Globals::StarMapScreen->BattleMusicSelected = false;
                GR_Main::MusicManager->PlayCategory(u"StarMap"_wref.get());
            }
        } else {
            GR_Main::MusicManager->RequestFadeOut();
        }
    }

    void TfStarMap::p_destroy() {
        fStarMap::TfStarMap_Destroy(this);
    }

} // namespace fStarMap
