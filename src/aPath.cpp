#include "layout/aPath.hpp"
#include "bezier24.hpp"
#include "types/EC_Struct.hpp"
#include "types/SystemImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/GR_Main.hpp"
#include "units/Math.hpp"
#include "units/SysUtils.hpp"
#include "units/WindowsImports.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aMyFunction.hpp"
#include "units/aPath.hpp"

namespace aPath {
    std::uint32_t PathNodeHeap = 0u;

    aPath::PSPathNode PathPoolHead = nullptr;

    aPath::PSPathNode PathPoolTail = nullptr;

    pas::CriticalSection* PathPoolLock = nullptr;

    std::int32_t PathGrowthBlockCount = -1;

    void* PathInitialBlock{};

    std::int32_t PathPoolFreeCount{};

    pas::DynArray<void*> PathGrowthBlocks{};

    void InitializePathNodePool() {
        std::int32_t Index{};
        PathPoolLock = pas::make_critical_section<pas::CriticalSection>();
        PathPoolFreeCount = 200000;
        PathNodeHeap = WindowsSdk::HeapCreate(0u, 16u, 0u);
        PathInitialBlock = WindowsSdk::HeapAlloc(PathNodeHeap, 0u,
                PathPoolFreeCount * static_cast<std::int32_t>(sizeof(TSPathNode)));
        if (PathInitialBlock == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("Error: HeapAlloc"_a));
        }
        WindowsImports::ZeroMemory(PathInitialBlock, PathPoolFreeCount * static_cast<std::int32_t>(sizeof(TSPathNode)));
        PSPathNode Node = static_cast<PSPathNode>(PathInitialBlock);
        PathPoolHead = Node;
        PSPathNode Prev = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PathPoolFreeCount - 1); cpp_range.next(Index); ) {
            Node->Prev = Prev;
            Node->Next = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node,
                    static_cast<std::int32_t>(sizeof(TSPathNode))));
            Prev = Node;
            Node = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node,
                    static_cast<std::int32_t>(sizeof(TSPathNode))));
        }
        PathPoolTail = Prev;
        PathPoolTail->Next = nullptr;
    }

    void EnsurePathGrowthBlockSlot() {
        std::int32_t Count = PathGrowthBlocks.length();
        if (PathGrowthBlockCount >= Count) {
            PathGrowthBlocks.set_length(Count + 1);
            PathGrowthBlocks[Count] = nullptr;
        }
    }

    void FreePathGrowthBlocks() {
        std::int32_t Index{};
        {
            const std::int32_t cpp_last = PathGrowthBlocks.length() - 1;
            if (0 <= cpp_last) {
                for (Index = 0; Index <= cpp_last; ++Index) {
                    if (PathGrowthBlocks[Index] != nullptr) {
                        WindowsSdk::HeapFree(PathNodeHeap, 0u, PathGrowthBlocks[Index]);
                        PathGrowthBlocks[Index] = nullptr;
                    }
                }
            }
        }
        PathGrowthBlocks.set_length(0);
        PathGrowthBlockCount = 0;
    }

    std::uint8_t ReservePathGrowthBlock() {
        std::uint8_t Result = false;
        if (GR_Main::PathGrowEnabled) {
            ++PathGrowthBlockCount;
            aPath::EnsurePathGrowthBlockSlot();
            return true;
        }
        return Result;
    }

    // Native growth increments the free count before allocation and never records the allocated block in PathGrowthBlocks.
    std::uint8_t GrowPathNodePool() {
        std::int32_t Index{};
        std::uint8_t Result = false;
        if (PathPoolTail == nullptr) {
            return Result;
        }
        if (!aPath::ReservePathGrowthBlock()) {
            return Result;
        }
        std::int32_t Count = 100000;
        PathPoolFreeCount += Count;
        std::int32_t ByteCount = Count * static_cast<std::int32_t>(sizeof(TSPathNode));
        void* Block = WindowsSdk::HeapAlloc(PathNodeHeap, WindowsSdk::HEAP_ZERO_MEMORY, ByteCount);
        if (Block == nullptr) {
            return Result;
        }
        PSPathNode Node = static_cast<PSPathNode>(Block);
        PathPoolTail->Next = Node;
        PSPathNode Prev = PathPoolTail;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Node->Prev = Prev;
            Node->Next = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node,
                    static_cast<std::int32_t>(sizeof(TSPathNode))));
            Prev = Node;
            Node = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node,
                    static_cast<std::int32_t>(sizeof(TSPathNode))));
        }
        PathPoolTail = Prev;
        PathPoolTail->Next = nullptr;
        return true;
    }

    void FinalizePathNodePool() {
        aPath::FreePathGrowthBlocks();
        if (PathInitialBlock != nullptr) {
            WindowsSdk::HeapFree(PathNodeHeap, 0u, PathInitialBlock);
            PathInitialBlock = nullptr;
        }
        if (PathNodeHeap != 0) {
            WindowsSdk::HeapDestroy(PathNodeHeap);
            PathNodeHeap = 0u;
        }
        if (PathPoolLock != nullptr) {
            pas::free(PathPoolLock);
            PathPoolLock = nullptr;
        }
    }

    void TSPath_Create(TSPath* Self) {
        pas::object_create(Self);
        Self->AllocateNodeUnit();
    }

    void TSPath_Destroy(TSPath* Self) {
        Self->Clear();
        pas::critical_enter(PathPoolLock);
        PathPoolFreeCount += TSPath::CountNodeRangeInclusive(Self->FreeHead, Self->FreeTail);
        if (PathPoolTail != nullptr) {
            PathPoolTail->Next = Self->FreeHead;
        }
        Self->FreeHead->Prev = PathPoolTail;
        Self->FreeTail->Next = nullptr;
        PathPoolTail = Self->FreeTail;
        if (PathPoolHead == nullptr) {
            PathPoolHead = Self->FreeHead;
        }
        pas::critical_leave(PathPoolLock);
        Self->FreeHead = nullptr;
        Self->FreeTail = nullptr;
        Self->NodeCount = 0;
        pas::object_destroy(Self);
    }

    // Acquires 24 nodes from the shared pool; raises on allocation failure.
    void TSPath::AllocateNodeUnit() {
        std::int32_t Index{};
        std::int32_t Count = 24;
        pas::critical_enter(PathPoolLock);
        if (Count >= PathPoolFreeCount) {
            if (!aPath::GrowPathNodePool()) {
                pas::critical_leave(PathPoolLock);
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error: Path.AllocUnit  UnitCount=",
                    SysUtils::IntToStr(Count)})));
            }
        }
        PSPathNode First = PathPoolHead;
        PSPathNode Last = PathPoolHead;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, Count - 1); cpp_range.next(Index); ) {
            Last = Last->Next;
        }
        PathPoolHead = Last->Next;
        PathPoolHead->Prev = nullptr;
        PathPoolFreeCount -= Count;
        pas::critical_leave(PathPoolLock);
        if (FreeTail != nullptr) {
            FreeTail->Next = First;
        }
        First->Prev = FreeTail;
        Last->Next = nullptr;
        FreeTail = Last;
        if (FreeHead == nullptr) {
            FreeHead = First;
        }
    }

    // Increments NodeCount without linking into the active list. May allocate; payload is uninitialized.
    PSPathNode TSPath::PopFreeNode() {
        if (FreeHead == FreeTail || FreeHead == nullptr) {
            AllocateNodeUnit();
        }
        PSPathNode Node = FreeHead;
        Node->Next->Prev = nullptr;
        FreeHead = Node->Next;
        ++NodeCount;
        return Node;
    }

    // Node must belong to this path; it is recycled.
    void TSPath::RemoveNode(PSPathNode Node) {
        if (Node->Prev != nullptr) {
            Node->Prev->Next = Node->Next;
        }
        if (Node->Next != nullptr) {
            Node->Next->Prev = Node->Prev;
        }
        if (ActiveTail == Node) {
            ActiveTail = Node->Prev;
        }
        if (ActiveHead == Node) {
            ActiveHead = Node->Next;
        }
        if (FreeTail != nullptr) {
            FreeTail->Next = Node;
        }
        Node->Prev = FreeTail;
        Node->Next = nullptr;
        FreeTail = Node;
        if (FreeHead == nullptr) {
            FreeHead = Node;
        }
        --NodeCount;
    }

    // Inclusive range must be ordered and belong to this path; nodes are recycled.
    void TSPath::RemoveNodeRange(PSPathNode FirstNode, PSPathNode LastNode) {
        NodeCount -= TSPath::CountNodeRangeInclusive(FirstNode, LastNode);
        if (FirstNode->Prev != nullptr) {
            FirstNode->Prev->Next = LastNode->Next;
        }
        if (LastNode->Next != nullptr) {
            LastNode->Next->Prev = FirstNode->Prev;
        }
        if (LastNode == ActiveTail) {
            ActiveTail = FirstNode->Prev;
        }
        if (FirstNode == ActiveHead) {
            ActiveHead = LastNode->Next;
        }
        if (FreeTail != nullptr) {
            FreeTail->Next = FirstNode;
        }
        FirstNode->Prev = FreeTail;
        LastNode->Next = nullptr;
        FreeTail = LastNode;
        if (FreeHead == nullptr) {
            FreeHead = FirstNode;
        }
    }

    // Recycles active nodes into this path's free list.
    void TSPath::Clear() {
        if (ActiveHead != nullptr) {
            RemoveNodeRange(ActiveHead, ActiveTail);
        }
    }

    // New node is ActiveTail; payload is uninitialized.
    void TSPath::AppendNode() {
        PSPathNode Node = PopFreeNode();
        if (ActiveTail != nullptr) {
            ActiveTail->Next = Node;
        }
        Node->Prev = ActiveTail;
        Node->Next = nullptr;
        ActiveTail = Node;
        if (ActiveHead == nullptr) {
            ActiveHead = Node;
        }
    }

    void TSPath::AppendWaypoint(EC_Struct::TPointF Position, float Heading) {
        PSPathNode Node = PopFreeNode();
        if (ActiveTail != nullptr) {
            ActiveTail->Next = Node;
        }
        Node->Prev = ActiveTail;
        Node->Next = nullptr;
        ActiveTail = Node;
        if (ActiveHead == nullptr) {
            ActiveHead = Node;
        }
        pas::store_unaligned<EC_Struct::TPointF>(&Node->Position, Position);
        Node->Heading = Heading;
    }

    // Nil appends. Payload is uninitialized.
    PSPathNode TSPath::InsertNodeBefore(PSPathNode Node) {
        if (Node == nullptr) {
            AppendNode();
            return ActiveTail;
        }
        PSPathNode NewNode = PopFreeNode();
        NewNode->Prev = Node->Prev;
        NewNode->Next = Node;
        if (Node->Prev != nullptr) {
            Node->Prev->Next = NewNode;
        }
        Node->Prev = NewNode;
        if (Node == ActiveHead) {
            ActiveHead = NewNode;
        }
        return NewNode;
    }

    // Starts at Node.Next; nonpositive SkipCount selects that immediate successor. Node must be non-nil.
    PSPathNode TSPath::GetFollowingNode(PSPathNode Node, std::int32_t SkipCount) {
        Node = Node->Next;
        while (Node != nullptr) {
            if (SkipCount <= 0) {
                return Node;
            }
            --SkipCount;
            Node = Node->Next;
        }
        return nullptr;
    }

    // Excludes Node itself, which must be non-nil. Ties keep the earlier node.
    PSPathNode TSPath::FindNearestFollowingNode(PSPathNode Node, EC_Struct::TPointF Position) {
        float Distance{};
        PSPathNode Result = nullptr;
        float BestDistance = 1.0E+20f;
        Node = Node->Next;
        while (Node != nullptr) {
            Distance = aMyFunction::PointDistanceSquared(Node->Position, Position);
            if (Distance < BestDistance) {
                Result = Node;
                BestDistance = Distance;
            }
            Node = Node->Next;
        }
        return Result;
    }

    float TSPath::GetLength() {
        float Result = 0.0f;
        if (ActiveHead == nullptr) {
            return Result;
        }
        PSPathNode Node = ActiveHead->Next;
        while (Node != nullptr) {
            Result = static_cast<float>(static_cast<double>(Result) + aMyFunction::PointDistance(Node->Position, Node->Prev->Position));
            Node = Node->Next;
        }
        return Result;
    }

    // Returns zero for nil endpoints or when LastNode is not reachable from FirstNode.
    std::int32_t TSPath::CountNodeRangeInclusive(PSPathNode FirstNode, PSPathNode LastNode) {
        if (FirstNode == nullptr || LastNode == nullptr) {
            return 0;
        }
        std::int32_t Count = 1;
        PSPathNode Node = FirstNode;
        while (Node != nullptr && Node != LastNode) {
            ++Count;
            Node = Node->Next;
        }
        if (Node == nullptr) {
            return 0;
        }
        return Count;
    }

    // Uses the inclusive nodes as Bezier controls, unwraps headings, inserts samples and recycles the controls.
    void TSPath::ResampleBezierRange(PSPathNode FirstNode, PSPathNode LastNode, std::int32_t SampleCount) {
        std::int32_t Count = TSPath::CountNodeRangeInclusive(FirstNode, LastNode);
        if (Count < 2 || SampleCount < 2)
            return;
        PSPathNode AfterNode = LastNode->Next;
        std::vector<bezier24::Wide> coefficients(Count);
        coefficients[0] = bezier24::Wide(1);
        coefficients[Count - 1] = bezier24::Wide(1);
        for (int i = 1; i <= (Count - 1) / 2; ++i) {
            coefficients[i] = bezier24::div(bezier24::mul(bezier24::Wide(Count - i), coefficients[i - 1]),
                                            bezier24::Wide(i));
            coefficients[Count - 1 - i] = coefficients[i];
        }
        float heading = FirstNode->Heading;
        PSPathNode node = LastNode;
        while (node != FirstNode) {
            node->Heading = aMyFunction::HeadingDifferenceDegrees(node->Prev->Heading, node->Heading);
            node = node->Prev;
        }
        FirstNode->Heading = 0;
        node = FirstNode;
        while (node != LastNode) {
            heading = heading + node->Heading;
            node->Heading = heading;
            node = node->Next;
        }
        heading = heading + LastNode->Heading;
        LastNode->Heading = heading;
        float t = 0;
        const float step = 1.0f / float(SampleCount - 1);
        PSPathNode newNode = nullptr;
        for (int sample = 0; sample < SampleCount; ++sample) {
            double x = 0, y = 0, angle = 0;
            node = FirstNode;
            int i = 0;
            bezier24::Wide tp(1), inv = bezier24::div(bezier24::Wide(1), bezier24::Wide(1.0f - t));
            bezier24::Wide rp = bezier24::power(bezier24::Wide(1.0f - t), Count - 1);
            const bezier24::Wide tw(t);
            while (node != LastNode) {
                const bezier24::Wide weight = bezier24::mul(bezier24::mul(tp, coefficients[i]), rp);
                x = bezier24::add_product(x, weight, node->Position.X);
                y = bezier24::add_product(y, weight, node->Position.Y);
                angle = bezier24::add_product(angle, weight, node->Heading);
                ++i;
                tp = bezier24::mul(tp, tw);
                rp = bezier24::mul(rp, inv);
                node = node->Next;
            }
            const bezier24::Wide weight = bezier24::mul(tp, coefficients[i]);
            x = bezier24::add_product(x, weight, node->Position.X);
            y = bezier24::add_product(y, weight, node->Position.Y);
            angle = bezier24::add_product(angle, weight, node->Heading);
            newNode = InsertNodeBefore(AfterNode);
            newNode->Position.X = x;
            newNode->Position.Y = y;
            newNode->Heading = aMyFunction::WrapHeadingDegrees(float(angle));
            t = t + step;
        }
        newNode = AfterNode ? AfterNode->Prev : ActiveTail;
        pas::store_unaligned<EC_Struct::TPointF>(&newNode->Position, LastNode->Position);
        newNode->Heading = LastNode->Heading;
        RemoveNodeRange(FirstNode, LastNode);
    }

    void TSPath::p_destroy() { aPath::TSPath_Destroy(this); }

} // namespace aPath
