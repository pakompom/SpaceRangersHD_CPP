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
        // Operators borrow operands; OutValue must be an existing, distinct object.
        // Power/add/subtract/multiply promote floats; integer results saturate at +/-2000000000.
        // Negative bases stay negative even for even exponents; integer results round.
        static void ApplyPower(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyAdd(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplySubtract(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyMultiply(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Returns float Left * (1 + Right * 0.01).
        static void ApplyPercentChange(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Exact integer quotients stay integer. Zero divisor: +/-2000000000 for integers; float operands incorrectly leave integer zero.
        static void ApplyDivide(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Truncates toward zero; zero-divisor behavior matches ApplyDivide.
        static void ApplyIntDivide(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Float operands produce a float remainder after truncation. Zero-divisor behavior matches ApplyDivide.
        static void ApplyModulo(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Uses operand extrema, rounds floats and swaps reversed bounds. Range operands must be nonempty.
        static void ApplyRange(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Two ranges sample Left once; scalar/range membership rounds the scalar.
        static void ApplyMembership(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Comparisons return integer 0 or 1 and sample each range operand once.
        static void ApplyLessThan(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyGreaterThan(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyLessOrEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyGreaterOrEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        static void ApplyNotEqual(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Range operands concatenate rather than intersect; duplicates remain.
        static void ApplyAnd(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // Range operands concatenate; duplicates remain.
        static void ApplyOr(CPVarClass::TCPVariant*& Left, CPVarClass::TCPVariant*& Right, CPVarClass::TCPVariant*& OutValue);
        // External spellings -> internal tokens: pct %, div f, mod g, in #,
        // to $, or |, and &, <> e, >= c, <= b, .. h, and decimal dot -> comma.
        // Text is read-only despite var. Uses ANSI lowercase and boundary-free substitutions; always wraps the result in parentheses.
        static pas::WideString NormalizeTokens(pas::WideString& Text);
        // Text is read-only despite var. Removes at most one enclosing parenthesis pair; leaves outer whitespace.
        pas::WideString FormatTokens(pas::WideString& Text);
        // Lower ranks bind tighter; -1 means not an operator.
        // 1: ^ / f g; 2: * %; 3: -; 4: +; 5: $; 6: #;
        // 7: < > = b c e; 8: &; 9: |.
        static std::int32_t GetOperatorRank(char16_t Token);
        // Requires a nonempty operator run. Minus parity controls the sign; ties choose the leftmost weakest operator.
        pas::WideString CollapseOperatorRun(const pas::WideString& Text);
        // Square brackets do not nest. An unmatched opening bracket silently discards the remaining suffix.
        pas::WideString NormalizeFragments(const pas::WideString& Text);
        // Silently discards unsupported characters, including decimal dots; call NormalizeTokens first.
        pas::WideString NormalizeScalarFragment(pas::WideString Text);
        // Any lowercase p selects parameter parsing, even outside the [pN] form.
        pas::WideString NormalizeBracketFragment(pas::WideString Text);
        // Uses only the first three digits; zero/missing digits produce [err]. Does not check parameter-list bounds.
        pas::WideString NormalizeParameterReference(pas::WideString Text);
        // Requires internal h notation, not '..'. Empty or rejected input yields [err]; existing errors remain set.
        pas::WideString NormalizeRangeLiteral(pas::WideString Text);
        static pas::WideString InsertImplicitMultiplication(pas::WideString Text);
        // One-based; zero when absent. Rightmost ties give left associativity. Delimiter balance is unchecked.
        std::int32_t FindTopLevelOperator(const std::u16string_view& Text, std::int32_t TextLength);
        // Caller owns the result. Evaluates right before left without short-circuiting. EvaluationError blocks evaluation; HasError alone does not. Native recursive intermediates leak.
        CPVarClass::TCPVariant* EvaluateExpression(pas::WideString Text);
        // Existing HasError preserves ResultValue; flags are not reset. Native scratch and returned variants leak.
        void Evaluate(pas::List* Parameters);
        // Resets state; stores Expression even on error. Empty input becomes (), not the default parameter.
        void Prepare(pas::WideString Text, std::int32_t DefaultParameterIndex);
        void Reset();
        // One-based inclusive bounds, unchecked. Empty slices pass; square brackets are ignored.
        static std::uint8_t HasBalancedParenthesesInSlice(const std::u16string_view& Text, std::int32_t FirstIndex, std::int32_t LastIndex);
        // Parameters: borrowed, non-nil TList of TParameter; [pN] is one-based.
        // Unmatched references remain unchanged; negative values are parenthesized.
        pas::WideString SubstituteParameters(pas::List* Parameters);
        // Empty text passes.
        std::uint8_t HasBalancedParentheses(const std::u16string_view& Text);
        // Nonzero limits: 0.0001..999999999. The lower clamp emits a dot-decimal literal that evaluation rejects. Drops trailing numbers; conversion errors set flags and leave the caller's result storage unchanged.
        pas::WideString ClampNumericLiterals(pas::WideString Text);
        pas::WideString SourceText;
        // Internal tokens; parameters not yet substituted.
        pas::WideString Expression;
        std::int32_t ResultValue;
        // Reset to zero; purpose remains unresolved.
        std::int32_t ResetValue10;
        // Processed text was empty or exactly the fallback [pN].
        std::uint8_t UsesDefaultParameter;
        // Compared against the readable form, not internal tokens.
        std::uint8_t SourceWasChanged;
        std::uint8_t UnbalancedParentheses;
        // EConvertError during preparation.
        std::uint8_t InvalidNumericLiteral;
        std::uint8_t InvalidParameterReference;
        std::uint8_t InvalidRangeLiteral;
        std::uint8_t EvaluationError;
        // Also set for empty parentheses.
        std::uint8_t HasError;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace CalcParseClass
