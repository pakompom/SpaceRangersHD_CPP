#pragma once
#include "types/EC_Str.hpp"

namespace EC_Str {
    extern const EC_Str::THexDigits HexDigits;

    // Delimiters is a set of separator characters, not a substring. Counts empty parts; empty Text returns zero.
    std::int32_t CountDelimitedPartsW(const pas::WideString& Text, const pas::WideString& Delimiters);

    // Zero-based part index, one-based character result. Nonpositive PartIndex returns 1; missing positive indexes raise.
    std::int32_t GetDelimitedPartStartIndexW(const pas::WideString& Text, std::int32_t PartIndex, const pas::WideString& Delimiters);

    // One-based character result. Nonpositive PartIndex returns 1; 1 returns the position after the first delimiter or -1. Native early exit makes every PartIndex above 1 return -1.
    std::int32_t GetCharDelimitedPartStartIndexW(const pas::WideString& Text, std::int32_t PartIndex, char16_t Delimiter);

    // StartIndex is a one-based character position, not a part index.
    std::int32_t GetDelimitedPartLengthW(const pas::WideString& Text, std::int32_t StartIndex, const pas::WideString& Delimiters);

    pas::WideString ExtractDelimitedPartW(const pas::WideString& Text, std::int32_t PartIndex, const pas::WideString& Delimiters);

    // Includes both zero-based part indexes and the separators between them.
    pas::WideString ExtractDelimitedRangeW(const pas::WideString& Text, std::int32_t FirstPart, std::int32_t LastPart, const pas::WideString& Delimiters);

    // Removes the returned prefix and first delimiter from Text; without a delimiter returns all of Text and clears it.
    pas::WideString ExtractNextDelimitedPartW(pas::WideString& Text, char16_t Delimiter);

    // Returns the first // and following text, including immediately preceding spaces, tabs, CR and LF. Empty when absent; does not recognize quoting.
    pas::WideString ExtractLineCommentW(const pas::WideString& Text);

    // Removes the first // and following text, then trims trailing characters <= #32. Without // returns Text unchanged; does not recognize quoting.
    pas::WideString RemoveLineCommentW(const pas::WideString& Text);

    // Case-sensitive, non-overlapping replacement; empty Search returns Text unchanged.
    pas::WideString ReplaceAllWideString(const pas::WideString& Text, const pas::WideString& Search, const pas::WideString& Replacement);

    // Zero-based start and result; starts at a nonnegative character offset and returns -1 when absent.
    std::int32_t FindTextOffsetW(const pas::WideString& Text, const pas::WideString& Search, std::int32_t StartIndex);

    // One-based result, with Search before Text as in Pos; returns zero when absent.
    std::int32_t FindTextPosW(const pas::WideString& Search, const pas::WideString& Text);

    // Ignores signs and other nondigits; unchecked 32-bit arithmetic.
    std::int32_t ExtractDigitsToIntW(const pas::WideString& Text);

    // True for any nonempty string containing only digits and minus signs, including '-' and '1--2'; does not validate numeric syntax or range.
    std::uint8_t IsIntegerTextW(const pas::WideString& Text);

    // Ignores nondigits; a minus sign encountered while the accumulated value is zero makes the result negative. Unchecked 32-bit arithmetic.
    std::int32_t ExtractSignedDigitsToIntW(const pas::WideString& Text);

    // Accepts '.' or ','; ignores other nondigits and treats any '-' as negative. No exponent syntax.
    float ExtractDecimalToSingleW(const pas::WideString& Text);

    // Same permissive conversion as ExtractDecimalToSingleW; all accumulation and the result use Single precision.
    float ParseDecimalToSingleW(const pas::WideString& Text);

    // Uses a decimal point by temporarily changing the RTL's global separator; not thread-safe, and an exception can leave the separator changed.
    pas::WideString FloatToWideString(double Value);

    // Lowercase hexadecimal without a prefix or padding; zero becomes '0'.
    pas::WideString CardinalToHexWideString(std::uint32_t Value);

    // Left-pads with zeros or keeps only the leftmost Width digits. Nonpositive Value produces zeros; nonpositive Width produces an empty string.
    pas::WideString IntToFixedWidthWideString(std::int32_t Value, std::int32_t Width);

    // Low(Integer) incorrectly produces '-0'.
    pas::WideString IntToWideString(std::int32_t Value);

    // Returns 'True' or 'False'.
    pas::WideString BoolToWideString(std::uint8_t Value);

    // Trims only spaces, tabs, CR, LF and NUL characters at both ends.
    pas::WideString TrimWideString(const pas::WideString& Text);

    // Uses the language CaseConv table; characters absent from it remain unchanged.
    pas::WideString UpperCaseWideString(const pas::WideString& Text);

    // Uses the language CaseConv table in reverse; characters absent from it remain unchanged.
    pas::WideString LowerCaseWideString(const pas::WideString& Text);

    // Chars is a set of individual characters, not a substring.
    pas::WideString RemoveWideStringChars(const pas::WideString& Text, pas::WideString Chars);

    // Returns the leading <...> token length, 1 for leading <<, or zero when no complete tag is present.
    std::int32_t GetTextTagLengthW(char16_t* Text, std::int32_t CharCount);

    // Requires leading < and equal-length patterns. Each character may match either pattern; no closing > or name boundary is required.
    std::uint8_t MatchTextTagPrefixW(char16_t* Text, std::int32_t CharCount, const pas::WideString& Pattern, const pas::WideString& AlternatePattern);

    // Removes complete <...> tokens; leading << consumes one character and scanning resumes at the second <. Incomplete tags remain.
    pas::WideString RemoveTextTagsW(const pas::WideString& Text);

    // Uses MatchTextTagPrefixW; opening and closing tags require separate patterns.
    pas::WideString RemoveMatchingTextTagsW(pas::WideString Text, const pas::WideString& Pattern, const pas::WideString& AlternatePattern);

    // Case-sensitive NUL-terminated comparison returning -1, 0 or 1. Nil sorts before every nonnil pointer, including an empty string.
    inline std::int32_t CompareWideChars(char16_t* Left, char16_t* Right);

    // Original unit ownership of these standalone helpers is unresolved.
    // Accepts slash and backslash; strips only the final dot and suffix from the last path component.
    pas::WideString ExtractFileNameNoExtW(const pas::WideString& Path);

    // Accepts slash and backslash; returns text after the last dot in the final component, or empty when absent.
    pas::WideString ExtractFileExtNoDotW(const pas::WideString& Path);

    // Accepts slash and backslash; excludes the final separator and component.
    pas::WideString ExtractFileDirW(const pas::WideString& Path);

    // Game text obfuscation: EncodeTextW inserts a random character after each input
    // character; DecodeTextW discards those interleaved characters.
    // Keeps characters 1, 3, 5, ... using Delphi's one-based string indexing.
    pas::WideString DecodeTextW(pas::WideString Text);

    // One-based Index; unlike the RTL Copy helper, does not clamp Index or Count to the source. Requires a valid source span and nonnegative Count.
    pas::WideString CopyWideStringUnchecked(pas::WideString Text, std::int32_t Index, std::int32_t Count);

    // Creates with KEY_WRITE. Passes an ANSI-converted buffer and ANSI byte count to RegSetValueExW; preserves this native encoding mismatch.
    void WriteRegistryStringLegacy(std::uint32_t RootKey, pas::WideString KeyPath, pas::WideString ValueName, pas::WideString Value);

    // Inserts a random language-table character after each input character; requires a nonempty WideCaseTable.
    pas::WideString EncodeTextW(pas::WideString Text);

    // Applies the native ordered replacement table, including its unusual letter mappings.
    pas::WideString TransliterateCyrillicToLatin(pas::WideString Text);

    void TStringsEC_Create(TStringsEC* Self);

    void TStringsEC_Destroy(TStringsEC* Self);

} // namespace EC_Str

#include "inline/EC_Str.hpp"
