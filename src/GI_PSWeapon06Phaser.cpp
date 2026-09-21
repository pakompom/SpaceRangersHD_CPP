#include "layout/GI_PSWeapon06Phaser.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GI_PSWeapon06Phaser.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aMyFunction.hpp"

// Native phaser class and routines:. Palette finalizer:.
namespace GI_PSWeapon06Phaser {
    pas::DynArray<GI_PSWeapon06Phaser::TPhaserPalette> PhaserPalettes{};

    // Loads SE.Weapon.5.Palettes; native visual numbering differs from the class name.
    void LoadPhaserPalettes() {
        EC_BlockPar::TBlockParEC* Block{};
        EC_BlockPar::TBlockParEC* PaletteBlock{};
        std::int32_t Index{};
        std::int32_t ColorIndex{};
        std::int32_t PartIndex{};
        pas::WideString Text{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Weapon.5.Palettes"_wref.get());
        ColorIndex = Block->GetBlockCount();
        std::int32_t Count = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ColorIndex - 1); cpp_range.next(Index); ) {
            Count = std::max<std::int32_t>(Count, EC_Str::ExtractDigitsToIntW(pas::view(Block->GetBlockNameByIndex(Index))) + 1);
        }
        PhaserPalettes.set_length(Count);
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_2.next(Index); ) {
            Text = pas::wide_int_to_str(Index);
            if (Block->CountBlocks(Text) != 0) {
                PaletteBlock = Block->GetBlockByPath(Text);
                for (auto cpp_range_3 = pas::for_to<std::int32_t>(0, 2); cpp_range_3.next(ColorIndex); ) {
                    if (PaletteBlock->CountParams(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))) > 0) {
                        Text = PaletteBlock->GetParam(pas::view(static_cast<pas::WideString>(pas::concat_ansi({"Color", SysUtils::IntToStr(ColorIndex)}))));
                        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, 2); cpp_range_4.next(PartIndex); ) {
                            PhaserPalettes[Index][3 * ColorIndex + PartIndex] = EC_Str::ExtractDecimalToSingleW(EC_Str::ExtractDelimitedPartW(pas::view(Text), PartIndex, u","sv));
                        }
                    }
                }
            }
        }
    }

    void TPSWeapon06Phaser_Create(TPSWeapon06Phaser* Self, GI_MessageLoop::TObjectGI* Owner, std::int32_t APaletteIndex) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 60;
        Self->LifetimeTicks = 60;
        Self->PaletteIndex = APaletteIndex;
    }

    void TPSWeapon06Phaser_Destroy(TPSWeapon06Phaser* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    // Native empty override.
    void TPSWeapon06Phaser::Invalidate() {
    }

    void TPSWeapon06Phaser::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
        }
    }

    void TPSWeapon06Phaser::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
        }
    }

    void TPSWeapon06Phaser::UpdateHitTestBounds() {
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        HitTestBounds.Right = GR_Main::GameScreenWidth;
        HitTestBounds.Bottom = GR_Main::GameScreenHeight;
    }

    void TPSWeapon06Phaser::ClearParticles() {
        if (Particles != nullptr) {
            EC_Mem::FreeEC(Particles);
            Particles = nullptr;
        }
        ParticleCount = 0;
        ParticleCapacity = 0;
    }

    void TPSWeapon06Phaser::GrowParticles() {
        std::int32_t Index{};
        PPhaserParticle Following{};
        ParticleCapacity += 100;
        Particles = static_cast<PPhaserParticle>(EC_Mem::ReAllocREC(Particles, ParticleCapacity * static_cast<std::int32_t>(sizeof(TPhaserParticle))));
        PPhaserParticle Particle = Particles;
        PPhaserParticle Previous = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, ParticleCount - 1); cpp_range.next(Index); ) {
            if (Index < ParticleCount - 1) {
                Following = static_cast<PPhaserParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TPhaserParticle))));
            } else {
                Following = nullptr;
            }
            Particle->Next = Following;
            Particle->Prev = Previous;
            Previous = Particle;
            Particle = Following;
        }
    }

    PPhaserParticle TPSWeapon06Phaser::AddParticle() {
        PPhaserParticle Previous{};
        if (ParticleCount >= ParticleCapacity) {
            GrowParticles();
        }
        PPhaserParticle Particle = static_cast<PPhaserParticle>(EC_Mem::AddPointerOffset(Particles, ParticleCount * static_cast<std::int32_t>(sizeof(TPhaserParticle))));
        if (ParticleCount == 0) {
            Previous = nullptr;
        } else {
            Previous = static_cast<PPhaserParticle>(EC_Mem::AddPointerOffset(Particles, (ParticleCount - 1) * static_cast<std::int32_t>(sizeof(TPhaserParticle))));
        }
        PPhaserParticle Result = Particle;
        ++ParticleCount;
        Particle->Next = nullptr;
        Particle->Prev = Previous;
        if (Previous != nullptr) {
            Previous->Next = Particle;
        }
        return Result;
    }

    void TPSWeapon06Phaser::AdvanceWave() {
        std::int32_t Index = 0;
        PPhaserParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        while (Count > 0) {
            if (Particle->Kind == 1) {
                if (Particle->Next != nullptr) {
                    Particle->Next->Incoming = Particle->Displacement;
                }
                --Particle->PhaseCountdown;
                if (Particle->PhaseCountdown == 0) {
                    Particle->PhaseCountdown = 16;
                    Particle->PhaseStep = aMyFunction::RandomFloatRange(pas::constant(static_cast<double>(SystemImports::Pi / 25.0L)), pas::constant(static_cast<double>(SystemImports::Pi / 2.0E+1L)));
                }
                Particle->Phase = static_cast<long double>(Particle->Phase) + Particle->PhaseStep;
                if (pas::constant(2.0L * SystemImports::Pi) <= Particle->Phase) {
                    Particle->Phase = Particle->Phase - pas::constant(2.0L * SystemImports::Pi);
                }
                {
                    pas::Extended cpp_left = System::Sin(Particle->Phase) * 2.0L;
                    Particle->Displacement = cpp_left + aMyFunction::RandomIntRange(-1, 1);
                }
            } else if (Particle->Kind == 2) {
                if (Particle->Next != nullptr) {
                    Particle->Next->Incoming = Particle->Displacement;
                }
                if (Particle->Prev != nullptr) {
                    Particle->Prev->Reflected = Particle->Reflected;
                }
                Particle->Displacement = Particle->Incoming;
            } else if (Particle->Kind == 4) {
                if (Particle->Next != nullptr) {
                    Particle->Next->Incoming = Particle->Displacement;
                }
                if (Particle->Prev != nullptr) {
                    Particle->Prev->Reflected = Particle->Reflected;
                }
                Particle->Displacement = -Particle->Incoming;
            } else if (Particle->Kind == 3) {
                if (Particle->Prev != nullptr) {
                    Particle->Prev->Reflected = Particle->Reflected;
                }
                Particle->Reflected = -Particle->Incoming;
                Particle->Displacement = Particle->Incoming;
            }
            ++Index;
            Particle = static_cast<PPhaserParticle>(EC_Mem::AddPointerOffset(Particles, Index * static_cast<std::int32_t>(sizeof(TPhaserParticle))));
            --Count;
        }
    }

    void TPSWeapon06Phaser::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PPhaserParticle Particle{};
        std::int32_t Index{};
        std::int32_t Step{};
        if (RemainingTicks == 60) {
            OriginalLength = System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y));
            ClearParticles();
            Particle = AddParticle();
            Particle->Kind = 1;
            pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(0.0f, 16.0f));
            Particle->Incoming = 0.0f;
            Particle->Displacement = 0.0f;
            Particle->Reflected = 0.0f;
            Particle->Color = GR_Main::CurrentPixelFormat->PackNormalizedRgb(1.0, 0.5, 0.33);
            Particle->Alpha = 0;
            Particle->Phase = 0.0f;
            Particle->PhaseCountdown = 1;
            Particle->PhaseStep = pas::constant(static_cast<float>(SystemImports::Pi / 8.0L));
            Index = 17;
            while (static_cast<long double>(Index) < OriginalLength) {
                Particle = AddParticle();
                Particle->Kind = 2;
                pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(0.0f, Index));
                Particle->Incoming = 0.0f;
                Particle->Displacement = aMyFunction::RandomIntRange(-1, 1);
                Particle->Reflected = 0.0f;
                Particle->Color = GI_PSWeapon::TPSWeaponGI::SampleGradientColor(pas::open_array(PhaserPalettes[PaletteIndex]), pas::real_divide(Index, OriginalLength) * 15.0L);
                if (Particle->Position.Y < 32.0L) {
                    Particle->Alpha = pas::shr(System::Trunc(Particle->Position.Y * 255.0L), 5);
                } else {
                    Particle->Alpha = 255;
                }
                ++Index;
            }
            Particle = AddParticle();
            Particle->Kind = 3;
            pas::store_unaligned<EC_Struct::TPointF>(&Particle->Position, EC_Struct::MakePointF(0.0f, OriginalLength));
            Particle->Incoming = 0.0f;
            Particle->Displacement = 0.0f;
            Particle->Reflected = 0.0f;
            Particle->Color = GR_Main::CurrentPixelFormat->PackNormalizedRgb(1.0, 0.5, 0.33);
            if (Particle->Position.Y < 32.0L) {
                Particle->Alpha = pas::shr(System::Trunc(Particle->Position.Y * 255.0L), 5);
            } else {
                Particle->Alpha = 255;
            }
            for (auto cpp_range = pas::for_to<std::int32_t>(1, static_cast<std::int32_t>(System::Trunc(0.3L * OriginalLength))); cpp_range.next(Step); ) {
                AdvanceWave();
            }
        }
        UpdateHitTestBounds();
        for (Step = 1; Step <= 6; ++Step) {
            AdvanceWave();
        }
        if (RemainingTicks > 0) {
            --RemainingTicks;
        }
    }

    void TPSWeapon06Phaser::Draw(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        float PX{};
        if (OriginalLength == 0.0L) {
            Advance(nullptr, 0);
        }
        float Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PPhaserParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                if (Particle->Kind >= 1 && Particle->Kind <= 3) {
                    Particle->Position.X = Particle->Displacement;
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = static_cast<PPhaserParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TPhaserParticle))));
                --Count;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Count > 0) {
                if (Particle->Kind >= 1 && Particle->Kind <= 3) {
                    Particle->Position.X = Particle->Displacement;
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = static_cast<PPhaserParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TPhaserParticle))));
                --Count;
            }
        }
    }

    void TPSWeapon06Phaser::p_destroy() {
        GI_PSWeapon06Phaser::TPSWeapon06Phaser_Destroy(this);
    }

} // namespace GI_PSWeapon06Phaser
