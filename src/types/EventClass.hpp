#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace TextFieldClass {
    struct TTextField;

} // namespace TextFieldClass

namespace EventClass {
    struct TEvent;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TEvent : EC_Struct::TObjectEx {
        PAS_CLASS_META(TEvent, EC_Struct::TObjectEx, "TEvent", 20)
        void p_destroy() override;
        void ClearTextFields();
        void Assign(TEvent* Source);
        TextFieldClass::TTextField* Text;
        TextFieldClass::TTextField* Picture;
        TextFieldClass::TTextField* Music;
        TextFieldClass::TTextField* Sound;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EventClass
