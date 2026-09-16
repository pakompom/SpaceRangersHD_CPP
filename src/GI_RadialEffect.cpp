#include "layout/GI_RadialEffect.hpp"
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
#include "units/GI_RadialEffect.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/MathImports.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_RadialEffect {
    std::uint32_t RadiationRingColor(std::int32_t Value, TPSRadEffectGI* Self);

    std::int32_t RadiationRingAlpha(float Distance, TPSRadEffectGI* Self);

    void QueueRadiationOctants(std::int32_t X, std::int32_t Y, TPSRadEffectGI* Self, std::int32_t& DistanceSquared, std::int32_t& FadeRadius, std::int32_t& CenterX, std::int32_t& CenterY);

    void BlendRadiationOctants(std::int32_t X, std::int32_t Y, TPSRadEffectGI* Self, Types::TRect& ClipRect, std::int32_t& DistanceSquared, std::int32_t& FadeRadius, std::int32_t& CenterX, std::int32_t& CenterY, std::int32_t& Edge);

    void BlendRadiationPoint(std::int32_t X, std::int32_t Y, Types::TRect& ClipRect, std::uint32_t& PixelColor, std::int32_t& PixelAlpha);

    pas::DynArray<pas::WideString> AuraAnimationPaths{};

    pas::DynArray<std::int32_t> RadiationRadii{};

    pas::DynArray<GI_RadialEffect::TRadiationPalette> RadiationPalettes{};

    pas::Array<Types::TPoint, 0, 23> RadiationEdgeStarts{};

    pas::Array<Types::TPoint, 0, 23> RadiationEdgeEnds{};

    pas::Array<EC_Struct::TPointF, 0, 5> RadiationHexagon = pas::Array<EC_Struct::TPointF, 0, 5>{{{.X = -0.8660254f, .Y = 0.5f}, {.X = 0.0f, .Y = 1.0f}, {.X = 0.8660254f, .Y = 0.5f}, {.X = 0.8660254f, .Y = -0.5f}, {.X = 0.0f, .Y = -1.0f}, {.X = -0.8660254f, .Y = -0.5f}}};

    void LoadRadiationPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        std::int32_t PartIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.RadialEffect.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        RadiationPalettes.set_length(Count);
        RadiationRadii.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 1); cpp_range_3.next(ColorIndex); ) {
                    Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                    for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 2); cpp_range_4.next(PartIndex); ) {
                        RadiationPalettes[Index][3 * ColorIndex + PartIndex] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, PartIndex, u","_wref.get()));
                    }
                }
                if (PaletteBlock->CountParams(u"Radius"_wref.get()) > 0) {
                    RadiationRadii[Index] = EC_Str::ExtractDigitsToIntW(PaletteBlock->GetParam(u"Radius"_wref.get()));
                } else {
                    RadiationRadii[Index] = 500;
                }
            }
        }
        for (auto cpp_range_5 = pas::for_to<std::int32_t>(0, 5); cpp_range_5.next(Index); ) {
            for (auto cpp_range_6 = pas::for_to<std::int32_t>(0, 3); cpp_range_6.next(ColorIndex); ) {
                RadiationEdgeStarts[Index * 4 + ColorIndex].X = System::Round(pas::real_divide((static_cast<long double>(4 - ColorIndex) * RadiationHexagon[Index].X + static_cast<long double>(ColorIndex) * RadiationHexagon[(Index + 1) % 6].X) * 3.0E+1L, 4.0L));
                RadiationEdgeStarts[Index * 4 + ColorIndex].Y = System::Round(pas::real_divide((static_cast<long double>(4 - ColorIndex) * RadiationHexagon[Index].Y + static_cast<long double>(ColorIndex) * RadiationHexagon[(Index + 1) % 6].Y) * 3.0E+1L, 4.0L));
            }
        }
        for (auto cpp_range_7 = pas::for_to<std::int32_t>(0, 23); cpp_range_7.next(Index); ) {
            RadiationEdgeEnds[Index].X = RadiationEdgeStarts[(Index + 1) % 24].X;
            RadiationEdgeEnds[Index].Y = RadiationEdgeStarts[(Index + 1) % 24].Y;
        }
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.AuraEffect.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        Count = 0;
        for (auto cpp_range_8 = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range_8.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        AuraAnimationPaths.set_length(Count);
        for (auto cpp_range_9 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_9.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                if (PaletteBlock->CountParams(u"GAI"_wref.get()) > 0) {
                    AuraAnimationPaths[Index] = PaletteBlock->GetParam(u"GAI"_wref.get());
                }
            }
        }
    }

    void TPSRadEffectGI_Create(TPSRadEffectGI* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 340;
        Self->UpdateProjectionBounds();
        Self->PositionInitialized = false;
        Self->PaletteIndex = APaletteIndex;
        Self->SetColor(GR_Main::CurrentPixelFormat->PackNormalizedRgb(RadiationPalettes[APaletteIndex][0], RadiationPalettes[APaletteIndex][1], RadiationPalettes[APaletteIndex][2]));
    }

    void TPSRadEffectGI_Destroy(TPSRadEffectGI* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSRadEffectGI::SetColor(std::uint16_t Value) {
        Color = Value;
    }

    void TPSRadEffectGI::SetPosition(Types::TPoint Position) {
        float X1{};
        float Y1{};
        float X2{};
        float Y2{};
        float Distance1{};
        float Distance2{};
        if (!PositionInitialized) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
            PositionInitialized = true;
            X1 = System::Round(pas::real_divide(LocalPosition.X, 51.96152424L)) * 2 * 0.866025404L * 3.0E+1L;
            Y1 = System::Round(pas::real_divide(LocalPosition.Y, 9.0E+1L)) * 3 * 30;
            X2 = (System::Round(pas::real_divide(LocalPosition.X, 51.96152424L) + 0.5L) - 0.5L) * 2.0L * 0.866025404L * 3.0E+1L;
            Y2 = (System::Round(pas::real_divide(LocalPosition.Y, 9.0E+1L) + 0.5L) - 0.5L) * 3.0L * 3.0E+1L;
            Distance1 = pas::sqr(static_cast<long double>(X1) - LocalPosition.X) + pas::sqr(static_cast<long double>(Y1) - LocalPosition.Y);
            Distance2 = pas::sqr(static_cast<long double>(X2) - LocalPosition.X) + pas::sqr(static_cast<long double>(Y2) - LocalPosition.Y);
            if (Distance1 < Distance2) {
                LatticeOffset.X = static_cast<long double>(X1) - LocalPosition.X;
                LatticeOffset.Y = static_cast<long double>(Y1) - LocalPosition.Y;
            } else {
                LatticeOffset.X = static_cast<long double>(X2) - LocalPosition.X;
                LatticeOffset.Y = static_cast<long double>(Y2) - LocalPosition.Y;
            }
        }
        UpdateProjectionBounds();
    }

    void TPSRadEffectGI::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
            UpdateProjectionBounds();
        }
    }

    void TPSRadEffectGI::UpdateProjectionBounds() {
        ProjectionBounds.Left = -RadiationRadii[PaletteIndex];
        ProjectionBounds.Right = RadiationRadii[PaletteIndex];
        ProjectionBounds.Top = -RadiationRadii[PaletteIndex];
        ProjectionBounds.Bottom = RadiationRadii[PaletteIndex];
    }

    void TPSRadEffectGI::UpdateHitTestBounds() {
        HitTestBounds.Left = ProjectionBounds.Left + AbsolutePosition.X;
        HitTestBounds.Top = ProjectionBounds.Top + AbsolutePosition.Y;
        HitTestBounds.Right = ProjectionBounds.Right + AbsolutePosition.X;
        HitTestBounds.Bottom = ProjectionBounds.Bottom + AbsolutePosition.Y;
    }

    Types::TRect TPSRadEffectGI::GetLocalBounds() {
        Types::TRect Result{};
        Result.Left = ProjectionBounds.Left + LocalPosition.X;
        Result.Top = ProjectionBounds.Top + LocalPosition.Y;
        Result.Right = ProjectionBounds.Right + LocalPosition.X;
        Result.Bottom = ProjectionBounds.Bottom + LocalPosition.Y;
        return Result;
    }

    PRadiationParticle TPSRadEffectGI::AddParticle() {
        PRadiationParticle Particle = static_cast<PRadiationParticle>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TRadiationParticle))));
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

    void TPSRadEffectGI::ClearParticles() {
        PRadiationParticle Current{};
        PRadiationParticle Particle = FirstParticle;
        while (Particle != nullptr) {
            Current = Particle;
            Particle = Particle->Next;
            EC_Mem::FreeEC(Current);
        }
        FirstParticle = nullptr;
        LastParticle = nullptr;
    }

    void TPSRadEffectGI::InvalidateRect(Types::TRect Rect) {
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

    void TPSRadEffectGI::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        std::int32_t Column{};
        std::int32_t Row{};
        std::int32_t Edge{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t Columns{};
        std::int32_t Rows{};
        std::int32_t RingOffset{};
        std::int32_t FadeAlpha{};
        PRadiationParticle Particle{};
        PRadiationParticle Current{};
        Invalidate();
        if (FirstParticle == nullptr && RemainingTicks >= 24) {
            Radius = 0;
            Columns = MathImports::Ceil(pas::real_divide(RadiationRadii[PaletteIndex], 25.98076212L)) + 1;
            Rows = MathImports::Ceil(pas::real_divide(RadiationRadii[PaletteIndex], 9.0E+1L)) + 1;
            for (auto cpp_range = pas::for_to<std::int32_t>(-Columns, Columns); cpp_range.next(Column); ) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(-2 * Rows, 2 * Rows); cpp_range_2.next(Row); ) {
                    Particle = AddParticle();
                    Particle->Color = Color;
                    Particle->Position.X = System::Round((Row % 2 * 0.5L + Column) * 3.0E+1L * 2.0L * 0.866025404L + LatticeOffset.X);
                    Particle->Position.Y = System::Round(30 * Row * 1.5L + LatticeOffset.Y);
                    for (Edge = 0; Edge <= 11; ++Edge) {
                        pas::store_unaligned<std::uint8_t>(pas::byte_offset(&Particle->Alpha, Edge * sizeof(std::uint8_t)), static_cast<std::uint8_t>(0));
                        X = Particle->Position.X + (RadiationEdgeStarts[Edge].X + RadiationEdgeEnds[Edge].X) / 2;
                        Y = Particle->Position.Y + (RadiationEdgeStarts[Edge].Y + RadiationEdgeEnds[Edge].Y) / 2;
                        pas::store_unaligned<std::int32_t>(pas::byte_offset(&Particle->EdgeDistance, Edge * sizeof(std::int32_t)), static_cast<std::int32_t>(System::Round(System::Sqrt(X * X + Y * Y))));
                    }
                }
            }
        } else {
            Radius += std::max<std::int64_t>(static_cast<std::int64_t>(1), System::Round(pas::real_divide(RadiationRadii[PaletteIndex] * 4, 5.0E+2L)));
            Alpha = System::Round(255.0L - pas::sqr(pas::real_divide(Radius, RadiationRadii[PaletteIndex])) * 255.0L);
            RingOffset = Radius;
            FadeAlpha = Alpha;
            RingOffset = System::Round(RingOffset - 3.0E+1L - 2.0L);
            FadeAlpha = System::Round(FadeAlpha * 0.7L);
            Particle = FirstParticle;
            while (Particle != nullptr) {
                Current = Particle;
                Particle = Particle->Next;
                for (Edge = 0; Edge <= 11; ++Edge) {
                    if (pas::load_unaligned<std::int32_t>(pas::byte_offset(&Current->EdgeDistance, Edge * sizeof(std::int32_t))) > RingOffset) {
                        pas::store_unaligned<std::uint8_t>(pas::byte_offset(&Current->Alpha, Edge * sizeof(std::uint8_t)), static_cast<std::uint8_t>(std::max<std::int32_t>(0, FadeAlpha - pas::sqr((pas::load_unaligned<std::int32_t>(pas::byte_offset(&Current->EdgeDistance, Edge * sizeof(std::int32_t))) - RingOffset) / 2))));
                    } else {
                        pas::store_unaligned<std::uint8_t>(pas::byte_offset(&Current->Alpha, Edge * sizeof(std::uint8_t)), static_cast<std::uint8_t>(System::Round(pas::real_max<pas::Extended>(0.0L, FadeAlpha - pas::abs(pas::load_unaligned<std::int32_t>(pas::byte_offset(&Current->EdgeDistance, Edge * sizeof(std::int32_t))) - RingOffset) * pas::real_max<pas::Extended>(0.1L, 1.0L - pas::real_divide(std::max<std::int32_t>(RingOffset, 0), RadiationRadii[PaletteIndex]))))));
                    }
                }
            }
            UpdateHitTestBounds();
        }
        --RemainingTicks;
        if (RadiationRadii[PaletteIndex] < Radius) {
            RemainingTicks = 0;
        }
    }

    void TPSRadEffectGI::Draw(Types::TRect ClipRect) {
        std::int32_t DistanceSquared{};
        std::int32_t Edge{};
        PRadiationParticle Particle{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t OuterRadius = Radius;
        std::int32_t InnerRadius = std::max<std::int32_t>(OuterRadius - 100, 0);
        std::int32_t FadeRadius = std::max<std::int32_t>(OuterRadius - 10, 0);
        std::int32_t OuterSquared = pas::sqr(OuterRadius);
        std::int32_t InnerSquared = pas::sqr(InnerRadius);
        std::int32_t CenterX = AbsolutePosition.X;
        std::int32_t CenterY = AbsolutePosition.Y;
        if (GlobalsV::HardwareRenderingEnabled) {
            Particle = FirstParticle;
            while (Particle != nullptr) {
                for (auto cpp_range = pas::for_to<std::int32_t>(0, 11); cpp_range.next(Edge); ) {
                    if (*static_cast<std::uint8_t*>(pas::byte_offset(&Particle->Alpha, Edge * sizeof(std::uint8_t))) > 0) {
                        GR_DX::DrawAlphaLine(AbsolutePosition.X + Particle->Position.X + RadiationEdgeStarts[Edge].X, AbsolutePosition.Y + Particle->Position.Y + RadiationEdgeStarts[Edge].Y, AbsolutePosition.X + Particle->Position.X + RadiationEdgeEnds[Edge].X, AbsolutePosition.Y + Particle->Position.Y + RadiationEdgeEnds[Edge].Y, GR_DX::Color565ToArgb(Particle->Color), *static_cast<std::uint8_t*>(pas::byte_offset(&Particle->Alpha, Edge * sizeof(std::uint8_t))), &ClipRect);
                    }
                }
                Particle = Particle->Next;
            }
            Y = 0;
            X = 0;
            while (true) {
                DistanceSquared = pas::sqr(X) + pas::sqr(Y);
                if (DistanceSquared < InnerSquared) {
                    ++X;
                    continue;
                }
                if (DistanceSquared > OuterSquared) {
                    ++Y;
                    X = Y;
                    DistanceSquared = pas::sqr(X) + pas::sqr(Y);
                    if (DistanceSquared > OuterSquared) {
                        break;
                    }
                    continue;
                }
                GI_RadialEffect::QueueRadiationOctants(X, Y, this, DistanceSquared, FadeRadius, CenterX, CenterY);
                ++X;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            Particle = FirstParticle;
            while (Particle != nullptr) {
                for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, 11); cpp_range_2.next(Edge); ) {
                    if (*static_cast<std::uint8_t*>(pas::byte_offset(&Particle->Alpha, Edge * sizeof(std::uint8_t))) > 0) {
                        if (*static_cast<std::uint8_t*>(pas::byte_offset(&Particle->Alpha, Edge * sizeof(std::uint8_t))) > 0) {
                            GR_Main::ScreenRenderBuffer->DrawAntialiasedLine16(AbsolutePosition.X + Particle->Position.X + RadiationEdgeStarts[Edge].X, AbsolutePosition.Y + Particle->Position.Y + RadiationEdgeStarts[Edge].Y, AbsolutePosition.X + Particle->Position.X + RadiationEdgeEnds[Edge].X, AbsolutePosition.Y + Particle->Position.Y + RadiationEdgeEnds[Edge].Y, Particle->Color, *static_cast<std::uint8_t*>(pas::byte_offset(&Particle->Alpha, Edge * sizeof(std::uint8_t))), ClipRect);
                        }
                    }
                }
                Particle = Particle->Next;
            }
            Y = 0;
            X = 0;
            while (true) {
                DistanceSquared = pas::sqr(X) + pas::sqr(Y);
                if (DistanceSquared < InnerSquared) {
                    ++X;
                    continue;
                }
                if (DistanceSquared > OuterSquared) {
                    ++Y;
                    X = Y;
                    DistanceSquared = pas::sqr(X) + pas::sqr(Y);
                    if (DistanceSquared > OuterSquared) {
                        break;
                    }
                    continue;
                }
                GI_RadialEffect::BlendRadiationOctants(X, Y, this, ClipRect, DistanceSquared, FadeRadius, CenterX, CenterY, Edge);
                ++X;
            }
        }
    }

    std::uint32_t RadiationRingColor(std::int32_t Value, TPSRadEffectGI* Self) {
        return GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(RadiationPalettes[Self->PaletteIndex]), pas::sqr(pas::real_divide(Value, 255.0L)));
    }

    std::int32_t RadiationRingAlpha(float Distance, TPSRadEffectGI* Self) {
        if (Distance > 0.0L) {
            return System::Round(pas::real_max<pas::Extended>(0.0L, Self->Alpha - pas::sqr(static_cast<pas::Extended>(Distance)) * 1.0E+1L));
        } else if (Distance > -1.0E+1L) {
            return System::Round(pas::real_max<pas::Extended>(0.0L, 1.0E+1L * Distance + Self->Alpha));
        } else {
            return System::Round(pas::real_max<pas::Extended>(0.0L, (Distance + 1.0E+1L) * 3.0L + (Self->Alpha - 100)));
        }
    }

    void QueueRadiationOctants(std::int32_t X, std::int32_t Y, TPSRadEffectGI* Self, std::int32_t& DistanceSquared, std::int32_t& FadeRadius, std::int32_t& CenterX, std::int32_t& CenterY) {
        std::uint32_t PixelColor{};
        float Distance = System::Sqrt(DistanceSquared);
        std::int32_t PixelAlpha = GI_RadialEffect::RadiationRingAlpha(static_cast<long double>(Distance) - FadeRadius, Self);
        if (PixelAlpha > 0) {
            PixelColor = GR_DX::Color565ToArgb(GI_RadialEffect::RadiationRingColor(PixelAlpha, Self));
            GR_DX::QueueDrawPoint(X + CenterX, Y + CenterY, PixelColor, PixelAlpha);
            GR_DX::QueueDrawPoint(-X + CenterX, -Y + CenterY, PixelColor, PixelAlpha);
            if (X > 0 && Y > 0) {
                GR_DX::QueueDrawPoint(-X + CenterX, Y + CenterY, PixelColor, PixelAlpha);
                GR_DX::QueueDrawPoint(X + CenterX, -Y + CenterY, PixelColor, PixelAlpha);
            }
            if (X != Y) {
                GR_DX::QueueDrawPoint(Y + CenterX, X + CenterY, PixelColor, PixelAlpha);
                GR_DX::QueueDrawPoint(-Y + CenterX, -X + CenterY, PixelColor, PixelAlpha);
                if (X > 0 && Y > 0) {
                    GR_DX::QueueDrawPoint(-Y + CenterX, X + CenterY, PixelColor, PixelAlpha);
                    GR_DX::QueueDrawPoint(Y + CenterX, -X + CenterY, PixelColor, PixelAlpha);
                }
            }
        }
    }

    void BlendRadiationOctants(std::int32_t X, std::int32_t Y, TPSRadEffectGI* Self, Types::TRect& ClipRect, std::int32_t& DistanceSquared, std::int32_t& FadeRadius, std::int32_t& CenterX, std::int32_t& CenterY, std::int32_t& Edge) {
        std::uint32_t PixelColor{};
        float Distance = System::Sqrt(DistanceSquared);
        std::int32_t PixelAlpha = GI_RadialEffect::RadiationRingAlpha(System::Round(Distance) - FadeRadius, Self);
        if (PixelAlpha > 0) {
            PixelColor = GI_RadialEffect::RadiationRingColor(PixelAlpha, Self);
            Edge = 0;
            GI_RadialEffect::BlendRadiationPoint(X + CenterX, Y + CenterY, ClipRect, PixelColor, PixelAlpha);
            GI_RadialEffect::BlendRadiationPoint(-X + CenterX, -Y + CenterY, ClipRect, PixelColor, PixelAlpha);
            if (X > 0 && Y > 0) {
                GI_RadialEffect::BlendRadiationPoint(-X + CenterX, Y + CenterY, ClipRect, PixelColor, PixelAlpha);
                GI_RadialEffect::BlendRadiationPoint(X + CenterX, -Y + CenterY, ClipRect, PixelColor, PixelAlpha);
            }
            if (X != Y) {
                GI_RadialEffect::BlendRadiationPoint(Y + CenterX, X + CenterY, ClipRect, PixelColor, PixelAlpha);
                GI_RadialEffect::BlendRadiationPoint(-Y + CenterX, -X + CenterY, ClipRect, PixelColor, PixelAlpha);
                if (X > 0 && Y > 0) {
                    GI_RadialEffect::BlendRadiationPoint(-Y + CenterX, X + CenterY, ClipRect, PixelColor, PixelAlpha);
                    GI_RadialEffect::BlendRadiationPoint(Y + CenterX, -X + CenterY, ClipRect, PixelColor, PixelAlpha);
                }
            }
        }
    }

    void BlendRadiationPoint(std::int32_t X, std::int32_t Y, Types::TRect& ClipRect, std::uint32_t& PixelColor, std::int32_t& PixelAlpha) {
        if (ClipRect.Left <= X && ClipRect.Right > X && ClipRect.Top <= Y && ClipRect.Bottom > Y) {
            GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, PixelColor, PixelAlpha);
        }
    }

    void TPSRadEffectGI::p_destroy() {
        GI_RadialEffect::TPSRadEffectGI_Destroy(this);
    }

} // namespace GI_RadialEffect
