#include "layout/GI_Circle.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Mem.hpp"
#include "units/GI_Circle.hpp"
#include "units/GI_Main.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GR_DX.hpp"
#include "units/GR_GraphBuf.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SysUtils.hpp"

namespace GI_Circle {
    void TCircleGI_Create(TCircleGI* Self, GI_MessageLoop::TObjectGI* Owner) {
        GI_MessageLoop::TObjectGI_Create(Self, Owner);
        Self->Kind = ckSimple;
        Self->Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->FillColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Self->Center = ClassesImports::Point(0, 0);
        Self->Radius = 10;
        Self->ShrLightInner = 1;
        Self->ShrLightOuter = 0;
        Self->LightBufferDirty = true;
    }

    void TCircleGI_Destroy(TCircleGI* Self) {
        GI_MessageLoop::TObjectGI_Destroy(Self);
    }

    void TCircleGI::Clear() {
        Kind = ckSimple;
        Color = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        FillColor = GR_Main::CurrentPixelFormat->PackRgbBytes(255, 255, 255);
        Center = ClassesImports::Point(0, 0);
        Radius = 10;
        ShrLightInner = 1;
        ShrLightOuter = 0;
        LightBufferDirty = true;
        if (LightBuffer != nullptr) {
            pas::free(LightBuffer);
            LightBuffer = nullptr;
        }
        GI_MessageLoop::TObjectGI::Clear();
    }

    void TCircleGI::SetKind(TCircleKindGI Value) {
        if (Value != Kind) {
            Kind = Value;
            if (Kind == ckShrLight || Kind == ckMulLight) {
                if (LightBuffer == nullptr) {
                    LightBuffer = pas::construct_call<GR_GraphBuf::TGraphBufGR>(GR_GraphBuf::TGraphBufGR_Create, false);
                }
            } else if (LightBuffer != nullptr) {
                pas::free(LightBuffer);
                LightBuffer = nullptr;
            }
            LightBufferDirty = true;
            Invalidate();
        }
    }

    void TCircleGI::SetColor(std::uint32_t Value) {
        if (Color != Value) {
            Color = Value;
            Invalidate();
        }
    }

    void TCircleGI::SetFillColor(std::uint32_t Value) {
        if (FillColor != Value) {
            FillColor = Value;
            Invalidate();
        }
    }

    void TCircleGI::SetCenter(Types::TPoint Value) {
        if (Center.X != Value.X || Center.Y != Value.Y) {
            Center = Value;
            LightBufferDirty = true;
            Invalidate();
        }
    }

    void TCircleGI::SetRadius(std::int32_t Value) {
        if (Radius != Value) {
            Radius = Value;
            LightBufferDirty = true;
            Invalidate();
        }
    }

    void TCircleGI::SetShrLightInner(std::uint8_t Value) {
        if (ShrLightInner != Value) {
            ShrLightInner = Value;
            LightBufferDirty = true;
            Invalidate();
        }
    }

    void TCircleGI::SetShrLightOuter(std::uint8_t Value) {
        if (ShrLightOuter != Value) {
            ShrLightOuter = Value;
            LightBufferDirty = true;
            Invalidate();
        }
    }

    void TCircleGI::SetSize(Types::TPoint Size) {
        GI_MessageLoop::TObjectGI::SetSize(Size);
        LightBufferDirty = true;
    }

    void TCircleGI::SetActive(std::uint8_t Enabled) {
        GI_MessageLoop::TObjectGI::SetActive(Enabled);
        if (static_cast<std::uint8_t>(Active ^ 1) && (Kind == ckShrLight || Kind == ckMulLight)) {
            if (LightBuffer != nullptr) {
                LightBuffer->Clear();
            }
        }
    }

    void TCircleGI::OnActivate() {
        GI_MessageLoop::TObjectGI::OnActivate();
        LightBufferDirty = true;
    }

    void TCircleGI::OnDeactivate() {
        GI_MessageLoop::TObjectGI::OnDeactivate();
        if (LightBuffer != nullptr) {
            LightBuffer->Clear();
        }
    }

    void TCircleGI_LoadFromConfigPath(TCircleGI* Self, const pas::WideString& Path) {
        GI_MessageLoop::TObjectGI_LoadFromConfigPath(Self, Path);
        EC_BlockPar::TBlockParEC* Block = GR_Main::UiStyleConfig->GetBlockByPath(Path);
        Self->LoadShapeProperties(Block);
    }

    void TCircleGI::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        GI_MessageLoop::TObjectGI::LoadFromBlock(Block);
        LoadShapeProperties(Block);
    }

    void TCircleGI::LoadShapeProperties(EC_BlockPar::TBlockParEC* Block) {
        pas::WideString Text{};
        if (Block->CountParams(u"Kind"_wref.get()) > 0) {
            Text = Block->GetParam(u"Kind"_wref.get());
            if (Text == u"Simple") {
                SetKind(ckSimple);
            } else if (Text == u"Circle") {
                SetKind(ckCircle);
            } else if (Text == u"Fill") {
                SetKind(ckFill);
            } else if (Text == u"ShrLight") {
                SetKind(ckShrLight);
            } else if (Text == u"MulLight") {
                SetKind(ckMulLight);
            }
        }
        if (Block->CountParams(u"Color"_wref.get()) > 0) {
            SetColor(GI_Main::GetColorGI(Block->GetParam(u"Color"_wref.get())));
        }
        if (Block->CountParams(u"ColorFill"_wref.get()) > 0) {
            SetFillColor(GI_Main::GetColorGI(Block->GetParam(u"ColorFill"_wref.get())));
        }
        if (Block->CountParams(u"Radius"_wref.get()) > 0) {
            SetRadius(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"Radius"_wref.get()))));
        }
        if (Block->CountParams(u"Center"_wref.get()) > 0) {
            SetCenter(GI_Main::GetPointGI(Block->GetParam(u"Center"_wref.get())));
        }
        if (Block->CountParams(u"ShrLightInner"_wref.get()) > 0) {
            SetShrLightInner(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"ShrLightInner"_wref.get()))));
        }
        if (Block->CountParams(u"ShrLightOuter"_wref.get()) > 0) {
            SetShrLightOuter(SysUtils::StrToInt(static_cast<pas::AnsiString>(Block->GetParam(u"ShrLightOuter"_wref.get()))));
        }
    }

    // MulLight is unimplemented.
    void TCircleGI::Draw(Types::TRect ClipRect) {
        std::int32_t R{};
        std::int32_t X{};
        std::int32_t Y{};
        std::int32_t Width{};
        std::int32_t Height{};
        Types::TRect Bounds{};
        if (Kind == ckSimple) {
            R = std::min<std::int32_t>(HitTestBounds.Bottom - HitTestBounds.Top, HitTestBounds.Right - HitTestBounds.Left) / 2;
            if (R > 0) {
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawCircle((HitTestBounds.Right + HitTestBounds.Left) / 2, (HitTestBounds.Bottom + HitTestBounds.Top) / 2, R, GR_DX::Color565ToArgb(Color), 255, 0, &ClipRect);
                } else {
                    GR_Main::ScreenRenderBuffer->DrawAntialiasedCircle16(ClassesImports::Point((HitTestBounds.Bottom + HitTestBounds.Top) / 2, (HitTestBounds.Right + HitTestBounds.Left) / 2), R, Color, ClipRect);
                }
            }
        } else if (Kind == ckCircle) {
            if (Radius > 0) {
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawCircle(Center.X, Center.Y, Radius, GR_DX::Color565ToArgb(Color), 255, 0, &ClipRect);
                } else {
                    GR_Main::ScreenRenderBuffer->DrawAntialiasedCircle16(Center, Radius, Color, ClipRect);
                }
            }
        } else if (Kind == ckFill) {
            R = std::min<std::int32_t>(HitTestBounds.Bottom - HitTestBounds.Top, HitTestBounds.Right - HitTestBounds.Left) / 2;
            if (R > 0) {
                if (GlobalsV::HardwareRenderingEnabled) {
                    GR_DX::DrawCircle((HitTestBounds.Bottom + HitTestBounds.Top) / 2, (HitTestBounds.Right + HitTestBounds.Left) / 2, R, GR_DX::Color565ToArgb(Color), 255, 1, &ClipRect);
                } else {
                    GR_Main::ScreenRenderBuffer->DrawCircle16(ClassesImports::Point((HitTestBounds.Bottom + HitTestBounds.Top) / 2, (HitTestBounds.Right + HitTestBounds.Left) / 2), R, Color, FillColor, ClipRect);
                }
            }
        } else if (Kind == ckShrLight) {
            if (GlobalsV::HardwareRenderingEnabled) {
                if (Radius > 0) {
                    if (Center.Y - Radius > ClipRect.Top) {
                        GR_DX::DrawColoredRect(ClipRect.Left, ClipRect.Top, ClipRect.Right - ClipRect.Left, Center.Y - Radius - ClipRect.Top, 0u, 127, true, &ClipRect);
                    }
                    if (Center.Y + Radius < ClipRect.Bottom) {
                        GR_DX::DrawColoredRect(ClipRect.Left, Center.Y + Radius, ClipRect.Right - ClipRect.Left, ClipRect.Bottom - (Center.Y + Radius), 0u, 127, true, &ClipRect);
                    }
                    if (Center.X - Radius > ClipRect.Left) {
                        X = ClipRect.Left;
                        Y = Center.Y - Radius;
                        if (Y < ClipRect.Top) {
                            Y = ClipRect.Top;
                        }
                        Width = Center.X - Radius - X;
                        Height = Center.Y + Radius - Y;
                        if (Y + Height > ClipRect.Bottom) {
                            Height = ClipRect.Bottom - Y;
                        }
                        GR_DX::DrawColoredRect(X, Y, Width, Height, 0u, 127, true, &ClipRect);
                    }
                    if (Center.X + Radius < ClipRect.Right) {
                        X = Center.X + Radius;
                        Y = Center.Y - Radius;
                        if (Y < ClipRect.Top) {
                            Y = ClipRect.Top;
                        }
                        Width = ClipRect.Right - X;
                        if (X + Width > ClipRect.Right) {
                            Width = ClipRect.Right - X;
                        }
                        Height = Center.Y + Radius - Y;
                        if (Y + Height > ClipRect.Bottom) {
                            Height = ClipRect.Bottom - Y;
                        }
                        GR_DX::DrawColoredRect(X, Y, Width, Height, 0u, 127, true, &ClipRect);
                    }
                    GR_DX::DrawCircle(Center.X, Center.Y, Radius, 0u, 127, 2, &ClipRect);
                }
            } else {
                if (LightBufferDirty == true || LightBuffer->GetPixels() == nullptr) {
                    LightBuffer->AllocateGrayscale(ClientSize.X, ClientSize.Y);
                    LightBuffer->FillPixels(ShrLightOuter);
                    if (Radius > 0) {
                        Bounds.Left = 0;
                        Bounds.Top = 0;
                        Bounds.Right = ClientSize.X;
                        Bounds.Bottom = ClientSize.Y;
                        LightBuffer->DrawCircle8(Center, Radius, ShrLightInner, ShrLightInner, Bounds);
                    }
                    LightBufferDirty = false;
                }
                {
                    std::int32_t pitchBytes = LightBuffer->PitchBytes;
                    std::int32_t cpp_arg_3 = ClipRect.Right - ClipRect.Left;
                    std::int32_t cpp_arg_4 = ClipRect.Bottom - ClipRect.Top;
                    std::int32_t cpp_arg_2 = (ClipRect.Top - HitTestBounds.Top) * LightBuffer->PitchBytes + (ClipRect.Left - HitTestBounds.Left);
                    void* pixels_2 = LightBuffer->GetPixels();
                    void* addPointerOffset = EC_Mem::AddPointerOffset(pixels_2, cpp_arg_2);
                    std::int32_t cpp_arg = GR_Main::ScreenRenderBuffer->PitchBytes * ClipRect.Top + ClipRect.Left * 2;
                    void* pixels = GR_Main::ScreenRenderBuffer->GetPixels();
                    void* addPointerOffset_2 = EC_Mem::AddPointerOffset(pixels, cpp_arg);
                    std::int32_t pitchBytes_2 = GR_Main::ScreenRenderBuffer->PitchBytes;
                    GR_Main::Ex_OKGR_ShrLightMask_16(addPointerOffset_2, pitchBytes_2, addPointerOffset, pitchBytes, cpp_arg_3, cpp_arg_4);
                }
            }
        } else if (Kind == ckMulLight) {
            // The native branch retains only this renderer test.
            static_cast<void>(GlobalsV::HardwareRenderingEnabled);
        }
    }

    void TCircleGI::p_destroy() {
        GI_Circle::TCircleGI_Destroy(this);
    }

    void TCircleGI::virtual_TObjectGI_LoadFromConfigPath(const pas::WideString& Path) {
        GI_Circle::TCircleGI_LoadFromConfigPath(this, Path);
    }

} // namespace GI_Circle
