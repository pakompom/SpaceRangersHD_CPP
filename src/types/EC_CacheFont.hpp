#pragma once
#include "runtime_support.hpp"
#include "types/Direct3D9.hpp"
#include "types/EC_Cache.hpp"
#include "types/System.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"

namespace EC_Buf {
    struct TBufEC;

} // namespace EC_Buf

namespace EC_CacheFont {
    struct TAftGlyphEC;

    struct TAftHeaderEC;

} // namespace EC_CacheFont

namespace EC_Str {
    struct TStringsEC;

} // namespace EC_Str

namespace EC_CacheFont {
    struct TAftGlyphPlaneEC;

    struct TFontObjectEC;

    struct TCFontControlEC;

    struct TCFontEC;

    using PFontObjectEC = TFontObjectEC*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCFontControlEC : EC_Cache::TCacheControlEC {
        PAS_CLASS_META(TCFontControlEC, EC_Cache::TCacheControlEC, "TCFontControlEC", 24)
        void QueueLoadIfMissing(pas::List* PendingLoads) override;
        EC_Cache::TCacheDataEC* CreateData() override;
        EC_Cache::TCacheDataEC* AcquireData() override;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TFontObjectEC {
        std::int32_t ObjectId;
        std::int32_t Width;
        std::int32_t Height;
        std::int32_t VerticalMode;
        std::int32_t X;
        std::int32_t Y;
    };
    #pragma pack(pop)

    using PAftHeaderEC = TAftHeaderEC*;

    using PAftGlyphEC = TAftGlyphEC*;

    using TFontGlyphLookupEC = pas::Array<std::uint16_t, 0, 65535>;

    using PFontGlyphLookupEC = TFontGlyphLookupEC*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TCFontEC : EC_Cache::TCacheDataEC {
        PAS_CLASS_META(TCFontEC, EC_Cache::TCacheDataEC, "TCFontEC", 92)
        void p_destroy() override;
        void ClearLoadedFontData();
        std::int32_t GetCenteringHeight();
        // Includes two extra pixels beyond the stored line height.
        std::int32_t GetLineHeight();
        // Preserves allocated storage.
        void ResetTextMeasureState();
        PFontObjectEC GetEmbeddedObject(std::int32_t Index);
        // Restores ObjectCount; FixedWidthDepth remains affected by the processed tags.
        WindowsSdk::TRect MeasureTaggedTextBounds(const pas::WideString& Text, std::int32_t X, std::int32_t Y, WindowsSdk::PInteger TopAdjustment);
        std::uint8_t HasGlyph(char16_t CharCode);
        // Replaces Lines, preserves tags in its output, and restores ObjectCount.
        void WrapTaggedTextIntoLines(EC_Str::TStringsEC* Lines, const pas::WideString& Text, std::int32_t MaxWidth);
        void DrawTaggedText16(void* Destination, std::int32_t PitchBytes, std::int32_t X, std::int32_t Y, const pas::WideString& Text, WindowsSdk::TRect ClipRect);
        void DrawTaggedText32(void* Destination, std::int32_t PitchBytes, std::int32_t X, std::int32_t Y, const pas::WideString& Text, WindowsSdk::TRect ClipRect);
        // Returns zero for incomplete tokens or a doubled opening bracket.
        static std::int32_t GetTaggedTextTokenLength(char16_t* Text, std::int32_t CharCount);
        // For td=n, raises X to at least n and returns the token length.
        static std::int32_t ParseTabTagAndAdjustX(char16_t* Text, std::int32_t CharCount, std::int32_t& X);
        // Handles align=right/center and restores ObjectCount. Uppercase value checks use incorrect source positions in the native code.
        std::int32_t ParseAlignTagAndAdjustX(char16_t* Text, std::int32_t CharCount, std::int32_t& X);
        static std::int32_t MatchAlignEndTag(char16_t* Text, std::int32_t CharCount);
        static std::int32_t MatchFixTag(char16_t* Text, std::int32_t CharCount);
        static std::int32_t MatchFixEndTag(char16_t* Text, std::int32_t CharCount);
        // Returns the token length; Alignment is -1 for left, 0 for center, 1 for right. FieldWidth counts characters.
        static std::int32_t ParseFormatTag(char16_t* Text, std::int32_t CharCount, std::int32_t& FieldWidth, std::int32_t& Alignment);
        static std::int32_t MatchFormatEndTag(char16_t* Text, std::int32_t CharCount);
        // Only characters present in GlyphLookup count.
        std::int32_t CountVisibleTaggedCharsUntilFormatEnd(char16_t* Text, std::int32_t CharCount);
        // Native capacity check is reversed: it reallocates when ObjectCount <= ObjectCapacity.
        std::int32_t ParseObjectTagCached(char16_t* Text, std::int32_t CharCount, std::int32_t& ObjectIndex);
        // Parses object=id,width,height,verticalMode; leaves X and Y unchanged.
        static std::int32_t ParseObjectTag(char16_t* Text, std::int32_t CharCount, TFontObjectEC& Item);
        // Parses color=r,g,b; emits ARGB or the current packed pixel format according to UseARGBColors.
        std::int32_t ParseColorTag(char16_t* Text, std::int32_t CharCount, std::uint32_t& Color);
        static std::int32_t MatchColorEndTag(char16_t* Text, std::int32_t CharCount);
        // Pushes or pops a color only while ColorTagsEnabled is true.
        void ApplyColorTag(char16_t* Text, std::int32_t CharCount);
        void ClearColorStack();
        void PushColor(std::uint32_t Color);
        // Returns zero when empty.
        std::uint32_t PopColor();
        // Returns DefaultColor when tags are disabled or the stack is empty.
        std::uint32_t GetCurrentColor();
        // Requires aft version 1 and at least 0x20 bytes; glyph offsets and counts are trusted. Ignores LoadOption; ResidentBytes remains zero.
        void LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) override;
        // Zero dimensions use measured text size. ActualSize may be nil. Enables UseARGBColors on the shared font.
        void RenderTaggedTextToTexture(const pas::WideString& Text, std::int32_t Width, std::int32_t Height, std::int32_t AlignX, std::int32_t AlignY, std::uint8_t WordWrap, WindowsSdk::PPoint ActualSize, Direct3D9::IDirect3DTexture9& Texture);
        PAftHeaderEC FontData;
        std::int32_t GlyphCount;
        PAftGlyphEC Glyphs;
        std::int32_t AboveBaseline;
        std::int32_t BelowBaseline;
        std::int32_t MaxGlyphAdvance;
        PFontGlyphLookupEC GlyphLookup;
        std::uint32_t DefaultColor;
        std::uint8_t UseARGBColors;
        std::uint8_t ColorTagsEnabled;
        std::uint8_t cpp_padding[2];
        std::int32_t ObjectCount;
        std::int32_t ObjectCapacity;
        pas::DynArray<TFontObjectEC> Objects;
        std::int32_t ColorStackCount;
        System::PCardinal ColorStack;
        std::int32_t FixedWidthDepth;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TAftHeaderEC {
        pas::Array<std::uint8_t, 0, 3> Magic;
        std::int32_t Version;
        std::int32_t GlyphCount;
        std::int32_t CenteringHeight;
        std::uint8_t cpp_padding[4];
        std::int32_t LineHeight;
        std::uint8_t cpp_padding_2[8];
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TAftGlyphPlaneEC {
        std::int32_t Left;
        std::int32_t Top;
        std::int32_t Width;
        std::int32_t Height;
        std::int32_t DataOffset;
        // Encoded buffer size including its 16-byte header.
        std::int32_t DataSize;
    };
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct TAftGlyphEC {
        std::uint32_t CharCode;
        std::int32_t AdvanceA;
        std::int32_t AdvanceB;
        std::int32_t AdvanceC;
        TAftGlyphPlaneEC OpaqueMaskPlane;
        TAftGlyphPlaneEC AlphaMaskPlane;
    };
    #pragma pack(pop)

    using TFontTextCharsEC = pas::Array<char16_t, 0, 1073741822>;

    using PFontTextCharsEC = TFontTextCharsEC*;

} // namespace EC_CacheFont
