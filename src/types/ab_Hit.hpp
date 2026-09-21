#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace ab_Hit {
    struct TabHit;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabHit : ab_Object::TabObject {
        PAS_CLASS_META(TabHit, ab_Object::TabObject, "TabHit", 208)
        void p_destroy() override;
        void ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) override;
        void UpdateState() override;
        void Advance() override;
        void UpdateVisuals() override;
        void ExplosionComplete(GI_MessageLoop::TObjectGI* Sender);
        void KellerDeathComplete(GI_MessageLoop::TObjectGI* Sender);
        void HitEffectComplete(GI_MessageLoop::TObjectGI* Sender);
        std::int32_t Health;
        std::int32_t MaxHealth;
        std::int32_t DisruptUntilTick;
        std::int32_t EffectOriginSpread;
        double TurnSpeedScale;
        pas::List* Effects;
        // Tested by TfAB.DrawShipHealthBars; hidden during Keller breakup/death and on fragments.
        std::uint8_t HealthBarVisible;
        std::uint8_t cpp_padding[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_Hit
