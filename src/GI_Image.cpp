#include "layout/GI_Image.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_AImage.hpp"
#include "units/GI_AlphaImage.hpp"
#include "units/GI_GAI.hpp"
#include "units/GI_GI.hpp"
#include "units/GI_GraphBuf.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_SimpleImage.hpp"
#include "units/GI_TransImage.hpp"
#include "units/GR_Main.hpp"

namespace GI_Image {
    void TImageGI_Create(TImageGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
    }

    void TImageGI_Destroy(TImageGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TImageGI::Clear() {
        GI_MessageLoop::TObjectGI::Clear();
    }

    // Empty paths remove the child; unknown modes raise.
    void TImageGI::SetImagePath(pas::WideString Path) {
        pas::WideString Mode{};
        if (ImagePath != Path) {
            if (SimpleImageControl != nullptr) {
                FreeOwnedChild(SimpleImageControl);
                SimpleImageControl = nullptr;
            }
            if (TransImageControl != nullptr) {
                FreeOwnedChild(TransImageControl);
                TransImageControl = nullptr;
            }
            if (AlphaImageControl != nullptr) {
                FreeOwnedChild(AlphaImageControl);
                AlphaImageControl = nullptr;
            }
            if (GiImageControl != nullptr) {
                FreeOwnedChild(GiImageControl);
                GiImageControl = nullptr;
            }
            if (AnimImageControl != nullptr) {
                FreeOwnedChild(AnimImageControl);
                AnimImageControl = nullptr;
            }
            if (GaiImageControl != nullptr) {
                FreeOwnedChild(GaiImageControl);
                GaiImageControl = nullptr;
            }
            if (GraphBufControl != nullptr) {
                FreeOwnedChild(GraphBufControl);
                GraphBufControl = nullptr;
            }
            if (Path == u"") {
                ImagePath = pas::WideString();
                Invalidate();
            } else {
                ImagePath = Path;
                Mode = EC_Str::ExtractNextDelimitedPartW(Path, u',');
                if (Mode == u"GraphBuf") {
                    GraphBufControl = pas::construct_call<GI_GraphBuf::TGraphBufGI>(GI_GraphBuf::TGraphBufGI_Create, this, false);
                    GraphBufControl->SetSize(ClientSize);
                    GraphBufControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                } else if (Path == u"") {
                    SimpleImageControl = pas::construct_call<GI_SimpleImage::TSimpleImageGI>(GI_SimpleImage::TSimpleImageGI_Create, this);
                    SimpleImageControl->SetImagePath(Mode);
                    SimpleImageControl->SetSize(ClientSize);
                    SimpleImageControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                } else if (Mode == u"Simple") {
                    SimpleImageControl = pas::construct_call<GI_SimpleImage::TSimpleImageGI>(GI_SimpleImage::TSimpleImageGI_Create, this);
                    SimpleImageControl->SetImagePath(Path);
                    SimpleImageControl->SetSize(ClientSize);
                    SimpleImageControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                } else if (Mode == u"Trans") {
                    TransImageControl = pas::construct_call<GI_TransImage::TTransImageGI>(GI_TransImage::TTransImageGI_Create, this);
                    TransImageControl->SetImagePath(Path);
                    TransImageControl->SetSize(ClientSize);
                    TransImageControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                } else if (Mode == u"Alpha") {
                    AlphaImageControl = pas::construct_call<GI_AlphaImage::TAlphaImageGI>(GI_AlphaImage::TAlphaImageGI_Create, this);
                    AlphaImageControl->SetImagePath(Path);
                    AlphaImageControl->SetSize(ClientSize);
                    AlphaImageControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                } else if (Mode == u"GI") {
                    GiImageControl = pas::construct_call<GI_GI::TgiGI>(GI_GI::TgiGI_Create, this);
                    GiImageControl->SetImagePath(Path);
                    GiImageControl->SetSize(ClientSize);
                    GiImageControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                } else if (Mode == u"Anim") {
                    AnimImageControl = pas::construct_call<GI_AImage::TAImageGI>(GI_AImage::TAImageGI_Create, this);
                    AnimImageControl->SetConfigPath(Path);
                    AnimImageControl->SetSize(ClientSize);
                    AnimImageControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                } else if (Mode == u"GAI") {
                    GaiImageControl = pas::construct_call<GI_GAI::TgaiGI>(GI_GAI::TgaiGI_Create, this);
                    GaiImageControl->SetImagePath(Path);
                    GaiImageControl->SetSize(ClientSize);
                    GaiImageControl->SetPosition(ClassesImports::Point(-OriginPoint.X, -OriginPoint.Y));
                    if (GaiImageControl->GetSequenceCount() > 0) {
                        GaiImageControl->SequenceIndex = 0;
                        GaiImageControl->UpdateAutoGeometry();
                        GaiImageControl->RestartPlayback();
                    }
                } else {
                    pas::raise(pas::make_exception<pas::Exception>(static_cast<pas::AnsiString>(pas::concat_wide({u"TImageGI.SetImage. Path=", Path}))));
                }
            }
        }
    }

    pas::WideString TImageGI::GetImagePath() {
        return ImagePath;
    }

    Types::TPoint TImageGI::GetContentSize() {
        Types::TPoint Result{};
        if (SimpleImageControl != nullptr) {
            Result = SimpleImageControl->GetContentSize();
        } else if (TransImageControl != nullptr) {
            Result = TransImageControl->GetContentSize();
        } else if (AlphaImageControl != nullptr) {
            Result = AlphaImageControl->GetContentSize();
        } else if (GiImageControl != nullptr) {
            Result = GiImageControl->GetContentSize();
        } else if (AnimImageControl != nullptr) {
            Result = AnimImageControl->GetContentSize();
        } else if (GaiImageControl != nullptr) {
            Result = GaiImageControl->GetContentSize();
        } else if (GraphBufControl != nullptr) {
            Result = ClassesImports::Point(GraphBufControl->GraphBuf->Width, GraphBufControl->GraphBuf->Height);
        } else {
            Result = ClassesImports::Point(0, 0);
        }
        return Result;
    }

    // Only GI children supply an origin; other kinds return (0,0).
    Types::TPoint TImageGI::GetContentOrigin() {
        Types::TPoint Result{};
        if (GiImageControl != nullptr) {
            Result = GiImageControl->GetContentOrigin();
        } else {
            Result = ClassesImports::Point(0, 0);
        }
        return Result;
    }

    void TImageGI::SetImageKindX(GI_Main::TImageKindXGI Value) {
        if (SimpleImageControl != nullptr) {
            SimpleImageControl->SetImageKindX(Value);
        } else if (TransImageControl != nullptr) {
            TransImageControl->SetImageKindX(Value);
        } else if (AlphaImageControl != nullptr) {
            AlphaImageControl->SetImageKindX(Value);
        } else if (GiImageControl != nullptr) {
            GiImageControl->SetImageKindX(Value);
        } else if (AnimImageControl != nullptr) {
            AnimImageControl->SetImageKindX(Value);
        } else if (GaiImageControl != nullptr) {
            GaiImageControl->SetImageKindX(Value);
        } else if (GraphBufControl != nullptr) {
            GraphBufControl->SetImageKindX(Value);
        }
    }

    void TImageGI::SetImageKindY(GI_Main::TImageKindYGI Value) {
        if (SimpleImageControl != nullptr) {
            SimpleImageControl->SetImageKindY(Value);
        } else if (TransImageControl != nullptr) {
            TransImageControl->SetImageKindY(Value);
        } else if (AlphaImageControl != nullptr) {
            AlphaImageControl->SetImageKindY(Value);
        } else if (GiImageControl != nullptr) {
            GiImageControl->SetImageKindY(Value);
        } else if (AnimImageControl != nullptr) {
            AnimImageControl->SetImageKindY(Value);
        } else if (GaiImageControl != nullptr) {
            GaiImageControl->SetImageKindY(Value);
        } else if (GraphBufControl != nullptr) {
            GraphBufControl->SetImageKindY(Value);
        }
    }

    // Only affects Simple, Trans and Anim children.
    void TImageGI::SetHalfAlpha(std::uint8_t Value) {
        if (SimpleImageControl != nullptr) {
            SimpleImageControl->SetHalfAlpha(Value);
        } else if (TransImageControl != nullptr) {
            TransImageControl->SetHalfAlpha(Value);
        } else if (AnimImageControl != nullptr) {
            AnimImageControl->SetHalfAlpha(Value);
        }
    }

    // Returns GI/GAI alpha, or 255 for other kinds.
    std::uint8_t TImageGI::GetAlpha() {
        if (GiImageControl != nullptr) {
            return GiImageControl->Alpha;
        } else if (GaiImageControl != nullptr) {
            return GaiImageControl->Alpha;
        } else {
            return 255;
        }
    }

    // Only affects GI/GAI children.
    void TImageGI::SetAlpha(std::uint8_t Value) {
        if (GiImageControl != nullptr) {
            GiImageControl->SetAlpha(Value);
        } else if (GaiImageControl != nullptr) {
            GaiImageControl->SetAlpha(Value);
        }
    }

    void TImageGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        if (SimpleImageControl != nullptr) {
            SimpleImageControl->SetSize(Size);
        } else if (TransImageControl != nullptr) {
            TransImageControl->SetSize(Size);
        } else if (AlphaImageControl != nullptr) {
            AlphaImageControl->SetSize(Size);
        } else if (GiImageControl != nullptr) {
            GiImageControl->SetSize(Size);
        } else if (AnimImageControl != nullptr) {
            AnimImageControl->SetSize(Size);
        } else if (GaiImageControl != nullptr) {
            GaiImageControl->SetSize(Size);
        } else if (GraphBufControl != nullptr) {
            GraphBufControl->SetSize(Size);
        }
    }

    void TImageGI::SetOrigin(Types::TPoint Origin) {
        Types::TPoint Position{};
        GI_MessageLoop::TObjectGI::SetOrigin(Origin);
        Position.X = -Origin.X;
        Position.Y = -Origin.Y;
        if (SimpleImageControl != nullptr) {
            SimpleImageControl->SetPosition(Position);
        } else if (TransImageControl != nullptr) {
            TransImageControl->SetPosition(Position);
        } else if (AlphaImageControl != nullptr) {
            AlphaImageControl->SetPosition(Position);
        } else if (GiImageControl != nullptr) {
            GiImageControl->SetPosition(Position);
        } else if (AnimImageControl != nullptr) {
            AnimImageControl->SetPosition(Position);
        } else if (GaiImageControl != nullptr) {
            GaiImageControl->SetPosition(Position);
        } else if (GraphBufControl != nullptr) {
            GraphBufControl->SetPosition(Position);
        }
    }

    // Returns false for kinds other than Alpha, Anim, GI and GAI.
    std::uint8_t TImageGI::HitTestPixel(Types::TPoint Point) {
        if (AlphaImageControl != nullptr) {
            return AlphaImageControl->HitTestPixel(Point);
        } else if (AnimImageControl != nullptr) {
            return AnimImageControl->HitTest(Point);
        } else if (GiImageControl != nullptr) {
            return GiImageControl->HitTestPixel(Point);
        } else if (GaiImageControl != nullptr) {
            return GaiImageControl->HitTestPixel(Point);
        } else {
            return false;
        }
    }

    // Only GI and GraphBuf write the result; other kinds leave it untouched.
    Types::TPoint TImageGI::GetVisualCenter() {
        Types::TPoint Result{};
        if (GiImageControl != nullptr) {
            Result = GiImageControl->GetVisualCenter();
        } else if (GraphBufControl != nullptr) {
            Result = GraphBufControl->GetVisualCenter();
        }
        return Result;
    }

    void TImageGI::RestartPlayback() {
        if (GaiImageControl != nullptr) {
            GaiImageControl->RestartPlayback();
        }
    }

    void TImageGI::StopPlayback() {
        if (GaiImageControl != nullptr) {
            GaiImageControl->StopAutoPlayback();
        }
    }

    void TImageGI_LoadFromConfigPath(TImageGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        Self->LoadImageProperties(GR_Main::UiStyleConfig->GetBlockByPath(Path));
    }

    void TImageGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadImageProperties(Block);
    }

    void TImageGI::LoadImageProperties(EC_BlockPar::TBlockParEC* Block) {
        if (Block->CountParams(u"Image"_wref.get()) > 0) {
            SetImagePath(Block->GetParam(u"Image"sv));
        }
        if (Block->CountParams(u"KindX"_wref.get()) > 0) {
            SetImageKindX(GI_Main::ParseImageKindXName(pas::view(Block->GetParam(u"KindX"sv))));
        }
        if (Block->CountParams(u"KindY"_wref.get()) > 0) {
            SetImageKindY(GI_Main::ParseImageKindYName(pas::view(Block->GetParam(u"KindY"sv))));
        }
        if (Block->CountParams(u"HalfAlpha"_wref.get()) > 0) {
            SetHalfAlpha(GI_Main::ParseEnabledNameGI(pas::view(Block->GetParam(u"HalfAlpha"sv))));
        }
        if (Block->CountParams(u"Auto"_wref.get()) > 0) {
            AutoUpdateFlags = GI_Main::ParseAutoGeometryFlagsGI(Block->GetParam(u"Auto"sv));
        }
    }

    // Auto-geometry bit 0 uses content origin; bit 1 uses content size.
    void TImageGI::UpdateAutoGeometry() {
        GI_MessageLoop::TObjectGI::UpdateAutoGeometry();
        if ((AutoUpdateFlags & GI_Main::agfPosition) == GI_Main::agfPosition) {
            WindowsSdk::TPoint contentOrigin = GetContentOrigin();
            GI_MessageLoop::TObjectGI* parent = Parent;
            SetPosition(parent->ToLocalPoint(contentOrigin));
        }
        if ((AutoUpdateFlags & GI_Main::agfSize) == GI_Main::agfSize) {
            SetSize(GetContentSize());
        }
    }

    // GI and GraphBuf children are skipped.
    void TImageGI::QueueImageLoad(pas::List* PendingLoads) {
        if (SimpleImageControl != nullptr) {
            SimpleImageControl->QueueImageLoad(PendingLoads);
        } else if (TransImageControl != nullptr) {
            TransImageControl->QueueImageLoad(PendingLoads);
        } else if (AlphaImageControl != nullptr) {
            AlphaImageControl->QueueImageLoad(PendingLoads);
        } else if (AnimImageControl != nullptr) {
            AnimImageControl->QueueImageLoad(PendingLoads);
        } else if (GaiImageControl != nullptr) {
            GaiImageControl->QueueImageLoad(PendingLoads);
        }
    }

    // Delegates to the GAI or GI child.
    void TImageGI::SetHardwareMirrorHorizontal(std::uint8_t Value) {
        if (GaiImageControl != nullptr) {
            GaiImageControl->SetHardwareMirrorHorizontal(Value);
        } else if (GiImageControl != nullptr) {
            GiImageControl->SetHardwareMirrorHorizontal(Value);
        }
    }

    void TImageGI::p_destroy() {
        GI_Image::TImageGI_Destroy(this);
    }

    void TImageGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Image::TImageGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Image
