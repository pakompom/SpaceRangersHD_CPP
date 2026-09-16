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
        // Retains the first event; frees parameter changes and Sequence.
        void Reset();
        std::int32_t GetParameterChangeCount();
        // Index is one-based.
        ParameterDeltaClass::TParameterDelta* GetParameterChange(std::int32_t Index);
        void AddParameterChange(ParameterDeltaClass::TParameterDelta* Change);
        // Evaluates every expression before applying any change.
        void ApplyParameterChanges(pas::List*& Parameters);
        // Removed entries are not freed.
        void PruneParameterChanges(pas::List* Parameters);
        ParameterDeltaClass::TParameterDelta* FindParameterChange(std::int32_t ParameterIndex);
        void AddEvent();
        // Retains at least one event.
        void RemoveLastEvent();
        // Location format used by quest versions 1111111126 and later.
        void LoadFromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111125; repeated parameter indices overwrite earlier changes.
        void LoadLegacyV8FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111124.
        void LoadLegacyV7FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111123.
        void LoadLegacyV6FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111121..1111111122.
        void LoadLegacyV5FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111119..1111111120.
        void LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111117..1111111118.
        void LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111116.
        void LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111115.
        void LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111111..1111111114.
        void LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader);
        // Expression selection falls back to random choice.
        EventClass::TEvent* SelectEvent(pas::List*& Parameters);
        // Owns its TParameterDelta entries.
        pas::List* ParameterChanges;
        std::int32_t EditorX;
        std::int32_t EditorY;
        std::int32_t Days;
        std::int32_t Id;
        std::int32_t EventCount;
        // Owned, one-based dynamic array.
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
