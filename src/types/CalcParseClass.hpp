#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace CPVarClass {
    struct TCPVariant;

} // namespace CPVarClass

namespace CalcParseClass {
    struct TCalcParse;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCalcParse : EC_Struct::TObjectEx {
        PAS_CLASS_META(TCalcParse, EC_Struct::TObjectEx, "TCalcParse", 28)
        static void ApplyPower(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyAdd(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplySubtract(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyMultiply(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyPercentChange(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyDivide(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyIntDivide(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyModulo(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyRange(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyMembership(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyLessThan(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyGreaterThan(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyLessOrEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyGreaterOrEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyNotEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyAnd(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyOr(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static pas::WideString NormalizeTokens(pas::WideString& Text);
        pas::WideString FormatTokens(pas::WideString& Text);
        static std::int32_t GetOperatorRank(char16_t Token);
        pas::WideString CollapseOperatorRun(const pas::WideString& Text);
        pas::WideString NormalizeFragments(const pas::WideString& Text);
        pas::WideString NormalizeScalarFragment(pas::WideString Text);
        pas::WideString NormalizeBracketFragment(pas::WideString Text);
        pas::WideString NormalizeParameterReference(pas::WideString Text);
        pas::WideString NormalizeRangeLiteral(pas::WideString Text);
        static pas::WideString InsertImplicitMultiplication(pas::WideString Text);
        std::int32_t FindTopLevelOperator(const pas::WideString& Text, std::int32_t TextLength);
        CPVarClass::TCPVariant* EvaluateExpression(pas::WideString Text);
        void Evaluate(pas::List* Parameters);
        void Prepare(pas::WideString Text, std::int32_t DefaultParameterIndex);
        void Reset();
        static std::uint8_t HasBalancedParenthesesInSlice(const pas::WideString& Text, std::int32_t FirstIndex, std::int32_t LastIndex);
        pas::WideString SubstituteParameters(pas::List* Parameters);
        std::uint8_t HasBalancedParentheses(const pas::WideString& Text);
        pas::WideString ClampNumericLiterals(pas::WideString Text);
        pas::WideString SourceText;
        pas::WideString Expression;
        std::int32_t ResultValue;
        std::int32_t ResetValue10;
        std::uint8_t UsesDefaultParameter;
        std::uint8_t SourceWasChanged;
        std::uint8_t UnbalancedParentheses;
        std::uint8_t InvalidNumericLiteral;
        std::uint8_t InvalidParameterReference;
        std::uint8_t InvalidRangeLiteral;
        std::uint8_t EvaluationError;
        std::uint8_t HasError;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace CalcParseClass
