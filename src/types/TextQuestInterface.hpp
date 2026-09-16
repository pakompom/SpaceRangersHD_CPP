#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace TextQuestInterface {
    struct TTextQuestInterface;

    enum TQuestOutcome : std::uint32_t {
        qoNone = 0,
        qoFailure = 1,
        qoSuccess = 2,
        qoDeath = 3,
    };

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TTextQuestInterface : EC_Struct::TObjectEx {
        PAS_CLASS_META(TTextQuestInterface, EC_Struct::TObjectEx, "TTextQuestInterface", 4)
        void p_destroy() override;
        virtual void ShowText(pas::WideString Text);
        virtual void ShowPicture(pas::WideString Name);
        virtual void PlayMusic(pas::WideString Name);
        virtual void PlaySound(pas::WideString Name);
        virtual void ShowParameters(pas::WideString Text);
        virtual void AddContinueAction();
        virtual void AddSuccessAction();
        virtual void AddDeathAction();
        virtual void AddFailureAction();
        virtual void AddPathAction(pas::WideString Text, std::int32_t PathId);
        virtual void AddDisabledPath(pas::WideString Text);
        virtual void AddPathContinueAction(std::int32_t PathId);
        virtual void AddLocationContinueAction(std::int32_t LocationId);
        virtual void AdvanceDays(std::int32_t Days);
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace TextQuestInterface
