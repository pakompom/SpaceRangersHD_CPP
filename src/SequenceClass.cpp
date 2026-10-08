#include "layout/SequenceClass.hpp"
#include "types/LocationClass.hpp"
#include "types/PathClass.hpp"
#include "units/EC_Struct.hpp"
#include "units/SequenceClass.hpp"

namespace SequenceClass {
    void TSequence_Create(TSequence* Self) {
        EC_Struct::TObjectEx_Create(Self);
        Self->Locations = pas::make_object<pas::List>();
        Self->Paths = pas::make_object<pas::List>();
        Self->TraversalLimit = 0;
        Self->UnknownFlag = 0;
    }

    // Clears member Sequence links without freeing the members.
    void TSequence_Destroy(TSequence* Self) {
        std::int32_t i{};
        LocationClass::TLocation* Location{};
        PathClass::TPath* Path{};
        {
            const std::int32_t cpp_last = pas::list_count(Self->Locations) - 1;
            if (0 <= cpp_last) {
                for (i = 0; i <= cpp_last; ++i) {
                    Location = pas::list_at<LocationClass::TLocation>(Self->Locations, i);
                    Location->Sequence = nullptr;
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = pas::list_count(Self->Paths) - 1;
            if (0 <= cpp_last_2) {
                for (i = 0; i <= cpp_last_2; ++i) {
                    Path = pas::list_at<PathClass::TPath>(Self->Paths, i);
                    Path->Sequence = nullptr;
                }
            }
        }
        pas::list_clear(Self->Locations);
        pas::free(Self->Locations);
        Self->Locations = nullptr;
        pas::list_clear(Self->Paths);
        pas::free(Self->Paths);
        Self->Paths = nullptr;
        EC_Struct::TObjectEx_Destroy(Self);
    }

    // Also updates every member's visit or traversal limit.
    void TSequence::SetTraversalLimit(std::int32_t Value) {
        std::int32_t i{};
        LocationClass::TLocation* Location{};
        PathClass::TPath* Path{};
        TraversalLimit = Value;
        {
            const std::int32_t cpp_last = pas::list_count(Locations) - 1;
            if (0 <= cpp_last) {
                for (i = 0; i <= cpp_last; ++i) {
                    Location = pas::list_at<LocationClass::TLocation>(Locations, i);
                    Location->VisitLimit = TraversalLimit;
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = pas::list_count(Paths) - 1;
            if (0 <= cpp_last_2) {
                for (i = 0; i <= cpp_last_2; ++i) {
                    Path = pas::list_at<PathClass::TPath>(Paths, i);
                    Path->TraversalLimit = TraversalLimit;
                }
            }
        }
    }

    // Propagates the minimum positive member limit, or zero if none.
    void TSequence::RecomputeTraversalLimit() {
        std::int32_t i{};
        LocationClass::TLocation* Location{};
        PathClass::TPath* Path{};
        TraversalLimit = 0;
        {
            const std::int32_t cpp_last = pas::list_count(Locations) - 1;
            if (0 <= cpp_last) {
                for (i = 0; i <= cpp_last; ++i) {
                    Location = pas::list_at<LocationClass::TLocation>(Locations, i);
                    if (Location->VisitLimit > 0) {
                        if (TraversalLimit == 0 || Location->VisitLimit < TraversalLimit) {
                            TraversalLimit = Location->VisitLimit;
                        }
                    }
                }
            }
        }
        {
            const std::int32_t cpp_last_2 = pas::list_count(Paths) - 1;
            if (0 <= cpp_last_2) {
                for (i = 0; i <= cpp_last_2; ++i) {
                    Path = pas::list_at<PathClass::TPath>(Paths, i);
                    if (Path->TraversalLimit > 0) {
                        if (TraversalLimit == 0 || Path->TraversalLimit < TraversalLimit) {
                            TraversalLimit = Path->TraversalLimit;
                        }
                    }
                }
            }
        }
        SetTraversalLimit(TraversalLimit);
    }

    void TSequence::AddLocation(void* Location) {
        LocationClass::TLocation* Member = static_cast<LocationClass::TLocation*>(Location);
        pas::list_add(Locations, Location);
        Member->Sequence = this;
    }

    void TSequence::AddPath(void* Path) {
        PathClass::TPath* Member = static_cast<PathClass::TPath*>(Path);
        pas::list_add(Paths, Path);
        Member->Sequence = this;
    }

    void TSequence::PrependPath(void* Path) {
        PathClass::TPath* Member = static_cast<PathClass::TPath*>(Path);
        pas::list_insert(Paths, 0, Path);
        Member->Sequence = this;
    }

    void TSequence::p_destroy() {
        SequenceClass::TSequence_Destroy(this);
    }

} // namespace SequenceClass
