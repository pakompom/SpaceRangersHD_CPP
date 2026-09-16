#include "layout/EventClass.hpp"
#include "types/TextFieldClass.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EventClass.hpp"

namespace EventClass {
    void TEvent_Create(TEvent* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Text = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Text->ClearText();
        Self->Picture = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Picture->ClearText();
        Self->Music = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Music->ClearText();
        Self->Sound = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Sound->ClearText();
    }

    void TEvent_Destroy(TEvent* Self) {
        pas::free(Self->Text);
        Self->Text = nullptr;
        pas::free(Self->Picture);
        Self->Picture = nullptr;
        pas::free(Self->Music);
        Self->Music = nullptr;
        pas::free(Self->Sound);
        Self->Sound = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TEvent::ClearTextFields() {
        Text->Text = pas::WideString();
        Picture->Text = pas::WideString();
        Music->Text = pas::WideString();
        Sound->Text = pas::WideString();
    }

    void TEvent::Assign(TEvent* Source) {
        Text->Text = EC_Str::TrimWideString(Source->Text->Text);
        Picture->Text = EC_Str::TrimWideString(Source->Picture->Text);
        Music->Text = EC_Str::TrimWideString(Source->Music->Text);
        Sound->Text = EC_Str::TrimWideString(Source->Sound->Text);
    }

    void TEvent::p_destroy() {
        EventClass::TEvent_Destroy(this);
    }

} // namespace EventClass
