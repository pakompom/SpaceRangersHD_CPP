#pragma once
#include "types/EC_Str.hpp"

namespace EC_Str {
    extern const EC_Str::THexDigits HexDigits;

    std::int32_t CountDelimitedPartsW(const pas::WideString& Text, const pas::WideString& Delimiters);

    std::int32_t GetDelimitedPartStartIndexW(const pas::WideString& Text, std::int32_t PartIndex, const pas::WideString& Delimiters);

    std::int32_t GetCharDelimitedPartStartIndexW(const pas::WideString& Text, std::int32_t PartIndex, char16_t Delimiter);

    std::int32_t GetDelimitedPartLengthW(const pas::WideString& Text, std::int32_t StartIndex, const pas::WideString& Delimiters);

    pas::WideString ExtractDelimitedPartW(const pas::WideString& Text, std::int32_t PartIndex, const pas::WideString& Delimiters);

    pas::WideString ExtractDelimitedRangeW(const pas::WideString& Text, std::int32_t FirstPart, std::int32_t LastPart, const pas::WideString& Delimiters);

    pas::WideString ExtractNextDelimitedPartW(pas::WideString& Text, char16_t Delimiter);

    pas::WideString ExtractLineCommentW(const pas::WideString& Text);

    pas::WideString RemoveLineCommentW(const pas::WideString& Text);

    pas::WideString ReplaceAllWideString(const pas::WideString& Text, const pas::WideString& Search, const pas::WideString& Replacement);

    std::int32_t FindTextOffsetW(const pas::WideString& Text, const pas::WideString& Search, std::int32_t StartIndex);

    std::int32_t FindTextPosW(const pas::WideString& Search, const pas::WideString& Text);

    std::int32_t ExtractDigitsToIntW(const pas::WideString& Text);

    std::uint8_t IsIntegerTextW(const pas::WideString& Text);

    std::int32_t ExtractSignedDigitsToIntW(const pas::WideString& Text);

    float ExtractDecimalToSingleW(const pas::WideString& Text);

    float ParseDecimalToSingleW(const pas::WideString& Text);

    pas::WideString FloatToWideString(double Value);

    pas::WideString CardinalToHexWideString(std::uint32_t Value);

    pas::WideString IntToFixedWidthWideString(std::int32_t Value, std::int32_t Width);

    pas::WideString IntToWideString(std::int32_t Value);

    pas::WideString BoolToWideString(std::uint8_t Value);

    pas::WideString TrimWideString(const pas::WideString& Text);

    pas::WideString UpperCaseWideString(const pas::WideString& Text);

    pas::WideString LowerCaseWideString(const pas::WideString& Text);

    pas::WideString RemoveWideStringChars(const pas::WideString& Text, pas::WideString Chars);

    std::int32_t GetTextTagLengthW(char16_t* Text, std::int32_t CharCount);

    std::uint8_t MatchTextTagPrefixW(char16_t* Text, std::int32_t CharCount, const pas::WideString& Pattern, const pas::WideString& AlternatePattern);

    pas::WideString RemoveTextTagsW(const pas::WideString& Text);

    pas::WideString RemoveMatchingTextTagsW(pas::WideString Text, const pas::WideString& Pattern, const pas::WideString& AlternatePattern);

    inline std::int32_t CompareWideChars(char16_t* Left, char16_t* Right);

    pas::WideString ExtractFileNameNoExtW(const pas::WideString& Path);

    pas::WideString ExtractFileExtNoDotW(const pas::WideString& Path);

    pas::WideString ExtractFileDirW(const pas::WideString& Path);

    pas::WideString DecodeTextW(pas::WideString Text);

    pas::WideString CopyWideStringUnchecked(pas::WideString Text, std::int32_t Index, std::int32_t Count);

    void WriteRegistryStringLegacy(std::uint32_t RootKey, pas::WideString KeyPath, pas::WideString ValueName, pas::WideString Value);

    pas::WideString EncodeTextW(pas::WideString Text);

    pas::WideString TransliterateCyrillicToLatin(pas::WideString Text);

    void TStringsEC_Create(TStringsEC* Self);

    void TStringsEC_Destroy(TStringsEC* Self);

} // namespace EC_Str

#include "inline/EC_Str.hpp"
