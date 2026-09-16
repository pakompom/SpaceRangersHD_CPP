#pragma once
#include "runtime_support.hpp"
#include "types/EC_Struct.hpp"

namespace aPath {
    struct TSPathNode;

    struct TSPath;

    using PSPathNode = TSPathNode*;

    #if INTPTR_MAX == INT32_MAX
    #pragma pack(push, 4)
    #endif
    struct TSPath : pas::Object {
        PAS_CLASS_META(TSPath, pas::Object, "TSPath", 24)
        void p_destroy() override;
        void AllocateNodeUnit();
        PSPathNode PopFreeNode();
        void RemoveNode(PSPathNode Node);
        void RemoveNodeRange(PSPathNode FirstNode, PSPathNode LastNode);
        void Clear();
        void AppendNode();
        void AppendWaypoint(EC_Struct::TPointF Position, float Heading);
        PSPathNode InsertNodeBefore(PSPathNode Node);
        static PSPathNode GetFollowingNode(PSPathNode Node, std::int32_t SkipCount);
        static PSPathNode FindNearestFollowingNode(PSPathNode Node, EC_Struct::TPointF Position);
        float GetLength();
        static std::int32_t CountNodeRangeInclusive(PSPathNode FirstNode, PSPathNode LastNode);
        void ResampleBezierRange(PSPathNode FirstNode, PSPathNode LastNode, std::int32_t SampleCount);
        PSPathNode ActiveHead;
        PSPathNode ActiveTail;
        PSPathNode FreeHead;
        PSPathNode FreeTail;
        std::int32_t NodeCount;
    };
    #if INTPTR_MAX == INT32_MAX
    #pragma pack(pop)
    #endif

    #pragma pack(push, 1)
    struct TSPathNode {
        PSPathNode Prev;
        PSPathNode Next;
        EC_Struct::TPointF Position;
        float Heading;
    };
    #pragma pack(pop)

} // namespace aPath
