#include "layout/MessageText.hpp"
#include "units/EC_BlockPar.hpp"
#include "units/EC_Str.hpp"
#include "units/MessageText.hpp"

namespace MessageText {
    MessageText::TQuestMessages* QuestMessages{};

    void TQuestMessages_Create(TQuestMessages* Self) {
        pas::object_create(Self);
        Self->Entries = pas::construct_call<EC_BlockPar::TBlockParEC>(EC_BlockPar::TBlockParEC_Create);
    }

    void TQuestMessages_Destroy(TQuestMessages* Self) {
        if (Self->Entries != nullptr) {
            pas::free(Self->Entries);
            Self->Entries = nullptr;
        }
        pas::object_destroy(Self);
    }

    pas::WideString TQuestMessages::GetText(pas::WideString Path) {
        pas::WideString Result{};
        std::int32_t i{};
        pas::WideString Name{};
        Result = Path;
        std::int32_t PartCount = EC_Str::CountDelimitedPartsW(Path, u"."_wref.get());
        EC_BlockPar::TBlockParEC* Block = Entries;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PartCount - 2); cpp_range.next(i); ) {
            Name = EC_Str::ExtractDelimitedPartW(Path, i, u"."_wref.get());
            if (Block->CountBlocks(Name) <= 0) {
                return Result;
            }
            Block = Block->GetBlock(Name);
        }
        Name = EC_Str::ExtractDelimitedPartW(Path, PartCount - 1, u"."_wref.get());
        return Block->GetParam(Name);
    }

    pas::WideString TQuestMessages::GetTextOrKey(pas::WideString Key) {
        if (Entries->CountParams(Key) > 0) {
            return Entries->GetParam(Key);
        }
        return Key;
    }

    void TQuestMessages::p_destroy() {
        MessageText::TQuestMessages_Destroy(this);
    }

} // namespace MessageText
