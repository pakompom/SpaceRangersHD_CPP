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
        // Does not free cells; use Clear for owned entries.
        void ClearStorage();
        void Clear();
        void CopyFrom(TVarArrayEC* Source, std::uint8_t CopyArrays);
        // Returns -1 when absent.
        std::int32_t FindNameOrderIndex(const pas::WideString& Name);
        std::int32_t FindNameInsertionIndex(const pas::WideString& Name);
        // Returns nil for an out-of-range index.
        TVarEC* GetItemNE(std::int32_t Index);
        TVarEC* GetVar(const pas::WideString& Name);
        // Returns nil when absent.
        TVarEC* GetVarNE(const pas::WideString& Name);
        // Frees the cell; ignores invalid indexes.
        void Delete(std::int32_t Index);
        void Remove(TVarEC* Value);
        void DeleteByName(const pas::WideString& Name);
        // Takes ownership of Value.
        void AddItem(TVarEC* Value);
        TVarEC* Add(const pas::WideString& Name, TVarKind Kind);
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        // Clears existing cells before reading.
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
        // Preserves the value where conversion is supported; ResetKind discards it.
        void ConvertToKind(TVarKind NewKind);
        void ResetKind(TVarKind NewKind);
        void AssignFrom(TVarEC* Source, std::uint8_t CopyArrays);
        // Tests this cell's tag without dereferencing.
        std::uint8_t IsEmpty();
        std::int32_t GetInt();
        // Reference cells delegate to GetInt, then reinterpret its bits.
        std::uint32_t GetDword();
        double GetFloat();
        // Library cells return their import specification string.
        pas::WideString GetString();
        void* GetExternFun();
        TCodeEC* GetFunction();
        // Reference cells delegate to GetFunction in the native code.
        TCodeEC* GetClass();
        TVarArrayEC* GetArray();
        void SetInt(std::int32_t Value);
        // Reference cells delegate to SetInt with the same bits.
        void SetDword(std::uint32_t Value);
        void SetFloat(double Value);
        // Assigns through references and converts to an existing destination kind; an empty cell becomes a string.
        void SetString(const pas::WideString& Value);
        void SetExternFun(void* Value);
        // Native leaves empty and function cells unchanged; other kinds clear their payload or delegate through a reference.
        void SetFunction(TCodeEC* Value);
        // Value is borrowed; vkRef assignment uses the function-value setter.
        void SetClass(TCodeEC* Value);
        // Value is borrowed; follows references.
        void SetArray(TVarArrayEC* Value);
        void SetRef(TVarEC* Value);
        // Stores ANSI bytes inside StringValue's UTF-16 allocation.
        void PackAnsiString();
        // Non-string cells are converted to string without unpacking.
        void UnpackAnsiString();
        // Requires at least one dimension.
        void CreateArray(pas::OpenArray<std::int32_t> Dimensions);
        // Assigns through references, converting to the destination kind.
        void Assume(TVarEC* Source, std::uint8_t CopyArrays);
        // Only scalar, string and array kinds have serialized payloads.
        void SaveToBuffer(EC_Buf::TBufEC* Buffer);
        void LoadFromBuffer(EC_Buf::TBufEC* Buffer);
        // Does not change Kind.
        void SetLibrarySignature(pas::OpenArray<std::uint32_t> Signature);
        pas::WideString Name;
        TVarKind Kind;
        std::uint8_t cpp_padding[3];
        std::int32_t IntValue;
        std::uint32_t DwordValue;
        pas::WideString StringValue;
        double FloatValue;
        void* ExternFunValue;
        // Delphi dynamic array: TLibraryValueKind return kind, native address,
        // then TLibraryValueKind argument kinds. Address word is not an enum.
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
        // Expression instructions remain shared with Source.
        void CopyFromFast(TCodeEC* Source);
        TVarEC* FindVar(pas::WideString Name);
        void DeleteCodeUnit(TCodeUnitEC* CodeUnit);
        TCodeUnitEC* AddCodeUnit();
        // Inserts before BeforeUnit; nil appends.
        TCodeUnitEC* InsertCodeUnitBefore(TCodeUnitEC* BeforeUnit);
        // NextToken may be nil.
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
        // Compiler-supplied source/debug identity.
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
        // Borrows Source's instruction array.
        void CopyFromFast(TExpressionEC* Source);
        // Returns a zero-based index; the new slot starts with zero-initialized evNamed kind.
        std::int32_t AddVariable();
        void DeleteVariable(std::int32_t Index);
        std::int32_t AddInstruction();
        void DeleteInstruction(std::int32_t Index);
        // EndToken is exclusive; nil FirstToken starts at Analyzer.First. NextToken may be nil. Clears the previous expression before compiling.
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
        // Replaces Name with its root component. Always returns true.
        std::uint8_t SplitMemberPath();
        pas::WideString GetFullName();
        // Only evOwned slots allocate values.
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
        // Indices into TExpressionEC.Variables: destination first, then sources.
        // eoCall uses destination, callee, arguments; eoIndex uses destination, array, indices.
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
        // Does not free the exception value; the caller assumes ownership.
        void PopException();
        PVarEC GetException();
        void RaiseUnhandledExceptions();
        pas::List* Handlers;
        pas::List* Exceptions;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    // Class form is inferred from the first field at +4 and the native anonymous
    // type counter after the public class declarations; no retained VMT is known.
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

    // DLL signature words use a separate numbering from TVarKind.
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
        // Also frees pooled nodes.
        void Clear();
        void ReserveTokens(std::int32_t Count);
        TCodeAnalyzerUnitEC* AcquireToken();
        void RecycleToken(TCodeAnalyzerUnitEC* Token);
        // Retains token storage for reuse.
        void ClearTokens();
        TCodeAnalyzerUnitEC* AddToken();
        void DeleteToken(TCodeAnalyzerUnitEC* Token);
        // Replaces existing tokens; source offsets start at zero.
        void Tokenize(pas::WideString Text, std::int32_t NewlineOffset);
        // Returns an empty string on success.
        pas::WideString ValidateDelimiters();
        void RemoveWhitespace();
        void RemoveNewlines();
        // Supports nested block comments.
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
        // Returns nil when no operator qualifies.
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
