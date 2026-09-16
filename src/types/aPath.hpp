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
        // Acquires 24 nodes from the shared pool; raises on allocation failure.
        void AllocateNodeUnit();
        // Increments NodeCount without linking into the active list. May allocate; payload is uninitialized.
        PSPathNode PopFreeNode();
        // Node must belong to this path; it is recycled.
        void RemoveNode(PSPathNode Node);
        // Inclusive range must be ordered and belong to this path; nodes are recycled.
        void RemoveNodeRange(PSPathNode FirstNode, PSPathNode LastNode);
        // Recycles active nodes into this path's free list.
        void Clear();
        // New node is ActiveTail; payload is uninitialized.
        void AppendNode();
        void AppendWaypoint(EC_Struct::TPointF Position, float Heading);
        // Nil appends. Payload is uninitialized.
        PSPathNode InsertNodeBefore(PSPathNode Node);
        // Starts at Node.Next; nonpositive SkipCount selects that immediate successor. Node must be non-nil.
        static PSPathNode GetFollowingNode(PSPathNode Node, std::int32_t SkipCount);
        // Excludes Node itself, which must be non-nil. Ties keep the earlier node.
        static PSPathNode FindNearestFollowingNode(PSPathNode Node, EC_Struct::TPointF Position);
        float GetLength();
        // Returns zero for nil endpoints or when LastNode is not reachable from FirstNode.
        static std::int32_t CountNodeRangeInclusive(PSPathNode FirstNode, PSPathNode LastNode);
        // Uses the inclusive nodes as Bezier controls, unwraps headings, inserts samples and recycles the controls.
        void ResampleBezierRange(PSPathNode FirstNode, PSPathNode LastNode, std::int32_t SampleCount);
        PSPathNode ActiveHead;
        PSPathNode ActiveTail;
        PSPathNode FreeHead;
        PSPathNode FreeTail;
        // Active nodes only.
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
