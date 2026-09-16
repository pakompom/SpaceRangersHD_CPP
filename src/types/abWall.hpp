#pragma once
#include "runtime_support.hpp"
#include "types/ab_Hit.hpp"
#include "types/ab_StopLine.hpp"
#include "types/ab_WorldImage.hpp"
#include "types/ab_Zone.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace ab_Object {
    struct TabObject;

} // namespace ab_Object

namespace abWall {
    struct TabWall;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TabWall : ab_Hit::TabHit {
        PAS_CLASS_META(TabWall, ab_Hit::TabHit, "TabWall", 224)
        void p_destroy() override;
        void BindZone(ab_Zone::PabZone Value);
        // Empty in this native version; called after arena wall setup.
        static void AttachVisual();
        void QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) override;
        void ApplyDamage(std::int32_t Amount, ab_Object::TabObject* Source, std::uint8_t Disrupt) override;
        void UpdateState() override;
        void Advance() override;
        void UpdateVisuals() override;
        ab_Zone::PabZone Zone;
        ab_WorldImage::PabWorldImage WorldImage;
        std::int32_t DirectionFrameCount;
        ab_StopLine::PabStopPoint StopPoint;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace abWall
