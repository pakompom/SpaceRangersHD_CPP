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

namespace PathClass {
    struct TPath;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPath : EC_Struct::TObjectEx {
        PAS_CLASS_META(TPath, EC_Struct::TObjectEx, "TPath", 232)
        void p_destroy() override;
        void Reset();
        std::int32_t GetParameterChangeCount();
        // Index is one-based.
        ParameterDeltaClass::TParameterDelta* GetParameterChange(std::int32_t Index);
        void AddParameterChange(ParameterDeltaClass::TParameterDelta* Change);
        // Evaluates every expression before applying any change.
        void ApplyParameterChanges(pas::List*& Parameters);
        // Removed entries are not freed.
        void PruneParameterChanges(pas::List* Parameters);
        // Updates Available. Invalid condition expressions are ignored; parameter constraints still apply.
        std::uint8_t CheckAvailable(pas::List* Parameters);
        ParameterDeltaClass::TParameterDelta* FindParameterChange(std::int32_t ParameterIndex);
        // Path format used by quest versions 1111111125 and later.
        void LoadFromReader(EC_Buf::TBufEC* Reader, pas::List* Parameters);
        // Quest version 1111111124.
        void LoadLegacyV9FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111123.
        void LoadLegacyV8FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111122.
        void LoadLegacyV7FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111119..1111111121.
        void LoadLegacyV6FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111117..1111111118.
        void LoadLegacyV5FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111116.
        void LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111115.
        void LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader);
        // Quest version 1111111114.
        void LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader);
        // Quest versions 1111111112..1111111113.
        void LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader);
        // Legacy readers derive IsAutomatic from the trimmed caption.
        // Quest version 1111111111.
        void LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader);
        pas::List* ParameterChanges;
        double Priority;
        std::uint8_t IsAutomatic;
        // Only reset; its purpose is unresolved.
        std::uint8_t UnknownFlag;
        std::uint8_t AlwaysShow;
        std::uint8_t Available;
        std::int32_t Days;
        std::int32_t DisplayOrder;
        std::int32_t Id;
        std::int32_t TraversalLimit;
        std::int32_t TraversalCount;
        std::int32_t FromLocationId;
        std::int32_t ToLocationId;
        TextFieldClass::TTextField* Caption;
        EventClass::TEvent* Event;
        TextFieldClass::TTextField* ConditionExpression;
        std::uint8_t cpp_padding[168];
        // No recovered routine accesses...
        SequenceClass::TSequence* Sequence;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace PathClass
