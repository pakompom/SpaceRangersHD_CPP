#pragma once
#include "types/EC_CacheFont.hpp"
#include "types/Windows_group.hpp"

namespace EC_Cache {
    struct TCacheControlEC;

} // namespace EC_Cache

namespace Types {
    struct TRect;

} // namespace Types

namespace EC_CacheFont {
    // Also clears the shared font's color stack, disables ARGB colors and enables color tags.
    TCFontEC* AcquireCachedFont(EC_Cache::TCacheControlEC* Control);

    void IncludeGlyphBounds(WindowsSdk::TRect& Bounds, std::int32_t X, std::int32_t Y, PAftGlyphEC Glyph);

    // ColorStack is a separately allocated buffer.
    void TCFontEC_Create(TCFontEC* Self);

    void TCFontEC_Destroy(TCFontEC* Self);

    // The native implementation is handwritten assembly.
    // Returns zero for an absent character.
    std::int32_t TCFontEC_GetGlyphAdvance(TCFontEC* Self, char16_t CharCode);

} // namespace EC_CacheFont
