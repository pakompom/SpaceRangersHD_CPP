#include "layout/ParameterDeltaClass.hpp"
#include "types/ParameterClass.hpp"
#include "types/TextFieldClass.hpp"
#include "types/TextQuestInterface.hpp"
#include "units/CalcParseClass.hpp"
#include "units/EC_Buf.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/EventClass.hpp"
#include "units/ParameterDeltaClass.hpp"
#include "units/System.hpp"
#include "units/ValueListClass.hpp"

namespace ParameterDeltaClass {
    void TParameterDelta_Create(TParameterDelta* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->CriticalEvent = pas::construct_call<EventClass::TEvent>(EventClass::TEvent_Create);
        Self->ValueConstraint = pas::construct_call<ValueListClass::TValuesList>(ValueListClass::TValuesList_Create);
        Self->MultipleConstraint = pas::construct_call<ValueListClass::TValuesList>(ValueListClass::TValuesList_Create);
        Self->ExpressionText = pas::construct_call<TextFieldClass::TTextField>(EC_Struct::TObjectEx_Create);
        Self->Reset();
    }

    void TParameterDelta_Destroy(TParameterDelta* Self) {
        Self->Reset();
        pas::free(Self->CriticalEvent);
        Self->CriticalEvent = nullptr;
        pas::free(Self->ValueConstraint);
        Self->ValueConstraint = nullptr;
        pas::free(Self->MultipleConstraint);
        Self->MultipleConstraint = nullptr;
        pas::free(Self->ExpressionText);
        Self->ExpressionText = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TParameterDelta::Reset() {
        ParameterIndex = 0;
        ClearValueConstraints();
        ClearChange();
    }

    void TParameterDelta::ClearValueConstraints() {
        MinValue = 0;
        MaxValue = 1;
        ValueConstraint->Clear();
        MultipleConstraint->Clear();
    }

    void TParameterDelta::ClearChange() {
        ChangeValue = 0;
        VisibilityChange = pvcUnchanged;
        CriticalEvent->ClearTextFields();
        LegacyFlag = false;
        ChangeByPercent = false;
        SetValue = false;
        UseExpression = false;
        ExpressionText->Text = pas::WideString();
    }

    std::uint8_t TParameterDelta::HasNoValueConstraint(pas::List* Parameters) {
        std::uint8_t Result = true;
        if (ParameterIndex <= 0 || pas::list_count(Parameters) < ParameterIndex) {
            return Result;
        }
        ParameterClass::TParameter* Parameter = pas::list_at<ParameterClass::TParameter>(Parameters, ParameterIndex - 1);
        Result = false;
        if (Parameter->GetNonCriticalMinimum() >= MinValue && Parameter->GetNonCriticalMaximum() <= MaxValue && ValueConstraint->Count <= 0 && MultipleConstraint->Count <= 0) {
            return true;
        }
        return Result;
    }

    std::uint8_t TParameterDelta::HasNoChange(pas::List* Parameters) {
        if (ParameterIndex <= 0 || pas::list_count(Parameters) < ParameterIndex) {
            return true;
        }
        std::uint8_t Result = false;
        if (VisibilityChange != pvcUnchanged) {
            return Result;
        }
        if (UseExpression) {
            if (EC_Str::TrimWideString(ExpressionText->Text) != u"") {
                return Result;
            }
        } else if (!(static_cast<std::uint8_t>(SetValue ^ 1) && ChangeValue == 0)) {
            return Result;
        }
        return true;
    }

    void TParameterDelta::EvaluateChangeExpression(pas::List*& Parameters) {
        pas::WideString Text{};
        CalcParseClass::TCalcParse* Calc{};
        ParameterClass::TParameter* Parameter{};
        if (ParameterIndex > 0 && pas::list_count(Parameters) >= ParameterIndex) {
            Parameter = pas::list_at<ParameterClass::TParameter>(Parameters, ParameterIndex - 1);
            if (Parameter->Enabled && UseExpression) {
                ChangeValue = Parameter->Value;
                Text = EC_Str::TrimWideString(ExpressionText->Text);
                if (Text != u"") {
                    Calc = pas::construct_call<CalcParseClass::TCalcParse>(CalcParseClass::TCalcParse_Create);
                    Calc->Expression = CalcParseClass::TCalcParse::NormalizeTokens(Text);
                    Calc->Evaluate(Parameters);
                    if (!Calc->HasError) {
                        ChangeValue = Calc->ResultValue;
                    }
                    pas::destroy(Calc);
                }
            }
        }
    }

    void TParameterDelta::ApplyChange(pas::List*& Parameters) {
        ParameterClass::TParameter* Parameter{};
        std::int32_t NewValue{};
        if (ParameterIndex > 0 && pas::list_count(Parameters) >= ParameterIndex) {
            Parameter = pas::list_at<ParameterClass::TParameter>(Parameters, ParameterIndex - 1);
            if (Parameter->Enabled) {
                if (UseExpression) {
                    NewValue = ChangeValue;
                } else if (SetValue) {
                    NewValue = ChangeValue;
                } else if (ChangeByPercent) {
                    NewValue = System::Round(Parameter->Value * 0.01L * ChangeValue) + Parameter->Value;
                } else {
                    NewValue = Parameter->Value + ChangeValue;
                }
                Parameter->SetValue(NewValue);
                if (Parameter->CriticalOutcome != TextQuestInterface::qoNone) {
                    if (EC_Str::TrimWideString(CriticalEvent->Text->Text) != u"") {
                        Parameter->CriticalEventOverride = CriticalEvent;
                    } else {
                        Parameter->CriticalEventOverride = nullptr;
                    }
                }
                if (VisibilityChange == pvcShow) {
                    Parameter->Hidden = false;
                } else if (VisibilityChange == pvcHide) {
                    Parameter->Hidden = true;
                }
            }
        }
    }

    std::uint8_t TParameterDelta::AcceptsParameter(pas::List* Parameters) {
        std::uint8_t Result = true;
        if (ParameterIndex <= 0 || pas::list_count(Parameters) < ParameterIndex) {
            return Result;
        }
        ParameterClass::TParameter* Parameter = pas::list_at<ParameterClass::TParameter>(Parameters, ParameterIndex - 1);
        if (Parameter->Enabled) {
            Result = false;
            if (Parameter->GetNonCriticalMaximum() > MaxValue && Parameter->Value > MaxValue) {
                return Result;
            }
            if (Parameter->GetNonCriticalMinimum() < MinValue && Parameter->Value < MinValue) {
                return Result;
            }
            if (!ValueConstraint->AcceptsValue(Parameter->Value)) {
                return Result;
            }
            if (!MultipleConstraint->AcceptsMultiple(Parameter->Value)) {
                return Result;
            }
            return true;
        }
        return Result;
    }

    void TParameterDelta::LoadLegacyV0FromReader(EC_Buf::TBufEC* Reader) {
        Reset();
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        ChangeValue = EC_Buf::TBufEC_GetInt32(Reader);
        VisibilityChange = static_cast<TParameterVisibilityChange>(EC_Buf::TBufEC_GetInt32(Reader));
        LegacyFlag = EC_Buf::TBufEC_GetBoolean(Reader);
        ChangeByPercent = EC_Buf::TBufEC_GetBoolean(Reader);
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
    }

    void TParameterDelta::LoadLegacyV1FromReader(EC_Buf::TBufEC* Reader) {
        Reset();
        EC_Buf::TBufEC_GetInt32(Reader);
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        ChangeValue = EC_Buf::TBufEC_GetInt32(Reader);
        VisibilityChange = static_cast<TParameterVisibilityChange>(EC_Buf::TBufEC_GetInt32(Reader));
        LegacyFlag = EC_Buf::TBufEC_GetBoolean(Reader);
        ChangeByPercent = EC_Buf::TBufEC_GetBoolean(Reader);
        ValueConstraint->LoadFromReader(Reader);
        MultipleConstraint->LoadFromReader(Reader);
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
    }

    void TParameterDelta::LoadLegacyV2FromReader(EC_Buf::TBufEC* Reader) {
        Reset();
        EC_Buf::TBufEC_GetInt32(Reader);
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        ChangeValue = EC_Buf::TBufEC_GetInt32(Reader);
        VisibilityChange = static_cast<TParameterVisibilityChange>(EC_Buf::TBufEC_GetInt32(Reader));
        LegacyFlag = EC_Buf::TBufEC_GetBoolean(Reader);
        ChangeByPercent = EC_Buf::TBufEC_GetBoolean(Reader);
        SetValue = EC_Buf::TBufEC_GetBoolean(Reader);
        ValueConstraint->LoadFromReader(Reader);
        MultipleConstraint->LoadFromReader(Reader);
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
    }

    void TParameterDelta::LoadLegacyV3FromReader(EC_Buf::TBufEC* Reader) {
        Reset();
        EC_Buf::TBufEC_GetInt32(Reader);
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        ChangeValue = EC_Buf::TBufEC_GetInt32(Reader);
        VisibilityChange = static_cast<TParameterVisibilityChange>(EC_Buf::TBufEC_GetInt32(Reader));
        LegacyFlag = EC_Buf::TBufEC_GetBoolean(Reader);
        ChangeByPercent = EC_Buf::TBufEC_GetBoolean(Reader);
        SetValue = EC_Buf::TBufEC_GetBoolean(Reader);
        UseExpression = EC_Buf::TBufEC_GetBoolean(Reader);
        ExpressionText->LoadTextLinesFromReader(Reader);
        ValueConstraint->LoadFromReader(Reader);
        MultipleConstraint->LoadFromReader(Reader);
        CriticalEvent->ClearTextFields();
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
    }

    void TParameterDelta::LoadValueConstraintsFromReader(EC_Buf::TBufEC* Reader) {
        ClearValueConstraints();
        MinValue = EC_Buf::TBufEC_GetInt32(Reader);
        MaxValue = EC_Buf::TBufEC_GetInt32(Reader);
        ValueConstraint->LoadFromReader(Reader);
        MultipleConstraint->LoadFromReader(Reader);
    }

    void TParameterDelta::LoadChangeFromReader(EC_Buf::TBufEC* Reader) {
        ClearChange();
        ChangeValue = EC_Buf::TBufEC_GetInt32(Reader);
        VisibilityChange = static_cast<TParameterVisibilityChange>(EC_Buf::TBufEC_GetByte(Reader));
        std::uint8_t ChangeKind = EC_Buf::TBufEC_GetByte(Reader);
        SetValue = ChangeKind == 0;
        ChangeByPercent = ChangeKind == 2;
        UseExpression = ChangeKind == 3;
        ExpressionText->LoadTextLinesFromReader(Reader);
        CriticalEvent->Text->LoadTextLinesFromReader(Reader);
        CriticalEvent->Picture->LoadTextLinesFromReader(Reader);
        CriticalEvent->Sound->LoadTextLinesFromReader(Reader);
        CriticalEvent->Music->LoadTextLinesFromReader(Reader);
    }

    void TParameterDelta::p_destroy() {
        ParameterDeltaClass::TParameterDelta_Destroy(this);
    }

} // namespace ParameterDeltaClass
