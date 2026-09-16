#include "layout/ab_WorldImage.hpp"
#include "types/GI_GAI.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/SystemImports.hpp"
#include "types/Types.hpp"
#include "types/ab_MainForm.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Image.hpp"
#include "units/Globals.hpp"
#include "units/System.hpp"
#include "units/WindowsSdk.hpp"
#include "units/aMyFunction.hpp"
#include "units/ab_Global.hpp"
#include "units/ab_WorldImage.hpp"

namespace ab_WorldImage {
    std::uint32_t WorldImageHeap = 0u;

    ab_WorldImage::PabWorldImage FirstWorldImage = nullptr;

    ab_WorldImage::PabWorldImage LastWorldImage = nullptr;

    void ab_WorldImage_Clear() {
        while (!(FirstWorldImage == nullptr)) {
            ab_WorldImage::ab_WorldImage_Delete(LastWorldImage);
        }
        if (WorldImageHeap != 0) {
            WindowsSdk::HeapDestroy(WorldImageHeap);
            WorldImageHeap = 0u;
        }
    }

    PabWorldImage ab_WorldImage_Add() {
        if (WorldImageHeap == 0) {
            WorldImageHeap = WindowsSdk::HeapCreate(1u, 0x00008000u, 0u);
            if (WorldImageHeap == 0) {
                pas::raise(pas::make_exception<pas::Exception>("ab_WorldImage_Add.HeapCreate"_a));
            }
        }
        PabWorldImage Entry = static_cast<PabWorldImage>(EC_Mem::AllocClearFromHeapEC(WorldImageHeap, static_cast<std::int32_t>(sizeof(TabWorldImage))));
        if (LastWorldImage != nullptr) {
            LastWorldImage->Next = Entry;
        }
        Entry->Prev = LastWorldImage;
        Entry->Next = nullptr;
        LastWorldImage = Entry;
        if (FirstWorldImage == nullptr) {
            FirstWorldImage = Entry;
        }
        return Entry;
    }

    void ab_WorldImage_Delete(PabWorldImage Entry) {
        if (Entry->Prev != nullptr) {
            Entry->Prev->Next = Entry->Next;
        }
        if (Entry->Next != nullptr) {
            Entry->Next->Prev = Entry->Prev;
        }
        if (LastWorldImage == Entry) {
            LastWorldImage = Entry->Prev;
        }
        if (FirstWorldImage == Entry) {
            FirstWorldImage = Entry->Next;
        }
        if (Entry->Image != nullptr) {
            pas::free(Entry->Image);
            Entry->Image = nullptr;
        }
        Entry->FrontImagePath = pas::WideString();
        Entry->BackImagePath = pas::WideString();
        if (WorldImageHeap != 0) {
            EC_Mem::FreeFromHeapEC(WorldImageHeap, Entry);
        }
    }

    PabWorldImage ab_WorldImage_Create(EC_Struct::TVector3D Position, const pas::WideString& FrontPath, const pas::WideString& BackPath, std::uint8_t StopAnimation) {
        PabWorldImage Entry = ab_WorldImage::ab_WorldImage_Add();
        pas::store_unaligned<EC_Struct::TVector3D>(&Entry->Position, Position);
        Entry->FrontImagePath = FrontPath;
        Entry->BackImagePath = BackPath;
        Entry->Dirty = true;
        Entry->LoopAnimation = true;
        Entry->Finished = false;
        Entry->FrontDepth = ab_Global::WorldImageFrontDepth;
        Entry->BackDepth = ab_Global::WorldImageBackDepth;
        Entry->FrameMode = afmRestart;
        Entry->StopAnimation = StopAnimation;
        return Entry;
    }

    void ab_WorldImage_Set(PabWorldImage Entry, EC_Struct::TVector3D Position, const pas::WideString& FrontPath, const pas::WideString& BackPath) {
        pas::store_unaligned<EC_Struct::TVector3D>(&Entry->Position, Position);
        Entry->FrontImagePath = FrontPath;
        Entry->BackImagePath = BackPath;
        Entry->LoopAnimation = true;
        Entry->Finished = false;
        Entry->FrontDepth = ab_Global::WorldImageFrontDepth;
        Entry->BackDepth = ab_Global::WorldImageBackDepth;
        Entry->FrameMode = afmRestart;
        Entry->Dirty = true;
    }

    void ab_WorldImage_SetPosition(PabWorldImage Entry, EC_Struct::TVector3D Position) {
        pas::store_unaligned<EC_Struct::TVector3D>(&Entry->Position, Position);
    }

    void ab_WorldImage_SetDepth(PabWorldImage Entry, float FrontDepth, float BackDepth) {
        Entry->FrontDepth = FrontDepth;
        Entry->BackDepth = BackDepth;
        Entry->Dirty = true;
    }

    void ab_WorldImage_SetFrameMode(PabWorldImage Entry, std::int32_t Value) {
        Entry->FrameMode = Value;
    }

    void ab_WorldImage_SetLooping(PabWorldImage Entry, std::uint8_t Value) {
        Entry->LoopAnimation = Value;
        Entry->Dirty = true;
        Entry->StopAnimation = false;
    }

    void ab_WorldImage_Update() {
        PabWorldImage Entry{};
        std::int32_t Frame{};
        EC_Struct::TVector3D Position{};
        EC_Struct::TVector3D Center{};
        Center = EC_Struct::MakeVector3D(0.0, 0.0, 0.0);
        Center = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Center));
        Entry = FirstWorldImage;
        while (Entry != nullptr) {
            if (Entry->Finished) {
                if (Entry->Image != nullptr) {
                    Entry->Image->SetActive(false);
                }
                Entry = Entry->Next;
                continue;
            }
            Position = ab_Global::ProjectPointByMatrix(ab_Global::SphereProjectionMatrix, pas::ConstRef<EC_Struct::TVector3D>(&Entry->Position));
            if (Entry->Image == nullptr) {
                Entry->Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Globals::ArcadeBattleScreen->WorldPanel);
            }
            Frame = 0;
            if (Entry->Image->GaiImageControl != nullptr) {
                Frame = Entry->Image->GaiImageControl->SequenceFrame;
            }
            if (!ab_Global::IsDepthBeforeSphereHorizon(Position.Z)) {
                if (Entry->Image->Depth != Entry->BackDepth || Entry->Dirty) {
                    Entry->Image->SetActive(Entry->BackImagePath != u"");
                    if (Entry->Image->Active) {
                        Entry->Image->SetImagePath(Entry->BackImagePath);
                        {
                            Types::TPoint contentSize = Entry->Image->GetContentSize();
                            GI_Image::TImageGI* image = Entry->Image;
                            image->SetSize(contentSize);
                        }
                        Entry->Image->SetOrigin(EC_Struct::HalfPoint(Entry->Image->ClientSize));
                        if (Entry->Image->GaiImageControl != nullptr && static_cast<std::uint8_t>(Entry->StopAnimation ^ 1)) {
                            Entry->Image->GaiImageControl->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Entry));
                            if (!Entry->LoopAnimation) {
                                Entry->Image->GaiImageControl->CycleCompleteCallback = pas::bind_static_method<&ab_MainForm::TfAB::WorldImageCycleComplete>(Globals::ArcadeBattleScreen);
                            } else {
                                Entry->Image->GaiImageControl->CycleCompleteCallback = nullptr;
                            }
                            if (Entry->FrameMode == afmRestart && Entry->Dirty) {
                                Entry->Image->GaiImageControl->SetSequenceFrame(0);
                            } else if (Entry->FrameMode == afmRandomStart && Entry->Dirty) {
                                std::int32_t randomIntRange = aMyFunction::RandomIntRange(0, Entry->Image->GaiImageControl->SequenceFrameCount - 1);
                                GI_GAI::TgaiGI* gaiImageControl = Entry->Image->GaiImageControl;
                                gaiImageControl->SetSequenceFrame(randomIntRange);
                            } else {
                                Entry->Image->GaiImageControl->SetSequenceFrame(Frame);
                            }
                            Entry->Image->RestartPlayback();
                        } else {
                            Entry->Image->StopPlayback();
                        }
                    }
                    Entry->Image->SetDepth(Entry->BackDepth);
                }
            } else if (Entry->Image->Depth != Entry->FrontDepth || Entry->Dirty) {
                Entry->Image->SetActive(Entry->FrontImagePath != u"");
                if (Entry->Image->Active) {
                    Entry->Image->SetImagePath(Entry->FrontImagePath);
                    {
                        Types::TPoint contentSize_2 = Entry->Image->GetContentSize();
                        GI_Image::TImageGI* image_2 = Entry->Image;
                        image_2->SetSize(contentSize_2);
                    }
                    Entry->Image->SetOrigin(EC_Struct::HalfPoint(Entry->Image->ClientSize));
                    if (Entry->Image->GaiImageControl != nullptr && static_cast<std::uint8_t>(Entry->StopAnimation ^ 1)) {
                        Entry->Image->GaiImageControl->UserValue = static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(Entry));
                        if (!Entry->LoopAnimation) {
                            Entry->Image->GaiImageControl->CycleCompleteCallback = pas::bind_static_method<&ab_MainForm::TfAB::WorldImageCycleComplete>(Globals::ArcadeBattleScreen);
                        } else {
                            Entry->Image->GaiImageControl->CycleCompleteCallback = nullptr;
                        }
                        if (Entry->FrameMode == afmRestart && Entry->Dirty) {
                            Entry->Image->GaiImageControl->SetSequenceFrame(0);
                        } else if (Entry->FrameMode == afmRandomStart && Entry->Dirty) {
                            std::int32_t randomIntRange_2 = aMyFunction::RandomIntRange(0, Entry->Image->GaiImageControl->SequenceFrameCount - 1);
                            GI_GAI::TgaiGI* gaiImageControl_2 = Entry->Image->GaiImageControl;
                            gaiImageControl_2->SetSequenceFrame(randomIntRange_2);
                        } else {
                            Entry->Image->GaiImageControl->SetSequenceFrame(Frame);
                        }
                        Entry->Image->RestartPlayback();
                    } else {
                        Entry->Image->StopPlayback();
                    }
                }
                Entry->Image->SetDepth(Entry->FrontDepth);
            }
            Entry->Dirty = false;
            {
                std::int32_t round = System::Round(Position.Y);
                std::int32_t round_2 = System::Round(Position.X);
                Entry->Image->SetPosition(ClassesImports::Point(round_2, round));
            }
            Entry = Entry->Next;
        }
    }

} // namespace ab_WorldImage
