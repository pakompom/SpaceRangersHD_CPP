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
        void RemoveAndFreeElement(TStringsElEC* Item);
        TStringsElEC* GetElement(std::int32_t Index);
        TStringsElEC* EnsureElement(std::int32_t Index);
        std::int32_t GetCount();
        pas::WideString GetTextAt(std::int32_t Index);
        void* GetDataAt(std::int32_t Index);
        void SetDataAt(std::int32_t Index, void* Data);
        std::int32_t IndexOf(const pas::WideString& Text);
        void Add(const pas::WideString& Text);
        void AddSlice(char16_t* Text, std::int32_t CharCount);
        void Delete(std::int32_t Index);
        pas::WideString GetCurrentText();
        void* GetCurrentData();
        std::uint8_t IsAtEnd();
        std::uint8_t IsAtLast();
        void First();
        void Next();
        std::uint8_t IsEmpty();
        void SetText(const pas::WideString& Text);
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
