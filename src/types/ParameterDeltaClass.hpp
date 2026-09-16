#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EventClass {
    struct TEvent;

} // namespace EventClass

namespace TextFieldClass {
    struct TTextField;

} // namespace TextFieldClass

namespace ValueListClass {
    struct TValuesList;

} // namespace ValueListClass

namespace ParameterDeltaClass {
    struct TParameterDelta;

    enum TParameterVisibilityChange : std::uint32_t {
        pvcUnchanged = 0,
        pvcShow = 1,
        pvcHide = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TParameterDelta : EC_Struct::TObjectEx {
        PAS_CLASS_META(TParameterDelta, EC_Struct::TObjectEx, "TParameterDelta", 48)
        void p_destroy() override;
        void Reset();
        void ClearValueConstraints();
        void ClearChange();
        std::uint8_t HasNoValueConstraint(pas::List* Parameters);
        std::uint8_t HasNoChange(pas::List* Parameters);
        void EvaluateChangeExpression(pas::List*& Parameters);
        void ApplyChange(pas::List*& Parameters);
        std::uint8_t AcceptsParameter(pas::List* Parameters);
        void LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader);
        void LoadValueConstraintsFromReader(EC_Buf::TBufEC* Reader);
        void LoadChangeFromReader(EC_Buf::TBufEC* Reader);
        std::int32_t ParameterIndex;
        ValueListClass::TValuesList* ValueConstraint;
        ValueListClass::TValuesList* MultipleConstraint;
        std::int32_t MinValue;
        std::int32_t MaxValue;
        std::int32_t ChangeValue;
        std::uint8_t ChangeByPercent;
        std::uint8_t SetValue;
        std::uint8_t UseExpression;
        std::uint8_t cpp_padding[1];
        TextFieldClass::TTextField* ExpressionText;
        EventClass::TEvent* CriticalEvent;
        TParameterVisibilityChange VisibilityChange;
        std::uint8_t LegacyFlag;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ParameterDeltaClass
