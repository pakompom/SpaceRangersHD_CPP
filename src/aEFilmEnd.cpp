#include "layout/aEFilmEnd.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_PSWeapon.hpp"
#include "types/SE_GAIEffect.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Weapon.hpp"
#include "types/aEFilm.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GR_Main.hpp"
#include "units/SE_Space.hpp"
#include "units/aEFilmEnd.hpp"

// is the Pascal class-name string "TEFilmEnd", referenced by its VMT.
// The former 12-byte IDA function there was metadata, not a tenth routine.
namespace aEFilmEnd {
    void TEFilmEnd_Create(TEFilmEnd* Self) {
        EC_Struct::TObjectEx_Create(Self);
    }

    void TEFilmEnd_Destroy(TEFilmEnd* Self) {
        Self->Clear();
        EC_Struct::TObjectEx_Destroy(Self);
    }

    void TEFilmEnd::Clear() {
        while (FirstEntry != nullptr) {
            RemoveEntry(LastEntry);
        }
    }

    PEFilmEndEntry TEFilmEnd::AppendEntry() {
        PEFilmEndEntry Entry = static_cast<PEFilmEndEntry>(EC_Mem::AllocEC(static_cast<std::int32_t>(sizeof(TEFilmEndEntry))));
        if (LastEntry != nullptr) {
            LastEntry->Next = Entry;
        }
        Entry->Prev = LastEntry;
        Entry->Next = nullptr;
        LastEntry = Entry;
        if (FirstEntry == nullptr) {
            FirstEntry = Entry;
        }
        Entry->SceneObject = nullptr;
        Entry->RelatedObject1 = nullptr;
        Entry->RelatedObject2 = nullptr;
        return Entry;
    }

    // Detaches and releases all three retained scene references, then frees Entry.
    void TEFilmEnd::RemoveEntry(PEFilmEndEntry Entry) {
        if (Entry->Prev != nullptr) {
            Entry->Prev->Next = Entry->Next;
        }
        if (Entry->Next != nullptr) {
            Entry->Next->Prev = Entry->Prev;
        }
        if (LastEntry == Entry) {
            LastEntry = Entry->Prev;
        }
        if (FirstEntry == Entry) {
            FirstEntry = Entry->Next;
        }
        if (Entry->SceneObject != nullptr) {
            Entry->SceneObject->DetachFromSpace();
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject));
        }
        if (Entry->RelatedObject1 != nullptr) {
            Entry->RelatedObject1->DetachFromSpace();
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1));
        }
        if (Entry->RelatedObject2 != nullptr) {
            Entry->RelatedObject2->DetachFromSpace();
            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject2));
        }
        EC_Mem::FreeEC(Entry);
    }

    // Transfers selected scene references from Film. Requires its 0x18 command marker.
    void TEFilmEnd::TakeTrailingEffects(aEFilm::TEFilm* Film) {
        aEFilm::PEFilmCommand Command{};
        aEFilm::TEFilmObj* Obj{};
        PEFilmEndEntry Entry{};
        SE_Weapon::TWeaponSE* Weapon{};
        aEFilm::PEFilmCommand FirstTrailing = Film->LastCommand;
        while (FirstTrailing != nullptr) {
            if (FirstTrailing->Kind == aEFilm::efcBeginTrailingEffects) {
                break;
            }
            FirstTrailing = FirstTrailing->Prev;
        }
        FirstTrailing = FirstTrailing->Next;
        Obj = Film->FirstObject;
        while (Obj != nullptr) {
            if (SE_GAIEffect::TGAIEffectSE* gAIEffectSE = pas::class_cast_if<SE_GAIEffect::TGAIEffectSE*>(Obj->SceneObject)) {
                if (gAIEffectSE->Animation != nullptr) {
                    gAIEffectSE->Animation->RestartPlayback();
                }
            } else if (SE_Weapon::TWeaponSE* weaponSE = pas::class_cast_if<SE_Weapon::TWeaponSE*>(Obj->SceneObject)) {
                Weapon = weaponSE;
                Entry = AppendEntry();
                SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject), Weapon);
                if (Weapon->TargetDestroyed) {
                    Command = FirstTrailing;
                    while (Command != nullptr) {
                        if (Command->Kind == aEFilm::efcReleaseObject && Command->Obj != nullptr && Command->Obj->SceneObject == Weapon->TargetObject) {
                            SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1), Command->Obj->SceneObject);
                            SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Command->Obj->SceneObject));
                            break;
                        }
                        Command = Command->Next;
                    }
                }
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj->SceneObject));
            } else if (pas::class_cast_if<SE_Hole::THoleSE*>(Obj->SceneObject) != nullptr) {
                Command = FirstTrailing;
                while (Command != nullptr) {
                    if (Command->Kind == aEFilm::efcReleaseObject && Command->Obj != nullptr && Command->Obj->SceneObject == Obj->SceneObject) {
                        Entry = AppendEntry();
                        SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject), Obj->SceneObject);
                        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj->SceneObject));
                        break;
                    }
                    Command = Command->Next;
                }
            }
            Obj = Obj->Next;
        }
    }

    void TEFilmEnd::AdvanceEffects() {
        PEFilmEndEntry Entry{};
        PEFilmEndEntry NextEntry = FirstEntry;
        while (NextEntry != nullptr) {
            Entry = NextEntry;
            NextEntry = NextEntry->Next;
            try {
                Entry->SceneObject->Advance();
            } catch (...) {
                auto cpp_exception = pas::caught_object();
                if (pas::Exception* E = pas::class_cast_if<pas::Exception*>(cpp_exception)) {
                    GR_Main::AppendLogLineThreadSafe(pas::concat_ansi({static_cast<pas::AnsiString>(pas::class_name(pas::class_type(E))), " ", E->message}));
                    GR_Main::AppendLogLineThreadSafe("Error in Event.Step"_a);
                    if (Entry->SceneObject != nullptr) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"event ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(Entry->SceneObject))), " "})), Entry->SceneObject->GraphKey})));
                    }
                    if (Entry->RelatedObject1 != nullptr) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"obj ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(Entry->RelatedObject1))), " "})), Entry->RelatedObject1->GraphKey})));
                    }
                    if (Entry->RelatedObject2 != nullptr) {
                        GR_Main::AppendLogLineThreadSafe(static_cast<pas::AnsiString>(pas::concat_wide({static_cast<pas::WideString>(pas::concat_ansi({"obj2 ", static_cast<pas::AnsiString>(pas::class_name(pas::class_type(Entry->RelatedObject2))), " "})), Entry->RelatedObject2->GraphKey})));
                    }
                    pas::raise(pas::make_exception<pas::Exception>("Error in TEFilmEnd.Run"_a));
                } else {
                    throw;
                }
            }
            if (!Entry->SceneObject->IsAttachedToSpace()) {
                Entry->SceneObject->DetachFromSpace();
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject));
                if (Entry->RelatedObject1 != nullptr) {
                    Entry->RelatedObject1->DetachFromSpace();
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1));
                }
                if (Entry->RelatedObject2 != nullptr) {
                    Entry->RelatedObject2->DetachFromSpace();
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject2));
                }
                RemoveEntry(Entry);
            }
        }
    }

    // Clears matching references without unlinking entries.
    void TEFilmEnd::ReleaseObjectReferences(SE_Space::TObjectSE* Obj) {
        PEFilmEndEntry Entry{};
        Entry = FirstEntry;
        while (Entry != nullptr) {
            if (Entry->SceneObject != nullptr) {
                if (Entry->SceneObject == Obj) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->SceneObject));
                }
            }
            if (Entry->RelatedObject1 != nullptr) {
                if (Entry->RelatedObject1 == Obj) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject1));
                }
            }
            if (Entry->RelatedObject2 != nullptr) {
                if (Entry->RelatedObject2 == Obj) {
                    SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Entry->RelatedObject2));
                }
            }
            Entry = Entry->Next;
        }
    }

    // Removes weapon entries whose Projectile is nonzero.
    void TEFilmEnd::RemoveLinkedWeaponEffects() {
        PEFilmEndEntry Entry{};
        PEFilmEndEntry NextEntry = FirstEntry;
        while (NextEntry != nullptr) {
            Entry = NextEntry;
            NextEntry = NextEntry->Next;
            if (pas::class_cast_if<SE_Weapon::TWeaponSE*>(Entry->SceneObject) != nullptr) {
                if (pas::checked_cast<SE_Weapon::TWeaponSE*>(Entry->SceneObject)->Projectile != nullptr) {
                    RemoveEntry(Entry);
                }
            }
        }
    }

    void TEFilmEnd::p_destroy() {
        aEFilmEnd::TEFilmEnd_Destroy(this);
    }

} // namespace aEFilmEnd
