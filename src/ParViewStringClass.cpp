#include "layout/ParViewStringClass.hpp"
#include "types/TextFieldClass.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Struct.hpp"
#include "units/ParViewStringClass.hpp"

namespace ParViewStringClass {
    void TParViewString_Create(TParViewString* Self, pas::WideString Value) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Text = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Text->Text = std::move(Value);
    }

    void TParViewString_Destroy(TParViewString* Self) {
        pas::free(Self->Text);
        Self->Text = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TParViewString::LoadFromReader(EC_Buf::TBufEC* Reader) {
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        Text->LoadTextLinesFromReader(Reader);
    }

    void TParViewString::p_destroy() {
        ParViewStringClass::TParViewString_Destroy(this);
    }

} // namespace ParViewStringClass
