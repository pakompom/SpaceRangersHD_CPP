#include "layout/ab_MainForm.hpp"
#include "types/Achievements.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MultiImage.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_StarField.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Music.hpp"
#include "types/GR_Rect.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Star.hpp"
#include "types/SystemImports.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/fScore.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBuf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EC_Thread.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PolyLine.hpp"
#include "units/GI_RotateImage5.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/SysUtilsImports.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aItem.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPath.hpp"
#include "units/aPlayer.hpp"
#include "units/aSaveLoad.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/abWall.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_Hit.hpp"
#include "units/ab_Item.hpp"
#include "units/ab_MainForm.hpp"
#include "units/ab_Object.hpp"
#include "units/ab_Polygon.hpp"
#include "units/ab_Ship.hpp"
#include "units/ab_ShipAI.hpp"
#include "units/ab_Space.hpp"
#include "units/ab_StopLine.hpp"
#include "units/ab_W.hpp"
#include "units/ab_WorldImage.hpp"
#include "units/ab_WorldLine.hpp"
#include "units/ab_Zone.hpp"
#include "units/fLoad.hpp"
#include "units/fPanelLoad.hpp"
#include "units/fShip2.hpp"
#include "units/fTalk.hpp"

namespace ab_MainForm {
    // Borrowed ActiveArcadeRequest.Ships.
    aMyFunction::TObjectList* ActiveArcadeRequestShips = nullptr;

    // Borrowed head of QueuedArcadeBattles.
    aScript::PScriptABRequest ActiveArcadeRequest = nullptr;

    void TfAB_Create(TfAB* Self) {
        GI_MessageLoop::TMessageLoopGI_Create(Self);
        Self->ListedObjects = pas::make_object<pas::List>();
        Self->LoadPanel = pas::construct_call<fPanelLoad::TfPanelLoad>(fPanelLoad::TfPanelLoad_Create);
    }

    void TfAB_Destroy(TfAB* Self) {
        if (Self->ListedObjects != nullptr) {
            pas::free(Self->ListedObjects);
            Self->ListedObjects = nullptr;
        }
        if (Self->LoadPanel != nullptr) {
            pas::free(Self->LoadPanel);
            Self->LoadPanel = nullptr;
        }
        GI_MessageLoop::TMessageLoopGI_Destroy(Self);
    }

    void TfAB::InitializeLayout() {
        std::int32_t Index{};
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        GR_Main::AppendLogTextThreadSafe("ab_MainForm... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* Map = MainPanel->FindByNameRecursive(u"Map"_wref.get());
                Map->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                {
                    GI_MessageLoop::TObjectGI* SE = Map->FindByNameRecursive(u"SE"_wref.get());
                    SE->SetPosition(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                    SE->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                    SE->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                }
                {
                    GI_MessageLoop::TObjectGI* UpdateObj = Map->FindByNameRecursive(u"UpdateObj"_wref.get());
                    UpdateObj->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                    {
                        GI_MessageLoop::TObjectGI* FPS = UpdateObj->FindByNameRecursive(u"FPS"_wref.get());
                        FPS->SetPosition(ClassesImports::Point(FPS->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, FPS->LocalPosition.Y));
                    }
                    {
                        GI_MessageLoop::TObjectGI* LInfo = UpdateObj->FindByNameRecursive(u"LInfo"_wref.get());
                        LInfo->SetPosition(ClassesImports::Point(LInfo->LocalPosition.X, LInfo->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                        LInfo->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, LInfo->ClientSize.Y));
                    }
                    {
                        GI_MessageLoop::TObjectGI* LHelp = UpdateObj->FindByNameRecursive(u"LHelp"_wref.get());
                        LHelp->SetPosition(ClassesImports::Point(LHelp->LocalPosition.X, LHelp->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                        LHelp->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth - 10, LHelp->ClientSize.Y));
                    }
                    if (GR_Main::GiResourceVariant() == 2) {
                        GI_MessageLoop::TObjectGI* ABInfo = UpdateObj->FindByNameRecursive(u"ABInfo"_wref.get());
                        ABInfo->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, ABInfo->ClientSize.Y));
                    }
                    {
                        GI_MessageLoop::TObjectGI* PanelWeapon = UpdateObj->FindByNameRecursive(u"PanelWeapon"_wref.get());
                        PanelWeapon->SetPosition(ClassesImports::Point(PanelWeapon->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PanelWeapon->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                    }
                    {
                        GI_MessageLoop::TObjectGI* PRight = UpdateObj->FindByNameRecursive(u"PRight"_wref.get());
                        PRight->SetPosition(ClassesImports::Point(PRight->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, PRight->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                    }
                    {
                        GI_MessageLoop::TObjectGI* PItem = UpdateObj->FindByNameRecursive(u"PItem"_wref.get());
                        PItem->SetPosition(ClassesImports::Point(PItem->LocalPosition.X, PItem->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                    }
                    {
                        GI_MessageLoop::TObjectGI* PanelWin = UpdateObj->FindByNameRecursive(u"PanelWin"_wref.get());
                        PanelWin->SetPosition(ClassesImports::Point(PanelWin->LocalPosition.X, PanelWin->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                        PanelWin->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, PanelWin->ClientSize.Y));
                        {
                            GI_MessageLoop::TObjectGI* PanelWinHide = PanelWin->FindByNameRecursive(u"PanelWinHide"_wref.get());
                            PanelWinHide->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, PanelWinHide->ClientSize.Y));
                            {
                                GI_MessageLoop::TObjectGI* WinText = PanelWinHide->FindByNameRecursive(u"WinText"_wref.get());
                                WinText->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, WinText->ClientSize.Y));
                            }
                            {
                                GI_MessageLoop::TObjectGI* WinItem = PanelWinHide->FindByNameRecursive(u"WinItem"_wref.get());
                                WinItem->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth - 4, WinItem->ClientSize.Y));
                            }
                            {
                                GI_MessageLoop::TObjectGI* WinShr = PanelWinHide->FindByNameRecursive(u"WinShr"_wref.get());
                                WinShr->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, WinShr->ClientSize.Y));
                            }
                        }
                    }
                    {
                        GI_MessageLoop::TObjectGI* PanelMenuLose = UpdateObj->FindByNameRecursive(u"PanelMenuLose"_wref.get());
                        PanelMenuLose->SetPosition(ClassesImports::Point(PanelMenuLose->LocalPosition.X, PanelMenuLose->LocalPosition.Y + GR_Main::ExtraScreenHeight));
                        PanelMenuLose->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, PanelMenuLose->ClientSize.Y));
                        {
                            GI_MessageLoop::TObjectGI* PanelLoseHide = PanelMenuLose->FindByNameRecursive(u"PanelLoseHide"_wref.get());
                            PanelLoseHide->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, PanelLoseHide->ClientSize.Y));
                            {
                                GI_MessageLoop::TObjectGI* PanelLose = PanelLoseHide->FindByNameRecursive(u"PanelLose"_wref.get());
                                PanelLose->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, PanelLose->ClientSize.Y));
                            }
                            {
                                GI_MessageLoop::TObjectGI* LoseKeyPress = PanelLoseHide->FindByNameRecursive(u"LoseKeyPress"_wref.get());
                                LoseKeyPress->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth - 4, LoseKeyPress->ClientSize.Y));
                            }
                            {
                                GI_MessageLoop::TObjectGI* LoseShr = PanelLoseHide->FindByNameRecursive(u"LoseShr"_wref.get());
                                LoseShr->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, LoseShr->ClientSize.Y));
                            }
                        }
                    }
                }
                {
                    GI_MessageLoop::TObjectGI* StarField = Map->FindByNameRecursive(u"StarField"_wref.get());
                    StarField->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
                    StarField->SetPosition(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                    StarField->SetOrigin(ClassesImports::Point(pas::shr(GR_Main::GameScreenWidth, 1), pas::shr(GR_Main::GameScreenHeight, 1)));
                }
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        LoadPanel->InitializeLayout(this);
        SetHelpCallback(pas::bind_method<&TfAB::UpdateHelp>(this));
        {
            GI_MessageLoop::TObjectGI* MainPanel_2 = GetByName(u"MainPanel"_wref.get());
            MainPanel_2->KeyDownCallback = pas::bind_method<&TfAB::BattleKeyDown>(this);
            MainPanel_2->KeyUpCallback = pas::bind_method<&TfAB::BattleKeyUp>(this);
            MainPanel_2->LeftButtonDownCallback = pas::bind_method<&TfAB::BattleMouseDown>(this);
            MainPanel_2->LeftButtonUpCallback = pas::bind_static_method<&TfAB::BattleMouseUp>(this);
            MainPanel_2->RightButtonDownCallback = pas::bind_method<&TfAB::BattleRightMouseDown>(this);
            MainPanel_2->RightButtonUpCallback = pas::bind_method<&TfAB::BattleRightMouseUp>(this);
            MainPanel_2->MouseMoveCallback = pas::bind_method<&TfAB::BattleMouseMove>(this);
        }
        MapPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"Map"_wref.get()));
        WorldPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"SE"_wref.get()));
        this->StarField = pas::checked_cast<GI_StarField::TStarFieldGI*>(GetByName(u"StarField"_wref.get()));
        StartStarImage = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"StarStart"_wref.get()));
        EndStarImage = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"StarEnd"_wref.get()));
        ItemPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PItem"_wref.get()));
        ItemInfoWindow = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"InfoItem"_wref.get()));
        AutoButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButAuto"_wref.get()));
        AutoButton->UpCallback = pas::bind_method<&TfAB::ToggleAutopilot>(this);
        ManualButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButManual"_wref.get()));
        ManualButton->UpCallback = pas::bind_method<&TfAB::ToggleAutopilot>(this);
        BattleHelpLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LHelp"_wref.get()));
        VictoryPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelWin"_wref.get()));
        DefeatPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"PanelMenuLose"_wref.get()));
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButShip"_wref.get()))->UpCallback = pas::bind_method<&TfAB::OpenShipEquipment>(this);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, 4); cpp_range.next(Index); ) {
            WeaponButtons[Index] = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"F", SysUtils::IntToStr(Index + 1)}))));
            pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(WeaponButtons[Index])->UpCallback = pas::bind_method<&TfAB::WeaponButtonClick>(this);
            WeaponChargeImages[Index] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(Index + 1), "C"}))));
            WeaponPrimaryImages[Index] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(Index + 1), "P"}))));
            WeaponSecondaryImages[Index] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(Index + 1), "P2"}))));
            WeaponIcons[Index] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"W", SysUtils::IntToStr(Index + 1), "I"}))));
        }
        PlayButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPlay"_wref.get()));
        PlayButton->UpCallback = pas::bind_method<&TfAB::TogglePause>(this);
        PauseButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButPause"_wref.get()));
        PauseButton->UpCallback = pas::bind_method<&TfAB::TogglePause>(this);
        SelectedMapName = pas::WideString();
    }

    void TfAB::OnOpen() {
        ab_Ship::TabShip* Ship{};
        ab_Ship::TabShip* ShipX0{};
        ab_Ship::TabShip* ShipX1{};
        std::int32_t Index{};
        if (!GlobalsV::MusicInHyperEnabled) {
            GR_Main::MusicManager->RequestFadeOut();
        }
        LoadPanel->OnOpen();
        if (pas::list_count(aScript::QueuedArcadeBattles) > 0) {
            ActiveArcadeRequest = pas::list_at<aScript::TScriptABRequest>(aScript::QueuedArcadeBattles, 0);
        } else {
            ActiveArcadeRequest = nullptr;
        }
        if (ActiveArcadeRequest != nullptr) {
            ActiveArcadeRequestShips = ActiveArcadeRequest->Ships;
        } else {
            ActiveArcadeRequestShips = nullptr;
        }
        if (aGalaxy::Galaxy != nullptr && aPlayer::GetPlayer() != nullptr && ActiveArcadeRequest == nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->IsChaoticRandomEnabled() ^ 1)) {
            if (aPlayer::GetPlayer()->TransitOriginStar != nullptr) {
                InitialRandomSeed = aPlayer::GetPlayer()->TransitOriginStar->GenerationSeed * aPlayer::GetPlayer()->CurrentStar->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 77 + 1783);
            } else {
                InitialRandomSeed = aPlayer::GetPlayer()->CurrentStar->GenerationSeed * (aGalaxy::Galaxy->CurrentTurn / 77 + 1783);
            }
            RandomSeed = InitialRandomSeed;
        } else {
            InitialRandomSeed = aMyFunction::RandomIntRange(100000, SystemImports::MaxInt);
            RandomSeed = InitialRandomSeed;
        }
        ab_Space::ArcadeKellerEncounter = false;
        ab_Global::ArcadeMapViewPosition = ClassesImports::Point(0, 0);
        DefeatCountdownTicks = 150;
        if (aPlayer::GetPlayer() != nullptr) {
            DepartureTurn = aGalaxy::Galaxy->CurrentTurn;
            ArrivalTurn = DepartureTurn + (aPlayer::GetPlayer()->OrderStateData & 0x0000ffff);
        }
        GetByName(u"LInfo"_wref.get())->SetActive(false);
        ShipPath = pas::construct_call<aPath::TSPath>(aPath::TSPath_Create);
        RouteSpaces = pas::make_object<pas::List>();
        if (ab_Global::ArcadeSpaceProcess != nullptr) {
            pas::free(ab_Global::ArcadeSpaceProcess);
            ab_Global::ArcadeSpaceProcess = nullptr;
        }
        ab_Global::ArcadeSpaceProcess = pas::construct_call<SE_Process::TProcessSE>(SE_Process::TProcessSE_Create, u"Process.Normal"_wref.get());
        ab_Global::ArcadeSpaceProcess->RadarCenter = EC_Struct::MakePointF(0.0f, 0.0f);
        ab_Global::ArcadeSpaceProcess->RadarRange = 0;
        ab_Global::ArcadeSpaceProcess->ActionRange = 0;
        ab_Global::ArcadeSpaceProcess->ActionColor = 0u;
        ab_Global::ArcadeSpaceProcess->OpenSpace(WorldPanel, this);
        ab_Global::ArcadeSpaceProcess->Space->AlphaShift = 0;
        if (aPlayer::GetPlayer() != nullptr) {
            if (aPlayer::GetPlayer()->IsHealthEffectActive(1)) {
                ab_Global::ArcadeSpaceProcess->Space->AlphaShift = 2;
            }
        }
        GlobalsV::SkipSavedPixelRestore = true;
        StarField->Stars->Clear();
        StarField->BackgroundScale = 8.0f;
        WorldLines = pas::construct_call<GI_PolyLine::TPolyLineGI>(GI_PolyLine::TPolyLineGI_Create, MapPanel);
        WorldLines->SetDepth(2.0E+2);
        WorldLines->NormalizeBounds = false;
        WorldLines->SetPosition(StarField->LocalPosition);
        WorldLines->SetSize(StarField->ClientSize);
        WorldLines->SetOrigin(StarField->OriginPoint);
        WorldLines->AutoRebuildBounds = true;
        WorldLines->SetPositionModeW(true);
        WorldLines->SetActive(true);
        WorldCenterX = MapPanel->ClientSize.X / 2;
        WorldCenterY = MapPanel->ClientSize.Y / 2;
        if (aPlayer::GetPlayer() == nullptr) {
            {
                SE_Space::TObjectSE* createSpaceObjectByName = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), u"Ship.People.Ranger"_wref.get(), ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> playerVisual = pas::Var<SE_Space::TObjectSE*>(&PlayerVisual);
                SE_Space::RetainSpaceObject(playerVisual, createSpaceObjectByName);
            }
            PlayerVisual->SetSize(ClassesImports::Point(GR_Main::GiScalePixels(64), GR_Main::GiScalePixels(64)));
        } else {
            if (pas::class_cast_if<SE_Ship2::TShip2SE*>(aPlayer::GetPlayer()->Graphic) != nullptr) {
                SE_Space::TObjectSE* createSpaceObjectByName_2 = SE_Process::CreateSpaceObjectByName(u"Ship2"_wref.get(), aPlayer::GetPlayer()->Graphic->GraphKey, ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> playerVisual_2 = pas::Var<SE_Space::TObjectSE*>(&PlayerVisual);
                SE_Space::RetainSpaceObject(playerVisual_2, createSpaceObjectByName_2);
            } else {
                SE_Space::TObjectSE* createSpaceObjectByName_3 = SE_Process::CreateSpaceObjectByName(u"Ruins"_wref.get(), aPlayer::GetPlayer()->Graphic->GraphKey, ClassesImports::Point(0, 0));
                pas::Var<SE_Space::TObjectSE*> playerVisual_3 = pas::Var<SE_Space::TObjectSE*>(&PlayerVisual);
                SE_Space::RetainSpaceObject(playerVisual_3, createSpaceObjectByName_3);
            }
            PlayerVisual->SetSize(ClassesImports::Point(aPlayer::GetPlayer()->Graphic->Size.X, aPlayer::GetPlayer()->Graphic->Size.Y));
        }
        PlayerVisual->SetAlpha(255);
        if (pas::class_cast_if<SE_Ship2::TShip2SE*>(PlayerVisual) != nullptr) {
            reinterpret_cast<SE_Ship2::TShip2SE*>(PlayerVisual)->TailEmitIntervalMs = 10u;
            if (GlobalsV::ShipTail != 0) {
                reinterpret_cast<SE_Ship2::TShip2SE*>(PlayerVisual)->SetTailMode(1);
            } else {
                reinterpret_cast<SE_Ship2::TShip2SE*>(PlayerVisual)->SetTailMode(0);
            }
        }
        ab_Object::ab_Object_Clear();
        Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
        ab_Object::ab_Object_Add(Ship);
        ab_Ship::PlayerArcadeShip = Ship;
        if (aPlayer::GetPlayer() == nullptr) {
            Ship->CreateShipVisual(u"Ship.People.Ranger"_wref.get(), 64);
        } else {
            if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(aPlayer::GetPlayer()->Graphic) != nullptr) {
                Ship->CreateRuinsVisual(aPlayer::GetPlayer()->Graphic->GraphKey, aPlayer::GetPlayer()->Graphic->Size.X);
            } else if (GR_Main::GiResourceVariant() == 2) {
                Ship->CreateShipVisual(aPlayer::GetPlayer()->Graphic->GraphKey, aPlayer::GetPlayer()->Graphic->Size.X);
            } else {
                Ship->CreateShipVisual(aPlayer::GetPlayer()->Graphic->GraphKey, System::Round(pas::real_divide(pas::shl(aPlayer::GetPlayer()->Graphic->Size.X, 10), 8.0E+2L)));
            }
            aPlayer::GetPlayer()->ScriptItemsAct(0x00000037, Ship, nullptr, 0);
        }
        Ship->MaxSpeed = 11.0;
        Ship->TurnSpeed = ab_Global::PlayerInitialTurnSpeed;
        Ship->Thrust = 0.0;
        if (aPlayer::GetPlayer() == nullptr) {
            Ship->MaxHealth = 1000;
            Ship->Health = 1000;
            Ship->WeaponCount = 5;
            {
                std::uint8_t pickRandomItemType = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg = &Ship->Weapons[0];
                ab_W::ab_Weapon_Initialize(cpp_arg, pickRandomItemType);
            }
            Ship->Weapons[0].SlotData = 0u;
            {
                std::uint8_t pickRandomItemType_2 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_2 = &Ship->Weapons[1];
                ab_W::ab_Weapon_Initialize(cpp_arg_2, pickRandomItemType_2);
            }
            Ship->Weapons[1].SlotData = 1u;
            {
                std::uint8_t pickRandomItemType_3 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_3 = &Ship->Weapons[2];
                ab_W::ab_Weapon_Initialize(cpp_arg_3, pickRandomItemType_3);
            }
            Ship->Weapons[2].SlotData = 2u;
            {
                std::uint8_t pickRandomItemType_4 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_4 = &Ship->Weapons[3];
                ab_W::ab_Weapon_Initialize(cpp_arg_4, pickRandomItemType_4);
            }
            Ship->Weapons[3].SlotData = 3 | aItem::EquipmentSecondaryFireFlag;
            {
                std::uint8_t pickRandomItemType_5 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_5 = &Ship->Weapons[4];
                ab_W::ab_Weapon_Initialize(cpp_arg_5, pickRandomItemType_5);
            }
            Ship->Weapons[4].SlotData = 4 | aItem::EquipmentSecondaryFireFlag;
            Ship->PrimaryWeapon = -1;
            Ship->SecondaryWeapon = -1;
            TfAB::NormalizeWeaponSelection();
        } else {
            Ship->MaxHealth = aPlayer::GetPlayer()->GetHull()->Weight;
            Ship->Health = aPlayer::GetPlayer()->GetHull()->HullPoints;
            for (Index = 0; Index <= 4; ++Index) {
                CampaignWeapons[Index] = nullptr;
            }
            SyncWeaponInventory();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Ship->WeaponCount - 1); cpp_range.next(Index); ) {
                Ship->Weapons[Index].Ammo = Ship->Weapons[Index].MaxAmmo;
            }
            Ship->PrimaryWeapon = -1;
            Ship->SecondaryWeapon = -1;
            TfAB::NormalizeWeaponSelection();
        }
        if (aPlayer::GetPlayer() == nullptr) {
            Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
            ab_Object::ab_Object_Add(Ship);
            Ship->CreateShipVisual(u"Ship.X.0"_wref.get(), 64);
            Ship->MaxSpeed = 12.0;
            Ship->TurnSpeed = 4.0;
            Ship->Thrust = 0.0;
            Ship->Health = 500;
            Ship->MaxHealth = 500;
            Ship->WeaponCount = 5;
            {
                std::uint8_t pickRandomItemType_6 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_6 = &Ship->Weapons[0];
                ab_W::ab_Weapon_Initialize(cpp_arg_6, pickRandomItemType_6);
            }
            {
                std::uint8_t pickRandomItemType_7 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_7 = &Ship->Weapons[1];
                ab_W::ab_Weapon_Initialize(cpp_arg_7, pickRandomItemType_7);
            }
            {
                std::uint8_t pickRandomItemType_8 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_8 = &Ship->Weapons[2];
                ab_W::ab_Weapon_Initialize(cpp_arg_8, pickRandomItemType_8);
            }
            {
                std::uint8_t pickRandomItemType_9 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_9 = &Ship->Weapons[3];
                ab_W::ab_Weapon_Initialize(cpp_arg_9, pickRandomItemType_9);
            }
            {
                std::uint8_t pickRandomItemType_10 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_10 = &Ship->Weapons[4];
                ab_W::ab_Weapon_Initialize(cpp_arg_10, pickRandomItemType_10);
            }
            Ship->PrimaryWeapon = 0;
            ShipX0 = Ship;
            Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
            ab_Object::ab_Object_Add(Ship);
            Ship->CreateShipVisual(u"Ship.X.1"_wref.get(), 64);
            Ship->MaxSpeed = 12.0;
            Ship->TurnSpeed = 4.0;
            Ship->Thrust = 0.0;
            Ship->Health = 500;
            Ship->MaxHealth = 500;
            Ship->WeaponCount = 5;
            {
                std::uint8_t pickRandomItemType_11 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_11 = &Ship->Weapons[0];
                ab_W::ab_Weapon_Initialize(cpp_arg_11, pickRandomItemType_11);
            }
            {
                std::uint8_t pickRandomItemType_12 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_12 = &Ship->Weapons[1];
                ab_W::ab_Weapon_Initialize(cpp_arg_12, pickRandomItemType_12);
            }
            {
                std::uint8_t pickRandomItemType_13 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_13 = &Ship->Weapons[2];
                ab_W::ab_Weapon_Initialize(cpp_arg_13, pickRandomItemType_13);
            }
            {
                std::uint8_t pickRandomItemType_14 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_14 = &Ship->Weapons[3];
                ab_W::ab_Weapon_Initialize(cpp_arg_14, pickRandomItemType_14);
            }
            {
                std::uint8_t pickRandomItemType_15 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_15 = &Ship->Weapons[4];
                ab_W::ab_Weapon_Initialize(cpp_arg_15, pickRandomItemType_15);
            }
            Ship->PrimaryWeapon = 0;
            ShipX1 = Ship;
            Ship = pas::construct_call<ab_ShipAI::TabShipAI>(ab_ShipAI::TabShipAI_Create);
            ab_Object::ab_Object_Add(Ship);
            Ship->CreateShipVisual(u"Ship.X.2"_wref.get(), 64);
            Ship->MaxSpeed = 12.0;
            Ship->TurnSpeed = 4.0;
            Ship->Thrust = 0.0;
            Ship->Health = 500;
            Ship->MaxHealth = 500;
            Ship->WeaponCount = 5;
            {
                std::uint8_t pickRandomItemType_16 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_16 = &Ship->Weapons[0];
                ab_W::ab_Weapon_Initialize(cpp_arg_16, pickRandomItemType_16);
            }
            {
                std::uint8_t pickRandomItemType_17 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_17 = &Ship->Weapons[1];
                ab_W::ab_Weapon_Initialize(cpp_arg_17, pickRandomItemType_17);
            }
            {
                std::uint8_t pickRandomItemType_18 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_18 = &Ship->Weapons[2];
                ab_W::ab_Weapon_Initialize(cpp_arg_18, pickRandomItemType_18);
            }
            {
                std::uint8_t pickRandomItemType_19 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_19 = &Ship->Weapons[3];
                ab_W::ab_Weapon_Initialize(cpp_arg_19, pickRandomItemType_19);
            }
            {
                std::uint8_t pickRandomItemType_20 = aConst::PickRandomItemType(pas::constant_set<aConst::TItemTypeSelection>({{50, 67}}));
                ab_W::PabWeapon cpp_arg_20 = &Ship->Weapons[4];
                ab_W::ab_Weapon_Initialize(cpp_arg_20, pickRandomItemType_20);
            }
            Ship->PrimaryWeapon = 0;
            for (Index = 0; Index <= 3; ++Index) {
                ab_Hit::KellerFragments[Index] = nullptr;
                ab_Hit::KellerFragmentDistances[Index] = 0.0;
                ab_Hit::KellerFragmentValuesAC[Index] = 0.0;
            }
            ab_Hit::KellerBreakupTicks = 0;
            ab_Hit::KellerSplitActive = false;
            Ship->AddEnemy(ab_Ship::PlayerArcadeShip);
            ShipX0->AddEnemy(ab_Ship::PlayerArcadeShip);
            ShipX1->AddEnemy(ab_Ship::PlayerArcadeShip);
            ab_Ship::PlayerArcadeShip->AddEnemy(Ship);
            ab_Ship::PlayerArcadeShip->AddEnemy(ShipX0);
            ab_Ship::PlayerArcadeShip->AddEnemy(ShipX1);
        }
        if (aPlayer::GetPlayer() == nullptr) {
            float headingDegreesToRadians = aMyFunction::HeadingDegreesToRadians(RandomRange(0, 355));
            std::int32_t randomIntRange = aMyFunction::RandomIntRange(3, 8);
            TfAB* self = this;
            self->ABSpaceBuild(randomIntRange, headingDegreesToRadians);
        } else if (ActiveArcadeRequest != nullptr) {
            float headingDegreesToRadians_2 = aMyFunction::HeadingDegreesToRadians(RandomRange(0, 355));
            TfAB* self_2 = this;
            self_2->ABSpaceBuild(1, headingDegreesToRadians_2);
        } else if (aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
            if (aPlayer::GetPlayer()->TransitOriginStar != nullptr) {
                ABSpaceBuild(1, aMyFunction::HeadingDegreesToRadians(aMyFunction::PointBearingDegrees(aPlayer::GetPlayer()->TransitOriginStar->Position, aPlayer::GetPlayer()->CurrentStar->Position)));
            } else {
                ABSpaceBuild(1, 0.0f);
            }
        } else if (aPlayer::GetPlayer()->TransitOriginStar != nullptr) {
            float headingDegreesToRadians_3 = aMyFunction::HeadingDegreesToRadians(aMyFunction::PointBearingDegrees(aPlayer::GetPlayer()->TransitOriginStar->Position, aPlayer::GetPlayer()->CurrentStar->Position));
            std::int32_t round = System::Round(aMyFunction::RemapClamped(aMyFunction::PointDistance(aPlayer::GetPlayer()->CurrentStar->Position, aPlayer::GetPlayer()->TransitOriginStar->Position), 1.0E+1, aConst::GalaxySizeY / 2, 3.0, 8.0));
            ABSpaceBuild(round, headingDegreesToRadians_3);
        } else {
            ABSpaceBuild(3, 0.0f);
        }
        ForwardKeyDown = false;
        ReverseKeyDown = false;
        BrakeKeyDown = false;
        TurnLeftKeyDown = false;
        TurnRightKeyDown = false;
        PrimaryFireKeyDown = false;
        SecondaryFireKeyDown = false;
        ab_Ship::ArcadePaused = false;
        ab_Ship::ArcadePauseWithShift = false;
        PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
        PauseButton->SetActive(ab_Ship::ArcadePaused);
        CampaignTransitionStarted = false;
        CampaignLoadStarted = false;
        CampaignLoadFinished = false;
        CacheLoader = nullptr;
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(605);
        }
        if (aPlayer::GetPlayer() != nullptr) {
            aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 2);
            CacheLoader = pas::construct_call<fLoad::TCacheLoader>(EC_Thread::TThreadEC_Create);
        }
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(607);
            aGalaxy::Galaxy->PrimeIntegrityChecksum2(608);
        }
        if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->Order == aShip::soJumpHole || ActiveArcadeRequest != nullptr || aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->IsOldHyperspaceEnabled() ^ 1)) {
            ab_Space::CurrentArcadeSpace = ab_Space::NextArcadeSpace;
            ab_Global::ArcadeMapViewPosition = ab_Space::CurrentArcadeSpace->MapPosition;
            EnterCurrentSpace();
            SelectMusic();
        } else {
            EnterMapView();
        }
        ab_Global::ArcadeTickCount = 0;
        UpdateTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfAB::TimerTakt>(this), 0);
        ScrollTimer = ScheduleCallbackTimer(GlobalsV::ScrollTime, GlobalsV::ScrollTime, pas::bind_method<&TfAB::ScrollMapTimer>(this), 0);
        TimerTakt(nullptr, 0);
        HideHelp();
    }

    void TfAB::OnClose() {
        std::int32_t Index{};
        LoadPanel->OnClose();
        SelectedMapName = pas::WideString();
        ClearShipPath();
        ClearBattle();
        ab_Space::ab_Space_Clear();
        if (CacheLoader != nullptr) {
            CacheLoader->ClearFlag18();
            pas::free(CacheLoader);
            CacheLoader = nullptr;
        }
        if (PlayerVisual != nullptr) {
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&PlayerVisual));
        }
        if (ScrollTimer != nullptr) {
            CancelCallbackTimer(ScrollTimer);
            ScrollTimer = nullptr;
        }
        if (UpdateTimer != nullptr) {
            CancelCallbackTimer(UpdateTimer);
            UpdateTimer = nullptr;
        }
        if (WorldLines != nullptr) {
            pas::free(WorldLines);
            WorldLines = nullptr;
        }
        if (ab_Global::ArcadeSpaceProcess != nullptr) {
            pas::free(ab_Global::ArcadeSpaceProcess);
            ab_Global::ArcadeSpaceProcess = nullptr;
        }
        if (ShipPath != nullptr) {
            pas::free(ShipPath);
            ShipPath = nullptr;
        }
        if (RouteSpaces != nullptr) {
            pas::free(RouteSpaces);
            RouteSpaces = nullptr;
        }
        if (ab_Global::ArcadeMapColorBuffer != nullptr) {
            pas::free(ab_Global::ArcadeMapColorBuffer);
            ab_Global::ArcadeMapColorBuffer = nullptr;
        }
        for (Index = 0; Index <= 7; ++Index) {
            ClearEnemyStatus(Index);
        }
        for (Index = 0; Index <= 7; ++Index) {
            ClearTrackedShipStatus(Index);
        }
        CloseVictory(nullptr, 0u);
    }

    void TfAB::RequestExit(GI_MessageLoop::TObjectGI* Sender) {
        std::uint8_t Standalone{};
        if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LanguageDataConfig->GetParamByPathOrMarker(u"FormGameMenu.QExit"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
            Standalone = aGalaxy::Galaxy == nullptr;
            if (CacheLoader != nullptr) {
                CacheLoader->ClearFlag18();
            }
            ClearShipPath();
            ab_Ship::PlayerArcadeShip = nullptr;
            ab_Object::ab_Object_Clear();
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
            if (Standalone) {
                GlobalsV::PostLoadScreenId = GlobalsV::screenLoadArcade;
            } else {
                GlobalsV::PostLoadScreenId = GlobalsV::screenMainMenu;
            }
            GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
            aScript::ClearPendingScriptRequests();
            ActiveArcadeRequest = nullptr;
            ActiveArcadeRequestShips = nullptr;
            aPlayer::ArcadeKellerDefeats = 0;
            if (aPlayer::ArcadeKellerReward != nullptr) {
                pas::free(aPlayer::ArcadeKellerReward);
                aPlayer::ArcadeKellerReward = nullptr;
            }
            RequestClose(1);
            GI_Main::BreakUiMessage();
        }
    }

    void TfAB::BattleKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey) {
        std::int32_t Index{};
        pas::Object* Obj{};
        std::int32_t EnemyCount{};
        CancelCargoPickup();
        if ((VirtualKey == WindowsSdk::VK_ESCAPE || VirtualKey == WindowsSdk::VK_SPACE) && (VictoryPanel->Active || DefeatPanel->Active)) {
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->GetHull()->HullPoints > 0 && VictoryPanel->Active) {
                if (GI_MessageBox::ShowMessageBoxGI(this, GR_Main::LookupLocalizedTextByKey(u"FormAB.QueryExit"_wref.get()), GI_MessageBox::mbgOK | GI_MessageBox::mbgCancel | GI_MessageBox::mbgQuestion, 0, 0, 0) == GI_MessageBox::mbgResultOK) {
                    if (aPlayer::GetPlayer()->Order == aShip::soJumpHole || ActiveArcadeRequest != nullptr) {
                        BeginBattleExit();
                    } else {
                        BeginMapTransition();
                    }
                }
            } else if (aGalaxy::Galaxy == nullptr) {
                Globals::ScreenLoadMode = 4;
                GlobalsV::PostLoadScreenId = GlobalsV::screenLoadArcade;
                GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
                RequestClose(1);
            } else {
                CloseVictory(nullptr, 0u);
            }
            return;
        }
        if (VirtualKey == WindowsSdk::VK_TAB && VictoryPanel->Active) {
            GI_MessageLoop::TObjectGI* PanelWinHide = GetByName(u"PanelWinHide"_wref.get());
            PanelWinHide->SetActive(static_cast<std::uint8_t>(PanelWinHide->Active ^ 1));
        }
        if (GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            if (VirtualKey == 'K' && aPlayer::GetPlayer() == nullptr && ab_Ship::PlayerArcadeShip != nullptr) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ab_Ship::PlayerArcadeShip->Enemies) - 1); cpp_range.next(Index); ) {
                    std::int32_t health = pas::list_at<ab_Ship::TabShip>(ab_Ship::PlayerArcadeShip->Enemies, Index)->Health;
                    ab_Ship::TabShip* cpp_arg = pas::list_at<ab_Ship::TabShip>(ab_Ship::PlayerArcadeShip->Enemies, Index);
                    cpp_arg->ApplyDamage(health, nullptr, false);
                }
            }
            return;
        }
        if (ab_Ship::PlayerArcadeShip != nullptr && VirtualKey >= '1' && VirtualKey <= '5' && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT)) {
            if (!pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(WeaponButtons[VirtualKey - '1'])->Disabled) {
                ToggleWeaponGroup(WeaponButtons[VirtualKey - '1']);
            }
        } else if (ab_Ship::PlayerArcadeShip != nullptr && VirtualKey >= '1' && VirtualKey <= '5') {
            if (static_cast<std::int32_t>(VirtualKey - '1') < 5) {
                WeaponSelect(WeaponButtons[VirtualKey - '1']);
            }
        } else if (VirtualKey == WindowsSdk::VK_SPACE && ab_Global::ArcadeViewMode == 2) {
            ab_Ship::ArcadePaused = static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1);
            PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
            PauseButton->SetActive(ab_Ship::ArcadePaused);
            ab_Ship::ArcadePauseWithShift = GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT);
        } else if (VirtualKey == WindowsSdk::VK_RETURN && ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr && ab_Space::CurrentArcadeSpace != ab_Space::EndArcadeSpace && ab_Space::CurrentArcadeSpace != ab_Space::EndArcadeSpace) {
            EnemyCount = 0;
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ab_Space::CurrentArcadeSpace->Objects) - 1); cpp_range_2.next(Index); ) {
                Obj = pas::list_at<pas::Object>(ab_Space::CurrentArcadeSpace->Objects, Index);
                if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Obj) != nullptr) {
                    ++EnemyCount;
                }
            }
            if (EnemyCount > 0) {
                EnterCurrentSpace();
            }
        } else if (VirtualKey == 'C' && ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr) {
            ab_Global::ArcadeMapViewPosition = EC_Struct::TruncatePointF(PlayerMapPosition);
        } else if (VirtualKey == 'Q' && aPlayer::GetPlayer() == nullptr) {
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[0], 50);
            ab_Ship::PlayerArcadeShip->Weapons[0].SlotData = 0u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[1], 51);
            ab_Ship::PlayerArcadeShip->Weapons[1].SlotData = 1u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[2], 52);
            ab_Ship::PlayerArcadeShip->Weapons[2].SlotData = 2u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[3], 53);
            ab_Ship::PlayerArcadeShip->Weapons[3].SlotData = 3 | aItem::EquipmentSecondaryFireFlag;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[4], 54);
            ab_Ship::PlayerArcadeShip->Weapons[4].SlotData = 4 | aItem::EquipmentSecondaryFireFlag;
            TfAB::NormalizeWeaponSelection();
            UpdateWeaponPanel();
        } else if (VirtualKey == 'W' && aPlayer::GetPlayer() == nullptr) {
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[0], 55);
            ab_Ship::PlayerArcadeShip->Weapons[0].SlotData = 0u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[1], 56);
            ab_Ship::PlayerArcadeShip->Weapons[1].SlotData = 1u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[2], 57);
            ab_Ship::PlayerArcadeShip->Weapons[2].SlotData = 2u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[3], 58);
            ab_Ship::PlayerArcadeShip->Weapons[3].SlotData = 3 | aItem::EquipmentSecondaryFireFlag;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[4], 59);
            ab_Ship::PlayerArcadeShip->Weapons[4].SlotData = 4 | aItem::EquipmentSecondaryFireFlag;
            TfAB::NormalizeWeaponSelection();
            UpdateWeaponPanel();
        } else if (VirtualKey == 'E' && aPlayer::GetPlayer() == nullptr) {
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[0], 60);
            ab_Ship::PlayerArcadeShip->Weapons[0].SlotData = 0u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[1], 61);
            ab_Ship::PlayerArcadeShip->Weapons[1].SlotData = 1u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[2], 62);
            ab_Ship::PlayerArcadeShip->Weapons[2].SlotData = 2u;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[3], 63);
            ab_Ship::PlayerArcadeShip->Weapons[3].SlotData = 3 | aItem::EquipmentSecondaryFireFlag;
            ab_W::ab_Weapon_Initialize(&ab_Ship::PlayerArcadeShip->Weapons[4], 64);
            ab_Ship::PlayerArcadeShip->Weapons[4].SlotData = 4 | aItem::EquipmentSecondaryFireFlag;
            TfAB::NormalizeWeaponSelection();
            UpdateWeaponPanel();
        } else if (VirtualKey == WindowsSdk::VK_SPACE && ab_Global::ArcadeViewMode == 2 && ab_Space::CurrentArcadeSpace == ab_Space::NextArcadeSpace) {
            EnemyCount = 0;
            for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(ab_Space::CurrentArcadeSpace->Objects) - 1); cpp_range_3.next(Index); ) {
                Obj = pas::list_at<pas::Object>(ab_Space::CurrentArcadeSpace->Objects, Index);
                if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Obj) != nullptr) {
                    ++EnemyCount;
                }
            }
            if (EnemyCount > 0) {
                EnterCurrentSpace();
            }
        }
        if (VirtualKey == WindowsSdk::VK_UP || VirtualKey == 'R' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            ForwardKeyDown = true;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_DOWN || VirtualKey == 'F' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            ReverseKeyDown = true;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_SPACE || VirtualKey == WindowsSdk::VK_SHIFT) {
            SecondaryFireKeyDown = true;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_LEFT || VirtualKey == 'D' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            TurnLeftKeyDown = true;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_RIGHT || VirtualKey == 'G' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            TurnRightKeyDown = true;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_CONTROL) {
            PrimaryFireKeyDown = true;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_ESCAPE) {
            RequestExit(nullptr);
        } else if (VirtualKey == 'A') {
            ToggleAutopilot(nullptr);
        } else if (VirtualKey == 'S' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            OpenShipEquipment(nullptr);
        } else if (VirtualKey == 'P' || VirtualKey == WindowsSdk::VK_PAUSE) {
            SimulationPaused = static_cast<std::uint8_t>(SimulationPaused ^ 1);
        }
    }

    void TfAB::BattleKeyUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey) {
        CancelCargoPickup();
        if (VirtualKey == WindowsSdk::VK_UP || VirtualKey == 'R' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            ForwardKeyDown = false;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_DOWN || VirtualKey == 'F' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            ReverseKeyDown = false;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_SPACE || VirtualKey == WindowsSdk::VK_SHIFT) {
            SecondaryFireKeyDown = false;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_LEFT || VirtualKey == 'D' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            TurnLeftKeyDown = false;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_RIGHT || VirtualKey == 'G' && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1)) {
            TurnRightKeyDown = false;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        } else if (VirtualKey == WindowsSdk::VK_CONTROL) {
            PrimaryFireKeyDown = false;
            ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
            ab_Global::ArcadeAutopilotEnabled = false;
            UpdateAutopilotButtons();
        }
    }

    void TfAB::BattleMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        ab_Space::TabSpace* Origin{};
        pas::List* Route{};
        std::int32_t Index{};
        pas::Object* Obj{};
        std::int32_t EnemyCount{};
        if (ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr && ab_Space::HoveredArcadeSpace != nullptr && static_cast<std::uint8_t>(Sender->IsOccludedAtPoint(Point) ^ 1)) {
            Origin = ab_Space::CurrentArcadeSpace;
            if (pas::list_count(RouteSpaces) > 0 && GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                Origin = pas::list_at<ab_Space::TabSpace>(RouteSpaces, pas::list_count(RouteSpaces) - 1);
            }
            if (ab_Space::HoveredArcadeSpace == ab_Space::CurrentArcadeSpace && ab_Space::CurrentArcadeSpace != ab_Space::EndArcadeSpace) {
                EnemyCount = 0;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ab_Space::CurrentArcadeSpace->Objects) - 1); cpp_range.next(Index); ) {
                    Obj = pas::list_at<pas::Object>(ab_Space::CurrentArcadeSpace->Objects, Index);
                    if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Obj) != nullptr) {
                        ++EnemyCount;
                    }
                }
                if (EnemyCount > 0) {
                    EnterCurrentSpace();
                }
            } else if (pas::list_count(RouteSpaces) > 0 && pas::list_get(RouteSpaces, pas::list_count(RouteSpaces) - 1) == ab_Space::HoveredArcadeSpace) {
                ab_Ship::ArcadePaused = true;
                PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
                PauseButton->SetActive(ab_Ship::ArcadePaused);
                ab_Ship::ArcadePauseWithShift = GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT);
                HideObjectInfo();
            } else {
                ClearShipPath();
                if (!GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL)) {
                    pas::list_clear(RouteSpaces);
                }
                Route = pas::make_object<pas::List>();
                TfAB::BuildSpaceRoute(Route, Origin, ab_Space::HoveredArcadeSpace);
                if (pas::list_count(Route) > 0) {
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Route) - 1); cpp_range_2.next(Index); ) {
                        pas::list_add(RouteSpaces, pas::list_get(Route, Index));
                    }
                }
                pas::free(Route);
                RebuildShipPath();
                BuildShipPathImages();
            }
        } else if (ab_Global::ArcadeViewMode == 0 && CargoPickupItem != nullptr && CargoPickupItem->BonusKind < 0 && ab_Ship::PlayerArcadeShip != nullptr && ab_Ship::PlayerArcadeShip->Health > 0 && aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CargoFreeSpace >= CargoPickupItem->Item->Weight && ab_Ship::PlayerArcadeShip->DistanceTo(CargoPickupItem) < ab_Global::ManualCargoPickupDistance && aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetCargoHook()) && aShip::TShip_CalculateCargoHookPower(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetCargoHook()) >= CargoPickupItem->Item->Weight) {
            TfAB::PickUpItem(CargoPickupItem);
            CancelCargoPickup();
        }
    }

    void TfAB::BattleMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
    }

    void TfAB::BattleRightMouseDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr && static_cast<std::uint8_t>(ContentPanel->IsOccludedAtPoint(Point) ^ 1)) {
            MapDrag.Active = true;
            pas::store_unaligned<WindowsSdk::TPoint>(&MapDrag.Position, Point);
            if (IsCursorImageSelected(u"Main"_wref.get())) {
                SetCursorByName(u"Scroll"_wref.get());
            }
        }
    }

    void TfAB::BattleRightMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (ab_Global::ArcadeViewMode == 2 && MapDrag.Active) {
            MapDrag.Active = false;
            if (IsCursorImageSelected(u"Scroll"_wref.get())) {
                SetCursorByName(u"Main"_wref.get());
            }
        }
    }

    void TfAB::BattleMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        double Distance{};
        double NearestDistance{};
        double ItemDistance{};
        ab_Space::TabSpace* Space{};
        ab_Object::TabObject* Obj{};
        WindowsSdk::TPoint LocalPoint{};
        EC_Struct::TVector3D Position{};
        if (MapDrag.Active) {
            if (IsCursorImageSelected(u"Main"_wref.get())) {
                SetCursorByName(u"Scroll"_wref.get());
            }
            ab_Global::ArcadeMapViewPosition = ClassesImports::Point(ab_Global::ArcadeMapViewPosition.X + MapDrag.Position.X - Point.X, ab_Global::ArcadeMapViewPosition.Y + MapDrag.Position.Y - Point.Y);
            pas::store_unaligned<WindowsSdk::TPoint>(&MapDrag.Position, Point);
            if (ab_Global::ArcadeMapBounds.Top - ab_Global::ArcadeMapPanMargin > ab_Global::ArcadeMapViewPosition.Y) {
                ab_Global::ArcadeMapViewPosition.Y = ab_Global::ArcadeMapBounds.Top - ab_Global::ArcadeMapPanMargin;
            }
            if (ab_Global::ArcadeMapBounds.Bottom + ab_Global::ArcadeMapPanMargin < ab_Global::ArcadeMapViewPosition.Y) {
                ab_Global::ArcadeMapViewPosition.Y = ab_Global::ArcadeMapBounds.Bottom + ab_Global::ArcadeMapPanMargin;
            }
            if (ab_Global::ArcadeMapBounds.Left - ab_Global::ArcadeMapPanMargin > ab_Global::ArcadeMapViewPosition.X) {
                ab_Global::ArcadeMapViewPosition.X = ab_Global::ArcadeMapBounds.Left - ab_Global::ArcadeMapPanMargin;
            }
            if (ab_Global::ArcadeMapBounds.Right + ab_Global::ArcadeMapPanMargin < ab_Global::ArcadeMapViewPosition.X) {
                ab_Global::ArcadeMapViewPosition.X = ab_Global::ArcadeMapBounds.Right + ab_Global::ArcadeMapPanMargin;
            }
        } else {
            if (Point.X == 0 || Point.Y == 0 || GR_Main::GameScreenWidth - 1 == Point.X || GR_Main::GameScreenHeight - 1 == Point.Y) {
                if (ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr) {
                    SetCursorByName(u"Scroll"_wref.get());
                    return;
                }
            } else if (ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr) {
                Point.X = Point.X - WorldCenterX + ab_Global::ArcadeMapViewPosition.X;
                Point.Y = Point.Y - WorldCenterY + ab_Global::ArcadeMapViewPosition.Y;
                ab_Space::HoveredArcadeSpace = nullptr;
                NearestDistance = 1.0E+20;
                Space = ab_Space::FirstArcadeSpace;
                while (Space != nullptr) {
                    Distance = aMyFunction::PointDistanceSquared(EC_Struct::PointToPointF(Point), EC_Struct::PointToPointF(Space->MapPosition));
                    if (Distance < NearestDistance && static_cast<long double>(pas::sqr(GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius))) > Distance) {
                        NearestDistance = Distance;
                        ab_Space::HoveredArcadeSpace = Space;
                    }
                    Space = Space->Next;
                }
                ShowSpaceInfo(ab_Space::HoveredArcadeSpace);
            } else if (ab_Global::ArcadeViewMode == 0) {
                LocalPoint = WorldPanel->ToLocalPoint(Point);
                Obj = ab_Object::FirstArcadeObject;
                while (Obj != nullptr) {
                    if (pas::class_cast_if<ab_Item::TabItem*>(Obj) != nullptr) {
                        Position = Obj->GetWorldPosition();
                        Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Position));
                        if (ab_Global::IsDepthBeforeSphereHorizon(Position.Z)) {
                            ItemDistance = pas::sqr(static_cast<long double>(LocalPoint.X) - Position.X) + pas::sqr(static_cast<long double>(LocalPoint.Y) - Position.Y);
                            if (ItemDistance < 256.0L) {
                                ShowItemInfo(reinterpret_cast<ab_Item::TabItem*>(Obj));
                                break;
                            }
                        }
                    }
                    Obj = Obj->Next;
                }
                if (Obj == nullptr) {
                    CancelCargoPickup();
                }
            }
            if (IsCursorImageSelected(u"Scroll"_wref.get())) {
                SetCursorByName(u"Main"_wref.get());
            }
        }
    }

    void TfAB::ClearOverlaySegments() {
        std::int32_t Index{};
        for (Index = 0; Index <= 3; ++Index) {
            if (OverlaySegments[Index] != nullptr) {
                WorldLines->RetireSegment(OverlaySegments[Index]);
                OverlaySegments[Index] = nullptr;
            }
        }
    }

    std::uint8_t TfAB::ScreenPointToSphere(WindowsSdk::TPoint Point, double& Longitude, double& PolarAngle) {
        EC_Struct::TVector3D Source{};
        EC_Struct::TVector3D RayOrigin{};
        EC_Struct::TVector3D RayDirection{};
        ab_Global::TMatrix4D Matrix{};
        Point = WorldPanel->ToLocalPoint(Point);
        Matrix = ab_Global::InvertMatrix4D(ab_Global::SpherePerspectiveMatrix);
        Source = EC_Struct::MakeVector3D(Point.X, Point.Y, 1.0);
        Source = ab_Global::ProjectPointByMatrix(Matrix, pas::ConstRef<EC_Struct::TVector3D>(&Source));
        Matrix = ab_Global::InvertMatrix4D(ab_Global::SphereViewMatrix);
        RayDirection.X = static_cast<long double>(Source.X) * Matrix[0][0] + static_cast<long double>(Source.Y) * Matrix[1][0] + static_cast<long double>(Source.Z) * Matrix[2][0];
        RayDirection.Y = static_cast<long double>(Source.X) * Matrix[0][1] + static_cast<long double>(Source.Y) * Matrix[1][1] + static_cast<long double>(Source.Z) * Matrix[2][1];
        RayDirection.Z = static_cast<long double>(Source.X) * Matrix[0][2] + static_cast<long double>(Source.Y) * Matrix[1][2] + static_cast<long double>(Source.Z) * Matrix[2][2];
        RayOrigin.X = Matrix[3][0];
        RayOrigin.Y = Matrix[3][1];
        RayOrigin.Z = Matrix[3][2];
        std::uint8_t Result = ab_Global::TryIntersectRayWithSphere(RayOrigin, EC_Struct::MakeVector3D(static_cast<long double>(RayOrigin.X) + RayDirection.X, static_cast<long double>(RayOrigin.Y) + RayDirection.Y, static_cast<long double>(RayOrigin.Z) + RayDirection.Z), EC_Struct::MakeVector3D(0.0, 0.0, 0.0), ab_Global::SphereRadius, RayOrigin);
        if (Result) {
            ab_Global::VectorToSphericalAngles(RayOrigin, Longitude, PolarAngle);
        }
        return Result;
    }

    void TfAB::UpdateWeaponPanel() {
        std::int32_t Value{};
        std::int32_t Index{};
        std::int32_t SlotIndex{};
        std::int32_t Group{};
        GI_GraphButton::TGraphButtonGI* Button{};
        aItem::TWeapon* Item{};
        pas::WideString WeaponName{};
        std::int32_t MicroModule{};
        if (ab_Ship::PlayerArcadeShip == nullptr || ab_Ship::PlayerArcadeShip->Health <= 0 || ab_Global::ArcadeViewMode == 5) {
            ClearWeaponPanel();
        } else {
            GetByName(u"PanelWeapon"_wref.get())->SetActive(true);
            for (auto cpp_range = pas::for_to<std::int32_t>(1, 5); cpp_range.next(Value); ) {
                Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(static_cast<pas::WideString>(pas::concat_ansi({"F", SysUtils::IntToStr(Value)}))));
                Button->HelpText = pas::WideString();
                Button->SetDisabled(true);
                Group = 0;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ab_Ship::PlayerArcadeShip->WeaponCount - 1); cpp_range_2.next(Index); ) {
                    SlotIndex = ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData & aItem::EquipmentSlotIndexMask;
                    if (Value - 1 == SlotIndex) {
                        if ((ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData & aItem::EquipmentSecondaryFireFlag) != 0) {
                            Group = 1;
                        }
                        break;
                    }
                }
                if (Group != 0) {
                    Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"GN"}));
                    Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"GA"}));
                    Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"GD"}));
                } else {
                    Button->SetImageNormalPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"BN"}));
                    Button->SetImageNormalActivePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"BA"}));
                    Button->SetImageDownPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"BD"}));
                }
                Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"H"}));
                Button->ImageNormal->SetImageKindY(GI_Main::ikyBottom);
                Button->ImageNormalActive->SetImageKindY(GI_Main::ikyBottom);
                Button->ImageDown->SetImageKindY(GI_Main::ikyBottom);
                Button->UserValue = -1;
                Button->UserIndex = -1;
                Button->UserData = Value;
                Button->StateChangedCallback = pas::bind_static_method<&TfAB::WeaponStateChanged>(this);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ab_Ship::PlayerArcadeShip->WeaponCount - 1); cpp_range_3.next(Index); ) {
                    SlotIndex = ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData & aItem::EquipmentSlotIndexMask;
                    if (Value - 1 == SlotIndex) {
                        Button->UserIndex = Index;
                        break;
                    }
                }
                if (aPlayer::GetPlayer() != nullptr) {
                    Item = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::t_Weapon1, Value - 1));
                    if (aPlayer::GetPlayer()->GetSlotCount(aConst::sskWeapon) <= Value - 1) {
                        Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"H"}));
                    } else if (Item == nullptr) {
                        Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"E"}));
                    } else if (!aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Item)) {
                        Button->SetImageDisabledPath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int_to_str(Value), u"R"}));
                    }
                }
                WeaponChargeImages[Value - 1]->SetImagePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"CurH"}));
                WeaponPrimaryImages[Value - 1]->SetActive(false);
                WeaponSecondaryImages[Value - 1]->SetActive(false);
                {
                    GI_Image::TImageGI* cpp_with = WeaponIcons[Value - 1];
                    if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::t_Weapon1, Value - 1) != nullptr) {
                        WeaponIcons[Value - 1]->SetActive(true);
                        cpp_with->SetImagePath(pas::concat_wide({u"GI,", aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::t_Weapon1, Value - 1)->GetBitmapResourceName(), u"s"}));
                    } else if (Button->UserIndex >= 0) {
                        WeaponIcons[Value - 1]->SetActive(true);
                        cpp_with->SetImagePath(pas::concat_wide({u"GI,Bm.Items.", GR_Main::GiResourceSuffix(), aConst::ItemTypeNames[ab_Ship::PlayerArcadeShip->Weapons[Button->UserIndex].ItemType], u"s"}));
                    } else {
                        WeaponIcons[Value - 1]->SetActive(false);
                    }
                    cpp_with->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with->SetImageKindY(GI_Main::ikyCenter);
                }
            }
            Value = 0;
            while (Value < ab_Ship::PlayerArcadeShip->WeaponCount) {
                SlotIndex = ab_Ship::PlayerArcadeShip->Weapons[Value].SlotData & aItem::EquipmentSlotIndexMask;
                pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(WeaponButtons[SlotIndex])->SetDisabled(false);
                WeaponButtons[SlotIndex]->SetActive(true);
                WeaponButtons[SlotIndex]->HelpCallback = pas::bind_method<&TfAB::UpdateHelp>(this);
                if (WeaponButtons[SlotIndex]->UserState == 0) {
                    WeaponButtons[SlotIndex]->UserState = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, WeaponButtons[SlotIndex])));
                }
                if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::t_Weapon1, SlotIndex) != nullptr) {
                    Item = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::t_Weapon1, SlotIndex));
                    MicroModule = Item->MicroModuleIndex;
                    Item->MicroModuleIndex = 0;
                    WeaponName = Item->GetShortName();
                    Item->MicroModuleIndex = MicroModule;
                } else {
                    WeaponName = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Weapon.Name.", SysUtils::IntToStr(ab_Ship::PlayerArcadeShip->Weapons[Value].ItemType - 50 + 1)})));
                }
                if ((ab_Ship::PlayerArcadeShip->Weapons[Value].SlotData & aItem::EquipmentSecondaryFireFlag) != 0) {
                    WeaponButtons[SlotIndex]->HelpText = ([&] {
                        const pas::WideString& replaceAllWideString = ([&] {
                            const pas::WideString& intToStr = pas::wide_int_to_str(Value + 1);
                            const pas::WideString& lookupLocalizedTextByKey = GR_Main::LookupLocalizedTextByKey(u"Help.ABWeapon2"_wref.get());
                            return EC_Str::ReplaceAllWideString(lookupLocalizedTextByKey, u"<SelectKey>"_wref.get(), intToStr);
                        }());
                        const pas::WideString& weaponName = WeaponName;
                        return EC_Str::ReplaceAllWideString(replaceAllWideString, u"<WeaponName>"_wref.get(), weaponName);
                    }());
                } else {
                    WeaponButtons[SlotIndex]->HelpText = ([&] {
                        const pas::WideString& replaceAllWideString_2 = ([&] {
                            const pas::WideString& intToStr_2 = pas::wide_int_to_str(Value + 1);
                            const pas::WideString& lookupLocalizedTextByKey_2 = GR_Main::LookupLocalizedTextByKey(u"Help.ABWeapon1"_wref.get());
                            return EC_Str::ReplaceAllWideString(lookupLocalizedTextByKey_2, u"<SelectKey>"_wref.get(), intToStr_2);
                        }());
                        const pas::WideString& weaponName_2 = WeaponName;
                        return EC_Str::ReplaceAllWideString(replaceAllWideString_2, u"<WeaponName>"_wref.get(), weaponName_2);
                    }());
                }
                pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(WeaponButtons[SlotIndex])->UpdateStateVisuals();
                ++Value;
            }
            UpdateWeaponHighlights(true);
        }
    }

    void TfAB::WeaponStateChanged(GI_MessageLoop::TObjectGI* Sender) {
        float Charge{};
        std::int32_t Index{};
        GI_GraphButton::TGraphButtonGI* Button = reinterpret_cast<GI_GraphButton::TGraphButtonGI*>(Sender);
        GI_Image::TImageGI* Image = reinterpret_cast<GI_Image::TImageGI*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Button->UserState)));
        if (Image != nullptr) {
            if (Button->Disabled) {
                Image->SetActive(false);
            } else {
                Image->SetActive(true);
                if (ab_Ship::PlayerArcadeShip == nullptr) {
                    Charge = 0.0f;
                } else {
                    Index = Sender->UserIndex;
                    Charge = pas::real_divide(ab_Ship::PlayerArcadeShip->Weapons[Index].Ammo, ab_Ship::PlayerArcadeShip->Weapons[Index].MaxAmmo);
                }
                if (Button->Down) {
                    Charge = Charge - 0.05L;
                    if (Charge < 0.0L) {
                        Charge = 0.0f;
                    }
                }
                if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Sender)->Down) {
                    Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Sender->UserData))), u"OD"}));
                } else if (pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(Sender)->IsHovered()) {
                    Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Sender->UserData))), u"OA"}));
                } else {
                    Image->SetImagePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"W", pas::wide_int64_to_str(static_cast<std::int64_t>(static_cast<std::uint32_t>(Sender->UserData))), u"ON"}));
                }
                Image->SetImageKindY(GI_Main::ikyTop);
                Image->SetSize(ClassesImports::Point(Button->ClientSize.X, Button->ClientSize.Y - System::Round(static_cast<long double>(Button->ClientSize.Y) * Charge)));
                Button->ImageNormal->SetPosition(ClassesImports::Point(Button->ImageNormal->LocalPosition.X, Button->ClientSize.Y - System::Round(static_cast<long double>(Button->ClientSize.Y) * Charge)));
                Button->ImageNormal->SetSize(ClassesImports::Point(Button->ImageNormal->ClientSize.X, System::Round(static_cast<long double>(Button->ClientSize.Y) * Charge)));
                Button->ImageNormalActive->SetPosition(ClassesImports::Point(Button->ImageNormalActive->LocalPosition.X, Button->ClientSize.Y - System::Round(static_cast<long double>(Button->ClientSize.Y) * Charge)));
                Button->ImageNormalActive->SetSize(ClassesImports::Point(Button->ImageNormalActive->ClientSize.X, System::Round(static_cast<long double>(Button->ClientSize.Y) * Charge)));
                Button->ImageDown->SetPosition(ClassesImports::Point(Button->ImageDown->LocalPosition.X, Button->ClientSize.Y - System::Round(static_cast<long double>(Button->ClientSize.Y) * Charge)));
                Button->ImageDown->SetSize(ClassesImports::Point(Button->ImageDown->ClientSize.X, System::Round(static_cast<long double>(Button->ClientSize.Y) * Charge)));
            }
        }
    }

    void TfAB::ClearWeaponPanel() {
        std::int32_t Index{};
        {
            GI_MessageLoop::TObjectGI* PanelWeapon = GetByName(u"PanelWeapon"_wref.get());
            if (PanelWeapon->Active) {
                PanelWeapon->FindByNameRecursive(u"PanelWeapon"_wref.get())->SetActive(false);
                for (Index = 0; Index <= 4; ++Index) {
                    WeaponButtons[Index]->SetActive(false);
                    WeaponIcons[Index]->SetActive(false);
                }
                for (Index = 0; Index <= 7; ++Index) {
                    if (BonusIcons[Index] != nullptr) {
                        pas::free(BonusIcons[Index]);
                        BonusIcons[Index] = nullptr;
                        pas::free(BonusRings[Index]);
                        BonusRings[Index] = nullptr;
                    }
                }
                for (Index = 0; Index <= 7; ++Index) {
                    if (EnemyIcons[Index] != nullptr) {
                        pas::free(EnemyIcons[Index]);
                        EnemyIcons[Index] = nullptr;
                        pas::free(EnemyHealthRings[Index]);
                        EnemyHealthRings[Index] = nullptr;
                        if (EnemyRewardIcons[Index] != nullptr) {
                            pas::free(EnemyRewardIcons[Index]);
                        }
                        EnemyRewardIcons[Index] = nullptr;
                        if (EnemyRewardBackdrops[Index] != nullptr) {
                            pas::free(EnemyRewardBackdrops[Index]);
                        }
                        EnemyRewardBackdrops[Index] = nullptr;
                    }
                }
                for (Index = 0; Index <= 7; ++Index) {
                    if (TrackedShipIcons[Index] != nullptr) {
                        pas::free(TrackedShipIcons[Index]);
                        TrackedShipIcons[Index] = nullptr;
                        pas::free(TrackedShipHealthRings[Index]);
                        TrackedShipHealthRings[Index] = nullptr;
                    }
                }
            }
        }
    }

    void TfAB::UpdateWeaponHighlights(std::uint8_t Force) {
        std::int32_t Index{};
        std::int32_t Top{};
        std::int32_t Height{};
        std::int32_t Slot{};
        GI_GraphButton::TGraphButtonGI* Button{};
        GI_Image::TImageGI* Icon{};
        GI_GAI::TgaiGI* Ring{};
        if (ab_Ship::PlayerArcadeShip == nullptr || ab_Ship::PlayerArcadeShip->Health <= 0) {
            ClearWeaponPanel();
        } else {
            Index = 0;
            while (Index < ab_Ship::PlayerArcadeShip->WeaponCount) {
                Slot = ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData & aItem::EquipmentSlotIndexMask;
                Button = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(WeaponButtons[Slot]);
                if (Force || (ab_Global::ArcadeTickCount & 3) == 0) {
                    Button->UpdateStateVisuals();
                    if (ab_Ship::PlayerArcadeShip->PrimaryWeapon == Index) {
                        WeaponChargeImages[Slot]->SetImagePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"CurB"}));
                    } else if (ab_Ship::PlayerArcadeShip->SecondaryWeapon == Index) {
                        WeaponChargeImages[Slot]->SetImagePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"CurG"}));
                    } else {
                        WeaponChargeImages[Slot]->SetImagePath(pas::concat_wide({u"GI,Bm.FormAB2.", GR_Main::GiResourceSuffix(), u"CurH"}));
                    }
                    WeaponPrimaryImages[Slot]->SetActive(ab_Ship::PlayerArcadeShip->Weapons[Index].Ammo < ab_Ship::PlayerArcadeShip->Weapons[Index].AmmoCost && (pas::shr(ab_Global::ArcadeTickCount, 2) & 1) == 0);
                    WeaponSecondaryImages[Slot]->SetActive(ab_Ship::PlayerArcadeShip->Weapons[Index].Ammo < ab_Ship::PlayerArcadeShip->Weapons[Index].AmmoCost && (pas::shr(ab_Global::ArcadeTickCount, 2) & 1) != 0);
                }
                WeaponButtons[Slot]->UserValue = Index;
                ++Index;
            }
            Height = GR_Main::GiScalePixels(64);
            for (Index = 0; Index <= 7; ++Index) {
                if (ab_Ship::PlayerArcadeShip->BonusTicks[Index] <= 0) {
                    if (BonusIcons[Index] != nullptr) {
                        pas::free(BonusIcons[Index]);
                        BonusIcons[Index] = nullptr;
                        pas::free(BonusRings[Index]);
                        BonusRings[Index] = nullptr;
                        HideHelp();
                    }
                } else if (BonusIcons[Index] == nullptr) {
                    BonusIcons[Index] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, ItemPanel);
                    Icon = BonusIcons[Index];
                    Icon->SetImagePath(pas::concat_wide({u"GI,Bm.ABItem.", GR_Main::GiResourceSuffix(), u"_0", pas::wide_int_to_str(Index), u"_i"}));
                    Icon->SetSize(ClassesImports::Point(ItemPanel->ClientSize.X, Height));
                    Icon->SetDepth(0.0);
                    BonusRings[Index] = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, ItemPanel);
                    Ring = BonusRings[Index];
                    Ring->SetImagePath(pas::concat_wide({u"Bm.ABItem.", GR_Main::GiResourceSuffix(), u"_Ring"}));
                    Ring->SetSize(ClassesImports::Point(ItemPanel->ClientSize.X, Height));
                    Ring->SequenceIndex = 0;
                    Ring->UpdateAutoGeometry();
                    Ring->SetMouseViewUpdates(true);
                    Ring->MouseBlocking = true;
                    Ring->StopAutoPlayback();
                    Ring->SetDepth(1.0);
                    Ring->HelpCallback = pas::bind_method<&TfAB::UpdateHelp>(this);
                    Ring->MouseEnterCallback = pas::bind_method<&TfAB::ControlMouseEnter>(this);
                    Ring->MouseLeaveCallback = pas::bind_method<&TfAB::ControlMouseLeave>(this);
                    if (Index == 0) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemLife"_wref.get());
                    } else if (Index == 1) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemFast"_wref.get());
                    } else if (Index == 2) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemSlow"_wref.get());
                    } else if (Index == 3) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemLock"_wref.get());
                    } else if (Index == 4) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemDamage"_wref.get());
                    } else if (Index == 5) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemReload"_wref.get());
                    } else if (Index == 6) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemDefence"_wref.get());
                    } else if (Index == 7) {
                        Ring->HelpText = GR_Main::LookupLocalizedTextByKey(u"Help.ABItemInvisible"_wref.get());
                    }
                }
            }
            Top = ItemPanel->ClientSize.Y - Height;
            for (Index = 0; Index <= 7; ++Index) {
                if (ab_Ship::PlayerArcadeShip->BonusTicks[Index] > 0) {
                    BonusIcons[Index]->SetPosition(ClassesImports::Point(0, Top));
                    BonusRings[Index]->SetPosition(ClassesImports::Point(0, Top));
                    BonusRings[Index]->SetSequenceFrame(System::Round((1.0L - pas::real_divide(ab_Ship::PlayerArcadeShip->BonusTicks[Index], ab_Global::BonusDurationSeconds[Index] * 20)) * BonusRings[Index]->SequenceFrameCount));
                    Top -= Height;
                }
            }
        }
    }

    void TfAB::WeaponSelect(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        if (ab_Ship::PlayerArcadeShip != nullptr) {
            Index = Sender->UserValue;
            if (Index >= 0 && ab_Ship::PlayerArcadeShip->PrimaryWeapon != Index) {
                ab_Ship::PlayerArcadeShip->SelectWeapon(Index);
                UpdateWeaponHighlights(true);
            }
        }
    }

    void TfAB::ToggleWeaponGroup(GI_MessageLoop::TObjectGI* Sender) {
        std::int32_t Index{};
        if (ab_Ship::PlayerArcadeShip != nullptr) {
            Index = Sender->UserValue;
            if (Index >= 0) {
                ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData = ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData ^ aItem::EquipmentSecondaryFireFlag;
                if (aGalaxy::Galaxy != nullptr) {
                    aGalaxy::Galaxy->CheckIntegrityChecksum1(640);
                }
                if (aPlayer::GetPlayer() != nullptr) {
                    aItem::TWeapon* cpp_with = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::t_Weapon1, ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData & aItem::EquipmentSlotIndexMask));
                    cpp_with->AssignedSlotData = ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData;
                }
                TfAB::NormalizeWeaponSelection();
                UpdateWeaponPanel();
                if (aGalaxy::Galaxy != nullptr) {
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(641);
                }
                UpdateHelp(Sender, true);
            }
        }
    }

    void TfAB::WeaponButtonClick(GI_MessageLoop::TObjectGI* Sender) {
        ToggleWeaponGroup(Sender);
    }

    void TfAB::NormalizeWeaponSelection() {
        std::int32_t Index{};
        if (ab_Ship::PlayerArcadeShip->PrimaryWeapon >= 0 && (ab_Ship::PlayerArcadeShip->Weapons[ab_Ship::PlayerArcadeShip->PrimaryWeapon].SlotData & aItem::EquipmentSecondaryFireFlag) != 0) {
            ab_Ship::PlayerArcadeShip->PrimaryWeapon = -1;
        }
        if (ab_Ship::PlayerArcadeShip->PrimaryWeapon < 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, ab_Ship::PlayerArcadeShip->WeaponCount - 1); cpp_range.next(Index); ) {
                if ((ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData & aItem::EquipmentSecondaryFireFlag) == 0) {
                    ab_Ship::PlayerArcadeShip->PrimaryWeapon = Index;
                    break;
                }
            }
        }
        if (ab_Ship::PlayerArcadeShip->SecondaryWeapon >= 0 && (ab_Ship::PlayerArcadeShip->Weapons[ab_Ship::PlayerArcadeShip->SecondaryWeapon].SlotData & aItem::EquipmentSecondaryFireFlag) == 0) {
            ab_Ship::PlayerArcadeShip->SecondaryWeapon = -1;
        }
        if (ab_Ship::PlayerArcadeShip->SecondaryWeapon < 0) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ab_Ship::PlayerArcadeShip->WeaponCount - 1); cpp_range_2.next(Index); ) {
                if ((ab_Ship::PlayerArcadeShip->Weapons[Index].SlotData & aItem::EquipmentSecondaryFireFlag) != 0) {
                    ab_Ship::PlayerArcadeShip->SecondaryWeapon = Index;
                    break;
                }
            }
        }
    }

    void TfAB::UpdateAutopilotButtons() {
        AutoButton->SetActive(ab_Global::ArcadeAutopilotEnabled);
        ManualButton->SetActive(static_cast<std::uint8_t>(ab_Global::ArcadeAutopilotEnabled ^ 1));
    }

    void TfAB::ToggleAutopilot(GI_MessageLoop::TObjectGI* Sender) {
        ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
        ab_Global::ArcadeAutopilotEnabled = static_cast<std::uint8_t>(ab_Global::ArcadeAutopilotEnabled ^ 1);
        UpdateAutopilotButtons();
    }

    void TfAB::TogglePause(GI_MessageLoop::TObjectGI* Sender) {
        HideObjectInfo();
        ab_Ship::ArcadePauseWithShift = GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT);
        ab_Ship::ArcadePaused = Sender == PlayButton;
        PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
        PauseButton->SetActive(ab_Ship::ArcadePaused);
        if (PlayButton->Active) {
            UpdateHelp(PlayButton, true);
        } else {
            UpdateHelp(PauseButton, true);
        }
        GI_Main::BreakUiMessage();
    }

    void TfAB::ReportSurvivingShips() {
        std::int32_t Index{};
        ab_Ship::TabShip* Ship{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (ab_Ship::PlayerArcadeShip != nullptr && ab_Ship::PlayerArcadeShip->Health > 0) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(ab_Ship::PlayerArcadeShip->TrackedShips) - 1); cpp_range.next(Index); ) {
                Ship = pas::list_at<ab_Ship::TabShip>(ab_Ship::PlayerArcadeShip->TrackedShips, Index);
                if (Ship != nullptr && Ship->ScriptLabel != u"" && Ship->Health > 0) {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"LabeledShipSurvivedInAB"_w, nullptr);
                    Event->AddTextData(Ship->ScriptLabel);
                    Event->AddData(Ship->Health);
                    Event->AddData(Ship->MaxHealth);
                }
            }
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(ab_Ship::PlayerArcadeShip->InitialEnemies) - 1); cpp_range_2.next(Index); ) {
                Ship = pas::list_at<ab_Ship::TabShip>(ab_Ship::PlayerArcadeShip->InitialEnemies, Index);
                if (Ship != nullptr && Ship->ScriptLabel != u"" && Ship->Health > 0) {
                    Event = aGalaxyEvent::AddGalaxyEvent(u"LabeledShipSurvivedInAB"_w, nullptr);
                    Event->AddTextData(Ship->ScriptLabel);
                    Event->AddData(Ship->Health);
                    Event->AddData(Ship->MaxHealth);
                }
            }
        }
    }

    void TfAB::ClearBattle() {
        ab_Ship::PlayerArcadeShip = nullptr;
        ab_Object::ab_Object_Clear();
        ClearGrid();
        ClearOverlaySegments();
        ab_Zone::ab_Zone_ClearImages();
        ab_Zone::ab_ZoneLink_ClearImages();
        ab_Polygon::ab_Polygon_Clear();
        ab_StopLine::ab_StopLine_Clear();
        ab_StopLine::ab_StopPoint_Clear();
        ab_WorldLine::ab_WorldLine_Clear();
        ab_WorldImage::ab_WorldImage_Clear();
        ab_Space::ab_Space_ClearImages();
        if (WorldLines != nullptr) {
            WorldLines->ClearSegments();
        }
        MapState2C8 = 0;
    }

    void TfAB::ClearMap() {
        ClearGrid();
        ClearOverlaySegments();
        ab_Zone::ab_Zone_ClearImages();
        ab_Zone::ab_ZoneLink_ClearImages();
        ab_Polygon::ab_Polygon_Clear();
        ab_StopLine::ab_StopLine_Clear();
        ab_StopLine::ab_StopPoint_Clear();
        ab_WorldLine::ab_WorldLine_Clear();
        ab_WorldImage::ab_WorldImage_Clear();
        ab_Space::ab_Space_ClearImages();
        WorldLines->ClearSegments();
    }

    void TfAB::LoadMap(EC_Buf::TBufEC* Buffer, std::uint8_t LoadPolygons) {
        ClearMap();
        if (Buffer == nullptr || Buffer->DataSize < 32) {
            ab_StopLine::ab_StopLine_AddLatitude(1.0E+1, 1.0E+1);
        } else {
            if (EC_Buf::TBufEC_GetUInt32(Buffer) != 0x6d776261) {
                GR_Main::RaiseWideMessage(u"Incorrect format ABMap"_wref.get());
            }
            ab_Global::ArcadeMapVersion = EC_Buf::TBufEC_GetUInt32(Buffer);
            ab_Global::SphereRadius = EC_Buf::TBufEC_GetSingle(Buffer);
            if (ab_Global::ArcadeMapColorBuffer == nullptr) {
                ab_Global::ArcadeMapColorBuffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
            } else {
                ab_Global::ArcadeMapColorBuffer->Clear();
            }
            {
                std::int32_t int32 = EC_Buf::TBufEC_GetInt32(Buffer);
                EC_Buf::TBufEC* arcadeMapColorBuffer = ab_Global::ArcadeMapColorBuffer;
                arcadeMapColorBuffer->SetSize(int32);
            }
            Buffer->ReadBytes(ab_Global::ArcadeMapColorBuffer->Data, ab_Global::ArcadeMapColorBuffer->DataSize);
            ab_StopLine::ab_StopLine_Load(Buffer);
            ab_Zone::ab_Zone_Load(Buffer);
            if (LoadPolygons) {
                ab_Polygon::ab_Polygon_Load(Buffer);
            }
        }
    }

    void TfAB::LoadMapResource(pas::WideString Path, std::uint8_t LoadPolygons) {
        EC_CacheBuf::TCBufEC* Data{};
        EC_CacheBuf::TCBufControlEC* Control = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                EC_Cache::TCacheEC::ResetControl(Control);
                Control->SetCacheKey(Path);
                Data = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                LoadMap(Data->Buffer, LoadPolygons);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Control != nullptr) {
                Control->Release();
                pas::free(Control);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        if (LoadPolygons) {
            Control = nullptr;
            {
                std::exception_ptr cpp_error_2{};
                try {
                    Control = pas::construct_call<EC_CacheBuf::TCBufControlEC>(EC_Cache::TCacheControlEC_Create);
                    EC_Cache::TCacheEC::ResetControl(Control);
                    Control->SetCacheKey(pas::concat_wide({Path, u"_"}));
                    Data = EC_CacheBuf::AcquireOrCreateBuffer(Control);
                    Data->Buffer->ExpandZlibPayloadInPlace();
                    ab_Polygon::ab_Polygon_LoadVisibility(Data->Buffer);
                } catch (...) {
                    cpp_error_2 = std::current_exception();
                }
                if (Control != nullptr) {
                    Control->Release();
                    pas::free(Control);
                }
                if (cpp_error_2) {
                    std::rethrow_exception(cpp_error_2);
                }
            }
        }
        ab_StopLine::ab_StopPoint_ClearIndex();
    }

    void TfAB::LoadMapFile(pas::WideString Path, std::uint8_t LoadPolygons) {
        pas::WideString cpp_text{};
        pas::WideString cpp_text_2{};
        EC_Buf::TBufEC* Buffer = pas::construct_call<EC_Buf::TBufEC>(EC_Buf::TBufEC_Create);
        {
            try {
                Buffer->LoadFromWideFilePath((cpp_text = pas::concat_wide({Path, u".map"}), cpp_text.pchar()));
                LoadMap(Buffer, LoadPolygons);
                if (LoadPolygons) {
                    Buffer->Clear();
                    Buffer->LoadFromWideFilePath((cpp_text_2 = pas::concat_wide({Path, u".opt"}), cpp_text_2.pchar()));
                    Buffer->ExpandZlibPayloadInPlace();
                    ab_Polygon::ab_Polygon_LoadVisibility(Buffer);
                }
            } catch (...) {
                pas::free(Buffer);
                throw;
            }
            pas::free(Buffer);
        }
        ab_StopLine::ab_StopPoint_ClearIndex();
    }

    void TfAB::ClearGrid() {
        std::int32_t Index{};
        if (GridLines != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(GridLines) - 1); cpp_range.next(Index); ) {
                ab_WorldLine::ab_WorldLine_Delete(pas::list_at<ab_WorldLine::TabWorldLine>(GridLines, Index));
            }
            pas::free(GridLines);
            GridLines = nullptr;
        }
    }

    void TfAB::BuildGrid() {
        double PolarAngle{};
        double Longitude{};
        double LongitudeStep{};
        double PolarStep{};
        EC_Struct::TVector3D First{};
        EC_Struct::TVector3D Last{};
        ClearGrid();
        if (ab_Global::ArcadeGridMode != 0 && ab_Global::ArcadeViewMode == 0) {
            GridLines = pas::make_object<pas::List>();
            LongitudeStep = pas::constant(static_cast<double>(SystemImports::Pi / 8.0L));
            PolarStep = pas::constant(static_cast<double>(SystemImports::Pi / 16.0L));
            Longitude = 0.0;
            while (Longitude < 6.282185307179586477L) {
                PolarAngle = PolarStep * 2.0L;
                while (PolarAngle < SystemImports::Pi - PolarStep * 2.0L - 0.001L) {
                    First = ab_Global::SphericalToVector3D(Longitude, PolarAngle, ab_Global::SphereRadius);
                    Last = ab_Global::SphericalToVector3D(Longitude, static_cast<long double>(PolarAngle) + PolarStep, ab_Global::SphereRadius);
                    {
                        std::uint32_t packRgbBytes = GR_Main::CurrentPixelFormat->PackRgbBytes(240, 240, 255);
                        std::uint32_t packRgbBytes_2 = GR_Main::CurrentPixelFormat->PackRgbBytes(75, 75, 75);
                        void* ab_WorldLine_Create = static_cast<void*>(ab_WorldLine::ab_WorldLine_Create(First, Last, 1, packRgbBytes, packRgbBytes_2, true));
                        pas::List* gridLines = GridLines;
                        pas::list_add(gridLines, ab_WorldLine_Create);
                    }
                    PolarAngle = static_cast<long double>(PolarAngle) + PolarStep;
                }
                Longitude = static_cast<long double>(Longitude) + LongitudeStep;
            }
            PolarStep = PolarStep * 2.0L;
            PolarAngle = PolarStep;
            while (PolarAngle < SystemImports::Pi - PolarStep + 0.001L) {
                Longitude = 0.0;
                while (Longitude < 6.282185307179586477L) {
                    First = ab_Global::SphericalToVector3D(Longitude, PolarAngle, ab_Global::SphereRadius);
                    Last = ab_Global::SphericalToVector3D(static_cast<long double>(Longitude) + LongitudeStep, PolarAngle, ab_Global::SphereRadius);
                    {
                        std::uint32_t packRgbBytes_3 = GR_Main::CurrentPixelFormat->PackRgbBytes(240, 240, 255);
                        std::uint32_t packRgbBytes_4 = GR_Main::CurrentPixelFormat->PackRgbBytes(75, 75, 75);
                        void* ab_WorldLine_Create_2 = static_cast<void*>(ab_WorldLine::ab_WorldLine_Create(First, Last, 1, packRgbBytes_3, packRgbBytes_4, true));
                        pas::List* gridLines_2 = GridLines;
                        pas::list_add(gridLines_2, ab_WorldLine_Create_2);
                    }
                    Longitude = static_cast<long double>(Longitude) + LongitudeStep;
                }
                PolarAngle = static_cast<long double>(PolarAngle) + PolarStep;
            }
        }
    }

    void TfAB::ABSpaceBuild(std::int32_t GridSize, float Angle) {
        std::int32_t Index{};
        std::int32_t Attempt{};
        std::int32_t OtherIndex{};
        std::int32_t Choice{};
        ab_Space::TabSpace* Space{};
        ab_Space::TabSpace* Other{};
        ab_Space::PabSpaceLink Link{};
        WindowsSdk::TPoint Current{};
        WindowsSdk::TPoint Candidate{};
        WindowsSdk::TPoint Previous{};
        WindowsSdk::TPoint StartPoint{};
        WindowsSdk::TPoint EndPoint{};
        EC_Struct::TPointF Position{};
        std::uint8_t Changed{};
        EC_BlockPar::TBlockParEC* Config{};
        EC_BlockPar::TBlockParEC* Selected{};
        pas::Array<std::int32_t, 0, 2> Exits{};
        std::int32_t Weight = std::min<std::int64_t>(static_cast<std::int64_t>(10), System::Round(GridSize * 0.8L));
        if (Weight < 1) {
            Weight = 1;
        }
        std::uint8_t Retry = true;
        while (Retry) {
            Retry = false;
            ab_Space::ab_Space_Clear();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, GridSize - 1); cpp_range.next(Index); ) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, GridSize - 1); cpp_range_2.next(Attempt); ) {
                    ab_Space::TabSpace* cpp_with = ab_Space::ab_Space_Add();
                    cpp_with->GridPosition = ClassesImports::Point(Index, Attempt);
                    {
                        std::int32_t cpp_left = RandomRange(-GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius), GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius));
                        Position.X = cpp_left + 5 * GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * Attempt;
                    }
                    {
                        std::int32_t cpp_left_2 = RandomRange(-GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius), GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius));
                        Position.Y = cpp_left_2 + 5 * GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * Index;
                    }
                    {
                        pas::Extended cpp_left_4 = System::Sin(Angle) * Position.X;
                        std::int32_t round = System::Round(cpp_left_4 - System::Cos(Angle) * Position.Y);
                        pas::Extended cpp_left_3 = System::Cos(Angle) * Position.X;
                        std::int32_t round_2 = System::Round(cpp_left_3 + System::Sin(Angle) * Position.Y);
                        cpp_with->MapPosition = ClassesImports::Point(round_2, round);
                    }
                }
            }
            StartPoint = ClassesImports::Point(0, GridSize / 2);
            EndPoint = ClassesImports::Point(GridSize - 1, GridSize / 2);
            Previous = StartPoint;
            if (Weight > 1) {
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, Weight - 1); cpp_range_3.next(Index); ) {
                    Current = StartPoint;
                    Space = ab_Space::ab_Space_Find(Current);
                    while (GridSize - 1 > Current.X) {
                        if (pas::abs(Current.X - EndPoint.X) <= 1 && pas::abs(Current.Y - EndPoint.Y) <= 1) {
                            Candidate = EndPoint;
                        } else {
                            Attempt = 0;
                            while (Attempt < 5) {
                                ++Attempt;
                                Candidate.X = Current.X + 1;
                                Candidate.Y = Current.Y + RandomRange(-1, 1);
                                if ((Candidate.X != Current.X || Candidate.Y != Current.Y) && (Candidate.X != Previous.X || Candidate.Y != Previous.Y) && Candidate.X >= 0 && Candidate.X < GridSize && Candidate.Y >= 0 && Candidate.Y < GridSize) {
                                    if (Candidate.X == Current.X || Candidate.Y == Current.Y || ab_Space::ab_SpaceLink_Find(ab_Space::ab_Space_Find(ClassesImports::Point(Candidate.X, Current.Y)), ab_Space::ab_Space_Find(ClassesImports::Point(Current.X, Candidate.Y))) == nullptr) {
                                        Other = ab_Space::ab_Space_Find(Candidate);
                                        if (Other->IncomingCount < 3 && Other->IncomingCount + Other->OutgoingCount < 4) {
                                            break;
                                        }
                                    }
                                }
                            }
                            if (Attempt >= 5) {
                                break;
                            }
                        }
                        Other = ab_Space::ab_Space_Find(Candidate);
                        if (ab_Space::ab_SpaceLink_Find(Space, Other) == nullptr) {
                            if (Current.X == StartPoint.X && Current.Y == StartPoint.Y && Space->OutgoingCount >= 3) {
                                break;
                            }
                            ab_Space::ab_SpaceLink_Connect(Space, Other);
                            ++Space->OutgoingCount;
                            ++Other->IncomingCount;
                        }
                        Previous = Current;
                        Current = Candidate;
                        Space = Other;
                    }
                    ab_Space::ab_Space_RecountLinks();
                }
                Changed = true;
                while (Changed) {
                    Changed = false;
                    Space = ab_Space::FirstArcadeSpace;
                    while (Space != nullptr) {
                        Other = Space;
                        Space = Space->Next;
                        if ((Other->GridPosition.X != EndPoint.X || Other->GridPosition.Y != EndPoint.Y) && (Other->GridPosition.X != StartPoint.X || Other->GridPosition.Y != StartPoint.Y) && (Other->OutgoingCount == 0 || Other->IncomingCount == 0)) {
                            Changed = true;
                            ab_Space::ab_Space_Delete(Other);
                        }
                    }
                    ab_Space::ab_Space_RecountLinks();
                }
                Space = ab_Space::ab_Space_Find(EndPoint);
                if (Space->IncomingCount <= 0) {
                    Retry = true;
                }
            }
        }
        ab_Space::StartArcadeSpace = ab_Space::ab_Space_Add();
        ab_Space::StartArcadeSpace->GridPosition = ClassesImports::Point(StartPoint.X - 1, StartPoint.Y);
        Position.X = 5 * GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * ab_Space::StartArcadeSpace->GridPosition.Y;
        Position.Y = 5 * GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * ab_Space::StartArcadeSpace->GridPosition.X;
        ab_Space::StartArcadeSpace->BoundaryKind = 1;
        {
            pas::Extended cpp_left_6 = System::Sin(Angle) * Position.X;
            std::int32_t round_3 = System::Round(cpp_left_6 - System::Cos(Angle) * Position.Y);
            pas::Extended cpp_left_5 = System::Cos(Angle) * Position.X;
            std::int32_t round_4 = System::Round(cpp_left_5 + System::Sin(Angle) * Position.Y);
            ab_Space::StartArcadeSpace->MapPosition = ClassesImports::Point(round_4, round_3);
        }
        ab_Space::EndArcadeSpace = ab_Space::ab_Space_Add();
        ab_Space::EndArcadeSpace->GridPosition = ClassesImports::Point(EndPoint.X + 1, EndPoint.Y);
        Position.X = 5 * GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * ab_Space::EndArcadeSpace->GridPosition.Y;
        Position.Y = 5 * GR_Main::GiScalePixels(ab_Global::ArcadeMapNodeRadius) * ab_Space::EndArcadeSpace->GridPosition.X;
        ab_Space::EndArcadeSpace->BoundaryKind = 1;
        {
            pas::Extended cpp_left_8 = System::Sin(Angle) * Position.X;
            std::int32_t round_5 = System::Round(cpp_left_8 - System::Cos(Angle) * Position.Y);
            pas::Extended cpp_left_7 = System::Cos(Angle) * Position.X;
            std::int32_t round_6 = System::Round(cpp_left_7 + System::Sin(Angle) * Position.Y);
            ab_Space::EndArcadeSpace->MapPosition = ClassesImports::Point(round_6, round_5);
        }
        ab_Space::ab_SpaceLink_Connect(ab_Space::StartArcadeSpace, ab_Space::ab_Space_Find(StartPoint));
        ab_Space::ab_SpaceLink_Connect(ab_Space::ab_Space_Find(EndPoint), ab_Space::EndArcadeSpace);
        ab_Space::ab_Space_RecountLinks();
        Space = ab_Space::FirstArcadeSpace;
        while (Space != nullptr) {
            if (ab_Space::StartArcadeSpace != Space && ab_Space::EndArcadeSpace != Space) {
                Space->Danger = RandomRange(10, 100);
            }
            Space = Space->Next;
        }
        Space = ab_Space::StartArcadeSpace;
        while (!(ab_Space::EndArcadeSpace == Space)) {
            Choice = RandomRange(0, Space->OutgoingCount - 1);
            Link = ab_Space::FirstArcadeSpaceLink;
            while (Link != nullptr) {
                if (Link->First == Space) {
                    --Choice;
                    if (Choice < 0) {
                        Space = Link->Last;
                        break;
                    }
                }
                Link = Link->Next;
            }
            Space->Danger = 0.0;
        }
        Other = nullptr;
        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->Order == aShip::soJumpHole && pas::checked_cast<aGalaxy::THole*>(aPlayer::GetPlayer()->OrderTarget)->HoleType == 4 && aGalaxy::Galaxy->KellerLeaveTurn == 0 && aKling::KellerShip != nullptr && aKling::KellerShip->InHyperspace && pas::in_set<4, 5>(aGalaxy::Galaxy->KellerMissionState)) {
            Other = ab_Space::ab_Space_Find(EndPoint);
            Other->Danger = 1.0E+2;
        }
        ab_Space::ab_Space_UpdateApproachDanger();
        Space = ab_Space::FirstArcadeSpace;
        while (Space != nullptr) {
            if (Space->Danger <= 0.0L) {
                Space->AppearanceIndex = RandomRange(0, 1);
            } else if (static_cast<long double>(Space->Danger) + Space->ApproachDanger < ab_Global::ArcadeHighDangerThreshold) {
                Space->AppearanceIndex = RandomRange(0, 1) + 2;
            } else {
                Space->AppearanceIndex = RandomRange(0, 1) + 4;
            }
            Space = Space->Next;
        }
        Space = ab_Space::FirstArcadeSpace;
        while (Space != nullptr) {
            if (ab_Space::StartArcadeSpace != Space && ab_Space::EndArcadeSpace != Space) {
                Space->Color28 = ab_Global::ArcadeMapPalette[Space->AppearanceIndex * 6 + 4];
                Space->Color30 = ab_Global::ArcadeMapPalette[Space->AppearanceIndex * 6 + 5];
                Space->Color2C = ab_Global::ArcadeMapPalette[Space->AppearanceIndex * 6 + 4] & 0x00ffffff | 0x80000000u;
                Space->Color34 = ab_Global::ArcadeMapPalette[Space->AppearanceIndex * 6 + 5] & 0x00ffffff | 0x80000000u;
                if (Space == Other) {
                    Space->PopulateKellerEncounter();
                } else if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->Order == aShip::soJumpHole && ActiveArcadeRequest == nullptr) {
                    Space->PopulateHoleEncounter();
                } else if (aPlayer::GetPlayer() != nullptr && ActiveArcadeRequest != nullptr) {
                    Space->PopulateScriptedEncounter();
                } else {
                    Space->PopulateObjects();
                }
            }
            Space = Space->Next;
        }
        ab_Space::CurrentArcadeSpace = ab_Space::StartArcadeSpace;
        ab_Space::NextArcadeSpace = ab_Space::ab_Space_Find(StartPoint);
        ab_Global::ArcadeMapViewPosition = ab_Space::CurrentArcadeSpace->MapPosition;
        PlayerVisual->SetAngle(aMyFunction::HeadingDegreesToByte(aMyFunction::RadiansToHeadingDegrees(Angle)));
        PlayerMapPosition = EC_Struct::PointToPointF(ab_Space::StartArcadeSpace->MapPosition);
        pas::list_add(RouteSpaces, reinterpret_cast<void*>(ab_Space::NextArcadeSpace));
        RebuildShipPath();
        ClearShipPath();
        ab_Global::ArcadeMapCenter = ClassesImports::Point((ab_Space::StartArcadeSpace->MapPosition.X + ab_Space::EndArcadeSpace->MapPosition.X) / 2, (ab_Space::StartArcadeSpace->MapPosition.Y + ab_Space::EndArcadeSpace->MapPosition.Y) / 2);
        if (aPlayer::GetPlayer() != nullptr) {
            if (ab_Ship::KellerArcadeShip != nullptr) {
                MapBackgroundPath = u"Bm.FormAB2.2bg3"_w;
            } else if (ActiveArcadeRequest != nullptr && ActiveArcadeRequest->BackgroundId != 0) {
                MapBackgroundPath = static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormAB2.2bg", SysUtils::IntToStr(ActiveArcadeRequest->BackgroundId)}));
            } else if (ActiveArcadeRequest != nullptr && ActiveArcadeRequest->BackgroundMapName != u"") {
                MapBackgroundPath = ActiveArcadeRequest->BackgroundMapName;
            } else {
                MapBackgroundPath = static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormAB2.2bg", SysUtils::IntToStr(RandomRange(1, 3))}));
            }
        } else {
            MapBackgroundPath = static_cast<pas::WideString>(pas::concat_ansi({"Bm.FormAB2.2bg", SysUtils::IntToStr(RandomRange(1, 3))}));
        }
        Config = GR_Main::GameDataConfig->GetBlock(u"ABMap"_wref.get());
        std::int32_t BlockCount = Config->GetBlockCount();
        Weight = 0;
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, BlockCount - 1); cpp_range_4.next(Index); ) {
            Weight += EC_Str::ExtractDigitsToIntW(Config->GetBlockByIndex(Index)->GetParam(u"Priority"_wref.get()));
        }
        Space = ab_Space::FirstArcadeSpace;
        while (Space != nullptr) {
            if (Space->BoundaryKind == 1) {
                Space = Space->Next;
            } else {
                if (Space->OutgoingCount > 3) {
                    GR_Main::RaiseWideMessage(u"Error in ABSpaceBuild"_wref.get());
                }
                Changed = false;
                Retry = false;
                Selected = nullptr;
                if (ab_Ship::KellerArcadeShip != nullptr && aPlayer::GetPlayer() != nullptr) {
                    Space->MapPath = u"ABMap.map_boss"_w;
                } else if (aPlayer::GetPlayer() != nullptr && ActiveArcadeRequest != nullptr) {
                    Space->MapPath = ActiveArcadeRequest->MapName;
                } else {
                    Attempt = RandomRange(0, Weight - 1);
                    Index = 0;
                    while (true) {
                        Selected = Config->GetBlockByIndex(Index);
                        if (([&] {
                            const pas::WideString& param = Selected->GetParam(u"Portal"_wref.get());
                            const pas::WideString& intToStr = pas::wide_int_to_str(Space->OutgoingCount);
                            return EC_Str::FindTextOffsetW(param, intToStr, 0);
                        }()) >= 0) {
                            Changed = true;
                            Retry = true;
                            Attempt -= EC_Str::ExtractDigitsToIntW(Selected->GetParam(u"Priority"_wref.get()));
                            if (Attempt < 0) {
                                break;
                            }
                        }
                        ++Index;
                        if (Index >= BlockCount) {
                            Index = 0;
                            if (!Retry) {
                                break;
                            }
                            Retry = false;
                        }
                    }
                    if (!Changed) {
                        GR_Main::RaiseWideMessage(u"ABMap not found"_wref.get());
                    }
                    Space->MapPath = Selected->GetParam(u"Path"_wref.get());
                }
                Space = Space->Next;
            }
        }
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ab_Global::ArcadeMapBounds, 0), ab_Space::FirstArcadeSpace->MapPosition);
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&ab_Global::ArcadeMapBounds, 8), ab_Space::FirstArcadeSpace->MapPosition);
        Space = ab_Space::FirstArcadeSpace;
        while (Space != nullptr) {
            ab_Global::ArcadeMapBounds.Left = std::min<std::int32_t>(ab_Global::ArcadeMapBounds.Left, Space->MapPosition.X);
            ab_Global::ArcadeMapBounds.Right = std::max<std::int32_t>(ab_Global::ArcadeMapBounds.Right, Space->MapPosition.X);
            ab_Global::ArcadeMapBounds.Top = std::min<std::int32_t>(ab_Global::ArcadeMapBounds.Top, Space->MapPosition.Y);
            ab_Global::ArcadeMapBounds.Bottom = std::max<std::int32_t>(ab_Global::ArcadeMapBounds.Bottom, Space->MapPosition.Y);
            Exits[0] = 0;
            Exits[1] = 0;
            Exits[2] = 0;
            if (Space->PortalSlotCount > 1) {
                Exits[1] = 1;
            }
            if (Space->PortalSlotCount > 2) {
                Exits[2] = 2;
            }
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, 4); cpp_range_5.next(Index); ) {
                Attempt = RandomRange(0, Space->PortalSlotCount - 1);
                OtherIndex = RandomRange(0, Space->PortalSlotCount - 1);
                Choice = Exits[Attempt];
                Exits[Attempt] = Exits[OtherIndex];
                Exits[OtherIndex] = Choice;
            }
            Index = 0;
            Link = ab_Space::FirstArcadeSpaceLink;
            while (Link != nullptr) {
                if (Link->First == Space) {
                    if (Index >= 3) {
                        GR_Main::RaiseWideMessage(u"EError"_wref.get());
                    }
                    Link->ExitIndex = Exits[Index];
                    ++Index;
                }
                Link = Link->Next;
            }
            Space = Space->Next;
        }
    }

    void TfAB::ResetBattleControls() {
        CloseVictory(nullptr, 0u);
        pas::list_clear(ListedObjects);
        SimulationPaused = false;
        ForwardKeyDown = false;
        ReverseKeyDown = false;
        BrakeKeyDown = false;
        TurnLeftKeyDown = false;
        TurnRightKeyDown = false;
        PrimaryFireKeyDown = false;
        SecondaryFireKeyDown = false;
        ab_Ship::PlayerArcadeShip->DisruptUntilTick = 0;
        ab_Global::ArcadeLastInputTick = ab_Global::ArcadeTickCount;
        ab_Global::ArcadeAutopilotEnabled = false;
        ab_Global::ArcadeEnemiesDefeated = false;
        UpdateAutopilotButtons();
        GR_Main::CacheLoadLoggingEnabled = false;
        ab_Global::ArcadeViewMode = 0;
        GI_MessageLoop::TMessageLoopGI::SetSystemCursorPosition(ClassesImports::Point(GR_Main::GameScreenWidth - 2, GR_Main::GameScreenHeight - 2));
    }

    void TfAB::BeginMapTransition() {
        ab_Object::TabObject* Obj{};
        CloseVictory(nullptr, 0u);
        ab_Global::ArcadeViewMode = 1;
        CancelCargoPickup();
        ab_Object::TabObject* NextObject = ab_Object::FirstArcadeObject;
        while (NextObject != nullptr) {
            Obj = NextObject;
            NextObject = NextObject->Next;
            if (ab_Ship::PlayerArcadeShip != Obj) {
                ab_Object::ab_Object_Delete(Obj);
            }
        }
        TransitionSpeed = 1.0E+1;
    }

    void TfAB::EnterMapView() {
        CloseVictory(nullptr, 0u);
        ForwardKeyDown = false;
        ReverseKeyDown = false;
        BrakeKeyDown = false;
        TurnLeftKeyDown = false;
        TurnRightKeyDown = false;
        PrimaryFireKeyDown = false;
        SecondaryFireKeyDown = false;
        ab_Global::ArcadeViewMode = 2;
        UpdateWeaponPanel();
        CancelCargoPickup();
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<ab_Ship::TabShip*>(Obj) != nullptr) {
                pas::checked_cast<ab_Ship::TabShip*>(Obj)->DetachVisual();
            }
            Obj = Obj->Next;
        }
        {
            GI_GAI::TgaiGI* cpp_with = StartStarImage;
            cpp_with->SetSize(cpp_with->GetContentSize());
            cpp_with->SetOrigin(EC_Struct::HalfPoint(cpp_with->ClientSize));
            cpp_with->SequenceIndex = 0;
            cpp_with->UpdateAutoGeometry();
            cpp_with->SetActive(true);
            cpp_with->RestartPlayback();
        }
        {
            GI_GAI::TgaiGI* cpp_with_2 = EndStarImage;
            cpp_with_2->SetSize(cpp_with_2->GetContentSize());
            cpp_with_2->SetOrigin(EC_Struct::HalfPoint(cpp_with_2->ClientSize));
            cpp_with_2->SequenceIndex = 0;
            cpp_with_2->UpdateAutoGeometry();
            cpp_with_2->SetActive(true);
            cpp_with_2->RestartPlayback();
        }
        StarField->SetBackgroundImage(MapBackgroundPath);
        StarField->BackgroundScale = 8.0f;
        ab_Global::SphereCameraDistance = 1.0E+3L + ab_Global::SphereNearCameraOffset;
        ab_Global::UpdateSphereProjectionMetrics();
        ab_Space::ab_Space_CreateImages();
        ab_Space::ab_SpaceLink_BuildGeometry();
        PlayerVisual->AttachToSpace(ab_Global::ArcadeSpaceProcess->Space);
        PlayerVisual->SetDepth(ab_Global::ShipFrontDepth);
        if (pas::class_cast_if<SE_Ship2::TShip2SE*>(PlayerVisual) != nullptr) {
            reinterpret_cast<SE_Ship2::TShip2SE*>(PlayerVisual)->SetTailDepth(ab_Global::ShipTailFrontDepth);
        }
        PlayerMapPosition = EC_Struct::PointToPointF(ab_Space::CurrentArcadeSpace->MapPosition);
        PlayerVisual->SetPosition(PlayerMapPosition);
        if (ab_Space::NextArcadeSpace != nullptr) {
            PlayerVisual->SetAngle(aMyFunction::HeadingDegreesToByte(aMyFunction::PointBearingDegrees(EC_Struct::PointToPointF(ab_Space::CurrentArcadeSpace->MapPosition), EC_Struct::PointToPointF(ab_Space::NextArcadeSpace->MapPosition))));
        }
        if (ab_Space::NextArcadeSpace == nullptr) {
            TfAB::BuildSpaceRoute(RouteSpaces, ab_Space::CurrentArcadeSpace, ab_Space::EndArcadeSpace);
        } else if (pas::list_count(RouteSpaces) < 1 || pas::list_get(RouteSpaces, 0) != ab_Space::NextArcadeSpace) {
            pas::list_clear(RouteSpaces);
            pas::list_add(RouteSpaces, reinterpret_cast<void*>(ab_Space::NextArcadeSpace));
        }
        RebuildShipPath();
        if (ab_Space::NextArcadeSpace == nullptr) {
            BuildShipPathImages();
        } else {
            ClearShipPath();
        }
        ab_Ship::ArcadePauseWithShift = false;
        ab_Ship::ArcadePaused = ab_Space::NextArcadeSpace != nullptr;
        PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
        PauseButton->SetActive(ab_Ship::ArcadePaused);
        ab_StopLine::ab_StopLine_Clear();
        ab_StopLine::ab_StopPoint_Clear();
        if (aPlayer::GetPlayer() != nullptr && CampaignTransitionStarted && CampaignLoadStarted && static_cast<std::uint8_t>(CampaignLoadFinished ^ 1)) {
            CacheLoader->ClearFlag18();
        }
        GetByName(u"PRight"_wref.get())->SetActive(true);
    }

    void TfAB::EnterCurrentSpace() {
        ab_Object::TabObject* Obj{};
        ab_Object::TabObject* Other{};
        ab_Space::PabSpaceLink ExitLink{};
        ab_StopLine::PabStopLine StopLine{};
        ab_Zone::PabZone Zone{};
        ab_ShipAI::TabShipAI* Ship{};
        abWall::TabWall* Wall{};
        abWall::TabWall* OtherWall{};
        std::int32_t Index{};
        std::int32_t OtherIndex{};
        std::int32_t DesiredColor{};
        std::int32_t ColorCount{};
        std::int32_t BestDifference{};
        std::int32_t ColorValue{};
        std::int32_t Selection{};
        PArcadeMapColorHeader ColorData{};
        CloseVictory(nullptr, 0u);
        GetByName(u"PRight"_wref.get())->SetActive(false);
        HideObjectInfo();
        CancelCargoPickup();
        std::uint8_t HasEnemies = aPlayer::GetPlayer() == nullptr;
        ab_Global::ArcadeViewMode = 3;
        StarField->SetViewPosition(EC_Struct::MakePointF(0.0f, 0.0f));
        ClearShipPath();
        if (aPlayer::GetPlayer() != nullptr && CampaignTransitionStarted && CampaignLoadStarted && static_cast<std::uint8_t>(CampaignLoadFinished ^ 1)) {
            CacheLoader->SetFlag18();
        }
        ab_Ship::PlayerArcadeShip->StopThrust();
        ab_Ship::PlayerArcadeShip->Velocity = EC_Struct::MakePointF(0.0f, 0.0f);
        ab_Ship::PlayerArcadeShip->SetTurnInput(0.0);
        ab_Space::ab_Space_ClearImages();
        StartStarImage->SetActive(false);
        EndStarImage->SetActive(false);
        PlayerVisual->DetachFromSpace();
        StarField->SetBackgroundImage(MapBackgroundPath);
        StarField->BackgroundScale = 8.0f;
        if (SelectedMapName.length() > 0 && SelectedMapName != u"SkipAB" && SelectedMapName != u"NoEntry") {
            if (SysUtilsImports::FileExists(static_cast<pas::AnsiString>(pas::concat_wide({SelectedMapName, u".map"})))) {
                LoadMapFile(SelectedMapName, true);
            } else {
                LoadMapResource(SelectedMapName, true);
            }
        } else {
            LoadMapResource(ab_Space::CurrentArcadeSpace->MapPath, true);
        }
        std::int32_t Remaining = ab_Global::ArcadeMapColorBuffer->DataSize;
        ColorData = static_cast<PArcadeMapColorHeader>(ab_Global::ArcadeMapColorBuffer->Data);
        while (Remaining > 0) {
            ColorCount = pas::load_unaligned<std::int32_t>(&ColorData->VariantCount);
            Selection = pas::load_unaligned<std::int32_t>(&ColorData->SelectedVariant);
            if (Selection == 0) {
                Selection = 0;
                for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorCount - 1); cpp_range.next(Index); ) {
                    ColorValue = pas::load_unaligned<std::int32_t>(reinterpret_cast<WindowsSdk::PInteger>(Index * static_cast<std::int32_t>(sizeof(TArcadeMapColorVariant)) + static_cast<std::int32_t>(sizeof(TArcadeMapColorHeader)) + static_cast<std::int32_t>(sizeof(std::int32_t)) + reinterpret_cast<std::uint8_t*>(ColorData)));
                    if (ColorValue == 0) {
                        Selection = Index;
                        break;
                    }
                }
            } else {
                ExitLink = ab_Space::ab_SpaceLink_FindExit(ab_Space::CurrentArcadeSpace, Selection - 1);
                if (ExitLink == nullptr) {
                    Selection = -1;
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, ColorCount - 1); cpp_range_2.next(Index); ) {
                        ColorValue = pas::load_unaligned<std::int32_t>(reinterpret_cast<WindowsSdk::PInteger>(Index * static_cast<std::int32_t>(sizeof(TArcadeMapColorVariant)) + static_cast<std::int32_t>(sizeof(TArcadeMapColorHeader)) + static_cast<std::int32_t>(sizeof(std::int32_t)) + reinterpret_cast<std::uint8_t*>(ColorData)));
                        if (ColorValue == 0) {
                            Selection = Index;
                            break;
                        }
                    }
                } else {
                    DesiredColor = 0;
                    switch (ExitLink->Last->AppearanceIndex) {
                        case 0: DesiredColor = 1; break;
                        case 1: DesiredColor = 2; break;
                        case 2: DesiredColor = 21; break;
                        case 3: DesiredColor = 22; break;
                        case 4: DesiredColor = 31; break;
                        case 5: DesiredColor = 32; break;
                        default: GR_Main::RaiseWideMessage(u"AB color"_wref.get()); break;
                    }
                    Selection = 0;
                    BestDifference = 999999;
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, ColorCount - 1); cpp_range_3.next(Index); ) {
                        ColorValue = pas::load_unaligned<std::int32_t>(reinterpret_cast<WindowsSdk::PInteger>(Index * static_cast<std::int32_t>(sizeof(TArcadeMapColorVariant)) + static_cast<std::int32_t>(sizeof(TArcadeMapColorHeader)) + static_cast<std::int32_t>(sizeof(std::int32_t)) + reinterpret_cast<std::uint8_t*>(ColorData)));
                        if (ColorValue < 20) {
                            ColorValue -= 10;
                        }
                        ColorValue = pas::abs(DesiredColor - ColorValue);
                        if (ColorValue < BestDifference) {
                            BestDifference = ColorValue;
                            Selection = Index;
                        }
                    }
                }
            }
            pas::store_unaligned<std::int32_t>(&ColorData->SelectedVariant, Selection);
            Remaining -= pas::load_unaligned<std::int32_t>(&ColorData->ByteSize);
            ColorData = static_cast<PArcadeMapColorHeader>(static_cast<void*>(pas::load_unaligned<std::int32_t>(&ColorData->ByteSize) + reinterpret_cast<std::uint8_t*>(ColorData)));
        }
        ab_Space::ArcadeKellerEncounter = aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->Order == aShip::soJumpHole && pas::checked_cast<aGalaxy::THole*>(aPlayer::GetPlayer()->OrderTarget)->HoleType == 4 && aGalaxy::Galaxy->KellerLeaveTurn == 0 && aKling::KellerShip != nullptr && pas::in_set<4, 5>(aGalaxy::Galaxy->KellerMissionState);
        if (aPlayer::GetPlayer() != nullptr) {
            for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(ab_Space::CurrentArcadeSpace->Objects) - 1); cpp_range_4.next(Index); ) {
                Obj = pas::list_at<ab_Object::TabObject>(ab_Space::CurrentArcadeSpace->Objects, Index);
                ab_Object::ab_Object_Add(Obj);
                if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Obj) != nullptr) {
                    if (ActiveArcadeRequest == nullptr) {
                        Ship = pas::checked_cast<ab_ShipAI::TabShipAI*>(Obj);
                        ab_Ship::PlayerArcadeShip->AddEnemy(Ship);
                        Ship->AddEnemy(ab_Ship::PlayerArcadeShip);
                        if (aGalaxy::Galaxy != nullptr) {
                            if (ab_Ship::KellerArcadeShip == nullptr && aGalaxy::Galaxy->IsArcadeBattleRoyaleEnabled()) {
                                for (auto cpp_range_5 = pas::for_to<std::int32_t>(Index + 1, pas::list_count(ab_Space::CurrentArcadeSpace->Objects) - 1); cpp_range_5.next(OtherIndex); ) {
                                    Other = pas::list_at<ab_Object::TabObject>(ab_Space::CurrentArcadeSpace->Objects, OtherIndex);
                                    if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Other) != nullptr && ab_Ship::PlayerArcadeShip != Other) {
                                        if (Ship->Visual->GraphKey != pas::checked_cast<ab_ShipAI::TabShipAI*>(Other)->Visual->GraphKey) {
                                            Ship->AddEnemy(pas::checked_cast<ab_ShipAI::TabShipAI*>(Other));
                                            pas::checked_cast<ab_ShipAI::TabShipAI*>(Other)->AddEnemy(Ship);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    HasEnemies = true;
                }
            }
            pas::list_clear(ab_Space::CurrentArcadeSpace->Objects);
        }
        ab_StopLine::ab_StopLine_BuildCollisionList();
        ab_Zone::ab_Zone_BuildAllRoutes();
        BuildGrid();
        ab_StopLine::ab_StopPoint_ClearImages();
        ab_StopLine::ab_StopLine_UpdateWorldLines();
        Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (pas::class_cast_if<ab_Ship::TabShip*>(Obj) != nullptr) {
                if (ab_Ship::PlayerArcadeShip == Obj) {
                    Zone = ab_Zone::ab_Zone_RandomKind(1);
                    if (Zone == nullptr) {
                        Zone = ab_Zone::ab_Zone_RandomKind(0);
                    }
                    pas::checked_cast<ab_Ship::TabShip*>(Obj)->State = ab_Zone::ab_Zone_RandomPosition(Zone);
                } else {
                    pas::checked_cast<ab_Ship::TabShip*>(Obj)->State = ab_Zone::ab_Zone_RandomPosition(ab_Zone::ab_Zone_RandomKind(0));
                }
                pas::checked_cast<ab_Ship::TabShip*>(Obj)->Visual->SetAlpha(0);
                pas::checked_cast<ab_Ship::TabShip*>(Obj)->AttachVisual();
                if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Obj) != nullptr) {
                    pas::checked_cast<ab_ShipAI::TabShipAI*>(Obj)->ResetIntent();
                }
            } else if (pas::class_cast_if<ab_Item::TabItem*>(Obj) != nullptr) {
                pas::checked_cast<ab_Item::TabItem*>(Obj)->State = ab_Zone::ab_Zone_RandomPosition(ab_Zone::ab_Zone_RandomKind(0));
            }
            Obj = Obj->Next;
        }
        Zone = ab_Zone::FirstZone;
        while (Zone != nullptr) {
            if (Zone->Kind == 5 || pas::in_range(Zone->Kind, 6, 8) && (HasEnemies && static_cast<std::uint8_t>(ab_Space::ArcadeKellerEncounter ^ 1))) {
                Wall = pas::construct_call<abWall::TabWall>(abWall::TabWall_Create);
                ab_Object::ab_Object_Add(Wall);
                Wall->BindZone(Zone);
                Wall->CollisionRadius = 4.0E+1;
                Wall->MaxSpeed = 0.0;
                if (Zone->Kind == 5) {
                    Wall->MaxHealth = Zone->BarrierHealth;
                    if (Wall->MaxHealth >= 1000000) {
                        Wall->CollisionRadius = 0.0;
                    }
                } else if (ab_Space::ArcadeKellerEncounter) {
                    Wall->MaxHealth = 1000000;
                    Wall->CollisionRadius = 0.0;
                } else if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
                    Wall->MaxHealth = Zone->BarrierHealth;
                } else {
                    Wall->MaxHealth = Zone->BarrierHealth;
                }
                Wall->Health = Wall->MaxHealth;
                Wall->State = ab_Global::MakeSphericalBearingState(Zone->Longitude, Zone->PolarAngle, 0.0);
                abWall::TabWall::AttachVisual();
            }
            Zone = Zone->Next;
        }
        ab_Zone::PabZoneLink ZoneLink = ab_Zone::FirstZoneLink;
        while (ZoneLink != nullptr) {
            if (ZoneLink->BarrierLinkMode == 1 && pas::in_range(ZoneLink->First->Kind, 6, 8) && pas::in_range(ZoneLink->Last->Kind, 6, 8)) {
                Wall = abWall::ab_Wall_FindZone(ZoneLink->First);
                OtherWall = abWall::ab_Wall_FindZone(ZoneLink->Last);
                if (Wall != nullptr) {
                    if (OtherWall != nullptr) {
                        if (Wall->StopPoint == nullptr) {
                            Wall->StopPoint = ab_StopLine::ab_StopPoint_Add();
                            Wall->StopPoint->Longitude = ZoneLink->First->Longitude;
                            Wall->StopPoint->PolarAngle = ZoneLink->First->PolarAngle;
                            Wall->StopPoint->Radius = ab_Global::SphereRadius;
                            ab_StopLine::ab_StopPoint_UpdatePosition(Wall->StopPoint);
                        }
                        if (OtherWall->StopPoint == nullptr) {
                            OtherWall->StopPoint = ab_StopLine::ab_StopPoint_Add();
                            OtherWall->StopPoint->Longitude = ZoneLink->Last->Longitude;
                            OtherWall->StopPoint->PolarAngle = ZoneLink->Last->PolarAngle;
                            OtherWall->StopPoint->Radius = ab_Global::SphereRadius;
                            ab_StopLine::ab_StopPoint_UpdatePosition(OtherWall->StopPoint);
                        }
                        StopLine = ab_StopLine::ab_StopLine_Add();
                        StopLine->First = Wall->StopPoint;
                        StopLine->Last = OtherWall->StopPoint;
                        StopLine->Visible = false;
                        StopLine->Collidable = true;
                    }
                }
            }
            ZoneLink = ZoneLink->Next;
        }
        ZoneLink = ab_Zone::FirstZoneLink;
        while (ZoneLink != nullptr) {
            if (ZoneLink->BarrierLinkMode == 1 && ZoneLink->First->Kind == 5 && ZoneLink->Last->Kind == 5) {
                Wall = abWall::ab_Wall_FindZone(ZoneLink->First);
                OtherWall = abWall::ab_Wall_FindZone(ZoneLink->Last);
                if (Wall != nullptr) {
                    if (OtherWall != nullptr) {
                        if (Wall->StopPoint == nullptr) {
                            Wall->StopPoint = ab_StopLine::ab_StopPoint_Add();
                            Wall->StopPoint->Longitude = ZoneLink->First->Longitude;
                            Wall->StopPoint->PolarAngle = ZoneLink->First->PolarAngle;
                            Wall->StopPoint->Radius = ab_Global::SphereRadius;
                            ab_StopLine::ab_StopPoint_UpdatePosition(Wall->StopPoint);
                        }
                        if (OtherWall->StopPoint == nullptr) {
                            OtherWall->StopPoint = ab_StopLine::ab_StopPoint_Add();
                            OtherWall->StopPoint->Longitude = ZoneLink->Last->Longitude;
                            OtherWall->StopPoint->PolarAngle = ZoneLink->Last->PolarAngle;
                            OtherWall->StopPoint->Radius = ab_Global::SphereRadius;
                            ab_StopLine::ab_StopPoint_UpdatePosition(OtherWall->StopPoint);
                        }
                        StopLine = ab_StopLine::ab_StopLine_Add();
                        StopLine->First = Wall->StopPoint;
                        StopLine->Last = OtherWall->StopPoint;
                        StopLine->Visible = false;
                        StopLine->Collidable = true;
                    }
                }
            }
            ZoneLink = ZoneLink->Next;
        }
        ab_StopLine::ab_StopLine_BuildCollisionList();
        abWall::ab_Wall_BuildBarrierImages();
        ab_StopLine::ab_StopLine_UpdateWorldLines();
        ab_Global::SphereViewState = ab_Ship::PlayerArcadeShip->State;
        ab_Global::SphereViewState.BearingDegrees = 0.0;
        ab_Global::SphereCameraDistance = static_cast<long double>(ab_Global::SphereRadius) + ab_Global::SphereFarCameraOffset;
        TransitionSpeed = 5.0E+2;
        UpdateWeaponPanel();
        pas::List* PendingLoads = pas::make_object<pas::List>();
        fLoad::QueueArcadeLoadingAssets(PendingLoads, ContentPanel);
        fLoad::LoadPendingAssets(PendingLoads);
        pas::free(PendingLoads);
        RefreshTimerTick();
    }

    void TfAB::AdvanceMapColors() {
        std::int32_t BlockBytes{};
        std::int32_t Selection{};
        std::int32_t FrameIndex{};
        std::int32_t FrameCount{};
        PArcadeMapColorHeader ColorData{};
        PArcadeMapColorSequence Frames{};
        // Variable-sized native color blocks: current color, variant count, selected
        // variant, byte size; then (sequence offset, appearance tag) pairs. Each
        // sequence stores its current frame, frame count and packed color frames.
        std::int32_t Remaining = ab_Global::ArcadeMapColorBuffer->DataSize;
        ColorData = static_cast<PArcadeMapColorHeader>(ab_Global::ArcadeMapColorBuffer->Data);
        while (Remaining > 0) {
            Selection = pas::load_unaligned<std::int32_t>(&ColorData->SelectedVariant);
            BlockBytes = pas::load_unaligned<std::int32_t>(&ColorData->ByteSize);
            if (Selection < 0) {
                ColorData->CurrentColor = 0;
            } else {
                Frames = static_cast<PArcadeMapColorSequence>(static_cast<void*>(reinterpret_cast<PArcadeMapColorVariant>(Selection * static_cast<std::int32_t>(sizeof(TArcadeMapColorVariant)) + static_cast<std::int32_t>(sizeof(TArcadeMapColorHeader)) + reinterpret_cast<std::uint8_t*>(ColorData))->SequenceOffset + reinterpret_cast<std::uint8_t*>(ColorData)));
                FrameIndex = Frames->FrameIndex;
                FrameCount = pas::load_unaligned<std::int32_t>(&Frames->FrameCount);
                ++FrameIndex;
                if (FrameIndex >= FrameCount) {
                    FrameIndex = 0;
                }
                Frames->FrameIndex = FrameIndex;
                ColorData->CurrentColor = pas::load_unaligned<std::int32_t>(reinterpret_cast<WindowsSdk::PInteger>(FrameIndex * static_cast<std::int32_t>(sizeof(std::int32_t)) + static_cast<std::int32_t>(sizeof(TArcadeMapColorSequence)) + reinterpret_cast<std::uint8_t*>(Frames)));
            }
            ColorData = static_cast<PArcadeMapColorHeader>(static_cast<void*>(reinterpret_cast<std::uint8_t*>(ColorData) + BlockBytes));
            Remaining -= BlockBytes;
        }
    }

    void TfAB::TimerTakt(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        ab_Object::TabObject* Obj{};
        ab_Object::TabObject* NextObject{};
        double BearingDegrees{};
        double Distance{};
        double Step{};
        EC_Struct::TPointF Position{};
        double TargetLongitude{};
        double TargetPolarAngle{};
        double SourceLongitude{};
        double SourcePolarAngle{};
        pas::List* Loads{};
        std::int32_t Index{};
        ab_Space::TabSpace* Space{};
        GI_MessageLoop::TObjectGI* Panel{};
        GI_Panel::TPanelGI* Owner{};
        std::int32_t Stage{};
        GI_Label::TLabelGI* LabelControl{};
        GI_Label::TLabelGI* DateLabel{};
        EC_Struct::TVector3D CameraPos{};
        EC_Struct::TVector3D TargetPos{};
        EC_Struct::TVector3D UpVector{};
        ab_Global::TMatrix4D View{};
        ab_Global::TMatrix4D Rotation{};
        ab_Global::TSphericalBearingDistance Bearing{};
        ab_Global::TSphericalBearingState State{};
        Stage = 0;
        Obj = nullptr;
        try {
            if (aGalaxy::Galaxy != nullptr) {
                if (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->GetHull()->HullPoints <= 0) {
                    if (aPlayer::GetPlayer() != nullptr) {
                        aGalaxy::Galaxy->ScoreScreenDismissed = 1;
                    }
                    while (aPlayer::GetPlayer() != nullptr) {
                        SysUtilsImports::Sleep(1u);
                    }
                    GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
                    if (UpdateTimer != nullptr) {
                        CancelCallbackTimer(UpdateTimer);
                        UpdateTimer = nullptr;
                    }
                    RequestClose(1);
                    return;
                }
            }
            Stage = 1;
            if (ab_Global::ArcadeViewMode == 0) {
                if (static_cast<std::uint8_t>(ab_Global::ArcadeAutopilotEnabled ^ 1) && static_cast<std::uint8_t>(ab_Global::ArcadeEnemiesDefeated ^ 1) && static_cast<std::uint8_t>(GlobalsV::DisableAutoPilot ^ 1) && (ab_Global::ArcadeTickCount - ab_Global::ArcadeLastInputTick) * 20 > GlobalsV::ChangeAutoPilot * 1000) {
                    ab_Global::ArcadeAutopilotEnabled = true;
                    UpdateAutopilotButtons();
                }
                TfAB::AdvanceMapColors();
                ab_Item::ab_Item_Update();
            }
            Stage = 2;
            if (aPlayer::GetPlayer() != nullptr && (ab_Ship::PlayerArcadeShip == nullptr || ab_Ship::PlayerArcadeShip->Health <= 0)) {
                Stage = 3;
                if (DefeatCountdownTicks <= 0) {
                    Globals::ScoreScreen->RecordPlayerResult(false);
                    pas::free(aPlayer::GetPlayer());
                    GlobalsV::GameEndReason = 3;
                    GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
                    aScript::ClearPendingScriptRequests();
                    ActiveArcadeRequest = nullptr;
                    ActiveArcadeRequestShips = nullptr;
                    aPlayer::ArcadeKellerDefeats = 0;
                    if (aPlayer::ArcadeKellerReward != nullptr) {
                        pas::free(aPlayer::ArcadeKellerReward);
                        aPlayer::ArcadeKellerReward = nullptr;
                    }
                    if (UpdateTimer != nullptr) {
                        CancelCallbackTimer(UpdateTimer);
                        UpdateTimer = nullptr;
                    }
                    RequestClose(1);
                    return;
                }
                --DefeatCountdownTicks;
                if (VictoryPanel->Active) {
                    CloseVictory(nullptr, 0u);
                }
            } else if (aGalaxy::Galaxy == nullptr && aPlayer::GetPlayer() == nullptr && (ab_Ship::PlayerArcadeShip == nullptr || ab_Ship::PlayerArcadeShip->Health <= 0)) {
                Stage = 4;
                if (DefeatCountdownTicks <= 0 && static_cast<std::uint8_t>(DefeatPanel->Active ^ 1)) {
                    Owner = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"LoseKeyPress"_wref.get()));
                    Owner->FreeOwnedChildren();
                    LabelControl = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                    LabelControl->SetFontName(GlobalsV::NormalFontName);
                    LabelControl->SetPosition(ClassesImports::Point(0, GR_Main::GiScalePixels(30)));
                    LabelControl->SetSize(ClassesImports::Point(Owner->ClientSize.X, 1));
                    LabelControl->SetTextAlignX(GI_Main::taxCenter);
                    LabelControl->SetTextAlignY(GI_Main::tayAuto);
                    LabelControl->SetText(aConst::LocalizedColorText(u"FormAB.TextExit"_wref.get()));
                    LabelControl->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
                    Owner->SetSize(ClassesImports::Point(Owner->ClientSize.X, GR_Main::GiScalePixels(30) + Owner->ClientSize.Y + LabelControl->ClientSize.Y));
                    Panel = DefeatPanel;
                    Panel->SetSize(ClassesImports::Point(Panel->ClientSize.X, Owner->LocalPosition.Y + Owner->ClientSize.Y + GR_Main::GiScalePixels(20)));
                    Panel->SetActive(true);
                    Panel = GetByName(u"LoseShr"_wref.get());
                    Panel->SetSize(ClassesImports::Point(Panel->ClientSize.X, Owner->LocalPosition.Y + Owner->ClientSize.Y + GR_Main::GiScalePixels(20)));
                    GetByName(u"PanelLoseHide"_wref.get())->SetActive(true);
                }
                if (DefeatCountdownTicks <= -500) {
                    Globals::ScreenLoadMode = 4;
                    GlobalsV::PostLoadScreenId = GlobalsV::screenLoadArcade;
                    GlobalsV::RequestedScreenId = GlobalsV::screenLoad;
                    if (UpdateTimer != nullptr) {
                        CancelCallbackTimer(UpdateTimer);
                        UpdateTimer = nullptr;
                    }
                    RequestClose(1);
                    return;
                }
                --DefeatCountdownTicks;
            } else if (ab_Space::ArcadeKellerEncounter) {
                Stage = 5;
                if (static_cast<std::uint8_t>(ab_Hit::KellerSplitActive ^ 1) && ab_Hit::KellerBreakupTicks > 50 && ab_Ship::KellerArcadeShip != nullptr && ab_Hit::KellerFragments[0] != nullptr && ab_Ship::PlayerArcadeShip != nullptr && ab_Ship::PlayerArcadeShip->Health > 0) {
                    ab_Hit::KellerSplitActive = true;
                    BeginKellerDialogTransition();
                }
            }
            Stage = 6;
            if (pas::in_set<2, 2, 5, 5>(ab_Global::ArcadeViewMode)) {
                Stage = 7;
                if (ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace != nullptr && ShipPath != nullptr && ShipPath->ActiveHead != nullptr) {
                    Stage = 8;
                    if (IsCursorImageSelected(u"Scroll"_wref.get())) {
                        SetCursorByName(u"Main"_wref.get());
                    }
                    MapDrag.Active = false;
                    BearingDegrees = aMyFunction::HeadingDegreesToRadians(aMyFunction::PointBearingDegrees(EC_Struct::PointToPointF(ab_Space::CurrentArcadeSpace->MapPosition), EC_Struct::PointToPointF(ab_Space::NextArcadeSpace->MapPosition)));
                    PlayerMapPosition.X = System::Sin(BearingDegrees) * 4.0L + PlayerMapPosition.X;
                    PlayerMapPosition.Y = PlayerMapPosition.Y - System::Cos(BearingDegrees) * 4.0L;
                    if (pas::class_cast_if<SE_Ship2::TShip2SE*>(PlayerVisual) != nullptr) {
                        pas::Extended cpp_left = pas::real_divide(aMyFunction::PointDistance(PlayerMapPosition, ShipPath->ActiveHead->Position), 2.5L);
                        float cpp_arg = cpp_left + aMyFunction::RandomIntRange(0, 1) * 0.3L;
                        SE_Ship2::TShip2SE* playerVisual = reinterpret_cast<SE_Ship2::TShip2SE*>(PlayerVisual);
                        playerVisual->OffsetTailsAlongHeading(cpp_arg);
                    }
                    PlayerMapPosition = ShipPath->ActiveHead->Position;
                    PlayerVisual->SetAngle(aMyFunction::HeadingDegreesToByte(ShipPath->ActiveHead->Heading));
                    ShipPath->RemoveNode(ShipPath->ActiveHead);
                    if (ShipPath->ActiveHead == nullptr || aMyFunction::PointDistanceSquared(EC_Struct::PointToPointF(ab_Space::NextArcadeSpace->MapPosition), PlayerMapPosition) <= static_cast<long double>(ab_Global::ArcadePathStep) * ab_Global::ArcadePathStep) {
                        Stage = 9;
                        ab_Space::CurrentArcadeSpace = ab_Space::NextArcadeSpace;
                        pas::list_delete(RouteSpaces, pas::list_indexof(RouteSpaces, reinterpret_cast<void*>(ab_Space::NextArcadeSpace)));
                        ab_Space::NextArcadeSpace = nullptr;
                        Index = 0;
                        while (Index < pas::list_count(ab_Space::CurrentArcadeSpace->Objects)) {
                            if (pas::class_cast_if<ab_Ship::TabShip*>(pas::list_at<pas::Object>(ab_Space::CurrentArcadeSpace->Objects, Index)) != nullptr) {
                                break;
                            }
                            ++Index;
                        }
                        if (Index < pas::list_count(ab_Space::CurrentArcadeSpace->Objects) && ([&] {
                            pas::Extended cpp_left_2 = RandomRange(1, 100);
                            return cpp_left_2 <= ab_Space::CurrentArcadeSpace->Danger;
                        }())) {
                            SelectMusic();
                            EnterCurrentSpace();
                        } else if (pas::list_count(RouteSpaces) > 0 && ab_Ship::ArcadePaused) {
                            ab_Space::NextArcadeSpace = pas::list_at<ab_Space::TabSpace>(RouteSpaces, 0);
                        } else {
                            ab_Ship::ArcadePauseWithShift = false;
                            ab_Ship::ArcadePaused = false;
                            PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
                            PauseButton->SetActive(ab_Ship::ArcadePaused);
                            if (pas::list_count(RouteSpaces) < 1) {
                                TfAB::BuildSpaceRoute(RouteSpaces, ab_Space::CurrentArcadeSpace, ab_Space::EndArcadeSpace);
                            }
                            RebuildShipPath();
                            BuildShipPathImages();
                        }
                    }
                }
                if (ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr && ab_Space::CurrentArcadeSpace != ab_Space::EndArcadeSpace && ab_Ship::ArcadePaused) {
                    Stage = 10;
                    HideObjectInfo();
                    ClearShipPath();
                    if (pas::list_count(RouteSpaces) < 1) {
                        TfAB::BuildSpaceRoute(RouteSpaces, ab_Space::CurrentArcadeSpace, ab_Space::EndArcadeSpace);
                    }
                    RebuildShipPath();
                    ab_Space::NextArcadeSpace = pas::list_at<ab_Space::TabSpace>(RouteSpaces, 0);
                    if (ab_Ship::ArcadePauseWithShift) {
                        ab_Ship::ArcadePauseWithShift = false;
                        ab_Ship::ArcadePaused = false;
                        PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
                        PauseButton->SetActive(ab_Ship::ArcadePaused);
                    }
                }
                if (ab_Space::CurrentArcadeSpace == ab_Space::EndArcadeSpace && aPlayer::GetPlayer() != nullptr && CampaignLoadFinished && CampaignTransitionStarted && CampaignLoadProgress >= 1.0L) {
                    Stage = 11;
                    if (UpdateTimer != nullptr) {
                        CancelCallbackTimer(UpdateTimer);
                        UpdateTimer = nullptr;
                    }
                    aGalaxy::Galaxy->PrimeIntegrityChecksum(614);
                    RequestClose(1);
                    return;
                }
                if (ab_Space::CurrentArcadeSpace == ab_Space::EndArcadeSpace && static_cast<std::uint8_t>(CampaignLoadFinished ^ 1) && ab_Global::ArcadeViewMode == 2) {
                    BeginBattleExit();
                }
                if (ab_Global::ArcadeViewMode == 5 && aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(CampaignTransitionStarted ^ 1)) {
                    FinishCampaignTransition();
                }
                if (aPlayer::GetPlayer() != nullptr && CampaignTransitionStarted && static_cast<std::uint8_t>(CampaignLoadStarted ^ 1)) {
                    Stage = 12;
                    CampaignLoadStarted = true;
                    Loads = pas::make_object<pas::List>();
                    fLoad::QueueSpaceLoadingAssets(Loads, RootUiObject);
                    if (pas::list_count(Loads) > 0) {
                        CacheLoader->SetPendingLoads(Loads, true);
                        if (ab_Global::ArcadeViewMode == 5) {
                            CacheLoader->SetPriority(3);
                        }
                    } else {
                        pas::free(Loads);
                        CampaignLoadFinished = true;
                        CampaignLoadProgress = 1.0f;
                    }
                }
                if (aPlayer::GetPlayer() != nullptr) {
                    if (CampaignTransitionStarted) {
                        if (CampaignLoadStarted) {
                            if (!CampaignLoadFinished) {
                                CampaignLoadFinished = static_cast<std::uint8_t>(CacheLoader->IsRunning() ^ 1);
                                if (CampaignLoadFinished) {
                                    if (ab_Global::ArcadeViewMode != 5) {
                                        CampaignLoadProgress = 1.0f;
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (ab_Global::ArcadeViewMode == 3) {
                Stage = 13;
                Step = pas::real_max<pas::Extended>(1.0E+1L, pas::real_divide(ab_Global::SphereCameraDistance - (static_cast<long double>(ab_Global::SphereRadius) + ab_Global::SphereNearCameraOffset), 2.0E+1L));
                if (Step > TransitionSpeed) {
                    Step = pas::real_min<pas::Extended>(static_cast<pas::Extended>(Step), TransitionSpeed + 5.0E+1L);
                } else if (Step < TransitionSpeed) {
                    Step = pas::real_max<pas::Extended>(static_cast<pas::Extended>(Step), TransitionSpeed - 1.0E+1L);
                }
                TransitionSpeed = Step;
                ab_Global::SphereCameraDistance = static_cast<long double>(ab_Global::SphereCameraDistance) - Step;
                if (ab_Global::SphereCameraDistance <= static_cast<long double>(ab_Global::SphereRadius) + ab_Global::SphereNearCameraOffset) {
                    ab_Global::SphereCameraDistance = static_cast<long double>(ab_Global::SphereRadius) + ab_Global::SphereNearCameraOffset;
                    ResetBattleControls();
                }
            }
            if (ab_Global::ArcadeViewMode == 1) {
                Stage = 14;
                Step = 1.0E+3;
                if (Step > TransitionSpeed) {
                    Step = pas::real_min<pas::Extended>(static_cast<pas::Extended>(Step), TransitionSpeed + 2.0E+1L);
                } else if (Step < TransitionSpeed) {
                    Step = pas::real_max<pas::Extended>(static_cast<pas::Extended>(Step), TransitionSpeed - 5.0L);
                }
                TransitionSpeed = Step;
                ab_Global::SphereCameraDistance = static_cast<long double>(ab_Global::SphereCameraDistance) + Step;
                if (ab_Global::SphereCameraDistance >= static_cast<long double>(ab_Global::SphereRadius) + ab_Global::SphereFarCameraOffset) {
                    ab_Global::SphereCameraDistance = static_cast<long double>(ab_Global::SphereRadius) + ab_Global::SphereFarCameraOffset;
                    EnterMapView();
                }
            }
            if (ab_Global::ArcadeViewMode == 5) {
                Stage = 15;
                if (CampaignLoadStarted) {
                    if (CacheLoader->TotalLoadCount <= 0) {
                        CampaignLoadProgress = 1.0f;
                    } else {
                        CampaignLoadProgress = pas::real_min<pas::Extended>(CampaignLoadProgress + 0.004L, pas::real_divide(CacheLoader->CompletedLoadCount, CacheLoader->TotalLoadCount));
                        if (CampaignLoadProgress > 0.99L) {
                            CampaignLoadProgress = 1.0f;
                        }
                    }
                    LoadPanel->SetProgress(CampaignLoadProgress);
                }
            }
            if (ab_Global::ArcadeViewMode == 0 && static_cast<std::uint8_t>(SimulationPaused ^ 1)) {
                Stage = 16;
                ++ab_Global::ArcadeTickCount;
                if (ab_Ship::PlayerArcadeShip != nullptr && static_cast<std::uint8_t>(ab_Global::ArcadeAutopilotEnabled ^ 1)) {
                    if (ForwardKeyDown) {
                        ab_Ship::PlayerArcadeShip->StartThrust();
                    } else if (ReverseKeyDown) {
                        ab_Ship::PlayerArcadeShip->StartReverseThrust();
                    } else {
                        ab_Ship::PlayerArcadeShip->StopThrust();
                    }
                    Stage = 17;
                    if (BrakeKeyDown) {
                        ab_Ship::PlayerArcadeShip->Brake();
                    }
                    Stage = 18;
                    if (TurnLeftKeyDown) {
                        ab_Ship::PlayerArcadeShip->SetTurnInput(-1.0E+2);
                        ab_Global::SphereViewState.BearingDegrees = aMyFunction::WrapHeadingDegrees(ab_Global::SphereViewState.BearingDegrees - 0.3L);
                    } else if (TurnRightKeyDown) {
                        ab_Ship::PlayerArcadeShip->SetTurnInput(1.0E+2);
                        ab_Global::SphereViewState.BearingDegrees = aMyFunction::WrapHeadingDegrees(ab_Global::SphereViewState.BearingDegrees + 0.3L);
                    } else {
                        ab_Ship::PlayerArcadeShip->SetTurnInput(0.0);
                    }
                    Stage = 19;
                    if (PrimaryFireKeyDown) {
                        ab_Ship::PlayerArcadeShip->FirePrimary();
                    }
                    Stage = 20;
                    if (SecondaryFireKeyDown) {
                        ab_Ship::PlayerArcadeShip->FireSecondary();
                    }
                }
                Stage = 21;
                Obj = ab_Object::FirstArcadeObject;
                while (Obj != nullptr) {
                    Obj->UpdateState();
                    Obj = Obj->Next;
                }
                Stage = 22;
                Obj = ab_Object::FirstArcadeObject;
                while (Obj != nullptr) {
                    Obj->Advance();
                    if (Obj->DeletionPending) {
                        Stage = 23;
                        NextObject = Obj;
                        Obj = Obj->Next;
                        ab_Object::ab_Object_Delete(NextObject);
                        Stage = 231;
                    } else {
                        Obj = Obj->Next;
                    }
                }
                Stage = 24;
                if (aPlayer::GetPlayer() != nullptr && ab_Ship::PlayerArcadeShip != nullptr && ab_Ship::PlayerArcadeShip->Health > 0 && reinterpret_cast<ab_ShipAI::TabShipAI*>(ab_Ship::PlayerArcadeShip)->InsideCurrentZone && pas::in_range(reinterpret_cast<ab_ShipAI::TabShipAI*>(ab_Ship::PlayerArcadeShip)->CurrentZone->Kind, 2, 4)) {
                    ab_Space::NextArcadeSpace = nullptr;
                    if (aPlayer::GetPlayer()->Order == aShip::soJumpHole) {
                        BeginBattleExit();
                    } else {
                        ab_Ship::ArcadePaused = false;
                        ab_Ship::ArcadePauseWithShift = false;
                        PlayButton->SetActive(static_cast<std::uint8_t>(ab_Ship::ArcadePaused ^ 1));
                        PauseButton->SetActive(ab_Ship::ArcadePaused);
                        BeginMapTransition();
                    }
                }
            } else if (ab_Global::ArcadeViewMode == 0) {
                Stage = 25;
                Step = 0.5;
                if ((WindowsSdk::GetAsyncKeyState(WindowsSdk::VK_CONTROL) & 0x00008000) == 0x00008000) {
                    Step = Step * 1.0E+1L;
                }
                if (ForwardKeyDown) {
                    ab_Global::SphereViewState.PolarAngleDegrees = pas::real_max<pas::Extended>(0.0L, static_cast<long double>(ab_Global::SphereViewState.PolarAngleDegrees) - Step);
                }
                if (ReverseKeyDown) {
                    ab_Global::SphereViewState.PolarAngleDegrees = pas::real_min<pas::Extended>(1.8E+2L, static_cast<long double>(ab_Global::SphereViewState.PolarAngleDegrees) + Step);
                }
                if (TurnLeftKeyDown) {
                    ab_Global::SphereViewState.LongitudeDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(ab_Global::SphereViewState.LongitudeDegrees) - Step);
                }
                if (TurnRightKeyDown) {
                    ab_Global::SphereViewState.LongitudeDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(ab_Global::SphereViewState.LongitudeDegrees) + Step);
                }
            }
            if (ab_Ship::PlayerArcadeShip != nullptr && pas::in_set<0, 0, 3, 3>(ab_Global::ArcadeViewMode)) {
                Stage = 26;
                State = ab_Ship::PlayerArcadeShip->State;
                State = ab_Global::AdvanceSphericalStateOnCurrentSphere(State, ab_Global::CameraLookAheadDistance);
                Bearing = ab_Global::GetSphericalBearingAndDistance(ab_Global::SphereViewState, State);
                if (Bearing.Distance > ab_Global::CameraFollowStep) {
                    Bearing.Distance = ab_Global::CameraFollowStep;
                }
                BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(ab_Global::SphereViewState.BearingDegrees) + Bearing.BearingDeltaDegrees);
                ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&ab_Global::SphereViewState.LongitudeDegrees), pas::Var<double>(&ab_Global::SphereViewState.PolarAngleDegrees), pas::Var<double>(&BearingDegrees), ab_Global::SphereRadius, Bearing.Distance);
            } else if (ab_Global::ArcadeViewMode == 4 && ab_Ship::KellerArcadeShip != nullptr) {
                Stage = 27;
                ScreenPointToSphere(ClassesImports::Point(GR_Main::GameScreenWidth / 2, GR_Main::GameScreenHeight / 2), TargetLongitude, TargetPolarAngle);
                ScreenPointToSphere(ClassesImports::Point(GR_Main::GiScalePixels(250), GR_Main::GiScalePixels(300)), SourceLongitude, SourcePolarAngle);
                ab_Global::ComputeSphericalBearingAndDistance(pas::Var<double>(&BearingDegrees), pas::Var<double>(&Distance), SourceLongitude, SourcePolarAngle, 0.0, TargetLongitude, TargetPolarAngle, ab_Global::SphereRadius);
                if (BearingDegrees < 0.0L) {
                    BearingDegrees = 3.6E+2L + BearingDegrees;
                }
                State = ab_Ship::KellerArcadeShip->State;
                State = ab_Global::AdvanceSphericalStateAlongBearing(State, BearingDegrees, Distance);
                Bearing = ab_Global::GetSphericalBearingAndDistance(ab_Global::SphereViewState, State);
                if (Bearing.Distance > ab_Global::CameraFollowStep * 2.0L) {
                    Bearing.Distance = ab_Global::CameraFollowStep * 2.0L;
                }
                BearingDegrees = aMyFunction::WrapHeadingDegrees(static_cast<long double>(ab_Global::SphereViewState.BearingDegrees) + Bearing.BearingDeltaDegrees);
                ab_Global::AdvanceSphericalBearingState(pas::Var<double>(&ab_Global::SphereViewState.LongitudeDegrees), pas::Var<double>(&ab_Global::SphereViewState.PolarAngleDegrees), pas::Var<double>(&BearingDegrees), ab_Global::SphereRadius, Bearing.Distance);
                ab_Global::SphereViewState.BearingDegrees = 0.0;
                if (Bearing.Distance < 5.0L) {
                    Stage = 28;
                    if (aKling::KellerShip == nullptr || aKling::KellerShip->ScriptShip == nullptr) {
                        GR_Main::RaiseWideMessage(u"Not found script"_wref.get());
                    }
                    aGalaxy::Galaxy->CheckIntegrityChecksum1(622);
                    if (aPlayer::ArcadeKellerReward != nullptr) {
                        fShip2::ClearPlayerHoldEntries();
                        if (pas::class_cast_if<aItem::TArtefact*>(aPlayer::ArcadeKellerReward) != nullptr) {
                            pas::list_insert(aPlayer::GetPlayer()->Artefacts, 0, reinterpret_cast<void*>(aPlayer::ArcadeKellerReward));
                        } else {
                            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(aPlayer::ArcadeKellerReward));
                        }
                        aPlayer::ArcadeKellerReward = nullptr;
                    }
                    Stage = 29;
                    Globals::ScriptDialogIndex = -1;
                    reinterpret_cast<aScript::TScriptShip*>(aKling::KellerShip->ScriptShip)->Script->PublishShipContext(pas::checked_cast<aScript::TScriptShip*>(aKling::KellerShip->ScriptShip));
                    aScript::CurrentScript->CallDialogByVariable(reinterpret_cast<aScript::TScriptShip*>(aKling::KellerShip->ScriptShip)->State->AuxiliaryText);
                    if (Globals::ScriptDialogIndex < 0) {
                        GR_Main::RaiseWideMessage(u"Not found dialog"_wref.get());
                    }
                    Globals::TalkShip = aKling::KellerShip;
                    Globals::TalkPlanet = nullptr;
                    Globals::TalkScripted = true;
                    SetCursorActive(false);
                    Present();
                    GR_Main::CaptureScreenBackground(true, 0);
                    SetCursorActive(true);
                    GlobalsV::TalkReturnScreenId = GlobalsV::FormToId(this);
                    fTalk::RunTalk(this);
                    ab_Global::ArcadeViewMode = 0;
                    ab_Hit::KellerDeathPending = false;
                    if (aGalaxy::Galaxy->KellerLeaveTurn != 0) {
                        ab_Hit::KellerDeathPending = true;
                    }
                    ab_Global::ArcadeEnemiesDefeated = false;
                    aGalaxy::Galaxy->PrimeIntegrityChecksum1(623);
                }
            }
            Stage = 30;
            CameraPos = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(ab_Global::SphereViewState.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(ab_Global::SphereViewState.PolarAngleDegrees), ab_Global::SphereCameraDistance);
            TargetPos = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
            UpVector = ab_Global::SphericalToVector3D(aMyFunction::HeadingDegreesToRadians(ab_Global::SphereViewState.LongitudeDegrees), aMyFunction::HeadingDegreesToRadians(ab_Global::SphereViewState.PolarAngleDegrees + 9.0E+1L), ab_Global::SphereCameraDistance);
            View = ab_Global::BuildLookAtMatrix(CameraPos, TargetPos, UpVector);
            Rotation = ab_Global::BuildZAxisRotationMatrix(aMyFunction::HeadingDegreesToRadians(ab_Global::SphereViewState.BearingDegrees));
            ab_Global::SphereViewMatrix = ab_Global::MultiplyMatrix4D(Rotation, View);
            ab_Global::SpherePerspectiveMatrix = ab_Global::BuildPerspectiveProjectionMatrix(static_cast<long double>(ab_Global::SphereCameraDistance) - ab_Global::SphereRadius - 1.0E+2L, static_cast<long double>(ab_Global::SphereCameraDistance) + ab_Global::SphereRadius + 1.0E+2L, aMyFunction::HeadingDegreesToRadians(ab_Global::SphereFieldOfView), static_cast<std::uint32_t>(GR_Main::GameScreenWidth));
            ab_Global::SphereProjectionMatrix = ab_Global::MultiplyMatrix4D(ab_Global::SpherePerspectiveMatrix, ab_Global::SphereViewMatrix);
            ab_Global::UpdateSphereProjectionMetrics();
            if (ab_Global::ArcadeViewMode == 2) {
                Stage = 31;
                if (ab_Space::NextArcadeSpace != nullptr) {
                    ab_Global::ArcadeMapViewPosition = EC_Struct::TruncatePointF(PlayerMapPosition);
                    ab_Space::HoveredArcadeSpace = nullptr;
                }
                ab_Space::ab_Space_Update();
                ab_Space::ab_Space_CreateImages();
                StartStarImage->SetPosition(EC_Struct::SubtractPoints(ab_Space::StartArcadeSpace->MapPosition, ab_Global::ArcadeMapViewPosition));
                EndStarImage->SetPosition(EC_Struct::SubtractPoints(ab_Space::EndArcadeSpace->MapPosition, ab_Global::ArcadeMapViewPosition));
                Space = ab_Space::FirstArcadeSpace;
                while (Space != nullptr) {
                    if (Space->Image != nullptr) {
                        Space->Image->SetPosition(EC_Struct::SubtractPoints(Space->MapPosition, ab_Global::ArcadeMapViewPosition));
                    }
                    Space = Space->Next;
                }
                Position = EC_Struct::PointToPointF(EC_Struct::SubtractPoints(EC_Struct::TruncatePointF(PlayerMapPosition), ab_Global::ArcadeMapViewPosition));
                if (pas::class_cast_if<SE_Ship2::TShip2SE*>(PlayerVisual) != nullptr) {
                    reinterpret_cast<SE_Ship2::TShip2SE*>(PlayerVisual)->OffsetTails(EC_Struct::MakePointF(static_cast<long double>(Position.X) - PlayerVisual->Position.X, static_cast<long double>(Position.Y) - PlayerVisual->Position.Y));
                }
                PlayerVisual->SetPosition(Position);
                if (ab_Space::NextArcadeSpace == nullptr) {
                    UpdateShipPathImages();
                }
            }
            Stage = 32;
            ab_StopLine::ab_StopLine_UpdateColors();
            ab_WorldLine::ab_WorldLine_Update();
            ab_WorldImage::ab_WorldImage_Update();
            Stage = 33;
            Obj = ab_Object::FirstArcadeObject;
            while (Obj != nullptr) {
                Obj->UpdateVisuals();
                Obj = Obj->Next;
            }
            Stage = 34;
            ab_Object::ab_Object_UpdateSounds();
            Stage = 35;
            if (ab_Global::ArcadeViewMode == 0) {
                UpdateWeaponHighlights(false);
            }
            Stage = 36;
            if (ab_Global::ArcadeViewMode == 2 && aPlayer::GetPlayer() != nullptr) {
                DateLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Turn"_wref.get()));
                CameraPos.X = ab_Space::EndArcadeSpace->MapPosition.X - ab_Space::StartArcadeSpace->MapPosition.X;
                CameraPos.Y = ab_Space::EndArcadeSpace->MapPosition.Y - ab_Space::StartArcadeSpace->MapPosition.Y;
                TargetPos.X = static_cast<long double>(PlayerMapPosition.X) - ab_Space::StartArcadeSpace->MapPosition.X;
                TargetPos.Y = static_cast<long double>(PlayerMapPosition.Y) - ab_Space::StartArcadeSpace->MapPosition.Y;
                DateLabel->SetText(aGalaxy::FormatGameTurnDate(System::Round(pas::real_divide(static_cast<long double>(CameraPos.X) * TargetPos.X + static_cast<long double>(CameraPos.Y) * TargetPos.Y, pas::sqr(static_cast<pas::Extended>(CameraPos.X)) + pas::sqr(static_cast<pas::Extended>(CameraPos.Y))) * (ArrivalTurn - DepartureTurn) + DepartureTurn)));
            }
            Stage = 37;
            if (!SimulationPaused) {
                ab_Ship::ab_Ship_RepelOverlaps();
            }
        } catch (...) {
            auto cpp_exception = pas::caught_object();
            if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Error in procedure TfAB.TimerTakt, label = ", SysUtils::IntToStr(Stage)}));
                if (Obj != nullptr) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({"Obj ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(Obj)))}));
                }
                throw;
            } else {
                throw;
            }
        }
    }

    void TfAB::ScrollMapTimer(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        WindowsSdk::TPoint Position{};
        WindowsSdk::TPoint PreviousPosition{};
        std::int16_t CursorX{};
        std::int16_t CursorY{};
        if (aGalaxy::Galaxy != nullptr && (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->GetHull()->HullPoints <= 0)) {
            if (aPlayer::GetPlayer() != nullptr) {
                aGalaxy::Galaxy->ScoreScreenDismissed = 1;
            }
            while (aPlayer::GetPlayer() != nullptr) {
                SysUtilsImports::Sleep(1u);
            }
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            RequestClose(1);
        } else if (ab_Global::ArcadeViewMode == 2 && ab_Space::NextArcadeSpace == nullptr) {
            PreviousPosition = ab_Global::ArcadeMapViewPosition;
            Position = PreviousPosition;
            if (TurnLeftKeyDown) {
                Position.X -= GlobalsV::ScrollStep;
            }
            if (TurnRightKeyDown) {
                Position.X += GlobalsV::ScrollStep;
            }
            if (ForwardKeyDown) {
                Position.Y -= GlobalsV::ScrollStep;
            }
            if (ReverseKeyDown) {
                Position.Y += GlobalsV::ScrollStep;
            }
            CursorX = GetCursorPoint().X;
            CursorY = GetCursorPoint().Y;
            if (CursorX < GlobalsV::ScrollSense) {
                Position.X -= GlobalsV::ScrollStep;
            }
            if (GR_Main::GameScreenWidth - GlobalsV::ScrollSense - 1 < CursorX) {
                Position.X += GlobalsV::ScrollStep;
            }
            if (CursorY < GlobalsV::ScrollSense) {
                Position.Y -= GlobalsV::ScrollStep;
            }
            if (GR_Main::GameScreenHeight - GlobalsV::ScrollSense - 1 < CursorY) {
                Position.Y += GlobalsV::ScrollStep;
            }
            if (PreviousPosition.X != Position.X || PreviousPosition.Y != Position.Y) {
                ab_Global::ArcadeMapViewPosition = Position;
                if (ab_Global::ArcadeMapBounds.Top - ab_Global::ArcadeMapPanMargin > ab_Global::ArcadeMapViewPosition.Y) {
                    ab_Global::ArcadeMapViewPosition.Y = ab_Global::ArcadeMapBounds.Top - ab_Global::ArcadeMapPanMargin;
                }
                if (ab_Global::ArcadeMapBounds.Bottom + ab_Global::ArcadeMapPanMargin < ab_Global::ArcadeMapViewPosition.Y) {
                    ab_Global::ArcadeMapViewPosition.Y = ab_Global::ArcadeMapBounds.Bottom + ab_Global::ArcadeMapPanMargin;
                }
                if (ab_Global::ArcadeMapBounds.Left - ab_Global::ArcadeMapPanMargin > ab_Global::ArcadeMapViewPosition.X) {
                    ab_Global::ArcadeMapViewPosition.X = ab_Global::ArcadeMapBounds.Left - ab_Global::ArcadeMapPanMargin;
                }
                if (ab_Global::ArcadeMapBounds.Right + ab_Global::ArcadeMapPanMargin < ab_Global::ArcadeMapViewPosition.X) {
                    ab_Global::ArcadeMapViewPosition.X = ab_Global::ArcadeMapBounds.Right + ab_Global::ArcadeMapPanMargin;
                }
            }
        }
    }

    void TfAB::FinishCampaignTransition() {
        aGalaxy::TStar* SavedStar{};
        if (aGalaxy::Galaxy != nullptr && (aPlayer::GetPlayer() == nullptr || aPlayer::GetPlayer()->GetHull()->HullPoints <= 0)) {
            if (UpdateTimer != nullptr) {
                CancelCallbackTimer(UpdateTimer);
                UpdateTimer = nullptr;
            }
            if (aPlayer::GetPlayer() != nullptr) {
                aGalaxy::Galaxy->ScoreScreenDismissed = 1;
            }
            while (aPlayer::GetPlayer() != nullptr) {
                SysUtilsImports::Sleep(1u);
            }
            GlobalsV::RequestedScreenId = GlobalsV::screenGameEnd;
            RequestClose(1);
        } else if (GlobalsV::RequestedScreenId != GlobalsV::screenStarMap) {
            CampaignTransitionStarted = true;
        } else if (static_cast<std::uint8_t>(aCalc::IsTurnCalculationRunningUI() ^ 1) && static_cast<std::uint8_t>(pas::is_one_of<ThreadCalc::tcpGalaxyRunning, ThreadCalc::tcpPlayerStarRunning>(aCalc::TurnCalculationPhase) ^ 1)) {
            if (aCalc::TurnCalculationPhase == ThreadCalc::tcpGalaxyFinished) {
                aCalc::QueuePlayerStarTurnCalculation();
            } else if (aPlayer::GetPlayer()->Order != aShip::soJump && aPlayer::GetPlayer()->Order != aShip::soJumpHole || aPlayer::GetPlayer()->Order == aShip::soJumpHole && aPlayer::GetPlayer()->OrderStateData == -65536) {
                Globals::StarMapScreen->SetMapCenterManually(EC_Struct::TruncatePointF(aPlayer::GetPlayer()->Position));
                Globals::StarMapScreen->ResumeMode = fStarMap::smrTurnFilm;
                CampaignTransitionStarted = true;
                aCalc::QueueGalaxyTurnCalculation();
            } else {
                aGalaxy::Galaxy->ClearJumpGates();
                SavedStar = aGalaxy::PlayerStar;
                aGalaxy::PlayerStar = aPlayer::GetPlayer()->CurrentStar;
                aGalaxy::PlayerStar->RebuildShipMovementPaths();
                SavedStar->RebuildShipMovementPaths();
                if ((static_cast<std::uint32_t>(aPlayer::GetPlayer()->OrderStateData) & 0x0000ffff) == 1) {
                    Globals::PruneExpiredPersistentPlayerMessages();
                    aScript::RunGlobalScriptsForContext(aPlayer::GetPlayer()->CurrentStar, 3);
                }
                aGalaxy::Galaxy->GenerateSpaceBackground(aPlayer::GetPlayer()->CurrentStar->BackgroundImage);
                aPlayer::GetPlayer()->InHyperspace = true;
                aCalc::QueueGalaxyTurnCalculation();
            }
        }
    }

    void TfAB::InvalidateFrame() {
        UpdateRectsEnabled = true;
        GetByName(u"UpdateObj"_wref.get())->InvalidateChildren(true);
        WorldPanel->InvalidateChildren(true);
        if (!pas::in_set<2, 2, 5, 5>(ab_Global::ArcadeViewMode)) {
            ab_Polygon::ab_Polygon_QueueUpdateRects();
        }
        if (ab_Global::ArcadeViewMode == 2) {
            ab_Space::ab_SpaceLink_Invalidate();
        }
        CursorControl->Invalidate();
        UpdateRectsEnabled = false;
    }

    void TfAB::DrawShipHealthBars() {
        GR_Main::TTriangleRasterizer16 Rasterizer{};
        ab_Ship::TabShip* Ship{};
        std::int32_t CenterY{};
        std::int32_t CenterX{};
        std::int32_t Width{};
        std::int32_t FilledWidth{};
        std::int32_t Height{};
        WindowsSdk::TPoint BottomLeft{};
        WindowsSdk::TPoint BottomRight{};
        WindowsSdk::TPoint TopRight{};
        WindowsSdk::TPoint TopLeft{};
        Rasterizer = GR_Main::TriangleRasterizer16;
        ab_Object::TabObject* Obj = ab_Object::FirstArcadeObject;
        while (Obj != nullptr) {
            if (!(pas::class_cast_if<ab_Ship::TabShip*>(Obj) != nullptr)) {
                Obj = Obj->Next;
            } else {
                Ship = reinterpret_cast<ab_Ship::TabShip*>(Obj);
                Obj = Obj->Next;
                if (Ship->Visual != nullptr && Ship->Visual->IsAttachedToSpace() && Ship->Visual->GetDepth() == ab_Global::ShipFrontDepth && Ship->StateCC && (Ship->BonusTicks[ab_Global::abkInvisibility] <= 0 || Ship->RevealTicks > 0 || ab_Ship::PlayerArcadeShip == Ship)) {
                    CenterX = System::Round(Ship->Visual->Position.X) + WorldCenterX;
                    CenterY = System::Round(Ship->Visual->Position.Y) + WorldCenterY;
                    Width = Ship->EffectOriginSpread;
                    Height = 3;
                    FilledWidth = System::Round(pas::real_divide(Ship->Health, Ship->MaxHealth) * Width);
                    TopLeft.Y = CenterY + Ship->EffectOriginSpread / 2;
                    TopRight.Y = TopLeft.Y;
                    BottomRight.Y = TopRight.Y + Height;
                    BottomLeft.Y = TopLeft.Y + Height;
                    if (FilledWidth > 0) {
                        TopLeft.X = CenterX - Width / 2;
                        TopRight.X = TopLeft.X + FilledWidth;
                        BottomRight.X = TopRight.X;
                        BottomLeft.X = TopLeft.X;
                        if (GlobalsV::HardwareRenderingEnabled) {
                            GR_DX::DrawColoredTriangle(TopLeft.X, TopLeft.Y, 0xffff0000u, TopRight.X, TopRight.Y, 0xffff0000u, BottomLeft.X, BottomLeft.Y, 0xffffffffu, true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                            GR_DX::DrawColoredTriangle(TopRight.X, TopRight.Y, 0xffff0000u, BottomRight.X, BottomRight.Y, 0xffffffffu, BottomLeft.X, BottomLeft.Y, 0xffffffffu, true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                        } else {
                            {
                                std::int32_t pitchBytes = GR_Main::ScreenRenderBuffer->PitchBytes;
                                void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                                Rasterizer(pixels, pitchBytes, TopLeft.X, TopLeft.Y, 0xffff0000u, TopRight.X, TopRight.Y, 0xffff0000u, BottomLeft.X, BottomLeft.Y, 0xffffffffu, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                            }
                            {
                                std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                void* pixels_2 = GR_Main::ScreenRenderBuffer->GetPixels();
                                Rasterizer(pixels_2, pitchBytes_2, TopRight.X, TopRight.Y, 0xffff0000u, BottomRight.X, BottomRight.Y, 0xffffffffu, BottomLeft.X, BottomLeft.Y, 0xffffffffu, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                            }
                        }
                    }
                    if (FilledWidth < Width) {
                        TopLeft.X = CenterX - Width / 2 + FilledWidth;
                        TopRight.X = TopLeft.X + Width - FilledWidth;
                        BottomRight.X = TopRight.X;
                        BottomLeft.X = TopLeft.X;
                        if (GlobalsV::HardwareRenderingEnabled) {
                            GR_DX::DrawColoredTriangle(TopLeft.X, TopLeft.Y, 0xff0000ffu, TopRight.X, TopRight.Y, 0xff0000ffu, BottomLeft.X, BottomLeft.Y, 0xffffffffu, true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                            GR_DX::DrawColoredTriangle(TopRight.X, TopRight.Y, 0xff0000ffu, BottomRight.X, BottomRight.Y, 0xffffffffu, BottomLeft.X, BottomLeft.Y, 0xffffffffu, true, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                        } else {
                            {
                                std::int32_t pitchBytes_3 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                void* pixels_3 = GR_Main::ScreenRenderBuffer->GetPixels();
                                Rasterizer(pixels_3, pitchBytes_3, TopLeft.X, TopLeft.Y, 0xff0000ffu, TopRight.X, TopRight.Y, 0xff0000ffu, BottomLeft.X, BottomLeft.Y, 0xffffffffu, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                            }
                            {
                                std::int32_t pitchBytes_4 = GR_Main::ScreenRenderBuffer->PitchBytes;
                                void* pixels_4 = GR_Main::ScreenRenderBuffer->GetPixels();
                                Rasterizer(pixels_4, pitchBytes_4, TopRight.X, TopRight.Y, 0xff0000ffu, BottomRight.X, BottomRight.Y, 0xffffffffu, BottomLeft.X, BottomLeft.Y, 0xffffffffu, reinterpret_cast<WindowsSdk::PRect>(&GR_Main::GameScreenRect));
                            }
                        }
                    }
                }
            }
        }
    }

    void TfAB::ClearEnemyStatus(std::int32_t Index) {
        if (EnemyIcons[Index] != nullptr) {
            pas::free(EnemyIcons[Index]);
            EnemyIcons[Index] = nullptr;
        }
        if (EnemyHealthRings[Index] != nullptr) {
            pas::free(EnemyHealthRings[Index]);
            EnemyHealthRings[Index] = nullptr;
        }
        if (EnemyRewardIcons[Index] != nullptr) {
            pas::free(EnemyRewardIcons[Index]);
            EnemyRewardIcons[Index] = nullptr;
        }
        if (EnemyRewardBackdrops[Index] != nullptr) {
            pas::free(EnemyRewardBackdrops[Index]);
            EnemyRewardBackdrops[Index] = nullptr;
        }
    }

    void TfAB::ClearTrackedShipStatus(std::int32_t Index) {
        if (TrackedShipIcons[Index] != nullptr) {
            pas::free(TrackedShipIcons[Index]);
            TrackedShipIcons[Index] = nullptr;
        }
        if (TrackedShipHealthRings[Index] != nullptr) {
            pas::free(TrackedShipHealthRings[Index]);
            TrackedShipHealthRings[Index] = nullptr;
        }
    }

    void TfAB::UpdateShipStatusIcons() {
        std::int32_t PosX{};
        std::int32_t PosY{};
        std::int32_t RingWidth{};
        std::int32_t RingHeight{};
        std::int32_t Index{};
        std::int32_t OffsetX{};
        std::int32_t OffsetY{};
        std::int32_t IconWidth{};
        std::int32_t IconHeight{};
        ab_ShipAI::TabShipAI* Ship{};
        aItem::TItem* Item{};
        if (ab_Ship::PlayerArcadeShip == nullptr || aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactScaner) <= 0 || ExitCode != 0 || ab_Global::ArcadeViewMode == 5) {
            for (Index = 0; Index <= 7; ++Index) {
                ClearEnemyStatus(Index);
            }
            for (Index = 0; Index <= 7; ++Index) {
                ClearTrackedShipStatus(Index);
            }
        } else {
            RingWidth = GR_Main::GiScalePixels(64);
            RingHeight = GR_Main::GiScalePixels(64);
            IconWidth = System::Round(RingWidth * 0.75L);
            IconHeight = System::Round(RingHeight * 0.75L);
            for (Index = 0; Index <= 7; ++Index) {
                if (Index >= pas::list_count(ab_Ship::PlayerArcadeShip->InitialEnemies)) {
                    ClearEnemyStatus(Index);
                } else {
                    Ship = pas::list_at<ab_ShipAI::TabShipAI>(ab_Ship::PlayerArcadeShip->InitialEnemies, Index);
                    if (Ship == nullptr || Ship->Health <= 0) {
                        ClearEnemyStatus(Index);
                    } else {
                        if (EnemyIcons[Index] == nullptr) {
                            if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Ship->Visual) != nullptr) {
                                EnemyIcons[Index] = pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, MapPanel);
                                {
                                    pas::WideString imagePath = pas::checked_cast<SE_Ship2::TShip2SE*>(Ship->Visual)->GetImagePath();
                                    GI_RotateImage5::TRotateImage5GI* cpp_arg = pas::checked_cast<GI_RotateImage5::TRotateImage5GI*>(EnemyIcons[Index]);
                                    cpp_arg->SetImage(std::move(imagePath), ClassesImports::Point(IconWidth, IconHeight), ClassesImports::Point(IconWidth, IconHeight));
                                }
                            } else if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(Ship->Visual) != nullptr) {
                                EnemyIcons[Index] = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, MapPanel, false);
                                {
                                    GI_GraphBuf::TGraphBufGI* cpp_with = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(EnemyIcons[Index]);
                                    cpp_with->SourceHasPerPixelAlpha = true;
                                    cpp_with->SetSize(ClassesImports::Point(IconWidth, IconHeight));
                                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(Ship->Visual)->StaticImagePath, 1, u","_wref.get()), cpp_with->GraphBuf);
                                    if (static_cast<std::uint32_t>(cpp_with->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with->GraphBuf->Height)) {
                                        cpp_with->GraphBuf->RescaleRgba(cpp_with->ClientSize.X, System::Round(pas::real_divide(cpp_with->ClientSize.X, static_cast<std::uint32_t>(cpp_with->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with->GraphBuf->Height)), 5);
                                    } else {
                                        cpp_with->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with->ClientSize.Y, static_cast<std::uint32_t>(cpp_with->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with->GraphBuf->Width)), cpp_with->ClientSize.Y, 5);
                                    }
                                }
                            }
                        }
                        if (EnemyHealthRings[Index] == nullptr) {
                            EnemyHealthRings[Index] = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MapPanel);
                            {
                                GI_GAI::TgaiGI* cpp_with_2 = EnemyHealthRings[Index];
                                cpp_with_2->SetImagePath(pas::concat_wide({u"Bm.ABItem.", GR_Main::GiResourceSuffix(), u"_Ring"}));
                                cpp_with_2->SetSize(ClassesImports::Point(RingWidth, RingHeight));
                                cpp_with_2->SequenceIndex = 0;
                                cpp_with_2->UpdateAutoGeometry();
                                cpp_with_2->SetMouseViewUpdates(true);
                                cpp_with_2->MouseBlocking = true;
                                cpp_with_2->StopAutoPlayback();
                                cpp_with_2->SetDepth(1.0);
                            }
                        }
                        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) <= 0) {
                            if (EnemyRewardIcons[Index] != nullptr) {
                                pas::free(EnemyRewardIcons[Index]);
                                EnemyRewardIcons[Index] = nullptr;
                                pas::free(EnemyRewardBackdrops[Index]);
                                EnemyRewardBackdrops[Index] = nullptr;
                            }
                        }
                        if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0 && EnemyRewardIcons[Index] == nullptr) {
                            EnemyRewardBackdrops[Index] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapPanel);
                            {
                                GI_Image::TImageGI* cpp_with_3 = pas::checked_cast<GI_Image::TImageGI*>(EnemyRewardBackdrops[Index]);
                                cpp_with_3->SetImagePath(pas::concat_wide({u"GI,Bm.Items.", GR_Main::GiResourceSuffix(), u"ABArtSlot"}));
                                cpp_with_3->SetSize(cpp_with_3->GetContentSize());
                                cpp_with_3->SetOrigin(EC_Struct::HalfPoint(cpp_with_3->ClientSize));
                            }
                            EnemyRewardIcons[Index] = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapPanel);
                            {
                                GI_Image::TImageGI* cpp_with_4 = pas::checked_cast<GI_Image::TImageGI*>(EnemyRewardIcons[Index]);
                                Item = pas::list_at<ab_ShipAI::TabShipAI>(ab_Ship::PlayerArcadeShip->InitialEnemies, Index)->GetRewardItem(true);
                                if (Item != nullptr && (!(pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) || static_cast<std::uint8_t>(reinterpret_cast<aItem::TEquipmentWithActCode*>(Item)->DisplayAsArtefact ^ 1))) {
                                    pas::free(Item);
                                    Item = nullptr;
                                }
                                if (Item != nullptr) {
                                    cpp_with_4->SetImagePath(pas::concat_wide({u"GI,", Item->GetBitmapResourceName(), u"ab"}));
                                    cpp_with_4->SetSize(cpp_with_4->GetContentSize());
                                    cpp_with_4->SetOrigin(EC_Struct::HalfPoint(cpp_with_4->ClientSize));
                                    pas::free(Item);
                                } else {
                                    pas::free(EnemyRewardIcons[Index]);
                                    EnemyRewardIcons[Index] = nullptr;
                                    pas::free(EnemyRewardBackdrops[Index]);
                                    EnemyRewardBackdrops[Index] = nullptr;
                                }
                            }
                        }
                    }
                }
            }
            for (Index = 0; Index <= 7; ++Index) {
                if (Index >= pas::list_count(ab_Ship::PlayerArcadeShip->TrackedShips)) {
                    ClearTrackedShipStatus(Index);
                } else {
                    Ship = pas::list_at<ab_ShipAI::TabShipAI>(ab_Ship::PlayerArcadeShip->TrackedShips, Index);
                    if (Ship == nullptr || Ship->Health <= 0) {
                        ClearTrackedShipStatus(Index);
                    } else {
                        if (TrackedShipIcons[Index] == nullptr) {
                            if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Ship->Visual) != nullptr) {
                                TrackedShipIcons[Index] = pas::construct_call<GI_RotateImage5::TRotateImage5GI>(GI_RotateImage5::TRotateImage5GI_Create, MapPanel);
                                {
                                    pas::WideString imagePath_2 = pas::checked_cast<SE_Ship2::TShip2SE*>(Ship->Visual)->GetImagePath();
                                    GI_RotateImage5::TRotateImage5GI* cpp_arg_2 = pas::checked_cast<GI_RotateImage5::TRotateImage5GI*>(TrackedShipIcons[Index]);
                                    cpp_arg_2->SetImage(std::move(imagePath_2), ClassesImports::Point(IconWidth, IconHeight), ClassesImports::Point(IconWidth, IconHeight));
                                }
                            } else if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(Ship->Visual) != nullptr) {
                                TrackedShipIcons[Index] = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, MapPanel, false);
                                {
                                    GI_GraphBuf::TGraphBufGI* cpp_with_5 = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(TrackedShipIcons[Index]);
                                    cpp_with_5->SourceHasPerPixelAlpha = true;
                                    cpp_with_5->SetSize(ClassesImports::Point(IconWidth, IconHeight));
                                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(Ship->Visual)->StaticImagePath, 1, u","_wref.get()), cpp_with_5->GraphBuf);
                                    if (static_cast<std::uint32_t>(cpp_with_5->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_5->GraphBuf->Height)) {
                                        cpp_with_5->GraphBuf->RescaleRgba(cpp_with_5->ClientSize.X, System::Round(pas::real_divide(cpp_with_5->ClientSize.X, static_cast<std::uint32_t>(cpp_with_5->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_5->GraphBuf->Height)), 5);
                                    } else {
                                        cpp_with_5->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_5->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_5->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_5->GraphBuf->Width)), cpp_with_5->ClientSize.Y, 5);
                                    }
                                }
                            }
                        }
                        if (TrackedShipHealthRings[Index] == nullptr) {
                            TrackedShipHealthRings[Index] = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MapPanel);
                            {
                                GI_GAI::TgaiGI* cpp_with_6 = TrackedShipHealthRings[Index];
                                cpp_with_6->SetImagePath(pas::concat_wide({u"Bm.ABItem.", GR_Main::GiResourceSuffix(), u"_Ring"}));
                                cpp_with_6->SetSize(ClassesImports::Point(RingWidth, RingHeight));
                                cpp_with_6->SequenceIndex = 0;
                                cpp_with_6->UpdateAutoGeometry();
                                cpp_with_6->SetMouseViewUpdates(true);
                                cpp_with_6->MouseBlocking = true;
                                cpp_with_6->StopAutoPlayback();
                                cpp_with_6->SetDepth(1.0);
                            }
                        }
                    }
                }
            }
            PosX = 5;
            PosY = 5;
            OffsetX = System::Round(pas::real_divide(RingWidth - IconWidth, 2.0L));
            OffsetY = System::Round(pas::real_divide(RingHeight - IconHeight, 2.0L));
            for (Index = 0; Index <= 7; ++Index) {
                if (Index < pas::list_count(ab_Ship::PlayerArcadeShip->InitialEnemies)) {
                    Ship = pas::list_at<ab_ShipAI::TabShipAI>(ab_Ship::PlayerArcadeShip->InitialEnemies, Index);
                    if (Ship != nullptr && EnemyIcons[Index] != nullptr && EnemyHealthRings[Index] != nullptr) {
                        if (pas::class_cast_if<GI_RotateImage5::TRotateImage5GI*>(EnemyIcons[Index]) != nullptr) {
                            EnemyIcons[Index]->SetPosition(ClassesImports::Point(PosX + IconWidth + OffsetX, PosY + IconHeight + OffsetY));
                        } else if (pas::class_cast_if<GI_GraphBuf::TGraphBufGI*>(EnemyIcons[Index]) != nullptr) {
                            EnemyIcons[Index]->SetPosition(ClassesImports::Point(PosX + OffsetX, PosY + OffsetY));
                        }
                        EnemyHealthRings[Index]->SetPosition(ClassesImports::Point(PosX, PosY));
                        if (EnemyRewardIcons[Index] != nullptr) {
                            EnemyHealthRings[Index]->SetSequenceFrame(System::Round((1.04L - pas::real_divide(Ship->Health * 0.96L, Ship->MaxHealth)) * EnemyHealthRings[Index]->SequenceFrameCount));
                        } else {
                            EnemyHealthRings[Index]->SetSequenceFrame(System::Round((1.0L - pas::real_divide(Ship->Health, Ship->MaxHealth)) * EnemyHealthRings[Index]->SequenceFrameCount));
                        }
                        if (EnemyRewardIcons[Index] != nullptr) {
                            GI_Image::TImageGI* cpp_with_7 = pas::checked_cast<GI_Image::TImageGI*>(EnemyRewardIcons[Index]);
                            if (aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) <= 0) {
                                pas::free(EnemyRewardIcons[Index]);
                                EnemyRewardIcons[Index] = nullptr;
                                pas::free(EnemyRewardBackdrops[Index]);
                                EnemyRewardBackdrops[Index] = nullptr;
                            } else {
                                cpp_with_7->SetPosition(ClassesImports::Point(PosX + GR_Main::GiScalePixels(31), PosY + GR_Main::GiScalePixels(60)));
                                Item = Ship->GetRewardItem(true);
                                if (Item != nullptr && (!(pas::class_cast_if<aItem::TEquipmentWithActCode*>(Item) != nullptr) || static_cast<std::uint8_t>(reinterpret_cast<aItem::TEquipmentWithActCode*>(Item)->DisplayAsArtefact ^ 1))) {
                                    pas::free(Item);
                                    Item = nullptr;
                                }
                                if (Item != nullptr) {
                                    cpp_with_7->SetImagePath(pas::concat_wide({u"GI,", Item->GetBitmapResourceName(), u"ab"}));
                                    cpp_with_7->SetSize(cpp_with_7->GetContentSize());
                                    cpp_with_7->SetOrigin(EC_Struct::HalfPoint(cpp_with_7->ClientSize));
                                    pas::free(Item);
                                } else {
                                    pas::free(EnemyRewardIcons[Index]);
                                    EnemyRewardIcons[Index] = nullptr;
                                    pas::free(EnemyRewardBackdrops[Index]);
                                    EnemyRewardBackdrops[Index] = nullptr;
                                }
                            }
                        }
                        if (EnemyRewardBackdrops[Index] != nullptr) {
                            pas::checked_cast<GI_Image::TImageGI*>(EnemyRewardBackdrops[Index])->SetPosition(ClassesImports::Point(PosX + GR_Main::GiScalePixels(31), PosY + GR_Main::GiScalePixels(60)));
                        }
                    }
                    PosX += RingWidth;
                }
            }
            PosX = GR_Main::GameScreenWidth - 5 - RingWidth;
            PosY = 5;
            OffsetX = System::Round(pas::real_divide(RingWidth - IconWidth, 2.0L));
            OffsetY = System::Round(pas::real_divide(RingHeight - IconHeight, 2.0L));
            for (Index = 0; Index <= 7; ++Index) {
                if (Index < pas::list_count(ab_Ship::PlayerArcadeShip->TrackedShips)) {
                    Ship = pas::list_at<ab_ShipAI::TabShipAI>(ab_Ship::PlayerArcadeShip->TrackedShips, Index);
                    if (Ship != nullptr && TrackedShipIcons[Index] != nullptr && TrackedShipHealthRings[Index] != nullptr) {
                        if (pas::class_cast_if<GI_RotateImage5::TRotateImage5GI*>(TrackedShipIcons[Index]) != nullptr) {
                            TrackedShipIcons[Index]->SetPosition(ClassesImports::Point(PosX + IconWidth + OffsetX, PosY + IconHeight + OffsetY));
                        } else if (pas::class_cast_if<GI_GraphBuf::TGraphBufGI*>(TrackedShipIcons[Index]) != nullptr) {
                            TrackedShipIcons[Index]->SetPosition(ClassesImports::Point(PosX + OffsetX, PosY + OffsetY));
                        }
                        TrackedShipHealthRings[Index]->SetPosition(ClassesImports::Point(PosX, PosY));
                        TrackedShipHealthRings[Index]->SetSequenceFrame(System::Round((1.0L - pas::real_divide(Ship->Health, Ship->MaxHealth)) * TrackedShipHealthRings[Index]->SequenceFrameCount));
                    }
                    PosX -= RingWidth;
                }
            }
        }
    }

    void TfAB::DrawFrame() {
        if (ExitCode != 0) {
            return;
        }
        ++ab_Global::ArcadeFrameCount;
        if (ab_Global::ArcadeViewMode == 0 && CargoPickupZone != nullptr && ab_Ship::PlayerArcadeShip != nullptr) {
            CargoPickupZone->Longitude = ab_Ship::PlayerArcadeShip->State.LongitudeDegrees;
            CargoPickupZone->PolarAngle = ab_Ship::PlayerArcadeShip->State.PolarAngleDegrees;
            ab_Zone::ab_Zone_UpdatePosition(CargoPickupZone);
            ab_Zone::ab_Zone_UpdateImages(CargoPickupZone);
        }
        if (!pas::in_set<2, 2, 5, 5>(ab_Global::ArcadeViewMode)) {
            ab_Polygon::ab_Polygon_SelectVisibilityCell();
            ab_Polygon::ab_Polygon_ProjectVisiblePoints();
        }
        if (ab_Global::ArcadeViewMode == 5) {
            SysUtilsImports::Sleep(10u);
        }
        InvalidateFrame();
        GI_StarField::TStarFieldGI* Background = pas::checked_cast<GI_StarField::TStarFieldGI*>(GetByName(u"StarField"_wref.get()));
        std::uint8_t PreviousSkipRestore = GlobalsV::SkipSavedPixelRestore;
        Background->UpdateBackgroundBounds();
        GlobalsV::SkipSavedPixelRestore = GlobalsV::SkipSavedPixelRestore || PreviousSkipRestore;
        if (GlobalsV::SkipSavedPixelRestore || GR_Main::FullFrameRedrawRequested) {
            UpdateRects->Clear();
            UpdateRectsEnabled = true;
            InvalidateViewport();
            UpdateRectsEnabled = false;
        }
        GR_Main::FullFrameRedrawRequested = false;
        GI_MessageLoop::TMessageLoopGI_RestoreSavedPixels16(this);
        ErasePreviousFrame();
        GR_Rect::TRectGR* Rect = UpdateRects->FirstRect;
        while (Rect != nullptr) {
            Background->DrawBackground(Rect->Bounds);
            Rect = Rect->Next;
        }
        PrepareFrameDraw();
        if (ab_Global::ArcadeViewMode == 2 || ab_Global::ArcadeViewMode == 5 && ViewModeBeforeDefeat == 2) {
            ab_Space::ab_SpaceLink_Draw();
        } else {
            ab_Polygon::ab_Polygon_Draw();
            DrawShipHealthBars();
            UpdateShipStatusIcons();
        }
        DrawQueuedControlRects();
        if (!GR_Main::BeginFramePresentation()) {
            GlobalsV::RequestedScreenId = GlobalsV::screenNone;
            GlobalsV::PostLoadScreenId = GlobalsV::FormToId(this);
            aPlayer::ArcadeKellerDefeats = 0;
            if (aPlayer::ArcadeKellerReward != nullptr) {
                pas::free(aPlayer::ArcadeKellerReward);
                aPlayer::ArcadeKellerReward = nullptr;
            }
            RequestClose(1);
        } else {
            FinishQueuedDraw();
            CommitFrameDraw();
            ResetSecondaryPixelCount();
            GR_Main::EndFramePresentation();
            InvalidateFrame();
            GlobalsV::SkipSavedPixelRestore = false;
        }
    }

    void TfAB::WorldImageCycleComplete(GI_MessageLoop::TObjectGI* Sender) {
        ab_WorldImage::PabWorldImage Entry = reinterpret_cast<ab_WorldImage::PabWorldImage>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(Sender->UserValue)));
        Entry->Finished = true;
        if (Entry->Image != nullptr) {
            Entry->Image->SetActive(false);
        }
    }

    void TfAB::AppendShipPathArc(EC_Struct::TPointF Destination) {
        aPath::PSPathNode Node{};
        EC_Struct::TPointF Position{};
        double AngleStep{};
        double Step{};
        double FromHeading{};
        double ToHeading{};
        double Difference{};
        double InitialDifference{};
        double TangentStep{};
        if (ShipPath->ActiveTail == nullptr) {
            Position = PlayerMapPosition;
            FromHeading = aMyFunction::ByteToHeadingDegrees(PlayerVisual->GetAngle());
        } else {
            Position = ShipPath->ActiveTail->Position;
            FromHeading = ShipPath->ActiveTail->Heading;
        }
        if (Position.X != Destination.X || Position.Y != Destination.Y) {
            AngleStep = ab_Global::ArcadePathStep;
            Step = ab_Global::ArcadePathArcStep;
            if (aMyFunction::PointDistanceSquared(Position, Destination) < Step) {
                ShipPath->AppendNode();
                Node = ShipPath->ActiveTail;
                pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Position);
                Node->Heading = FromHeading;
            } else {
                ToHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(Position.X) - Destination.X), static_cast<long double>(Position.Y) - Destination.Y));
                InitialDifference = aMyFunction::HeadingDifferenceDegrees(FromHeading, ToHeading);
                if (std::fabs(static_cast<pas::Extended>(InitialDifference)) >= AngleStep) {
                    TangentStep = aMyFunction::CalculateTangentArcOffset(Position, Destination, FromHeading, AngleStep);
                    if (TangentStep < Step) {
                        Step = TangentStep;
                    }
                    while (Position.X != Destination.X || Position.Y != Destination.Y) {
                        ToHeading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(Position.X) - Destination.X), static_cast<long double>(Position.Y) - Destination.Y));
                        Difference = aMyFunction::HeadingDifferenceDegrees(FromHeading, ToHeading);
                        if (std::fabs(static_cast<pas::Extended>(Difference)) <= AngleStep) {
                            break;
                        }
                        if (InitialDifference > 0.0L) {
                            FromHeading = aMyFunction::WrapHeadingDegrees(static_cast<long double>(FromHeading) + AngleStep);
                        } else {
                            FromHeading = aMyFunction::WrapHeadingDegrees(static_cast<long double>(FromHeading) - AngleStep);
                        }
                        Position.X = System::Sin(aMyFunction::HeadingDegreesToRadians(FromHeading)) * Step + Position.X;
                        Position.Y = Position.Y - System::Cos(aMyFunction::HeadingDegreesToRadians(FromHeading)) * Step;
                        if ((static_cast<long double>(Position.X) - Destination.X) * (static_cast<long double>(Position.X) - Destination.X) + (static_cast<long double>(Position.Y) - Destination.Y) * (static_cast<long double>(Position.Y) - Destination.Y) <= static_cast<long double>(Step) * Step) {
                            Position = Destination;
                        }
                        ShipPath->AppendNode();
                        Node = ShipPath->ActiveTail;
                        pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Position);
                        Node->Heading = FromHeading;
                    }
                }
            }
        }
    }

    void TfAB::AppendShipPathLine(EC_Struct::TPointF Destination) {
        std::uint8_t Vertical{};
        double Distance{};
        double Travelled{};
        double Slope{};
        double AxisScale{};
        double AxisOrigin{};
        double Heading{};
        EC_Struct::TPointF Position{};
        EC_Struct::TPointF Origin{};
        aPath::PSPathNode Node{};
        double Step{};
        if (ShipPath->ActiveTail == nullptr) {
            Origin = PlayerMapPosition;
        } else {
            Origin = ShipPath->ActiveTail->Position;
        }
        if (Origin.X != Destination.X || Origin.Y != Destination.Y) {
            Step = ab_Global::ArcadePathStep;
            Heading = aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(-(static_cast<long double>(Origin.X) - Destination.X), static_cast<long double>(Origin.Y) - Destination.Y));
            if (std::fabs(static_cast<long double>(Origin.X) - Destination.X) < std::fabs(static_cast<long double>(Origin.Y) - Destination.Y)) {
                Vertical = true;
            } else {
                Vertical = false;
            }
            Distance = System::Sqrt((static_cast<long double>(Origin.X) - Destination.X) * (static_cast<long double>(Origin.X) - Destination.X) + (static_cast<long double>(Origin.Y) - Destination.Y) * (static_cast<long double>(Origin.Y) - Destination.Y));
            if (Vertical) {
                Slope = pas::real_divide(static_cast<long double>(Destination.X) - Origin.X, static_cast<long double>(Destination.Y) - Origin.Y);
                AxisScale = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
                if (static_cast<long double>(Destination.Y) - Origin.Y < 0.0L) {
                    AxisScale = -AxisScale;
                }
                AxisOrigin = Origin.Y;
            } else {
                Slope = pas::real_divide(static_cast<long double>(Destination.Y) - Origin.Y, static_cast<long double>(Destination.X) - Origin.X);
                AxisScale = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
                if (static_cast<long double>(Destination.X) - Origin.X < 0.0L) {
                    AxisScale = -AxisScale;
                }
                AxisOrigin = Origin.X;
            }
            Travelled = Step;
            if (Travelled >= Distance) {
                ShipPath->AppendNode();
                Node = ShipPath->ActiveTail;
                pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Destination);
                Node->Heading = Heading;
            } else {
                while (Travelled < Distance) {
                    if (Vertical) {
                        Position.Y = static_cast<long double>(Travelled) * AxisScale + AxisOrigin;
                        Position.X = (static_cast<long double>(Position.Y) - Origin.Y) * Slope + Origin.X;
                    } else {
                        Position.X = static_cast<long double>(Travelled) * AxisScale + AxisOrigin;
                        Position.Y = (static_cast<long double>(Position.X) - Origin.X) * Slope + Origin.Y;
                    }
                    ShipPath->AppendNode();
                    Node = ShipPath->ActiveTail;
                    pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Position);
                    Node->Heading = Heading;
                    Travelled = static_cast<long double>(Travelled) + Step;
                }
            }
        }
    }

    void TfAB::AppendShipPath(EC_Struct::TPointF Destination) {
        AppendShipPathArc(Destination);
        AppendShipPathLine(Destination);
        float Step = ab_Global::ArcadePathStep;
        if (ShipPath->ActiveHead != nullptr && static_cast<long double>(Step) * Step > aMyFunction::PointDistanceSquared(ShipPath->ActiveTail->Position, Destination)) {
            pas::store_unaligned<EC_Struct::TPointF>(&ShipPath->ActiveTail->Position, Destination);
        }
    }

    void TfAB::BuildSpaceRoute(pas::List* Route, ab_Space::TabSpace* Origin, ab_Space::TabSpace* Destination) {
        ab_Space::TabSpace* BestSpace{};
        ab_Space::PabSpaceLink Link{};
        pas::List* Swap{};
        std::int32_t Index{};
        double BestCost{};
        pas::list_clear(Route);
        ab_Space::TabSpace* Space = ab_Space::FirstArcadeSpace;
        while (Space != nullptr) {
            Space->RouteCost = -1.0;
            Space = Space->Next;
        }
        pas::List* Pending = pas::make_object<pas::List>();
        pas::List* Following = pas::make_object<pas::List>();
        pas::list_add(Pending, reinterpret_cast<void*>(Destination));
        Destination->RouteCost = 0.0;
        while (pas::list_count(Pending) > 0) {
            pas::list_clear(Following);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Pending) - 1); cpp_range.next(Index); ) {
                Space = pas::list_at<ab_Space::TabSpace>(Pending, Index);
                Link = ab_Space::FirstArcadeSpaceLink;
                while (Link != nullptr) {
                    if (Link->Last == Space && (Link->First->RouteCost < 0.0L || static_cast<long double>(Space->RouteCost) + Link->First->Danger + 0.001L < Link->First->RouteCost)) {
                        Link->First->RouteCost = static_cast<long double>(Space->RouteCost) + Link->First->Danger + 0.001L;
                        if (pas::list_indexof(Following, reinterpret_cast<void*>(Link->First)) < 0) {
                            pas::list_add(Following, reinterpret_cast<void*>(Link->First));
                        }
                    }
                    Link = Link->Next;
                }
            }
            Swap = Pending;
            Pending = Following;
            Following = Swap;
        }
        pas::free(Following);
        pas::free(Pending);
        Space = Origin;
        while (Space != Destination) {
            BestCost = 1.0E+20;
            BestSpace = nullptr;
            Link = ab_Space::FirstArcadeSpaceLink;
            while (Link != nullptr) {
                if (Link->First == Space && Link->Last->RouteCost >= 0.0L && Link->Last->RouteCost < BestCost) {
                    BestSpace = Link->Last;
                    BestCost = Link->Last->RouteCost;
                }
                Link = Link->Next;
            }
            if (BestSpace == nullptr) {
                break;
            }
            Space = BestSpace;
            pas::list_add(Route, reinterpret_cast<void*>(Space));
        }
    }

    void TfAB::RebuildShipPath() {
        std::int32_t Index{};
        ab_Space::TabSpace* Space{};
        ShipPath->Clear();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(RouteSpaces) - 1); cpp_range.next(Index); ) {
            Space = pas::list_at<ab_Space::TabSpace>(RouteSpaces, Index);
            AppendShipPath(EC_Struct::PointToPointF(Space->MapPosition));
        }
    }

    void TfAB::BuildShipPathImages() {
        EC_Struct::TPointF PreviousPosition{};
        aPath::PSPathNode Node{};
        aPath::PSPathNode First{};
        aPath::PSPathNode Last{};
        GI_MultiImage::TMultiImageGI* Images{};
        GI_MultiImage::TMultiImageUnitGI* Item{};
        std::int32_t Index{};
        ab_Space::TabSpace* Space{};
        if (ShipPath->ActiveHead != nullptr) {
            Images = pas::checked_cast<GI_MultiImage::TMultiImageGI*>(GetByName(u"ShipPath"_wref.get()));
            if (pas::list_count(Images->Images) < 1) {
                Images->AddImage(u"Bm.PI.Path1"_w);
                Images->AddImage(u"Bm.PI.Path2"_w);
                Images->AddImage(u"Bm.PI.Path3"_w);
                Images->AddImage(u"Bm.PI.Path4"_w);
            }
            Images->ClearUnits();
            First = ShipPath->ActiveHead;
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(RouteSpaces) - 1); cpp_range.next(Index); ) {
                Space = pas::list_at<ab_Space::TabSpace>(RouteSpaces, Index);
                Last = aPath::TSPath::FindNearestFollowingNode(First, EC_Struct::PointToPointF(Space->MapPosition));
                if (Last == nullptr) {
                    Last = ShipPath->ActiveTail;
                }
                PreviousPosition = EC_Struct::MakePointF(1.0E+10f, 1.0E+10f);
                Node = Last;
                while (true) {
                    if (aMyFunction::PointDistanceSquared(PreviousPosition, Node->Position) > 225.0L) {
                        PreviousPosition = Node->Position;
                        if (ShipPath->ActiveTail != Node) {
                            Item = Images->AddUnit();
                            Item->UserData = Node;
                            Images->SetUnitPosition(Item, EC_Struct::SubtractPoints(EC_Struct::TruncatePointF(PreviousPosition), ab_Global::ArcadeMapViewPosition));
                            if (Node != Last) {
                                Item->ImageIndex = 1;
                            } else {
                                Item->ImageIndex = 2;
                            }
                        }
                    }
                    if (Node == First) {
                        break;
                    }
                    Node = Node->Prev;
                }
                First = Last->Next;
            }
            {
                GI_GAI::TgaiGI* ShipPathEnd = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"ShipPathEnd"_wref.get()));
                ShipPathEnd->SetActive(true);
                ShipPathEnd->SetOrigin(EC_Struct::HalfPoint(ShipPathEnd->GetContentSize()));
                ShipPathEnd->SetPosition(EC_Struct::SubtractPoints(EC_Struct::TruncatePointF(ShipPath->ActiveTail->Position), ab_Global::ArcadeMapViewPosition));
                ShipPathEnd->RestartPlayback();
            }
        }
    }

    void TfAB::UpdateShipPathImages() {
        aPath::PSPathNode Node{};
        GI_MultiImage::TMultiImageUnitGI* Item{};
        if (ShipPath->ActiveHead != nullptr) {
            {
                GI_MultiImage::TMultiImageGI* ShipPath = pas::checked_cast<GI_MultiImage::TMultiImageGI*>(GetByName(u"ShipPath"_wref.get()));
                Item = ShipPath->FirstUnit;
                while (Item != nullptr) {
                    Node = static_cast<aPath::PSPathNode>(Item->UserData);
                    ShipPath->SetUnitPosition(Item, EC_Struct::SubtractPoints(EC_Struct::TruncatePointF(Node->Position), ab_Global::ArcadeMapViewPosition));
                    Item = Item->Next;
                }
            }
            {
                GI_GAI::TgaiGI* ShipPathEnd = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"ShipPathEnd"_wref.get()));
                ShipPathEnd->SetActive(true);
                ShipPathEnd->SetOrigin(EC_Struct::HalfPoint(ShipPathEnd->GetContentSize()));
                ShipPathEnd->SetPosition(EC_Struct::SubtractPoints(EC_Struct::TruncatePointF(this->ShipPath->ActiveTail->Position), ab_Global::ArcadeMapViewPosition));
                ShipPathEnd->RestartPlayback();
            }
        }
    }

    void TfAB::ClearShipPath() {
        pas::checked_cast<GI_MultiImage::TMultiImageGI*>(GetByName(u"ShipPath"_wref.get()))->ClearUnits();
        pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"ShipPathEnd"_wref.get()))->SetActive(false);
    }

    void TfAB::BeginKellerDialogTransition() {
        CloseVictory(nullptr, 0u);
        ab_Global::ArcadeViewMode = 4;
    }

    void TfAB::BeginBattleExit() {
        Globals::StarMapWeaponPanelOpen = false;
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(613);
            aGalaxy::Galaxy->CheckIntegrityChecksum2(615);
            if (aPlayer::GetPlayer() != nullptr) {
                aPlayer::GetPlayer()->GetHull()->HullPoints = ab_Ship::PlayerArcadeShip->Health;
                if (!pas::is_one_of<aShip::soJump, aShip::soJumpHole>(aPlayer::GetPlayer()->Order)) {
                    aPlayer::GetPlayer()->InHyperspace = false;
                }
            }
            if (ActiveArcadeRequest != nullptr) {
                TfAB::ReportSurvivingShips();
                ActiveArcadeRequest = nullptr;
                ActiveArcadeRequestShips = nullptr;
                aScript::CompleteQueuedArcadeBattle(2);
                GlobalsV::RequestedScreenId = static_cast<GlobalsV::TGameScreenId>(aScript::ScriptArcadeReturnScreenId);
            } else {
                GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
            }
            aPlayer::ArcadeKellerDefeats = 0;
            if (aPlayer::ArcadeKellerReward != nullptr) {
                pas::free(aPlayer::ArcadeKellerReward);
                aPlayer::ArcadeKellerReward = nullptr;
            }
            aGalaxy::Galaxy->PrimeIntegrityChecksum(633);
        }
        ClearWeaponPanel();
        CloseVictory(nullptr, 0u);
        ViewModeBeforeDefeat = ab_Global::ArcadeViewMode;
        ab_Global::ArcadeViewMode = 5;
        ab_Space::NextArcadeSpace = ab_Space::EndArcadeSpace;
        ab_Space::CurrentArcadeSpace = ab_Space::EndArcadeSpace;
        if (CampaignLoadStarted && static_cast<std::uint8_t>(CampaignLoadFinished ^ 1)) {
            CacheLoader->SetPriority(3);
        }
        LoadPanel->SetProgress(0.0f);
        LoadPanel->SetShutterOpenFraction(0.0f);
        LoadPanel->Show();
        CampaignLoadProgress = 0.0f;
    }

    void TfAB::ShowSpaceInfo(ab_Space::TabSpace* Space) {
        float Distance{};
        std::int32_t Index{};
        std::int32_t InsertIndex{};
        std::int32_t RowHeight{};
        std::int32_t ShipCount{};
        pas::Object* Obj{};
        pas::WideString Text{};
        aGalaxy::TStar* Star{};
        GI_Panel::TPanelGI* Owner{};
        pas::List* Objects{};
        std::uint8_t OwnerId{};
        if (Space == nullptr) {
            HideObjectInfo();
        } else if (InfoObject != Space) {
            InfoObject = Space;
            if (aPlayer::GetPlayer() != nullptr && (ab_Space::StartArcadeSpace == Space || ab_Space::EndArcadeSpace == Space)) {
                GetByName(u"InfoStar"_wref.get())->SetActive(true);
                if (ab_Space::StartArcadeSpace == Space) {
                    Star = aPlayer::GetPlayer()->TransitOriginStar;
                } else {
                    Star = aPlayer::GetPlayer()->CurrentStar;
                }
                {
                    const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(Star->Name, u"<color=255,240,100>"_w);
                    GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStarName"_wref.get()));
                    cpp_arg->SetText(wrapTextInColor);
                }
                {
                    GI_GraphBuf::TGraphBufGI* InfoStarImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStarImage"_wref.get()));
                    InfoStarImage->SourceHasPerPixelAlpha = true;
                    GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(reinterpret_cast<SE_Star::TStarSE*>(Star->Graphic)->StaticImagePath, 1, u","_wref.get()), InfoStarImage->GraphBuf);
                    if (static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) {
                        InfoStarImage->GraphBuf->RescaleBilinearRgba(InfoStarImage->ClientSize.X, System::Round(pas::real_divide(InfoStarImage->ClientSize.X, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)));
                    } else {
                        InfoStarImage->GraphBuf->RescaleBilinearRgba(System::Round(pas::real_divide(InfoStarImage->ClientSize.Y, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)), InfoStarImage->ClientSize.Y);
                    }
                    InfoStarImage->SetImageKindX(GI_Main::ikxCenter);
                    InfoStarImage->SetImageKindY(GI_Main::ikyCenter);
                }
                Owner = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoStarPanel"_wref.get()));
                Owner->FreeOwnedChildren();
                Objects = pas::make_object<pas::List>();
                for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range.next(Index); ) {
                    pas::list_add(Objects, pas::list_get(Star->Planets, Index));
                }
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(Index); ) {
                    if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Star->Ships, Index)) != nullptr) {
                        Distance = aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(Star->Ships, Index)->Position, EC_Struct::MakePointF(0.0f, 0.0f));
                        InsertIndex = 0;
                        while (InsertIndex < pas::list_count(Objects)) {
                            if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, InsertIndex)) != nullptr) {
                                if (aMyFunction::PointDistanceSquared(pas::list_at<aPlanet::TPlanet>(Objects, InsertIndex)->GetPosition(), EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                    break;
                                }
                            } else if (aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(Objects, InsertIndex)->Position, EC_Struct::MakePointF(0.0f, 0.0f)) > Distance) {
                                break;
                            }
                            ++InsertIndex;
                        }
                        pas::list_insert(Objects, InsertIndex, pas::list_get(Star->Ships, Index));
                    }
                }
                RowHeight = GR_Main::GiScalePixels(20);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range_3.next(Index); ) {
                    {
                        GI_Label::TLabelGI* cpp_with_2 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                        cpp_with_2->SetFontName(GlobalsV::NormalFontName);
                        cpp_with_2->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
                        cpp_with_2->SetSize(ClassesImports::Point(Owner->ClientSize.X / 2 + 15, RowHeight));
                        cpp_with_2->SetPosition(ClassesImports::Point(0, RowHeight * Index));
                        cpp_with_2->SetWordWrapEnabled(false);
                        cpp_with_2->SetTextAlignX(GI_Main::taxRight);
                        cpp_with_2->SetTextAlignY(GI_Main::tayCenterEx);
                        if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, Index)) != nullptr) {
                            cpp_with_2->SetText(pas::list_at<aPlanet::TPlanet>(Objects, Index)->Name);
                        } else {
                            cpp_with_2->SetText(pas::list_at<aShip::TShip>(Objects, Index)->Name);
                        }
                    }
                    {
                        GI_GraphBuf::TGraphBufGI* cpp_with_3 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Owner, false);
                        cpp_with_3->SourceHasPerPixelAlpha = true;
                        cpp_with_3->SetPosition(ClassesImports::Point(Owner->ClientSize.X / 2 + 15 + 5 + 1, RowHeight * Index + 1));
                        cpp_with_3->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                        if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, Index)) != nullptr) {
                            pas::list_at<aPlanet::TPlanet>(Objects, Index)->Graphic->RenderToBuffer(this, cpp_with_3->GraphBuf, true);
                            cpp_with_3->GraphBuf->RescaleBilinearRgba(cpp_with_3->ClientSize.X, cpp_with_3->ClientSize.Y);
                        } else {
                            if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(pas::list_at<aShip::TShip>(Objects, Index)->Graphic) != nullptr) {
                                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::list_at<aShip::TShip>(Objects, Index)->Graphic)->StaticImagePath, 1, u","_wref.get()), cpp_with_3->GraphBuf);
                            } else {
                                GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ship2::TShip2SE*>(pas::list_at<aShip::TShip>(Objects, Index)->Graphic)->AlternateImagePath, 1, u","_wref.get()), cpp_with_3->GraphBuf);
                            }
                            if (static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Height)) {
                                cpp_with_3->GraphBuf->RescaleRgba(cpp_with_3->ClientSize.X, System::Round(pas::real_divide(cpp_with_3->ClientSize.X, static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Height)), 5);
                            } else {
                                cpp_with_3->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_3->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_3->GraphBuf->Width)), cpp_with_3->ClientSize.Y, 5);
                            }
                        }
                        cpp_with_3->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_3->SetImageKindY(GI_Main::ikyCenter);
                    }
                    if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, Index)) != nullptr) {
                        OwnerId = pas::list_at<aPlanet::TPlanet>(Objects, Index)->OwnerId;
                    } else {
                        OwnerId = pas::list_at<aShip::TShip>(Objects, Index)->OwnerId;
                    }
                    if (OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                        GI_GraphBuf::TGraphBufGI* cpp_with_4 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Owner, false);
                        cpp_with_4->SourceHasPerPixelAlpha = true;
                        if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, Index)) != nullptr) {
                            cpp_with_4->LoadBitmapPathAsRgba(pas::concat_wide({EC_Str::ExtractDelimitedPartW(aConst::GetFactionEmblemPath(pas::list_at<aPlanet::TPlanet>(Objects, Index)->GetFactionResourceName()), 1, u","_wref.get()), u"?RGBA"}));
                        } else {
                            cpp_with_4->LoadBitmapPathAsRgba(pas::concat_wide({EC_Str::ExtractDelimitedPartW(aConst::GetFactionEmblemPath(pas::list_at<aShip::TShip>(Objects, Index)->GetFactionNameKey()), 1, u","_wref.get()), u"?RGBA"}));
                        }
                        cpp_with_4->SetPosition(ClassesImports::Point(Owner->ClientSize.X / 2 + 15 + 5 + RowHeight + 5 + 1, RowHeight * Index + 1));
                        cpp_with_4->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                        if (static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) {
                            cpp_with_4->GraphBuf->RescaleRgba(cpp_with_4->ClientSize.X, System::Round(pas::real_divide(cpp_with_4->ClientSize.X, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)), 5);
                        } else {
                            cpp_with_4->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_4->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)), cpp_with_4->ClientSize.Y, 5);
                        }
                        cpp_with_4->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_4->SetImageKindY(GI_Main::ikyCenter);
                    }
                }
                Owner->SetSize(ClassesImports::Point(Owner->ClientSize.X, pas::list_count(Objects) * RowHeight));
                {
                    GI_Window::TWindowGI* InfoStar = pas::checked_cast<GI_Window::TWindowGI*>(GetByName(u"InfoStar"_wref.get()));
                    InfoStar->SetSize(ClassesImports::Point(InfoStar->ClientSize.X, InfoStar->WorkSubRect.Top + InfoStar->WorkSubRect.Bottom + pas::list_count(Objects) * RowHeight));
                    InfoStar->UpdateAutoGeometry();
                }
                pas::free(Objects);
                GetByName(u"InfoPanel"_wref.get())->SetActive(false);
            } else {
                GetByName(u"InfoStar"_wref.get())->SetActive(false);
                GetByName(u"InfoPanel"_wref.get())->SetActive(true);
                {
                    const pas::WideString& wrapTextInColor_2 = aMyFunction::WrapTextInColor(aConst::LocalizedColorText(u"FormAB.InfoName"_wref.get()), u"<color=255,240,100>"_w);
                    GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoName"_wref.get()));
                    cpp_arg_2->SetText(wrapTextInColor_2);
                }
                {
                    const pas::WideString& intToStr = pas::wide_int_to_str(Space->OutgoingCount);
                    GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoExit"_wref.get()));
                    cpp_arg_3->SetText(intToStr);
                }
                Text = Space->GetDangerText();
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoDanger"_wref.get()))->SetText(Text);
                ShipCount = 0;
                for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Space->Objects) - 1); cpp_range_4.next(Index); ) {
                    Obj = pas::list_at<pas::Object>(Space->Objects, Index);
                    if (pas::class_cast_if<ab_ShipAI::TabShipAI*>(Obj) != nullptr) {
                        ++ShipCount;
                    }
                }
                if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactAnalyzer) > 0) {
                    Text = pas::wide_int_to_str(ShipCount);
                } else {
                    Text = aConst::LocalizedColorText(u"FormAB.Unknow"_wref.get());
                }
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoPirate"_wref.get()))->SetText(Text);
                {
                    GI_GraphBuf::TGraphBufGI* InfoPlanetImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoPlanetImage"_wref.get()));
                    InfoPlanetImage->SetActive(true);
                    InfoPlanetImage->SourceHasPerPixelAlpha = true;
                    if (ab_Space::StartArcadeSpace == Space) {
                        GI_GAI::LoadGaiFrameToGraphBuf(StartStarImage->GetImagePath(), InfoPlanetImage->GraphBuf, 0u);
                    } else if (ab_Space::EndArcadeSpace == Space) {
                        GI_GAI::LoadGaiFrameToGraphBuf(EndStarImage->GetImagePath(), InfoPlanetImage->GraphBuf, 0u);
                    } else {
                        GI_GAI::LoadGaiFrameToGraphBuf(Space->Image->GetImagePath(), InfoPlanetImage->GraphBuf, 0u);
                    }
                    if (static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)) {
                        InfoPlanetImage->GraphBuf->RescaleRgba(InfoPlanetImage->ClientSize.X, System::Round(pas::real_divide(InfoPlanetImage->ClientSize.X, static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)), 5);
                    } else {
                        InfoPlanetImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoPlanetImage->ClientSize.Y, static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoPlanetImage->GraphBuf->Width)), InfoPlanetImage->ClientSize.Y, 5);
                    }
                    {
                        Types::TPoint visualCenter = InfoPlanetImage->GetVisualCenter();
                        Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                        InfoPlanetImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                    }
                }
                BattleHelpLabel->SetActive(true);
                {
                    const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"Help.ABSphere"_wref.get());
                    GI_Label::TLabelGI* battleHelpLabel = BattleHelpLabel;
                    battleHelpLabel->SetText(localizedColorText);
                }
            }
        }
    }

    void TfAB::HideObjectInfo() {
        if (InfoObject != nullptr) {
            GetByName(u"InfoPanel"_wref.get())->SetActive(false);
            GetByName(u"InfoStar"_wref.get())->SetActive(false);
            HideHelp();
        }
        InfoObject = nullptr;
    }

    void TfAB::ShowItemInfo(ab_Item::TabItem* Item) {
        aItem::TItem* Instance{};
        std::int32_t Width{};
        std::int32_t LeftWidth{};
        std::int32_t MinimumWidth{};
        if (Item == nullptr || ab_Ship::PlayerArcadeShip == nullptr) {
            CancelCargoPickup();
            return;
        }
        if (Item->BonusKind >= 0) {
            CancelCargoPickup();
            return;
        }
        if (CargoPickupItem == Item) {
            return;
        }
        CargoPickupItem = Item;
        // The native routine retains this branch after the earlier bonus rejection.
        if (Item->BonusKind >= 0) {
            if (!IsCursorImageSelected(u"Take"_wref.get())) {
                SetCursorByName(u"Take"_wref.get());
            }
        } else {
            if (aPlayer::GetPlayer() != nullptr && aPlayer::GetPlayer()->CargoFreeSpace >= Item->Item->Weight && ab_Ship::PlayerArcadeShip->DistanceTo(Item) < ab_Global::ManualCargoPickupDistance && aPlayer::GetPlayer()->CargoFreeSpace >= Item->Item->Weight && aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetCargoHook()) && aShip::TShip_CalculateCargoHookPower(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetCargoHook()) >= Item->Item->Weight) {
                if (!IsCursorImageSelected(u"Take"_wref.get())) {
                    SetCursorByName(u"Take"_wref.get());
                }
            } else if (!IsCursorImageSelected(u"Main"_wref.get())) {
                SetCursorByName(u"Main"_wref.get());
            }
            Instance = Item->Item;
            if (aGalaxy::Galaxy != nullptr && static_cast<std::uint8_t>(aGalaxy::Galaxy->Destroying ^ 1) && aPlayer::GetPlayer() != nullptr) {
                if (Instance->ScriptItem != nullptr) {
                    reinterpret_cast<aScript::TScriptItem*>(Instance->ScriptItem)->RunActionCode(aConst::satOnShowingItemInfo, nullptr, nullptr, nullptr, 0);
                }
                if (pas::class_cast_if<aItem::TEquipmentWithActCode*>(Instance) != nullptr) {
                    aScript::RunItemConfigActionCode(Instance, aConst::satOnShowingItemInfo, nullptr, nullptr, nullptr, 0);
                }
            }
            ItemInfoWindow->SetActive(true);
            {
                GI_Image::TImageGI* InfoItemImage = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoItemImage"_wref.get()));
                if (pas::class_cast_if<aItem::TGoods*>(Instance) != nullptr) {
                    InfoItemImage->SetImagePath(pas::concat_wide({u"GI,", aItem::GetItemTypeBitmapPath(Instance->ItemType)}));
                } else {
                    InfoItemImage->SetImagePath(pas::concat_wide({u"GI,", Instance->GetBitmapResourceName(), u"s"}));
                }
                InfoItemImage->SetImageKindX(GI_Main::ikxCenter);
                InfoItemImage->SetImageKindY(GI_Main::ikyCenter);
                {
                    Types::TPoint visualCenter = InfoItemImage->GetVisualCenter();
                    Types::TPoint itemImageCenter = Globals::ShipScreen->ItemImageCenter;
                    InfoItemImage->SetPosition(EC_Struct::SubtractPoints(itemImageCenter, visualCenter));
                }
            }
            if (pas::class_cast_if<aItem::TGoods*>(Instance) != nullptr) {
                {
                    const pas::WideString& wrapTextInColor = aMyFunction::WrapTextInColor(aConst::GoodsMarket[Instance->ItemType].DisplayName, aMyFunction::InfoNameColorTag);
                    GI_Label::TLabelGI* cpp_arg = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"_wref.get()));
                    cpp_arg->SetText(wrapTextInColor);
                }
                {
                    const pas::WideString& localizedText = aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"Items.Goods.Text.", SysUtils::IntToStr(Instance->ItemType + 1)})));
                    GI_Label::TLabelGI* cpp_arg_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"_wref.get()));
                    cpp_arg_2->SetText(localizedText);
                }
            } else {
                pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"_wref.get()))->SetText(u""_wref.get());
                {
                    const pas::WideString& wrapTextInColor_2 = ([&] {
                        pas::WideString displayName = Instance->GetDisplayName();
                        pas::WideString infoNameColorTag = aMyFunction::InfoNameColorTag;
                        return aMyFunction::WrapTextInColor(std::move(displayName), std::move(infoNameColorTag));
                    }());
                    GI_Label::TLabelGI* cpp_arg_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"_wref.get()));
                    cpp_arg_3->SetText(wrapTextInColor_2);
                }
                {
                    const pas::WideString& infoText = Instance->virtual_TItem_GetInfoText(u"<color=255,240,100>"_w, nullptr);
                    GI_Label::TLabelGI* cpp_arg_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"_wref.get()));
                    cpp_arg_4->SetText(infoText);
                }
            }
            {
                const pas::WideString& intToStr = pas::wide_int_to_str(Instance->Weight);
                GI_Label::TLabelGI* cpp_arg_5 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemSize"_wref.get()));
                cpp_arg_5->SetText(intToStr);
            }
            {
                const pas::WideString& intToStr_2 = pas::wide_int_to_str(Instance->Cost);
                GI_Label::TLabelGI* cpp_arg_6 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemPrice"_wref.get()));
                cpp_arg_6->SetText(intToStr_2);
            }
            {
                GI_Image::TImageGI* InfoItemEmRace = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoItemEmRace"_wref.get()));
                if (pas::class_cast_if<aItem::TGoods*>(Instance) != nullptr) {
                    InfoItemEmRace->SetImagePath(aConst::GetFactionEmblemPath(aConst::OwnerInfo[aGalaxyStruct::oiUninhabited].InternalName));
                } else {
                    InfoItemEmRace->SetImagePath(aConst::GetFactionEmblemPath(aItem::TItem_GetOwnerConfigName(Instance)));
                }
                InfoItemEmRace->SetImageKindX(GI_Main::ikxCenter);
                InfoItemEmRace->SetImageKindY(GI_Main::ikyCenter);
            }
            if (!(pas::contains(pas::constant_set<pas::Set<0, 255>>({{0, 79}}) - pas::constant_set<pas::Set<0, 255>>({{0, 7}, {9}, {23, 25}, {35, 38}, {42}, {69, 72}, {74, 79}}), static_cast<std::uint8_t>(Instance->ItemType)) || Instance->ItemType == aConst::t_Hull)) {
                {
                    GI_Image::TImageGI* InfoDurable = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"_wref.get()));
                    InfoDurable->Parent->Parent->SetActive(false);
                }
                MinimumWidth = 0;
            } else {
                if (pas::class_cast_if<aItem::THull*>(Instance) != nullptr) {
                    pas::Extended cpp_right = pas::real_max<float>(0.1f, pas::checked_cast<aItem::THull*>(Instance)->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})));
                    Width = System::Round(System::Sqrt(pas::real_divide(pas::real_divide(Instance->Weight, aConst::HullBaseSize), cpp_right)) * 64.0L);
                } else {
                    Width = System::Round(pas::real_divide(64.0L, pas::real_max<float>(0.1f, pas::checked_cast<aItem::TEquipment*>(Instance)->GetFragilityFactor(pas::constant_set<aGalaxyStruct::TDamageFlagSet>({})))));
                }
                Width = std::min<std::int32_t>(192, std::max<std::int32_t>(32, Width));
                {
                    GI_Image::TImageGI* InfoDurableLeft = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableLeft"_wref.get()));
                    LeftWidth = InfoDurableLeft->GetContentSize().X;
                    MinimumWidth = LeftWidth * 2 + Width + InfoDurableLeft->LocalPosition.X + InfoDurableLeft->Parent->LocalPosition.X + InfoDurableLeft->Parent->Parent->LocalPosition.X * 2;
                }
                {
                    GI_Image::TImageGI* InfoDurable_2 = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurable"_wref.get()));
                    InfoDurable_2->Parent->Parent->SetActive(true);
                    InfoDurable_2->Parent->Parent->SetSize(ClassesImports::Point(LeftWidth * 2 + Width, InfoDurable_2->Parent->Parent->ClientSize.Y));
                    InfoDurable_2->Parent->SetSize(ClassesImports::Point(Width + 2, InfoDurable_2->Parent->Parent->ClientSize.Y));
                    if (Instance->ItemType == aConst::t_Hull) {
                        pas::Extended cpp_left_2 = pas::checked_cast<aItem::THull*>(Instance)->HullPoints;
                        std::int64_t cpp_left = System::Round(pas::real_divide(cpp_left_2, pas::checked_cast<aItem::THull*>(Instance)->Weight) * Width);
                        std::int32_t cpp_arg_7 = cpp_left - (InfoDurable_2->GetContentSize().X - 5);
                        std::int32_t y = InfoDurable_2->LocalPosition.Y;
                        InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_7, y));
                    } else {
                        std::int64_t cpp_left_3 = System::Round(pas::real_divide(pas::checked_cast<aItem::TEquipment*>(Instance)->ConditionPercent, 1.0E+2L) * Width);
                        std::int32_t cpp_arg_8 = cpp_left_3 - (InfoDurable_2->GetContentSize().X - 5);
                        std::int32_t y_2 = InfoDurable_2->LocalPosition.Y;
                        InfoDurable_2->SetPosition(ClassesImports::Point(cpp_arg_8, y_2));
                    }
                }
                {
                    GI_Image::TImageGI* InfoDurableRight = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableRight"_wref.get()));
                    {
                        std::int32_t cpp_arg_9 = Width + LeftWidth - InfoDurableRight->GetContentSize().X;
                        std::int32_t y_3 = InfoDurableRight->LocalPosition.Y;
                        InfoDurableRight->SetPosition(ClassesImports::Point(cpp_arg_9, y_3));
                    }
                    InfoDurableRight->Parent->SetPosition(ClassesImports::Point(LeftWidth, InfoDurableRight->Parent->LocalPosition.Y));
                    InfoDurableRight->Parent->SetSize(ClassesImports::Point(Width + LeftWidth, InfoDurableRight->Parent->ClientSize.Y));
                }
                {
                    GI_Image::TImageGI* InfoDurableBack = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"InfoDurableBack"_wref.get()));
                    {
                        std::int32_t cpp_arg_10 = Width + 1 - InfoDurableBack->GetContentSize().X;
                        std::int32_t y_4 = InfoDurableBack->LocalPosition.Y;
                        InfoDurableBack->SetPosition(ClassesImports::Point(cpp_arg_10, y_4));
                    }
                    InfoDurableBack->Parent->SetSize(ClassesImports::Point(Width + LeftWidth, InfoDurableBack->Parent->ClientSize.Y));
                }
            }
            {
                GI_Label::TLabelGI* cpp_arg_11 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemText"_wref.get()));
                GI_Label::TLabelGI* cpp_arg_12 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoItemName"_wref.get()));
                fShip2::TfShip2::LayoutItemInfo(ItemInfoWindow, cpp_arg_12, cpp_arg_11, true, true, MinimumWidth);
            }
            GetByName(u"InfoItemSize"_wref.get())->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemSizeLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemSizeLabelPosition.Y));
            GetByName(u"InfoItemPrice"_wref.get())->SetPosition(ClassesImports::Point(Globals::ShipScreen->ItemPriceLabelPosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemPriceLabelPosition.Y));
            GetByName(u"InfoItemEmRace"_wref.get())->SetPosition(ClassesImports::Point(ItemInfoWindow->ClientSize.X + Globals::ShipScreen->ItemRaceImagePosition.X, ItemInfoWindow->ClientSize.Y + Globals::ShipScreen->ItemRaceImagePosition.Y));
        }
        if (CargoPickupZone == nullptr) {
            CargoPickupZone = ab_Zone::ab_Zone_Add();
            CargoPickupZone->Kind = 20;
            CargoPickupZone->Radius = ab_Global::ManualCargoPickupDistance;
            CargoPickupZone->RadiusDegrees = pas::real_divide(CargoPickupZone->Radius * 1.8E+2L, SystemImports::Pi * ab_Global::SphereRadius);
        }
        CargoPickupZone->Longitude = ab_Ship::PlayerArcadeShip->State.LongitudeDegrees;
        CargoPickupZone->PolarAngle = ab_Ship::PlayerArcadeShip->State.PolarAngleDegrees;
        ab_Zone::ab_Zone_UpdatePosition(CargoPickupZone);
        ab_Zone::ab_Zone_UpdateImages(CargoPickupZone);
    }

    void TfAB::CancelCargoPickup() {
        if (CargoPickupItem != nullptr) {
            if (!IsCursorImageSelected(u"Main"_wref.get())) {
                SetCursorByName(u"Main"_wref.get());
            }
            ItemInfoWindow->SetActive(false);
            CargoPickupItem = nullptr;
            if (CargoPickupZone != nullptr) {
                ab_Zone::ab_Zone_Delete(CargoPickupZone);
                CargoPickupZone = nullptr;
            }
        }
    }

    void TfAB::OpenShipEquipment(GI_MessageLoop::TObjectGI* Sender) {
        if (ab_Global::ArcadeViewMode != 5 && aPlayer::GetPlayer() != nullptr && ab_Ship::PlayerArcadeShip != nullptr && ab_Ship::PlayerArcadeShip->Health > 0) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(601);
            aGalaxy::Galaxy->CheckIntegrityChecksum2(602);
            ForwardKeyDown = false;
            ReverseKeyDown = false;
            BrakeKeyDown = false;
            TurnLeftKeyDown = false;
            TurnRightKeyDown = false;
            PrimaryFireKeyDown = false;
            SecondaryFireKeyDown = false;
            aPlayer::GetPlayer()->GetHull()->HullPoints = ab_Ship::PlayerArcadeShip->Health;
            SetCursorActive(false);
            Present();
            GR_Main::CaptureScreenBackground(true, 0);
            SetCursorActive(true);
            fShip2::RunShipEquipment(this);
            SyncWeaponInventory();
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(603);
            aGalaxy::Galaxy->PrimeIntegrityChecksum2(604);
            UpdateWeaponPanel();
            Present();
        }
    }

    void TfAB::SyncWeaponInventory() {
        pas::Array<aItem::TWeapon*, 0, 4> SavedWeapons{};
        pas::Array<std::int32_t, 0, 4> SavedAmmo{};
        std::int32_t SlotIndex{};
        std::int32_t SlotCount{};
        aItem::TWeapon* Item{};
        auto SaveWeaponInventory = [&]() -> void {
            std::int32_t Index{};
            for (Index = 0; Index <= 4; ++Index) {
                SavedWeapons[Index] = this->CampaignWeapons[Index];
                if (SavedWeapons[Index] != nullptr) {
                    SavedAmmo[Index] = ab_Ship::PlayerArcadeShip->Weapons[Index].Ammo;
                } else {
                    SavedAmmo[Index] = 0;
                }
            }
        };
        auto FindSavedWeaponAmmo = [&](aItem::TWeapon* Weapon) -> std::int32_t {
            std::int32_t Index{};
            std::int32_t Result = 0;
            for (Index = 0; Index <= 4; ++Index) {
                if (SavedWeapons[Index] == Weapon) {
                    Result = SavedAmmo[Index];
                    break;
                }
            }
            return Result;
        };
        if (ab_Ship::PlayerArcadeShip != nullptr) {
            SaveWeaponInventory();
            ab_Ship::PlayerArcadeShip->WeaponCount = 0;
            SlotCount = aPlayer::GetPlayer()->GetSlotCount(aConst::sskWeapon);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, SlotCount - 1); cpp_range.next(SlotIndex); ) {
                Item = pas::checked_cast<aItem::TWeapon*>(aPlayer::GetPlayer()->FindEquippedItemInSlot(aConst::t_Weapon1, SlotIndex));
                if (aShip::TShip_IsEquipmentUsable(aPlayer::GetPlayer(), Item)) {
                    if (CampaignWeapons[ab_Ship::PlayerArcadeShip->WeaponCount] != Item || ab_Ship::PlayerArcadeShip->Weapons[ab_Ship::PlayerArcadeShip->WeaponCount].SlotData != Item->AssignedSlotData) {
                        ab_W::ab_Weapon_InitializeFromInfo(&ab_Ship::PlayerArcadeShip->Weapons[ab_Ship::PlayerArcadeShip->WeaponCount], Item->GetWeaponInfo());
                        CampaignWeapons[ab_Ship::PlayerArcadeShip->WeaponCount] = Item;
                        {
                            ab_W::TabWeapon& cpp_with = ab_Ship::PlayerArcadeShip->Weapons[ab_Ship::PlayerArcadeShip->WeaponCount];
                            cpp_with.Ammo = FindSavedWeaponAmmo(Item);
                            if (cpp_with.Ammo > cpp_with.MaxAmmo) {
                                cpp_with.Ammo = 0;
                            }
                            cpp_with.SlotData = Item->AssignedSlotData;
                        }
                    }
                    ++ab_Ship::PlayerArcadeShip->WeaponCount;
                }
            }
            {
                const std::int32_t cpp_first = ab_Ship::PlayerArcadeShip->WeaponCount;
                if (cpp_first <= 4) {
                    for (SlotIndex = cpp_first; SlotIndex <= 4; ++SlotIndex) {
                        CampaignWeapons[SlotIndex] = nullptr;
                    }
                }
            }
            TfAB::NormalizeWeaponSelection();
        }
    }

    void TfAB::PickUpItem(ab_Item::TabItem* Item) {
        pas::Object* Other{};
        std::int32_t Index{};
        std::int32_t Count{};
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(616);
        }
        aItem::TItem* Instance = Item->Item;
        Instance->GetGraphObject()->DetachFromSpace();
        Instance->ReleaseGraphObject();
        Item->Item = nullptr;
        ab_Object::ab_Object_Delete(Item);
        if (pas::class_cast_if<aItem::TArtefact*>(Instance) != nullptr) {
            pas::checked_cast<aItem::TEquipment*>(Instance)->EquippedFlag = 0;
            pas::list_add(aPlayer::GetPlayer()->Artefacts, reinterpret_cast<void*>(Instance));
            if (pas::class_cast_if<aItem::TArtefactTranclucator*>(Instance) != nullptr) {
                pas::checked_cast<aTranclucator::TTranclucator*>(static_cast<pas::Object*>(pas::checked_cast<aItem::TArtefactTranclucator*>(Instance)->Ship))->OwnerShip = aPlayer::GetPlayer();
            }
        } else if (pas::class_cast_if<aItem::TCountableItem*>(Instance) != nullptr) {
            reinterpret_cast<aItem::TCountableItem*>(Instance)->DropFlag = 0;
            pas::checked_cast<aItem::TEquipment*>(Instance)->EquippedFlag = 0;
            Index = 0;
            Other = nullptr;
            Count = pas::list_count(aPlayer::GetPlayer()->Inventory);
            while (Index < Count) {
                Other = pas::list_at<pas::Object>(aPlayer::GetPlayer()->Inventory, Index);
                if (aItem::TCountableItem_CanMerge(pas::checked_cast<aItem::TCountableItem*>(Instance), Other)) {
                    break;
                }
                ++Index;
            }
            if (Index < Count && Other != nullptr) {
                aItem::TCountableItem_Merge(pas::checked_cast<aItem::TCountableItem*>(Other), Instance);
                pas::free(Instance);
            } else {
                pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Instance));
            }
        } else if (pas::class_cast_if<aItem::TEquipment*>(Instance) != nullptr) {
            pas::checked_cast<aItem::TEquipment*>(Instance)->EquippedFlag = 0;
            pas::list_add(aPlayer::GetPlayer()->Inventory, reinterpret_cast<void*>(Instance));
        } else if (pas::class_cast_if<aItem::TGoods*>(Instance) != nullptr) {
            {
                std::int32_t& cpp_target = aPlayer::GetPlayer()->CargoGoods[pas::checked_cast<aItem::TGoods*>(Instance)->ItemType].Count;
                std::int32_t cpp_step = pas::checked_cast<aItem::TGoods*>(Instance)->Quantity;
                cpp_target += cpp_step;
            }
            {
                std::int32_t& cpp_target_2 = aPlayer::GetPlayer()->CargoGoods[pas::checked_cast<aItem::TGoods*>(Instance)->ItemType].TotalCost;
                std::int32_t cpp_step_2 = pas::checked_cast<aItem::TGoods*>(Instance)->Cost;
                cpp_target_2 += cpp_step_2;
            }
            pas::free(Instance);
        }
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        if (aGalaxy::Galaxy != nullptr) {
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(616);
        }
    }

    std::int32_t TfAB::RandomRange(std::int32_t BoundA, std::int32_t BoundB) {
        RandomSeed = RandomSeed / 7981 + (RandomSeed * 7981 + 567);
        if (BoundA < BoundB) {
            return pas::imod(RandomSeed, static_cast<std::uint32_t>(BoundB - BoundA + 1)) + BoundA;
        }
        return pas::imod(RandomSeed, static_cast<std::uint32_t>(BoundA - BoundB + 1)) + BoundB;
    }

    double TfAB::RandomFloat(double BoundA, double BoundB) {
        RandomSeed = RandomSeed / 7931 + (RandomSeed * 7981 + 567);
        std::int32_t trunc = System::Trunc(BoundB * 1.0E+3L + 1.0L);
        std::int32_t trunc_2 = System::Trunc(BoundA * 1.0E+3L + 1.0L);
        return pas::real_divide(aMyFunction::SeededRandomIntRange(trunc_2, trunc, RandomSeed), 1.0E+3L);
    }

    void TfAB::UpdateHelp(GI_MessageLoop::TObjectGI* Sender, std::uint8_t Show) {
        BattleHelpLabel->SetActive(Show);
        if (Show) {
            BattleHelpLabel->SetText(Sender->HelpText);
        }
    }

    void TfAB::ControlMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        UpdateHelp(Sender, true);
    }

    void TfAB::ControlMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        HideHelp();
    }

    void TfAB::HideHelp() {
        BattleHelpLabel->SetActive(false);
    }

    void TfAB::ShowVictory() {
        std::int32_t Index{};
        GI_Label::TLabelGI* Heading{};
        GI_Label::TLabelGI* ItemLabel{};
        GetByName(u"WinItem"_wref.get())->FreeOwnedChildren();
        GI_Panel::TPanelGI* ItemsPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"WinItem"_wref.get()));
        ItemsPanel->FreeOwnedChildren();
        Index = 0;
        if (pas::list_count(ListedObjects) <= 0) {
            ItemsPanel->SetSize(ClassesImports::Point(ItemsPanel->ClientSize.X, 0));
        } else {
            ItemsPanel->SetSize(ClassesImports::Point(ItemsPanel->ClientSize.X, (pas::list_count(ListedObjects) + 1) * GR_Main::GiScalePixels(20) + 5));
            Heading = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ItemsPanel);
            Heading->SetFontName(GlobalsV::NormalFontName);
            Heading->SetPosition(ClassesImports::Point(0, 0 * GR_Main::GiScalePixels(20)));
            Heading->SetSize(ClassesImports::Point(ItemsPanel->ClientSize.X, GR_Main::GiScalePixels(20)));
            Heading->SetTextAlignX(GI_Main::taxCenter);
            Heading->SetTextAlignY(GI_Main::tayCenterEx);
            Heading->SetText(aConst::LocalizedColorText(u"FormAB.WinItems"_wref.get()));
            Heading->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(219, 218, 156));
            while (pas::list_count(ListedObjects) > Index) {
                ItemLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ItemsPanel);
                ItemLabel->SetFontName(GlobalsV::NormalFontName);
                ItemLabel->SetPosition(ClassesImports::Point(0, (Index + 1) * GR_Main::GiScalePixels(20) + 5));
                ItemLabel->SetSize(ClassesImports::Point(ItemsPanel->ClientSize.X, GR_Main::GiScalePixels(20)));
                ItemLabel->SetTextAlignX(GI_Main::taxCenter);
                ItemLabel->SetTextAlignY(GI_Main::tayCenterEx);
                ItemLabel->SetText(pas::list_at<aItem::TEquipment>(ListedObjects, Index)->GetDisplayName());
                ItemLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
                ++Index;
            }
        }
        GI_Label::TLabelGI* Footer = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, ItemsPanel);
        Footer->SetFontName(GlobalsV::NormalFontName);
        Footer->SetPosition(ClassesImports::Point(0, (Index + 1) * GR_Main::GiScalePixels(30)));
        Footer->SetSize(ClassesImports::Point(ItemsPanel->ClientSize.X, 1));
        Footer->SetTextAlignX(GI_Main::taxCenter);
        Footer->SetTextAlignY(GI_Main::tayAuto);
        Footer->SetText(aConst::LocalizedColorText(u"FormAB.TextExit"_wref.get()));
        Footer->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
        ItemsPanel->SetSize(ClassesImports::Point(ItemsPanel->ClientSize.X, GR_Main::GiScalePixels(30) + ItemsPanel->ClientSize.Y + Footer->ClientSize.Y));
        GI_MessageLoop::TObjectGI* Panel = VictoryPanel;
        Panel->SetSize(ClassesImports::Point(Panel->ClientSize.X, ItemsPanel->LocalPosition.Y + ItemsPanel->ClientSize.Y + GR_Main::GiScalePixels(20)));
        Panel->SetActive(true);
        Panel = GetByName(u"WinShr"_wref.get());
        Panel->SetSize(ClassesImports::Point(Panel->ClientSize.X, ItemsPanel->LocalPosition.Y + ItemsPanel->ClientSize.Y + GR_Main::GiScalePixels(20)));
        GetByName(u"PanelWinHide"_wref.get())->SetActive(true);
        if (aPlayer::GetPlayer() != nullptr && ab_Ship::PlayerArcadeShip != nullptr && static_cast<std::uint8_t>(ab_Ship::PlayerArcadeShip->HasFiredWeapon ^ 1)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum1(630);
            static_cast<void>(aPlayer::GetPlayer()->AchievementStats), Achievements::TAchievementStats::CheckNoShotsArcadeVictoryAchievement();
            aGalaxy::Galaxy->PrimeIntegrityChecksum1(631);
        }
    }

    void TfAB::CloseVictory(GI_MessageLoop::TObjectGI* Sender, std::uint32_t VirtualKey) {
        VictoryPanel->SetActive(false);
        GetByName(u"WinItem"_wref.get())->FreeOwnedChildren();
        DefeatPanel->SetActive(false);
        if (VictoryTimer != nullptr) {
            CancelCallbackTimer(VictoryTimer);
            VictoryTimer = nullptr;
        }
    }

    void TfAB::SelectMusic() {
        // Native arcade playback follows the hyper-space music setting.
        if (GlobalsV::MusicInHyperEnabled) {
            GR_Main::MusicManager->PlayCategory(u"ArcadeBattle"_wref.get());
        } else {
            GR_Main::MusicManager->RequestFadeOut();
        }
    }

    void TfAB::p_destroy() {
        ab_MainForm::TfAB_Destroy(this);
    }

} // namespace ab_MainForm
