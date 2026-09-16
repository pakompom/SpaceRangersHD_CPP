#pragma once
#include "types/CalcParseClass.hpp"

namespace CalcParseClass {
    static_assert(sizeof(void*) != 4 || sizeof(CalcParseClass::TCalcParse) == 28);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, SourceText) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, Expression) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, ResultValue) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, ResetValue10) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, UsesDefaultParameter) == 20);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, SourceWasChanged) == 21);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, UnbalancedParentheses) == 22);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, InvalidNumericLiteral) == 23);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, InvalidParameterReference) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, InvalidRangeLiteral) == 25);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, EvaluationError) == 26);
    static_assert(sizeof(void*) != 4 || offsetof(CalcParseClass::TCalcParse, HasError) == 27);

} // namespace CalcParseClass
