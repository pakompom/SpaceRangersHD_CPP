#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace SequenceClass {
    struct TSequence;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSequence : EC_Struct::TObjectEx {
        PAS_CLASS_META(TSequence, EC_Struct::TObjectEx, "TSequence", 20)
        void p_destroy() override;
        // Also updates every member's visit or traversal limit.
        void SetTraversalLimit(std::int32_t Value);
        // Propagates the minimum positive member limit, or zero if none.
        void RecomputeTraversalLimit();
        void AddLocation(void* Location);
        void AddPath(void* Path);
        void PrependPath(void* Path);
        // Initialized to zero; its purpose remains unresolved.
        std::uint8_t UnknownFlag;
        std::uint8_t cpp_padding[3];
        std::int32_t TraversalLimit;
        // // Owns the list, not its TLocation entries.
        pas::List* Locations;
        // // Owns the list, not its TPath entries.
        pas::List* Paths;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace SequenceClass
