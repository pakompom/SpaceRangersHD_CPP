#pragma once
#include "runtime_support.hpp"

namespace EC_Str {
    struct TWideCasePair;

    struct TStringsElEC;

    struct TStringsEC;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStringsEC : pas::Object {
        PAS_CLASS_META(TStringsEC, pas::Object, "TStringsEC", 16)
        void p_destroy() override;
        void Clear();
        TStringsElEC* AddEmptyElement();
        void AppendElement(TStringsElEC* Item);
        // Does not adjust CurrentElement.
        void RemoveAndFreeElement(TStringsElEC* Item);
        // Raises when the index is outside the list.
        TStringsElEC* GetElement(std::int32_t Index);
        // Creates missing entries; negative indexes raise.
        TStringsElEC* EnsureElement(std::int32_t Index);
        std::int32_t GetCount();
        // Reading beyond the end extends the list.
        pas::WideString GetTextAt(std::int32_t Index);
        // Reading beyond the end extends the list.
        void* GetDataAt(std::int32_t Index);
        // Creates missing entries; Data is borrowed.
        void SetDataAt(std::int32_t Index, void* Data);
        // Case-sensitive comparison; returns -1 when absent.
        std::int32_t IndexOf(const pas::WideString& Text);
        void Add(const pas::WideString& Text);
        // Nonpositive CharCount still appends an empty element.
        void AddSlice(char16_t* Text, std::int32_t CharCount);
        // If deleting CurrentElement, moves it to the next element or otherwise the previous one.
        void Delete(std::int32_t Index);
        // Raises when CurrentElement is nil.
        pas::WideString GetCurrentText();
        // Raises when CurrentElement is nil.
        void* GetCurrentData();
        std::uint8_t IsAtEnd();
        // Requires nonnil CurrentElement.
        std::uint8_t IsAtLast();
        void First();
        // Requires nonnil CurrentElement.
        void Next();
        std::uint8_t IsEmpty();
        // Splits CR, LF and CRLF lines; does not append an empty line after a trailing separator.
        void SetText(const pas::WideString& Text);
        // Joins elements with CRLF, without a trailing separator.
        pas::WideString GetText();
        TStringsElEC* FirstElement;
        TStringsElEC* LastElement;
        TStringsElEC* CurrentElement;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TStringsElEC : pas::Object {
        PAS_CLASS_META(TStringsElEC, pas::Object, "TStringsElEC", 20)
        TStringsElEC* Prev;
        TStringsElEC* Next;
        pas::WideString Text;
        void* Data;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TWideCasePair {
        char16_t LowerChar;
        char16_t UpperChar;
    };
    #pragma pack(pop)

    using THexDigits = pas::Array<char16_t, 0, 15>;

} // namespace EC_Str
