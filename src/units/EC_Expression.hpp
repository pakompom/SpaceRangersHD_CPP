#pragma once
#include "types/EC_Expression.hpp"

namespace EC_Expression {
    extern pas::Array<EC_Expression::TVarEC*, 0, 19> ScriptCallTrace;

    extern std::int32_t ScriptCallTracePosition;

    extern std::int32_t ScriptCallTraceCount;

    extern const pas::Array<char16_t, 0, 15> ScriptHexDigits;

    extern std::int32_t ScriptStepInterval;

    extern EC_Expression::TScriptStepCallback ScriptStepCallback;

    void SetScriptStepCallback(TScriptStepCallback Callback, std::int32_t Interval);

    void FreeScriptArrayTree(TVarArrayEC* Values);

    void GrowScriptArray(TVarArrayEC* Values, pas::OpenArray<std::int32_t> Dimensions, std::int32_t DimensionIndex);

    void ResizeScriptArray(TVarArrayEC* Values, std::int32_t Count);

    void RegisterExpressionBuiltins(TVarArrayEC* Scope);

    inline std::int32_t CompareScriptNames(char16_t* Left, char16_t* Right);

    pas::WideString TrimScriptString(pas::WideString Text);

    std::int32_t ScriptStringToInt(pas::WideString Text);

    pas::WideString ScriptFloatToString(double Value);

    pas::WideString ScriptDwordToHex(std::uint32_t Value);

    double ScriptStringToFloat(pas::WideString Text);

    std::uint8_t IsScriptIntegerText(pas::WideString Text);

    std::uint8_t IsNonIntegerScriptText(pas::WideString Text);

    std::uint8_t TryReadFloatLiteral(TCodeAnalyzerUnitEC*& Token, double& Value);

    std::uint8_t TryReadIntegerLiteral(TCodeAnalyzerUnitEC*& Token, std::int32_t& Value);

    std::uint8_t TryReadStringLiteral(TCodeAnalyzerUnitEC*& Token, pas::WideString& Value);

    std::uint8_t TryReadDwordLiteral(TCodeAnalyzerUnitEC*& Token, std::uint32_t& Value);

    std::uint8_t TryReadMemberName(TCodeAnalyzerUnitEC*& Token, pas::WideString& Name);

    void EF_Min(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Max(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_NewArray(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_ArrayChange(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Free(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Count(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Copy(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Abs(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_ArcTan(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Exp(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Ln(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Round(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Sin(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Cos(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Sqr(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Sqrt(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Frac(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Int(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Ord(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Rnd(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Randomize(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_RandSeed(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_SubStr(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_FindSubStr(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Trim(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_ToAnsi(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_ToUnicode(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_LowerCase(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_UpperCase(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_LoadLibrary(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_FreeLibrary(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_LibraryFunction(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_New(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void EF_Delete(pas::OpenArray<TVarEC*> av, TCodeEC* code);

    void InitInstr(TExpressionInstrEC* Instruction, TCodeTokenKind Token);

    void FormatScriptError(std::int32_t Code, std::int32_t Position, pas::WideString& Text);

    void FlushTokenRun(TCodeAnalyzerEC*& Analyzer, const pas::WideString& Text, std::int32_t& RunStart, std::int32_t RunLength);

    void FlushQuotedRun(TCodeAnalyzerEC*& Analyzer, const pas::WideString& Text, std::int32_t RunStart, std::int32_t RunLength);

    void EmitSourceToken(TCodeAnalyzerEC*& Analyzer, TCodeAnalyzerUnitEC*& Token, TCodeTokenKind Kind, std::int32_t& Index, std::int32_t& SourceOffset, std::int32_t SourceLength);

    void EmitToken(TCodeAnalyzerEC*& Analyzer, const pas::WideString& Text, std::int32_t& RunStart, std::int32_t RunLength, TCodeAnalyzerUnitEC*& Token, TCodeTokenKind Kind, std::int32_t& Index, std::int32_t& SourceOffset, std::int32_t SourceLength);

    void TVarEC_Create(TVarEC* Self, TVarKind InitialKind);

    void TVarEC_Destroy(TVarEC* Self);

    void TVarArrayEC_Create(TVarArrayEC* Self);

    void TVarArrayEC_Destroy(TVarArrayEC* Self);

    inline void TVarArrayEC_SetNameOrderIndex(TVarArrayEC* Self, std::int32_t Index, std::int32_t DataIndex);

    inline std::int32_t TVarArrayEC_GetNameOrderIndex(TVarArrayEC* Self, std::int32_t Index);

    inline TVarEC* TVarArrayEC_GetItemByNameOrder(TVarArrayEC* Self, std::int32_t Index);

    inline std::int32_t TVarArrayEC_FindNameOrderForDataIndex(TVarArrayEC* Self, std::int32_t DataIndex);

    inline TVarEC* TVarArrayEC_GetItem(TVarArrayEC* Self, std::int32_t Index);

    inline void TVarArrayEC_SetItem(TVarArrayEC* Self, std::int32_t Index, TVarEC* Value);

    inline std::int32_t TVarArrayEC_IndexOf(TVarArrayEC* Self, TVarEC* Value);

    void TCodeAnalyzerEC_Create(TCodeAnalyzerEC* Self);

    void TCodeAnalyzerEC_Destroy(TCodeAnalyzerEC* Self);

    void TCodeAnalyzerEC_AppendText(TCodeAnalyzerEC* Self, pas::WideString Text, std::int32_t SourceOffset, std::int32_t NewlineOffset);

    void TExpressionInstrEC_Destroy(TExpressionInstrEC* Self);

    void TExpressionVarEC_Destroy(TExpressionVarEC* Self);

    void TExpressionEC_Create(TExpressionEC* Self);

    void TExpressionEC_Destroy(TExpressionEC* Self);

    inline TExpressionVarEC* TExpressionEC_GetVariable(TExpressionEC* Self, std::int32_t Index);

    inline void TExpressionEC_SetVariable(TExpressionEC* Self, std::int32_t Index, TExpressionVarEC* Value);

    inline TExpressionInstrEC* TExpressionEC_GetInstruction(TExpressionEC* Self, std::int32_t Index);

    inline void TExpressionEC_SetInstruction(TExpressionEC* Self, std::int32_t Index, TExpressionInstrEC* Value);

    std::uint8_t IsBinaryToken(TCodeAnalyzerUnitEC* Token);

    std::uint8_t IsUnaryMinusPosition(TCompilerUnitEC* Item);

    std::uint8_t InvalidBinaryOperands(TCompilerUnitEC* Item);

    std::uint8_t InvalidAssignmentOperands(TCompilerUnitEC* Item);

    std::uint8_t InvalidUnaryOperand(TCompilerUnitEC* Item);

    void RejectExpression(TCompilerEC*& Compiler, std::int32_t SourceStart, pas::WideString& ErrorText);

    void TExpressionEC_Evaluate(TExpressionEC* Self, TCodeProcessEC* Process, TCodeEC* Code, TScriptDebugState* DebugContext);

    void TCompilerEC_Create(TCompilerEC* Self);

    void TCompilerEC_Destroy(TCompilerEC* Self);

    void TCodeUnitEC_Destroy(TCodeUnitEC* Self);

    void TCodeProcessEC_Create(TCodeProcessEC* Self);

    void TCodeProcessEC_Destroy(TCodeProcessEC* Self);

    void TCodeEC_Create(TCodeEC* Self);

    void TCodeEC_Destroy(TCodeEC* Self);

} // namespace EC_Expression

#include "inline/EC_Expression.hpp"
