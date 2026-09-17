#pragma once
#include "runtime_support.hpp"
#include "types/ab_Object.hpp"
#include "types/ab_WorldImage.hpp"

namespace ab_W08 {
    struct TabW08;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabW08 : ab_Object::TabObject {
        PAS_CLASS_META(TabW08, ab_Object::TabObject, "TabW08", 196)
        void p_destroy() override;
        void Launch(ab_Object::TabObject* Owner, std::int32_t Amount, float Angle, std::int32_t AGeneration, ab_Object::TabObject* Origin);
        void virtual_TabObject_Advance() override;
        void UpdateVisuals() override;
        std::int32_t Damage;
        ab_WorldImage::PabWorldImage Image;
        std::uint8_t Exploding;
        std::uint8_t cpp_padding[3];
        std::int32_t Generation;
        std::int32_t ExpireTick;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ab_W08
