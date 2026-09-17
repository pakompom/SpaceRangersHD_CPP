#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/aConst.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace aEFilm {
    struct TEFilmObj;

} // namespace aEFilm

namespace aGalaxy {
    struct TGalaxy;

    struct TStar;

} // namespace aGalaxy

namespace aItem {
    struct TWeapon;

} // namespace aItem

namespace aShip {
    struct TShip;

} // namespace aShip

namespace aMissile {
    struct TMissile;

    struct TCustomMissile;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TMissile : EC_Struct::TObjectEx {
        PAS_CLASS_META(TMissile, EC_Struct::TObjectEx, "TMissile", 116)
        void p_destroy() override;
        // Registers the missile in Star, copies weapon data and initializes position, heading and speed.
        void InitializeShot(aGalaxy::TStar* Star, aShip::TShip* OwnerShip, aItem::TWeapon* Weapon, pas::Object* Target, std::int32_t ShotIndex);
        void InitializeUnownedShot(aGalaxy::TStar* Star, pas::Object* Target, std::int32_t X, std::int32_t Y, float Direction, std::int32_t MinDamage, std::int32_t MaxDamage, float MaximumSpeed, std::uint8_t ItemType, std::int32_t ModuleIndex, std::int32_t SpecialIndex);
        virtual void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        virtual void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* World);
        void ResolveLoadedReferences(aGalaxy::TGalaxy* World);
        // Lazily creates and initializes the retained missile scene object.
        SE_Space::TObjectSE* GetGraphObject();
        // Returns a hit ship, item or asteroid, or nil when no object was hit.
        pas::Object* StepDay(std::int32_t StepIndex, std::uint8_t RecordFilm);
        std::uint8_t TryReturnToOwner(std::int32_t StepIndex, std::uint8_t RecordFilm, EC_Struct::TPointF PreviousPosition, aShip::TShip* Ship);
        void RetargetTorpedo();
        pas::WideString GetDisplayName();
        pas::WideString GetInfoText();
        std::uint8_t CanBeHit(aShip::TShip* Attacker, aItem::TWeapon* UnusedWeapon);
        void ClearReferencesTo(pas::Object* Obj);
        // Special micromodule override unless -1, otherwise the weapon-info default.
        std::int32_t GetShotVisual();
        virtual pas::WideString GetGraphSuffix();
        virtual aConst::PWeaponInfo GetWeaponInfo();
        SE_Space::TObjectSE* Graphic;
        std::uint32_t Id;
        // Matches TItem.Id.
        std::int32_t WeaponId;
        std::uint8_t ItemType;
        std::uint8_t TechLevel;
        std::uint8_t cpp_padding[2];
        std::int32_t MinDamage;
        std::int32_t MaxDamage;
        // Copied from the weapon: one-based MicroModuleTemplates indexes, zero when absent.
        std::int32_t MicroModuleIndex;
        std::int32_t SpecialModuleIndex;
        EC_Struct::TPointF Position;
        float Direction;
        float Speed;
        float MaximumSpeed;
        aGalaxy::TStar* CurrentStar;
        aShip::TShip* OwnerShip;
        pas::Object* Target;
        // Last invalidated target, excluded by retargeting.
        pas::Object* PreviousTarget;
        std::int32_t ShotIndex;
        float TurnDirection;
        float SourceHeading;
        // Advanced by 200 / MovementStepCount.
        std::int32_t FlightTicks;
        // StepDay marks intercepted/expired missiles; NextDay removes them.
        std::uint8_t DestroyQueued;
        std::uint8_t cpp_padding_2[3];
        aEFilm::TEFilmObj* FilmObject;
        // Serialized target discriminator.
        std::uint8_t SavedTargetKind;
        std::uint8_t SavedPreviousTargetKind;
        std::uint8_t cpp_padding_3[2];
        EC_Struct::TPointF LastTargetPosition;
        // Squared distance used to detect overshooting, not a random seed.
        float LastTargetDistance;
        std::int32_t OvershootTicks;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCustomMissile : aMissile::TMissile {
        PAS_CLASS_META(TCustomMissile, aMissile::TMissile, "TCustomMissile", 120)
        // Caches Weapon.GetWeaponInfo before the base initializer.
        void InitializeShot_2(aGalaxy::TStar* Star, aShip::TShip* OwnerShip, aItem::TWeapon* Weapon, pas::Object* Target, std::int32_t ShotIndex);
        void InitializeUnownedShot_2(aGalaxy::TStar* Star, pas::Object* Target, std::int32_t X, std::int32_t Y, float Direction, std::int32_t MinDamage, std::int32_t MaxDamage, float MaximumSpeed, pas::WideString WeaponName, std::int32_t ModuleIndex, std::int32_t SpecialIndex);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer) override;
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer, aGalaxy::TGalaxy* World) override;
        pas::WideString GetGraphSuffix() override;
        aConst::PWeaponInfo GetWeaponInfo() override;
        // Borrowed custom weapon definition.
        aConst::PWeaponInfo WeaponInfo;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace aMissile
