#pragma once
#include "units/EC_Expression.hpp"

namespace EC_Expression {
    inline std::int32_t CompareScriptNames(char16_t* Left, char16_t* Right) {
        return pas::compare_wide_chars(Left, Right);
    }

    inline void TVarArrayEC_SetNameOrderIndex(TVarArrayEC* Self, std::int32_t Index, std::int32_t DataIndex) {
        pas::store_unaligned<std::int32_t>(Self->NameOrder + Index, DataIndex);
    }

    inline std::int32_t TVarArrayEC_GetNameOrderIndex(TVarArrayEC* Self, std::int32_t Index) {
        return pas::load_unaligned<std::int32_t>(Self->NameOrder + Index);
    }

    inline TVarEC* TVarArrayEC_GetItemByNameOrder(TVarArrayEC* Self, std::int32_t Index) {
        return pas::load_unaligned<TVarEC*>(Self->Data + pas::load_unaligned<std::int32_t>(Self->NameOrder + Index));
    }

    inline std::int32_t TVarArrayEC_FindNameOrderForDataIndex(TVarArrayEC* Self, std::int32_t DataIndex) {
        return pas::index_in_slots<std::int32_t>(Self->NameOrder, Self->Count, DataIndex);
    }

    inline TVarEC* TVarArrayEC_GetItem(TVarArrayEC* Self, std::int32_t Index) {
        return pas::load_unaligned<TVarEC*>(Self->Data + Index);
    }

    inline void TVarArrayEC_SetItem(TVarArrayEC* Self, std::int32_t Index, TVarEC* Value) {
        pas::store_unaligned<TVarEC*>(Self->Data + Index, Value);
    }

    inline std::int32_t TVarArrayEC_IndexOf(TVarArrayEC* Self, TVarEC* Value) {
        return pas::index_in_slots<TVarEC*>(Self->Data, Self->Count, Value);
    }

    inline TExpressionVarEC* TExpressionEC_GetVariable(TExpressionEC* Self, std::int32_t Index) {
        return pas::load_unaligned<TExpressionVarEC*>(Self->Variables + Index);
    }

    inline void TExpressionEC_SetVariable(TExpressionEC* Self, std::int32_t Index, TExpressionVarEC* Value) {
        pas::store_unaligned<TExpressionVarEC*>(Self->Variables + Index, Value);
    }

    inline TExpressionInstrEC* TExpressionEC_GetInstruction(TExpressionEC* Self, std::int32_t Index) {
        return pas::load_unaligned<TExpressionInstrEC*>(Self->Instructions + Index);
    }

    inline void TExpressionEC_SetInstruction(TExpressionEC* Self, std::int32_t Index, TExpressionInstrEC* Value) {
        pas::store_unaligned<TExpressionInstrEC*>(Self->Instructions + Index, Value);
    }

} // namespace EC_Expression
