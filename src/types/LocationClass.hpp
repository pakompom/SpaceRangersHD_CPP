#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EventClass {
    struct TEvent;

} // namespace EventClass

namespace ParameterDeltaClass {
    struct TParameterDelta;

} // namespace ParameterDeltaClass

namespace SequenceClass {
    struct TSequence;

} // namespace SequenceClass

namespace TextFieldClass {
    struct TTextField;

} // namespace TextFieldClass

namespace LocationClass {
    struct TLocation;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TLocation : EC_Struct::TObjectEx {
        PAS_CLASS_META(TLocation, EC_Struct::TObjectEx, "TLocation", 64)
        void p_destroy() override;
        void Reset();
        std::int32_t GetParameterChangeCount();
        ParameterDeltaClass::TParameterDelta* GetParameterChange(std::int32_t Index);
        void AddParameterChange(ParameterDeltaClass::TParameterDelta* Change);
        void ApplyParameterChanges(pas::List*& Parameters);
        void PruneParameterChanges(pas::List* Parameters);
        ParameterDeltaClass::TParameterDelta* FindParameterChange(std::int32_t ParameterIndex);
        void AddEvent();
        void RemoveLastEvent();
        void LoadFromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV8FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV7FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV6FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV5FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader);
        EventClass::TEvent* SelectEvent(pas::List*& Parameters);
        pas::List* ParameterChanges;
        std::int32_t EditorX;
        std::int32_t EditorY;
        std::int32_t Days;
        std::int32_t Id;
        std::int32_t EventCount;
        pas::DynArray<EventClass::TEvent*> Events;
        std::uint8_t UseEventExpression;
        std::uint8_t cpp_padding[3];
        std::int32_t NextEventIndex;
        TextFieldClass::TTextField* EventExpression;
        std::uint8_t IsDeath;
        std::uint8_t IsEmpty;
        std::uint8_t IsStart;
        std::uint8_t IsSuccess;
        std::uint8_t IsFailure;
        std::uint8_t cpp_padding_2[3];
        std::int32_t VisitLimit;
        std::int32_t VisitCount;
        SequenceClass::TSequence* Sequence;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace LocationClass
