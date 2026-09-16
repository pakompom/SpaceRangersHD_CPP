#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W09 {
    struct TabW09;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW09 : ab_Object::TabObject {
        PAS_CLASS_META(TabW09, ab_Object::TabObject, "TabW09", 192)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount);
        void LaunchChild(TabW09* Parent, float Angle);
        void Advance() override;
        void UpdateVisuals() override;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        // 0 parent, 1 parent explosion, 2 child, 3 child explosion.
        std::int32_t Phase;
        std::int32_t ExpireTick;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W09
