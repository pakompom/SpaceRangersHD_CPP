#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"
#include "types/Types.hpp"

namespace aVector {
    struct TPolygonEdge;

    struct TRectF;

    struct TPolygon2D;

    #pragma pack(push, 1)
    struct TRectF {
        float Left;
        float Top;
        float Right;
        float Bottom;
    };
    #pragma pack(pop)

    using PPolygonEdge = TPolygonEdge*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TPolygon2D : pas::Object {
        PAS_CLASS_META(TPolygon2D, pas::Object, "TPolygon2D", 60)
        void p_destroy() override;
        void Clear();
        void SetRectangle(Types::TRect Rect);
        // Takes ownership of the list and its PPointF entries.
        void TakePoints(pas::List* NewPoints);
        void SetTriangle(EC_Struct::TPointF A, EC_Struct::TPointF B, EC_Struct::TPointF C);
        void RecalculateBounds();
        // Appends at the tail and sets Polygon.Previous; requires nonnil Polygon.
        void Append(TPolygon2D* Polygon);
        void InsertAfter(TPolygon2D* Polygon);
        void SplitChainByLine(float A, float B, float C);
        void SplitChainByPoints(EC_Struct::TPointF First, EC_Struct::TPointF Last);
        TPolygon2D* ExtractFollowingGroup(std::int32_t Id);
        std::uint8_t ContainsPoint(EC_Struct::TPointF Point);
        std::uint8_t ChainContainsPoint(EC_Struct::TPointF Point);
        TPolygon2D* FindContainingPolygon(EC_Struct::TPointF Point);
        std::uint8_t AssignGroupAtPoint(EC_Struct::TPointF Point, std::int32_t Id);
        pas::List* ExtractBoundaryEdges();
        // Consumes both lists and frees their edge records.
        static pas::List* MergeUnsharedEdges(pas::List* First, pas::List* Second);
        // Caller owns the list and its PPolygonEdge entries.
        pas::List* ExtractEdges();
        void ResetChainGroups();
        // Includes Self; nil returns zero.
        std::int32_t CountChain();
        TPolygon2D* GetChainItem(std::int32_t Index);
        // The first uncached call fills CachedArea but returns zero; later calls return the cache.
        float GetArea();
        float GetChainArea();
        std::uint8_t IntersectsPolygon(TPolygon2D* Polygon);
        std::uint8_t IntersectsEdge(PPolygonEdge Edge);
        std::uint8_t IntersectsSegment(EC_Struct::TPointF First, EC_Struct::TPointF Last);
        std::uint8_t ChainSelfIntersects();
        std::uint8_t IntersectsChain(TPolygon2D* Polygon);
        TPolygon2D* Next;
        TPolygon2D* Previous;
        // Owns PPointF entries.
        pas::List* Points;
        std::int32_t GroupId;
        // Reset to -1; other meaning unresolved.
        std::int32_t Unknown14;
        EC_Struct::TPointF Extent;
        float CachedArea;
        std::uint8_t AreaValid;
        std::uint8_t cpp_padding[3];
        TRectF Bounds;
        std::uint8_t cpp_padding_2[1];
        // Cleared on geometry changes; other meaning unresolved.
        std::uint8_t Flag39;
        std::uint8_t cpp_padding_3[2];
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TPolygonEdge {
        EC_Struct::TPointF First;
        EC_Struct::TPointF Last;
        std::uint8_t cpp_padding[12];
    };
    #pragma pack(pop)

} // namespace aVector
