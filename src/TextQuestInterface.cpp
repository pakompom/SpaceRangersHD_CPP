#include "layout/TextQuestInterface.hpp"
#include "units/TextQuestInterface.hpp"

namespace TextQuestInterface {
    void TTextQuestInterface_Create(TTextQuestInterface* Self) {
    }

    void TTextQuestInterface_Destroy(TTextQuestInterface* Self) {
    }

    void TTextQuestInterface::ShowText(pas::WideString Text) {
    }

    void TTextQuestInterface::ShowPicture(pas::WideString Name) {
    }

    void TTextQuestInterface::PlayMusic(pas::WideString Name) {
    }

    void TTextQuestInterface::PlaySound(pas::WideString Name) {
    }

    void TTextQuestInterface::ShowParameters(pas::WideString Text) {
    }

    void TTextQuestInterface::AddContinueAction() {
    }

    void TTextQuestInterface::AddSuccessAction() {
    }

    void TTextQuestInterface::AddDeathAction() {
    }

    void TTextQuestInterface::AddFailureAction() {
    }

    void TTextQuestInterface::AddPathAction(pas::WideString Text, std::int32_t PathId) {
    }

    void TTextQuestInterface::AddDisabledPath(pas::WideString Text) {
    }

    void TTextQuestInterface::AddPathContinueAction(std::int32_t PathId) {
    }

    void TTextQuestInterface::AddLocationContinueAction(std::int32_t LocationId) {
    }

    void TTextQuestInterface::AdvanceDays(std::int32_t Days) {
    }

    void TTextQuestInterface::p_destroy() {
        TextQuestInterface::TTextQuestInterface_Destroy(this);
    }

} // namespace TextQuestInterface
