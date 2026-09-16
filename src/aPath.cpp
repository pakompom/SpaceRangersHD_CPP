#include "layout/aPath.hpp"
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
        PathInitialBlock = WindowsSdk::HeapAlloc(PathNodeHeap, 0u, PathPoolFreeCount * static_cast<std::int32_t>(sizeof(TSPathNode)));
        if (PathInitialBlock == nullptr) {
            pas::raise(pas::make_exception<pas::Exception>("Error: HeapAlloc"_a));
        }
        WindowsImports::ZeroMemory(PathInitialBlock, PathPoolFreeCount * static_cast<std::int32_t>(sizeof(TSPathNode)));
        PSPathNode Node = static_cast<PSPathNode>(PathInitialBlock);
        PathPoolHead = Node;
        PSPathNode Prev = nullptr;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, PathPoolFreeCount - 1); cpp_range.next(Index); ) {
            Node->Prev = Prev;
            Node->Next = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node, static_cast<std::int32_t>(sizeof(TSPathNode))));
            Prev = Node;
            Node = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node, static_cast<std::int32_t>(sizeof(TSPathNode))));
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
            Node->Next = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node, static_cast<std::int32_t>(sizeof(TSPathNode))));
            Prev = Node;
            Node = static_cast<PSPathNode>(EC_Mem::AddPointerOffset(Node, static_cast<std::int32_t>(sizeof(TSPathNode))));
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

    void TSPath::AllocateNodeUnit() {
        std::int32_t Index{};
        std::int32_t Count = 24;
        pas::critical_enter(PathPoolLock);
        if (Count >= PathPoolFreeCount) {
            if (!aPath::GrowPathNodePool()) {
                pas::critical_leave(PathPoolLock);
                pas::raise(pas::make_exception<pas::Exception>(pas::concat_ansi({"Error: Path.AllocUnit  UnitCount=", SysUtils::IntToStr(Count)})));
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

    void TSPath::Clear() {
        if (ActiveHead != nullptr) {
            RemoveNodeRange(ActiveHead, ActiveTail);
        }
    }

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

    PSPathNode TSPath::InsertNodeBefore(PSPathNode Node) {
        if (Node == nullptr) {
            AppendNode();
            return ActiveTail;
        }
        PSPathNode NewNode = PopFreeNode();
        reinterpret_cast<PSPathNode>(reinterpret_cast<std::uint8_t*>(NewNode) + 0)->Prev = Node->Prev;
        reinterpret_cast<PSPathNode>(reinterpret_cast<std::uint8_t*>(NewNode) + 0)->Next = Node;
        if (Node->Prev != nullptr) {
            Node->Prev->Next = NewNode;
        }
        Node->Prev = NewNode;
        if (reinterpret_cast<PSPathNode>(reinterpret_cast<std::uint8_t*>(Node) + 0) == ActiveHead) {
            ActiveHead = reinterpret_cast<PSPathNode>(reinterpret_cast<std::uint8_t*>(NewNode) + 0);
        }
        return NewNode;
    }

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
            Result = static_cast<long double>(Result) + aMyFunction::PointDistance(Node->Position, Node->Prev->Position);
            Node = Node->Next;
        }
        return Result;
    }

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

    void TSPath::ResampleBezierRange(PSPathNode FirstNode, PSPathNode LastNode, std::int32_t SampleCount) {
        pas::DynArray<double> Coefficients{};
        PSPathNode NewNode{};
        std::int32_t Index{};
        std::int32_t Sample{};
        double Weight{};
        double X{};
        double Y{};
        double Angle{};
        pas::Extended TPower{};
        pas::Extended InvRemaining{};
        pas::Extended RemainingPower{};
        std::int32_t Count = TSPath::CountNodeRangeInclusive(FirstNode, LastNode);
        if (Count < 2) {
            return;
        }
        if (SampleCount < 2) {
            return;
        }
        PSPathNode AfterNode = LastNode->Next;
        Coefficients.set_length(Count);
        Coefficients[0] = 1.0;
        Coefficients[Count - 1] = 1.0;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, (Count - 1) / 2); cpp_range.next(Index); ) {
            Coefficients[Index] = pas::real_divide(static_cast<long double>(Count - Index) * Coefficients[Index - 1], Index);
            Coefficients[Count - 1 - Index] = Coefficients[Index];
        }
        double Heading = FirstNode->Heading;
        PSPathNode Node = LastNode;
        while (Node != FirstNode) {
            Node->Heading = aMyFunction::HeadingDifferenceDegrees(Node->Prev->Heading, Node->Heading);
            Node = Node->Prev;
        }
        FirstNode->Heading = 0.0f;
        Node = FirstNode;
        while (Node != LastNode) {
            Heading = static_cast<long double>(Heading) + Node->Heading;
            Node->Heading = Heading;
            Node = Node->Next;
        }
        Heading = static_cast<long double>(Heading) + LastNode->Heading;
        LastNode->Heading = Heading;
        double T = 0.0;
        for (auto cpp_range_2 = pas::for_to<std::int32_t>(0, SampleCount - 1); cpp_range_2.next(Sample); ) {
            X = 0.0;
            Y = 0.0;
            Angle = 0.0;
            Node = FirstNode;
            Index = 0;
            TPower = 1.0L;
            InvRemaining = pas::real_divide(1.0L, 1.0L - T);
            RemainingPower = Math::Power(1.0L - T, Count - 1 - Index);
            while (Node != LastNode) {
                Weight = TPower * Coefficients[Index] * RemainingPower;
                X = X + static_cast<long double>(Weight) * Node->Position.X;
                Y = Y + static_cast<long double>(Weight) * Node->Position.Y;
                Angle = Angle + static_cast<long double>(Weight) * Node->Heading;
                ++Index;
                TPower = TPower * T;
                RemainingPower = RemainingPower * InvRemaining;
                Node = Node->Next;
            }
            Weight = TPower * Coefficients[Index];
            X = X + static_cast<long double>(Weight) * Node->Position.X;
            Y = Y + static_cast<long double>(Weight) * Node->Position.Y;
            Angle = Angle + static_cast<long double>(Weight) * Node->Heading;
            NewNode = InsertNodeBefore(AfterNode);
            NewNode->Position.X = X;
            NewNode->Position.Y = Y;
            NewNode->Heading = aMyFunction::WrapHeadingDegrees(Angle);
            T = T + pas::real_divide(1.0L, SampleCount - 1);
        }
        if (AfterNode == nullptr) {
            NewNode = ActiveTail;
        } else {
            NewNode = AfterNode->Prev;
        }
        pas::store_unaligned<EC_Struct::TPointF>(&NewNode->Position, LastNode->Position);
        NewNode->Heading = LastNode->Heading;
        RemoveNodeRange(FirstNode, LastNode);
        Coefficients = nullptr;
    }

    void TSPath::p_destroy() {
        aPath::TSPath_Destroy(this);
    }

} // namespace aPath
