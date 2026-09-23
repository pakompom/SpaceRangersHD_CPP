#include "layout/EC_CacheFont.hpp"
#include "types/EC_Buf.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "units/ClassesImports.hpp"
#include "units/Direct3D9.hpp"
#include "units/EC_Cache.hpp"
#include "units/EC_CacheFont.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Str.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_Main.hpp"
#include "units/System.hpp"
#include "units/Windows.hpp"
#include "units/WindowsSdk.hpp"

namespace EC_CacheFont {
    // Also clears the shared font's color stack, disables ARGB colors and enables color tags.
    TCFontEC* AcquireCachedFont(EC_Cache::TCacheControlEC* Control) {
        TCFontEC* Result = pas::checked_cast<TCFontEC*>(Control->AcquireDataFromConfig(pas::class_ref<TCFontEC>()));
        Result->ClearColorStack();
        Result->UseARGBColors = false;
        Result->ColorTagsEnabled = true;
        return Result;
    }

    void IncludeGlyphBounds(WindowsSdk::TRect& Bounds, std::int32_t X, std::int32_t Y, PAftGlyphEC Glyph) {
        if (Glyph->AlphaMaskPlane.DataOffset != 0) {
            if (X + Glyph->AlphaMaskPlane.Left < Bounds.Left) {
                Bounds.Left = X + Glyph->AlphaMaskPlane.Left;
            }
            if (Y + Glyph->AlphaMaskPlane.Top < Bounds.Top) {
                Bounds.Top = Y + Glyph->AlphaMaskPlane.Top;
            }
            if (X + Glyph->AlphaMaskPlane.Left + Glyph->AlphaMaskPlane.Width > Bounds.Right) {
                Bounds.Right = X + Glyph->AlphaMaskPlane.Left + Glyph->AlphaMaskPlane.Width;
            }
            if (Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height > Bounds.Bottom) {
                Bounds.Bottom = Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height;
            }
        }
        if (Glyph->OpaqueMaskPlane.DataOffset != 0) {
            if (X + Glyph->OpaqueMaskPlane.Left < Bounds.Left) {
                Bounds.Left = X + Glyph->OpaqueMaskPlane.Left;
            }
            if (Y + Glyph->OpaqueMaskPlane.Top < Bounds.Top) {
                Bounds.Top = Y + Glyph->OpaqueMaskPlane.Top;
            }
            if (X + Glyph->OpaqueMaskPlane.Left + Glyph->OpaqueMaskPlane.Width > Bounds.Right) {
                Bounds.Right = X + Glyph->OpaqueMaskPlane.Left + Glyph->OpaqueMaskPlane.Width;
            }
            if (Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height > Bounds.Bottom) {
                Bounds.Bottom = Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height;
            }
        }
    }

    void TCFontControlEC::QueueLoadIfMissing(pas::List* PendingLoads) {
        TCFontControlEC* Control{};
        if (RetainCount > 0) {
            return;
        }
        if (BoundData != nullptr) {
            return;
        }
        if (HasEmptyCacheKey()) {
            return;
        }
        if (GR_Main::GlobalCache->FindDataByKeyAndClass(pas::view(CacheKey), pas::class_ref<TCFontEC>()) == nullptr) {
            Control = pas::construct_call<TCFontControlEC>(EC_Cache::TCacheControlEC_Create);
            EC_Cache::TCacheEC::ResetControl(Control);
            Control->SetCacheKey(CacheKey);
            pas::list_add(PendingLoads, reinterpret_cast<void*>(Control));
        }
    }

    EC_Cache::TCacheDataEC* TCFontControlEC::CreateData() {
        return pas::construct_call<TCFontEC>(TCFontEC_Create);
    }

    EC_Cache::TCacheDataEC* TCFontControlEC::AcquireData() {
        return EC_CacheFont::AcquireCachedFont(this);
    }

    // ColorStack is a separately allocated buffer.
    void TCFontEC_Create(TCFontEC* Self) {
        EC_Cache::TCacheDataEC_Create(Self);
        Self->ColorTagsEnabled = true;
    }

    void TCFontEC_Destroy(TCFontEC* Self) {
        Self->ClearLoadedFontData();
        EC_Cache::TCacheDataEC_Destroy(Self);
    }

    void TCFontEC::ClearLoadedFontData() {
        ObjectCount = 0;
        ObjectCapacity = 0;
        Objects = nullptr;
        if (FontData != nullptr) {
            EC_Mem::FreeEC(FontData);
            FontData = nullptr;
        }
        Glyphs = nullptr;
        GlyphCount = 0;
        if (GlyphLookup != nullptr) {
            EC_Mem::FreeEC(GlyphLookup);
            GlyphLookup = nullptr;
        }
        ClearColorStack();
    }

    std::int32_t TCFontEC::GetCenteringHeight() {
        return FontData->CenteringHeight;
    }

    // Includes two extra pixels beyond the stored line height.
    std::int32_t TCFontEC::GetLineHeight() {
        return FontData->LineHeight + 2;
    }

    // Preserves allocated storage.
    void TCFontEC::ResetTextMeasureState() {
        ObjectCount = 0;
        FixedWidthDepth = 0;
    }

    PFontObjectEC TCFontEC::GetEmbeddedObject(std::int32_t Index) {
        return &Objects[Index];
    }

    // Restores ObjectCount; FixedWidthDepth remains affected by the processed tags.
    WindowsSdk::TRect TCFontEC::MeasureTaggedTextBounds(const pas::WideString& Text, std::int32_t X, std::int32_t Y, WindowsSdk::PInteger TopAdjustment) {
        WindowsSdk::TRect Result{};
        std::int32_t GlyphIndex{};
        std::int32_t TokenLength{};
        std::int32_t MiddleY{};
        char16_t Ch{};
        PAftGlyphEC Glyph{};
        std::int32_t ObjectIndex{};
        std::int32_t FieldWidth{};
        std::int32_t Alignment{};
        std::int32_t SavedObjectCount = ObjectCount;
        std::int32_t CharCount = Text.length();
        std::int32_t PosX = X;
        Result.Left = 999999999;
        Result.Right = -999999999;
        Result.Top = 999999999;
        Result.Bottom = -999999999;
        std::int32_t Index = 0;
        while (Index < CharCount) {
            Ch = Text.read(Index + 1);
            ++Index;
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, CharCount - Index + 1);
            if (TokenLength > 0) {
                if (ParseObjectTagCached(Text.pchar() + Index - 1, CharCount - Index + 1, ObjectIndex) > 0) {
                    PosX += Objects[ObjectIndex].Width;
                } else if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    ++FixedWidthDepth;
                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    FixedWidthDepth = std::max<std::int32_t>(FixedWidthDepth - 1, 0);
                } else if (TCFontEC::ParseFormatTag(Text.pchar() + Index - 1, CharCount - Index + 1, FieldWidth, Alignment) > 0) {
                    Index = Index + TokenLength - 1;
                    while (Index < CharCount) {
                        Ch = Text.read(Index + 1);
                        ++Index;
                        TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, CharCount - Index + 1);
                        if (TokenLength > 0) {
                            if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                                ++FixedWidthDepth;
                            } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                                FixedWidthDepth = std::max<std::int32_t>(FixedWidthDepth - 1, 0);
                            } else if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                                Index = Index + TokenLength - 1;
                                break;
                            }
                            Index = Index + TokenLength - 1;
                            continue;
                        }
                        GlyphIndex = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, Ch * 2));
                        if (GlyphIndex != 0) {
                            Glyph = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(Glyphs, (GlyphIndex - 1) * static_cast<std::int32_t>(sizeof(TAftGlyphEC))));
                            EC_CacheFont::IncludeGlyphBounds(Result, PosX, Y, Glyph);
                            if (FixedWidthDepth > 0) {
                                PosX += MaxGlyphAdvance;
                            } else {
                                PosX = PosX + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
                            }
                            --FieldWidth;
                        }
                    }
                    if (FieldWidth > 0) {
                        GlyphIndex = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, ' ' * 2));
                        if (GlyphIndex != 0) {
                            Glyph = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(Glyphs, (GlyphIndex - 1) * static_cast<std::int32_t>(sizeof(TAftGlyphEC))));
                            if (FixedWidthDepth > 0) {
                                PosX += MaxGlyphAdvance * FieldWidth;
                            } else {
                                PosX += (Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC) * FieldWidth;
                            }
                        }
                    }
                    continue;
                }
                Index = Index + TokenLength - 1;
                continue;
            }
            GlyphIndex = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, Ch * 2));
            if (GlyphIndex == 0) {
                continue;
            }
            Glyph = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(Glyphs, (GlyphIndex - 1) * static_cast<std::int32_t>(sizeof(TAftGlyphEC))));
            EC_CacheFont::IncludeGlyphBounds(Result, PosX, Y, Glyph);
            if (FixedWidthDepth > 0) {
                PosX += MaxGlyphAdvance;
            } else {
                PosX = PosX + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
            }
        }
        Result.Right = std::max<std::int32_t>(Result.Right, PosX);
        if (TopAdjustment != nullptr) {
            pas::store_unaligned<std::int32_t>(TopAdjustment, 0);
        }
        if (ObjectCount - SavedObjectCount > 0) {
            MiddleY = (Result.Top + Result.Bottom) / 2;
            for (auto cpp_range = pas::for_to<std::int32_t>(SavedObjectCount, ObjectCount - 1); cpp_range.next(Index); ) {
                if (Objects[Index].VerticalMode == 0) {
                    if (TopAdjustment != nullptr && MiddleY - Objects[Index].Height / 2 < Result.Top) {
                        pas::store_unaligned<std::int32_t>(TopAdjustment, pas::load_unaligned<std::int32_t>(TopAdjustment) + Result.Top - (MiddleY - Objects[Index].Height / 2));
                    }
                    Result.Top = std::min<std::int32_t>(Result.Top, MiddleY - Objects[Index].Height / 2);
                    Result.Bottom = std::max<std::int32_t>(Result.Bottom, MiddleY - Objects[Index].Height / 2 + Objects[Index].Height);
                }
            }
        }
        ObjectCount = SavedObjectCount;
        return Result;
    }

    std::uint8_t TCFontEC::HasGlyph(char16_t CharCode) {
        return EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, CharCode * 2)) > 0;
    }

    // Replaces Lines, preserves tags in its output, and restores ObjectCount.
    void TCFontEC::WrapTaggedTextIntoLines(EC_Str::TStringsEC* Lines, const pas::WideString& Text, std::int32_t MaxWidth) {
        std::int32_t TokenLength{};
        std::int32_t Index{};
        std::int32_t WordLength{};
        std::int32_t FitEnd{};
        std::int32_t WordWidth{};
        std::uint8_t SeenCharacter{};
        std::int32_t ObjectIndex{};
        std::int32_t FieldWidth{};
        std::int32_t Alignment{};
        char16_t Ch{};
        std::int32_t SavedObjectCount = ObjectCount;
        Lines->Clear();
        std::int32_t LineWidth = 0;
        std::int32_t WordStart = 0;
        std::int32_t LineStart = 0;
        std::int32_t LineLength = 0;
        std::int32_t CharCount = Text.length();
        std::uint8_t FirstLine = true;
        while (WordStart < CharCount) {
            SeenCharacter = false;
            for (auto cpp_range = pas::for_to<std::int32_t>(WordStart, CharCount - 1); cpp_range.next(Index); ) {
                if (pas::load_unaligned<char16_t>(pas::byte_offset(static_cast<PFontTextCharsEC>(static_cast<void*>(Text.data())), Index * sizeof(char16_t))) == u' ') {
                    if (SeenCharacter) {
                        break;
                    }
                }
                SeenCharacter = true;
            }
            WordLength = Index - WordStart;
            WordWidth = 0;
            Index = WordStart;
            FitEnd = 0;
            while (Index <= WordStart + WordLength - 1) {
                Ch = pas::load_unaligned<char16_t>(pas::byte_offset(static_cast<PFontTextCharsEC>(static_cast<void*>(Text.data())), Index * sizeof(char16_t)));
                ++Index;
                TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, WordLength - (Index - 1 - WordStart));
                if (TokenLength > 0) {
                    if (ParseObjectTagCached(Text.pchar() + Index - 1, WordLength - (Index - 1 - WordStart), ObjectIndex) > 0) {
                        WordWidth += Objects[ObjectIndex].Width;
                        if (WordWidth <= MaxWidth) {
                            FitEnd = Index - 1 + TokenLength - 1;
                        }
                    } else if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, WordLength - (Index - 1 - WordStart)) > 0) {
                        ++FixedWidthDepth;
                    } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, WordLength - (Index - 1 - WordStart)) > 0) {
                        FixedWidthDepth = std::max<std::int32_t>(FixedWidthDepth - 1, 0);
                    } else if (TCFontEC::ParseFormatTag(Text.pchar() + Index - 1, WordLength - (Index - 1 - WordStart), FieldWidth, Alignment) > 0) {
                        Index = Index + TokenLength - 1;
                        while (Index < CharCount) {
                            Ch = pas::load_unaligned<char16_t>(pas::byte_offset(static_cast<PFontTextCharsEC>(static_cast<void*>(Text.data())), Index * sizeof(char16_t)));
                            ++Index;
                            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, CharCount - Index + 1);
                            if (TokenLength > 0) {
                                if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                                    ++FixedWidthDepth;
                                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                                    FixedWidthDepth = std::max<std::int32_t>(FixedWidthDepth - 1, 0);
                                } else if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                                    Index = Index + TokenLength - 1;
                                    if (FieldWidth > 0) {
                                        if (FixedWidthDepth > 0) {
                                            WordWidth += MaxGlyphAdvance * FieldWidth;
                                        } else {
                                            WordWidth += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, u' ') * FieldWidth;
                                        }
                                    }
                                    break;
                                }
                                Index = Index + TokenLength - 1;
                            } else {
                                --FieldWidth;
                                if (FixedWidthDepth > 0) {
                                    WordWidth += MaxGlyphAdvance;
                                } else {
                                    WordWidth += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, Ch);
                                }
                            }
                        }
                        WordLength = Index - WordStart;
                        if (WordWidth <= MaxWidth) {
                            FitEnd = Index - 1 - 1;
                        }
                        continue;
                    }
                    Index = Index + TokenLength - 1;
                    continue;
                }
                if (FixedWidthDepth > 0) {
                    WordWidth += MaxGlyphAdvance;
                } else {
                    WordWidth += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, Ch);
                }
                if (WordWidth <= MaxWidth) {
                    FitEnd = Index - 1 - 1;
                }
            }
            if (LineWidth == 0) {
                if (WordWidth > MaxWidth) {
                    TokenLength = 0;
                    if (!FirstLine) {
                        while (Text.read(LineStart + 1 + TokenLength) == u' ') {
                            ++TokenLength;
                        }
                    }
                    if (LineLength - TokenLength > 0) {
                        Lines->AddSlice(Text.pchar() + LineStart + TokenLength, LineLength - TokenLength);
                    } else {
                        Lines->AddSlice(Text.pchar() + LineStart + TokenLength, FitEnd + 1 - WordStart);
                        WordStart = FitEnd + 1;
                        WordLength = 0;
                        WordWidth = 0;
                    }
                    FirstLine = false;
                }
                LineStart = WordStart;
                WordStart += WordLength;
                LineWidth = WordWidth;
                LineLength = WordLength;
            } else if (LineWidth + WordWidth <= MaxWidth) {
                WordStart += WordLength;
                LineWidth += WordWidth;
                LineLength += WordLength;
            } else {
                TokenLength = 0;
                if (!FirstLine) {
                    while (Text.read(LineStart + 1 + TokenLength) == u' ') {
                        ++TokenLength;
                    }
                }
                Lines->AddSlice(Text.pchar() + LineStart + TokenLength, LineLength - TokenLength);
                FirstLine = false;
                LineStart = WordStart;
                WordStart += WordLength;
                LineWidth = WordWidth;
                LineLength = WordLength;
            }
        }
        TokenLength = 0;
        LineLength = CharCount - LineStart;
        if (!FirstLine) {
            while (Text.read(LineStart + 1 + TokenLength) == u' ') {
                ++TokenLength;
            }
        }
        if (LineLength - TokenLength > 0) {
            Lines->AddSlice(Text.pchar() + LineStart + TokenLength, LineLength - TokenLength);
        }
        ObjectCount = SavedObjectCount;
    }

    void TCFontEC::DrawTaggedText16(void* Destination, std::int32_t PitchBytes, std::int32_t X, std::int32_t Y, const pas::WideString& Text, WindowsSdk::TRect ClipRect) {
        std::int32_t GlyphIndex{};
        std::int32_t TokenLength{};
        std::int32_t PosX{};
        char16_t Ch{};
        PAftGlyphEC Glyph{};
        std::int32_t ObjectIndex{};
        std::int32_t MiddleY{};
        std::int32_t FieldWidth{};
        std::int32_t Alignment{};
        WindowsSdk::TRect Clip{};
        std::int32_t Top = 999999999;
        std::int32_t Bottom = -999999999;
        std::int32_t SavedObjectCount = ObjectCount;
        Clip.Left = ClipRect.Left;
        Clip.Top = ClipRect.Top;
        Clip.Right = ClipRect.Right - 1;
        Clip.Bottom = ClipRect.Bottom - 1;
        std::int32_t CharCount = Text.length();
        if (CharCount < 1) {
            return;
        }
        std::int32_t Index = 0;
        PosX = 0;
        FieldWidth = -1;
        while (Index < CharCount) {
            Ch = Text.read(Index + 1);
            ++Index;
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, CharCount - Index + 1);
            if (TokenLength > 0) {
                ApplyColorTag(Text.pchar() + Index - 1, CharCount - Index + 1);
                TCFontEC::ParseTabTagAndAdjustX(Text.pchar() + Index - 1, CharCount - Index + 1, PosX);
                ParseAlignTagAndAdjustX(Text.pchar() + Index - 1, CharCount - Index + 1, PosX);
                if (ParseObjectTagCached(Text.pchar() + Index - 1, CharCount - Index + 1, ObjectIndex) > 0) {
                    Objects[ObjectIndex].X = X + PosX;
                    PosX += Objects[ObjectIndex].Width;
                } else if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    ++FixedWidthDepth;
                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    FixedWidthDepth = std::max<std::int32_t>(FixedWidthDepth - 1, 0);
                } else if (FieldWidth < 0 && TCFontEC::ParseFormatTag(Text.pchar() + Index - 1, CharCount - Index + 1, FieldWidth, Alignment) > 0) {
                    Index = Index + TokenLength - 1;
                    FieldWidth -= CountVisibleTaggedCharsUntilFormatEnd(Text.pchar() + Index, CharCount - Index);
                    if (FieldWidth > 0) {
                        if (Alignment == 0) {
                            if (FixedWidthDepth > 0) {
                                PosX += MaxGlyphAdvance * pas::shr(FieldWidth, 1);
                            } else {
                                PosX += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, u' ') * pas::shr(FieldWidth, 1);
                            }
                            FieldWidth -= pas::shr(FieldWidth, 1);
                        } else if (Alignment > 0) {
                            if (FixedWidthDepth > 0) {
                                PosX += MaxGlyphAdvance * FieldWidth;
                            } else {
                                PosX += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, u' ') * FieldWidth;
                            }
                            FieldWidth = -1;
                        }
                    } else {
                        FieldWidth = -1;
                    }
                    continue;
                } else if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    if (FieldWidth > 0) {
                        if (FixedWidthDepth > 0) {
                            PosX += MaxGlyphAdvance * FieldWidth;
                        } else {
                            PosX += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, u' ') * FieldWidth;
                        }
                        FieldWidth = -1;
                    }
                }
                Index = Index + TokenLength - 1;
                continue;
            }
            GlyphIndex = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, Ch * 2));
            if (GlyphIndex == 0) {
                continue;
            }
            Glyph = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(Glyphs, (GlyphIndex - 1) * static_cast<std::int32_t>(sizeof(TAftGlyphEC))));
            if (Glyph->OpaqueMaskPlane.DataOffset != 0) {
                if (Y + Glyph->OpaqueMaskPlane.Top < Top) {
                    Top = Y + Glyph->OpaqueMaskPlane.Top;
                }
                if (Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height > Bottom) {
                    Bottom = Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height;
                }
                GR_Main::Ex_OKGR_MaskBuf_DrawClip_WORD(Destination, PitchBytes, X + PosX + Glyph->OpaqueMaskPlane.Left, Y + Glyph->OpaqueMaskPlane.Top, EC_Mem::AddPointerOffset(FontData, Glyph->OpaqueMaskPlane.DataOffset), GetCurrentColor(), Clip);
            }
            if (Glyph->AlphaMaskPlane.DataOffset != 0) {
                if (Y + Glyph->AlphaMaskPlane.Top < Top) {
                    Top = Y + Glyph->AlphaMaskPlane.Top;
                }
                if (Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height > Bottom) {
                    Bottom = Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height;
                }
                GR_Main::Ex_OKGR_TransBuf_FillAlphaClip_16(Destination, PitchBytes, X + PosX + Glyph->AlphaMaskPlane.Left, Y + Glyph->AlphaMaskPlane.Top, EC_Mem::AddPointerOffset(FontData, Glyph->AlphaMaskPlane.DataOffset), Clip, GetCurrentColor());
            }
            if (FixedWidthDepth > 0) {
                PosX += MaxGlyphAdvance;
            } else {
                PosX = PosX + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
            }
        }
        if (ObjectCount - SavedObjectCount > 0) {
            MiddleY = (Top + Bottom) / 2;
            for (auto cpp_range = pas::for_to<std::int32_t>(SavedObjectCount, ObjectCount - 1); cpp_range.next(Index); ) {
                if (Objects[Index].VerticalMode == 0) {
                    Objects[Index].Y = MiddleY - Objects[Index].Height / 2;
                }
            }
        }
    }

    void TCFontEC::DrawTaggedText32(void* Destination, std::int32_t PitchBytes, std::int32_t X, std::int32_t Y, const pas::WideString& Text, WindowsSdk::TRect ClipRect) {
        std::int32_t GlyphIndex{};
        std::int32_t TokenLength{};
        std::int32_t PosX{};
        char16_t Ch{};
        PAftGlyphEC Glyph{};
        std::int32_t ObjectIndex{};
        std::int32_t MiddleY{};
        std::int32_t FieldWidth{};
        std::int32_t Alignment{};
        WindowsSdk::TRect Clip{};
        std::int32_t Top = 999999999;
        std::int32_t Bottom = -999999999;
        std::int32_t SavedObjectCount = ObjectCount;
        Clip.Left = ClipRect.Left;
        Clip.Top = ClipRect.Top;
        Clip.Right = ClipRect.Right - 1;
        Clip.Bottom = ClipRect.Bottom - 1;
        std::int32_t CharCount = Text.length();
        if (CharCount < 1) {
            return;
        }
        std::int32_t Index = 0;
        PosX = 0;
        FieldWidth = -1;
        while (Index < CharCount) {
            Ch = Text.read(Index + 1);
            ++Index;
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, CharCount - Index + 1);
            if (TokenLength > 0) {
                ApplyColorTag(Text.pchar() + Index - 1, CharCount - Index + 1);
                TCFontEC::ParseTabTagAndAdjustX(Text.pchar() + Index - 1, CharCount - Index + 1, PosX);
                ParseAlignTagAndAdjustX(Text.pchar() + Index - 1, CharCount - Index + 1, PosX);
                if (ParseObjectTagCached(Text.pchar() + Index - 1, CharCount - Index + 1, ObjectIndex) > 0) {
                    Objects[ObjectIndex].X = X + PosX;
                    PosX += Objects[ObjectIndex].Width;
                } else if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    ++FixedWidthDepth;
                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    FixedWidthDepth = std::max<std::int32_t>(FixedWidthDepth - 1, 0);
                } else if (FieldWidth < 0 && TCFontEC::ParseFormatTag(Text.pchar() + Index - 1, CharCount - Index + 1, FieldWidth, Alignment) > 0) {
                    Index = Index + TokenLength - 1;
                    FieldWidth -= CountVisibleTaggedCharsUntilFormatEnd(Text.pchar() + Index, CharCount - Index);
                    if (FieldWidth > 0) {
                        if (Alignment == 0) {
                            if (FixedWidthDepth > 0) {
                                PosX += MaxGlyphAdvance * pas::shr(FieldWidth, 1);
                            } else {
                                PosX += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, u' ') * pas::shr(FieldWidth, 1);
                            }
                            FieldWidth -= pas::shr(FieldWidth, 1);
                        } else if (Alignment > 0) {
                            if (FixedWidthDepth > 0) {
                                PosX += MaxGlyphAdvance * FieldWidth;
                            } else {
                                PosX += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, u' ') * FieldWidth;
                            }
                            FieldWidth = -1;
                        }
                    } else {
                        FieldWidth = -1;
                    }
                    continue;
                } else if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    if (FieldWidth > 0) {
                        if (FixedWidthDepth > 0) {
                            PosX += MaxGlyphAdvance * FieldWidth;
                        } else {
                            PosX += EC_CacheFont::TCFontEC_GetGlyphAdvance(this, u' ') * FieldWidth;
                        }
                        FieldWidth = -1;
                    }
                }
                Index = Index + TokenLength - 1;
                continue;
            }
            GlyphIndex = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, Ch * 2));
            if (GlyphIndex == 0) {
                continue;
            }
            Glyph = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(Glyphs, (GlyphIndex - 1) * static_cast<std::int32_t>(sizeof(TAftGlyphEC))));
            if (Glyph->OpaqueMaskPlane.DataOffset != 0) {
                if (Y + Glyph->OpaqueMaskPlane.Top < Top) {
                    Top = Y + Glyph->OpaqueMaskPlane.Top;
                }
                if (Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height > Bottom) {
                    Bottom = Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height;
                }
                GR_Main::Ex_OKGR_MaskBuf_DrawClip_DWORD(Destination, PitchBytes, X + PosX + Glyph->OpaqueMaskPlane.Left, Y + Glyph->OpaqueMaskPlane.Top, EC_Mem::AddPointerOffset(FontData, Glyph->OpaqueMaskPlane.DataOffset), GetCurrentColor(), Clip);
            }
            if (Glyph->AlphaMaskPlane.DataOffset != 0) {
                if (Y + Glyph->AlphaMaskPlane.Top < Top) {
                    Top = Y + Glyph->AlphaMaskPlane.Top;
                }
                if (Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height > Bottom) {
                    Bottom = Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height;
                }
                GR_Main::Ex_OKGR_TransBuf_FillAlphaClip_RGBA(Destination, PitchBytes, X + PosX + Glyph->AlphaMaskPlane.Left, Y + Glyph->AlphaMaskPlane.Top, EC_Mem::AddPointerOffset(FontData, Glyph->AlphaMaskPlane.DataOffset), Clip, GetCurrentColor());
            }
            if (FixedWidthDepth > 0) {
                PosX += MaxGlyphAdvance;
            } else {
                PosX = PosX + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
            }
        }
        if (ObjectCount - SavedObjectCount > 0) {
            MiddleY = (Top + Bottom) / 2;
            for (auto cpp_range = pas::for_to<std::int32_t>(SavedObjectCount, ObjectCount - 1); cpp_range.next(Index); ) {
                if (Objects[Index].VerticalMode == 0) {
                    Objects[Index].Y = MiddleY - Objects[Index].Height / 2;
                }
            }
        }
    }

    void TCFontEC_DrawJustifiedTaggedText16(TCFontEC* Self, void* Destination, std::int32_t PitchBytes, std::int32_t X, std::int32_t Y, const pas::WideString& Text, std::int32_t Width, WindowsSdk::TRect ClipRect) {
        char16_t Character{};
        std::int32_t GlyphIndex{};
        PAftGlyphEC GlyphBase{};
        PAftGlyphEC Glyph{};
        std::int32_t TokenLength{};
        std::int32_t ObjectIndex{};
        std::int32_t MiddleY{};
        std::int32_t Alignment{};
        std::int32_t ParsedWidth{};
        std::int32_t ParsedAlignment{};
        WindowsSdk::TRect Clip{};
        std::int32_t Top = 999999999;
        std::int32_t Bottom = -999999999;
        std::int32_t SavedObjectCount = Self->ObjectCount;
        Clip.Left = ClipRect.Left;
        Clip.Top = ClipRect.Top;
        Clip.Right = ClipRect.Right - 1;
        Clip.Bottom = ClipRect.Bottom - 1;
        std::int32_t CharCount = Text.length();
        if (CharCount < 1) {
            return;
        }
        std::int32_t ContentWidth = 0;
        std::int32_t SpaceCount = 0;
        std::uint8_t Leading = true;
        std::int32_t Index = 0;
        std::int32_t FieldWidth = -1;
        ParsedWidth = -1;
        while (Index < CharCount) {
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index, CharCount - Index);
            if (TokenLength > 0) {
                if (Self->ParseObjectTagCached(Text.pchar() + Index, CharCount - Index, ObjectIndex) > 0) {
                    ContentWidth += Self->Objects[ObjectIndex].Width;
                    Leading = false;
                } else if (TCFontEC::MatchFixTag(Text.pchar() + Index, CharCount - Index) > 0) {
                    ++Self->FixedWidthDepth;
                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index, CharCount - Index) > 0) {
                    Self->FixedWidthDepth = std::max<std::int32_t>(Self->FixedWidthDepth - 1, 0);
                } else if (!(FieldWidth < 0 && TCFontEC::ParseFormatTag(Text.pchar() + Index, CharCount - Index, ParsedWidth, ParsedAlignment) > 0)) {
                    if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index, CharCount - Index) > 0) {
                        ParsedWidth = -1;
                    }
                }
                Index += TokenLength;
            } else {
                if (Text.read(Index + 1) != u' ' || FieldWidth > 0 || Self->FixedWidthDepth > 0) {
                    if (Self->FixedWidthDepth > 0) {
                        ContentWidth += Self->MaxGlyphAdvance;
                    } else {
                        ContentWidth += EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, Text.read(Index + 1));
                    }
                    Leading = false;
                } else if (Leading == true) {
                    ContentWidth += EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, Text.read(Index + 1));
                } else {
                    ++SpaceCount;
                }
                ++Index;
            }
        }
        if (SpaceCount < 1) {
            Self->DrawTaggedText16(Destination, PitchBytes, X, Y, Text, ClipRect);
            return;
        }
        double SpaceWidth = pas::real_divide(Width - ContentWidth, SpaceCount);
        if (SpaceWidth < 2.0L) {
            Self->DrawTaggedText16(Destination, PitchBytes, X, Y, Text, ClipRect);
            return;
        }
        Self->ObjectCount = SavedObjectCount;
        PFontGlyphLookupEC Lookup = Self->GlyphLookup;
        GlyphBase = Self->Glyphs;
        double PosX = X;
        Leading = true;
        Index = 0;
        while (Index < CharCount) {
            Character = Text.read(Index + 1);
            ++Index;
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, CharCount - Index + 1);
            if (TokenLength > 0) {
                Self->ApplyColorTag(Text.pchar() + Index - 1, CharCount - Index + 1);
                if (Self->ParseObjectTagCached(Text.pchar() + Index - 1, CharCount - Index + 1, ObjectIndex) > 0) {
                    Self->Objects[ObjectIndex].X = System::Trunc(PosX);
                    PosX = static_cast<long double>(PosX) + Self->Objects[ObjectIndex].Width;
                    Leading = false;
                } else if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    ++Self->FixedWidthDepth;
                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    Self->FixedWidthDepth = std::max<std::int32_t>(Self->FixedWidthDepth - 1, 0);
                } else if (FieldWidth < 0 && TCFontEC::ParseFormatTag(Text.pchar() + Index - 1, CharCount - Index + 1, ParsedWidth, ParsedAlignment) > 0) {
                    FieldWidth = ParsedWidth;
                    Alignment = ParsedAlignment;
                    Index = Index + TokenLength - 1;
                    FieldWidth -= Self->CountVisibleTaggedCharsUntilFormatEnd(Text.pchar() + Index, CharCount - Index);
                    if (FieldWidth > 0) {
                        if (Alignment == 0) {
                            if (Self->FixedWidthDepth > 0) {
                                PosX = static_cast<long double>(PosX) + Self->MaxGlyphAdvance * pas::shr(FieldWidth, 1);
                            } else {
                                PosX = static_cast<long double>(PosX) + EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, u' ') * pas::shr(FieldWidth, 1);
                            }
                            FieldWidth -= pas::shr(FieldWidth, 1);
                        } else if (Alignment > 0) {
                            PosX = static_cast<long double>(PosX) + EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, u' ') * FieldWidth;
                            FieldWidth = -1;
                        }
                    } else {
                        FieldWidth = -1;
                    }
                    continue;
                } else if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    ParsedWidth = -1;
                    if (FieldWidth > 0) {
                        if (Self->FixedWidthDepth > 0) {
                            PosX = static_cast<long double>(PosX) + Self->MaxGlyphAdvance * FieldWidth;
                        } else {
                            PosX = static_cast<long double>(PosX) + EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, u' ') * FieldWidth;
                        }
                        FieldWidth = -1;
                    }
                }
                Index = Index + TokenLength - 1;
            } else {
                GlyphIndex = pas::load_unaligned<std::uint16_t>(pas::byte_offset(Lookup, Character * sizeof(std::uint16_t)));
                if (GlyphIndex != 0) {
                    Glyph = GlyphBase + (GlyphIndex - 1);
                    if (Glyph->OpaqueMaskPlane.DataOffset != 0) {
                        if (Y + Glyph->OpaqueMaskPlane.Top < Top) {
                            Top = Y + Glyph->OpaqueMaskPlane.Top;
                        }
                        if (Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height > Bottom) {
                            Bottom = Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height;
                        }
                        GR_Main::Ex_OKGR_MaskBuf_DrawClip_WORD(Destination, PitchBytes, static_cast<std::int32_t>(System::Trunc(PosX)) + Glyph->OpaqueMaskPlane.Left, Y + Glyph->OpaqueMaskPlane.Top, EC_Mem::AddPointerOffset(Self->FontData, Glyph->OpaqueMaskPlane.DataOffset), Self->GetCurrentColor(), Clip);
                    }
                    if (Glyph->AlphaMaskPlane.DataOffset != 0) {
                        if (Y + Glyph->AlphaMaskPlane.Top < Top) {
                            Top = Y + Glyph->AlphaMaskPlane.Top;
                        }
                        if (Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height > Bottom) {
                            Bottom = Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height;
                        }
                        GR_Main::Ex_OKGR_TransBuf_FillAlphaClip_16(Destination, PitchBytes, static_cast<std::int32_t>(System::Trunc(PosX)) + Glyph->AlphaMaskPlane.Left, Y + Glyph->AlphaMaskPlane.Top, EC_Mem::AddPointerOffset(Self->FontData, Glyph->AlphaMaskPlane.DataOffset), Clip, Self->GetCurrentColor());
                    }
                    if (Character != u' ' || FieldWidth > 0 || Self->FixedWidthDepth > 0) {
                        if (Self->FixedWidthDepth > 0) {
                            PosX = static_cast<long double>(PosX) + Self->MaxGlyphAdvance;
                        } else {
                            PosX = static_cast<long double>(PosX) + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
                        }
                        Leading = false;
                    } else if (Leading == true) {
                        PosX = static_cast<long double>(PosX) + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
                    } else {
                        PosX = static_cast<long double>(PosX) + SpaceWidth;
                    }
                }
            }
        }
        if (Self->ObjectCount - SavedObjectCount > 0) {
            MiddleY = (Top + Bottom) / 2;
            for (auto cpp_range = pas::for_to<std::int32_t>(SavedObjectCount, Self->ObjectCount - 1); cpp_range.next(Index); ) {
                if (Self->Objects[Index].VerticalMode == 0) {
                    Self->Objects[Index].Y = MiddleY - Self->Objects[Index].Height / 2;
                }
            }
        }
    }

    void TCFontEC_DrawJustifiedTaggedText32(TCFontEC* Self, void* Destination, std::int32_t PitchBytes, std::int32_t X, std::int32_t Y, const pas::WideString& Text, std::int32_t Width, WindowsSdk::TRect ClipRect) {
        char16_t Character{};
        std::int32_t GlyphIndex{};
        PAftGlyphEC GlyphBase{};
        PAftGlyphEC Glyph{};
        std::int32_t TokenLength{};
        std::int32_t ObjectIndex{};
        std::int32_t MiddleY{};
        std::int32_t Alignment{};
        std::int32_t ParsedWidth{};
        std::int32_t ParsedAlignment{};
        WindowsSdk::TRect Clip{};
        std::int32_t Top = 999999999;
        std::int32_t Bottom = -999999999;
        std::int32_t SavedObjectCount = Self->ObjectCount;
        Clip.Left = ClipRect.Left;
        Clip.Top = ClipRect.Top;
        Clip.Right = ClipRect.Right - 1;
        Clip.Bottom = ClipRect.Bottom - 1;
        std::int32_t CharCount = Text.length();
        if (CharCount < 1) {
            return;
        }
        std::int32_t ContentWidth = 0;
        std::int32_t SpaceCount = 0;
        std::uint8_t Leading = true;
        std::int32_t Index = 0;
        std::int32_t FieldWidth = -1;
        ParsedWidth = -1;
        while (Index < CharCount) {
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index, CharCount - Index);
            if (TokenLength > 0) {
                if (Self->ParseObjectTagCached(Text.pchar() + Index, CharCount - Index, ObjectIndex) > 0) {
                    ContentWidth += Self->Objects[ObjectIndex].Width;
                    Leading = false;
                } else if (TCFontEC::MatchFixTag(Text.pchar() + Index, CharCount - Index) > 0) {
                    ++Self->FixedWidthDepth;
                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index, CharCount - Index) > 0) {
                    Self->FixedWidthDepth = std::max<std::int32_t>(Self->FixedWidthDepth - 1, 0);
                } else if (!(FieldWidth < 0 && TCFontEC::ParseFormatTag(Text.pchar() + Index, CharCount - Index, ParsedWidth, ParsedAlignment) > 0)) {
                    if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index, CharCount - Index) > 0) {
                        ParsedWidth = -1;
                    }
                }
                Index += TokenLength;
            } else {
                if (Text.read(Index + 1) != u' ' || FieldWidth > 0 || Self->FixedWidthDepth > 0) {
                    if (Self->FixedWidthDepth > 0) {
                        ContentWidth += Self->MaxGlyphAdvance;
                    } else {
                        ContentWidth += EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, Text.read(Index + 1));
                    }
                    Leading = false;
                } else if (Leading == true) {
                    ContentWidth += EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, Text.read(Index + 1));
                } else {
                    ++SpaceCount;
                }
                ++Index;
            }
        }
        if (SpaceCount < 1) {
            Self->DrawTaggedText32(Destination, PitchBytes, X, Y, Text, ClipRect);
            return;
        }
        double SpaceWidth = pas::real_divide(Width - ContentWidth, SpaceCount);
        if (SpaceWidth < 2.0L) {
            Self->DrawTaggedText32(Destination, PitchBytes, X, Y, Text, ClipRect);
            return;
        }
        Self->ObjectCount = SavedObjectCount;
        PFontGlyphLookupEC Lookup = Self->GlyphLookup;
        GlyphBase = Self->Glyphs;
        double PosX = X;
        Leading = true;
        Index = 0;
        while (Index < CharCount) {
            Character = Text.read(Index + 1);
            ++Index;
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text.pchar() + Index - 1, CharCount - Index + 1);
            if (TokenLength > 0) {
                Self->ApplyColorTag(Text.pchar() + Index - 1, CharCount - Index + 1);
                if (Self->ParseObjectTagCached(Text.pchar() + Index - 1, CharCount - Index + 1, ObjectIndex) > 0) {
                    Self->Objects[ObjectIndex].X = System::Trunc(PosX);
                    PosX = static_cast<long double>(PosX) + Self->Objects[ObjectIndex].Width;
                    Leading = false;
                } else if (TCFontEC::MatchFixTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    ++Self->FixedWidthDepth;
                } else if (TCFontEC::MatchFixEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    Self->FixedWidthDepth = std::max<std::int32_t>(Self->FixedWidthDepth - 1, 0);
                } else if (FieldWidth < 0 && TCFontEC::ParseFormatTag(Text.pchar() + Index - 1, CharCount - Index + 1, ParsedWidth, ParsedAlignment) > 0) {
                    FieldWidth = ParsedWidth;
                    Alignment = ParsedAlignment;
                    Index = Index + TokenLength - 1;
                    FieldWidth -= Self->CountVisibleTaggedCharsUntilFormatEnd(Text.pchar() + Index, CharCount - Index);
                    if (FieldWidth > 0) {
                        if (Alignment == 0) {
                            if (Self->FixedWidthDepth > 0) {
                                PosX = static_cast<long double>(PosX) + Self->MaxGlyphAdvance * pas::shr(FieldWidth, 1);
                            } else {
                                PosX = static_cast<long double>(PosX) + EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, u' ') * pas::shr(FieldWidth, 1);
                            }
                            FieldWidth -= pas::shr(FieldWidth, 1);
                        } else if (Alignment > 0) {
                            if (Self->FixedWidthDepth > 0) {
                                PosX = static_cast<long double>(PosX) + Self->MaxGlyphAdvance * FieldWidth;
                            } else {
                                PosX = static_cast<long double>(PosX) + EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, u' ') * FieldWidth;
                            }
                            FieldWidth = -1;
                        }
                    } else {
                        FieldWidth = -1;
                    }
                    continue;
                } else if (TCFontEC::MatchFormatEndTag(Text.pchar() + Index - 1, CharCount - Index + 1) > 0) {
                    ParsedWidth = -1;
                    if (FieldWidth > 0) {
                        if (Self->FixedWidthDepth > 0) {
                            PosX = static_cast<long double>(PosX) + Self->MaxGlyphAdvance * FieldWidth;
                        } else {
                            PosX = static_cast<long double>(PosX) + EC_CacheFont::TCFontEC_GetGlyphAdvance(Self, u' ') * FieldWidth;
                        }
                        FieldWidth = -1;
                    }
                }
                Index = Index + TokenLength - 1;
            } else {
                GlyphIndex = pas::load_unaligned<std::uint16_t>(pas::byte_offset(Lookup, Character * sizeof(std::uint16_t)));
                if (GlyphIndex != 0) {
                    Glyph = GlyphBase + (GlyphIndex - 1);
                    if (Glyph->OpaqueMaskPlane.DataOffset != 0) {
                        if (Y + Glyph->OpaqueMaskPlane.Top < Top) {
                            Top = Y + Glyph->OpaqueMaskPlane.Top;
                        }
                        if (Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height > Bottom) {
                            Bottom = Y + Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height;
                        }
                        GR_Main::Ex_OKGR_MaskBuf_DrawClip_DWORD(Destination, PitchBytes, static_cast<std::int32_t>(System::Trunc(PosX)) + Glyph->OpaqueMaskPlane.Left, Y + Glyph->OpaqueMaskPlane.Top, EC_Mem::AddPointerOffset(Self->FontData, Glyph->OpaqueMaskPlane.DataOffset), Self->GetCurrentColor(), Clip);
                    }
                    if (Glyph->AlphaMaskPlane.DataOffset != 0) {
                        if (Y + Glyph->AlphaMaskPlane.Top < Top) {
                            Top = Y + Glyph->AlphaMaskPlane.Top;
                        }
                        if (Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height > Bottom) {
                            Bottom = Y + Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height;
                        }
                        GR_Main::Ex_OKGR_TransBuf_FillAlphaClip_RGBA(Destination, PitchBytes, static_cast<std::int32_t>(System::Trunc(PosX)) + Glyph->AlphaMaskPlane.Left, Y + Glyph->AlphaMaskPlane.Top, EC_Mem::AddPointerOffset(Self->FontData, Glyph->AlphaMaskPlane.DataOffset), Clip, Self->GetCurrentColor());
                    }
                    if (Character != u' ' || FieldWidth > 0 || Self->FixedWidthDepth > 0) {
                        if (Self->FixedWidthDepth > 0) {
                            PosX = static_cast<long double>(PosX) + Self->MaxGlyphAdvance;
                        } else {
                            PosX = static_cast<long double>(PosX) + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
                        }
                        Leading = false;
                    } else if (Leading == true) {
                        PosX = static_cast<long double>(PosX) + Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC;
                    } else {
                        PosX = static_cast<long double>(PosX) + SpaceWidth;
                    }
                }
            }
        }
        if (Self->ObjectCount - SavedObjectCount > 0) {
            MiddleY = (Top + Bottom) / 2;
            for (auto cpp_range = pas::for_to<std::int32_t>(SavedObjectCount, Self->ObjectCount - 1); cpp_range.next(Index); ) {
                if (Self->Objects[Index].VerticalMode == 0) {
                    Self->Objects[Index].Y = MiddleY - Self->Objects[Index].Height / 2;
                }
            }
        }
    }

    // Returns zero for incomplete tokens or a doubled opening bracket.
    std::int32_t TCFontEC::GetTaggedTextTokenLength(char16_t* Text, std::int32_t CharCount) {
        std::int32_t Result = 0;
        if (CharCount < 2) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) == u'<') {
            return 0;
        }
        std::int32_t Index = 1;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) == u'>') {
                break;
            }
            ++Index;
        }
        if (Index < CharCount) {
            return Index + 1;
        }
        return Result;
    }

    // For td=n, raises X to at least n and returns the token length.
    std::int32_t TCFontEC::ParseTabTagAndAdjustX(char16_t* Text, std::int32_t CharCount, std::int32_t& X) {
        std::int32_t Result = 0;
        if (CharCount < 4) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u't' && pas::load_unaligned<char16_t>(Text + 1) != u'T') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'd' && pas::load_unaligned<char16_t>(Text + 2) != u'D') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'=' && pas::load_unaligned<char16_t>(Text + 3) != u'=') {
            return Result;
        }
        std::int32_t Value = 0;
        std::int32_t Index = 4;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Value = Value * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index < CharCount && pas::load_unaligned<char16_t>(Text + Index) == u'>') {
            Result = Index + 1;
            if (X < Value) {
                X = Value;
            }
        }
        return Result;
    }

    // Handles align=right/center and restores ObjectCount. Uppercase value checks use incorrect source positions in the native code.
    std::int32_t TCFontEC::ParseAlignTagAndAdjustX(char16_t* Text, std::int32_t CharCount, std::int32_t& X) {
        char16_t Ch{};
        std::int32_t GlyphIndex{};
        std::int32_t TokenLength{};
        PAftGlyphEC Glyph{};
        std::int32_t ObjectIndex{};
        pas::WideString TextCopy{};
        std::int32_t SavedObjectCount = ObjectCount;
        std::int32_t Result = 0;
        if (CharCount < 7) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'a' && pas::load_unaligned<char16_t>(Text + 1) != u'A') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'l' && pas::load_unaligned<char16_t>(Text + 2) != u'L') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'i' && pas::load_unaligned<char16_t>(Text + 3) != u'I') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'g' && pas::load_unaligned<char16_t>(Text + 4) != u'G') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) != u'n' && pas::load_unaligned<char16_t>(Text + 5) != u'N') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 6) != u'=' && pas::load_unaligned<char16_t>(Text + 6) != u'=') {
            return Result;
        }
        std::int32_t Index = 0;
        std::uint8_t AlignRight = false;
        if (CharCount >= 13) {
            AlignRight = (pas::load_unaligned<char16_t>(Text + 7) == u'r' || pas::load_unaligned<char16_t>(Text + 1) == u'R') && (pas::load_unaligned<char16_t>(Text + 8) == u'i' || pas::load_unaligned<char16_t>(Text + 2) == u'I') && (pas::load_unaligned<char16_t>(Text + 9) == u'g' || pas::load_unaligned<char16_t>(Text + 3) == u'G') && (pas::load_unaligned<char16_t>(Text + 10) == u'h' || pas::load_unaligned<char16_t>(Text + 4) == u'H') && (pas::load_unaligned<char16_t>(Text + 11) == u't' || pas::load_unaligned<char16_t>(Text + 5) == u'T') && pas::load_unaligned<char16_t>(Text + 12) == u'>';
            Index = 13;
        }
        if (!AlignRight) {
            if (CharCount < 14) {
                return Result;
            }
            if (pas::load_unaligned<char16_t>(Text + 7) != u'c' && pas::load_unaligned<char16_t>(Text + 1) != u'C') {
                return Result;
            }
            if (pas::load_unaligned<char16_t>(Text + 8) != u'e' && pas::load_unaligned<char16_t>(Text + 2) != u'E') {
                return Result;
            }
            if (pas::load_unaligned<char16_t>(Text + 9) != u'n' && pas::load_unaligned<char16_t>(Text + 3) != u'N') {
                return Result;
            }
            if (pas::load_unaligned<char16_t>(Text + 10) != u't' && pas::load_unaligned<char16_t>(Text + 4) != u'T') {
                return Result;
            }
            if (pas::load_unaligned<char16_t>(Text + 11) != u'e' && pas::load_unaligned<char16_t>(Text + 5) != u'E') {
                return Result;
            }
            if (pas::load_unaligned<char16_t>(Text + 12) != u'r' && pas::load_unaligned<char16_t>(Text + 5) != u'R') {
                return Result;
            }
            if (pas::load_unaligned<char16_t>(Text + 13) != u'>') {
                return Result;
            }
            Index = 14;
        }
        Result = Index;
        TextCopy = static_cast<pas::WideString>(Text);
        std::int32_t TextLength = TextCopy.length();
        std::int32_t Width = 0;
        while (Index < TextLength) {
            Ch = pas::load_unaligned<char16_t>(Text + Index);
            ++Index;
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text + Index - 1, TextLength - Index + 1);
            if (TokenLength > 0) {
                if (TCFontEC::MatchAlignEndTag(Text + Index - 1, TextLength - Index + 1) > 0) {
                    break;
                }
                if (ParseObjectTagCached(Text + Index - 1, TextLength - Index + 1, ObjectIndex) > 0) {
                    Width += Objects[ObjectIndex].Width;
                }
                Index = Index + TokenLength - 1;
            } else {
                GlyphIndex = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, Ch * 2));
                if (GlyphIndex != 0) {
                    Glyph = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(Glyphs, (GlyphIndex - 1) * static_cast<std::int32_t>(sizeof(TAftGlyphEC))));
                    Width = Glyph->AdvanceA + Width + Glyph->AdvanceB + Glyph->AdvanceC;
                }
            }
        }
        if (AlignRight) {
            X -= Width;
        } else {
            X -= Width / 2;
        }
        ObjectCount = SavedObjectCount;
        return Result;
    }

    std::int32_t TCFontEC::MatchAlignEndTag(char16_t* Text, std::int32_t CharCount) {
        std::int32_t Result = 0;
        if (CharCount < 8) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'/') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'a' && pas::load_unaligned<char16_t>(Text + 2) != u'A') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'l' && pas::load_unaligned<char16_t>(Text + 3) != u'L') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'i' && pas::load_unaligned<char16_t>(Text + 4) != u'I') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) != u'g' && pas::load_unaligned<char16_t>(Text + 5) != u'G') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 6) != u'n' && pas::load_unaligned<char16_t>(Text + 6) != u'N') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 7) == u'>') {
            return 8;
        }
        return Result;
    }

    std::int32_t TCFontEC::MatchFixTag(char16_t* Text, std::int32_t CharCount) {
        std::int32_t Result = 0;
        if (CharCount < 5) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'f' && pas::load_unaligned<char16_t>(Text + 1) != u'F') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'i' && pas::load_unaligned<char16_t>(Text + 2) != u'I') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'x' && pas::load_unaligned<char16_t>(Text + 3) != u'X') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) == u'>') {
            return 5;
        }
        return Result;
    }

    std::int32_t TCFontEC::MatchFixEndTag(char16_t* Text, std::int32_t CharCount) {
        std::int32_t Result = 0;
        if (CharCount < 6) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'/') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'f' && pas::load_unaligned<char16_t>(Text + 2) != u'F') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'i' && pas::load_unaligned<char16_t>(Text + 3) != u'I') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'x' && pas::load_unaligned<char16_t>(Text + 4) != u'X') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) == u'>') {
            return 6;
        }
        return Result;
    }

    // Returns the token length; Alignment is -1 for left, 0 for center, 1 for right. FieldWidth counts characters.
    std::int32_t TCFontEC::ParseFormatTag(char16_t* Text, std::int32_t CharCount, std::int32_t& FieldWidth, std::int32_t& Alignment) {
        std::int32_t Result = 0;
        if (CharCount < 13) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'f' && pas::load_unaligned<char16_t>(Text + 1) != u'F') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'o' && pas::load_unaligned<char16_t>(Text + 2) != u'O') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'r' && pas::load_unaligned<char16_t>(Text + 3) != u'R') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'm' && pas::load_unaligned<char16_t>(Text + 4) != u'M') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) != u'a' && pas::load_unaligned<char16_t>(Text + 5) != u'A') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 6) != u't' && pas::load_unaligned<char16_t>(Text + 6) != u'T') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 7) != u'=' && pas::load_unaligned<char16_t>(Text + 7) != u'=') {
            return Result;
        }
        FieldWidth = 0;
        Alignment = 0;
        std::int32_t Index = 8;
        if (Index + 5 <= CharCount && (pas::load_unaligned<char16_t>(Text + Index) == u'r' || pas::load_unaligned<char16_t>(Text + Index) == u'R') && (pas::load_unaligned<char16_t>(Text + (Index + 1)) == u'i' || pas::load_unaligned<char16_t>(Text + (Index + 1)) == u'I') && (pas::load_unaligned<char16_t>(Text + (Index + 2)) == u'g' || pas::load_unaligned<char16_t>(Text + (Index + 2)) == u'G') && (pas::load_unaligned<char16_t>(Text + (Index + 3)) == u'h' || pas::load_unaligned<char16_t>(Text + (Index + 3)) == u'H') && (pas::load_unaligned<char16_t>(Text + (Index + 4)) == u't' || pas::load_unaligned<char16_t>(Text + (Index + 4)) == u'T')) {
            Alignment = 1;
            Index += 5;
        } else if (Index + 4 <= CharCount && (pas::load_unaligned<char16_t>(Text + Index) == u'l' || pas::load_unaligned<char16_t>(Text + Index) == u'L') && (pas::load_unaligned<char16_t>(Text + (Index + 1)) == u'e' || pas::load_unaligned<char16_t>(Text + (Index + 1)) == u'E') && (pas::load_unaligned<char16_t>(Text + (Index + 2)) == u'f' || pas::load_unaligned<char16_t>(Text + (Index + 2)) == u'F') && (pas::load_unaligned<char16_t>(Text + (Index + 3)) == u't' || pas::load_unaligned<char16_t>(Text + (Index + 3)) == u'T')) {
            Alignment = -1;
            Index += 4;
        } else if (Index + 6 <= CharCount && (pas::load_unaligned<char16_t>(Text + Index) == u'c' || pas::load_unaligned<char16_t>(Text + Index) == u'C') && (pas::load_unaligned<char16_t>(Text + (Index + 1)) == u'e' || pas::load_unaligned<char16_t>(Text + (Index + 1)) == u'E') && (pas::load_unaligned<char16_t>(Text + (Index + 2)) == u'n' || pas::load_unaligned<char16_t>(Text + (Index + 2)) == u'N') && (pas::load_unaligned<char16_t>(Text + (Index + 3)) == u't' || pas::load_unaligned<char16_t>(Text + (Index + 3)) == u'T') && (pas::load_unaligned<char16_t>(Text + (Index + 4)) == u'e' || pas::load_unaligned<char16_t>(Text + (Index + 4)) == u'E') && (pas::load_unaligned<char16_t>(Text + (Index + 5)) == u'r' || pas::load_unaligned<char16_t>(Text + (Index + 5)) == u'R')) {
            Alignment = 0;
            Index += 6;
        } else {
            return Result;
        }
        if (Index >= CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + Index) != u',') {
            return Result;
        }
        ++Index;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            FieldWidth = FieldWidth * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index < CharCount && pas::load_unaligned<char16_t>(Text + Index) == u'>') {
            return Index + 1;
        }
        return Result;
    }

    std::int32_t TCFontEC::MatchFormatEndTag(char16_t* Text, std::int32_t CharCount) {
        std::int32_t Result = 0;
        if (CharCount < 9) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'/') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'f' && pas::load_unaligned<char16_t>(Text + 2) != u'F') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'o' && pas::load_unaligned<char16_t>(Text + 3) != u'O') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'r' && pas::load_unaligned<char16_t>(Text + 4) != u'R') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) != u'm' && pas::load_unaligned<char16_t>(Text + 5) != u'M') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 6) != u'a' && pas::load_unaligned<char16_t>(Text + 6) != u'A') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 7) != u't' && pas::load_unaligned<char16_t>(Text + 7) != u'T') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 8) == u'>') {
            return 9;
        }
        return Result;
    }

    // Only characters present in GlyphLookup count.
    std::int32_t TCFontEC::CountVisibleTaggedCharsUntilFormatEnd(char16_t* Text, std::int32_t CharCount) {
        std::int32_t GlyphIndex{};
        std::int32_t TokenLength{};
        char16_t Ch{};
        std::int32_t Result = 0;
        std::int32_t Index = 0;
        while (Index < CharCount) {
            Ch = pas::load_unaligned<char16_t>(pas::byte_offset(reinterpret_cast<PFontTextCharsEC>(Text), Index * sizeof(char16_t)));
            ++Index;
            TokenLength = TCFontEC::GetTaggedTextTokenLength(Text + Index - 1, CharCount - Index + 1);
            if (TokenLength > 0) {
                if (TCFontEC::MatchFormatEndTag(Text + Index - 1, CharCount - Index + 1) > 0) {
                    break;
                }
                Index = Index + TokenLength - 1;
            } else {
                GlyphIndex = EC_Mem::ReadWordEC(EC_Mem::AddPointerOffset(GlyphLookup, Ch * 2));
                if (GlyphIndex != 0) {
                    ++Result;
                }
            }
        }
        return Result;
    }

    // Native capacity check is reversed: it reallocates when ObjectCount <= ObjectCapacity.
    std::int32_t TCFontEC::ParseObjectTagCached(char16_t* Text, std::int32_t CharCount, std::int32_t& ObjectIndex) {
        if (ObjectCount <= ObjectCapacity) {
            ObjectCapacity = ObjectCount + 4;
            Objects.set_length(ObjectCapacity);
        }
        ObjectIndex = -1;
        std::int32_t Result = TCFontEC::ParseObjectTag(Text, CharCount, Objects[ObjectCount]);
        if (Result > 0) {
            ObjectIndex = ObjectCount;
            ++ObjectCount;
        }
        return Result;
    }

    // Parses object=id,width,height,verticalMode; leaves X and Y unchanged.
    std::int32_t TCFontEC::ParseObjectTag(char16_t* Text, std::int32_t CharCount, TFontObjectEC& Item) {
        std::int32_t Result = 0;
        if (CharCount < 13) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'o' && pas::load_unaligned<char16_t>(Text + 1) != u'O') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'b' && pas::load_unaligned<char16_t>(Text + 2) != u'B') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'j' && pas::load_unaligned<char16_t>(Text + 3) != u'J') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'e' && pas::load_unaligned<char16_t>(Text + 4) != u'E') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) != u'c' && pas::load_unaligned<char16_t>(Text + 5) != u'C') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 6) != u't' && pas::load_unaligned<char16_t>(Text + 6) != u'T') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 7) != u'=' && pas::load_unaligned<char16_t>(Text + 7) != u'=') {
            return Result;
        }
        Item.ObjectId = 0;
        Item.Width = 0;
        Item.Height = 0;
        Item.VerticalMode = 0;
        std::int32_t Index = 8;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Item.ObjectId = Item.ObjectId * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index >= CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + Index) != u',') {
            return Result;
        }
        ++Index;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Item.Width = Item.Width * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index >= CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + Index) != u',') {
            return Result;
        }
        ++Index;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Item.Height = Item.Height * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index >= CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + Index) != u',') {
            return Result;
        }
        ++Index;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Item.VerticalMode = Item.VerticalMode * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index < CharCount && pas::load_unaligned<char16_t>(Text + Index) == u'>') {
            return Index + 1;
        }
        return Result;
    }

    // Parses color=r,g,b; emits ARGB or the current packed pixel format according to UseARGBColors.
    std::int32_t TCFontEC::ParseColorTag(char16_t* Text, std::int32_t CharCount, std::uint32_t& Color) {
        std::int32_t Result = 0;
        if (CharCount < 13) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'c' && pas::load_unaligned<char16_t>(Text + 1) != u'C') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'o' && pas::load_unaligned<char16_t>(Text + 2) != u'O') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'l' && pas::load_unaligned<char16_t>(Text + 3) != u'L') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'o' && pas::load_unaligned<char16_t>(Text + 4) != u'O') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) != u'r' && pas::load_unaligned<char16_t>(Text + 5) != u'R') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 6) != u'=' && pas::load_unaligned<char16_t>(Text + 6) != u'=') {
            return Result;
        }
        std::int32_t Red = 0;
        std::int32_t Green = 0;
        std::int32_t Blue = 0;
        std::int32_t Index = 7;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Red = Red * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index >= CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + Index) != u',') {
            return Result;
        }
        ++Index;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Green = Green * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index >= CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + Index) != u',') {
            return Result;
        }
        ++Index;
        while (Index < CharCount) {
            if (pas::load_unaligned<char16_t>(Text + Index) < u'0' || pas::load_unaligned<char16_t>(Text + Index) > u'9') {
                break;
            }
            Blue = Blue * 10 + pas::load_unaligned<char16_t>(Text + Index) - '0';
            ++Index;
        }
        if (Index >= CharCount) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + Index) != u'>') {
            return Result;
        }
        Result = Index + 1;
        if (!UseARGBColors) {
            Color = GR_Main::CurrentPixelFormat->PackRgbBytes(Red, Green, Blue);
        } else {
            Color = pas::shl(Red, 16) | pas::shl(Green, 8) | Blue | 0xff000000u;
        }
        return Result;
    }

    std::int32_t TCFontEC::MatchColorEndTag(char16_t* Text, std::int32_t CharCount) {
        std::int32_t Result = 0;
        if (CharCount < 8) {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 0) != u'<') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 1) != u'/') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 2) != u'c' && pas::load_unaligned<char16_t>(Text + 2) != u'C') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 3) != u'o' && pas::load_unaligned<char16_t>(Text + 3) != u'O') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 4) != u'l' && pas::load_unaligned<char16_t>(Text + 4) != u'L') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 5) != u'o' && pas::load_unaligned<char16_t>(Text + 5) != u'O') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 6) != u'r' && pas::load_unaligned<char16_t>(Text + 6) != u'R') {
            return Result;
        }
        if (pas::load_unaligned<char16_t>(Text + 7) == u'>') {
            return 8;
        }
        return Result;
    }

    // Pushes or pops a color only while ColorTagsEnabled is true.
    void TCFontEC::ApplyColorTag(char16_t* Text, std::int32_t CharCount) {
        std::uint32_t Color{};
        if (ParseColorTag(Text, CharCount, Color) > 0) {
            if (ColorTagsEnabled) {
                PushColor(Color);
            }
        } else if (TCFontEC::MatchColorEndTag(Text, CharCount) > 0) {
            if (ColorTagsEnabled) {
                PopColor();
            }
        }
    }

    void TCFontEC::ClearColorStack() {
        if (ColorStack != nullptr) {
            EC_Mem::FreeEC(ColorStack);
            ColorStack = nullptr;
        }
        ColorStackCount = 0;
    }

    void TCFontEC::PushColor(std::uint32_t Color) {
        ++ColorStackCount;
        ColorStack = static_cast<System::PCardinal>(EC_Mem::ReAllocREC(ColorStack, ColorStackCount * static_cast<std::int32_t>(sizeof(std::uint32_t))));
        EC_Mem::WriteIntegerEC(EC_Mem::AddPointerOffset(ColorStack, (ColorStackCount - 1) * static_cast<std::int32_t>(sizeof(std::uint32_t))), Color);
    }

    // Returns zero when empty.
    std::uint32_t TCFontEC::PopColor() {
        if (ColorStackCount < 1) {
            return 0u;
        }
        std::uint32_t Result = EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(ColorStack, (ColorStackCount - 1) * static_cast<std::int32_t>(sizeof(std::uint32_t))));
        --ColorStackCount;
        return Result;
    }

    // Returns DefaultColor when tags are disabled or the stack is empty.
    std::uint32_t TCFontEC::GetCurrentColor() {
        if (static_cast<std::uint8_t>(ColorTagsEnabled ^ 1) || ColorStackCount < 1) {
            return DefaultColor;
        }
        return EC_Mem::ReadDWordEC(EC_Mem::AddPointerOffset(ColorStack, (ColorStackCount - 1) * static_cast<std::int32_t>(sizeof(std::uint32_t))));
    }

    // Requires aft version 1 and at least 0x20 bytes; glyph offsets and counts are trusted. Ignores LoadOption; ResidentBytes remains zero.
    void TCFontEC::LoadFromConfigBuffer(EC_Buf::TBufEC* SourceBuffer, const pas::WideString& LoadOption) {
        std::int32_t Index{};
        ClearLoadedFontData();
        if (SourceBuffer->DataSize < static_cast<std::int32_t>(sizeof(TAftHeaderEC))) {
            pas::raise(pas::make_exception<pas::Exception>("TCFontEC.Load. Error format file."_a));
        }
        FontData = static_cast<PAftHeaderEC>(EC_Mem::AllocEC(SourceBuffer->DataSize));
        Windows::CopyMemory(FontData, SourceBuffer->Data, SourceBuffer->DataSize);
        Glyphs = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(FontData, static_cast<std::int32_t>(sizeof(TAftHeaderEC))));
        if (*static_cast<std::uint8_t*>(pas::byte_offset(&FontData->Magic, 0 * sizeof(std::uint8_t))) != 'a' || *static_cast<std::uint8_t*>(pas::byte_offset(&FontData->Magic, 1 * sizeof(std::uint8_t))) != 'f' || *static_cast<std::uint8_t*>(pas::byte_offset(&FontData->Magic, 2 * sizeof(std::uint8_t))) != 't') {
            pas::raise(pas::make_exception<pas::Exception>("TCFontEC.Load. Error format file."_a));
        }
        if (FontData->Version != 1) {
            pas::raise(pas::make_exception<pas::Exception>("TCFontEC.Load. Unknown version of font."_a));
        }
        GlyphCount = FontData->GlyphCount;
        GlyphLookup = static_cast<PFontGlyphLookupEC>(EC_Mem::AllocClearEC(static_cast<std::int32_t>(sizeof(TFontGlyphLookupEC))));
        AboveBaseline = 0;
        BelowBaseline = 0;
        MaxGlyphAdvance = 0;
        PAftGlyphEC Glyph = Glyphs;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, GlyphCount - 1); cpp_range.next(Index); ) {
            EC_Mem::WriteWordEC(EC_Mem::AddPointerOffset(GlyphLookup, Glyph->CharCode * 2), static_cast<std::uint16_t>(Index) + 1);
            if (Glyph->OpaqueMaskPlane.DataOffset != 0) {
                if (Glyph->OpaqueMaskPlane.Top <= 0 && -Glyph->OpaqueMaskPlane.Top + 1 > AboveBaseline) {
                    AboveBaseline = -Glyph->OpaqueMaskPlane.Top + 1;
                }
                if (Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height > 0 && Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height - 1 > BelowBaseline) {
                    BelowBaseline = Glyph->OpaqueMaskPlane.Top + Glyph->OpaqueMaskPlane.Height - 1;
                }
            }
            if (Glyph->AlphaMaskPlane.DataOffset != 0) {
                if (Glyph->AlphaMaskPlane.Top <= 0 && -Glyph->AlphaMaskPlane.Top + 1 > AboveBaseline) {
                    AboveBaseline = -Glyph->AlphaMaskPlane.Top + 1;
                }
                if (Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height > 0 && Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height - 1 > BelowBaseline) {
                    BelowBaseline = Glyph->AlphaMaskPlane.Top + Glyph->AlphaMaskPlane.Height - 1;
                }
            }
            MaxGlyphAdvance = std::max<std::int32_t>(MaxGlyphAdvance, Glyph->AdvanceA + Glyph->AdvanceB + Glyph->AdvanceC);
            Glyph = static_cast<PAftGlyphEC>(EC_Mem::AddPointerOffset(Glyph, static_cast<std::int32_t>(sizeof(TAftGlyphEC))));
        }
        ResidentBytes = 0;
    }

    // Zero dimensions use measured text size. ActualSize may be nil. Enables UseARGBColors on the shared font.
    void TCFontEC::RenderTaggedTextToTexture(const pas::WideString& Text, std::int32_t Width, std::int32_t Height, std::int32_t AlignX, std::int32_t AlignY, std::uint8_t WordWrap, WindowsSdk::PPoint ActualSize, Direct3D9::IDirect3DTexture9& Texture) {
        Direct3D9::IDirect3DTexture9 cpp_result{};
        std::int32_t TopAdjustment{};
        std::int32_t CurrentY{};
        EC_Str::TStringsEC* Lines{};
        WindowsSdk::TRect Bounds{};
        EC_Str::TStringsEC* WrappedLines{};
        std::int32_t DrawX{};
        std::int32_t DrawY{};
        std::uint8_t SavedColorTags{};
        std::uint8_t CenteredTabs{};
        WindowsSdk::TPoint ImageSize{};
        Direct3D9::IDirect3DTexture9 Image{};
        Direct3D9::TD3DLockedRect Locked{};
        WindowsSdk::TRect Clip{};
        auto MeasureFontTextureTextSize = [&]() -> WindowsSdk::TPoint {
            WindowsSdk::TPoint Result{};
            std::uint8_t First{};
            WindowsSdk::TRect MergedBounds{};
            MergedBounds.Left = 0;
            MergedBounds.Right = 0;
            MergedBounds.Top = 0;
            MergedBounds.Bottom = 0;
            CurrentY = 0;
            std::int32_t ExtraHeight = 0;
            Lines->First();
            if (!WordWrap) {
                if (!Lines->IsAtEnd()) {
                    MergedBounds = MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, CurrentY, &TopAdjustment);
                    CurrentY += GetLineHeight();
                    Lines->Next();
                }
                while (!Lines->IsAtEnd()) {
                    Bounds = MeasureTaggedTextBounds(Lines->GetCurrentText(), 0, CurrentY, nullptr);
                    WindowsSdk::UnionRect(MergedBounds, MergedBounds, Bounds);
                    CurrentY += GetLineHeight();
                    Lines->Next();
                }
            } else {
                First = true;
                while (!Lines->IsAtEnd()) {
                    WrapTaggedTextIntoLines(WrappedLines, Lines->GetCurrentText(), Width - 4);
                    if (!WrappedLines->IsEmpty()) {
                        WrappedLines->First();
                        if (First) {
                            MergedBounds = MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, CurrentY, &TopAdjustment);
                            First = false;
                            CurrentY += GetLineHeight();
                            WrappedLines->Next();
                        }
                        while (!WrappedLines->IsAtEnd()) {
                            Bounds = MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, CurrentY, nullptr);
                            WindowsSdk::UnionRect(MergedBounds, MergedBounds, Bounds);
                            CurrentY += GetLineHeight();
                            WrappedLines->Next();
                        }
                    }
                    Lines->Next();
                }
                ExtraHeight = 2;
            }
            Result = ClassesImports::Point(MergedBounds.Right - MergedBounds.Left, ExtraHeight + MergedBounds.Bottom - MergedBounds.Top);
            return Result;
        };
        Lines = nullptr;
        WrappedLines = nullptr;
        Image = nullptr;
        {
            std::exception_ptr cpp_error{};
            try {
                Lines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                Lines->SetText(Text);
                UseARGBColors = true;
                SavedColorTags = ColorTagsEnabled;
                ColorTagsEnabled = false;
                CenteredTabs = AlignX == 1 && pas::pos(u"<td=", Text) > 0;
                if (WordWrap) {
                    WrappedLines = pas::construct_call<EC_Str::TStringsEC>(EC_Str::TStringsEC_Create);
                }
                if (Width == 0 && WordWrap) {
                    GR_Main::RaiseWideMessage(u"draw text to texture 1"_wref.get());
                }
                ImageSize = MeasureFontTextureTextSize();
                if (Width == 0) {
                    Width = ImageSize.X;
                }
                if (Height == 0) {
                    Height = ImageSize.Y;
                }
                DrawX = 0;
                DrawY = 0;
                if (AlignX == 0 || WordWrap || CenteredTabs) {
                    DrawX = 2;
                } else if (AlignX == 2) {
                    DrawX = Width - ImageSize.X - 2;
                } else if (AlignX == 1) {
                    DrawX = Width / 2 - ImageSize.X / 2;
                } else if (AlignX == 3) {
                    DrawX = 2;
                    Width = ImageSize.X + 4;
                }
                if (AlignY == 0) {
                    DrawY = TopAdjustment + 2;
                } else if (AlignY == 3) {
                    DrawY = Height - ImageSize.Y - 2 + TopAdjustment;
                } else if (AlignY == 1) {
                    DrawY = Height / 2 - ImageSize.Y / 2 + TopAdjustment;
                } else if (AlignY == 2) {
                    DrawY = Height / 2 - ImageSize.Y / 2 + TopAdjustment;
                } else if (AlignY == 4) {
                    DrawY = TopAdjustment + 2;
                    Height = ImageSize.Y + 4;
                }
                Clip = ClassesImports::Rect(0, 0, Width, Height);
                Image = (GR_DX::GR_CreateTexture(Width, Height, Direct3D9::D3DFMT_A8R8G8B8, Direct3D9::D3DPOOL_MANAGED, cpp_result), cpp_result);
                if (Image == nullptr) {
                    GR_Main::RaiseWideMessage(u"draw text to texture 2"_wref.get());
                }
                Direct3D9::IDirect3DTexture9_LockRect(Image, 0u, Locked, nullptr, 0u);
                ColorTagsEnabled = SavedColorTags;
                if (!WordWrap) {
                    if (AlignY == 2) {
                        CurrentY = Height / 2 - (GetLineHeight() * (Lines->GetCount() - 1) + GetCenteringHeight()) / 2 + GetCenteringHeight();
                    } else {
                        CurrentY = AboveBaseline + DrawY - 2;
                    }
                    Lines->First();
                    while (!Lines->IsAtEnd()) {
                        DrawTaggedText32(Locked.Bits, Locked.Pitch, DrawX, CurrentY, Lines->GetCurrentText(), Clip);
                        CurrentY += GetLineHeight();
                        Lines->Next();
                    }
                } else {
                    CurrentY = AboveBaseline + DrawY - 2;
                    Lines->First();
                    while (!Lines->IsAtEnd()) {
                        WrapTaggedTextIntoLines(WrappedLines, Lines->GetCurrentText(), Width - 4);
                        WrappedLines->First();
                        while (!WrappedLines->IsAtEnd()) {
                            if (AlignX == 0 || CenteredTabs) {
                                DrawTaggedText32(Locked.Bits, Locked.Pitch, DrawX, CurrentY, WrappedLines->GetCurrentText(), Clip);
                            } else if (AlignX == 2) {
                                Bounds = MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, 0, nullptr);
                                DrawTaggedText32(Locked.Bits, Locked.Pitch, Width - (Bounds.Right - Bounds.Left), CurrentY, WrappedLines->GetCurrentText(), Clip);
                            } else if (AlignX == 1) {
                                Bounds = MeasureTaggedTextBounds(WrappedLines->GetCurrentText(), 0, 0, nullptr);
                                DrawTaggedText32(Locked.Bits, Locked.Pitch, Width / 2 - (Bounds.Right - Bounds.Left) / 2, CurrentY, WrappedLines->GetCurrentText(), Clip);
                            } else if (AlignX == 3 && static_cast<std::uint8_t>(WrappedLines->IsAtLast() ^ 1)) {
                                EC_CacheFont::TCFontEC_DrawJustifiedTaggedText32(this, Locked.Bits, Locked.Pitch, DrawX, CurrentY, WrappedLines->GetCurrentText(), Width - 4, Clip);
                            } else {
                                DrawTaggedText32(Locked.Bits, Locked.Pitch, DrawX, CurrentY, WrappedLines->GetCurrentText(), Clip);
                            }
                            CurrentY += GetLineHeight();
                            WrappedLines->Next();
                        }
                        Lines->Next();
                    }
                }
                Direct3D9::IDirect3DTexture9_UnlockRect(Image, 0u);
            } catch (...) {
                cpp_error = std::current_exception();
            }
            if (Lines != nullptr) {
                pas::free(Lines);
            }
            if (WrappedLines != nullptr) {
                pas::free(WrappedLines);
            }
            if (cpp_error) {
                std::rethrow_exception(cpp_error);
            }
        }
        if (ActualSize != nullptr) {
            pas::store_unaligned<Types::TPoint>(ActualSize, ClassesImports::Point(Width, Height));
        }
        Texture = Image;
    }

    void TCFontEC::p_destroy() {
        EC_CacheFont::TCFontEC_Destroy(this);
    }

} // namespace EC_CacheFont
