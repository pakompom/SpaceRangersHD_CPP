#include "layout/GI_PSWeapon10AVision.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon10AVision.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"

namespace GI_PSWeapon10AVision {
    pas::DynArray<GI_PSWeapon10AVision::TAVisionPalette> AVisionPalettes{};

    pas::DynArray<GI_PSWeapon10AVision::TGAISet> AVisionAnimationPaths{};

    void LoadAVisionPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.9.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(Block->GetBlockNameByIndex(Index)) + 1);
        }
        AVisionPalettes.set_length(Count);
        AVisionAnimationPaths.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 3); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)})));
                        AVisionPalettes[Index][ColorIndex] = ([&] {
                            double extractDecimalToSingleW = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 0, u","_wref.get()));
                            double extractDecimalToSingleW_2 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 1, u","_wref.get()));
                            double extractDecimalToSingleW_3 = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(Text, 2, u","_wref.get()));
                            return GR_Main::CurrentPixelFormat->PackNormalizedRgb(extractDecimalToSingleW, extractDecimalToSingleW_2, extractDecimalToSingleW_3);
                        }());
                    }
                }
                if (PaletteBlock->CountParams(u"GAI"_wref.get()) > 0) {
                    AVisionAnimationPaths[Index][0] = PaletteBlock->GetParam(u"GAI"_wref.get());
                }
            }
        }
    }

    void TPSWeapon10AVision_Create(TPSWeapon10AVision* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 40;
        Self->LifetimeTicks = 40;
        Self->Unknown130 = 0;
        Self->Colors[0] = AVisionPalettes[APaletteIndex][0];
        Self->Colors[1] = AVisionPalettes[APaletteIndex][1];
        Self->Colors[2] = AVisionPalettes[APaletteIndex][2];
        Self->Colors[3] = AVisionPalettes[APaletteIndex][3];
    }

    void TPSWeapon10AVision_Destroy(TPSWeapon10AVision* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSWeapon10AVision::Invalidate() {
    }

    void TPSWeapon10AVision::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
        }
    }

    void TPSWeapon10AVision::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
        }
    }

    void TPSWeapon10AVision::UpdateHitTestBounds() {
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        HitTestBounds.Right = GR_Main::GameScreenWidth;
        HitTestBounds.Bottom = GR_Main::GameScreenHeight;
    }

    void TPSWeapon10AVision::ClearParticles() {
        if (Particles != nullptr) {
            EC_Mem::FreeEC(Particles);
            Particles = nullptr;
        }
        ParticleCount = 0;
        ParticleCapacity = 0;
    }

    void TPSWeapon10AVision::GrowParticles() {
        ParticleCapacity += 100;
        Particles = static_cast<PAVisionParticle>(EC_Mem::ReAllocREC(Particles, ParticleCapacity * static_cast<std::int32_t>(sizeof(TAVisionParticle))));
    }

    PAVisionParticle TPSWeapon10AVision::AddParticle() {
        if (ParticleCount >= ParticleCapacity) {
            GrowParticles();
        }
        PAVisionParticle Result = static_cast<PAVisionParticle>(EC_Mem::AddPointerOffset(Particles, ParticleCount * static_cast<std::int32_t>(sizeof(TAVisionParticle))));
        ++ParticleCount;
        return Result;
    }

    void TPSWeapon10AVision::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PAVisionParticle Particle{};
        std::int32_t I{};
        float Y{};
        if (RemainingTicks == 40) {
            OriginalLength = System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y));
            for (I = 0; I <= 3; ++I) {
                Y = 0.0f;
                while (Y < OriginalLength) {
                    Particle = AddParticle();
                    if (pas::random(2, &System::RandSeed) == 0) {
                        Particle->Position.X = -I;
                    } else {
                        Particle->Position.X = I;
                    }
                    Particle->Position.Y = Y;
                    Particle->Kind = 1;
                    Particle->Alpha = 0;
                    Particle->InitialFadeInTicks = 11 - I;
                    Particle->FadeInTicks = Particle->InitialFadeInTicks;
                    Particle->Velocity.X = 0.0f;
                    Particle->Velocity.Y = 7.0L - I * 2;
                    Particle->FadeOutThreshold = System::Trunc(System::Sin(pas::real_divide(SystemImports::Pi * Y, 4.0E+1L)) * 8.0L + 2.0E+1L);
                    Particle->Color = Colors[I];
                    Y = Y + 1.5L + I;
                }
            }
        }
        Particle = Particles;
        std::int32_t Count = ParticleCount;
        I = 0;
        UpdateHitTestBounds();
        while (Count > 0) {
            if (Particle->Kind == 1) {
                Particle->Position.X = static_cast<long double>(Particle->Position.X) + Particle->Velocity.X;
                Particle->Position.Y = static_cast<long double>(Particle->Position.Y) + Particle->Velocity.Y;
                if (Particle->Position.Y > OriginalLength) {
                    Particle->Position.Y = static_cast<long double>(Particle->Position.Y) - OriginalLength;
                    Particle->Alpha = 0;
                    Particle->FadeInTicks = Particle->InitialFadeInTicks;
                } else {
                    Particle->Alpha += 20;
                    --Particle->FadeInTicks;
                    if (Particle->FadeInTicks == 0) {
                        Particle->FadeInTicks = 100;
                        Particle->Kind = 2;
                    }
                }
                if (RemainingTicks < Particle->FadeOutThreshold) {
                    Particle->Kind = 3;
                }
            } else if (Particle->Kind == 2) {
                Particle->Position.X = static_cast<long double>(Particle->Position.X) + Particle->Velocity.X;
                Particle->Position.Y = static_cast<long double>(Particle->Position.Y) + Particle->Velocity.Y;
                if (OriginalLength - 32.0L < Particle->Position.Y) {
                    if (Particle->Alpha < 245) {
                        Particle->Alpha += 10;
                    } else {
                        Particle->Alpha = 255;
                    }
                }
                if (Particle->Position.Y > OriginalLength) {
                    Particle->Position.Y = static_cast<long double>(Particle->Position.Y) - OriginalLength;
                    Particle->Alpha = 0;
                    Particle->FadeInTicks = Particle->InitialFadeInTicks;
                    Particle->Kind = 1;
                }
                if (RemainingTicks < Particle->FadeOutThreshold) {
                    Particle->Kind = 3;
                }
            } else if (Particle->Kind == 3) {
                Particle->Position.X = static_cast<long double>(Particle->Position.X) + Particle->Velocity.X;
                Particle->Position.Y = static_cast<long double>(Particle->Position.Y) + Particle->Velocity.Y;
                if (Particle->Position.Y > OriginalLength) {
                    Particle->Position.Y = static_cast<long double>(Particle->Position.Y) - OriginalLength;
                    Particle->Alpha = 0;
                }
                if (Particle->Alpha > 12) {
                    Particle->Alpha -= 12;
                } else {
                    Particle->Alpha = 0;
                }
            }
            ++I;
            Particle = static_cast<PAVisionParticle>(EC_Mem::AddPointerOffset(Particles, I * static_cast<std::int32_t>(sizeof(TAVisionParticle))));
            --Count;
        }
        --RemainingTicks;
    }

    void TPSWeapon10AVision::Draw(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        float PX{};
        if (OriginalLength == 0.0L) {
            OriginalLength = 1.0f;
        }
        float Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PAVisionParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                if (Particle->Kind >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Round(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Round(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = static_cast<PAVisionParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TAVisionParticle))));
                --Count;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Count > 0) {
                if (Particle->Kind >= 1) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Round(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Round(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = static_cast<PAVisionParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TAVisionParticle))));
                --Count;
            }
        }
    }

    void TPSWeapon10AVision::p_destroy() {
        GI_PSWeapon10AVision::TPSWeapon10AVision_Destroy(this);
    }

} // namespace GI_PSWeapon10AVision
