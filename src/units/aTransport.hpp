#pragma once
#include "types/aTransport.hpp"

namespace aTransport {
    extern pas::Array<std::int32_t, 22, 27> TransportSkillBonusWeights;

    extern pas::Array<std::int32_t, 13, 20> TransportSlotBonusWeights;

    void TTransport_Destroy(TTransport* Self);

} // namespace aTransport
