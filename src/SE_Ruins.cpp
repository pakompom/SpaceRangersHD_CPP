#include "layout/SE_Ruins.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Data.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SE_Process.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_Image.hpp"
#include "units/GR_Main.hpp"
#include "units/Globals.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Ruins.hpp"
#include "units/SE_Space.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native class and methods:.
namespace SE_Ruins {
    void TRuinsSE_Create(TRuinsSE* Self, pas::WideString GraphKey, Types::TPoint UnusedPosition) {
        if (EC_Str::CountDelimitedPartsW(GraphKey, u","_wref.get()) > 1) {
            SE_Space::TObjectSE_Create(Self, EC_Str::ExtractDelimitedPartW(GraphKey, 0, u","_wref.get()), UnusedPosition);
            Self->AlphaLimit = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(GraphKey, 1, u","_wref.get()));
        } else {
            SE_Space::TObjectSE_Create(Self, GraphKey, UnusedPosition);
            Self->AlphaLimit = 255;
        }
        Self->HasTransitionImages = GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Self->ImagePath, u"To"})) && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({Self->ImagePath, u"From"}));
        Self->State = 0;
        Self->Alpha = 255;
    }

    void TRuinsSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (!IsAttachedToSpace()) {
            ConfigureLoopSound(pas::concat_wide({u"Ruins.", EC_Str::ExtractDelimitedPartW(ImagePath, EC_Str::CountDelimitedPartsW(ImagePath, u"."_wref.get()) - 1, u"."_wref.get())}));
            ConfigureRandomSound(pas::concat_wide({u"Ruins.", EC_Str::ExtractDelimitedPartW(ImagePath, EC_Str::CountDelimitedPartsW(ImagePath, u"."_wref.get()) - 1, u"."_wref.get())}));
            SE_Space::TObjectSE::AttachToSpace(ASpace);
            if (GlobalsV::AnimShipFull || GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle) {
                Animation = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Space->MapPanel);
                if (State == 3) {
                    Animation->SetImagePath(pas::concat_wide({ImagePath, u"From"}));
                    Animation->CycleCompleteCallback = pas::bind_method<&TRuinsSE::AnimationCycleComplete>(this);
                    FrameIndex = 0;
                } else if (State == 2) {
                    Animation->SetImagePath(pas::concat_wide({ImagePath, u"To"}));
                    Animation->CycleCompleteCallback = pas::bind_method<&TRuinsSE::AnimationCycleComplete>(this);
                    State = 4;
                    FrameIndex = 0;
                } else {
                    Animation->SetImagePath(ImagePath);
                }
                {
                    WindowsSdk::TPoint contentSize = Animation->GetContentSize();
                    GI_GAI::TgaiGI* animation = Animation;
                    animation->SetSize(contentSize);
                }
                Animation->SetOrigin(EC_Struct::HalfPoint(Animation->ClientSize));
                Animation->SetDepthByName(DepthExpression);
                Animation->SetPosition(EC_Struct::TruncatePointF(Position));
                Animation->SetPositionModeW(true);
                Animation->SequenceIndex = 0;
                Animation->UpdateAutoGeometry();
                if (!pas::in_set<2, 3>(State)) {
                    FrameIndex = pas::random(Animation->SequenceFrameCount - 1, &System::RandSeed);
                }
                Animation->SetSequenceFrame(FrameIndex);
                Animation->RestartPlayback();
                Animation->SetAlpha(pas::shr(std::min<std::int32_t>(static_cast<std::int32_t>(Alpha), static_cast<std::int32_t>(AlphaLimit)), Space->AlphaShift));
                if (!KeepSize) {
                    Size = Animation->ClientSize;
                }
            } else {
                StaticImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Space->MapPanel);
                StaticImage->SetImagePath(StaticImagePath);
                {
                    Types::TPoint contentSize_2 = StaticImage->GetContentSize();
                    GI_Image::TImageGI* staticImage = StaticImage;
                    staticImage->SetSize(contentSize_2);
                }
                StaticImage->SetOrigin(EC_Struct::HalfPoint(StaticImage->ClientSize));
                StaticImage->SetDepthByName(DepthExpression);
                StaticImage->SetPosition(EC_Struct::TruncatePointF(Position));
                StaticImage->SetPositionModeW(true);
                StaticImage->SetAlpha(pas::shr(std::min<std::int32_t>(static_cast<std::int32_t>(Alpha), static_cast<std::int32_t>(AlphaLimit)), Space->AlphaShift));
                if (State == 3) {
                    FadeTimer = Space->Screen->ScheduleCallbackTimer(20, 20, pas::bind_method<&TRuinsSE::AdvanceFade>(this), 0);
                    SetAlpha(0);
                } else if (State == 2) {
                    FadeTimer = Space->Screen->ScheduleCallbackTimer(20, 20, pas::bind_method<&TRuinsSE::AdvanceFade>(this), 0);
                    SetAlpha(255);
                }
                if (!KeepSize) {
                    Size = StaticImage->ClientSize;
                }
            }
            MinimapImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Globals::SpaceObjectUiLoop->ContentPanel);
            MinimapImage->SetPositionModeW(true);
            MinimapImage->SetDepthByName(DepthExpression);
            MinimapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(Position.X) * Space->MinimapScale, static_cast<long double>(Position.Y) * Space->MinimapScale)));
            MinimapImage->SetImagePath(MinimapImagePath);
            {
                Types::TPoint contentSize_3 = MinimapImage->GetContentSize();
                GI_Image::TImageGI* minimapImage = MinimapImage;
                minimapImage->SetSize(contentSize_3);
            }
            MinimapImage->SetOrigin(EC_Struct::HalfPoint(MinimapImage->ClientSize));
        }
    }

    void TRuinsSE::DetachFromSpace() {
        if (IsAttachedToSpace()) {
            if (FadeTimer != nullptr) {
                Space->Screen->CancelCallbackTimer(FadeTimer);
                FadeTimer = nullptr;
            }
            if (Animation != nullptr) {
                FrameIndex = Animation->SequenceFrame;
                pas::free(Animation);
                Animation = nullptr;
            }
            if (StaticImage != nullptr) {
                StaticImage->SetActive(false);
                pas::free(StaticImage);
                StaticImage = nullptr;
            }
            if (MinimapImage != nullptr) {
                pas::free(MinimapImage);
                MinimapImage = nullptr;
            }
            SE_Space::TObjectSE::DetachFromSpace();
        }
    }

    void TRuinsSE::SetState(std::int32_t Value) {
        if (!IsAttachedToSpace()) {
            State = Value;
            return;
        }
        if (State == Value) {
            return;
        }
        State = Value;
        if (Animation != nullptr) {
            if (!pas::in_set<0, 1>(State)) {
                Animation->CycleCompleteCallback = pas::bind_method<&TRuinsSE::AnimationCycleComplete>(this);
            }
        } else if (StaticImage != nullptr) {
            if (pas::in_set<2, 3>(State)) {
                FadeTimer = Space->Screen->ScheduleCallbackTimer(20, 20, pas::bind_method<&TRuinsSE::AdvanceFade>(this), 0);
            } else if (FadeTimer != nullptr) {
                Space->Screen->CancelCallbackTimer(FadeTimer);
                FadeTimer = nullptr;
            }
        }
    }

    void TRuinsSE::AnimationCycleComplete(GI_MessageLoop::TObjectGI* Sender) {
        if (State == 2) {
            Animation->SetImagePath(pas::concat_wide({ImagePath, u"To"}));
            Animation->SequenceIndex = 0;
            Animation->UpdateAutoGeometry();
            Animation->SetSequenceFrame(0);
            Animation->RestartPlayback();
            State = 4;
        } else if (State == 3 || State == 1) {
            Animation->SetImagePath(ImagePath);
            Animation->SequenceIndex = 0;
            Animation->UpdateAutoGeometry();
            Animation->SetSequenceFrame(0);
            Animation->RestartPlayback();
            Animation->CycleCompleteCallback = nullptr;
            State = 1;
        } else {
            Animation->CycleCompleteCallback = nullptr;
            DetachFromSpace();
        }
    }

    void TRuinsSE::AdvanceFade(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        if (State == 2) {
            SetAlpha(std::max<std::int32_t>(0, GetAlpha() - 5));
            if (GetAlpha() == 0) {
                State = 1;
                DetachFromSpace();
            }
        } else {
            SetAlpha(std::min<std::int32_t>(255, GetAlpha() + 5));
            if (GetAlpha() == 255) {
                State = 1;
                if (FadeTimer != nullptr) {
                    Space->Screen->CancelCallbackTimer(FadeTimer);
                    FadeTimer = nullptr;
                }
            }
        }
    }

    void TRuinsSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            if (StaticImage != nullptr) {
                StaticImage->SetPosition(EC_Struct::TruncatePointF(APosition));
            }
            if (Animation != nullptr) {
                Animation->SetPosition(EC_Struct::TruncatePointF(APosition));
            }
            MinimapImage->SetPosition(EC_Struct::TruncatePointF(EC_Struct::MakePointF(static_cast<long double>(APosition.X) * Space->MinimapScale, static_cast<long double>(APosition.Y) * Space->MinimapScale)));
        }
    }

    void TRuinsSE::SetDepth(float Value) {
        if (Animation != nullptr) {
            Animation->SetDepth(Value);
        }
        if (StaticImage != nullptr) {
            StaticImage->SetDepth(Value);
        }
    }

    float TRuinsSE::GetDepth() {
        float Result = 0.0f;
        if (Animation != nullptr) {
            Result = Animation->Depth;
        }
        if (StaticImage != nullptr) {
            return StaticImage->Depth;
        }
        return Result;
    }

    std::uint8_t TRuinsSE::GetAlpha() {
        return Alpha;
    }

    void TRuinsSE::SetAlpha(std::uint8_t Value) {
        Alpha = Value;
        if (IsAttachedToSpace()) {
            if (Animation != nullptr) {
                Animation->SetAlpha(pas::shr(std::min<std::int32_t>(static_cast<std::int32_t>(Alpha), static_cast<std::int32_t>(AlphaLimit)), Space->AlphaShift));
            }
            if (StaticImage != nullptr) {
                StaticImage->SetAlpha(pas::shr(std::min<std::int32_t>(static_cast<std::int32_t>(Alpha), static_cast<std::int32_t>(AlphaLimit)), Space->AlphaShift));
            }
        }
    }

    void TRuinsSE::DrawMap() {
        SE_Process::TProcessSE* CurrentProcess = pas::checked_cast<SE_Process::TProcessSE*>(Space->Process);
        if (CurrentProcess->RadarRange > 0) {
            MinimapImage->Draw(ClassesImports::Rect(0, 0, GR_Main::RenderScratchBuffer->Width, GR_Main::RenderScratchBuffer->Height));
        }
    }

    EC_Struct::TPointF TRuinsSE::GetWeaponPortPoint(std::uint32_t Seed) {
        EC_Struct::TPointF Result{};
        EC_Struct::TPointF Point{};
        if (WeaponPortCount < 1) {
            Result = Position;
            return Result;
        }
        Point = WeaponPorts[1 + pas::imod(pas::sqr(static_cast<std::int32_t>(Seed)) / 11, WeaponPortCount)];
        Result.X = static_cast<long double>(Position.X) + Point.X;
        Result.Y = static_cast<long double>(Position.Y) + Point.Y;
        return Result;
    }

    std::uint8_t TRuinsSE::HitTestCursor() {
        std::uint8_t Result = false;
        if (IsAttachedToSpace()) {
            if (Animation != nullptr) {
                return Animation->HitTestPixel(Animation->MessageLoop->GetCursorPoint());
            } else if (StaticImage != nullptr) {
                return StaticImage->HitTestPixel(StaticImage->MessageLoop->GetCursorPoint());
            } else {
                return Result;
            }
        }
        return Result;
    }

    void TRuinsSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        std::int32_t Index{};
        auto ParseRuinsPoint = [&](pas::WideString PointText) -> Types::TPoint {
            Types::TPoint Result{};
            if (EC_Str::CountDelimitedPartsW(PointText, u","_wref.get()) < 2) {
                pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"GetPointGI. tstr=", PointText}))));
            }
            Result = ([&] {
                std::int32_t strToInt = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(PointText, 1, u","_wref.get())));
                std::int32_t strToInt_2 = SysUtils::StrToInt(static_cast<pas::AnsiString>(EC_Str::ExtractDelimitedPartW(PointText, 0, u","_wref.get())));
                return ClassesImports::Point(strToInt_2, strToInt);
            }());
            return Result;
        };
        auto ParseRuinsEnabled = [&](pas::WideString Name) -> std::uint8_t {
            return Name == u"Yes" || Name == u"yes" || Name == u"True" || Name == u"true" || Name == u"TRUE" || Name == u"1";
        };
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"_wref.get());
        StaticImagePath = Block->GetParam(u"ImageI"_wref.get());
        MinimapImagePath = Block->GetParam(u"ImageMap"_wref.get());
        HasTransitionImages = GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({ImagePath, u"To"})) && GR_Main::CacheDataRoot->FileExistsByPath(pas::concat_wide({ImagePath, u"From"}));
        if (Block->CountParams(u"PanelPartnerImage"_wref.get()) > 0) {
            PanelPartnerImage = Block->GetParam(u"PanelPartnerImage"_wref.get());
        } else {
            PanelPartnerImage = pas::WideString();
        }
        if (Block->CountParams(u"HideOnStarInfo"_wref.get()) > 0) {
            HideOnStarInfo = ParseRuinsEnabled(Block->GetParam(u"HideOnStarInfo"_wref.get()));
        } else {
            HideOnStarInfo = false;
        }
        WeaponPortCount = 0u;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, 10); cpp_range.next(Index); ) {
            if (Block->CountParams(pas::concat_wide({u"WeaponPort", EC_Str::IntToWideString(Index)})) <= 0) {
                break;
            }
            WeaponPorts[Index] = EC_Struct::PointToPointF(ParseRuinsPoint(Block->GetParam(pas::concat_wide({u"WeaponPort", EC_Str::IntToWideString(Index)}))));
            ++WeaponPortCount;
        }
    }

    void TRuinsSE::ApplyConfig(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::ApplyConfig(Block);
    }

    void TRuinsSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        GI_GAI::TgaiGI* Anim{};
        GI_Image::TImageGI* Image{};
        if (GlobalsV::AnimShipFull || GlobalsV::CurrentScreenId == GlobalsV::screenArcadeBattle) {
            Anim = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, Owner);
            Anim->SetImagePath(ImagePath);
            Anim->QueueImageLoad(PendingLoads);
            pas::free(Anim);
        } else {
            Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
            Image->SetImagePath(StaticImagePath);
            Image->QueueImageLoad(PendingLoads);
            pas::free(Image);
        }
        GI_Image::TImageGI* MapImage = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
        MapImage->SetImagePath(MinimapImagePath);
        MapImage->QueueImageLoad(PendingLoads);
        pas::free(MapImage);
    }

} // namespace SE_Ruins
