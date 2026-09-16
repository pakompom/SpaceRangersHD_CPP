#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"

namespace GR_Rect {
    struct TRectGR;

    struct TArrayRectGR;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TArrayRectGR : EC_Struct::TObjectEx {
        PAS_CLASS_META(TArrayRectGR, EC_Struct::TObjectEx, "TArrayRectGR", 12)
        void p_destroy() override;
        void Clear();
        TRectGR* AllocateRectNode();
        void RemoveRectNode(TRectGR* RectNode);
        // Maintains nonoverlapping coverage.
        void AddRect(Types::TRect Rect);
        void InsertRectFragment(std::int32_t Left, std::int32_t Top, std::int32_t Right, std::int32_t Bottom);
        void AddScreenClippedRect(Types::TRect Rect, Types::TPoint UnusedPoint1, Types::TPoint UnusedPoint2);
        TRectGR* FirstRect;
        TRectGR* LastRect;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TRectGR : pas::Object {
        PAS_CLASS_META(TRectGR, pas::Object, "TRectGR", 28)
        void p_destroy() override;
        TRectGR* Prev;
        TRectGR* Next;
        Types::TRect Bounds;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

} // namespace GR_Rect
