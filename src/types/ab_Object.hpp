#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/ab_Global.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GR_Sound {
    struct TSoundBufferControl;

} // namespace GR_Sound

namespace ab_Object {
    struct TabObject;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabObject : EC_Struct::TObjectEx {
        PAS_CLASS_META(TabObject, EC_Struct::TObjectEx, "TabObject", 176)
        void p_destroy() override;
        EC_Struct::TVector3D GetWorldPosition();
        EC_Struct::TVector3D GetProjectedPosition();
        double DistanceTo(TabObject* Other);
        ab_Global::TSphericalBearingDistance BearingAndDistanceTo(TabObject* Other);
        double GetProjectedHeading(EC_Struct::TVector3D Position);
        void ChangeSpeed(double Delta);
        std::uint8_t CollidesWith(TabObject* Other);
        TabObject* FindCollision();
        virtual void ApplyDamage(std::int32_t Amount, TabObject* Source, std::uint8_t Disrupt);
        virtual void UpdateState();
        virtual void virtual_TabObject_Advance();
        virtual void UpdateVisuals();
        virtual void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);
        std::int32_t RandomRange(std::int32_t BoundA, std::int32_t BoundB);
        TabObject* Prev;
        TabObject* Next;
        std::uint8_t cpp_padding[4];
        ab_Global::TSphericalBearingState State;
        double Mass;
        double Thrust;
        EC_Struct::TPointF Velocity;
        double MaxSpeed;
        double SpeedScale;
        double DistanceTravelled;
        double CollisionRadius;
        std::uint8_t Collidable;
        std::uint8_t cpp_padding_2[7];
        double ZoneRadius;
        std::uint8_t DeletionPending;
        std::uint8_t WallCollisionEnabled;
        std::uint8_t GravityEnabled;
        std::uint8_t ZoneDamageEnabled;
        std::uint8_t Active;
        std::uint8_t cpp_padding_3[3];
        // Non-owning original firing object: Launch stores Owner (), child
        // projectiles inherit it, and hits pass it as damage source ().
        // Used to exclude self-collisions and select enemies; cleared on destruction.
        TabObject* SourceObject;
        std::uint32_t InitialRandomSeed;
        std::uint32_t RandomState;
        std::int32_t SoundDelay;
        pas::WideString SoundPath;
        std::int32_t SoundGroup;
        GR_Sound::TSoundBufferControl* Sound;
        float WeaponDamageScale;
        float AmmoRechargeScale;
        float MovementScale;
        float GravityScale;
        float RegenerationRate;
        float DamageTakenScale;
        // Default 1; SF_ABShipModifiers exposes luck. Multiplies the random reward roll.
        float LuckScale;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_Object
