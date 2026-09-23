#include "layout/SE_Planet.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Process.hpp"
#include "types/SE_Star.hpp"
#include "types/Types.hpp"
#include "types/aGalaxyStruct.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheBitmap.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_AlphaImage.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_Planet.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SE_Planet.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aConst.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Planet {
    SE_Planet::PPlanetCollisionCircle FirstPlanetCollisionCircle = nullptr;

    // Links a new entry at the head; only links are initialized.
    PPlanetCollisionCircle AllocatePlanetCollisionCircle() {
        PPlanetCollisionCircle Entry{};
        pas::new_value(Entry);
        Entry->Next = FirstPlanetCollisionCircle;
        Entry->Prev = nullptr;
        if (Entry->Next != nullptr) {
            Entry->Next->Prev = Entry;
        }
        FirstPlanetCollisionCircle = Entry;
        return Entry;
    }

    void FreePlanetCollisionCircle(PPlanetCollisionCircle Entry) {
        if (Entry->Next != nullptr) {
            Entry->Next->Prev = Entry->Prev;
        }
        if (Entry->Prev != nullptr) {
            Entry->Prev->Next = Entry->Next;
        }
        if (Entry == FirstPlanetCollisionCircle) {
            FirstPlanetCollisionCircle = Entry->Next;
        }
        pas::dispose(Entry);
    }

    void TPlanetSE_Create(TPlanetSE* Self) {
        SE_Space::TObjectSE_CreateEmpty(Self);
    }

    void TPlanetSE_CreateFromGraph(TPlanetSE* Self, const pas::WideString& AGraphKey, WindowsSdk::TPoint UnusedPosition) {
        Self->IsRuins = EC_Str::FindTextOffsetW(AGraphKey, u"Ruins"_wref.get(), 0) == 0;
        SE_Space::TObjectSE_Create(Self, AGraphKey, UnusedPosition);
    }

    // Destination must be a TPlanetSE. Copies configuration, not attached controls/timers.
    void TPlanetSE::CopyTo(SE_Space::TObjectSE* Destination) {
        SE_Space::TObjectSE::CopyTo(Destination);
        {
            TPlanetSE* cpp_with = pas::checked_cast<TPlanetSE*>(Destination);
            cpp_with->ImagePath = ImagePath;
            cpp_with->ImageOrigin = ImageOrigin;
            cpp_with->SurfaceMapOffset = SurfaceMapOffset;
            cpp_with->LightAngle = LightAngle;
            cpp_with->RotationTimerInterval = RotationTimerInterval;
            cpp_with->SurfaceMapStep = SurfaceMapStep;
            cpp_with->MinimapImagePath = MinimapImagePath;
            cpp_with->MinimapImageOrigin = MinimapImageOrigin;
            cpp_with->OrbitalVelocity = OrbitalVelocity;
            cpp_with->Radius = Radius;
            cpp_with->RingKind = RingKind;
            cpp_with->MinimapOwner = MinimapOwner;
            cpp_with->Cloud1ImagePath = Cloud1ImagePath;
            cpp_with->Cloud1RelativeRotationSpeed = Cloud1RelativeRotationSpeed;
            cpp_with->Cloud1MapOffset = Cloud1MapOffset;
            cpp_with->Cloud2ImagePath = Cloud2ImagePath;
            cpp_with->Cloud2RelativeRotationSpeed = Cloud2RelativeRotationSpeed;
            cpp_with->Cloud2MapOffset = Cloud2MapOffset;
            cpp_with->Cloud3ImagePath = Cloud3ImagePath;
            cpp_with->Cloud3RelativeRotationSpeed = Cloud3RelativeRotationSpeed;
            cpp_with->Cloud3MapOffset = Cloud3MapOffset;
            cpp_with->AtmosphereColor = AtmosphereColor;
            cpp_with->SpaceConfigValues[0] = SpaceConfigValues[0];
            cpp_with->SpaceConfigValues[1] = SpaceConfigValues[1];
            cpp_with->SpaceConfigValues[2] = SpaceConfigValues[2];
            cpp_with->BackgroundGraph = BackgroundGraph;
            cpp_with->QuestEnabled = QuestEnabled;
            cpp_with->IsRuins = IsRuins;
            cpp_with->RuinsAnimationPath = RuinsAnimationPath;
            cpp_with->RuinsImagePath = RuinsImagePath;
            cpp_with->RuinsMinimapPath = RuinsMinimapPath;
            cpp_with->RuinsAnimationFrame = RuinsAnimationFrame;
        }
    }

    void TPlanetSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        Globals::TPlanetTempl* Template{};
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t Interval{};
        std::int32_t OwnerIndex{};
        if (IsAttachedToSpace()) {
            return;
        }
        if (Civilized) {
            ConfigureLoopSound(u"Planet.Civil"_wref.get());
        } else {
            ConfigureLoopSound(u"Planet.NotCivil"_wref.get());
        }
        if (Civilized) {
            ConfigureRandomSound(u"Planet.Civil"_wref.get());
        } else {
            ConfigureRandomSound(u"Planet.NotCivil"_wref.get());
        }
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        if (IsRuins) {
            if (GlobalsV::AnimShipFull || GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle) {
                RuinsAnimationControl = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                RuinsAnimationControl->SetImagePath(RuinsAnimationPath);
                {
                    WindowsSdk::TPoint contentSize = RuinsAnimationControl->GetContentSize();
                    GI_GAI::TgaiGI* ruinsAnimationControl = RuinsAnimationControl;
                    ruinsAnimationControl->SetSize(contentSize);
                }
                RuinsAnimationControl->SetOrigin(EC_Struct::HalfPoint(RuinsAnimationControl->ClientSize));
                RuinsAnimationControl->SetDepthByName(DepthExpression);
                RuinsAnimationControl->SetPosition(EC_Struct::TruncatePointF(Position));
                RuinsAnimationControl->SetPositionModeW(true);
                RuinsAnimationControl->SequenceIndex = 0;
                RuinsAnimationControl->UpdateAutoGeometry();
                RuinsAnimationControl->SetSequenceFrame(RuinsAnimationFrame);
                RuinsAnimationControl->RestartPlayback();
                RuinsAnimationControl->SetAlpha(255);
                Size = RuinsAnimationControl->ClientSize;
            } else {
                RuinsImageControl = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Space->MapPanel);
                RuinsImageControl->SetImagePath(RuinsImagePath);
                {
                    Types::TPoint contentSize_2 = RuinsImageControl->GetContentSize();
                    GI_Image::TImageGI* ruinsImageControl = RuinsImageControl;
                    ruinsImageControl->SetSize(contentSize_2);
                }
                RuinsImageControl->SetOrigin(EC_Struct::HalfPoint(RuinsImageControl->ClientSize));
                RuinsImageControl->SetDepthByName(DepthExpression);
                RuinsImageControl->SetPosition(EC_Struct::TruncatePointF(Position));
                RuinsImageControl->SetPositionModeW(true);
                RuinsImageControl->SetAlpha(255);
                Size = RuinsImageControl->ClientSize;
            }
            RuinsMinimapControl = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
            RuinsMinimapControl->SetPositionModeW(true);
            RuinsMinimapControl->SetDepthByName(DepthExpression);
            RuinsMinimapControl->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(Position.X) * Space->MinimapScale, static_cast<long double>(Position.Y) * Space->MinimapScale)));
            RuinsMinimapControl->SetImagePath(RuinsMinimapPath);
            {
                Types::TPoint contentSize_3 = RuinsMinimapControl->GetContentSize();
                GI_Image::TImageGI* ruinsMinimapControl = RuinsMinimapControl;
                ruinsMinimapControl->SetSize(contentSize_3);
            }
            RuinsMinimapControl->SetOrigin(EC_Struct::HalfPoint(RuinsMinimapControl->ClientSize));
        } else {
            Template = nullptr;
            Count = pas::list_count(Globals::PlanetRenderTemplates);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                Template = pas::list_at<Globals::TPlanetTempl>(Globals::PlanetRenderTemplates, Index);
                if (Template->Radius == Radius) {
                    break;
                }
            }
            if (Template == nullptr) {
                pas::raise(pas::make_exception<pas::Exception>("Error in TPlanetSE.Connect"_a));
            }
            PlanetControl = pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Space->MapPanel);
            PlanetControl->SetPositionModeW(true);
            PlanetControl->SetDepthByName(DepthExpression);
            {
                std::int32_t trunc = System::Trunc(Position.Y);
                std::int32_t trunc_2 = System::Trunc(Position.X);
                PlanetControl->SetPosition(ClassesImports::Point(trunc_2, trunc));
            }
            PlanetControl->SetSurfaceMapOffset(SurfaceMapOffset);
            PlanetControl->SetOrigin(ImageOrigin);
            PlanetControl->SetImageWithRadius(Template->MaskName, ImagePath, Template->LightName, Radius);
            if (GlobalsV::PlanetClouds) {
                if (Cloud1ImagePath != u"") {
                    PlanetControl->SetCloud1Image(Cloud1ImagePath);
                }
                if (Cloud2ImagePath != u"") {
                    PlanetControl->SetCloud2Image(Cloud2ImagePath);
                }
                if (Cloud3ImagePath != u"") {
                    PlanetControl->SetCloud3Image(Cloud3ImagePath);
                }
                PlanetControl->SetCloud1MapOffset(Cloud1MapOffset);
                PlanetControl->SetCloud2MapOffset(Cloud2MapOffset);
                PlanetControl->SetCloud3MapOffset(Cloud3MapOffset);
            }
            PlanetControl->SetLightAngle(LightAngle);
            if (GlobalsV::PlanetAtm && AtmosphereColor != 0) {
                {
                    pas::WideString cpp_arg = pas::concat_wide({u"Bm.Atm.", GR_Main::GiResourceSuffix(), u"mask", pas::wide_int_to_str(Radius * 2)});
                    pas::WideString cpp_arg_2 = pas::concat_wide({u"Bm.Atm.", GR_Main::GiResourceSuffix(), u"atm", pas::wide_int_to_str(Radius * 2)});
                    PlanetControl->SetAtmosphere(std::move(cpp_arg_2), std::move(cpp_arg), AtmosphereColor);
                }
                PlanetControl->SetOrigin(EC_Struct::HalfPoint(PlanetControl->ClientSize));
            }
            MinimapControl = pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
            MinimapControl->SetPositionModeW(true);
            MinimapControl->SetDepthByName(DepthExpression);
            MinimapControl->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(Position.X) * Space->MinimapScale, static_cast<long double>(Position.Y) * Space->MinimapScale)));
            MinimapControl->SetOrigin(MinimapImageOrigin);
            if (MinimapOwner <= 7) {
                MinimapControl->SetImagePath(pas::concat_wide({u"Bm.Planet.M.", aConst::OwnerInfo[static_cast<aGalaxyStruct::TOwnerId>(MinimapOwner)].InternalName}));
            } else {
                OwnerIndex = MinimapOwner - 7 - 1;
                if (OwnerIndex <= 9) {
                    MinimapControl->SetImagePath(pas::concat_wide({u"Bm.Planet.M.0", EC_Str::IntToWideString(OwnerIndex)}));
                } else {
                    MinimapControl->SetImagePath(pas::concat_wide({u"Bm.Planet.M.", EC_Str::IntToWideString(OwnerIndex)}));
                }
            }
            {
                WindowsSdk::TPoint contentSize_4 = MinimapControl->GetContentSize();
                GI_MessageLoop::TObjectGI* minimapControl = MinimapControl;
                minimapControl->SetSize(contentSize_4);
            }
            UpdateLightAngleFromStar();
            CollisionCircle = SE_Planet::AllocatePlanetCollisionCircle();
            CollisionCircle->Position.X = Position.X;
            CollisionCircle->Position.Y = Position.Y;
            CollisionCircle->Radius = Radius;
            RebuildRings();
            RebuildSurfaceAnimation();
            RotationTimer = Space->CreateTimer(0, RotationTimerInterval, pas::bind_method<&TPlanetSE::AdvanceRotationTimer>(this), 0);
            if (GlobalsV::PlanetClouds) {
                if (Cloud1ImagePath != u"" && Cloud1RelativeRotationSpeed != -1.0L) {
                    if (Cloud1RelativeRotationSpeed > -1.0L) {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (1.0L + Cloud1RelativeRotationSpeed))));
                        Cloud1MapStep = 1;
                    } else {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (-Cloud1RelativeRotationSpeed - 1.0L))));
                        Cloud1MapStep = -1;
                    }
                    Cloud1Timer = Space->CreateTimer(0, pas::abs(Interval), pas::bind_method<&TPlanetSE::AdvanceCloudTimer>(this), 1);
                }
                if (Cloud2ImagePath != u"" && Cloud2RelativeRotationSpeed != -1.0L) {
                    if (Cloud2RelativeRotationSpeed > -1.0L) {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (1.0L + Cloud2RelativeRotationSpeed))));
                        Cloud2MapStep = 1;
                    } else {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (-Cloud2RelativeRotationSpeed - 1.0L))));
                        Cloud2MapStep = -1;
                    }
                    Cloud2Timer = Space->CreateTimer(0, pas::abs(Interval), pas::bind_method<&TPlanetSE::AdvanceCloudTimer>(this), 2);
                }
                if (Cloud3ImagePath != u"" && Cloud3RelativeRotationSpeed != -1.0L) {
                    if (Cloud3RelativeRotationSpeed > -1.0L) {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (1.0L + Cloud3RelativeRotationSpeed))));
                        Cloud3MapStep = 1;
                    } else {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (-Cloud3RelativeRotationSpeed - 1.0L))));
                        Cloud3MapStep = -1;
                    }
                    Cloud3Timer = Space->CreateTimer(0, pas::abs(Interval), pas::bind_method<&TPlanetSE::AdvanceCloudTimer>(this), 3);
                }
            }
            Size = ClassesImports::Point((Radius + 5) * 2, (Radius + 5) * 2);
        }
    }

    void TPlanetSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        if (IsRuins) {
            if (RuinsAnimationControl != nullptr) {
                RuinsAnimationFrame = RuinsAnimationControl->SequenceFrame;
                pas::free(RuinsAnimationControl);
                RuinsAnimationControl = nullptr;
            }
            if (RuinsImageControl != nullptr) {
                RuinsImageControl->SetActive(false);
                pas::free(RuinsImageControl);
                RuinsImageControl = nullptr;
            }
            if (RuinsMinimapControl != nullptr) {
                pas::free(RuinsMinimapControl);
                RuinsMinimapControl = nullptr;
            }
        } else {
            SE_Planet::FreePlanetCollisionCircle(CollisionCircle);
            CollisionCircle = nullptr;
            if (MapOrbitPoints != nullptr) {
                EC_Mem::FreeEC(MapOrbitPoints);
                MapOrbitPoints = nullptr;
            }
            if (RotationTimer != nullptr) {
                Space->DeleteTimer(RotationTimer);
                RotationTimer = nullptr;
            }
            if (Cloud1Timer != nullptr) {
                Space->DeleteTimer(Cloud1Timer);
                Cloud1Timer = nullptr;
            }
            if (Cloud2Timer != nullptr) {
                Space->DeleteTimer(Cloud2Timer);
                Cloud2Timer = nullptr;
            }
            if (Cloud3Timer != nullptr) {
                Space->DeleteTimer(Cloud3Timer);
                Cloud3Timer = nullptr;
            }
            Space->MapPanel->FreeOwnedChild(PlanetControl);
            PlanetControl = nullptr;
            if (RingControl1 != nullptr) {
                pas::free(RingControl1);
                RingControl1 = nullptr;
            }
            if (RingControl2 != nullptr) {
                pas::free(RingControl2);
                RingControl2 = nullptr;
            }
            if (LegacySurfaceControl != nullptr) {
                pas::free(LegacySurfaceControl);
                LegacySurfaceControl = nullptr;
            }
            if (SurfaceImageControl != nullptr) {
                pas::free(SurfaceImageControl);
                SurfaceImageControl = nullptr;
            }
            if (MinimapControl != nullptr) {
                pas::free(MinimapControl);
                MinimapControl = nullptr;
            }
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TPlanetSE::RebuildRings() {
        pas::WideString Path{};
        WindowsSdk::TPoint Center{};
        WindowsSdk::TPoint Offset{};
        WindowsSdk::TRect Bounds{};
        WindowsSdk::TRect FirstBounds{};
        WindowsSdk::TRect SecondBounds{};
        if (IsRuins) {
            return;
        }
        if (RingControl1 != nullptr) {
            pas::free(RingControl1);
            RingControl1 = nullptr;
        }
        if (RingControl2 != nullptr) {
            pas::free(RingControl2);
            RingControl2 = nullptr;
        }
        if (RingKind != 0) {
            Path = pas::concat_wide({u"GI,Bm.PlanetRing.", GR_Main::GiResourceSuffix(), u"r"});
            if (RingKind - 1 < 10) {
                Path = pas::concat_wide({Path, u"0"});
            }
            Path = pas::concat_wide({Path, pas::wide_int_to_str(RingKind - 1), u"_"});
            if (RingKind >= 20) {
                Path = pas::concat_wide({Path, u"0"});
            } else if (Radius == 100) {
                Path = pas::concat_wide({Path, u"0"});
            } else if (Radius == 90) {
                Path = pas::concat_wide({Path, u"1"});
            } else if (Radius == 80) {
                Path = pas::concat_wide({Path, u"2"});
            } else if (Radius == 70) {
                Path = pas::concat_wide({Path, u"3"});
            } else if (Radius == 60) {
                Path = pas::concat_wide({Path, u"4"});
            } else {
                GR_Main::RaiseWideMessage(u"TPlanetSE.CreateRing"_wref.get());
            }
            RingControl1 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Space->MapPanel);
            RingControl1->SetPositionModeW(true);
            RingControl1->SetDepth(PlanetControl->Depth - 0.01L);
            RingControl1->SetImagePath(pas::concat_wide({Path, u"_1"}));
            {
                Types::TPoint contentSize = RingControl1->GetContentSize();
                GI_Image::TImageGI* ringControl1 = RingControl1;
                ringControl1->SetSize(contentSize);
            }
            RingControl1->SetPosition(PlanetControl->LocalPosition);
            RingControl2 = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Space->MapPanel);
            RingControl2->SetPositionModeW(true);
            RingControl2->SetDepth(PlanetControl->Depth + 0.01L);
            RingControl2->SetImagePath(pas::concat_wide({Path, u"_2"}));
            {
                Types::TPoint contentSize_2 = RingControl2->GetContentSize();
                GI_Image::TImageGI* ringControl2 = RingControl2;
                ringControl2->SetSize(contentSize_2);
            }
            RingControl2->SetPosition(PlanetControl->LocalPosition);
            pas::store_unaligned<Types::TPoint>(pas::byte_offset(&FirstBounds, 0), RingControl1->GetContentOrigin());
            pas::store_unaligned<Types::TPoint>(pas::byte_offset(&FirstBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&FirstBounds, 0)), RingControl1->ClientSize));
            pas::store_unaligned<Types::TPoint>(pas::byte_offset(&SecondBounds, 0), RingControl2->GetContentOrigin());
            pas::store_unaligned<Types::TPoint>(pas::byte_offset(&SecondBounds, 8), EC_Struct::AddPoints(pas::load_unaligned<Types::TPoint>(pas::byte_offset(&SecondBounds, 0)), RingControl2->ClientSize));
            WindowsSdk::UnionRect(Bounds, FirstBounds, SecondBounds);
            Center = ClassesImports::Point((Bounds.Right + Bounds.Left) / 2, (Bounds.Bottom + Bounds.Top) / 2);
            Offset = ClassesImports::Point(0, Bounds.Right / 2 - Bounds.Bottom / 2);
            if (RingKind == 8) {
                Offset.Y += GR_Main::GiScalePixels(10);
            }
            if (RingKind == 8) {
                Offset.X += GR_Main::GiScalePixels(5);
            }
            if (RingKind == 21) {
                Offset.Y += GR_Main::GiScalePixels(25);
            } else if (RingKind == 22) {
                Offset.Y += GR_Main::GiScalePixels(15);
            }
            if (RingKind == 21) {
                Offset.X -= GR_Main::GiScalePixels(8);
            } else if (RingKind == 22) {
                Offset.X -= GR_Main::GiScalePixels(5);
            }
            RingControl1->SetOrigin(EC_Struct::SubtractPoints(EC_Struct::SubtractPoints(Center, pas::load_unaligned<Types::TPoint>(pas::byte_offset(&FirstBounds, 0))), Offset));
            RingControl2->SetOrigin(EC_Struct::SubtractPoints(EC_Struct::SubtractPoints(Center, pas::load_unaligned<Types::TPoint>(pas::byte_offset(&SecondBounds, 0))), Offset));
        }
    }

    void TPlanetSE::RebuildSurfaceAnimation() {
        float Scale{};
        if (IsRuins) {
            return;
        }
        if (LegacySurfaceControl != nullptr) {
            pas::free(LegacySurfaceControl);
            LegacySurfaceControl = nullptr;
        }
        if (SurfaceImageControl != nullptr) {
            pas::free(SurfaceImageControl);
            SurfaceImageControl = nullptr;
        }
        if (SurfaceAnimationMask > 0 && GR_Main::GiResourceVariant() == 2 && Radius == 100) {
            SurfaceAnimationIndex = -1;
            Scale = pas::real_divide(Radius - 60, 4.0E+1L) * 0.7L + 0.3L;
            SurfaceAnimationOffset.X = GR_Main::GiScalePixels(System::Round(static_cast<long double>(Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Position.X) * Scale));
            SurfaceAnimationOffset.Y = GR_Main::GiScalePixels(System::Round(static_cast<long double>(Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Position.Y) * Scale));
            SurfaceAnimationFrame = 0;
            SurfaceImageControl = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Space->MapPanel);
            SurfaceImageControl->SetPositionModeW(true);
            SurfaceImageControl->SetDepth(PlanetControl->Depth - 0.02L);
            StartRandomSurfaceAnimation();
        }
    }

    // Requires an allocated image control and a mask with positive total animation weight.
    void TPlanetSE::StartRandomSurfaceAnimation() {
        std::int32_t Index{};
        std::int32_t Attempts{};
        std::int32_t Choice{};
        if (IsRuins) {
            return;
        }
        std::int32_t TotalWeight = 0;
        {
            Globals::TPlanetAdvtGroup& cpp_with = Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)];
            for (Index = 0; Index <= 23; ++Index) {
                if ((SurfaceAnimationMask & pas::shl(1, Index)) != 0) {
                    TotalWeight += cpp_with.Lists[Index].Weight;
                }
            }
            Attempts = 10;
            while (Attempts > 0) {
                Choice = aMyFunction::RandomIntRange(0, TotalWeight - 1);
                for (Index = 0; Index <= 23; ++Index) {
                    if ((SurfaceAnimationMask & pas::shl(1, Index)) != 0) {
                        Choice -= cpp_with.Lists[Index].Weight;
                        if (Choice < 0) {
                            SurfaceAnimationIndex = Index;
                            // Native compares the value just assigned; retain the unreachable assignment.
                            if (SurfaceAnimationIndex != Index) {
                                Attempts = 0;
                            }
                            break;
                        }
                    }
                }
                --Attempts;
            }
        }
        SurfaceAnimationFrame = 0;
        if (GR_Main::GiResourceVariant() == 1) {
            SurfaceImageControl->SetImagePath(Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Adverts[Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Lists[SurfaceAnimationIndex].Indices[SurfaceAnimationFrame]].Image1);
        } else {
            SurfaceImageControl->SetImagePath(Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Adverts[Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Lists[SurfaceAnimationIndex].Indices[SurfaceAnimationFrame]].Image2);
        }
        {
            Types::TPoint contentSize = SurfaceImageControl->GetContentSize();
            GI_Image::TImageGI* surfaceImageControl = SurfaceImageControl;
            surfaceImageControl->SetSize(contentSize);
        }
        SurfaceImageControl->SetOrigin(EC_Struct::HalfPoint(SurfaceImageControl->ClientSize));
        SurfaceImageControl->SetPosition(EC_Struct::AddPoints(PlanetControl->LocalPosition, SurfaceAnimationOffset));
        if (SurfaceImageControl->GaiImageControl != nullptr) {
            SurfaceImageControl->GaiImageControl->CycleCompleteCallback = pas::bind_method<&TPlanetSE::SurfaceAnimationFinished>(this);
        }
        SurfaceImageControl->RestartPlayback();
    }

    void TPlanetSE::SetMinimapOwner(std::uint8_t Owner) {
        std::int32_t Index{};
        if (IsRuins) {
            return;
        }
        if (MinimapOwner == Owner) {
            return;
        }
        MinimapOwner = Owner;
        if (MinimapControl != nullptr) {
            if (MinimapOwner <= 7) {
                MinimapControl->SetImagePath(pas::concat_wide({u"Bm.Planet.M.", aConst::OwnerInfo[static_cast<aGalaxyStruct::TOwnerId>(MinimapOwner)].InternalName}));
            } else {
                Index = MinimapOwner - 7 - 1;
                if (Index <= 9) {
                    MinimapControl->SetImagePath(pas::concat_wide({u"Bm.Planet.M.0", EC_Str::IntToWideString(Index)}));
                } else {
                    MinimapControl->SetImagePath(pas::concat_wide({u"Bm.Planet.M.", EC_Str::IntToWideString(Index)}));
                }
            }
            {
                WindowsSdk::TPoint contentSize = MinimapControl->GetContentSize();
                GI_MessageLoop::TObjectGI* minimapControl = MinimapControl;
                minimapControl->SetSize(contentSize);
            }
        }
    }

    void TPlanetSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsRuins) {
            if (IsAttachedToSpace()) {
                if (RuinsImageControl != nullptr) {
                    RuinsImageControl->SetPosition(EC_Struct::TruncatePointF(APosition));
                }
                if (RuinsAnimationControl != nullptr) {
                    RuinsAnimationControl->SetPosition(EC_Struct::TruncatePointF(APosition));
                }
                RuinsMinimapControl->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
            }
        } else {
            if (CollisionCircle != nullptr) {
                CollisionCircle->Position.X = Position.X;
                CollisionCircle->Position.Y = Position.Y;
                CollisionCircle->Radius = Radius;
                CollisionCircle->RadiusSquared = Radius * Radius;
            }
            UpdateLightAngleFromStar();
            if (IsAttachedToSpace()) {
                {
                    std::int32_t round = System::Round(APosition.Y);
                    std::int32_t round_2 = System::Round(APosition.X);
                    PlanetControl->SetPosition(ClassesImports::Point(round_2, round));
                }
                MinimapControl->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
                if (RingControl1 != nullptr) {
                    RingControl1->SetPosition(PlanetControl->LocalPosition);
                }
                if (RingControl2 != nullptr) {
                    RingControl2->SetPosition(PlanetControl->LocalPosition);
                }
                if (LegacySurfaceControl != nullptr) {
                    LegacySurfaceControl->SetPosition(EC_Struct::AddPoints(PlanetControl->LocalPosition, SurfaceAnimationOffset));
                }
                if (SurfaceImageControl != nullptr) {
                    SurfaceImageControl->SetPosition(EC_Struct::AddPoints(PlanetControl->LocalPosition, SurfaceAnimationOffset));
                }
            }
        }
    }

    void TPlanetSE::SetSurfaceMapOffset(std::int32_t Value) {
        SurfaceMapOffset = Value;
        if (static_cast<std::uint8_t>(IsRuins ^ 1) && IsAttachedToSpace()) {
            PlanetControl->SetSurfaceMapOffset(SurfaceMapOffset);
        }
    }

    void TPlanetSE::SetCloud1MapOffset(std::int32_t Value) {
        Cloud1MapOffset = Value;
        if (static_cast<std::uint8_t>(IsRuins ^ 1) && IsAttachedToSpace()) {
            PlanetControl->SetCloud1MapOffset(Cloud1MapOffset);
        }
    }

    void TPlanetSE::SetCloud2MapOffset(std::int32_t Value) {
        Cloud2MapOffset = Value;
        if (static_cast<std::uint8_t>(IsRuins ^ 1) && IsAttachedToSpace()) {
            PlanetControl->SetCloud2MapOffset(Cloud2MapOffset);
        }
    }

    void TPlanetSE::SetCloud3MapOffset(std::int32_t Value) {
        Cloud3MapOffset = Value;
        if (static_cast<std::uint8_t>(IsRuins ^ 1) && IsAttachedToSpace()) {
            PlanetControl->SetCloud3MapOffset(Cloud3MapOffset);
        }
    }

    void TPlanetSE::SetLightAngle(std::uint8_t Value) {
        if (IsRuins) {
            return;
        }
        LightAngle = Value;
        if (IsAttachedToSpace()) {
            PlanetControl->SetLightAngle(LightAngle);
        }
    }

    void TPlanetSE::SetRotationTimerInterval(std::uint32_t Value) {
        std::int32_t Interval{};
        RotationTimerInterval = Value;
        if (IsRuins) {
            return;
        }
        if (IsAttachedToSpace()) {
            if (RotationTimer != nullptr) {
                Space->DeleteTimer(RotationTimer);
                RotationTimer = nullptr;
            }
            RotationTimer = Space->CreateTimer(0, RotationTimerInterval, pas::bind_method<&TPlanetSE::AdvanceRotationTimer>(this), 0);
            if (Cloud1Timer != nullptr) {
                Space->DeleteTimer(Cloud1Timer);
                Cloud1Timer = nullptr;
            }
            if (Cloud2Timer != nullptr) {
                Space->DeleteTimer(Cloud2Timer);
                Cloud2Timer = nullptr;
            }
            if (Cloud3Timer != nullptr) {
                Space->DeleteTimer(Cloud3Timer);
                Cloud3Timer = nullptr;
            }
            if (GlobalsV::PlanetClouds) {
                if (Cloud1ImagePath != u"" && Cloud1RelativeRotationSpeed != -1.0L) {
                    if (Cloud1RelativeRotationSpeed > -1.0L) {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (1.0L + Cloud1RelativeRotationSpeed))));
                        Cloud1MapStep = 1;
                    } else {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (-Cloud1RelativeRotationSpeed - 1.0L))));
                        Cloud1MapStep = -1;
                    }
                    Cloud1Timer = Space->CreateTimer(0, pas::abs(Interval), pas::bind_method<&TPlanetSE::AdvanceCloudTimer>(this), 1);
                }
                if (Cloud2ImagePath != u"" && Cloud2RelativeRotationSpeed != -1.0L) {
                    if (Cloud2RelativeRotationSpeed > -1.0L) {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (1.0L + Cloud2RelativeRotationSpeed))));
                        Cloud2MapStep = 1;
                    } else {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (-Cloud2RelativeRotationSpeed - 1.0L))));
                        Cloud2MapStep = -1;
                    }
                    Cloud2Timer = Space->CreateTimer(0, pas::abs(Interval), pas::bind_method<&TPlanetSE::AdvanceCloudTimer>(this), 2);
                }
                if (Cloud3ImagePath != u"" && Cloud3RelativeRotationSpeed != -1.0L) {
                    if (Cloud3RelativeRotationSpeed > -1.0L) {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (1.0L + Cloud3RelativeRotationSpeed))));
                        Cloud3MapStep = 1;
                    } else {
                        Interval = std::max<std::int64_t>(static_cast<std::int64_t>(10), System::Round(pas::real_divide(1.0E+3L, pas::real_divide(1.0E+3L, RotationTimerInterval) * (-Cloud3RelativeRotationSpeed - 1.0L))));
                        Cloud3MapStep = -1;
                    }
                    Cloud3Timer = Space->CreateTimer(0, pas::abs(Interval), pas::bind_method<&TPlanetSE::AdvanceCloudTimer>(this), 3);
                }
            }
        }
    }

    void TPlanetSE::SetSurfaceMapStep(std::int32_t Value) {
        SurfaceMapStep = Value;
    }

    // Does nothing for ruins; rebuilds rings when attached to space.
    void TPlanetSE::SetRingKind(std::uint8_t Kind) {
        if (IsRuins) {
            return;
        }
        RingKind = Kind;
        if (IsAttachedToSpace()) {
            RebuildRings();
        }
    }

    // Does nothing for ruins; rebuilds the animation when attached to space.
    void TPlanetSE::SetSurfaceAnimationMask(std::int32_t Mask) {
        if (IsRuins) {
            return;
        }
        SurfaceAnimationMask = Mask;
        if (IsAttachedToSpace()) {
            RebuildSurfaceAnimation();
        }
    }

    void TPlanetSE::UpdateLightAngleFromStar() {
        SE_Space::TObjectSE* Obj{};
        if (IsAttachedToSpace() && static_cast<std::uint8_t>(IsRuins ^ 1)) {
            Obj = Space->FirstObject;
            while (Obj != nullptr) {
                if (pas::class_cast_if<SE_Star::TStarSE*>(Obj) != nullptr) {
                    SetLightAngle(System::Trunc(pas::real_divide(pas::real_divide(Math::ArcTan2(-(static_cast<long double>(Position.X) - Obj->Position.X), static_cast<long double>(Position.Y) - Obj->Position.Y) * 1.8E+2L, aMyFunction::GamePi) * 256.0L, 3.6E+2L)));
                    break;
                }
                Obj = Obj->Next;
            }
        }
    }

    void TPlanetSE::SurfaceAnimationFinished(GI_MessageLoop::TObjectGI* Sender) {
        if (IsRuins) {
            return;
        }
        ++SurfaceAnimationFrame;
        if (Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Lists[SurfaceAnimationIndex].Indices.length() - 1 < SurfaceAnimationFrame) {
            StartRandomSurfaceAnimation();
        } else {
            // Native retains this empty diagnostic branch.
            if (SurfaceAnimationFrame == 2) {
                static_cast<void>(SurfaceAnimationFrame == 2);
            }
            if (GR_Main::GiResourceVariant() == 1) {
                SurfaceImageControl->SetImagePath(Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Adverts[Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Lists[SurfaceAnimationIndex].Indices[SurfaceAnimationFrame]].Image1);
            } else {
                SurfaceImageControl->SetImagePath(Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Adverts[Globals::PlanetAdvertDefinitions[pas::shr(SurfaceAnimationMask, 24)].Lists[SurfaceAnimationIndex].Indices[SurfaceAnimationFrame]].Image2);
            }
            if (SurfaceImageControl->GaiImageControl != nullptr) {
                SurfaceImageControl->GaiImageControl->CycleCompleteCallback = pas::bind_method<&TPlanetSE::SurfaceAnimationFinished>(this);
            }
            SurfaceImageControl->RestartPlayback();
        }
    }

    void TPlanetSE::AdvanceRotationTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        if (!IsRuins) {
            SetSurfaceMapOffset(SurfaceMapOffset + SurfaceMapStep);
        }
    }

    // UserData selects cloud 1..3.
    void TPlanetSE::AdvanceCloudTimer(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        if (IsRuins) {
            return;
        }
        if (SurfaceMapStep > 0) {
            if (UserData == 1) {
                SetCloud1MapOffset(Cloud1MapOffset + Cloud1MapStep);
            } else if (UserData == 2) {
                SetCloud2MapOffset(Cloud2MapOffset + Cloud2MapStep);
            } else if (UserData == 3) {
                SetCloud3MapOffset(Cloud3MapOffset + Cloud3MapStep);
            }
        } else if (UserData == 1) {
            SetCloud1MapOffset(Cloud1MapOffset - Cloud1MapStep);
        } else if (UserData == 2) {
            SetCloud2MapOffset(Cloud2MapOffset - Cloud2MapStep);
        } else if (UserData == 3) {
            SetCloud3MapOffset(Cloud3MapOffset - Cloud3MapStep);
        }
    }

    std::uint8_t TPlanetSE::HitTestCursor() {
        if (!IsAttachedToSpace()) {
            return false;
        }
        std::uint8_t Result = false;
        if (IsRuins) {
            if (RuinsAnimationControl != nullptr) {
                return RuinsAnimationControl->HitTestPixel(RuinsAnimationControl->MessageLoop->GetCursorPoint());
            } else if (RuinsImageControl != nullptr) {
                return RuinsImageControl->HitTestPixel(RuinsImageControl->MessageLoop->GetCursorPoint());
            } else {
                return Result;
            }
        }
        return PlanetControl->HitTestCursor();
    }

    void TPlanetSE::DrawMap() {
        std::int32_t Capacity{};
        std::int32_t Index{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t CenterX{};
        std::int32_t CenterY{};
        std::int32_t ProjectedXi{};
        std::int32_t ProjectedYi{};
        std::int32_t Decision{};
        std::int32_t OrbitRadius{};
        std::int32_t Pitch{};
        float ProjectedX{};
        float ProjectedY{};
        PPlanetMapOrbitPoint Dest{};
        PPlanetMapOrbitPoint P0{};
        PPlanetMapOrbitPoint P1{};
        PPlanetMapOrbitPoint P2{};
        PPlanetMapOrbitPoint P3{};
        PPlanetMapOrbitPoint P4{};
        PPlanetMapOrbitPoint P5{};
        PPlanetMapOrbitPoint P6{};
        PPlanetMapOrbitPoint P7{};
        void* Pixels{};
        void* Scratch{};
        std::int32_t OctantCount{};
        float Intensity{};
        float IntensityStep{};
        {
            SE_Process::TProcessSE* cpp_with = pas::checked_cast<SE_Process::TProcessSE*>(Space->Process);
            if (cpp_with->RadarRange <= 0) {
                return;
            }
            if (IsRuins) {
                RuinsMinimapControl->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
            } else {
                Pixels = GR_Main::RenderScratchBuffer->GetPixels();
                Pitch = GR_Main::RenderScratchBuffer->PitchBytes;
                ProjectedX = static_cast<long double>(Position.X) * Space->MinimapScale;
                ProjectedY = static_cast<long double>(Position.Y) * Space->MinimapScale;
                ProjectedXi = System::Round(ProjectedX);
                ProjectedYi = System::Round(ProjectedY);
                if (MapOrbitPoints == nullptr) {
                    Capacity = pas::shr(GR_Main::RenderScratchBuffer->Width, 1);
                    Scratch = EC_Mem::AllocEC(Capacity * 8 * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint)));
                    OrbitRadius = System::Round(System::Sqrt(ProjectedXi * ProjectedXi + ProjectedYi * ProjectedYi));
                    CenterX = pas::shr(GR_Main::RenderScratchBuffer->Width, 1);
                    CenterY = pas::shr(GR_Main::RenderScratchBuffer->Height, 1);
                    Decision = 3 - 2 * OrbitRadius;
                    X = 0;
                    Y = OrbitRadius - 1;
                    P0 = static_cast<PPlanetMapOrbitPoint>(Scratch);
                    P1 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P0, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P2 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P1, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P3 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P2, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P4 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P3, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P5 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P4, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P6 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P5, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P7 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P6, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    OctantCount = 0;
                    do {
                        P0->Position.X = CenterX + X;
                        P0->Position.Y = CenterY - Y;
                        P0 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P0, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        P1->Position.X = CenterX + Y;
                        P1->Position.Y = CenterY - X;
                        P1 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P1, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        P2->Position.X = CenterX + Y;
                        P2->Position.Y = CenterY + X;
                        P2 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P2, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        P3->Position.X = CenterX + X;
                        P3->Position.Y = CenterY + Y;
                        P3 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P3, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        P4->Position.X = CenterX - X;
                        P4->Position.Y = CenterY + Y;
                        P4 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P4, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        P5->Position.X = CenterX - Y;
                        P5->Position.Y = CenterY + X;
                        P5 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P5, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        P6->Position.X = CenterX - Y;
                        P6->Position.Y = CenterY - X;
                        P6 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P6, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        P7->Position.X = CenterX - X;
                        P7->Position.Y = CenterY - Y;
                        P7 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P7, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        ++OctantCount;
                        if (Decision < 0) {
                            Decision = 4 * X + Decision + 6;
                        } else {
                            Decision = 4 * (X - Y) + Decision + 10;
                            --Y;
                        }
                        ++X;
                    } while (!(X > Y));
                    MapOrbitPointCount = OctantCount * 8;
                    MapOrbitPoints = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AllocEC(MapOrbitPointCount * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    Dest = MapOrbitPoints;
                    P0 = static_cast<PPlanetMapOrbitPoint>(Scratch);
                    P1 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Scratch, Capacity * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint)) + (OctantCount - 1) * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P2 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Scratch, Capacity * 2 * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P3 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Scratch, Capacity * 3 * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint)) + (OctantCount - 1) * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P4 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Scratch, Capacity * 4 * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P5 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Scratch, Capacity * 5 * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint)) + (OctantCount - 1) * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P6 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Scratch, Capacity * 6 * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    P7 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Scratch, Capacity * 7 * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint)) + (OctantCount - 1) * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    X = -10000;
                    Y = -10000;
                    // Preserve octant traversal and duplicate suppression at the joins.
                    for (auto cpp_range = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range.next(Index); ) {
                        if (P0->Position.X != X || P0->Position.Y != Y) {
                            Dest->Position.X = P0->Position.X;
                            Dest->Position.Y = P0->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P0 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P0, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range_2.next(Index); ) {
                        if (P1->Position.X != X || P1->Position.Y != Y) {
                            Dest->Position.X = P1->Position.X;
                            Dest->Position.Y = P1->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P1 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P1, -static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range_3.next(Index); ) {
                        if (P2->Position.X != X || P2->Position.Y != Y) {
                            Dest->Position.X = P2->Position.X;
                            Dest->Position.Y = P2->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P2 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P2, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range_4.next(Index); ) {
                        if (P3->Position.X != X || P3->Position.Y != Y) {
                            Dest->Position.X = P3->Position.X;
                            Dest->Position.Y = P3->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P3 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P3, -static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range_5.next(Index); ) {
                        if (P4->Position.X != X || P4->Position.Y != Y) {
                            Dest->Position.X = P4->Position.X;
                            Dest->Position.Y = P4->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P4 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P4, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range_6.next(Index); ) {
                        if (P5->Position.X != X || P5->Position.Y != Y) {
                            Dest->Position.X = P5->Position.X;
                            Dest->Position.Y = P5->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P5 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P5, -static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range_7.next(Index); ) {
                        if (P6->Position.X != X || P6->Position.Y != Y) {
                            Dest->Position.X = P6->Position.X;
                            Dest->Position.Y = P6->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P6 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P6, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, OctantCount - 1); cpp_range_8.next(Index); ) {
                        if (P7->Position.X != X || P7->Position.Y != Y) {
                            Dest->Position.X = P7->Position.X;
                            Dest->Position.Y = P7->Position.Y;
                            Dest->PixelOffset = 2 * Dest->Position.X + Dest->Position.Y * Pitch;
                            X = Dest->Position.X;
                            Y = Dest->Position.Y;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            --MapOrbitPointCount;
                        }
                        P7 = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(P7, -static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                    }
                    EC_Mem::FreeEC(Scratch);
                }
                Index = System::Round(pas::real_divide(aMyFunction::RadiansToHeadingDegrees(Math::ArcTan2(ProjectedX, -ProjectedY)), 3.6E+2L) * (MapOrbitPointCount - 1));
                Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(MapOrbitPoints, Index * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                Intensity = 255.0f;
                IntensityStep = pas::real_divide(-5.1E+2L, MapOrbitPointCount);
                while (Intensity > 0.0L) {
                    {
                        std::uint8_t round = System::Round(Intensity);
                        std::uint16_t readWordEC = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GR_Main::InterfaceBlendPalette, System::Round(Intensity) * 2));
                        GR_Main::BlendPixel16(EC_Mem::AddPointerOffset(Pixels, Dest->PixelOffset), readWordEC, round);
                    }
                    if (OrbitalVelocity > 0.0L) {
                        --Index;
                        if (Index < 0) {
                            Index = MapOrbitPointCount - 1;
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(MapOrbitPoints, Index * static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        } else {
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, -static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        }
                    } else {
                        ++Index;
                        if (Index >= MapOrbitPointCount) {
                            Index = 0;
                            Dest = MapOrbitPoints;
                        } else {
                            Dest = static_cast<PPlanetMapOrbitPoint>(EC_Mem::AddPointerOffset(Dest, static_cast<std::int32_t>(sizeof(TPlanetMapOrbitPoint))));
                        }
                    }
                    Intensity = static_cast<long double>(Intensity) + IntensityStep;
                }
                MinimapControl->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
            }
        }
    }

    // SmallPreview affects detached normal planets only. Attached planets reuse their current surface renderer; ruins use their static image.
    void TPlanetSE::RenderToBuffer(GI_MessageLoop::TMessageLoopGI* Screen, GR_GraphBuf::TGraphBufGR* Buffer, std::uint8_t SmallPreview) {
        Globals::TPlanetTempl* Template{};
        GI_Planet::TPlanetGI* Planet{};
        std::int32_t Index{};
        std::int32_t Count{};
        std::int32_t TemplateIndex{};
        std::int32_t Diameter{};
        EC_CacheBitmap::TCBitmapControlEC* Control{};
        std::int32_t RenderRadius{};
        Globals::TSputnikTempl* SatelliteTemplate{};
        if (IsRuins) {
            GI_GI::LoadGiByPathIntoGraphBuf(EC_Str::ExtractDelimitedPartW(pas::view(RuinsImagePath), 1, u","sv), Buffer);
        } else if (PlanetControl != nullptr) {
            PlanetControl->RenderSurfaceToBuffer(Buffer);
        } else {
            SatelliteTemplate = nullptr;
            Template = nullptr;
            if (!SmallPreview) {
                Count = pas::list_count(Globals::PlanetRenderTemplates);
                for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                    Template = pas::list_at<Globals::TPlanetTempl>(Globals::PlanetRenderTemplates, Index);
                    if (Template->Radius == Radius) {
                        break;
                    }
                }
                if (Template == nullptr) {
                    pas::raise(pas::make_exception<pas::Exception>("Error1 in TPlanetSE.DrawBufRGBA"_a));
                }
            }
            if (SmallPreview) {
                RenderRadius = 25;
            } else {
                RenderRadius = Radius * 2 / 2;
            }
            Diameter = RenderRadius * 2;
            if (Diameter < 1) {
                pas::raise(pas::make_exception<pas::Exception>("Error2 in TPlanetSE.DrawBufRGBA"_a));
            }
            Planet = pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Screen->ContentPanel);
            if (SmallPreview) {
                TemplateIndex = RenderRadius * 2 - GlobalsV::MinimumSatelliteTemplateRadius;
                SatelliteTemplate = pas::list_at<Globals::TSputnikTempl>(GlobalsV::SatelliteRenderTemplates, TemplateIndex);
                Planet->SetImageFromTemplate(SatelliteTemplate->MaskName, ImagePath, SatelliteTemplate->Radius);
            } else {
                Planet->SetImageWithRadius(Template->MaskName, ImagePath, Template->LightName, RenderRadius);
            }
            Planet->SetLightAngle(224);
            Planet->HitTestBounds = ClassesImports::Rect(-1, -1, Diameter, Diameter);
            Control = pas::construct_call<EC_CacheBitmap::TCBitmapControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            if (SmallPreview) {
                Control->SetCacheKey(pas::concat_wide({EC_Str::ExtractDelimitedPartW(pas::view(SatelliteTemplate->MaskName), 0, u"?"sv), EC_CacheBitmap::RgbaImagePathSuffix}));
            } else {
                Control->SetCacheKey(pas::concat_wide({Template->MaskName, EC_CacheBitmap::RgbaImagePathSuffix}));
            }
            EC_CacheBitmap::AcquireOrCreateBitmap(Control);
            Planet->RenderSurfaceToBuffer(Buffer);
            pas::free(Planet);
            Control->Release();
            pas::free(Control);
        }
    }

    void TPlanetSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        SE_Space::TObjectSE::LoadTemplate(Block);
        if (IsRuins) {
            RuinsAnimationPath = Block->GetParam(u"Image"sv);
            RuinsImagePath = Block->GetParam(u"ImageI"sv);
            RuinsMinimapPath = Block->GetParam(u"ImageMap"sv);
        } else {
            RotationTimerInterval = 100u;
            SurfaceMapStep = -1;
            ImagePath = Block->GetParam(u"Image"sv);
            MinimapImagePath = Block->GetParam(u"ImageMap"sv);
            ImageOrigin = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SmeImage"sv)));
            MinimapImageOrigin = GI_Main::GetPointGI(pas::view(Block->GetParam(u"SmeImageMap"sv)));
            Radius = SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Radius"sv)));
            if (Block->CountParams(u"Cloud0"_wref.get()) > 0) {
                Text = Block->GetParam(u"Cloud0"sv);
                Cloud1RelativeRotationSpeed = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv));
                Cloud1ImagePath = EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv);
            }
            if (Block->CountParams(u"Cloud1"_wref.get()) > 0) {
                Text = Block->GetParam(u"Cloud1"sv);
                Cloud2RelativeRotationSpeed = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv)));
                Cloud2ImagePath = EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv);
            }
            if (Block->CountParams(u"Cloud2"_wref.get()) > 0) {
                Text = Block->GetParam(u"Cloud2"sv);
                Cloud3RelativeRotationSpeed = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv)));
                Cloud3ImagePath = EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv);
            }
            if (Block->CountParams(u"AtmColor"_wref.get()) > 0) {
                Text = Block->GetParam(u"AtmColor"sv);
                AtmosphereColor = static_cast<std::uint8_t>(EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv))));
                AtmosphereColor |= pas::shl(static_cast<std::int32_t>(static_cast<std::uint8_t>(EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv))))), 8);
                AtmosphereColor |= pas::shl(static_cast<std::int32_t>(static_cast<std::uint8_t>(EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 2, u","sv))))), 16);
            }
            if (Block->CountParams(u"Space"_wref.get()) > 0) {
                Text = Block->GetParam(u"Space"sv);
                SpaceConfigValues[0] = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 0, u","sv)));
                SpaceConfigValues[1] = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 1, u","sv)));
                SpaceConfigValues[2] = EC_Str::ExtractDigitsToIntW(pas::view(EC_Str::ExtractDelimitedPartW(pas::view(Text), 2, u","sv)));
            }
            if (Block->CountParams(u"BG"_wref.get()) > 0) {
                BackgroundGraph = Block->GetParam(u"BG"sv);
            }
            if (Block->CountParams(u"Quest"_wref.get()) > 0) {
                QuestEnabled = GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"Quest"sv)));
            }
        }
    }

    void TPlanetSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
        if (IsRuins) {
            return;
        }
        if (Block->CountParams(u"SmeMap"_wref.get()) > 0) {
            SetSurfaceMapOffset(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"SmeMap"sv))));
        }
        if (Block->CountParams(u"AngleLight"_wref.get()) > 0) {
            SetLightAngle(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"AngleLight"sv))));
        }
        if (Block->CountParams(u"SpeedRotate"_wref.get()) > 0) {
            SetRotationTimerInterval(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"SpeedRotate"sv))));
        }
        if (Block->CountParams(u"StepRotate"_wref.get()) > 0) {
            SetSurfaceMapStep(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"StepRotate"sv))));
        }
    }

    void TPlanetSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        Globals::TPlanetTempl* Template{};
        std::int32_t Index{};
        std::int32_t Count{};
        GI_GAI::TgaiGI* Gai{};
        GI_Image::TImageGI* Image{};
        GI_Image::TImageGI* MapImage{};
        if (IsRuins) {
            if (GlobalsV::AnimShipFull || GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle) {
                Gai = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
                Gai->SetImagePath(RuinsAnimationPath);
                Gai->QueueImageLoad(PendingLoads);
                pas::free(Gai);
            } else {
                Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
                Image->SetImagePath(RuinsImagePath);
                Image->QueueImageLoad(PendingLoads);
                pas::free(Image);
            }
            MapImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
            MapImage->SetImagePath(RuinsMinimapPath);
            MapImage->QueueImageLoad(PendingLoads);
            pas::free(MapImage);
        } else {
            Template = nullptr;
            Count = pas::list_count(Globals::PlanetRenderTemplates);
            for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
                Template = pas::list_at<Globals::TPlanetTempl>(Globals::PlanetRenderTemplates, Index);
                if (Template->Radius == Radius) {
                    break;
                }
            }
            if (Template == nullptr) {
                pas::raise(pas::make_exception<pas::Exception>("Error in TPlanetSE.BuildLoadList"_a));
            }
            {
                GI_Planet::TPlanetGI* cpp_with = pas::construct_call<GI_Planet::TPlanetGI>(GI_Planet::TPlanetGI_Create, Owner);
                cpp_with->SetImageWithRadius(Template->SmallMaskName, ImagePath, Template->SmallLightName, Radius);
                cpp_with->QueueImageLoad(PendingLoads);
                pas::free(cpp_with);
            }
            {
                GI_AlphaImage::TAlphaImageGI* cpp_with_2 = pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, Owner);
                cpp_with_2->SetImagePath(MinimapImagePath);
                cpp_with_2->QueueImageLoad(PendingLoads);
                pas::free(cpp_with_2);
            }
        }
    }

} // namespace SE_Planet
