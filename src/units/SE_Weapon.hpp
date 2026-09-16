#pragma once
#include "types/SE_Weapon.hpp"
#include "types/Types.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace SE_Weapon {
    void InitializeWeaponVisualResources();

    void TWeaponSE_Create(TWeaponSE* Self, const pas::WideString& GraphKey, Types::TPoint UnusedPosition, std::int32_t ShotVisual, std::int32_t Variant);

    void TWeaponSE_Destroy(TWeaponSE* Self);

    void TWeaponEffect_Create(TWeaponEffect* Self, std::int32_t AEffectIndex, GI_MessageLoop::TObjectGI* AOwner);

    void TWeaponEffect_Destroy(TWeaponEffect* Self);

} // namespace SE_Weapon
