#pragma once
#include "runtime_support.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_Expression {
    struct TCodeExceptionHandler;

    struct ExceptionExpressionEC;

    struct TVarEC;

    struct TVarArrayEC;

    struct TCodeAnalyzerUnitEC;

    struct TCodeAnalyzerEC;

    struct TExpressionInstrEC;

    struct TExpressionVarEC;

    struct TExpressionEC;

    struct TCodeUnitEC;

    struct TCodeProcessEC;

    struct TCodeEC;

    struct TCompilerUnitEC;

    struct TCompilerEC;

    struct TScriptDebugState;

    using PVarEC = TVarEC**;

    enum TVarKind : std::uint8_t {
        vkEmpty = 0,
        vkInt = 1,
        vkDword = 2,
        vkFloat = 3,
        vkString = 4,
        vkExternFun = 5,
        vkLibraryFun = 6,
        vkFunction = 7,
        vkClass = 8,
        vkArray = 9,
        vkRef = 10,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TVarArrayEC : pas::Object {
        PAS_CLASS_META(TVarArrayEC, pas::Object, "TVarArrayEC", 16)
        void p_destroy() override;
        void ClearStorage();
        void Clear();
        void CopyFrom(TVarArrayEC* Source, std::uint8_t CopyArrays);
        std::int32_t FindNameOrderIndex(const pas::WideString& Name);
        std::int32_t FindNameInsertionIndex(const pas::WideString& Name);
        TVarEC* GetItemNE(std::int32_t Index);
        TVarEC* GetVar(const pas::WideString& Name);
        TVarEC* GetVarNE(const pas::WideString& Name);
        void Delete(std::int32_t Index);
        void Remove(TVarEC* Value);
        void DeleteByName(const pas::WideString& Name);
        void AddItem(TVarEC* Value);
        TVarEC* Add(const pas::WideString& Name, TVarKind Kind);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void AppendFromBuffer(EC_Buf::TBufEC* Buffer);
        std::int32_t Count;
        PVarEC Data;
        std::int32_t* NameOrder;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TVarEC : pas::Object {
        PAS_CLASS_META(TVarEC, pas::Object, "TVarEC", 56)
        void p_destroy() override;
        void ConvertToKind(TVarKind NewKind);
        void ResetKind(TVarKind NewKind);
        TVarKind RealVType();
        void AssignFrom(TVarEC* Source, std::uint8_t CopyArrays);
        std::uint8_t IsEmpty();
        std::int32_t GetInt();
        std::uint32_t GetDword();
        double GetFloat();
        pas::WideString GetString();
        void* GetExternFun();
        TCodeEC* GetFunction();
        TCodeEC* GetClass();
        TVarArrayEC* GetArray();
        void SetInt(std::int32_t Value);
        void SetDword(std::uint32_t Value);
        void SetFloat(double Value);
        void SetString(const pas::WideString& Value);
        void SetExternFun(void* Value);
        void SetFunction(TCodeEC* Value);
        void SetClass(TCodeEC* Value);
        void SetArray(TVarArrayEC* Value);
        void SetRef(TVarEC* Value);
        TVarEC* Resolve();
        void PackAnsiString();
        void UnpackAnsiString();
        void CreateArray(pas::OpenArray<std::int32_t> Dimensions);
        void ResizeArray(std::int32_t Count, std::int32_t Dimension);
        void FreeArray();
        void OAdd(TVarEC* Left, TVarEC* Right);
        void OSub(TVarEC* Left, TVarEC* Right);
        void OMul(TVarEC* Left, TVarEC* Right);
        void ODiv(TVarEC* Left, TVarEC* Right);
        void OMod(TVarEC* Left, TVarEC* Right);
        void OBitAnd(TVarEC* Left, TVarEC* Right);
        void OBitOr(TVarEC* Left, TVarEC* Right);
        void OBitXor(TVarEC* Left, TVarEC* Right);
        void OAnd(TVarEC* Left, TVarEC* Right);
        void OOr(TVarEC* Left, TVarEC* Right);
        void OShl(TVarEC* Left, TVarEC* Right);
        void OShr(TVarEC* Left, TVarEC* Right);
        void OEqual(TVarEC* Left, TVarEC* Right);
        void ONotEqual(TVarEC* Left, TVarEC* Right);
        void OLess(TVarEC* Left, TVarEC* Right);
        void OMore(TVarEC* Left, TVarEC* Right);
        void OLessEqual(TVarEC* Left, TVarEC* Right);
        void OMoreEqual(TVarEC* Left, TVarEC* Right);
        void OMinus(TVarEC* Value);
        void OBitNot(TVarEC* Value);
        void ONot(TVarEC* Value);
        void Assume(TVarEC* Source, std::uint8_t CopyArrays);
        std::uint8_t EqualsValue(TVarEC* Other);
        std::uint8_t LessThan(TVarEC* Other);
        std::uint8_t GreaterThan(TVarEC* Other);
        std::uint8_t IsTrue();
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        void SetLibrarySignature(pas::OpenArray<std::uint32_t> Signature);
        pas::WideString Name;
        TVarKind Kind;
        std::uint8_t cpp_padding[3];
        std::int32_t IntValue;
        std::uint32_t DwordValue;
        pas::WideString StringValue;
        double FloatValue;
        void* ExternFunValue;
        pas::DynArray<std::uint32_t> LibraryFunData;
        TCodeEC* FunctionValue;
        TCodeEC* ClassValue;
        TVarArrayEC* ArrayValue;
        TVarEC* RefValue;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using TScriptIncludeResolver = pas::Proc<std::int32_t(void*, const pas::WideString&, std::uint8_t, void*&, TCodeAnalyzerEC*)>;

    using PCodeAnalyzerUnitEC = TCodeAnalyzerUnitEC**;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCodeEC : pas::Object {
        PAS_CLASS_META(TCodeEC, pas::Object, "TCodeEC", 40)
        void p_destroy() override;
        void Clear();
        void CopyFrom(TCodeEC* Source);
        void CopyFromFast(TCodeEC* Source);
        TVarEC* FindVar(pas::WideString Name);
        void DeleteCodeUnit(TCodeUnitEC* CodeUnit);
        TCodeUnitEC* AddCodeUnit();
        TCodeUnitEC* InsertCodeUnitBefore(TCodeUnitEC* BeforeUnit);
        void Compile(TCodeAnalyzerEC* Analyzer, void* SourceContext, TScriptIncludeResolver IncludeResolver, TCodeAnalyzerUnitEC* FirstToken, PCodeAnalyzerUnitEC NextToken, pas::WideString& ErrorText);
        void CompileBlock(TCodeAnalyzerEC* Analyzer, void* SourceContext, TScriptIncludeResolver IncludeResolver, TCodeAnalyzerUnitEC* Token, TCodeUnitEC* BeforeUnit, PCodeAnalyzerUnitEC NextToken, PCodeAnalyzerUnitEC StatementEnd, TCodeUnitEC* BreakTarget, TCodeUnitEC* ContinueTarget, pas::WideString& ErrorText);
        void LinkAll(TVarArrayEC* Scope, std::uint8_t OnlyUnlinked);
        void LinkLocalScopes();
        void Run(TCodeProcessEC* Process);
        void RunDebug(TCodeProcessEC* Process, TScriptDebugState* DebugContext);
        TCodeEC* Parent;
        std::uint8_t IsClassDefinition;
        std::uint8_t cpp_padding[3];
        pas::WideString Name;
        TCodeUnitEC* First;
        TCodeUnitEC* Last;
        TVarArrayEC* LocalVar;
        TCodeProcessEC* Process;
        TScriptDebugState* DebugContext;
        std::uint8_t ScriptFunLinked;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TCodeOpcode : std::uint8_t {
        coLabel = 0,
        coExpression = 1,
        coBranchFalse = 2,
        coJump = 3,
        coExit = 4,
        coPushHandler = 5,
        coPopHandler = 6,
        coThrow = 7,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCodeUnitEC : pas::Object {
        PAS_CLASS_META(TCodeUnitEC, pas::Object, "TCodeUnitEC", 44)
        void p_destroy() override;
        TCodeUnitEC* Prev;
        TCodeUnitEC* Next;
        TCodeOpcode Opcode;
        std::uint8_t cpp_padding[3];
        TExpressionEC* Expression;
        TCodeUnitEC* Target;
        TVarEC* ExceptionVar;
        std::int32_t SourceStart;
        std::int32_t SourceLength;
        void* SourceContext;
        std::uint8_t Breakpoint;
        std::uint8_t cpp_padding_2[3];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TExpressionEC : pas::Object {
        PAS_CLASS_META(TExpressionEC, pas::Object, "TExpressionEC", 28)
        void p_destroy() override;
        void Clear();
        void CopyFrom(TExpressionEC* Source);
        void CopyFromFast(TExpressionEC* Source);
        std::int32_t AddVariable();
        void DeleteVariable(std::int32_t Index);
        std::int32_t AddInstruction();
        void DeleteInstruction(std::int32_t Index);
        void Compile(TCodeAnalyzerEC* Analyzer, TCodeAnalyzerUnitEC* FirstToken, TCodeAnalyzerUnitEC* EndToken, PCodeAnalyzerUnitEC NextToken, pas::WideString& ErrorText);
        void Link(TVarArrayEC* Scope, std::uint8_t OnlyUnlinked);
        TVarEC* GetResult();
        std::int32_t VariableCount;
        TExpressionVarEC** Variables;
        std::int32_t InstructionCount;
        TExpressionInstrEC** Instructions;
        std::uint8_t SharedInstructions;
        std::uint8_t cpp_padding[3];
        std::int32_t ResultIndex;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TExpressionVarKind : std::uint8_t {
        evNamed = 0,
        evOwned = 1,
        evIndexed = 2,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TExpressionVarEC : pas::Object {
        PAS_CLASS_META(TExpressionVarEC, pas::Object, "TExpressionVarEC", 20)
        void p_destroy() override;
        void CopyFrom(TExpressionVarEC* Source);
        std::uint8_t SplitMemberPath();
        pas::WideString GetFullName();
        TVarEC* Resolve(TVarKind InitialKind);
        TExpressionVarKind Kind;
        std::uint8_t cpp_padding[3];
        pas::WideString Name;
        pas::DynArray<pas::WideString> MemberPath;
        TVarEC* Value;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TExpressionOpcode : std::uint8_t {
        eoNegate = 0,
        eoAdd = 1,
        eoSubtract = 2,
        eoMultiply = 3,
        eoDivide = 4,
        eoModulo = 5,
        eoBitAnd = 6,
        eoBitOr = 7,
        eoBitXor = 8,
        eoBitNot = 9,
        eoAnd = 10,
        eoOr = 11,
        eoNot = 12,
        eoShiftLeft = 13,
        eoShiftRight = 14,
        eoLess = 15,
        eoGreater = 16,
        eoEqual = 17,
        eoNotEqual = 18,
        eoLessEqual = 19,
        eoGreaterEqual = 20,
        eoAssign = 21,
        eoCall = 22,
        eoIndex = 23,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TExpressionInstrEC : pas::Object {
        PAS_CLASS_META(TExpressionInstrEC, pas::Object, "TExpressionInstrEC", 16)
        void p_destroy() override;
        void CopyFrom(TExpressionInstrEC* Source);
        TExpressionOpcode Opcode;
        std::uint8_t cpp_padding[3];
        std::int32_t OperandCount;
        pas::DynArray<std::int32_t> Operands;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    using PCodeExceptionHandler = TCodeExceptionHandler*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCodeProcessEC : pas::Object {
        PAS_CLASS_META(TCodeProcessEC, pas::Object, "TCodeProcessEC", 12)
        void p_destroy() override;
        void Clear();
        void PushHandler(TCodeEC* Code, TCodeUnitEC* Handler);
        void PopHandler();
        PCodeExceptionHandler GetHandler();
        void PushException(TVarEC* Value);
        void PopException();
        PVarEC GetException();
        void RaiseUnhandledExceptions();
        pas::List* Handlers;
        pas::List* Exceptions;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TScriptDebugState : pas::Object {
        PAS_CLASS_META(TScriptDebugState, pas::Object, "TScriptDebugState", 28)
        std::uint8_t Paused;
        std::uint8_t cpp_padding[3];
        std::uint32_t StopEvent;
        std::uint32_t ResumeEvent;
        std::uint8_t StepMode;
        std::uint8_t cpp_padding_2[3];
        TCodeUnitEC* CurrentUnit;
        TCodeEC* CurrentCode;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct ExceptionExpressionEC : pas::Exception {
        PAS_CLASS_META(ExceptionExpressionEC, pas::Exception, "ExceptionExpressionEC", 12)
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TCodeExceptionHandler {
        TCodeEC* Code;
        TCodeUnitEC* Handler;
    };
    #pragma pack(pop)

    using TScriptStepCallback = pas::Proc<void(std::int32_t)>;

    enum TLibraryValueKind : std::uint32_t {
        lvVoid = 0,
        lvInt = 1,
        lvDword = 2,
        lvFloat = 3,
        lvString = 4,
        lvRef = 5,
        lvCode = 6,
    };

    using TExpressionCallback = pas::Proc<void(pas::OpenArray<TVarEC*>, TCodeEC*)>;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCodeAnalyzerEC : pas::Object {
        PAS_CLASS_META(TCodeAnalyzerEC, pas::Object, "TCodeAnalyzerEC", 20)
        void p_destroy() override;
        void Clear();
        void ReserveTokens(std::int32_t Count);
        TCodeAnalyzerUnitEC* AcquireToken();
        void RecycleToken(TCodeAnalyzerUnitEC* Token);
        void ClearTokens();
        TCodeAnalyzerUnitEC* AddToken();
        void DeleteToken(TCodeAnalyzerUnitEC* Token);
        void Tokenize(pas::WideString Text, std::int32_t NewlineOffset);
        pas::WideString ValidateDelimiters();
        void RemoveWhitespace();
        void RemoveNewlines();
        void RemoveComments();
        TCodeAnalyzerUnitEC* FirstFree;
        TCodeAnalyzerUnitEC* LastFree;
        TCodeAnalyzerUnitEC* First;
        TCodeAnalyzerUnitEC* Last;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TCodeTokenKind : std::uint8_t {
        ctNewline = 0,
        ctOpenParen = 1,
        ctCloseParen = 2,
        ctOpenBrace = 3,
        ctCloseBrace = 4,
        ctOpenBracket = 5,
        ctCloseBracket = 6,
        ctBlockCommentStart = 7,
        ctBlockCommentEnd = 8,
        ctLineComment = 9,
        ctDot = 10,
        ctArrow = 11,
        ctAdd = 12,
        ctSubtract = 13,
        ctMultiply = 14,
        ctDivide = 15,
        ctModulo = 16,
        ctBitAnd = 17,
        ctBitOr = 18,
        ctBitXor = 19,
        ctBitNot = 20,
        ctAnd = 21,
        ctOr = 22,
        ctNot = 23,
        ctShiftLeft = 24,
        ctShiftRight = 25,
        ctAssign = 26,
        ctEqual = 27,
        ctNotEqual = 28,
        ctLess = 29,
        ctGreater = 30,
        ctLessEqual = 31,
        ctGreaterEqual = 32,
        ctSemicolon = 33,
        ctColon = 34,
        ctComma = 35,
        ctWhitespace = 36,
        ctStringLiteral = 37,
        ctText = 38,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCodeAnalyzerUnitEC : pas::Object {
        PAS_CLASS_META(TCodeAnalyzerUnitEC, pas::Object, "TCodeAnalyzerUnitEC", 28)
        TCodeAnalyzerUnitEC* Prev;
        TCodeAnalyzerUnitEC* Next;
        TCodeTokenKind TokenKind;
        std::uint8_t cpp_padding[3];
        std::int32_t SourceStart;
        std::int32_t SourceLength;
        pas::WideString Text;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCompilerEC : pas::Object {
        PAS_CLASS_META(TCompilerEC, pas::Object, "TCompilerEC", 12)
        void p_destroy() override;
        void Clear();
        TCompilerUnitEC* AddUnit();
        void DeleteUnit(TCompilerUnitEC* UnitNode);
        TCompilerUnitEC* FindReducibleOperator();
        TCompilerUnitEC* FindReducibleIndex();
        TCompilerUnitEC* FindReducibleCall();
        TCompilerUnitEC* First;
        TCompilerUnitEC* Last;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    enum TCompilerUnitKind : std::uint8_t {
        cuIntLiteral = 0,
        cuDwordLiteral = 1,
        cuFloatLiteral = 2,
        cuStringLiteral = 3,
        cuBinaryOperator = 4,
        cuUnaryOperator = 5,
        cuOpenParen = 6,
        cuCloseParen = 7,
        cuOpenBracket = 8,
        cuCloseBracket = 9,
        cuName = 10,
        cuCall = 11,
        cuIndex = 12,
        cuVariable = 13,
        cuComma = 14,
        cuAssignment = 15,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCompilerUnitEC : pas::Object {
        PAS_CLASS_META(TCompilerUnitEC, pas::Object, "TCompilerUnitEC", 48)
        TCompilerUnitEC* Prev;
        TCompilerUnitEC* Next;
        TCompilerUnitKind Kind;
        TCodeTokenKind OperatorToken;
        std::uint8_t cpp_padding[2];
        pas::WideString Text;
        std::int32_t VariableIndex;
        std::int32_t IntValue;
        std::uint32_t DwordValue;
        double FloatValue;
        std::int32_t SourceStart;
        std::int32_t SourceLength;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace EC_Expression
