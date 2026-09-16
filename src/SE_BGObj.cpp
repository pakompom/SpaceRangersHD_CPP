#include "layout/SE_BGObj.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/SE_Space.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/EC_Str.hpp"
#include "units/EC_Struct.hpp"
#include "units/GI_Image.hpp"
#include "units/SE_BGObj.hpp"

namespace SE_BGObj {
    void TBGObjSE::AttachToSpace(SE_Space::TSpaceSE* ASpace) {
        if (IsAttachedToSpace()) {
            return;
        }
        SE_Space::TObjectSE::AttachToSpace(ASpace);
        Image = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Space->MapPanel);
        Image->SetPositionModeW(true);
        Image->SetDepthByName(DepthExpression);
        Image->SetPosition(EC_Struct::TruncatePointF(Position));
        Image->SetImagePath(ImagePath);
        {
            Types::TPoint contentSize = Image->GetContentSize();
            GI_Image::TImageGI* image = Image;
            image->SetSize(contentSize);
        }
    }

    void TBGObjSE::DetachFromSpace() {
        if (!IsAttachedToSpace()) {
            return;
        }
        Space->MapPanel->FreeOwnedChild(Image);
        Image = nullptr;
        SE_Space::TObjectSE::DetachFromSpace();
    }

    void TBGObjSE::SetPosition(EC_Struct::TPointF APosition) {
        SE_Space::TObjectSE::SetPosition(APosition);
        if (IsAttachedToSpace()) {
            Image->SetPosition(EC_Struct::TruncatePointF(APosition));
        }
    }

    void TBGObjSE::LoadTemplate(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE::LoadTemplate(Block);
        ImagePath = Block->GetParam(u"Image"_wref.get());
        Radius = EC_Str::ExtractDigitsToIntW(Block->GetParam(u"Radius"_wref.get()));
    }

    void TBGObjSE::QueueImageLoad(pas::List* PendingLoads, GI_MessageLoop::TObjectGI* Owner) {
        GI_Image::TImageGI* cpp_with = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Owner);
        cpp_with->SetImagePath(ImagePath);
        cpp_with->QueueImageLoad(PendingLoads);
        pas::free(cpp_with);
    }

} // namespace SE_BGObj
