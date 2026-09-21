#include "layout/SE_StarsField.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_Main.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_InfiniteImage.hpp"
#include "units/GI_SimpleImage.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_StarsField.hpp"

namespace SE_StarsField {
    void TStarsFieldSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (IsAttachedToSpace()) {
            return;
        }
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        if (GlobalsV::StaticBackground) {
            StaticImage = pas::construct_call<GI_SimpleImage::TSimpleImageGI>(GI_SimpleImage::TSimpleImageGI_Create, Space->MapPanel);
            StaticImage->SetDepthByName(DepthExpression);
            StaticImage->SetPositionModeW(false);
            StaticImage->SetImageKindX(GI_Main::ikxLeftFill);
            StaticImage->SetImageKindY(GI_Main::ikyTopFill);
            StaticImage->SetPosition(ClassesImports::Point(-Space->MapPanel->OriginPoint.X, -Space->MapPanel->OriginPoint.Y));
            StaticImage->SetSize(ClassesImports::Point(Space->MapPanel->ClientSize.X, Space->MapPanel->ClientSize.Y));
            StaticImage->SetImagePath(ImagePath);
        } else {
            InfiniteImage = pas::construct_call<GI_InfiniteImage::TInfiniteImageGI>(GI_InfiniteImage::TInfiniteImageGI_Create, Space->MapPanel);
            InfiniteImage->SetDepthByName(DepthExpression);
            InfiniteImage->SetPositionModeW(true);
            InfiniteImage->SetImagePath(ImagePath);
        }
    }

    void TStarsFieldSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        if (InfiniteImage != nullptr) {
            Space->MapPanel->FreeOwnedChild(InfiniteImage);
            InfiniteImage = nullptr;
        }
        if (StaticImage != nullptr) {
            Space->MapPanel->FreeOwnedChild(StaticImage);
            StaticImage = nullptr;
        }
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TStarsFieldSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"sv);
    }

    void TStarsFieldSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        if (GlobalsV::StaticBackground) {
            GI_SimpleImage::TSimpleImageGI* cpp_with = pas::construct_call<GI_SimpleImage::TSimpleImageGI>(GI_SimpleImage::TSimpleImageGI_Create, Owner);
            cpp_with->SetImagePath(ImagePath);
            cpp_with->QueueImageLoad(PendingLoads);
            pas::free(cpp_with);
        } else {
            GI_InfiniteImage::TInfiniteImageGI* cpp_with_2 = pas::construct_call<GI_InfiniteImage::TInfiniteImageGI>(GI_InfiniteImage::TInfiniteImageGI_Create, Owner);
            cpp_with_2->SetImagePath(ImagePath);
            cpp_with_2->QueueImageLoad(PendingLoads);
            pas::free(cpp_with_2);
        }
    }

} // namespace SE_StarsField
