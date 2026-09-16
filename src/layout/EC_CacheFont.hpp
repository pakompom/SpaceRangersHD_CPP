#pragma once
#include "types/EC_CacheFont.hpp"

namespace EC_CacheFont {
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheFont::TCFontControlEC) == 24);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheFont::TFontObjectEC) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TFontObjectEC, ObjectId) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TFontObjectEC, Width) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TFontObjectEC, Height) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TFontObjectEC, VerticalMode) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TFontObjectEC, X) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TFontObjectEC, Y) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheFont::TCFontEC) == 92);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, FontData) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, GlyphCount) == 36);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, Glyphs) == 40);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, AboveBaseline) == 44);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, BelowBaseline) == 48);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, MaxGlyphAdvance) == 52);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, GlyphLookup) == 56);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, DefaultColor) == 60);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, UseARGBColors) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, ColorTagsEnabled) == 65);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, ObjectCount) == 68);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, ObjectCapacity) == 72);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, Objects) == 76);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, ColorStackCount) == 80);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, ColorStack) == 84);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TCFontEC, FixedWidthDepth) == 88);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheFont::TAftHeaderEC) == 32);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftHeaderEC, Magic) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftHeaderEC, Version) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftHeaderEC, GlyphCount) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftHeaderEC, CenteringHeight) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftHeaderEC, LineHeight) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheFont::TAftGlyphPlaneEC) == 24);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphPlaneEC, Left) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphPlaneEC, Top) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphPlaneEC, Width) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphPlaneEC, Height) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphPlaneEC, DataOffset) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphPlaneEC, DataSize) == 20);
    static_assert(sizeof(void*) != 4 || sizeof(EC_CacheFont::TAftGlyphEC) == 64);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphEC, CharCode) == 0);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphEC, AdvanceA) == 4);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphEC, AdvanceB) == 8);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphEC, AdvanceC) == 12);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphEC, OpaqueMaskPlane) == 16);
    static_assert(sizeof(void*) != 4 || offsetof(EC_CacheFont::TAftGlyphEC, AlphaMaskPlane) == 40);

} // namespace EC_CacheFont
