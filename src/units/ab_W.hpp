#pragma once
#include "types/aConst.hpp"
#include "types/ab_W.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace ab_Object {
    struct TabObject;

} // namespace ab_Object

namespace ab_W {
    void ab_Weapon_InitializeFromInfo(PabWeapon Weapon, aConst::PWeaponInfo Info);

    void ab_Weapon_Initialize(PabWeapon Weapon, std::uint8_t ItemType);

    void ab_Weapon_Fire(PabWeapon Weapon, ab_Object::TabObject* Owner, float DamageScale);

    void ab_Weapon_QueueImageLoad(PabWeapon Weapon, pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);

} // namespace ab_W
