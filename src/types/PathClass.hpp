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
        ParameterDeltaClass::TParameterDelta* GetParameterChange(std::int32_t Index);
        void AddParameterChange(ParameterDeltaClass::TParameterDelta* Change);
        void ApplyParameterChanges(pas::List*& Parameters);
        void PruneParameterChanges(pas::List* Parameters);
        std::uint8_t CheckAvailable(pas::List* Parameters);
        ParameterDeltaClass::TParameterDelta* FindParameterChange(std::int32_t ParameterIndex);
        void LoadFromReader(EC_Buf::TBufEC* Reader, pas::List* Parameters);
        void LoadLegacyV9FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV8FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV7FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV6FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV5FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV4FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader);
        void LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader);
        pas::List* ParameterChanges;
        double Priority;
        std::uint8_t IsAutomatic;
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
        SequenceClass::TSequence* Sequence;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace PathClass
