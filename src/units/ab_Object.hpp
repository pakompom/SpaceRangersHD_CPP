#pragma once
#include "types/ab_Object.hpp"

namespace GI_MessageLoop {
    struct TObjectGI;

} // namespace GI_MessageLoop

namespace ab_Object {
    extern ab_Object::TabObject* FirstArcadeObject;

    extern ab_Object::TabObject* LastArcadeObject;

    void ab_Object_Clear();

    void ab_Object_Add(TabObject* Obj);

    void ab_Object_Delete(TabObject* Obj);

    void ab_Object_QueueImageLoads(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner);

    void ab_Object_UpdateSounds();

    void TabObject_Create(TabObject* Self);

    void TabObject_Destroy(TabObject* Self);

    void TabObject_Advance(TabObject* Self);

} // namespace ab_Object
