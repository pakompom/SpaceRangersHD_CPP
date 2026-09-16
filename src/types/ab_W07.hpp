#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W07 {
    struct TabW07;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW07 : ab_Object::TabObject {
        PAS_CLASS_META(TabW07, ab_Object::TabObject, "TabW07", 204)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Offset);
        void Advance() override;
        void UpdateVisuals() override;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        std::uint8_t Exploding;
        std::uint8_t cpp_padding[3];
        float TrailDistance;
        std::int32_t ExpireTick;
        float TurnSpeed;
        pas::List* TrailImages;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W07
