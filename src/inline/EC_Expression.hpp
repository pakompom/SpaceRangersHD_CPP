#pragma once
#include "units/EC_Expression.hpp"
#include "units/EC_Str.hpp"

namespace EC_Expression {
    // Case-sensitive ordinal comparison; accepts nil and returns -1, 0 or 1.
    // Preserve nil ordering, unsigned UTF-16 comparison and the first NUL boundary.
    inline std::int32_t CompareScriptNames(char16_t* Left, char16_t* Right) {
        return EC_Str::CompareWideChars(Left, Right);
    }

    inline void TVarArrayEC_Create(TVarArrayEC* Self) {
        pas::object_create(Self);
    }

    inline void TCodeAnalyzerEC_Create(TCodeAnalyzerEC* Self) {
        pas::object_create(Self);
    }

} // namespace EC_Expression
