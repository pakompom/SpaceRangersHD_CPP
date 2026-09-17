#include "layout/fGalaxy2.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Circle.hpp"
#include "types/GI_GraphButton.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GI_Window.hpp"
#include "types/GR_Music.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Planet.hpp"
#include "types/SE_Ruins.hpp"
#include "types/SE_Ship2.hpp"
#include "types/SE_Space.hpp"
#include "types/SE_Star.hpp"
#include "types/ThreadCalc.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "types/aGalaxyStruct.hpp"
#include "types/aItem.hpp"
#include "types/aPirate.hpp"
#include "types/aPlanet.hpp"
#include "types/aRuins.hpp"
#include "types/aTranclucator.hpp"
#include "types/aTransport.hpp"
#include "types/aWarrior.hpp"
#include "types/fStarMap.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageBox.hpp"
#include "units/GI_PolyLine.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aCalc.hpp"
#include "units/aConst.hpp"
#include "units/aGalaxy.hpp"
#include "units/aGalaxyEvent.hpp"
#include "units/aKling.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPlayer.hpp"
#include "units/aRanger.hpp"
#include "units/aScript.hpp"
#include "units/aShip.hpp"
#include "units/aVector.hpp"
#include "units/fGalaxy2.hpp"
#include "units/fPanelMain.hpp"

namespace fGalaxy2 {
    const pas::Array<std::uint8_t, 1, 7> GalaxyMapFriendlyShipOrder = pas::Array<std::uint8_t, 1, 7>{{static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(1), static_cast<std::uint8_t>(10), static_cast<std::uint8_t>(2), static_cast<std::uint8_t>(3), static_cast<std::uint8_t>(4), static_cast<std::uint8_t>(5)}};

    void CaptureGalaxyPreview(GI_MessageLoop::TMessageLoopGI* ParentLoop) {
        Globals::GalaxyScreen->ParentLoop = nullptr;
        Globals::GalaxyScreen->CapturePreviewOnOpen = true;
        Globals::GalaxyScreen->PlayTransitionSounds = false;
        Globals::GalaxyScreen->Run();
        Globals::GalaxyScreen->PlayTransitionSounds = true;
        Globals::GalaxyScreen->ParentLoop = nullptr;
        ParentLoop->InvalidateViewport();
        ParentLoop->Present();
    }

    std::uint8_t RunGalaxyMap(GI_MessageLoop::TMessageLoopGI* ParentLoop) {
        std::uint8_t Result{};
        GI_MessageLoop::TCursorStateGI State{};
        ParentLoop->RootUiObject->NativeHook50();
        ParentLoop->CaptureCursorState(&State);
        ParentLoop->SetCursorActive(false);
        ParentLoop->DrawQueuedUpdateRects();
        Globals::GalaxyScreen->ParentLoop = ParentLoop;
        ParentLoop->ChildLoop = Globals::GalaxyScreen;
        if (Globals::GalaxyScreen->Run() == 1) {
            Result = true;
        } else {
            Result = false;
        }
        Globals::GalaxyScreen->ParentLoop = nullptr;
        ParentLoop->ChildLoop = nullptr;
        ParentLoop->InvalidateViewport();
        ParentLoop->RestoreCursorState(&State);
        ParentLoop->UpdateCursorPosition();
        ParentLoop->RootUiObject->NativeHook48();
        GR_Main::PostMouseMoveMessage();
        return Result;
    }

    void TfGalaxy2_Create(TfGalaxy2* Self) {
        fPanelMain::TMessageLoopGIWithMainPanel_Create(Self);
        Self->RouteStars = pas::make_object<pas::List>();
        Self->ViewMode = 0;
    }

    void TfGalaxy2_Destroy(TfGalaxy2* Self) {
        if (Self->RouteStars != nullptr) {
            pas::free(Self->RouteStars);
            Self->RouteStars = nullptr;
        }
        fPanelMain::TMessageLoopGIWithMainPanel_Destroy(Self);
    }

    void TfGalaxy2::InitializeLayout() {
        GI_MessageLoop::TMessageLoopGI::InitializeLayout();
        MainPanel->InitializeLayout(this);
        GR_Main::AppendLogTextThreadSafe("fGalaxy2... "_a);
        ViewportRect = ClassesImports::Rect(0, 0, GR_Main::GameScreenWidth, GR_Main::GameScreenHeight);
        {
            GI_MessageLoop::TObjectGI* MainPanel = GetByName(u"MainPanel"_wref.get());
            MainPanel->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            MainPanel->FindByNameRecursive(u"BGBuf"_wref.get())->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            {
                GI_MessageLoop::TObjectGI* ButExit_Parent = MainPanel->FindByNameRecursive(u"ButExit"_wref.get())->Parent;
                ButExit_Parent->SetPosition(ClassesImports::Point(ButExit_Parent->LocalPosition.X + GR_Main::ExtraScreenWidth / 2, ButExit_Parent->LocalPosition.Y + GR_Main::ExtraScreenHeight / 2));
            }
        }
        GR_Main::AppendLogLineThreadSafe("ok"_a);
        {
            GI_MessageLoop::TObjectGI* MainPanel_2 = GetByName(u"MainPanel"_wref.get());
            MainPanel_2->KeyDownCallback = pas::bind_method<&TfGalaxy2::MainPanelKeyDown>(this);
            MainPanel_2->LeftButtonUpCallback = pas::bind_method<&TfGalaxy2::MainPanelMouseUp>(this);
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExit"_wref.get()))->UpCallback = pas::bind_method<&TfGalaxy2::CloseClicked>(this);
        MapPanel = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"Map"_wref.get()));
        HideBuffer = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"HideBuf"_wref.get()));
        JumpButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButJump"_wref.get()));
        JumpDestinationLabel = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"LabelJumpTo"_wref.get()));
        JumpAnimation = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"Anim"_wref.get()));
        JumpAnimation->StopAutoPlayback();
        JumpLightImages[0] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Light1"_wref.get()));
        JumpLightImages[1] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Light2"_wref.get()));
        JumpLightImages[2] = pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"Light3"_wref.get()));
        CreateMarkerButton = pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButCreate"_wref.get()));
        {
            GI_GraphButton::TGraphButtonGI* cpp_with_4 = CreateMarkerButton;
            cpp_with_4->UpCallback = pas::bind_method<&TfGalaxy2::CreateMarkerClicked>(this);
            CreateMarkerImagePath = cpp_with_4->ImageNormal->GetImagePath();
            CreateMarkerActiveImagePath = cpp_with_4->ImageNormalActive->GetImagePath();
        }
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButUndo"_wref.get()))->UpCallback = pas::bind_method<&TfGalaxy2::UndoMarkerClicked>(this);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButDel"_wref.get()))->UpCallback = pas::bind_method<&TfGalaxy2::ClearMarkersClicked>(this);
    }

    void TfGalaxy2::OnOpen() {
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        aVector::TPolygon2D* Polygon{};
        aGalaxy::TConstellation* Constellation{};
        aGalaxy::PMapLineSegment Segment{};
        std::uint32_t Color{};
        pas::DynArray<WindowsSdk::TPoint> Points{};
        EC_Struct::TPointF Maximum{};
        EC_Struct::PPointF Vertex{};
        WindowsSdk::TPoint First{};
        WindowsSdk::TPoint Second{};
        std::int32_t StarImageCount{};
        EC_BlockPar::TBlockParEC* Block{};
        aGalaxy::TStar* Star{};
        GI_GAI::TgaiGI* StarImage{};
        GI_GAI::TgaiGI* BattleRing{};
        GI_Label::TLabelGI* NameLabel{};
        GI_Label::TLabelGI* ForceLabel{};
        pas::WideString Text{};
        pas::WideString ColoredName{};
        aPlanet::TPlanet* Planet{};
        std::uint8_t OwnerId{};
        GI_Image::TImageGI* HoleImage{};
        WindowsSdk::TPoint BufferOffset{};
        aGalaxy::THole* Hole{};
        aGalaxy::THole* SelectedHole{};
        pas::WideString ForceText{};
        pas::WideString BossText{};
        std::int32_t CoalitionCount{};
        std::int32_t BlazerCount{};
        std::int32_t KellerCount{};
        std::int32_t TerronCount{};
        std::int32_t PirateCount{};
        std::int32_t CustomCount{};
        std::int32_t OtherCount{};
        pas::Extended Strength{};
        std::uint8_t CoalitionPresent{};
        std::uint8_t DominatorsPresent{};
        std::uint8_t PiratesPresent{};
        std::uint8_t CustomPresent{};
        std::uint8_t PlayerPartyPresent{};
        GR_GraphBuf::TGraphBufGR* CaptureBuffer{};
        pas::WideString CustomFaction{};
        pas::WideString OtherFaction{};
        GI_MessageLoop::TMessageLoopGI::OnOpen();
        if (GR_Main::AuxRenderBuffer->GetPixels() == nullptr) {
            GR_Main::CaptureScreenBackground(true, 0);
        }
        pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"BGBuf"_wref.get()))->BindExternalGraphBuf(GR_Main::AuxRenderBuffer);
        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(CapturePreviewOnOpen ^ 1)) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnEnteringForm, nullptr, nullptr, 0);
        }
        MainPanel->OnOpen();
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Ship"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Gal"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Quest"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_EndTurn"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"PM_Logo"_wref.get()))->SetHitTestDisabled(true);
        pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImagePanel"_wref.get()))->SetActive(true);
        pas::list_clear(RouteStars);
        CreateMarkerMode = false;
        CreateMarkerButton->SetImageNormalPath(CreateMarkerImagePath);
        CreateMarkerButton->SetImageNormalActivePath(CreateMarkerActiveImagePath);
        aPlayer::GetPlayer()->RefreshDerivedStats(true);
        Points = nullptr;
        HideBuffer->LoadBitmapPathAsRgba(pas::concat_wide({u"Bm.FormGalaxy2.", GR_Main::GiResourceSuffix(), u"img?RGBA"}));
        if (GR_Main::GiResourceVariant() == 2) {
            MapPixelBounds = ClassesImports::Rect(50, 50, 974, 718);
        } else {
            MapPixelBounds = ClassesImports::Rect(39, 39, 761, 561);
        }
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&MapPixelBounds, 0), ClassesImports::Point(0, 0));
        pas::store_unaligned<Types::TPoint>(pas::byte_offset(&MapPixelBounds, 8), MapPanel->ClientSize);
        BufferOffset = EC_Struct::HalfPoint(EC_Struct::SubtractPoints(ClassesImports::Point(HideBuffer->GraphBuf->Width, HideBuffer->GraphBuf->Height), MapPanel->ClientSize));
        GalaxyOrigin = EC_Struct::MakePointF(1.0E+20f, 1.0E+20f);
        Maximum = EC_Struct::MakePointF(-1.0E+20f, -1.0E+20f);
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1); cpp_range.next(I); ) {
            Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, aVector::TPolygon2D_CountChain(Constellation->OutlinePolygons) - 1); cpp_range_2.next(J); ) {
                Polygon = aVector::TPolygon2D_GetChainItem(Constellation->OutlinePolygons, J);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Polygon->Points) - 1); cpp_range_3.next(K); ) {
                    Vertex = pas::list_at<EC_Struct::TPointF>(Polygon->Points, K);
                    GalaxyOrigin.X = pas::real_min<float>(GalaxyOrigin.X, Vertex->X);
                    GalaxyOrigin.Y = pas::real_min<float>(GalaxyOrigin.Y, Vertex->Y);
                    Maximum.X = pas::real_max<float>(Maximum.X, Vertex->X);
                    Maximum.Y = pas::real_max<float>(Maximum.Y, Vertex->Y);
                }
            }
        }
        GalaxyExtent.X = static_cast<long double>(Maximum.X) - GalaxyOrigin.X;
        GalaxyExtent.Y = static_cast<long double>(Maximum.Y) - GalaxyOrigin.Y;
        StarLinks = pas::construct_call<GI_PolyLine::TPolyLineGI>(GI_PolyLine::TPolyLineGI_Create, MapPanel);
        StarLinks->SetDepth(1.0E+1);
        if (GR_Main::GameDataConfig->CountBlocks(u"StyleConstellation"_wref.get()) > 0) {
            Block = GR_Main::GameDataConfig->GetBlock(u"StyleConstellation"_wref.get());
        } else {
            Block = nullptr;
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1); cpp_range_4.next(I); ) {
            Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I);
            if (Constellation->Visible) {
                for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, aVector::TPolygon2D_CountChain(Constellation->OutlinePolygons) - 1); cpp_range_5.next(J); ) {
                    Polygon = aVector::TPolygon2D_GetChainItem(Constellation->OutlinePolygons, J);
                    Points.set_length(pas::list_count(Polygon->Points));
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, pas::list_count(Polygon->Points) - 1); cpp_range_6.next(K); ) {
                        Vertex = pas::list_at<EC_Struct::TPointF>(Polygon->Points, K);
                        Points[K] = EC_Struct::AddPoints(GalaxyPointToMapPoint(pas::load_unaligned<EC_Struct::TPointF>(Vertex)), BufferOffset);
                    }
                    HideBuffer->GraphBuf->FillPolygon32(pas::open_array(Points), 0u);
                }
            }
            if (!Constellation->Visible) {
                for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->OutlineSegments) - 1); cpp_range_7.next(J); ) {
                    Segment = pas::list_at<aGalaxy::TMapLineSegment>(Constellation->OutlineSegments, J);
                    First = EC_Struct::AddPoints(GalaxyPointToMapPoint(Segment->StartPoint), BufferOffset);
                    Second = EC_Struct::AddPoints(GalaxyPointToMapPoint(Segment->EndPoint), BufferOffset);
                    HideBuffer->GraphBuf->DrawAntialiasedLine(First, Second, 0xff008080u);
                }
            }
            if (Constellation->Visible) {
                if (Block == nullptr || Block->CountParams(u"DrawLines"_wref.get()) <= 0 || GI_Main::ParseEnabledNameGI(Block->GetParam(u"DrawLines"_wref.get()))) {
                    for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->StarLinks) - 1); cpp_range_8.next(J); ) {
                        Segment = pas::list_at<aGalaxy::TMapLineSegment>(Constellation->StarLinks, J);
                        if (Block == nullptr || Block->CountParams(u"LinesColor"_wref.get()) <= 0) {
                            Color = GR_Main::CurrentPixelFormat->PackNormalizedRgb(1.0, 1.0, 0.0);
                        } else {
                            Text = Block->GetParam(u"LinesColor"_wref.get());
                            Color = ([&] {
                                double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                                double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                                double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                                return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW, extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                            }());
                        }
                        ([&] {
                            WindowsSdk::TPoint galaxyPointToMapPoint = GalaxyPointToMapPoint(Segment->EndPoint);
                            WindowsSdk::TPoint galaxyPointToMapPoint_2 = GalaxyPointToMapPoint(Segment->StartPoint);
                            return StarLinks->AddParentLine(galaxyPointToMapPoint_2, galaxyPointToMapPoint, Color, Constellation->Id);
                        }())->Animated = true;
                    }
                }
            }
        }
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1); cpp_range_9.next(I); ) {
            Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I);
            if (Constellation->Visible) {
                for (auto cpp_range_10 = pas::for_to<std::int32_t>(0, pas::list_count(Constellation->OutlineSegments) - 1); cpp_range_10.next(J); ) {
                    Segment = pas::list_at<aGalaxy::TMapLineSegment>(Constellation->OutlineSegments, J);
                    if (aGalaxy::Galaxy->CountVisibleConstellationsWithBoundaryPoints(Segment->StartPoint, Segment->EndPoint) <= 1) {
                        WindowsSdk::TPoint addPoints = EC_Struct::AddPoints(GalaxyPointToMapPoint(Segment->EndPoint), BufferOffset);
                        WindowsSdk::TPoint addPoints_2 = EC_Struct::AddPoints(GalaxyPointToMapPoint(Segment->StartPoint), BufferOffset);
                        HideBuffer->GraphBuf->DrawAntialiasedLine(addPoints_2, addPoints, 0xffffff00u);
                    } else {
                        WindowsSdk::TPoint addPoints_3 = EC_Struct::AddPoints(GalaxyPointToMapPoint(Segment->EndPoint), BufferOffset);
                        WindowsSdk::TPoint addPoints_4 = EC_Struct::AddPoints(GalaxyPointToMapPoint(Segment->StartPoint), BufferOffset);
                        HideBuffer->GraphBuf->DrawAntialiasedLine(addPoints_4, addPoints_3, 0xff000090u);
                    }
                }
            }
        }
        for (auto cpp_range_11 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Constellations) - 1); cpp_range_11.next(I); ) {
            Constellation = pas::list_at<aGalaxy::TConstellation>(aGalaxy::Galaxy->Constellations, I);
            if (Constellation->OutlineSegments != nullptr && pas::list_count(Constellation->OutlineSegments) != 0) {
                if (!Constellation->Visible) {
                    First = GalaxyPointToMapPoint(Constellation->CalculateLabelPosition());
                    NameLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, MapPanel);
                    NameLabel->SetFontName(GlobalsV::BigFontName);
                    NameLabel->SetDepth(1.0E+2);
                    NameLabel->SetTextAlignX(GI_Main::taxCenter);
                    NameLabel->SetTextAlignY(GI_Main::tayCenter);
                    NameLabel->SetPositionModeW(false);
                    NameLabel->SetSize(ClassesImports::Point(250, 40));
                    NameLabel->SetPosition(ClassesImports::Point(First.X - 125, First.Y - 20));
                    NameLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0x000000db, 0x000000da, 0x0000009c));
                    NameLabel->SetTextBorderWidth(1);
                    NameLabel->SetTextBorderColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
                    if (GR_Main::GiResourceVariant() == 1) {
                        NameLabel->SetShadowOffset(2);
                    } else {
                        NameLabel->SetShadowOffset(3);
                    }
                    NameLabel->SetText(Constellation->GetName());
                } else {
                    First = GalaxyPointToMapPoint(Constellation->CalculateLabelPosition());
                    NameLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, MapPanel);
                    NameLabel->SetFontName(GlobalsV::BigFontName);
                    NameLabel->SetDepth(1.0E+3);
                    NameLabel->SetTextAlignX(GI_Main::taxCenter);
                    NameLabel->SetTextAlignY(GI_Main::tayCenter);
                    NameLabel->SetPositionModeW(false);
                    NameLabel->SetSize(ClassesImports::Point(200, 40));
                    NameLabel->SetPosition(ClassesImports::Point(First.X - 100, First.Y - 20));
                    NameLabel->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0x00000055, 0x00000066, 0x0000006e));
                    NameLabel->SetText(Constellation->GetName());
                }
            }
        }
        StarLinks->SetPositionModeW(false);
        StarLinks->SetActive(true);
        Block = GR_Main::GameDataConfig->GetBlock(u"GalaxyStar"_wref.get());
        StarImageCount = Block->GetParamCount();
        for (auto cpp_range_12 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range_12.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (Star->IsConstellationVisible()) {
                StarImage = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MapPanel);
                StarImage->SetImagePath(Block->GetParamValue(pas::imod(static_cast<std::int32_t>(Star->GenerationSeed), StarImageCount)));
                StarImage->SetSize(StarImage->GetContentSize());
                StarImage->SetOrigin(EC_Struct::HalfPoint(StarImage->ClientSize));
                StarImage->SetPosition(GalaxyPointToMapPoint(Star->Position));
                StarImage->SetDepth(5.0);
                StarImage->SetPositionModeW(true);
                StarImage->SequenceIndex = 0;
                StarImage->UpdateAutoGeometry();
                StarImage->SetSequenceFrame(aMyFunction::RandomIntRange(0, StarImage->SequenceFrameCount - 1));
                StarImage->SetName(static_cast<pas::WideString>(pas::concat_ansi({"gs_", SysUtils::Int64ToStr(Star->Id)})));
                StarImage->UserValue = 1;
                StarImage->RestartPlayback();
                NameLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, MapPanel);
                NameLabel->SetFontName(GlobalsV::NormalBoldFontName);
                NameLabel->SetDepth(6.0);
                switch (GlobalsV::GalaxyMapFontChoice) {
                    case GlobalsV::gmfRanger: NameLabel->SetFontName(GlobalsV::RangerFontName); break;
                    case GlobalsV::gmfMini: NameLabel->SetFontName(GlobalsV::MiniFontName); break;
                    case GlobalsV::gmfSmall: NameLabel->SetFontName(GlobalsV::SmallFontName); break;
                    case GlobalsV::gmfSmallBold: NameLabel->SetFontName(GlobalsV::SmallBoldFontName); break;
                    case GlobalsV::gmfNormal: NameLabel->SetFontName(GlobalsV::NormalFontName); break;
                    case GlobalsV::gmfNormalBold: NameLabel->SetFontName(GlobalsV::NormalBoldFontName); break;
                    default: NameLabel->SetFontName(GlobalsV::NormalBoldFontName); break;
                }
                NameLabel->SetSize(ClassesImports::Point(150, 1));
                NameLabel->SetTextAlignX(GI_Main::taxCenter);
                NameLabel->SetTextAlignY(GI_Main::tayAuto);
                NameLabel->SetPosition(ClassesImports::Point(StarImage->LocalPosition.X - NameLabel->ClientSize.X / 2, StarImage->LocalPosition.Y + StarImage->ClientSize.Y / 2 - 5));
                NameLabel->SetPositionModeW(false);
                Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, 0);
                for (auto cpp_range_13 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_13.next(J); ) {
                    Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, J);
                    if (Planet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                        break;
                    }
                }
                Text = Star->Name;
                if (Star->Status.CustomFaction != u"") {
                    ColoredName = ([&] {
                        pas::WideString lookupNamedColorTag = aConst::LookupNamedColorTag(Star->Status.CustomFaction);
                        pas::WideString text = Text;
                        return aMyFunction::WrapTextInColor(std::move(text), std::move(lookupNamedColorTag));
                    }());
                } else {
                    ColoredName = pas::WideString();
                    J = 1;
                    K = pas::idiv(Star->Name.length(), Star->CountDistinctInhabitedPlanetOwners());
                    ColoredName = pas::concat_wide({ColoredName, aMyFunction::WrapTextInColor(pas::copy(Text, 1, K), aConst::OwnerInfo[Planet->OwnerId].ColorTag)});
                    Text = pas::copy(Text, K + 1, Text.length());
                    if (Text != u"") {
                        for (OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiMaloc); OwnerId <= static_cast<std::uint8_t>(7); ++OwnerId) {
                            if (OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) && Star->CountPlanetsByOwner(OwnerId) > 0 && Planet->OwnerId != OwnerId) {
                                K = pas::idiv(Star->Name.length(), Star->CountDistinctInhabitedPlanetOwners());
                                ++J;
                                if (J == Star->CountDistinctInhabitedPlanetOwners()) {
                                    K = Text.length();
                                }
                                ColoredName = pas::concat_wide({ColoredName, aMyFunction::WrapTextInColor(pas::copy(Text, 1, K), aConst::OwnerInfo[OwnerId].ColorTag)});
                                Text = pas::copy(Text, K + 1, Text.length());
                            }
                        }
                    }
                }
                ColoredName = pas::concat_wide({ColoredName, Star->MapLabel});
                ForceLabel = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, MapPanel);
                ForceLabel->SetFontName(GlobalsV::MiniFontName);
                ForceLabel->SetDepth(6.0);
                ForceLabel->SetSize(ClassesImports::Point(150, 1));
                ForceLabel->SetTextAlignX(GI_Main::taxCenter);
                ForceLabel->SetTextAlignY(GI_Main::tayAuto);
                ForceLabel->SetPositionModeW(false);
                ForceText = pas::WideString();
                BossText = pas::WideString();
                if (CanShowExtendedRadarInfo(Star)) {
                    CoalitionCount = Star->CountForcesByOwnerGroups(Strength, true, false, false, false);
                    BlazerCount = Star->CountDominatorForces(aGalaxyStruct::dsBlazer, false, false, Strength);
                    KellerCount = Star->CountDominatorForces(aGalaxyStruct::dsKeller, false, false, Strength);
                    TerronCount = Star->CountDominatorForces(aGalaxyStruct::dsTerron, false, false, Strength);
                    PirateCount = Star->CountPirateForces(false, Strength, true, true);
                    // Keep the recovered out-string clearing explicit at these two call sites.
                    CustomFaction = pas::WideString();
                    CustomCount = Star->CountCustomFactionForces(false, CustomFaction, Strength);
                    OtherFaction = pas::WideString();
                    OtherCount = Star->CountOtherCustomFactionForces(false, OtherFaction, Strength);
                    if (aKling::BlazerShip != nullptr && aKling::BlazerShip->CurrentStar == Star && aKling::BlazerShip->InNormalSpace()) {
                        if (TfGalaxy2::CanRevealBossPresence(aKling::BlazerShip) && aShip::TShip_CanResolveObjectWithScanner(aPlayer::GetPlayer(), aKling::BlazerShip)) {
                            if (BossText.length() > 0) {
                                BossText = pas::concat_wide({BossText, u"-"});
                            }
                            BossText = pas::concat_wide({BossText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(aConst::LocalizedText(u"FormGalaxy.Boss1"_wref.get()).read(1)), u"<color=255,0,0>"_w)});
                        } else {
                            ++BlazerCount;
                        }
                    }
                    if (aKling::KellerShip != nullptr && aKling::KellerShip->CurrentStar == Star && aKling::KellerShip->InNormalSpace()) {
                        if (TfGalaxy2::CanRevealBossPresence(aKling::KellerShip) && aShip::TShip_CanResolveObjectWithScanner(aPlayer::GetPlayer(), aKling::KellerShip)) {
                            if (BossText.length() > 0) {
                                BossText = pas::concat_wide({BossText, u"-"});
                            }
                            BossText = pas::concat_wide({BossText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(aConst::LocalizedText(u"FormGalaxy.Boss2"_wref.get()).read(1)), u"<color=0,128,255>"_w)});
                        } else {
                            ++KellerCount;
                        }
                    }
                    if (aKling::TerronShip != nullptr && aKling::TerronShip->CurrentStar == Star && aKling::TerronShip->InNormalSpace()) {
                        if (TfGalaxy2::CanRevealBossPresence(aKling::TerronShip) && aShip::TShip_CanResolveObjectWithScanner(aPlayer::GetPlayer(), aKling::TerronShip)) {
                            if (BossText.length() > 0) {
                                BossText = pas::concat_wide({BossText, u"-"});
                            }
                            BossText = pas::concat_wide({BossText, aMyFunction::WrapTextInColor(static_cast<pas::WideString>(aConst::LocalizedText(u"FormGalaxy.Boss3"_wref.get()).read(1)), u"<color=0,255,0>"_w)});
                        } else {
                            ++TerronCount;
                        }
                    }
                    if (CoalitionCount > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        ForceText = pas::concat_wide({ForceText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(CoalitionCount), u"<color=255,240,100>"_w)});
                    }
                    if (BlazerCount > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        ForceText = pas::concat_wide({ForceText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(BlazerCount), u"<color=255,0,0>"_w)});
                    }
                    if (KellerCount > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        ForceText = pas::concat_wide({ForceText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(KellerCount), u"<color=0,128,255>"_w)});
                    }
                    if (TerronCount > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        ForceText = pas::concat_wide({ForceText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(TerronCount), u"<color=0,255,0>"_w)});
                    }
                    if (PirateCount > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        ForceText = pas::concat_wide({ForceText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(PirateCount), u"<color=255,255,254>"_w)});
                    }
                    if (CustomCount > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        ForceText = pas::concat_wide_reverse({([&] {
                            pas::WideString lookupNamedColorTag_2 = aConst::LookupNamedColorTag(CustomFaction);
                            pas::WideString intToStr = pas::wide_int_to_str(CustomCount);
                            return aMyFunction::WrapTextInColor(std::move(intToStr), std::move(lookupNamedColorTag_2));
                        }()), ForceText});
                    }
                    if (OtherCount > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        if (OtherFaction != u"") {
                            ForceText = pas::concat_wide_reverse({([&] {
                                pas::WideString lookupNamedColorTag_3 = aConst::LookupNamedColorTag(OtherFaction);
                                pas::WideString intToStr_2 = pas::wide_int_to_str(OtherCount);
                                return aMyFunction::WrapTextInColor(std::move(intToStr_2), std::move(lookupNamedColorTag_3));
                            }()), ForceText});
                        } else {
                            ForceText = pas::concat_wide({ForceText, aMyFunction::WrapTextInColor(pas::wide_int_to_str(OtherCount), u"<color=127,127,127>"_w)});
                        }
                    }
                    if (BossText.length() > 0) {
                        if (ForceText.length() > 0) {
                            ForceText = pas::concat_wide({ForceText, u"-"});
                        }
                        ForceText = pas::concat_wide({ForceText, BossText});
                    }
                }
                NameLabel->SetText(ColoredName);
                ForceLabel->SetText(ForceText);
                ForceLabel->SetPosition(ClassesImports::Point(NameLabel->LocalPosition.X, NameLabel->LocalPosition.Y + ForceLabel->ClientSize.Y + 2));
                Star->GetControlPresence(PlayerPartyPresent, CoalitionPresent, DominatorsPresent, PiratesPresent, CustomPresent);
                if (CustomPresent) {
                    DominatorsPresent = true;
                }
                if (Star->Status.Battle != 0 && (static_cast<std::uint8_t>(PlayerPartyPresent ^ 1) || (CoalitionPresent || PiratesPresent) && (PiratesPresent || DominatorsPresent) && (CoalitionPresent || DominatorsPresent))) {
                    BattleRing = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MapPanel);
                    BattleRing->SetImagePath(u"Bm.FormGalaxy.RedRing"_wref.get());
                    BattleRing->SetSize(BattleRing->GetContentSize());
                    BattleRing->SetOrigin(ClassesImports::Point(BattleRing->ClientSize.X / 2, BattleRing->ClientSize.Y / 2));
                    BattleRing->SetPosition(GalaxyPointToMapPoint(Star->Position));
                    BattleRing->SetDepth(4.0);
                    BattleRing->SetPositionModeW(true);
                    BattleRing->SequenceIndex = 0;
                    BattleRing->UpdateAutoGeometry();
                    BattleRing->RestartPlayback();
                    StarImage = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, MapPanel);
                    if (CoalitionPresent && PiratesPresent && DominatorsPresent) {
                        StarImage->SetImagePath(u"Bm.FormGalaxy.BattleRoyale"_wref.get());
                    } else if (CoalitionPresent && PiratesPresent) {
                        StarImage->SetImagePath(u"Bm.FormGalaxy.BattleNormalsVsPirates"_wref.get());
                    } else if (CoalitionPresent && DominatorsPresent) {
                        StarImage->SetImagePath(u"Bm.FormGalaxy.BattleNormalsVsDominators"_wref.get());
                    } else if (PiratesPresent && DominatorsPresent) {
                        StarImage->SetImagePath(u"Bm.FormGalaxy.BattlePiratesVsDominators"_wref.get());
                    } else {
                        StarImage->SetImagePath(u"Bm.FormGalaxy.Defend"_wref.get());
                    }
                    StarImage->SetSize(StarImage->GetContentSize());
                    StarImage->SetOrigin(ClassesImports::Point(StarImage->ClientSize.X / 2, StarImage->ClientSize.Y / 2));
                    StarImage->SetPosition(EC_Struct::AddPoints(GalaxyPointToMapPoint(Star->Position), ClassesImports::Point(BattleRing->ClientSize.X / 2 - 5, -BattleRing->ClientSize.Y / 2 + 5)));
                    StarImage->SetDepth(3.0);
                    StarImage->SetPositionModeW(true);
                    StarImage->SequenceIndex = 0;
                    StarImage->UpdateAutoGeometry();
                    StarImage->RestartPlayback();
                }
                SelectedHole = nullptr;
                for (auto cpp_range_14 = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Holes) - 1); cpp_range_14.next(J); ) {
                    Hole = pas::list_at<aGalaxy::THole>(aGalaxy::Galaxy->Holes, J);
                    if (Hole->ArcadeMapName != u"NoEntry" && Hole->Star1 == Star && (SelectedHole == nullptr || reinterpret_cast<SE_Hole::THoleSE*>(SelectedHole->Graphic)->GalaxyPriority < reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->GalaxyPriority)) {
                        SelectedHole = Hole;
                    }
                }
                if (SelectedHole != nullptr) {
                    HoleImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapPanel);
                    HoleImage->SetImagePath(reinterpret_cast<SE_Hole::THoleSE*>(SelectedHole->Graphic)->GalaxyImagePath);
                    HoleImage->SetSize(HoleImage->GetContentSize());
                    HoleImage->SetOrigin(ClassesImports::Point(HoleImage->ClientSize.X / 2, HoleImage->ClientSize.Y / 2));
                    HoleImage->SetPosition(EC_Struct::AddPoints(GalaxyPointToMapPoint(Star->Position), ClassesImports::Point(HoleImage->ClientSize.X / 2 + 3, HoleImage->ClientSize.Y / 2 + 3)));
                    HoleImage->SetDepth(3.0);
                    HoleImage->SetPositionModeW(true);
                }
            }
        }
        {
            GI_GAI::TgaiGI* PathCurPos = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"PathCurPos"_wref.get()));
            PathCurPos->SetPosition(GalaxyPointToMapPoint(aPlayer::GetPlayer()->CurrentStar->Position));
            PathCurPos->SetOrigin(EC_Struct::HalfPoint(PathCurPos->ClientSize));
            PathCurPos->SetActive(true);
            PathCurPos->RestartPlayback();
        }
        Points = nullptr;
        {
            GI_Circle::TCircleGI* JampMaxShr = pas::checked_cast<GI_Circle::TCircleGI*>(GetByName(u"JampMaxShr"_wref.get()));
            JampMaxShr->SetCenter(GalaxyPointToMapPoint(aPlayer::GetPlayer()->CurrentStar->Position));
            if (aPlayer::GetPlayer()->GetFuelTanks() == nullptr || static_cast<std::uint8_t>(aShip::TShip_CanUseEquipmentTech(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetFuelTanks()) ^ 1)) {
                JampMaxShr->SetRadius(1);
            } else {
                JampMaxShr->SetRadius(GalaxyDistanceToMapDistance(aPlayer::GetPlayer()->JumpRange));
            }
            if (pas::in_set<0, 0, 3, 3>(ViewMode)) {
                JampMaxShr->SetRadius(1000);
            }
        }
        {
            GI_Circle::TCircleGI* JampMaxColor = pas::checked_cast<GI_Circle::TCircleGI*>(GetByName(u"JampMaxColor"_wref.get()));
            JampMaxColor->SetCenter(JampMaxColor->ToAbsolutePoint(GalaxyPointToMapPoint(aPlayer::GetPlayer()->CurrentStar->Position)));
            if (aPlayer::GetPlayer()->GetFuelTanks() == nullptr || static_cast<std::uint8_t>(aShip::TShip_CanUseEquipmentTech(aPlayer::GetPlayer(), aPlayer::GetPlayer()->GetFuelTanks()) ^ 1)) {
                JampMaxColor->SetRadius(1);
            } else {
                JampMaxColor->SetRadius(GalaxyDistanceToMapDistance(aPlayer::GetPlayer()->JumpRange));
            }
            if (pas::in_set<0, 0, 3, 3>(ViewMode)) {
                JampMaxColor->SetRadius(1000);
            }
        }
        {
            GI_Circle::TCircleGI* RadarDetect = pas::checked_cast<GI_Circle::TCircleGI*>(GetByName(u"RadarDetect"_wref.get()));
            if (aPlayer::GetPlayer()->GetRadar() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactRadar) > 0) {
                RadarDetect->SetActive(true);
                RadarDetect->SetCenter(RadarDetect->ToAbsolutePoint(GalaxyPointToMapPoint(aPlayer::GetPlayer()->CurrentStar->Position)));
                RadarDetect->SetRadius(GalaxyDistanceToMapDistance(TfGalaxy2::GetRadarSummaryRadius()));
            } else {
                RadarDetect->SetActive(false);
            }
        }
        if (ViewMode == 1) {
            ConfigureReadOnlyMap();
        } else if (pas::in_set<0, 0, 2, 3>(ViewMode)) {
            ConfigureJumpSelection();
        }
        if (JumpAnimationTimer != nullptr) {
            CancelCallbackTimer(JumpAnimationTimer);
            JumpAnimationTimer = nullptr;
        }
        JumpAnimationTimer = ScheduleCallbackTimer(20, 20, pas::bind_method<&TfGalaxy2::UpdateJumpAnimations>(this), 0);
        JumpHintAnimationState = 0;
        ShowStarInfo(nullptr);
        if (CapturePreviewOnOpen) {
            {
                GI_GAI::TgaiGI* PathCurPos_2 = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"PathCurPos"_wref.get()));
                PathCurPos_2->SetSequenceFrame(PathCurPos_2->SequenceFrameCount / 2);
            }
            ClearJumpPath();
            pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImagePanel"_wref.get()))->SetActive(false);
            DrawQueuedUpdateRects();
            First = EC_Struct::SubtractPoints(MapPanel->ToAbsolutePoint(GalaxyPointToMapPoint(aPlayer::GetPlayer()->CurrentStar->Position)), ClassesImports::Point(pas::shr(GR_Main::SecondarySavePreviewGraph->Width, 1), pas::shr(GR_Main::SecondarySavePreviewGraph->Height, 1)));
            if (First.X < MapPanel->HitTestBounds.Left) {
                First.X = MapPanel->HitTestBounds.Left;
            }
            if (First.X + GR_Main::SecondarySavePreviewGraph->Width >= MapPanel->HitTestBounds.Right) {
                First.X = MapPanel->HitTestBounds.Right - GR_Main::SecondarySavePreviewGraph->Width;
            }
            if (First.Y < MapPanel->HitTestBounds.Top) {
                First.Y = MapPanel->HitTestBounds.Top;
            }
            if (First.Y + GR_Main::SecondarySavePreviewGraph->Height >= MapPanel->HitTestBounds.Bottom) {
                First.Y = MapPanel->HitTestBounds.Bottom - GR_Main::SecondarySavePreviewGraph->Height;
            }
            if (GlobalsV::HardwareRenderingEnabled) {
                CaptureBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                CaptureBuffer->LoadFromScreen(1);
                {
                    std::int32_t pitchBytes = CaptureBuffer->PitchBytes;
                    std::int32_t width = GR_Main::SecondarySavePreviewGraph->Width;
                    std::int32_t height = GR_Main::SecondarySavePreviewGraph->Height;
                    std::int32_t cpp_arg = CaptureBuffer->PitchBytes * First.Y + First.X * 4;
                    void* pixels = CaptureBuffer->GetPixels();
                    void* addPointerOffset = EC_Mem::AddPointerOffset(pixels, cpp_arg);
                    void* pixels_2 = GR_Main::SecondarySavePreviewGraph->GetPixels();
                    std::int32_t pitchBytes_2 = GR_Main::SecondarySavePreviewGraph->PitchBytes;
                    GR_Main::CopyBgraToRgb24(pixels_2, pitchBytes_2, addPointerOffset, pitchBytes, width, height);
                }
                pas::free(CaptureBuffer);
                CaptureBuffer = nullptr;
            } else {
                std::int32_t pitchBytes_3 = GR_Main::SecondarySavePreviewGraph->PitchBytes;
                std::int32_t width_2 = GR_Main::SavePreviewGraph->Width;
                std::int32_t height_2 = GR_Main::SavePreviewGraph->Height;
                std::uint8_t* cpp_left = static_cast<std::uint8_t*>(GR_Main::ScreenRenderBuffer->GetPixels()) + First.X * 2;
                void* cpp_arg_2 = cpp_left + First.Y * GR_Main::ScreenRenderBuffer->PitchBytes;
                void* pixels_3 = GR_Main::SecondarySavePreviewGraph->GetPixels();
                std::int32_t pitchBytes_4 = GR_Main::ScreenRenderBuffer->PitchBytes;
                GR_Main::Ex_OKGF_Convert565toRGB(cpp_arg_2, pitchBytes_4, pixels_3, pitchBytes_3, width_2, height_2);
            }
            RequestClose(1);
        }
        aGalaxy::Galaxy->PrimeIntegrityChecksum(1111);
        MainPanel->RebuildMessageButtons(false);
    }

    void TfGalaxy2::OnClose() {
        GI_MessageLoop::TMessageLoopGI::OnClose();
        aGalaxy::Galaxy->CheckIntegrityChecksum(1112);
        if (aPlayer::GetPlayer() != nullptr && static_cast<std::uint8_t>(CapturePreviewOnOpen ^ 1)) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnLeavingForm, nullptr, nullptr, 0);
        }
        CapturePreviewOnOpen = false;
        if (JumpAnimationTimer != nullptr) {
            CancelCallbackTimer(JumpAnimationTimer);
            JumpAnimationTimer = nullptr;
        }
        if (StarInfoHideTimer != nullptr) {
            CancelCallbackTimer(StarInfoHideTimer);
            StarInfoHideTimer = nullptr;
        }
        ClearJumpPath();
        if (pas::in_set<0, 0, 2, 3>(ViewMode)) {
            ClearJumpSelectionCallbacks();
        } else if (ViewMode == 1) {
            ClearReadOnlyMapCallbacks();
        }
        GetByName(u"InfoStarPanel"_wref.get())->FreeOwnedChildren();
        MapPanel->FreeOwnedChildren();
        HideBuffer->ClearOwnedBuffer();
        MainPanel->OnClose();
        GR_Main::AuxRenderBuffer->Clear();
        ViewMode = 0;
    }

    void TfGalaxy2::CloseClicked(GI_MessageLoop::TObjectGI* Sender) {
        GlobalsV::RequestedScreenId = GlobalsV::GalaxyReturnScreenId;
        RequestClose(1);
    }

    void TfGalaxy2::MainPanelKeyDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) ^ 1) && static_cast<std::uint8_t>(GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU) ^ 1)) {
            if (Key == 'M') {
                CloseClicked(nullptr);
            } else if (Key == WindowsSdk::VK_RETURN || Key == 'J') {
                if (!JumpButton->Disabled) {
                    JumpClicked(nullptr);
                }
            } else if (Key == WindowsSdk::VK_ESCAPE) {
                CloseClicked(nullptr);
            } else if (Key == WindowsSdk::VK_F11) {
                if (!MainPanel->RemoveDismissibleMessages(u"GOODS"_w)) {
                    MainPanel->RemoveDismissibleMessages(pas::WideString());
                }
            }
        }
    }

    void TfGalaxy2::MainPanelMouseUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (GlobalsV::ClickAutoCloseForm) {
            if (static_cast<std::uint8_t>(pas::checked_cast<GI_Image::TImageGI*>(GetByName(u"ImagePanel"_wref.get()))->HitTestPixel(Point) ^ 1) && static_cast<std::uint8_t>(GetByName(u"HideBuf"_wref.get())->ContainsPoint(Point) ^ 1) && static_cast<std::uint8_t>(pas::checked_cast<GI_GraphButton::TGraphButtonGI*>(GetByName(u"ButExit"_wref.get()))->ContainsPoint(Point) ^ 1) && static_cast<std::uint8_t>(JumpButton->ContainsPoint(Point) ^ 1) && static_cast<std::uint8_t>(GetByName(u"PM_PanelMsg"_wref.get())->ContainsPoint(Point) ^ 1)) {
                CloseClicked(nullptr);
            }
        }
    }

    // Requires an active Prolonger effect as well as radar coverage.
    std::uint8_t TfGalaxy2::CanShowExtendedRadarInfo(aGalaxy::TStar* Star) {
        EC_Struct::TPointF First{};
        EC_Struct::TPointF Second{};
        std::uint8_t Result = false;
        if (aPlayer::GetPlayer()->GetRadar() != nullptr && aPlayer::GetPlayer()->CountActiveArtefacts(aConst::t_ArtefactRadar) > 0) {
            First = Star->Position;
            Second = aPlayer::GetPlayer()->CurrentStar->Position;
            {
                std::int64_t cpp_right = System::Round(System::Sqrt(pas::sqr(static_cast<long double>(First.X) - Second.X) + pas::sqr(static_cast<long double>(First.Y) - Second.Y)));
                if (TfGalaxy2::GetRadarSummaryRadius() < cpp_right) {
                    return Result;
                }
            }
            return true;
        }
        return Result;
    }

    // Measured in radar-summary units of 150 range units.
    std::int32_t TfGalaxy2::GetRadarSummaryRadius() {
        return System::Round(pas::real_divide(aShip::TShip_GetRadarRange(aPlayer::GetPlayer()), 1.5E+2L));
    }

    EC_Struct::TPointF TfGalaxy2::MapPointToGalaxyPoint(WindowsSdk::TPoint Point) {
        EC_Struct::TPointF Result{};
        Result.X = pas::real_divide(Point.X - MapPixelBounds.Left, MapPixelBounds.Right - MapPixelBounds.Left + 1) * GalaxyExtent.X + GalaxyOrigin.X;
        Result.Y = pas::real_divide(Point.Y - MapPixelBounds.Top, MapPixelBounds.Bottom - MapPixelBounds.Top + 1) * GalaxyExtent.Y + GalaxyOrigin.Y;
        return Result;
    }

    WindowsSdk::TPoint TfGalaxy2::GalaxyPointToMapPoint(EC_Struct::TPointF Point) {
        WindowsSdk::TPoint Result{};
        Result.X = System::Round(pas::real_divide(static_cast<long double>(Point.X) - GalaxyOrigin.X, GalaxyExtent.X) * (MapPixelBounds.Right - MapPixelBounds.Left + 1)) + MapPixelBounds.Left;
        Result.Y = System::Round(pas::real_divide(static_cast<long double>(Point.Y) - GalaxyOrigin.Y, GalaxyExtent.Y) * (MapPixelBounds.Bottom - MapPixelBounds.Top + 1)) + MapPixelBounds.Top;
        return Result;
    }

    // Uses the horizontal projection scale.
    std::int32_t TfGalaxy2::GalaxyDistanceToMapDistance(double Distance) {
        return System::Round(pas::real_divide(Distance, GalaxyExtent.X) * (MapPixelBounds.Right - MapPixelBounds.Left + 1));
    }

    void TfGalaxy2::RebuildJumpPath() {
        std::int32_t I{};
        std::int32_t TotalDistance{};
        std::int32_t JumpDistance{};
        WindowsSdk::TPoint ImageSize{};
        EC_Struct::TPointF First{};
        EC_Struct::TPointF Second{};
        EC_Struct::TPointF DotPoint{};
        GI_Image::TImageGI* DotImage{};
        double Length{};
        double Distance{};
        double Slope{};
        double Step{};
        double Origin{};
        std::uint8_t UseY{};
        std::uint8_t Reachable{};
        ClearJumpPath();
        {
            GI_GAI::TgaiGI* PathCurPos = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"PathCurPos"_wref.get()));
            PathCurPos->SetPosition(GalaxyPointToMapPoint(aPlayer::GetPlayer()->CurrentStar->Position));
            PathCurPos->SetOrigin(EC_Struct::HalfPoint(PathCurPos->ClientSize));
            PathCurPos->SetActive(true);
            PathCurPos->RestartPlayback();
        }
        pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Distance"_wref.get()))->SetText(u""_wref.get());
        if (aPlayer::GetPlayer()->CurrentStar != SelectedJumpStar && SelectedJumpStar != nullptr) {
            {
                GI_GAI::TgaiGI* PathDesPos = pas::checked_cast<GI_GAI::TgaiGI*>(GetByName(u"PathDesPos"_wref.get()));
                PathDesPos->SetPosition(GalaxyPointToMapPoint(SelectedJumpStar->Position));
                PathDesPos->SetOrigin(EC_Struct::HalfPoint(PathDesPos->ClientSize));
                PathDesPos->SetActive(true);
                PathDesPos->RestartPlayback();
            }
            First = EC_Struct::PointToPointF(GalaxyPointToMapPoint(SelectedJumpStar->Position));
            Second = EC_Struct::PointToPointF(GalaxyPointToMapPoint(aPlayer::GetPlayer()->CurrentStar->Position));
            if (std::fabs(static_cast<long double>(First.X) - Second.X) < std::fabs(static_cast<long double>(First.Y) - Second.Y)) {
                UseY = true;
            } else {
                UseY = false;
            }
            Length = System::Sqrt((static_cast<long double>(First.X) - Second.X) * (static_cast<long double>(First.X) - Second.X) + (static_cast<long double>(First.Y) - Second.Y) * (static_cast<long double>(First.Y) - Second.Y));
            if (UseY) {
                Slope = pas::real_divide(static_cast<long double>(Second.X) - First.X, static_cast<long double>(Second.Y) - First.Y);
                Step = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
                if (static_cast<long double>(Second.Y) - First.Y < 0.0L) {
                    Step = -Step;
                }
                Origin = First.Y;
            } else {
                Slope = pas::real_divide(static_cast<long double>(Second.Y) - First.Y, static_cast<long double>(Second.X) - First.X);
                Step = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
                if (static_cast<long double>(Second.X) - First.X < 0.0L) {
                    Step = -Step;
                }
                Origin = First.X;
            }
            Distance = 0.0;
            while (Distance < Length) {
                if (UseY) {
                    DotPoint.Y = static_cast<long double>(Distance) * Step + Origin;
                    DotPoint.X = (static_cast<long double>(DotPoint.Y) - First.Y) * Slope + First.X;
                } else {
                    DotPoint.X = static_cast<long double>(Distance) * Step + Origin;
                    DotPoint.Y = (static_cast<long double>(DotPoint.X) - First.X) * Slope + First.Y;
                }
                DotImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapPanel);
                DotImage->SetDepth(1.0);
                DotImage->SetPosition(EC_Struct::TruncatePointF(DotPoint));
                DotImage->SetPositionModeW(true);
                DotImage->SetImagePath(u"GI,Bm.PI.Path1"_w);
                ImageSize = DotImage->GetContentSize();
                DotImage->SetOrigin(ClassesImports::Point(ImageSize.X / 2, ImageSize.Y / 2));
                DotImage->SetSize(ImageSize);
                Distance = Distance + 1.0E+1L;
            }
            First = EC_Struct::PointToPointF(GalaxyPointToMapPoint(SelectedJumpStar->Position));
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(RouteStars) - 1); cpp_range.next(I); ) {
                Second = EC_Struct::PointToPointF(GalaxyPointToMapPoint(pas::list_at<aGalaxy::TStar>(RouteStars, I)->Position));
                if (std::fabs(static_cast<long double>(First.X) - Second.X) < std::fabs(static_cast<long double>(First.Y) - Second.Y)) {
                    UseY = true;
                } else {
                    UseY = false;
                }
                Length = System::Sqrt((static_cast<long double>(First.X) - Second.X) * (static_cast<long double>(First.X) - Second.X) + (static_cast<long double>(First.Y) - Second.Y) * (static_cast<long double>(First.Y) - Second.Y));
                if (UseY) {
                    Slope = pas::real_divide(static_cast<long double>(Second.X) - First.X, static_cast<long double>(Second.Y) - First.Y);
                    Step = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
                    if (static_cast<long double>(Second.Y) - First.Y < 0.0L) {
                        Step = -Step;
                    }
                    Origin = First.Y;
                } else {
                    Slope = pas::real_divide(static_cast<long double>(Second.Y) - First.Y, static_cast<long double>(Second.X) - First.X);
                    Step = pas::real_divide(1.0L, System::Sqrt(static_cast<long double>(Slope) * Slope + 1.0L));
                    if (static_cast<long double>(Second.X) - First.X < 0.0L) {
                        Step = -Step;
                    }
                    Origin = First.X;
                }
                Distance = 0.0;
                while (Distance < Length) {
                    if (UseY) {
                        DotPoint.Y = static_cast<long double>(Distance) * Step + Origin;
                        DotPoint.X = (static_cast<long double>(DotPoint.Y) - First.Y) * Slope + First.X;
                    } else {
                        DotPoint.X = static_cast<long double>(Distance) * Step + Origin;
                        DotPoint.Y = (static_cast<long double>(DotPoint.X) - First.X) * Slope + First.Y;
                    }
                    DotImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, MapPanel);
                    DotImage->SetDepth(1.0);
                    DotImage->SetPosition(EC_Struct::TruncatePointF(DotPoint));
                    DotImage->SetPositionModeW(true);
                    DotImage->SetImagePath(u"GI,Bm.PI.Path2"_w);
                    ImageSize = DotImage->GetContentSize();
                    DotImage->SetOrigin(ClassesImports::Point(ImageSize.X / 2, ImageSize.Y / 2));
                    DotImage->SetSize(ImageSize);
                    Distance = Distance + 1.0E+1L;
                }
                First = Second;
            }
            Reachable = true;
            if (pas::list_count(RouteStars) <= 0) {
                First = SelectedJumpStar->Position;
                Second = aPlayer::GetPlayer()->CurrentStar->Position;
                {
                    GI_Label::TLabelGI* Distance_2 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Distance"_wref.get()));
                    ImageSize = GalaxyPointToMapPoint(SelectedJumpStar->Position);
                    Distance_2->SetPosition(ClassesImports::Point(ImageSize.X + 15, ImageSize.Y - Distance_2->ClientSize.Y / 2));
                    JumpDistance = System::Round(System::Sqrt(pas::sqr(static_cast<long double>(First.X) - Second.X) + pas::sqr(static_cast<long double>(First.Y) - Second.Y)));
                    if (aPlayer::GetPlayer()->GetJumpRange() < JumpDistance) {
                        Reachable = pas::in_set<0, 0, 3, 3>(ViewMode);
                    }
                    Distance_2->SetText(pas::wide_int_to_str(JumpDistance));
                }
            } else {
                First = aPlayer::GetPlayer()->CurrentStar->Position;
                Second = SelectedJumpStar->Position;
                TotalDistance = System::Round(System::Sqrt(pas::sqr(static_cast<long double>(First.X) - Second.X) + pas::sqr(static_cast<long double>(First.Y) - Second.Y)));
                if (aPlayer::GetPlayer()->GetJumpRange() < TotalDistance) {
                    Reachable = pas::in_set<0, 0, 3, 3>(ViewMode);
                }
                First = Second;
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(RouteStars) - 1); cpp_range_2.next(I); ) {
                    Second = pas::list_at<aGalaxy::TStar>(RouteStars, I)->Position;
                    JumpDistance = System::Round(System::Sqrt(pas::sqr(static_cast<long double>(First.X) - Second.X) + pas::sqr(static_cast<long double>(First.Y) - Second.Y)));
                    if (aPlayer::GetPlayer()->GetJumpRange() < JumpDistance) {
                        Reachable = pas::in_set<0, 0, 3, 3>(ViewMode);
                    }
                    TotalDistance += JumpDistance;
                    First = Second;
                }
                {
                    GI_Label::TLabelGI* Distance_3 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Distance"_wref.get()));
                    ImageSize = GalaxyPointToMapPoint(First);
                    Distance_3->SetPosition(ClassesImports::Point(ImageSize.X + 15, ImageSize.Y - Distance_3->ClientSize.Y / 2));
                    Distance_3->SetText(pas::wide_int_to_str(TotalDistance));
                }
            }
            {
                GI_Label::TLabelGI* Distance_4 = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"Distance"_wref.get()));
                if (Reachable) {
                    Distance_4->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
                } else {
                    Distance_4->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 0, 0));
                }
            }
        }
    }

    void TfGalaxy2::ClearJumpPath() {
        GI_MessageLoop::TObjectGI* Current{};
        GI_MessageLoop::TObjectGI* Control = MapPanel->FirstChild;
        while (Control != nullptr) {
            Current = Control;
            Control = Control->NextSibling;
            if (Current->Depth == 1.0L) {
                Current->SetActive(false);
                pas::free(Current);
            }
        }
        GetByName(u"PathDesPos"_wref.get())->SetActive(false);
    }

    void TfGalaxy2::ConfigureReadOnlyMap() {
        {
            GI_GraphButton::TGraphButtonGI* cpp_with = JumpButton;
            cpp_with->SetDisabled(true);
            cpp_with->UpCallback = nullptr;
            cpp_with->MouseEnterCallback = nullptr;
            cpp_with->MouseLeaveCallback = nullptr;
        }
        MapPanel->LeftButtonDownCallback = pas::bind_method<&TfGalaxy2::MapLeftButtonDown>(this);
        MapPanel->LeftButtonUpCallback = pas::bind_method<&TfGalaxy2::MapButtonUp>(this);
        MapPanel->MouseMoveCallback = pas::bind_static_method<&TfGalaxy2::MapMouseMove>(this);
        MapPanel->RightButtonDownCallback = pas::bind_method<&TfGalaxy2::MapRightButtonDown>(this);
        MapPanel->RightButtonUpCallback = pas::bind_method<&TfGalaxy2::MapButtonUp>(this);
        SelectedJumpStar = aPlayer::GetPlayer()->CurrentStar;
        RebuildJumpPath();
    }

    void TfGalaxy2::ClearReadOnlyMapCallbacks() {
        MapPanel->LeftButtonDownCallback = nullptr;
        MapPanel->MouseMoveCallback = nullptr;
    }

    void TfGalaxy2::ConfigureJumpSelection() {
        MapPanel->LeftButtonDownCallback = pas::bind_method<&TfGalaxy2::MapLeftButtonDown>(this);
        MapPanel->LeftButtonUpCallback = pas::bind_method<&TfGalaxy2::MapButtonUp>(this);
        MapPanel->MouseMoveCallback = pas::bind_static_method<&TfGalaxy2::MapMouseMove>(this);
        MapPanel->RightButtonDownCallback = pas::bind_method<&TfGalaxy2::MapRightButtonDown>(this);
        MapPanel->RightButtonUpCallback = pas::bind_method<&TfGalaxy2::MapButtonUp>(this);
        {
            GI_GraphButton::TGraphButtonGI* cpp_with = JumpButton;
            cpp_with->SetDisabled(false);
            cpp_with->UpCallback = pas::bind_method<&TfGalaxy2::JumpClicked>(this);
            cpp_with->MouseEnterCallback = pas::bind_method<&TfGalaxy2::JumpMouseEnter>(this);
            cpp_with->MouseLeaveCallback = pas::bind_method<&TfGalaxy2::JumpMouseLeave>(this);
        }
        MapPanel->LeftButtonDoubleClickCallback = pas::bind_method<&TfGalaxy2::MapDoubleClick>(this);
        if (aPlayer::GetPlayer()->Order == aShip::soJump && pas::class_cast_if<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget) != nullptr) {
            SelectedJumpStar = pas::checked_cast<aGalaxy::TStar*>(aPlayer::GetPlayer()->OrderTarget);
        } else {
            SelectedJumpStar = aPlayer::GetPlayer()->CurrentStar;
        }
        RebuildJumpPath();
        JumpButton->SetDisabled(SelectedJumpStar == nullptr || aPlayer::GetPlayer()->CurrentStar == SelectedJumpStar || ViewMode == 1 || aPlayer::GetPlayer()->NoJump);
    }

    void TfGalaxy2::ClearJumpSelectionCallbacks() {
        ClearJumpPath();
        MapPanel->LeftButtonDownCallback = nullptr;
        JumpButton->DownCallback = nullptr;
        MapPanel->MouseMoveCallback = nullptr;
        MapPanel->LeftButtonDoubleClickCallback = nullptr;
    }

    void TfGalaxy2::MapMouseMove(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (Sender->IsOccludedAtPoint(Point)) {
            return;
        }
    }

    void TfGalaxy2::MapLeftButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aGalaxy::TStar* Star{};
        EC_Struct::TPointF GalaxyPoint{};
        double Distance{};
        std::int32_t I{};
        if (!GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU)) {
            MapRightButtonDown(Sender, KeyState, Point);
        }
        if (Sender->IsOccludedAtPoint(Point)) {
            return;
        }
        Point = MapPanel->ToLocalPoint(Point);
        GalaxyPoint = MapPointToGalaxyPoint(Point);
        aGalaxy::TStar* Selected = nullptr;
        double BestDistance = 1.0E+20;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (!Star->IsConstellationVisible()) {
                continue;
            }
            Distance = (static_cast<long double>(Star->Position.X) - GalaxyPoint.X) * (static_cast<long double>(Star->Position.X) - GalaxyPoint.X) + (static_cast<long double>(Star->Position.Y) - GalaxyPoint.Y) * (static_cast<long double>(Star->Position.Y) - GalaxyPoint.Y);
            if (Distance < BestDistance) {
                Selected = Star;
                BestDistance = Distance;
            }
        }
        if (SelectedJumpStar != nullptr && aPlayer::GetPlayer()->CurrentStar != SelectedJumpStar && (CreateMarkerMode || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_CONTROL) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_SHIFT) || GR_Main::IsVirtualKeyDown(WindowsSdk::VK_MENU))) {
            if (BestDistance < aConst::GalaxySizeY * 0.1L * (aConst::GalaxySizeY * 0.1L)) {
                if (pas::list_count(RouteStars) < 1 || pas::list_get(RouteStars, pas::list_count(RouteStars) - 1) != Selected) {
                    pas::list_add(RouteStars, reinterpret_cast<void*>(Selected));
                    RebuildJumpPath();
                } else if (pas::list_count(RouteStars) > 0 && pas::list_get(RouteStars, pas::list_count(RouteStars) - 1) == Selected) {
                    pas::list_delete(RouteStars, pas::list_count(RouteStars) - 1);
                    RebuildJumpPath();
                }
            }
        } else {
            pas::list_clear(RouteStars);
            if (BestDistance < aConst::GalaxySizeY * 0.1L * (aConst::GalaxySizeY * 0.1L)) {
                SelectedJumpStar = Selected;
            } else {
                SelectedJumpStar = aPlayer::GetPlayer()->CurrentStar;
            }
            RebuildJumpPath();
            JumpButton->SetDisabled(SelectedJumpStar == nullptr || aPlayer::GetPlayer()->CurrentStar == SelectedJumpStar || ViewMode == 1 || aPlayer::GetPlayer()->NoJump);
        }
    }

    void TfGalaxy2::MapRightButtonDown(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        aGalaxy::TStar* Star{};
        EC_Struct::TPointF GalaxyPoint{};
        double Distance{};
        std::int32_t I{};
        if (Sender->IsOccludedAtPoint(Point)) {
            return;
        }
        Point = MapPanel->ToLocalPoint(Point);
        GalaxyPoint = MapPointToGalaxyPoint(Point);
        double BestDistance = 1.0E+20;
        aGalaxy::TStar* Selected = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(I); ) {
            Star = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, I);
            if (!Star->IsConstellationVisible()) {
                continue;
            }
            Distance = (static_cast<long double>(Star->Position.X) - GalaxyPoint.X) * (static_cast<long double>(Star->Position.X) - GalaxyPoint.X) + (static_cast<long double>(Star->Position.Y) - GalaxyPoint.Y) * (static_cast<long double>(Star->Position.Y) - GalaxyPoint.Y);
            if (Distance < BestDistance) {
                Selected = Star;
                BestDistance = Distance;
            }
        }
        if (BestDistance >= aConst::GalaxySizeY * 0.1L * (aConst::GalaxySizeY * 0.1L)) {
            Selected = nullptr;
        }
        ShowStarInfo(Selected);
    }

    void TfGalaxy2::MapButtonUp(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (StarInfoHideTimer != nullptr) {
            CancelCallbackTimer(StarInfoHideTimer);
            StarInfoHideTimer = nullptr;
        }
        StarInfoHideTimer = ScheduleCallbackTimer(50, 50, pas::bind_method<&TfGalaxy2::HideStarInfo>(this), 0);
    }

    void TfGalaxy2::MapDoubleClick(GI_MessageLoop::TObjectGI* Sender, std::uint32_t KeyState, WindowsSdk::TPoint Point) {
        if (pas::in_set<0, 0, 2, 3>(ViewMode)) {
            if (!aPlayer::GetPlayer()->NoJump) {
                JumpClicked(nullptr);
            }
        }
    }

    void TfGalaxy2::HideStarInfo(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        ShowStarInfo(nullptr);
    }

    void TfGalaxy2::JumpClicked(GI_MessageLoop::TObjectGI* Sender) {
        EC_Struct::TPointF First{};
        EC_Struct::TPointF Second{};
        aGalaxy::THole* Hole{};
        double Angle{};
        double Radius{};
        aGalaxyEvent::TGalaxyEvent* Event{};
        if (aPlayer::GetPlayer()->CurrentStar == SelectedJumpStar || SelectedJumpStar == nullptr) {
            return;
        }
        if (ViewMode == 2) {
            if (aPlayer::GetPlayer()->Speed <= 0) {
                {
                    const pas::WideString& localizedColorText = aConst::LocalizedColorText(u"Help.Speed0"_wref.get());
                    GI_MessageLoop::TMessageLoopGI* self = this;
                    GI_MessageBox::ShowMessageBoxGI(self, localizedColorText, GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
                }
                return;
            }
            if (!aPlayer::GetPlayer()->HasPositiveSpeed()) {
                return;
            }
            First = SelectedJumpStar->Position;
            Second = aPlayer::GetPlayer()->CurrentStar->Position;
            aGalaxy::Galaxy->CheckIntegrityChecksum(149);
            if (aPlayer::GetPlayer()->JumpRange < System::Round(System::Sqrt(pas::sqr(static_cast<long double>(First.X) - Second.X) + pas::sqr(static_cast<long double>(First.Y) - Second.Y)))) {
                aPlayer::GetPlayer()->OrderNone(false);
                aGalaxy::Galaxy->PrimeIntegrityChecksum(152);
                {
                    const pas::WideString& formatText1 = ([&] {
                        auto name = pas::borrow(SelectedJumpStar->Name);
                        pas::WideString localizedText = aConst::LocalizedText(u"FormGalaxy.NeedFuelOrEngine"_wref.get());
                        return aMyFunction::FormatText1(std::move(localizedText), u"<color=255,240,100>"_w, u"<Star>"_w, name.get());
                    }());
                    GI_MessageLoop::TMessageLoopGI* self_2 = this;
                    GI_MessageBox::ShowMessageBoxGI(self_2, formatText1, GI_MessageBox::mbgCancel | GI_MessageBox::mbgError, 0, 0, 0);
                }
                if (Sender != nullptr) {
                    GI_Main::BreakUiMessage();
                }
                return;
            } else {
                aPlayer::GetPlayer()->OrderJump(SelectedJumpStar, false);
            }
        } else if (ViewMode == 3) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(153);
            Hole = pas::construct_call<aGalaxy::THole>(aGalaxy::THole_Create);
            Hole->InitializeGraphic(pas::WideString());
            reinterpret_cast<SE_Hole::THoleSE*>(Hole->Graphic)->SetState(1);
            Hole->Star1 = aPlayer::GetPlayer()->CurrentStar;
            Angle = Math::ArcTan2(aPlayer::GetPlayer()->Position.X, -aPlayer::GetPlayer()->Position.Y);
            Radius = pas::real_max<pas::Extended>(aPlayer::GetPlayer()->CurrentStar->SafeRadius + 1.0E+2L, System::Sqrt(aMyFunction::PointDistanceSquared(aPlayer::GetPlayer()->Position, EC_Struct::MakePointF(0.0f, 0.0f))) + 2.0E+2L);
            {
                float cpp_arg = System::Sin(Angle) * Radius;
                float cpp_arg_2 = -System::Cos(Angle) * Radius;
                Hole->Position1 = EC_Struct::MakePointF(cpp_arg, cpp_arg_2);
            }
            Hole->Star2 = SelectedJumpStar;
            Angle = aMyFunction::HeadingDegreesToRadians(aMyFunction::SeededRandomIntRange(0, 359, aGalaxy::Galaxy->RandomState));
            Radius = aMyFunction::SeededRandomIntRange(1000, 2000, aGalaxy::Galaxy->RandomState);
            {
                float cpp_arg_3 = System::Sin(Angle) * Radius;
                float cpp_arg_4 = -System::Cos(Angle) * Radius;
                Hole->Position2 = EC_Struct::MakePointF(cpp_arg_3, cpp_arg_4);
            }
            Hole->CreatedTurn = aGalaxy::Galaxy->CurrentTurn - 190;
            Hole->HoleType = 1;
            Globals::StarMapScreen->PendingHoleRefresh = Hole;
            pas::list_add(aGalaxy::Galaxy->Holes, reinterpret_cast<void*>(Hole));
            Hole->ArcadeMapName = u"SkipAB"_w;
            aPlayer::GetPlayer()->OrderJumpHole(Hole, false);
            aPlayer::GetPlayer()->GetHull()->Energy = std::max<std::int32_t>(0, aPlayer::GetPlayer()->GetHull()->Energy - 600);
            Event = aGalaxyEvent::AddGalaxyEvent(u"PlayerCreatedStarDestroyerBH"_w, nullptr);
            Event->AddData(Hole->Id);
            Event->AddData(Hole->CreatedTurn);
        } else if (ViewMode == 0) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(153);
            aPlayer::GetPlayer()->OrderTeleport(SelectedJumpStar, aPlayer::GetPlayer()->Position, 10, false);
        } else {
            aGalaxy::Galaxy->CheckIntegrityChecksum(152);
        }
        aRanger::PendingPlayerFollowTarget = nullptr;
        aGalaxy::Galaxy->PrimeIntegrityChecksum(154);
        Globals::SpaceViewPosition = aPlayer::GetPlayer()->Position;
        GlobalsV::RequestedScreenId = GlobalsV::screenStarMap;
        RequestClose(1);
    }

    void TfGalaxy2::JumpMouseEnter(GI_MessageLoop::TObjectGI* Sender) {
        EC_Struct::TPointF First{};
        EC_Struct::TPointF Second{};
        if (ViewMode == 0) {
            return;
        }
        if (ViewMode == 2) {
            if (aPlayer::GetPlayer()->CurrentStar == SelectedJumpStar || SelectedJumpStar == nullptr) {
                return;
            }
            if (!aPlayer::GetPlayer()->HasPositiveSpeed()) {
                return;
            }
            First = SelectedJumpStar->Position;
            Second = aPlayer::GetPlayer()->CurrentStar->Position;
            if (aPlayer::GetPlayer()->JumpRange < System::Round(System::Sqrt(pas::sqr(static_cast<long double>(First.X) - Second.X) + pas::sqr(static_cast<long double>(First.Y) - Second.Y)))) {
                return;
            }
        }
        {
            const pas::WideString& replaceAllWideString = ([&] {
                const pas::WideString& localizedText = aConst::LocalizedText(u"FormGalaxy.JumpTo"_wref.get());
                const pas::WideString& name = SelectedJumpStar->Name;
                return EC_Str::ReplaceAllWideString(localizedText, u"<Name>"_wref.get(), name);
            }());
            GI_Label::TLabelGI* jumpDestinationLabel = JumpDestinationLabel;
            jumpDestinationLabel->SetText(replaceAllWideString);
        }
        if (JumpHintAnimationState == 0) {
            JumpHintAnimationState = 1;
        } else if (JumpHintAnimationState == 4) {
            JumpHintAnimationState = 3;
        } else if (JumpHintAnimationState == 5) {
            JumpHintAnimationState = 2;
        }
    }

    void TfGalaxy2::JumpMouseLeave(GI_MessageLoop::TObjectGI* Sender) {
        if (JumpHintAnimationState == 1) {
            JumpHintAnimationState = 0;
        } else if (JumpHintAnimationState == 2) {
            JumpHintAnimationState = 5;
        } else if (JumpHintAnimationState == 3) {
            JumpHintAnimationState = 4;
        }
    }

    // Nil hides the panel; cancels StarInfoHideTimer.
    void TfGalaxy2::ShowStarInfo(aGalaxy::TStar* Star) {
        pas::List* Objects{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t IconX{};
        std::uint32_t IconInset{};
        std::int32_t SummaryLines{};
        float ObjectDistance{};
        std::uint8_t OwnerId{};
        aPlanet::TPlanet* Planet{};
        aGalaxy::TCustomSystemInfo* CustomInfo{};
        pas::WideString Value{};
        if (Star != nullptr && aPlayer::GetPlayer() != nullptr) {
            aPlayer::GetPlayer()->ScriptItemsAct(aConst::satOnShowingStarInfo, Star, nullptr, 0);
        }
        if (StarInfoHideTimer != nullptr) {
            CancelCallbackTimer(StarInfoHideTimer);
            StarInfoHideTimer = nullptr;
        }
        GI_Window::TWindowGI* InfoPanel = reinterpret_cast<GI_Window::TWindowGI*>(GetByName(u"InfoStar"_wref.get()));
        if (Star == nullptr) {
            InfoPanel->SetActive(false);
            return;
        }
        InfoPanel->SetActive(true);
        {
            GI_GraphBuf::TGraphBufGI* InfoStarImage = pas::checked_cast<GI_GraphBuf::TGraphBufGI*>(GetByName(u"InfoStarImage"_wref.get()));
            InfoStarImage->SourceHasPerPixelAlpha = true;
            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(reinterpret_cast<SE_Star::TStarSE*>(Star->Graphic)->StaticImagePath, 1, u","_wref.get()), InfoStarImage->GraphBuf);
            if (static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width) >= static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) {
                InfoStarImage->GraphBuf->RescaleRgba(InfoStarImage->ClientSize.X, System::Round(pas::real_divide(InfoStarImage->ClientSize.X, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)), 5);
            } else {
                InfoStarImage->GraphBuf->RescaleRgba(System::Round(pas::real_divide(InfoStarImage->ClientSize.Y, static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Height)) * static_cast<std::uint32_t>(InfoStarImage->GraphBuf->Width)), InfoStarImage->ClientSize.Y, 5);
            }
            InfoStarImage->SetImageKindX(GI_Main::ikxCenter);
            InfoStarImage->SetImageKindY(GI_Main::ikyCenter);
        }
        GI_Panel::TPanelGI* Owner = pas::checked_cast<GI_Panel::TPanelGI*>(GetByName(u"InfoStarPanel"_wref.get()));
        Owner->FreeOwnedChildren();
        Owner->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - InfoPanel->WorkSubRect.Left - InfoPanel->WorkSubRect.Right, Owner->ClientSize.Y));
        Objects = pas::make_object<pas::List>();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range.next(I); ) {
            pas::list_add(Objects, pas::list_get(Star->Planets, I));
        }
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_2.next(I); ) {
            if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Star->Ships, I)) != nullptr) {
                if (pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(Star->Ships, I))->InNormalSpace()) {
                    if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(Star->Ships, I))->Graphic) != nullptr || pas::class_cast_if<SE_Ship2::TShip2SE*>(pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(Star->Ships, I))->Graphic) != nullptr && pas::checked_cast<SE_Ship2::TShip2SE*>(pas::checked_cast<aRuins::TRuins*>(pas::list_at<pas::Object>(Star->Ships, I))->Graphic)->AlternateImagePath != u"") {
                        ObjectDistance = aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(Star->Ships, I)->Position, EC_Struct::MakePointF(0.0f, 0.0f));
                        J = 0;
                        while (J < pas::list_count(Objects)) {
                            if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, J)) != nullptr) {
                                if (aMyFunction::PointDistanceSquared(pas::list_at<aPlanet::TPlanet>(Objects, J)->GetPosition(), EC_Struct::MakePointF(0.0f, 0.0f)) > ObjectDistance) {
                                    break;
                                }
                            } else if (aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(Objects, J)->Position, EC_Struct::MakePointF(0.0f, 0.0f)) > ObjectDistance) {
                                break;
                            }
                            ++J;
                        }
                        pas::list_insert(Objects, J, pas::list_get(Star->Ships, I));
                    }
                }
            }
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->CustomSystemInfos) - 1); cpp_range_3.next(I); ) {
            CustomInfo = pas::list_at<aGalaxy::TCustomSystemInfo>(Star->CustomSystemInfos, I);
            ObjectDistance = pas::sqr(CustomInfo->Distance);
            J = 0;
            while (J < pas::list_count(Objects)) {
                if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, J)) != nullptr) {
                    if (aMyFunction::PointDistanceSquared(pas::list_at<aPlanet::TPlanet>(Objects, J)->GetPosition(), EC_Struct::MakePointF(0.0f, 0.0f)) > ObjectDistance) {
                        break;
                    }
                } else if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, J)) != nullptr) {
                    if (aMyFunction::PointDistanceSquared(pas::list_at<aShip::TShip>(Objects, J)->Position, EC_Struct::MakePointF(0.0f, 0.0f)) > ObjectDistance) {
                        break;
                    }
                } else if (static_cast<long double>(pas::sqr(pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, J)->Distance)) > ObjectDistance) {
                    break;
                }
                ++J;
            }
            pas::list_insert(Objects, J, reinterpret_cast<void*>(CustomInfo));
        }
        std::int32_t RowHeight = GR_Main::GiScalePixels(20);
        std::int32_t NameWidth = GR_Main::GiScalePixels(100);
        std::int32_t DetailWidth = GR_Main::GiScalePixels(100);
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range_4.next(I); ) {
            GI_Label::TLabelGI* cpp_with_2 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
            cpp_with_2->SetFontName(GlobalsV::NormalFontName);
            cpp_with_2->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
            cpp_with_2->SetSize(ClassesImports::Point(1, RowHeight));
            cpp_with_2->SetPosition(ClassesImports::Point(0, RowHeight * I));
            cpp_with_2->SetWordWrapEnabled(false);
            cpp_with_2->SetTextAlignX(GI_Main::taxAuto);
            cpp_with_2->SetTextAlignY(GI_Main::tayCenterEx);
            if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                cpp_with_2->SetText(pas::list_at<aPlanet::TPlanet>(Objects, I)->Name);
            } else if (pas::class_cast_if<aShip::TShip*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                cpp_with_2->SetText(pas::list_at<aShip::TShip>(Objects, I)->Name);
            } else {
                cpp_with_2->SetText(pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I)->Name);
            }
            NameWidth = std::max<std::int32_t>(NameWidth, cpp_with_2->ClientSize.X);
        }
        GI_MessageLoop::TObjectGI* CurrentChild = Owner->FirstChild;
        while (CurrentChild != nullptr) {
            if (pas::class_cast_if<GI_Label::TLabelGI*>(CurrentChild) != nullptr) {
                GI_Label::TLabelGI* cpp_with_3 = pas::checked_cast<GI_Label::TLabelGI*>(CurrentChild);
                cpp_with_3->SetTextAlignX(GI_Main::taxRight);
                cpp_with_3->SetSize(ClassesImports::Point(NameWidth, RowHeight));
            }
            CurrentChild = CurrentChild->NextSibling;
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Objects) - 1); cpp_range_5.next(I); ) {
            {
                GI_GraphBuf::TGraphBufGI* cpp_with_4 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Owner, false);
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
                cpp_with_4->SourceHasPerPixelAlpha = true;
                cpp_with_4->SetPosition(ClassesImports::Point(NameWidth + 5 + 1 + (IconInset >> 1), RowHeight * I + 1 + (IconInset >> 1)));
                cpp_with_4->SetSize(ClassesImports::Point(RowHeight - 2 - IconInset, RowHeight - 2 - IconInset));
                if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                    pas::list_at<aPlanet::TPlanet>(Objects, I)->Graphic->RenderToBuffer(this, cpp_with_4->GraphBuf, true);
                    if (static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) {
                        cpp_with_4->GraphBuf->RescaleRgba(cpp_with_4->ClientSize.X, System::Round(pas::real_divide(cpp_with_4->ClientSize.X, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)), 5);
                    } else {
                        cpp_with_4->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_4->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)), cpp_with_4->ClientSize.Y, 5);
                    }
                } else if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                    if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(pas::list_at<aRuins::TRuins>(Objects, I)->Graphic) != nullptr) {
                        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ruins::TRuinsSE*>(pas::list_at<aRuins::TRuins>(Objects, I)->Graphic)->StaticImagePath, 1, u","_wref.get()), cpp_with_4->GraphBuf);
                    } else {
                        GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::checked_cast<SE_Ship2::TShip2SE*>(pas::list_at<aRuins::TRuins>(Objects, I)->Graphic)->AlternateImagePath, 1, u","_wref.get()), cpp_with_4->GraphBuf);
                    }
                    if (static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) {
                        cpp_with_4->GraphBuf->RescaleRgba(cpp_with_4->ClientSize.X, System::Round(pas::real_divide(cpp_with_4->ClientSize.X, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)), 5);
                    } else {
                        cpp_with_4->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_4->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)), cpp_with_4->ClientSize.Y, 5);
                    }
                } else if (pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I)->Icon != u"") {
                    GI_GI::LoadGiByPathIntoGraphBuf(pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I)->Icon, cpp_with_4->GraphBuf);
                    if (static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) {
                        cpp_with_4->GraphBuf->RescaleRgba(cpp_with_4->ClientSize.X, System::Round(pas::real_divide(cpp_with_4->ClientSize.X, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)), 5);
                    } else {
                        cpp_with_4->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_4->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_4->GraphBuf->Width)), cpp_with_4->ClientSize.Y, 5);
                    }
                }
                cpp_with_4->SetImageKindX(GI_Main::ikxCenter);
                cpp_with_4->SetImageKindY(GI_Main::ikyCenter);
            }
            if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                OwnerId = pas::list_at<aPlanet::TPlanet>(Objects, I)->OwnerId;
            } else if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                OwnerId = pas::list_at<aRuins::TRuins>(Objects, I)->OwnerId;
            } else {
                OwnerId = static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited);
            }
            if (pas::class_cast_if<aRuins::TRuins*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                GI_Label::TLabelGI* cpp_with_5 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                if (GR_Main::GiResourceVariant() == 2) {
                    cpp_with_5->SetFontName(GlobalsV::MiniFontName);
                } else {
                    cpp_with_5->SetFontName(GlobalsV::SmallFontName);
                }
                cpp_with_5->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                cpp_with_5->SetSize(ClassesImports::Point(1, RowHeight));
                cpp_with_5->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                cpp_with_5->SetWordWrapEnabled(false);
                cpp_with_5->SetTextAlignX(GI_Main::taxAuto);
                cpp_with_5->SetTextAlignY(GI_Main::tayCenterEx);
                cpp_with_5->SetText(EC_Str::LowerCaseWideString(pas::list_at<aShip::TShip>(Objects, I)->GetLocalizedTypeName()));
                DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_5->ClientSize.X + GR_Main::GiScalePixels(35));
            } else if (pas::class_cast_if<aGalaxy::TCustomSystemInfo*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                CustomInfo = pas::list_at<aGalaxy::TCustomSystemInfo>(Objects, I);
                if (EC_Str::CountDelimitedPartsW(CustomInfo->Info, u":"_wref.get()) > 1 && EC_Str::ExtractDelimitedPartW(CustomInfo->Info, 0, u":"_wref.get()) == u"Image") {
                    Value = EC_Str::ExtractDelimitedPartW(CustomInfo->Info, 1, u":"_wref.get());
                    IconX = NameWidth + 5 + RowHeight + 5 + 1;
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Value, u","_wref.get()) - 1); cpp_range_6.next(J); ) {
                        GI_Image::TImageGI* cpp_with_6 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
                        cpp_with_6->SetImagePath(pas::concat_wide({u"GI,", EC_Str::ExtractDelimitedPartW(Value, J, u","_wref.get())}));
                        cpp_with_6->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                        cpp_with_6->SetPosition(ClassesImports::Point(IconX, RowHeight * I + 1));
                        IconX = IconX + RowHeight + 2;
                    }
                } else if (EC_Str::CountDelimitedPartsW(CustomInfo->Info, u":"_wref.get()) > 1 && EC_Str::ExtractDelimitedPartW(CustomInfo->Info, 0, u":"_wref.get()) == u"RGBA") {
                    Value = EC_Str::ExtractDelimitedPartW(CustomInfo->Info, 1, u":"_wref.get());
                    IconX = NameWidth + 5 + RowHeight + 5 + 1;
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, EC_Str::CountDelimitedPartsW(Value, u","_wref.get()) - 1); cpp_range_7.next(J); ) {
                        GI_GraphBuf::TGraphBufGI* cpp_with_7 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Owner, false);
                        cpp_with_7->SourceHasPerPixelAlpha = true;
                        cpp_with_7->LoadBitmapPathAsRgba(pas::concat_wide({EC_Str::ExtractDelimitedPartW(Value, J, u","_wref.get()), u"?RGBA"}));
                        cpp_with_7->SetPosition(ClassesImports::Point(IconX, RowHeight * I + 1));
                        cpp_with_7->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                        if (cpp_with_7->ClientSize.X < cpp_with_7->GraphBuf->Width || cpp_with_7->ClientSize.Y < cpp_with_7->GraphBuf->Height) {
                            if (static_cast<std::uint32_t>(cpp_with_7->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_7->GraphBuf->Height)) {
                                cpp_with_7->GraphBuf->RescaleRgba(cpp_with_7->ClientSize.X, System::Round(pas::real_divide(cpp_with_7->ClientSize.X, static_cast<std::uint32_t>(cpp_with_7->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_7->GraphBuf->Height)), 5);
                            } else {
                                cpp_with_7->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_7->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_7->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_7->GraphBuf->Width)), cpp_with_7->ClientSize.Y, 5);
                            }
                        }
                        cpp_with_7->SetImageKindX(GI_Main::ikxCenter);
                        cpp_with_7->SetImageKindY(GI_Main::ikyCenter);
                        IconX = IconX + RowHeight + 2;
                    }
                } else {
                    GI_Label::TLabelGI* cpp_with_8 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                    if (GR_Main::GiResourceVariant() == 2) {
                        cpp_with_8->SetFontName(GlobalsV::MiniFontName);
                    } else {
                        cpp_with_8->SetFontName(GlobalsV::SmallFontName);
                    }
                    cpp_with_8->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                    cpp_with_8->SetSize(ClassesImports::Point(1, RowHeight));
                    cpp_with_8->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                    cpp_with_8->SetWordWrapEnabled(false);
                    cpp_with_8->SetTextAlignX(GI_Main::taxAuto);
                    cpp_with_8->SetTextAlignY(GI_Main::tayCenterEx);
                    cpp_with_8->SetText(CustomInfo->Info);
                    DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_8->ClientSize.X + GR_Main::GiScalePixels(35));
                }
            } else if (OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                if (!(pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) || static_cast<std::uint8_t>(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->IsMainPiratePlanet ^ 1)) {
                    GI_GraphBuf::TGraphBufGI* cpp_with_9 = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, Owner, false);
                    cpp_with_9->SourceHasPerPixelAlpha = true;
                    cpp_with_9->LoadBitmapPathAsRgba(pas::concat_wide({EC_Str::ExtractDelimitedPartW(aConst::GetFactionEmblemPath(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->GetFactionResourceName()), 1, u","_wref.get()), u"?RGBA"}));
                    cpp_with_9->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I + 1));
                    cpp_with_9->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                    if (static_cast<std::uint32_t>(cpp_with_9->GraphBuf->Width) >= static_cast<std::uint32_t>(cpp_with_9->GraphBuf->Height)) {
                        cpp_with_9->GraphBuf->RescaleRgba(cpp_with_9->ClientSize.X, System::Round(pas::real_divide(cpp_with_9->ClientSize.X, static_cast<std::uint32_t>(cpp_with_9->GraphBuf->Width)) * static_cast<std::uint32_t>(cpp_with_9->GraphBuf->Height)), 5);
                    } else {
                        cpp_with_9->GraphBuf->RescaleRgba(System::Round(pas::real_divide(cpp_with_9->ClientSize.Y, static_cast<std::uint32_t>(cpp_with_9->GraphBuf->Height)) * static_cast<std::uint32_t>(cpp_with_9->GraphBuf->Width)), cpp_with_9->ClientSize.Y, 5);
                    }
                    cpp_with_9->SetImageKindX(GI_Main::ikxCenter);
                    cpp_with_9->SetImageKindY(GI_Main::ikyCenter);
                }
            }
            if (pas::class_cast_if<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I)) != nullptr) {
                Planet = pas::list_at<aPlanet::TPlanet>(Objects, I);
                if (pas::in_set<0, 4, 7, 7>(Planet->OwnerId) && static_cast<std::uint8_t>(Planet->IsMainPiratePlanet ^ 1) && Planet->CurrentStar->Status.CustomFaction == u"") {
                    IconX = NameWidth + 5 + RowHeight + 5 + 1;
                    {
                        GI_Image::TImageGI* cpp_with_10 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
                        switch (static_cast<std::int32_t>(pas::checked_cast<aPlanet::TPlanet*>(pas::list_at<pas::Object>(Objects, I))->GetRelationLevelToShip(aPlayer::GetPlayer())) & 0x0000007f) {
                            case 0: cpp_with_10->SetImagePath(u"GI,Bm.FormGalaxy2.Face4"_w); break;
                            case 1: cpp_with_10->SetImagePath(u"GI,Bm.FormGalaxy2.Face3"_w); break;
                            case 2: cpp_with_10->SetImagePath(u"GI,Bm.FormGalaxy2.Face2"_w); break;
                            case 3: cpp_with_10->SetImagePath(u"GI,Bm.FormGalaxy2.Face1"_w); break;
                            case 4: cpp_with_10->SetImagePath(u"GI,Bm.FormGalaxy2.Face0"_w); break;
                            default: cpp_with_10->SetImagePath(u"GI,Bm.FormGalaxy2.Face2"_w); break;
                        }
                        cpp_with_10->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                        cpp_with_10->SetPosition(ClassesImports::Point(IconX + RowHeight + 2, RowHeight * I + 1));
                    }
                    IconX = IconX + RowHeight + 2;
                    if (pas::is_one_of<aGalaxyStruct::peAgricultural, aGalaxyStruct::peIndustrial>(Planet->Economy)) {
                        GI_Image::TImageGI* cpp_with_11 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
                        switch (Planet->Economy) {
                            case aGalaxyStruct::peAgricultural: {
                                cpp_with_11->SetImagePath(u"GI,Bm.FormGalaxy.EconAgrar"_w);
                                break;
                            }
                            case aGalaxyStruct::peIndustrial: {
                                cpp_with_11->SetImagePath(u"GI,Bm.FormGalaxy.EconIndustr"_w);
                                break;
                            }
                        }
                        cpp_with_11->SetSize(ClassesImports::Point(RowHeight - 2, RowHeight - 2));
                        cpp_with_11->SetPosition(ClassesImports::Point(IconX + RowHeight, RowHeight * I + 1));
                    }
                } else if (Planet->IsMainPiratePlanet) {
                    GI_Label::TLabelGI* cpp_with_12 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                    if (GR_Main::GiResourceVariant() == 2) {
                        cpp_with_12->SetFontName(GlobalsV::MiniFontName);
                    } else {
                        cpp_with_12->SetFontName(GlobalsV::SmallFontName);
                    }
                    cpp_with_12->SetTextColor(GR_Main::GetStyleColorGI(u"StarInfoObjectType"_w, 40, 237, 245));
                    cpp_with_12->SetSize(ClassesImports::Point(1, RowHeight));
                    cpp_with_12->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                    cpp_with_12->SetWordWrapEnabled(false);
                    cpp_with_12->SetTextAlignX(GI_Main::taxAuto);
                    cpp_with_12->SetTextAlignY(GI_Main::tayCenterEx);
                    cpp_with_12->SetText(EC_Str::LowerCaseWideString(aConst::LocalizedText(u"ShipType.TypeName.PB"_wref.get())));
                    DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_12->ClientSize.X + GR_Main::GiScalePixels(35));
                } else if (Planet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited) && Planet->GetUnexploredSurfaceTileCount() == 0) {
                    GI_Label::TLabelGI* cpp_with_13 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                    if (GR_Main::GiResourceVariant() == 2) {
                        cpp_with_13->SetFontName(GlobalsV::MiniFontName);
                    } else {
                        cpp_with_13->SetFontName(GlobalsV::SmallFontName);
                    }
                    cpp_with_13->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(140, 140, 140));
                    cpp_with_13->SetSize(ClassesImports::Point(1, RowHeight));
                    cpp_with_13->SetPosition(ClassesImports::Point(NameWidth + 5 + RowHeight + 5 + 1, RowHeight * I));
                    cpp_with_13->SetWordWrapEnabled(false);
                    cpp_with_13->SetTextAlignX(GI_Main::taxAuto);
                    cpp_with_13->SetTextAlignY(GI_Main::tayCenterEx);
                    cpp_with_13->SetText(EC_Str::LowerCaseWideString(aConst::LocalizedText(u"Planet.NotCivil.AllExplore"_wref.get())));
                    DetailWidth = std::max<std::int32_t>(DetailWidth, cpp_with_13->ClientSize.X + GR_Main::GiScalePixels(35));
                }
            }
        }
        std::int32_t RowCount = pas::list_count(Objects);
        if (CanShowExtendedRadarInfo(Star)) {
            {
                GI_Label::TLabelGI* cpp_with_14 = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Owner);
                cpp_with_14->SetFontName(GlobalsV::SmallFontName);
                cpp_with_14->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255));
                cpp_with_14->SetPosition(ClassesImports::Point(0, RowCount * RowHeight));
                cpp_with_14->SetWordWrapEnabled(false);
                cpp_with_14->SetTextAlignX(GI_Main::taxCenter);
                cpp_with_14->SetTextAlignY(GI_Main::tayCenterEx);
                cpp_with_14->SetText(BuildStarShipSummary(Star, SummaryLines));
                cpp_with_14->SetSize(ClassesImports::Point(NameWidth + DetailWidth, RowHeight * SummaryLines));
            }
            RowCount += SummaryLines;
        }
        Owner->SetSize(ClassesImports::Point(NameWidth + DetailWidth, RowCount * RowHeight));
        Owner->SetPosition(pas::load_unaligned<WindowsSdk::TPoint>(pas::byte_offset(&InfoPanel->WorkSubRect, 0)));
        InfoPanel->SetSize(ClassesImports::Point(Owner->ClientSize.X + InfoPanel->WorkSubRect.Left + InfoPanel->WorkSubRect.Right, InfoPanel->WorkSubRect.Top + InfoPanel->WorkSubRect.Bottom + RowCount * RowHeight));
        InfoPanel->UpdateAutoGeometry();
        {
            GI_Label::TLabelGI* InfoStarName = pas::checked_cast<GI_Label::TLabelGI*>(GetByName(u"InfoStarName"_wref.get()));
            InfoStarName->SetText(aMyFunction::WrapTextInColor(Star->Name, aMyFunction::InfoNameColorTag));
            InfoStarName->SetSize(ClassesImports::Point(InfoPanel->ClientSize.X - InfoPanel->WorkSubRect.Right - InfoStarName->LocalPosition.X - 15, InfoStarName->ClientSize.Y));
        }
        pas::free(Objects);
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, 3); cpp_range_8.next(I); ) {
            if (I == 0) {
                InfoPanel->SetPosition(EC_Struct::AddPoints(MapPanel->ToAbsolutePoint(GalaxyPointToMapPoint(Star->Position)), ClassesImports::Point(-InfoPanel->ClientSize.X - GR_Main::GiScalePixels(50), -InfoPanel->ClientSize.Y - GR_Main::GiScalePixels(50))));
            } else if (I == 1) {
                InfoPanel->SetPosition(EC_Struct::AddPoints(MapPanel->ToAbsolutePoint(GalaxyPointToMapPoint(Star->Position)), ClassesImports::Point(GR_Main::GiScalePixels(50), -InfoPanel->ClientSize.Y - GR_Main::GiScalePixels(50))));
            } else if (I == 2) {
                InfoPanel->SetPosition(EC_Struct::AddPoints(MapPanel->ToAbsolutePoint(GalaxyPointToMapPoint(Star->Position)), ClassesImports::Point(GR_Main::GiScalePixels(50), GR_Main::GiScalePixels(50))));
            } else {
                InfoPanel->SetPosition(EC_Struct::AddPoints(MapPanel->ToAbsolutePoint(GalaxyPointToMapPoint(Star->Position)), ClassesImports::Point(-InfoPanel->ClientSize.X - GR_Main::GiScalePixels(50), GR_Main::GiScalePixels(50))));
            }
            if (InfoPanel->LocalPosition.X >= 0 && InfoPanel->LocalPosition.Y >= 0 && GR_Main::GameScreenWidth - GR_Main::GiScalePixels(100) >= InfoPanel->LocalPosition.X + InfoPanel->ClientSize.X && GR_Main::GameScreenHeight - GR_Main::GiScalePixels(100) >= InfoPanel->LocalPosition.Y + InfoPanel->ClientSize.Y) {
                break;
            }
        }
    }

    // Requires boss-specific scanner technology; does not test general visibility.
    std::uint8_t TfGalaxy2::CanRevealBossPresence(aShip::TShip* Ship) {
        if (Ship == nullptr) {
            return false;
        }
        if (aPlayer::GetPlayer()->GetScanner() == nullptr || aPlayer::GetPlayer()->GetScanner()->BrokenFlag != 0) {
            return false;
        }
        if (Ship->TypeId != aGalaxyStruct::stKling) {
            return true;
        }
        std::int32_t TechLevel = aPlayer::GetPlayer()->GetScanner()->TechLevel;
        std::uint8_t Result = true;
        if (aKling::BlazerShip == Ship && TechLevel < 7) {
            return false;
        }
        if (aKling::KellerShip == Ship && TechLevel < 5) {
            return false;
        }
        if (aKling::TerronShip == Ship && TechLevel < 3) {
            return false;
        }
        return Result;
    }

    pas::WideString TfGalaxy2::BuildStarShipSummary(aGalaxy::TStar* Star, std::int32_t& LineCount) {
        aShip::TShip* Ship{};
        std::int32_t UnknownCount{};
        std::int32_t OtherFactionCount{};
        pas::WideString OtherFaction{};
        pas::WideString StarFaction{};
        std::int32_t StarFactionCount{};
        aKling::TKling* Kling{};
        pas::Array<pas::Array<std::int32_t, 0, 7>, 0, 2> DominatorCounts{};
        std::int32_t ScriptedPirates{};
        std::int32_t ScriptedCoalition{};
        pas::Array<std::int32_t, 0, 13> RoleCounts{};
        aTransport::TTransport* Transport{};
        std::int32_t CoalitionTranclucators{};
        std::int32_t PirateTranclucators{};
        std::int32_t CoalitionStations{};
        std::int32_t PirateStations{};
        aGalaxyStruct::TDominatorSeries Series{};
        std::uint8_t LineActive{};
        pas::WideString Summary{};
        pas::WideString Line{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t GroupIndex{};
        aPlanet::TPlanet* Planet{};
        aGalaxyStruct::TKlingType Kind{};
        std::uint8_t Role{};
        std::int32_t PirateRole{};
        pas::WideString ColorTag{};
        auto AccumulateShip = [&]() -> void {
            pas::WideString Faction{};
            if (Ship->InHyperspace) {
                return;
            }
            if (static_cast<std::uint8_t>(aShip::TShip_CanResolveObjectWithScanner(aPlayer::GetPlayer(), Ship) ^ 1) || static_cast<std::uint8_t>(TfGalaxy2::CanRevealBossPresence(Ship) ^ 1)) {
                ++UnknownCount;
                return;
            }
            if (Ship->CurrentStanding == aGalaxyStruct::ssCustom) {
                if (Ship->ScriptShip == nullptr || reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText == u"") {
                    ++OtherFactionCount;
                    OtherFaction = pas::WideString();
                } else {
                    Faction = reinterpret_cast<aScript::TScriptShip*>(Ship->ScriptShip)->StateText;
                    if (Faction == StarFaction) {
                        ++StarFactionCount;
                    } else {
                        if (OtherFactionCount == 0) {
                            OtherFaction = Faction;
                        } else if (OtherFaction != Faction) {
                            OtherFaction = pas::WideString();
                        }
                        ++OtherFactionCount;
                    }
                }
            } else if (Ship->TypeId == aGalaxyStruct::stKling) {
                Kling = pas::checked_cast<aKling::TKling*>(Ship);
                ++DominatorCounts[Kling->DominatorSeries][Kling->KlingType];
            } else {
                switch (Ship->TypeId) {
                    case aGalaxyStruct::stRanger: {
                        if (reinterpret_cast<aRanger::TRanger*>(Ship)->ExcludedFromRating || Ship->HasScriptStateText()) {
                            if ((aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Ship->PartnerShip) && aPlayer::GetPlayer()->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                                ++ScriptedPirates;
                            } else {
                                ++ScriptedCoalition;
                            }
                        } else if ((aPlayer::GetPlayer() == Ship || aPlayer::GetPlayer() == Ship->PartnerShip) && aPlayer::GetPlayer()->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                            ++RoleCounts[11];
                        } else {
                            ++RoleCounts[0];
                        }
                        break;
                    }
                    case aGalaxyStruct::stPirate: {
                        if (Ship->HasScriptStateText()) {
                            if (Ship->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                                ++ScriptedPirates;
                            } else {
                                ++ScriptedCoalition;
                            }
                        } else if (Ship->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
                            ++RoleCounts[2];
                        } else if (reinterpret_cast<aPirate::TPirate*>(Ship)->PirateType != 0) {
                            ++RoleCounts[12];
                        } else {
                            ++RoleCounts[13];
                        }
                        break;
                    }
                    case aGalaxyStruct::stWarrior: {
                        if (Ship->HasScriptStateText()) {
                            ++ScriptedCoalition;
                        } else if (pas::checked_cast<aWarrior::TWarrior*>(Ship)->WarriorType == aWarrior::wtFlagship) {
                            ++RoleCounts[10];
                        } else {
                            ++RoleCounts[1];
                        }
                        break;
                    }
                    case aGalaxyStruct::stTransport: {
                        Transport = pas::checked_cast<aTransport::TTransport*>(Ship);
                        if (Ship->HasScriptStateText()) {
                            ++ScriptedCoalition;
                        } else {
                            switch (Transport->TransportType) {
                                case aTransport::ttTransport: ++RoleCounts[3]; break;
                                case aTransport::ttLiner: ++RoleCounts[4]; break;
                                case aTransport::ttDiplomat: ++RoleCounts[5]; break;
                            }
                        }
                        break;
                    }
                    case aGalaxyStruct::stTranclucator: {
                        if (Ship->HasScriptStateText()) {
                            if (pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == nullptr) {
                                ++ScriptedCoalition;
                            } else if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip->OwnerId)) {
                                ++ScriptedCoalition;
                            } else if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.PirateClan), pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip->OwnerId)) {
                                ++ScriptedPirates;
                            }
                        } else if (pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip == nullptr) {
                            ++CoalitionTranclucators;
                        } else if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.Coalition), pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip->OwnerId)) {
                            ++CoalitionTranclucators;
                        } else if (pas::contains(pas::load_unaligned<aGalaxyStruct::TOwnerMask>(&aConst::PlanetOwnerMasks.PirateClan), pas::checked_cast<aTranclucator::TTranclucator*>(Ship)->OwnerShip->OwnerId)) {
                            ++PirateTranclucators;
                        }
                        break;
                    }
                    default: {
                        if (pas::in_range(Ship->TypeId, static_cast<std::int32_t>(aGalaxyStruct::rstRangerCenter), static_cast<std::int32_t>(aGalaxyStruct::rstCustomStation))) {
                            switch (Star->Status.ControlFaction) {
                                case aGalaxyStruct::sfCoalition: {
                                    if (pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssPiratePassive)) {
                                        ++CoalitionStations;
                                    } else if (pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssPirateActive, aGalaxyStruct::ssPirateMilitary)) {
                                        ++PirateStations;
                                    }
                                    break;
                                }
                                case aGalaxyStruct::sfPirates: {
                                    if (pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssCoalitionPassive, aGalaxyStruct::ssPirateMilitary)) {
                                        ++PirateStations;
                                    } else if (pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssCoalitionActive)) {
                                        ++CoalitionStations;
                                    }
                                    break;
                                }
                                default: {
                                    if (pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssCoalitionMilitary, aGalaxyStruct::ssNeutral)) {
                                        ++CoalitionStations;
                                    } else if (pas::in_range(Ship->CurrentStanding, aGalaxyStruct::ssPiratePassive, aGalaxyStruct::ssPirateMilitary)) {
                                        ++PirateStations;
                                    }
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        };
        auto SeriesColor = [&]() -> pas::WideString {
            pas::WideString Result{};
            switch (Series) {
                case aGalaxyStruct::dsBlazer: return u"<color=255,0,0>"_w;
                case aGalaxyStruct::dsKeller: return u"<color=0,128,255>"_w;
                case aGalaxyStruct::dsTerron: return u"<color=0,255,0>"_w;
                default: return Result;
            }
        };
        auto AppendLine = [&]() -> void {
            if (LineActive) {
                Summary = pas::concat_wide({Summary, Line, u"\r\n"});
                ++LineCount;
            }
        };
        UnknownCount = 0;
        OtherFactionCount = 0;
        StarFactionCount = 0;
        CoalitionTranclucators = 0;
        PirateTranclucators = 0;
        CoalitionStations = 0;
        PirateStations = 0;
        ScriptedCoalition = 0;
        ScriptedPirates = 0;
        StarFaction = Star->Status.CustomFaction;
        OtherFaction = pas::WideString();
        Summary = pas::WideString();
        for (auto cpp_range = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range.next(Series); ) {
            for (auto cpp_range_2 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_2.next(Kind); ) {
                DominatorCounts[Series][Kind] = 0;
            }
        }
        for (Role = static_cast<std::uint8_t>(0); Role <= static_cast<std::uint8_t>(10); ++Role) {
            RoleCounts[Role] = 0;
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Ships) - 1); cpp_range_3.next(I); ) {
            Ship = pas::list_at<aShip::TShip>(Star->Ships, I);
            if (Ship->CurrentPlanet == nullptr || Ship->CurrentPlanet->OwnerId != static_cast<std::uint8_t>(aGalaxyStruct::oiUninhabited)) {
                AccumulateShip();
            }
        }
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, pas::list_count(Star->Planets) - 1); cpp_range_4.next(I); ) {
            Planet = pas::list_at<aPlanet::TPlanet>(Star->Planets, I);
            for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, pas::list_count(Planet->Warriors) - 1); cpp_range_5.next(J); ) {
                Ship = pas::list_at<aShip::TShip>(Planet->Warriors, J);
                if (Ship->CurrentStar == Star && pas::list_indexof(Star->Ships, reinterpret_cast<void*>(Ship)) < 0) {
                    AccumulateShip();
                }
            }
        }
        Line = pas::WideString();
        LineActive = false;
        std::uint8_t HasSeparator = false;
        for (auto cpp_range_6 = pas::for_to<std::int32_t>(1, 7); cpp_range_6.next(GroupIndex); ) {
            Role = GalaxyMapFriendlyShipOrder[GroupIndex];
            if (RoleCounts[Role] > 0) {
                if (HasSeparator) {
                    Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
                }
                HasSeparator = true;
                LineActive = true;
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"FormGalaxy.FriendShip", SysUtils::IntToStr(GroupIndex)}))), u"<color=254,217,7>"_w)});
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(RoleCounts[Role]), u"<color=255,255,254>"_w)});
            }
        }
        if (ScriptedCoalition > 0) {
            if (HasSeparator) {
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
            }
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"?"_w, u"<color=254,217,7>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(ScriptedCoalition), u"<color=255,255,254>"_w)});
            LineActive = true;
        }
        if (CoalitionStations > 0 || CoalitionTranclucators > 0) {
            Line = pas::concat_wide({Line, u"     "});
        }
        if (CoalitionStations > 0) {
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"("_w, u"<color=127,127,127>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(CoalitionStations), u"<color=255,0,255>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u")"_w, u"<color=127,127,127>"_w)});
            LineActive = true;
        }
        if (CoalitionTranclucators > 0) {
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"("_w, u"<color=127,127,127>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(CoalitionTranclucators), u"<color=0,255,255>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u")"_w, u"<color=127,127,127>"_w)});
            LineActive = true;
        }
        AppendLine();
        Line = pas::WideString();
        LineActive = false;
        HasSeparator = false;
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(1, 3); cpp_range_7.next(PirateRole); ) {
            if (RoleCounts[PirateRole + 10] > 0) {
                if (HasSeparator) {
                    Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
                }
                HasSeparator = true;
                LineActive = true;
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"FormGalaxy.PirateClanShip", SysUtils::IntToStr(PirateRole)}))), u"<color=255,255,254>"_w)});
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(RoleCounts[PirateRole + 10]), u"<color=255,255,254>"_w)});
            }
        }
        if (ScriptedPirates > 0) {
            if (HasSeparator) {
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
            }
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"?"_w, u"<color=255,255,254>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(ScriptedPirates), u"<color=255,255,254>"_w)});
            LineActive = true;
        }
        if (PirateStations > 0 || PirateTranclucators > 0) {
            Line = pas::concat_wide({Line, u"     "});
        }
        if (PirateStations > 0) {
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"("_w, u"<color=127,127,127>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(PirateStations), u"<color=255,0,255>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u")"_w, u"<color=127,127,127>"_w)});
            LineActive = true;
        }
        if (PirateTranclucators > 0) {
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"("_w, u"<color=127,127,127>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(PirateTranclucators), u"<color=0,255,255>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u")"_w, u"<color=127,127,127>"_w)});
            LineActive = true;
        }
        AppendLine();
        Line = pas::WideString();
        if (StarFactionCount > 0) {
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"("_w, u"<color=127,127,127>"_w)});
            Line = pas::concat_wide({Line, ([&] {
                pas::WideString lookupNamedColorTag = aConst::LookupNamedColorTag(StarFaction);
                pas::WideString intToStr = pas::wide_int_to_str(StarFactionCount);
                return aMyFunction::WrapTextInColor(std::move(intToStr), std::move(lookupNamedColorTag));
            }())});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u")"_w, u"<color=127,127,127>"_w)});
            LineActive = true;
        }
        if (OtherFactionCount > 0) {
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"("_w, u"<color=127,127,127>"_w)});
            if (OtherFaction != u"") {
                Line = pas::concat_wide({Line, ([&] {
                    pas::WideString lookupNamedColorTag_2 = aConst::LookupNamedColorTag(OtherFaction);
                    pas::WideString intToStr_2 = pas::wide_int_to_str(OtherFactionCount);
                    return aMyFunction::WrapTextInColor(std::move(intToStr_2), std::move(lookupNamedColorTag_2));
                }())});
            } else {
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(OtherFactionCount), u"<color=127,127,127>"_w)});
            }
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u")"_w, u"<color=127,127,127>"_w)});
            LineActive = true;
        }
        AppendLine();
        for (auto cpp_range_8 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_8.next(Series); ) {
            Line = pas::WideString();
            LineActive = false;
            HasSeparator = false;
            ColorTag = SeriesColor();
            for (auto cpp_range_9 = pas::for_to<aGalaxyStruct::TKlingType>(aGalaxyStruct::ktBoss, aGalaxyStruct::ktKlig); cpp_range_9.next(Kind); ) {
                if (aConst::DominatorDisplayOrder[Kind] != aGalaxyStruct::ktBoss && DominatorCounts[Series][aConst::DominatorDisplayOrder[Kind]] > 0) {
                    if (HasSeparator) {
                        Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u"-"_w, u"<color=127,127,127>"_w)});
                    }
                    HasSeparator = true;
                    LineActive = true;
                    Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"FormGalaxy.DomikShip", SysUtils::IntToStr(aConst::DominatorDisplayOrder[Kind])}))), ColorTag)});
                    Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(DominatorCounts[Series][aConst::DominatorDisplayOrder[Kind]]), u"<color=255,255,254>"_w)});
                }
            }
            AppendLine();
        }
        Line = pas::WideString();
        LineActive = false;
        GroupIndex = 1;
        for (auto cpp_range_10 = pas::for_to<aGalaxyStruct::TDominatorSeries>(aGalaxyStruct::dsBlazer, aGalaxyStruct::dsTerron); cpp_range_10.next(Series); ) {
            ColorTag = SeriesColor();
            if (DominatorCounts[Series][aGalaxyStruct::ktBoss] > 0) {
                if (LineActive) {
                    Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(u", "_w, u"<color=127,127,127>"_w)});
                }
                Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(aConst::LocalizedText(static_cast<pas::WideString>(pas::concat_ansi({"FormGalaxy.Boss", SysUtils::IntToStr(GroupIndex)}))), ColorTag)});
                LineActive = true;
            }
            ++GroupIndex;
        }
        AppendLine();
        Line = pas::WideString();
        LineActive = false;
        if (UnknownCount > 0) {
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::concat_wide({aConst::LocalizedText(u"FormGalaxy.UnknowShip"_wref.get()), u": "}), u"<color=127,127,127>"_w)});
            Line = pas::concat_wide({Line, aMyFunction::WrapTextInColor(pas::wide_int_to_str(UnknownCount), u"<color=255,255,254>"_w)});
            LineActive = true;
        }
        AppendLine();
        return Summary;
    }

    void TfGalaxy2::UpdateJumpAnimations(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        EC_Struct::TPointF First{};
        EC_Struct::TPointF Second{};
        std::int32_t LightIndex{};
        if (JumpHintAnimationState == 0) {
            JumpDestinationLabel->SetActive(false);
            JumpAnimation->SetActive(true);
            JumpAnimation->SetSequenceFrame(0);
        } else if (JumpHintAnimationState == 1) {
            JumpDestinationLabel->SetActive(true);
            JumpAnimation->SetActive(true);
            JumpAnimation->SetSequenceFrame(0);
            JumpHintAnimationState = 2;
        } else if (JumpHintAnimationState == 2) {
            JumpDestinationLabel->SetActive(true);
            JumpAnimation->SetActive(true);
            if (JumpAnimation->SequenceFrame + 1 >= JumpAnimation->SequenceFrameCount) {
                JumpHintAnimationState = 3;
                JumpAnimation->SetSequenceFrame(JumpAnimation->SequenceFrameCount - 1);
            } else {
                JumpAnimation->SetSequenceFrame(JumpAnimation->SequenceFrame + 1);
            }
        } else if (JumpHintAnimationState == 3) {
            JumpDestinationLabel->SetActive(true);
            JumpAnimation->SetActive(true);
            JumpAnimation->SetSequenceFrame(JumpAnimation->SequenceFrameCount - 1);
        } else if (JumpHintAnimationState == 4) {
            JumpDestinationLabel->SetActive(true);
            JumpAnimation->SetActive(true);
            JumpAnimation->SetSequenceFrame(JumpAnimation->SequenceFrameCount - 1);
            JumpHintAnimationState = 5;
        } else if (JumpHintAnimationState == 5) {
            JumpDestinationLabel->SetActive(true);
            if (JumpAnimation->SequenceFrame - 1 < 0) {
                JumpHintAnimationState = 0;
                JumpAnimation->SetSequenceFrame(0);
                JumpAnimation->SetActive(true);
            } else {
                JumpAnimation->SetSequenceFrame(JumpAnimation->SequenceFrame - 1);
                JumpAnimation->SetActive(true);
            }
        }
        JumpLightImages[0]->SetActive(false);
        JumpLightImages[1]->SetActive(false);
        JumpLightImages[2]->SetActive(false);
        if (static_cast<std::uint8_t>(JumpButton->Disabled ^ 1) && aPlayer::GetPlayer()->CurrentStar != SelectedJumpStar && SelectedJumpStar != nullptr && aPlayer::GetPlayer()->HasPositiveSpeed()) {
            First = SelectedJumpStar->Position;
            Second = aPlayer::GetPlayer()->CurrentStar->Position;
            if (aPlayer::GetPlayer()->JumpRange < System::Round(System::Sqrt(pas::sqr(static_cast<long double>(First.X) - Second.X) + pas::sqr(static_cast<long double>(First.Y) - Second.Y)))) {
                return;
            }
            ++JumpLightTick;
            LightIndex = JumpLightTick / 5 % 3;
            JumpLightImages[0]->SetActive(LightIndex == 2);
            JumpLightImages[1]->SetActive(LightIndex == 1);
            JumpLightImages[2]->SetActive(LightIndex == 0);
        }
    }

    void TfGalaxy2::SelectMusic() {
        if (aPlayer::GetPlayer() == nullptr) {
            GR_Main::MusicManager->PlayCategory(u"Base"_wref.get());
        } else if (aPlayer::GetPlayer()->IsOnPlanet()) {
            if (!GlobalsV::MusicInPlanetEnabled) {
                GR_Main::MusicManager->RequestFadeOut();
            } else if (aPlayer::GetPlayer()->CurrentPlanet->OwnerId == static_cast<std::uint8_t>(aGalaxyStruct::oiPirate)) {
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
        } else if (aPlayer::GetPlayer()->InNormalSpace()) {
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
    }

    void TfGalaxy2::ProcessCallbackTimers() {
        GI_MessageLoop::TMessageLoopGI::ProcessCallbackTimers();
        if (ParentLoop != nullptr && ParentLoop->ExitCode != 0 && ExitCode == 0) {
            RequestClose(2);
        }
    }

    void TfGalaxy2::CreateMarkerClicked(GI_MessageLoop::TObjectGI* Sender) {
        CreateMarkerMode = static_cast<std::uint8_t>(CreateMarkerMode ^ 1);
        if (CreateMarkerMode) {
            CreateMarkerButton->SetImageNormalPath(CreateMarkerButton->ImageDown->GetImagePath());
            CreateMarkerButton->SetImageNormalActivePath(CreateMarkerButton->ImageDown->GetImagePath());
        } else {
            CreateMarkerButton->SetImageNormalPath(CreateMarkerImagePath);
            CreateMarkerButton->SetImageNormalActivePath(CreateMarkerActiveImagePath);
        }
    }

    void TfGalaxy2::UndoMarkerClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (pas::list_count(RouteStars) > 0) {
            pas::list_delete(RouteStars, pas::list_count(RouteStars) - 1);
            RebuildJumpPath();
        }
    }

    void TfGalaxy2::ClearMarkersClicked(GI_MessageLoop::TObjectGI* Sender) {
        if (pas::list_count(RouteStars) > 0) {
            pas::list_clear(RouteStars);
            RebuildJumpPath();
        }
    }

    void TfGalaxy2::ExecuteUiCode(EC_BlockPar::TBlockParEC* Block, std::uint32_t Key) {
        if (static_cast<std::uint8_t>(GR_Main::ExitScreenLoop ^ 1) && pas::is_one_of<ThreadCalc::tcpIdle, ThreadCalc::tcpGalaxyFinished, ThreadCalc::tcpPlayerStarFinished, ThreadCalc::tcpPlayerStarPrepared>(aCalc::TurnCalculationPhase)) {
            aGalaxy::Galaxy->CheckIntegrityChecksum(10005);
            aScript::ExecuteGameplayUiCode(Block, Key);
            aGalaxy::Galaxy->PrimeIntegrityChecksum(20005);
        }
    }

    void TfGalaxy2::p_destroy() {
        fGalaxy2::TfGalaxy2_Destroy(this);
    }

} // namespace fGalaxy2
