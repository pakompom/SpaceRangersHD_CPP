#pragma once
#include "runtime_support.hpp"
#include "types/ab_Hit.hpp"
#include "types/ab_W.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Space {
    struct TObjectSE;

} // namespace SE_Space

namespace ab_Global {
    struct TSphericalBearingDistance;

} // namespace ab_Global

namespace ab_Object {
    struct TabObject;

} // namespace ab_Object

namespace ab_Ship {
    struct TabShip;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabShip : ab_Hit::TabHit {
        PAS_CLASS_META(TabShip, ab_Hit::TabHit, "TabShip", 736)
        void p_destroy() override;
        void CreateShipVisual(const pas::WideString& GraphKey, std::int32_t Diameter);
        void CreateRuinsVisual(const pas::WideString& GraphKey, std::int32_t Diameter);
        void AttachVisual();
        void DetachVisual();
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        TabShip* FindNearestEnemy(ab_Object::TabObject* Origin);
        TabShip* FindNearestEnemyWithBearing(ab_Object::TabObject* Origin, ab_Global::TSphericalBearingDistance& Bearing);
        void AddEnemy(TabShip* Ship);
        void AddTrackedShip(TabShip* Ship);
        void ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) override;
        void SetTurnInput(double Value);
        void StartThrust();
        void StopThrust();
        void StartReverseThrust();
        void Brake();
        void FirePrimary();
        void FireSecondary();
        void FirePrimaryAt(ab_Object::TabObject* Target);
        void FireSecondaryAt(ab_Object::TabObject* Target);
        void SelectWeapon(std::int32_t Index);
        std::uint8_t CanFireWeapon(std::int32_t Index);
        double MinimumWeaponRange();
        double MaximumWeaponRange();
        void AddWeapon(std::int32_t Kind);
        void UpdateState() override;
        void Advance() override;
        void UpdateVisuals() override;
        void UpdateAvoidanceDistances();
        void UpdateObstacleSensors();
        SE_Space::TObjectSE* Visual;
        std::int32_t VisualDiameter;
        SE_Space::TObjectSE* OffscreenMarker;
        GI_MessageLoop::TObjectGI* OffscreenLabel;
        pas::List* Enemies;
        pas::List* InitialEnemies;
        pas::List* TrackedShips;
        std::uint8_t cpp_padding[4];
        double TurnSpeed;
        double TurnInput;
        std::int32_t WeaponCount;
        std::uint8_t cpp_padding_2[4];
        pas::Array<ab_W::TabWeapon, 0, 4> Weapons;
        std::int32_t PrimaryWeapon;
        std::int32_t SecondaryWeapon;
        std::int32_t LastPrimaryWeapon;
        std::int32_t LastPrimaryFireTick;
        std::int32_t LastSecondaryWeapon;
        std::int32_t LastSecondaryFireTick;
        pas::Array<std::int32_t, 0, 7> BonusTicks;
        std::int32_t RevealTicks;
        std::uint8_t cpp_padding_3[4];
        double OuterAvoidanceDistance;
        double MiddleAvoidanceDistance;
        double InnerAvoidanceDistance;
        std::int32_t NextObstacleScanTick;
        std::uint8_t cpp_padding_4[4];
        pas::Array<double, 0, 7> ObstacleDistances;
        pas::Array<std::int32_t, 0, 7> ObstacleLevels;
        std::int32_t EncounterTag;
        std::int32_t TickCounter;
        std::uint8_t ConvertedFromGameShip;
        std::uint8_t RandomRewardsDisabled;
        std::uint8_t cpp_padding_5[2];
        pas::WideString SpawnGraphKey;
        pas::WideString ScriptLabel;
        pas::Object* RewardObject;
        std::uint8_t Team;
        std::uint8_t cpp_padding_6[3];
        std::int32_t HealthScalePercent;
        std::int32_t DamageScalePercent;
        std::uint8_t HasFiredWeapon;
        std::uint8_t cpp_padding_7[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_Ship
