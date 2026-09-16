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
        // An empty or invalid expression preserves the current parameter value.
        void EvaluateChangeExpression(pas::List*& Parameters);
        void ApplyChange(pas::List*& Parameters);
        // Invalid indices and disabled parameters pass; full noncritical bounds impose no constraint.
        std::uint8_t AcceptsParameter(pas::List* Parameters);
        // Legacy readers leave ParameterIndex zero; the location/path reader assigns it.
        // Quest versions 1111111111..1111111115.
        void LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111116.
        void LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111117..1111111118.
        void LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111119..1111111124.
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
        // Loaded and cleared, but not read by the execution routines.
        std::uint8_t LegacyFlag;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ParameterDeltaClass
