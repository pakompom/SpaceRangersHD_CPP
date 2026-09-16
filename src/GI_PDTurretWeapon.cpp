#include "layout/GI_PDTurretWeapon.hpp"
#include "types/EC_Struct.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_PDTurretWeapon.hpp"
#include "units/GI_PSWeapon.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/Math.hpp"
#include "units/System.hpp"

namespace GI_PDTurretWeapon {
    void TPSPDWeaponGI_Create(TPSPDWeaponGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_PSWeapon::TPSWeaponGI_Create(Self, Owner);
        Self->RemainingTicks = 25;
        Self->LifetimeTicks = 25;
        Self->ParticleColor = GR_Main::CurrentPixelFormat->PackNormalizedRgb(1.0, 0.6, 0.6);
    }

    void TPSPDWeaponGI_Destroy(TPSPDWeaponGI* Self) {
        Self->ClearParticles();
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TPSPDWeaponGI::Invalidate() {
    }

    void TPSPDWeaponGI::SetPosition(Types::TPoint Position) {
        if (LocalPosition.X != Position.X || LocalPosition.Y != Position.Y) {
            GI_MessageLoop::TObjectGI::SetPosition(Position);
        }
    }

    void TPSPDWeaponGI::SetTargetPoint(Types::TPoint Point) {
        if (TargetPoint.X != Point.X || TargetPoint.Y != Point.Y) {
            TargetPoint = Point;
        }
    }

    void TPSPDWeaponGI::UpdateHitTestBounds() {
        HitTestBounds.Left = 0;
        HitTestBounds.Top = 0;
        HitTestBounds.Right = GR_Main::GameScreenWidth;
        HitTestBounds.Bottom = GR_Main::GameScreenHeight;
    }

    void TPSPDWeaponGI::SetActive(std::uint8_t Enabled) {
        GI_MessageLoop::TObjectGI::SetActive(Enabled);
    }

    void TPSPDWeaponGI::ClearParticles() {
        if (Particles != nullptr) {
            EC_Mem::FreeEC(Particles);
            Particles = nullptr;
        }
        ParticleCount = 0;
        ParticleCapacity = 0;
    }

    void TPSPDWeaponGI::GrowParticles() {
        ParticleCapacity += 100;
        Particles = static_cast<PPDWeaponParticle>(EC_Mem::ReAllocREC(Particles, ParticleCapacity * static_cast<std::int32_t>(sizeof(TPDWeaponParticle))));
    }

    PPDWeaponParticle TPSPDWeaponGI::AddParticle() {
        if (ParticleCount >= ParticleCapacity) {
            GrowParticles();
        }
        PPDWeaponParticle Result = static_cast<PPDWeaponParticle>(EC_Mem::AddPointerOffset(Particles, ParticleCount * static_cast<std::int32_t>(sizeof(TPDWeaponParticle))));
        ++ParticleCount;
        return Result;
    }

    void TPSPDWeaponGI::Advance(GI_MessageLoop::PCallbackTimerGI Timer, std::int32_t UserData) {
        PPDWeaponParticle Particle{};
        std::int32_t I{};
        std::int32_t J{};
        std::int32_t K{};
        float Speed{};
        if (RemainingTicks == 25) {
            OriginalLength = System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y));
            Speed = pas::real_divide(OriginalLength + 288.0L, 25.0L);
            for (I = 0; I <= 7; ++I) {
                for (J = 0; J <= 1; ++J) {
                    for (K = 0; K <= 5; ++K) {
                        Particle = AddParticle();
                        Particle->Kind = 2;
                        Particle->Position.X = J * 6 - 3;
                        Particle->Position.Y = I * 36 - 288 + K;
                        Particle->Color = ParticleColor;
                        Particle->Alpha = 0;
                        Particle->Velocity.X = 0.0f;
                        Particle->Velocity.Y = Speed;
                        Particle->Unknown1A = 0;
                        Particle = AddParticle();
                        Particle->Kind = 2;
                        Particle->Position.X = J * 8 - 4;
                        Particle->Position.Y = I * 36 - 288 + K;
                        Particle->Color = ParticleColor;
                        Particle->Alpha = 0;
                        Particle->Velocity.X = 0.0f;
                        Particle->Velocity.Y = Speed;
                        Particle->Unknown1A = 0;
                    }
                }
            }
        }
        K = 0;
        Particle = Particles;
        I = ParticleCount;
        UpdateHitTestBounds();
        while (I > 0) {
            if (Particle->Kind == 2) {
                Particle->Position.X = static_cast<long double>(Particle->Position.X) + Particle->Velocity.X;
                Particle->Position.Y = static_cast<long double>(Particle->Position.Y) + Particle->Velocity.Y;
                if (Particle->Position.Y >= OriginalLength) {
                    Particle->Kind = 0;
                }
                if (Particle->Position.Y > 0.0L) {
                    if (Particle->Alpha < 219) {
                        Particle->Alpha += 36;
                    } else {
                        Particle->Alpha = 255;
                    }
                }
            }
            ++K;
            Particle = static_cast<PPDWeaponParticle>(EC_Mem::AddPointerOffset(Particles, K * static_cast<std::int32_t>(sizeof(TPDWeaponParticle))));
            --I;
        }
        if (RemainingTicks > 0) {
            --RemainingTicks;
        }
    }

    void TPSPDWeaponGI::Draw(Types::TRect ClipRect) {
        std::int32_t X{};
        std::int32_t Y{};
        float PX{};
        float Scale = pas::real_divide(System::Sqrt(pas::sqr(LocalPosition.X - TargetPoint.X) + pas::sqr(LocalPosition.Y - TargetPoint.Y)), OriginalLength);
        float PY = -(TargetPoint.Y - LocalPosition.Y);
        if (PY == 0.0L) {
            PY = 1.0f;
        }
        float Angle = Math::ArcTan2(TargetPoint.X - LocalPosition.X, PY);
        float Sine = System::Sin(Angle);
        float Cosine = System::Cos(Angle);
        PPDWeaponParticle Particle = Particles;
        std::int32_t Count = ParticleCount;
        if (GlobalsV::HardwareRenderingEnabled) {
            while (Count > 0) {
                if (Particle->Kind >= 2) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    GR_DX::QueueDrawPoint(X, Y, GR_DX::Color565ToArgb(Particle->Color), Particle->Alpha);
                }
                Particle = static_cast<PPDWeaponParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TPDWeaponParticle))));
                --Count;
            }
            GR_DX::FlushDrawPoints(&ClipRect);
        } else {
            while (Count > 0) {
                if (Particle->Kind >= 2) {
                    PX = Particle->Position.X;
                    PY = static_cast<long double>(Particle->Position.Y) * Scale;
                    X = System::Trunc(static_cast<long double>(PX) * Cosine + static_cast<long double>(PY) * Sine) + AbsolutePosition.X;
                    Y = System::Trunc(static_cast<long double>(PX) * Sine - static_cast<long double>(PY) * Cosine) + AbsolutePosition.Y;
                    if (X >= ClipRect.Left && X < ClipRect.Right && Y >= ClipRect.Top && Y < ClipRect.Bottom) {
                        GR_Main::ScreenRenderBuffer->BlendPixel16(X, Y, Particle->Color, Particle->Alpha);
                    }
                }
                Particle = static_cast<PPDWeaponParticle>(EC_Mem::AddPointerOffset(Particle, static_cast<std::int32_t>(sizeof(TPDWeaponParticle))));
                --Count;
            }
        }
    }

    void TPSPDWeaponGI::p_destroy() {
        GI_PDTurretWeapon::TPSPDWeaponGI_Destroy(this);
    }

} // namespace GI_PDTurretWeapon
