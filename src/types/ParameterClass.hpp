#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/TextQuestInterface.hpp"

namespace CPDiapClass {
    struct TCPDiapazone;

} // namespace CPDiapClass

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EventClass {
    struct TEvent;

} // namespace EventClass

namespace ParViewStringClass {
    struct TParViewString;

} // namespace ParViewStringClass

namespace TextFieldClass {
    struct TTextField;

} // namespace TextFieldClass

namespace ParameterClass {
    struct TParameter;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TParameter : EC_Struct::TObjectEx {
        PAS_CLASS_META(TParameter, EC_Struct::TObjectEx, "TParameter", 60)
        void p_destroy() override;
        void Reset(std::int32_t Index);
        // ParameterIndex labels new entries; ViewStringCount is unchanged.
        void EnsureViewStringCapacity(std::int32_t RequiredCapacity, std::int32_t ParameterIndex);
        pas::WideString GetValueText(std::int32_t Value);
        std::int32_t GetNonCriticalMinimum();
        std::int32_t GetNonCriticalMaximum();
        // Clamps ordinary parameters to their bounds; money is only clamped at zero.
        void SetValue(std::int32_t NewValue);
        // Parameter format used by quest versions 1111111125 and later.
        void LoadFromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111121..1111111124.
        void LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111119..1111111120.
        void LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111118.
        void LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111113..1111111117.
        void LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111111..1111111112.
        void LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader);
        std::int32_t MinValue;
        std::int32_t MaxValue;
        std::int32_t Value;
        TextFieldClass::TTextField* NameText;
        EventClass::TEvent* CriticalEvent;
        EventClass::TEvent* CriticalEventOverride;
        TextQuestInterface::TQuestOutcome CriticalOutcome;
        std::uint8_t Hidden;
        std::uint8_t ShowWhenZero;
        std::uint8_t CriticalAtMinimum;
        std::uint8_t Enabled;
        std::uint8_t IsMoney;
        std::uint8_t cpp_padding[3];
        TextFieldClass::TTextField* ValueText;
        // Delphi dynamic array of owned entries, indexed from one.
        pas::DynArray<ParViewStringClass::TParViewString*> ViewStrings;
        std::int32_t ViewStringCount;
        std::int32_t ViewStringCapacity;
        CPDiapClass::TCPDiapazone* InitialRange;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace ParameterClass
