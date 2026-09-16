#pragma once
#include "units/EC_CacheFont.hpp"

namespace EC_CacheFont {
    inline std::int32_t TCFontEC_GetGlyphAdvance(TCFontEC* Self, char16_t CharCode) {
        std::int32_t Result = pas::load_unaligned<std::uint16_t>(pas::byte_offset(Self->GlyphLookup, CharCode * sizeof(std::uint16_t)));
        if (Result != 0) {
            return (Self->Glyphs + (Result - 1))->AdvanceA + (Self->Glyphs + (Result - 1))->AdvanceB + (Self->Glyphs + (Result - 1))->AdvanceC;
        }
        return Result;
    }

} // namespace EC_CacheFont
