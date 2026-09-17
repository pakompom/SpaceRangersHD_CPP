#include "layout/PopUp.hpp"
#include "types/GI_Main.hpp"
#include "types/GR_GraphBuf.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/GI_Image.hpp"
#include "units/GI_Label.hpp"
#include "units/GI_MessageLoop.hpp"
#include "units/GI_Panel.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/PopUp.hpp"
#include "units/System.hpp"

// Placement follows the reviewed PopUp association in reference/unit_ownership.json.
// Native TfPopUpController VMT:.
namespace PopUp {
    // Created.
    PopUp::TfPopUpController* PopupController = nullptr;

    void TfPopUpController_Create(TfPopUpController* Self) {
        GI_MessageLoop::TObjectGI_Create(Self, nullptr);
        Self->QueueLock = pas::make_critical_section<pas::CriticalSection>();
        Self->TextQueue = pas::make_object<pas::List>();
        Self->ImageQueue = pas::make_object<pas::List>();
    }

    // Frees queued cells without finalizing their strings; does not call inherited Destroy.
    void TfPopUpController_Destroy(TfPopUpController* Self) {
        pas::critical_enter(Self->QueueLock);
        while (pas::list_count(Self->TextQueue) > 0) {
            pas::dispose(pas::list_get(Self->TextQueue, 0));
            pas::list_delete(Self->TextQueue, 0);
        }
        while (pas::list_count(Self->ImageQueue) > 0) {
            pas::dispose(pas::list_get(Self->ImageQueue, 0));
            pas::list_delete(Self->ImageQueue, 0);
        }
        pas::free(Self->TextQueue);
        pas::free(Self->ImageQueue);
        Self->FreeOwnedChildren();
        pas::critical_leave(Self->QueueLock);
        pas::free(Self->QueueLock);
    }

    GI_MessageLoop::TObjectGI* TfPopUpController_CreatePopup(TfPopUpController* Self, pas::WideString Text, pas::WideString ImagePath) {
        GI_Panel::TPanelGI* Panel = pas::construct_call<GI_Panel::TPanelGI>(GI_Panel::TPanelGI_Create, Self);
        GI_Image::TImageGI* Background = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
        Background->SetPosition(ClassesImports::Point(0, 0));
        Background->SetImagePath(u"GI,Bm.FormAchievements.MessageBG"_w);
        Background->SetSize(Background->GetContentSize());
        Background->SetActive(true);
        Panel->SetSize(Background->ClientSize);
        GI_Image::TImageGI* Icon = pas::construct_call<GI_Image::TImageGI>(GI_Image::TImageGI_Create, Panel);
        Icon->SetImagePath(ImagePath);
        Icon->SetSize(Icon->GetContentSize());
        Icon->SetPosition(ClassesImports::Point(27, 41));
        Icon->SetActive(true);
        GI_Label::TLabelGI* LabelControl = pas::construct_call<GI_Label::TLabelGI>(GI_Label::TLabelGI_Create, Panel);
        LabelControl->SetPosition(ClassesImports::Point(Icon->LocalPosition.X + Icon->ClientSize.X + 15, 54));
        LabelControl->SetSize(ClassesImports::Point(Panel->ClientSize.X - Icon->ClientSize.X - Icon->LocalPosition.X - 30, 72));
        LabelControl->SetFontName(GlobalsV::NormalBoldFontName);
        LabelControl->SetTextAlignX(GI_Main::taxCenter);
        LabelControl->SetTextAlignY(GI_Main::tayCenterEx);
        LabelControl->SetWordWrapEnabled(true);
        LabelControl->SetText(Text);
        LabelControl->SetTextColor(GR_Main::CurrentPixelFormat->PackRgbBytes(0, 0, 0));
        LabelControl->SetActive(true);
        return Panel;
    }

    // Drains queued notifications, advances their vertical animation, and retires off-screen controls.
    void TfPopUpController_AdvancePopups(TfPopUpController* Self, std::uint32_t Tick) {
        GI_MessageLoop::TObjectGI* Popup{};
        GI_MessageLoop::TObjectGI* Previous{};
        std::int32_t Movement{};
        std::int32_t BottomOffset{};
        pas::WideString Text{};
        pas::WideString ImagePath{};
        if (pas::list_count(Self->TextQueue) > 0) {
            pas::critical_enter(Self->QueueLock);
            Self->SetActive(true);
            Self->SetPosition(ClassesImports::Point(0, 0));
            Self->SetDepth(-1.0E+3);
            Self->SetSize(ClassesImports::Point(GR_Main::GameScreenWidth, GR_Main::GameScreenHeight));
            Self->SetPositionModeW(true);
            Self->MessageLoop = pas::checked_cast<GI_MessageLoop::TMessageLoopGI*>(GlobalsV::RegisteredScreens[GlobalsV::CurrentScreenId]);
            while (pas::list_count(Self->TextQueue) > 0) {
                Text = *pas::list_at<pas::WideString>(Self->TextQueue, 0);
                pas::dispose(pas::list_get(Self->TextQueue, 0));
                pas::list_delete(Self->TextQueue, 0);
                ImagePath = *pas::list_at<pas::WideString>(Self->ImageQueue, 0);
                pas::dispose(pas::list_get(Self->ImageQueue, 0));
                pas::list_delete(Self->ImageQueue, 0);
                Previous = Self->LastChild;
                Popup = PopUp::TfPopUpController_CreatePopup(Self, Text, ImagePath);
                if (Previous == nullptr) {
                    Popup->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - Popup->ClientSize.X - 10, GR_Main::GameScreenHeight + 10));
                    Self->MotionRemainder = 0.0f;
                } else {
                    Popup->SetPosition(ClassesImports::Point(GR_Main::GameScreenWidth - Popup->ClientSize.X - 10, Previous->LocalPosition.Y + Previous->ClientSize.Y + 10));
                }
                Self->MovingUp = true;
                Self->PauseRemaining = 0u;
            }
            pas::critical_leave(Self->QueueLock);
        }
        if (Self->FirstChild == nullptr) {
            Self->LastTick = Tick;
        } else {
            if (Self->PauseRemaining > 0) {
                Self->PauseRemaining = std::max<std::int64_t>(static_cast<std::int64_t>(Tick - Self->LastTick), static_cast<std::int64_t>(Self->PauseRemaining)) - (Tick - Self->LastTick);
                if (Self->PauseRemaining == 0) {
                    Self->MovingUp = false;
                }
            } else {
                Self->MotionRemainder = Self->MotionRemainder + std::max<std::int64_t>(static_cast<std::int64_t>(0), static_cast<std::int64_t>(Tick - Self->LastTick)) * 0.07L;
                if (Self->MotionRemainder >= 1.0L) {
                    Movement = System::Trunc(Self->MotionRemainder);
                    Self->MotionRemainder = static_cast<long double>(Self->MotionRemainder) - Movement;
                    if (Self->MovingUp) {
                        BottomOffset = Self->LastChild->LocalPosition.Y + Self->LastChild->ClientSize.Y - GR_Main::GameScreenHeight;
                        if (BottomOffset - Movement <= -10) {
                            Movement = BottomOffset + 10;
                            Self->PauseRemaining = 2000u;
                        }
                        Popup = Self->FirstChild;
                        while (Popup != nullptr) {
                            Popup->SetPosition(ClassesImports::Point(Popup->LocalPosition.X, Popup->LocalPosition.Y - Movement));
                            Popup = Popup->NextSibling;
                        }
                    } else {
                        Popup = Self->FirstChild;
                        while (Popup != nullptr) {
                            Popup->SetPosition(ClassesImports::Point(Popup->LocalPosition.X, Popup->LocalPosition.Y + Movement));
                            Popup = Popup->NextSibling;
                        }
                    }
                }
            }
            Self->LastTick = Tick;
            if (!Self->MovingUp) {
                while (Self->LastChild != nullptr && Self->LastChild->LocalPosition.Y > GR_Main::GameScreenHeight) {
                    Self->FreeOwnedChild(Self->LastChild);
                }
            }
            if (Self->FirstChild == nullptr) {
                Self->SetActive(false);
            }
        }
    }

    // Enqueues parallel managed-string cells under QueueLock.
    void TfPopUpController::QueueNotification(pas::WideString Text, pas::WideString ImagePath) {
        System::PWideString Cell{};
        pas::critical_enter(QueueLock);
        pas::new_value(Cell);
        *Cell = std::move(Text);
        pas::list_add(TextQueue, static_cast<void*>(Cell));
        pas::new_value(Cell);
        *Cell = std::move(ImagePath);
        pas::list_add(ImageQueue, static_cast<void*>(Cell));
        pas::critical_leave(QueueLock);
    }

    void TfPopUpController::p_destroy() {
        PopUp::TfPopUpController_Destroy(this);
    }

} // namespace PopUp
