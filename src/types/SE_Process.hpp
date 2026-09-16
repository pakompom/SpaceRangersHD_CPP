#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_BlockPar {
    struct TBlockParEC;

} // namespace EC_BlockPar

namespace GI_MessageLoop {
    struct TMessageLoopGI;

    struct TObjectGI;

} // namespace GI_MessageLoop

namespace GI_Panel {
    struct TPanelGI;

} // namespace GI_Panel

namespace SE_Process {
    struct TProcessSE;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TProcessSE : pas::Object {
        PAS_CLASS_META(TProcessSE, pas::Object, "TProcessSE", 80)
        void p_destroy() override;
        void AddObject(SE_Space::TObjectSE* Obj);
        void RemoveObject(SE_Space::TObjectSE* Obj);
        virtual void OpenSpace(GI_Panel::TPanelGI* MapPanel, GI_MessageLoop::TMessageLoopGI* Screen);
        virtual void BindMinimap(GI_MessageLoop::TObjectGI* Control);
        virtual void CloseSpace();
        std::uint8_t IsSpaceOpen();
        void PopulateAmbientObjects(std::int32_t Radius, std::int32_t BackgroundImage, std::uint32_t Seed);
        void StartBackgroundEffects();
        void StopBackgroundEffects();
        void AdvanceBackgroundEffects(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData);
        void UpdateViewRect();
        static pas::WideString SelectBackgroundAnimation();
        virtual void LoadFromBlock(EC_BlockPar::TBlockParEC* Block);
        SE_Space::TSpaceSE* Space;
        SE_Space::TObjectSE* FirstObject;
        SE_Space::TObjectSE* LastObject;
        pas::List* RetainedObjects;
        SE_Space::PSpaceTimerSE BackgroundTimer;
        WindowsSdk::TRect PreviousViewRect;
        WindowsSdk::TRect ViewRect;
        EC_Struct::TPointF RadarCenter;
        std::int32_t RadarRange;
        std::int32_t ActionRange;
        std::uint32_t ActionColor;
        std::int32_t SystemRadius;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SE_Process
