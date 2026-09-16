#include "layout/TextFieldClass.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/TextFieldClass.hpp"

namespace TextFieldClass {
    void TTextField::ClearText() {
        Text = pas::WideString();
    }

    void TTextField::LoadTextLinesFromReader(EC_Buf::TBufEC* Reader) {
        pas::WideString Line{};
        std::int32_t i{};
        std::int32_t j{};
        std::int32_t CharCount{};
        ClearText();
        std::int32_t LineCount = EC_Buf::TBufEC_GetInt32(Reader);
        for (auto cpp_range = pas::for_to<std::int32_t>(1, LineCount); cpp_range.next(j); ) {
            CharCount = EC_Buf::TBufEC_GetInt32(Reader);
            Line.set_length(CharCount);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, CharCount); cpp_range_2.next(i); ) {
                Line.write(i) = Reader->GetWideChar();
            }
            if (Text != u"") {
                Text = pas::concat_wide({Text, u"\r\n", EC_Str::TrimWideString(Line)});
            } else {
                Text = EC_Str::TrimWideString(Line);
            }
        }
        Text = EC_Str::TrimWideString(Text);
    }

} // namespace TextFieldClass
