#pragma once
#include "types/aScript.hpp"
#include "types/ab_MainForm.hpp"

namespace aMyFunction {
    struct TObjectList;

} // namespace aMyFunction

namespace ab_MainForm {
    extern aMyFunction::TObjectList* ActiveArcadeRequestShips;

    extern aScript::PScriptABRequest ActiveArcadeRequest;

    void TfAB_Create(TfAB* Self);

    void TfAB_Destroy(TfAB* Self);

} // namespace ab_MainForm
