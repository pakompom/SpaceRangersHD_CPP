#include "layout/GI_PSWeapon17Kafacitor.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Rect.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon17Kafacitor.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

// Native Kafacitor beam: linked control points and interpolated lateral movement.
namespace GI_PSWeapon17Kafacitor {
    pas::DynArray<GI_PSWeapon17Kafacitor::TKafacitorPalette> KafacitorPalettes{};

    pas::DynArray<GI_PSWeapon17Kafacitor::TGAISet> KafacitorAnimationPaths{};

    void LoadKafacitorPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.16.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        KafacitorPalettes.set_length(Count);
        KafacitorAnimationPaths.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 1); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        KafacitorPalettes[Index][ColorIndex] = ([&] {
                            double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                            double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                            double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                            return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW, extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                        }());
                    }
                }
                if (PaletteBlock->CountParams(u"GAI"_wref.get()) > 0) {
                    KafacitorAnimationPaths[Index][0] = PaletteBlock->GetParam(u"GAI"_wref.get());
                }
            }
        }
    }

    void TPSWeapon17Kafacitor_Create(TPSWeapon17Kafacitor* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = 10;
        Self->LengthScale = 1.0;
        Self->OriginalLength = 1.0;
        Self->RemainingTicks = 40;
        Self->UpdateProjectionBounds();
        Self->SetColors(KafacitorPalettes[APaletteIndex][0], KafacitorPalettes[APaletteIndex][1]);
    }

    void TPSWeapon17Kafacitor_Destroy(TPSWeapon17Kafacitor* Self) {
        // Native destructor does not release the allocated particle list.
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon17Kafacitor::SetColors(std::uint16_t Primary, std::uint16_t Secondary) {
        PrimaryColor = Primary;
        SecondaryColor = Secondary;
    }

    void TPSWeapon17Kafacitor::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon17Kafacitor::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSWeapon17Kafacitor::UpdateProjectionBounds() {
        float Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
        if (Distance == 0.0L) {
            Distance = 1.0f;
        }
        float Cosine = pas::real_divide(-(TargetPoint.Y - LocalPosition.Y), Distance);
        float Sine = pas::real_divide(TargetPoint.X - LocalPosition.X, Distance);
        float A = static_cast<long double>(-HalfWidth - 12) * Cosine - static_cast<long double>(-Distance) * Sine;
        float B = static_cast<long double>(HalfWidth + 12) * Cosine - static_cast<long double>(-Distance) * Sine;
        float C = static_cast<long double>(-HalfWidth - 12) * Cosine;
        float D = static_cast<long double>(HalfWidth + 12) * Cosine;
        ProjectionBounds.Left = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Right = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
        A = static_cast<long double>(-HalfWidth - 12) * Sine + static_cast<long double>(-Distance) * Cosine;
        B = static_cast<long double>(HalfWidth + 12) * Sine + static_cast<long double>(-Distance) * Cosine;
        C = static_cast<long double>(-HalfWidth - 12) * Sine;
        // Native uses Cosine for this final corner as well.
        D = static_cast<long double>(HalfWidth + 12) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSWeapon17Kafacitor::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSWeapon17Kafacitor::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PKafacitorParticle TPSWeapon17Kafacitor::AddParticle() {
        PKafacitorParticle Particle = static_cast<PKafacitorParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TKafacitorParticle))));
        if (LastParticle != nullptr) {
            LastParticle->Next = Particle;
        }
        Particle->Prev = LastParticle;
        Particle->Next = nullptr;
        LastParticle = Particle;
        if (FirstParticle == nullptr) {
            FirstParticle = Particle;
        }
        return Particle;
    }

    void TPSWeapon17Kafacitor::InvalidateRect(Types::TRect Rect) {
        Types::TPoint Target{};
        Types::TRect Intersection{};
        MessageLoop->UpdateRects->AddScreenClippedRect(HitTestBounds, Parent->ToAbsolutePoint(LocalPosition), Parent->ToAbsolutePoint(TargetPoint));
        Target = Parent->ToAbsolutePoint(TargetPoint);
        Rect.Left = Target.X - 24;
        Rect.Right = Target.X + 24;
        Rect.Top = Target.Y - 24;
        Rect.Bottom = Target.Y + 24;
        if (EC_Struct::IntersectRects(Intersection, Rect, GR_Main::GameScreenRect)) {
            MessageLoop->QueueUpdateRect(Intersection);
        }
    }

    void TPSWeapon17Kafacitor::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t I{};
        std::int32_t Pass{};
        std::int32_t Direction{};
        std::int32_t SegmentPosition{};
        std::int32_t SegmentLength{};
        std::int32_t Offset{};
        float Distance{};
        float Spacing{};
        PKafacitorParticle Particle{};
        PKafacitorParticle Current{};
        PKafacitorParticle PreviousControl{};
        PKafacitorParticle NextControl{};
        Invalidate();
        if (FirstParticle == nullptr && RemainingTicks >= 24) {
            I = 0;
            Distance = System::Round(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)));
            OriginalLength = Distance;
            if (OriginalLength == 0.0L) {
                OriginalLength = 1.0;
            }
            HalfWidth = std::max<std::int64_t>(static_cast<std::int64_t>(HalfWidth), System::Round(0.07L * OriginalLength));
            Spacing = pas::real_divide(Distance, 2.0E+1L);
            SegmentPosition = 0;
            SegmentLength = 0;
            LengthScale = 1.0;
            Direction = 1;
            for (Pass = 1; Pass <= 2; ++Pass) {
                while (Direction == 1 && I <= 4.0L * Distance || Direction == -1 && I >= 0) {
                    Particle = AddParticle();
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(0.0f, I / 4));
                    if (Direction == 1) {
                        Particle->Color = PrimaryColor;
                    } else {
                        Particle->Color = SecondaryColor;
                    }
                    Particle->Alpha = 255;
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(0.0f, 0.0f));
                    if (I == 0 || static_cast<long double>(I) == Distance) {
                        Particle->State = 1;
                        SegmentPosition = 0;
                    } else if (SegmentPosition < 4.0L * Spacing) {
                        Particle->State = 2;
                        ++SegmentPosition;
                    } else {
                        Particle->State = 3;
                        SegmentPosition = 0;
                        if (SegmentLength == 1) {
                            Particle->Velocity.X = 1.0f;
                        } else {
                            Particle->Velocity.X = -1.0f;
                        }
                        SegmentLength = 1 - SegmentLength;
                    }
                    I += Direction;
                }
                Direction = -Direction;
            }
        } else {
            Distance = OriginalLength;
            LengthScale = pas::real_divide(System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y)), OriginalLength);
            UpdateHitTestBounds();
            Particle = FirstParticle;
            NextControl = Particle;
            PreviousControl = Particle;
            SegmentLength = 1;
            Offset = 0;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                if (pas::in_set<1, 1, 3, 3>(Current->State) && Particle != nullptr) {
                    PreviousControl = Current;
                    SegmentLength = 1;
                    Offset = 0;
                    NextControl = Particle;
                    while (!pas::in_set<1, 1, 3, 3>(NextControl->State)) {
                        NextControl = NextControl->Next;
                        ++SegmentLength;
                    }
                    if (std::fabs(static_cast<pas::Extended>(NextControl->Position.X)) > 0.1L * Distance && static_cast<long double>(NextControl->Position.X) * NextControl->Velocity.X > 0.0L) {
                        NextControl->Velocity.X = NextControl->Velocity.X * -1.0L;
                    } else if (static_cast<long double>(NextControl->Position.X) * NextControl->Velocity.X > 0.0L && ([&] {
                        pas::Extended cpp_left = pas::random(101, &System::RandSeed);
                        return cpp_left < (pas::real_divide(std::fabs(static_cast<pas::Extended>(NextControl->Position.X)), 0.05L * Distance) - 1.0L) * 1.0E+2L;
                    }())) {
                        NextControl->Velocity.X = NextControl->Velocity.X * -1.0L;
                    } else if (pas::random(101, &System::RandSeed) < 15) {
                        NextControl->Velocity.X = NextControl->Velocity.X * -1.0L;
                    }
                } else {
                    ++Offset;
                }
                {
                    pas::Extended cpp_left_2 = (1.0L - pas::real_divide(Offset, SegmentLength)) * PreviousControl->Velocity.X + Current->Position.X;
                    Current->Position.X = cpp_left_2 + pas::real_divide(static_cast<long double>(Offset) * NextControl->Velocity.X, SegmentLength);
                }
                if (RemainingTicks < 24) {
                    if (Current->Alpha - 10 > 0) {
                        Current->Alpha -= 10;
                    } else {
                        Current->Alpha = 0;
                    }
                }
            }
        }
        --RemainingTicks;
    }

    void TPSWeapon17Kafacitor::Draw(Types::TRect ClipRect) {
        float PX{};
        float PY{};
        std::int32_t X{};
        std::int32_t Y = -(TargetPoint.Y - LocalPosition.Y);
        if (Y == 0) {
            ++Y;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, Y);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PKafacitorParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                PX = static_cast<long double>(Particle->Position.X) * LengthScale;
                PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Particle != nullptr) {
                PX = static_cast<long double>(Particle->Position.X) * LengthScale;
                PY = static_cast<long double>(-Particle->Position.Y) * LengthScale;
                X = System::Round(static_cast<long double>(PX) * Cosine - static_cast<long double>(PY) * Sine + AbsolutePosition.X);
                Y = System::Round(static_cast<long double>(PX) * Sine + static_cast<long double>(PY) * Cosine + AbsolutePosition.Y);
                if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                    GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                }
                Particle = Particle->Next;
            }
        }
    }

    void TPSWeapon17Kafacitor::p_destroy() {
        GI_PSWeapon17Kafacitor::TPSWeapon17Kafacitor_Destroy(this);
    }

} // namespace GI_PSWeapon17Kafacitor
