#include "layout/GI_PSEyes.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/GR_Rect.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSEyes.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/MathImports.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

// Native eye/lightning effect, particles and dormant line-list storage.
namespace GI_PSEyes {
    pas::DynArray<GI_PSEyes::TEyesPalette> EyesPalettes{};

    pas::DynArray<std::int32_t> EyesWidths{};

    pas::DynArray<std::int32_t> EyesSegmentLengths{};

    pas::DynArray<std::int32_t> EyesDispersions{};

    pas::DynArray<std::int32_t> EyesStartingAlphas{};

    void LoadEyesPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t BlockCount{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->FindBlockByPath(u"SE.Weapon.Eyes.Palettes"_wref.get());
        std::int32_t Count = 0;
        if (Block != nullptr) {
            BlockCount = Block->GetBlockCount();
            for (auto cpp_range = pas::for_to<std::int32_t>(0, BlockCount - 1); cpp_range.next(Index); ) {
                Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
            }
        }
        if (Block == nullptr || Count == 0) {
            EyesPalettes.set_length(1);
            EyesWidths.set_length(1);
            EyesSegmentLengths.set_length(1);
            EyesDispersions.set_length(1);
            EyesStartingAlphas.set_length(1);
            EyesPalettes[0][0] = GR_Main::CurrentPixelFormat->PackNormalizedRgb(0.9, 0.7, 1.0);
            EyesPalettes[0][1] = GR_Main::CurrentPixelFormat->PackNormalizedRgb(0.25, 0.15, 0.6);
            EyesWidths[0] = 32;
            EyesSegmentLengths[0] = 32;
            EyesDispersions[0] = 5;
            EyesStartingAlphas[0] = 64;
        } else {
            EyesPalettes.set_length(Count);
            EyesWidths.set_length(Count);
            EyesSegmentLengths.set_length(Count);
            EyesDispersions.set_length(Count);
            EyesStartingAlphas.set_length(Count);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
                EyesPalettes[Index][0] = GR_Main::CurrentPixelFormat->PackNormalizedRgb(0.9, 0.7, 1.0);
                EyesPalettes[Index][1] = GR_Main::CurrentPixelFormat->PackNormalizedRgb(0.25, 0.15, 0.6);
                EyesWidths[Index] = 32;
                EyesSegmentLengths[Index] = 32;
                EyesDispersions[Index] = 5;
                EyesStartingAlphas[Index] = 64;
                Text = pas::wide_int_to_str(Index);
                if (Block->CountBlocks(Text) != 0) {
                    PaletteBlock = Block->GetBlockByPath(Text);
                    if (PaletteBlock->CountParams(u"Color"_wref.get()) > 0) {
                        Text = PaletteBlock->GetParam(u"Color"_wref.get());
                        EyesPalettes[Index][0] = ([&] {
                            double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                            double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                            double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                            return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW, extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                        }());
                    }
                    if (PaletteBlock->CountParams(u"ColorDark"_wref.get()) > 0) {
                        Text = PaletteBlock->GetParam(u"ColorDark"_wref.get());
                        EyesPalettes[Index][1] = ([&] {
                            double extractDecimalToSingleW_4 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                            double extractDecimalToSingleW_5 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                            double extractDecimalToSingleW_6 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                            return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW_4, extractDecimalToSingleW_5, extractDecimalToSingleW_6);
                        }());
                    }
                    if (PaletteBlock->CountParams(u"Width"_wref.get()) > 0) {
                        EyesWidths[Index] = EC_Str::ExtractDigitsToIntW(PaletteBlock->GetParam(u"Width"_wref.get()));
                    }
                    if (PaletteBlock->CountParams(u"SegmentLength"_wref.get()) > 0) {
                        EyesSegmentLengths[Index] = EC_Str::ExtractDigitsToIntW(PaletteBlock->GetParam(u"SegmentLength"_wref.get()));
                    }
                    if (PaletteBlock->CountParams(u"Dispersion"_wref.get()) > 0) {
                        EyesDispersions[Index] = EC_Str::ExtractDigitsToIntW(PaletteBlock->GetParam(u"Dispersion"_wref.get()));
                    }
                    if (PaletteBlock->CountParams(u"StartingAlpha"_wref.get()) > 0) {
                        EyesStartingAlphas[Index] = EC_Str::ExtractDigitsToIntW(PaletteBlock->GetParam(u"StartingAlpha"_wref.get()));
                    }
                }
            }
        }
    }

    void TPSEyesGI_Create(TPSEyesGI* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t PaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->HalfWidth = EyesWidths[PaletteIndex];
        Self->SegmentLength = EyesSegmentLengths[PaletteIndex];
        Self->Dispersion = EyesDispersions[PaletteIndex];
        Self->RemainingTicks = 60;
        Self->LifetimeTicks = Self->RemainingTicks;
        Self->PrimaryColor = EyesPalettes[PaletteIndex][0];
        Self->SecondaryColor = EyesPalettes[PaletteIndex][1];
        Self->BeamTicks = 20;
        Self->UpdateProjectionBounds();
        Self->FirstLine = nullptr;
        Self->LastLine = nullptr;
        Self->StartingAlpha = EyesStartingAlphas[PaletteIndex];
    }

    void TPSEyesGI_Destroy(TPSEyesGI* Self) {
        Self->InvalidateRect(Self->HitTestBounds);
        Self->ClearLines();
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSEyesGI::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            UpdateProjectionBounds();
        }
    }

    void TPSEyesGI::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSEyesGI::UpdateProjectionBounds() {
        std::int32_t DY = -(TargetPoint.Y - LocalPosition.Y);
        if (DY == 0) {
            ++DY;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, DY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        float Distance = System::Sqrt(pas::sqr(TargetPoint.X - LocalPosition.X) + pas::sqr(TargetPoint.Y - LocalPosition.Y));
        float A = static_cast<long double>(-HalfWidth) * Cosine - static_cast<long double>(-Distance) * Sine;
        float B = static_cast<long double>(HalfWidth) * Cosine - static_cast<long double>(-Distance) * Sine;
        float C = static_cast<long double>(-HalfWidth) * Cosine;
        float D = static_cast<long double>(HalfWidth) * Cosine;
        ProjectionBounds.Left = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Right = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
        A = static_cast<long double>(-HalfWidth) * Sine + static_cast<long double>(-Distance) * Cosine;
        B = static_cast<long double>(HalfWidth) * Sine + static_cast<long double>(-Distance) * Cosine;
        C = static_cast<long double>(-HalfWidth) * Sine;
        // Native uses Cosine for this final corner as well.
        D = static_cast<long double>(HalfWidth) * Cosine;
        ProjectionBounds.Top = MathImports::Floor(pas::real_min<float>(pas::real_min<float>(pas::real_min<float>(A, B), C), D));
        ProjectionBounds.Bottom = MathImports::Ceil(pas::real_max<float>(pas::real_max<float>(pas::real_max<float>(A, B), C), D));
    }

    void TPSEyesGI::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSEyesGI::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PEyesParticle TPSEyesGI::AddParticle() {
        PEyesParticle Particle = static_cast<PEyesParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TEyesParticle))));
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

    void TPSEyesGI::ClearParticles() {
        PEyesParticle Current{};
        PEyesParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSEyesGI::ClearLines() {
        PEyesLine Current{};
        PEyesLine Particle = FirstLine;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstLine = nullptr;
        LastLine = nullptr;
    }

    // Native empty override.
    void TPSEyesGI::Invalidate() {
    }

    void TPSEyesGI::InvalidateRect(Types::TRect Rect) {
        Types::TPoint Target{};
        Types::TRect Intersection{};
        MessageLoop->UpdateRects->AddScreenClippedRect(HitTestBounds, Parent->ToAbsolutePoint(LocalPosition), Parent->ToAbsolutePoint(TargetPoint));
        Target = Parent->ToAbsolutePoint(TargetPoint);
        Rect.Left = Target.X - HalfWidth;
        Rect.Right = Target.X + HalfWidth;
        Rect.Top = Target.Y - HalfWidth;
        Rect.Bottom = Target.Y + HalfWidth;
        if (EC_Struct::IntersectRects(Intersection, Rect, GR_Main::GameScreenRect)) {
            MessageLoop->QueueUpdateRect(Intersection);
        }
    }

    void TPSEyesGI::EmitBurst(Types::TPoint Point, std::int32_t Radius) {
        std::int32_t X{};
        std::int32_t Y{};
        PEyesParticle Particle{};
        for (auto cpp_range = pas::for_to<std::int32_t>(-Radius, Radius); cpp_range.next(Y); ) {
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(-Radius + pas::abs(Y), Radius - pas::abs(Y)); cpp_range_2.next(X); ) {
                Particle = AddParticle();
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Origin, EC_Struct::MakePointF(Point.X, Point.Y));
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, Particle->Origin);
                Particle->Color = PrimaryColor;
                Particle->Alpha = 255;
                Particle->State = 1;
                Particle->Countdown = 7;
                {
                    float cpp_arg = pas::real_divide(X, Radius) * 1.1L;
                    float cpp_arg_2 = pas::real_divide(Y, Radius) * 1.1L;
                    pas::store_unaligned<EC_Struct::TPointF>(&Particle->Velocity, EC_Struct::MakePointF(cpp_arg, cpp_arg_2));
                }
                if (Particle->Velocity.X < 0.0L) {
                    pas::Extended cpp_right = pas::real_divide(pas::random(11, &System::RandSeed), 16.0L);
                    Particle->Velocity.X = Particle->Velocity.X - cpp_right;
                } else {
                    pas::Extended cpp_right_2 = pas::real_divide(pas::random(11, &System::RandSeed), 16.0L);
                    Particle->Velocity.X = Particle->Velocity.X + cpp_right_2;
                }
                if (Particle->Velocity.Y < 0.0L) {
                    pas::Extended cpp_right_3 = pas::real_divide(pas::random(11, &System::RandSeed), 16.0L);
                    Particle->Velocity.Y = Particle->Velocity.Y - cpp_right_3;
                } else {
                    pas::Extended cpp_right_4 = pas::real_divide(pas::random(11, &System::RandSeed), 16.0L);
                    Particle->Velocity.Y = Particle->Velocity.Y + cpp_right_4;
                }
            }
        }
    }

    void TPSEyesGI::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PEyesParticle Particle{};
        PEyesParticle Current{};
        if (FirstParticle == nullptr && RemainingTicks >= 20) {
            EmitBurst(ClassesImports::Point(-6, -6), 8);
            EmitBurst(ClassesImports::Point(6, 0), 8);
            EmitBurst(ClassesImports::Point(0, 6), 8);
            EmitBurst(ClassesImports::Point(-6, 1), 8);
            BeamTicks = 20;
        } else {
            Particle = FirstParticle;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                switch (Current->State) {
                    case 2: {
                        pas::store_unaligned<EC_Struct::TPointF>(&Current->Position, EC_Struct::MakePointF(static_cast<long double>(Current->Position.X) + Current->Velocity.X, static_cast<long double>(Current->Position.Y) + Current->Velocity.Y));
                        Current->Velocity.X = 0.95L * Current->Velocity.X;
                        Current->Velocity.Y = 0.95L * Current->Velocity.Y;
                        --Current->Countdown;
                        if (Current->Countdown == 0) {
                            Current->State = 3;
                            Current->Countdown = 50;
                        }
                        break;
                    }
                    case 3: {
                        pas::store_unaligned<EC_Struct::TPointF>(&Current->Position, EC_Struct::MakePointF(static_cast<long double>(Current->Position.X) + Current->Velocity.X, static_cast<long double>(Current->Position.Y) + Current->Velocity.Y));
                        Current->Velocity.X = 0.95L * Current->Velocity.X;
                        Current->Velocity.Y = 0.95L * Current->Velocity.Y;
                        if (Current->Alpha > 10) {
                            Current->Alpha -= 4;
                        }
                        --Current->Countdown;
                        break;
                    }
                }
            }
        }
        --RemainingTicks;
        --BeamTicks;
    }

    void TPSEyesGI::Draw(Types::TRect ClipRect) {
        std::int32_t DX{};
        std::int32_t DY{};
        std::int32_t Progress{};
        std::int32_t Distance{};
        std::int32_t Step{};
        Types::TPoint P{};
        TEyesLine Line{};
        TEyesLine Shadow{};
        std::int32_t X = TargetPoint.X - LocalPosition.X + AbsolutePosition.X;
        std::int32_t Y = TargetPoint.Y - LocalPosition.Y + AbsolutePosition.Y;
        PEyesParticle Particle = FirstParticle;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Particle != nullptr) {
                DX = System::Round(Particle->Position.X) + X;
                DY = System::Round(Particle->Position.Y) + Y;
                GR_DX::QueueDrawPoint(DX, DY, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                Particle = Particle->Next;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
            X = AbsolutePosition.X;
            Y = AbsolutePosition.Y;
            DX = TargetPoint.X - LocalPosition.X;
            DY = TargetPoint.Y - LocalPosition.Y;
            if (pas::abs(DX) > pas::abs(DY)) {
                Distance = pas::abs(DX);
            } else {
                Distance = pas::abs(DY);
            }
            if (Distance == 0) {
                Distance = 1;
            }
            Progress = 0;
            Step = Distance / 4;
            if (Step < 1) {
                Step = 1;
            }
            if (Step > SegmentLength) {
                Step = SegmentLength;
            }
            if (BeamTicks > 0) {
                P = ClassesImports::Point(X, Y);
                while (Distance - Progress > Step) {
                    Line.First = P;
                    {
                        std::int32_t cpp_right_2 = aMyFunction::RandomIntRange(-Dispersion, Dispersion);
                        std::int32_t cpp_arg = Y + pas::idiv((Progress + Step) * DY, Distance) + cpp_right_2;
                        std::int32_t cpp_right = aMyFunction::RandomIntRange(-Dispersion, Dispersion);
                        std::int32_t cpp_arg_2 = X + pas::idiv((Progress + Step) * DX, Distance) + cpp_right;
                        P = ClassesImports::Point(cpp_arg_2, cpp_arg);
                    }
                    Line.Last = P;
                    Line.Alpha = pas::idiv((255 - StartingAlpha) * (Progress + Step), Distance) + StartingAlpha;
                    Line.Color = SecondaryColor;
                    if (pas::abs(DX) > pas::abs(DY)) {
                        Shadow.First = ClassesImports::Point(Line.First.X, Line.First.Y - 1);
                        Shadow.Last = ClassesImports::Point(Line.Last.X, Line.Last.Y - 1);
                    } else {
                        Shadow.First = ClassesImports::Point(Line.First.X - 1, Line.First.Y);
                        Shadow.Last = ClassesImports::Point(Line.Last.X - 1, Line.Last.Y);
                    }
                    Shadow.Alpha = Line.Alpha;
                    Shadow.Color = PrimaryColor;
                    Progress += Step;
                    GR_DX::DrawAntialiasedLineDX(Line.First.X, Line.First.Y, Line.Last.X, Line.Last.Y, GR_DX::Color565ToArgb(Line.Color), Line.Alpha, &ClipRect);
                    GR_DX::DrawAntialiasedLineDX(Shadow.First.X, Shadow.First.Y, Shadow.Last.X, Shadow.Last.Y, GR_DX::Color565ToArgb(Shadow.Color), Line.Alpha, &ClipRect);
                }
                Line.First = P;
                Line.Last = ClassesImports::Point(X + DX, Y + DY);
                Line.Alpha = 255;
                Line.Color = PrimaryColor;
                if (pas::abs(DX) > pas::abs(DY)) {
                    Shadow.First = ClassesImports::Point(Line.First.X, Line.First.Y - 1);
                    Shadow.Last = ClassesImports::Point(Line.Last.X, Line.Last.Y - 1);
                } else {
                    Shadow.First = ClassesImports::Point(Line.First.X - 1, Line.First.Y);
                    Shadow.Last = ClassesImports::Point(Line.Last.X - 1, Line.Last.Y);
                }
                Shadow.Alpha = Line.Alpha;
                Shadow.Color = PrimaryColor;
                GR_DX::DrawAntialiasedLineDX(Line.First.X, Line.First.Y, Line.Last.X, Line.Last.Y, GR_DX::Color565ToArgb(Line.Color), Line.Alpha, &ClipRect);
                GR_DX::DrawAntialiasedLineDX(Shadow.First.X, Shadow.First.Y, Shadow.Last.X, Shadow.Last.Y, GR_DX::Color565ToArgb(Shadow.Color), Line.Alpha, &ClipRect);
            }
        } else {
            while (Particle != nullptr) {
                DX = System::Round(Particle->Position.X) + X;
                DY = System::Round(Particle->Position.Y) + Y;
                if (DX >= ClipRect.Left && DX < ClipRect.Right && DY >= ClipRect.Top && DY < ClipRect.Bottom) {
                    GR_Main::ScreenRenderBuffer->BlendPixel16(DX, DY, Particle->Color, Particle->Alpha);
                }
                Particle = Particle->Next;
            }
            X = AbsolutePosition.X;
            Y = AbsolutePosition.Y;
            DX = TargetPoint.X - LocalPosition.X;
            DY = TargetPoint.Y - LocalPosition.Y;
            if (pas::abs(DX) > pas::abs(DY)) {
                Distance = pas::abs(DX);
            } else {
                Distance = pas::abs(DY);
            }
            if (Distance == 0) {
                Distance = 1;
            }
            Progress = 0;
            Step = Distance / 4;
            if (Step < 1) {
                Step = 1;
            }
            if (Step > SegmentLength) {
                Step = SegmentLength;
            }
            if (BeamTicks > 0) {
                P = ClassesImports::Point(X, Y);
                while (Distance - Progress > Step) {
                    Line.First = P;
                    {
                        std::int32_t cpp_right_4 = aMyFunction::RandomIntRange(-Dispersion, Dispersion);
                        std::int32_t cpp_arg_3 = Y + pas::idiv((Progress + Step) * DY, Distance) + cpp_right_4;
                        std::int32_t cpp_right_3 = aMyFunction::RandomIntRange(-Dispersion, Dispersion);
                        std::int32_t cpp_arg_4 = X + pas::idiv((Progress + Step) * DX, Distance) + cpp_right_3;
                        P = ClassesImports::Point(cpp_arg_4, cpp_arg_3);
                    }
                    Line.Last = P;
                    Line.Alpha = pas::idiv((255 - StartingAlpha) * (Progress + Step), Distance) + StartingAlpha;
                    Line.Color = SecondaryColor;
                    if (pas::abs(DX) > pas::abs(DY)) {
                        Shadow.First = ClassesImports::Point(Line.First.X, Line.First.Y - 1);
                        Shadow.Last = ClassesImports::Point(Line.Last.X, Line.Last.Y - 1);
                    } else {
                        Shadow.First = ClassesImports::Point(Line.First.X - 1, Line.First.Y);
                        Shadow.Last = ClassesImports::Point(Line.Last.X - 1, Line.Last.Y);
                    }
                    Shadow.Alpha = Line.Alpha;
                    Shadow.Color = PrimaryColor;
                    Progress += Step;
                    GR_Main::ScreenRenderBuffer->DrawAlphaLine16(Line.First.X, Line.First.Y, Line.Last.X, Line.Last.Y, Line.Color, Line.Alpha, ClipRect);
                    GR_Main::ScreenRenderBuffer->DrawAlphaLine16(Shadow.First.X, Shadow.First.Y, Shadow.Last.X, Shadow.Last.Y, Shadow.Color, Line.Alpha, ClipRect);
                }
                Line.First = P;
                Line.Last = ClassesImports::Point(X + DX, Y + DY);
                Line.Alpha = 255;
                Line.Color = PrimaryColor;
                if (pas::abs(DX) > pas::abs(DY)) {
                    Shadow.First = ClassesImports::Point(Line.First.X, Line.First.Y - 1);
                    Shadow.Last = ClassesImports::Point(Line.Last.X, Line.Last.Y - 1);
                } else {
                    Shadow.First = ClassesImports::Point(Line.First.X - 1, Line.First.Y);
                    Shadow.Last = ClassesImports::Point(Line.Last.X - 1, Line.Last.Y);
                }
                Shadow.Alpha = Line.Alpha;
                Shadow.Color = PrimaryColor;
                GR_Main::ScreenRenderBuffer->DrawAlphaLine16(Line.First.X, Line.First.Y, Line.Last.X, Line.Last.Y, Line.Color, Line.Alpha, ClipRect);
                GR_Main::ScreenRenderBuffer->DrawAlphaLine16(Shadow.First.X, Shadow.First.Y, Shadow.Last.X, Shadow.Last.Y, Shadow.Color, Line.Alpha, ClipRect);
            }
        }
    }

    void TPSEyesGI::p_destroy() {
        GI_PSEyes::TPSEyesGI_Destroy(this);
    }

} // namespace GI_PSEyes
