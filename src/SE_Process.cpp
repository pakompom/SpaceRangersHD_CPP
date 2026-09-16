#include "layout/SE_Process.hpp"
#include "types/EC_BlockPar.hpp"
#include "types/EC_Struct.hpp"
#include "types/GI_MessageLoop.hpp"
#include "types/GI_Panel.hpp"
#include "types/SE_Angel.hpp"
#include "types/SE_Anim.hpp"
#include "types/SE_Asteroid.hpp"
#include "types/SE_BGObj.hpp"
#include "types/SE_Container.hpp"
#include "types/SE_Hole.hpp"
#include "types/SE_Laser.hpp"
#include "types/SE_Missile.hpp"
#include "types/SE_Sputnik.hpp"
#include "types/SE_Star.hpp"
#include "types/SE_StarsField.hpp"
#include "types/Types.hpp"
#include "types/Windows_group.hpp"
#include "units/ClassesImports.hpp"
#include "units/EC_Str.hpp"
#include "units/GI_Main.hpp"
#include "units/GR_Main.hpp"
#include "units/GlobalsV.hpp"
#include "units/SE_Comet.hpp"
#include "units/SE_GAIEffect.hpp"
#include "units/SE_Gate.hpp"
#include "units/SE_Meteorite.hpp"
#include "units/SE_Planet.hpp"
#include "units/SE_Process.hpp"
#include "units/SE_Ruins.hpp"
#include "units/SE_Ship2.hpp"
#include "units/SE_Space.hpp"
#include "units/SE_Weapon.hpp"
#include "units/SysUtils.hpp"
#include "units/System.hpp"
#include "units/aGalaxy.hpp"
#include "units/aMyFunction.hpp"

namespace SE_Process {
    SE_Space::TObjectSE* CreateSpaceObjectByName(const pas::WideString& ClassName, const pas::WideString& GraphKey, WindowsSdk::TPoint UnusedPosition) {
        if (ClassName == u"Star") {
            return pas::construct_call<SE_Star::TStarSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"StarsField") {
            return pas::construct_call<SE_StarsField::TStarsFieldSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Planet") {
            return pas::construct_call<SE_Planet::TPlanetSE>(SE_Planet::TPlanetSE_CreateFromGraph, GraphKey, UnusedPosition);
        } else if (ClassName == u"Sputnik") {
            return pas::construct_call<SE_Sputnik::TSputnikSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Asteroid") {
            return pas::construct_call<SE_Asteroid::TAsteroidSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Hole") {
            return pas::construct_call<SE_Hole::THoleSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Ruins") {
            return pas::construct_call<SE_Ruins::TRuinsSE>(SE_Ruins::TRuinsSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Ship2" || ClassName == u"Ship") {
            return pas::construct_call<SE_Ship2::TShip2SE>(SE_Ship2::TShip2SE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Container") {
            return pas::construct_call<SE_Container::TContainerSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Laser") {
            return pas::construct_call<SE_Laser::TLaserSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Anim") {
            return pas::construct_call<SE_Anim::TAnimSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"BGObj") {
            return pas::construct_call<SE_BGObj::TBGObjSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Weapon") {
            return pas::construct_call<SE_Weapon::TWeaponSE>(SE_Weapon::TWeaponSE_Create, GraphKey, UnusedPosition, 0, -1);
        } else if (ClassName == u"Effect") {
            return pas::construct_call<SE_GAIEffect::TGAIEffectSE>(SE_GAIEffect::TGAIEffectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Gate") {
            return pas::construct_call<SE_Gate::TGateSE>(SE_Gate::TGateSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"GateEffect") {
            return pas::construct_call<SE_Gate::TGateEffectSE>(SE_Gate::TGateEffectSE_Create, GraphKey, UnusedPosition);
        } else if (ClassName == u"Missile") {
            return pas::construct_call<SE_Missile::TMissileSE>(SE_Space::TObjectSE_Create, GraphKey, UnusedPosition);
        } else {
            return nullptr;
        }
    }

    pas::WideString ClassSEtoName(SE_Space::TObjectSE* Obj) {
        if (pas::class_cast_if<SE_Star::TStarSE*>(Obj) != nullptr) {
            return u"Star"_w;
        } else if (pas::class_cast_if<SE_Planet::TPlanetSE*>(Obj) != nullptr) {
            return u"Planet"_w;
        } else if (pas::class_cast_if<SE_Sputnik::TSputnikSE*>(Obj) != nullptr) {
            return u"Sputnik"_w;
        } else if (pas::class_cast_if<SE_Asteroid::TAsteroidSE*>(Obj) != nullptr) {
            return u"Asteroid"_w;
        } else if (pas::class_cast_if<SE_Hole::THoleSE*>(Obj) != nullptr) {
            return u"Hole"_w;
        } else if (pas::class_cast_if<SE_Ruins::TRuinsSE*>(Obj) != nullptr) {
            return u"Ruins"_w;
        } else if (pas::class_cast_if<SE_Ship2::TShip2SE*>(Obj) != nullptr) {
            return u"Ship2"_w;
        } else if (pas::class_cast_if<SE_Container::TContainerSE*>(Obj) != nullptr) {
            return u"Container"_w;
        } else if (pas::class_cast_if<SE_Laser::TLaserSE*>(Obj) != nullptr) {
            return u"Laser"_w;
        } else if (pas::class_cast_if<SE_Anim::TAnimSE*>(Obj) != nullptr) {
            return u"Anim"_w;
        } else if (pas::class_cast_if<SE_BGObj::TBGObjSE*>(Obj) != nullptr) {
            return u"BGObj"_w;
        } else if (pas::class_cast_if<SE_Weapon::TWeaponSE*>(Obj) != nullptr) {
            return u"Weapon"_w;
        } else if (pas::class_cast_if<SE_GAIEffect::TGAIEffectSE*>(Obj) != nullptr) {
            return u"Effect"_w;
        } else if (pas::class_cast_if<SE_Gate::TGateSE*>(Obj) != nullptr) {
            return u"Gate"_w;
        } else if (pas::class_cast_if<SE_Gate::TGateEffectSE*>(Obj) != nullptr) {
            return u"GateEffect"_w;
        } else if (pas::class_cast_if<SE_Missile::TMissileSE*>(Obj) != nullptr) {
            return u"Missile"_w;
        } else {
            pas::raise(pas::make_exception<pas::Exception>("Error in ClassSEtoName"_a));
        }
    }

    void TProcessSE_Create(TProcessSE* Self, const pas::WideString& ConfigName) {
        pas::object_create(Self);
        Self->LoadFromBlock(GR_Main::GameDataConfig->GetBlockByPath(pas::concat_wide({u"SE.", ConfigName})));
        Self->RetainedObjects = pas::make_object<pas::List>();
    }

    void TProcessSE_Destroy(TProcessSE* Self) {
        SE_Space::TObjectSE* Obj{};
        std::int32_t Index{};
        Self->CloseSpace();
        while (Self->FirstObject != nullptr) {
            Obj = Self->LastObject;
            Self->RemoveObject(Obj);
        }
        if (Self->RetainedObjects != nullptr) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(Self->RetainedObjects) - 1); cpp_range.next(Index); ) {
                Obj = pas::list_at<SE_Space::TObjectSE>(Self->RetainedObjects, Index);
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj));
            }
            pas::free(Self->RetainedObjects);
            Self->RetainedObjects = nullptr;
        }
        pas::object_destroy(Self);
    }

    void TProcessSE::AddObject(SE_Space::TObjectSE* Obj) {
        if (LastObject != nullptr) {
            LastObject->ProcessNext = Obj;
        }
        Obj->ProcessPrev = LastObject;
        Obj->ProcessNext = nullptr;
        SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&LastObject), Obj);
        if (FirstObject == nullptr) {
            FirstObject = Obj;
        }
    }

    void TProcessSE::RemoveObject(SE_Space::TObjectSE* Obj) {
        if (Obj->ProcessPrev != nullptr) {
            Obj->ProcessPrev->ProcessNext = Obj->ProcessNext;
        }
        if (Obj->ProcessNext != nullptr) {
            Obj->ProcessNext->ProcessPrev = Obj->ProcessPrev;
        }
        if (LastObject == Obj) {
            LastObject = Obj->ProcessPrev;
        }
        if (FirstObject == Obj) {
            FirstObject = Obj->ProcessNext;
        }
        SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj));
    }

    void TProcessSE::OpenSpace(GI_Panel::TPanelGI* MapPanel, GI_MessageLoop::TMessageLoopGI* Screen) {
        SE_Space::TObjectSE* Obj{};
        if (!IsSpaceOpen()) {
            Space = pas::construct_call<SE_Space::TSpaceSE>(SE_Space::TSpaceSE_Create, MapPanel, Screen);
            Space->Process = this;
            Obj = FirstObject;
            while (Obj != nullptr) {
                Obj->AttachToSpace(Space);
                Obj = Obj->ProcessNext;
            }
            StartBackgroundEffects();
        }
    }

    void TProcessSE::BindMinimap(GI_MessageLoop::TObjectGI* Control) {
        if (IsSpaceOpen()) {
            Space->MinimapControl = Control;
            Space->MinimapControl->LeftButtonDownCallback = pas::bind_method<&SE_Space::TSpaceSE::MinimapMouseDown>(Space);
            Space->MinimapControl->RightButtonDownCallback = pas::bind_method<&SE_Space::TSpaceSE::MinimapMouseDown>(Space);
            Space->MinimapControl->MouseEnterCallback = pas::bind_method<&SE_Space::TSpaceSE::MinimapMouseEnter>(Space);
            Space->MinimapControl->MouseMoveCallback = pas::bind_method<&SE_Space::TSpaceSE::MinimapMouseMove>(Space);
            Space->CreateMinimapViewport();
        }
    }

    void TProcessSE::CloseSpace() {
        if (IsSpaceOpen()) {
            StopBackgroundEffects();
            Space->FreeMinimapViewport();
            pas::free(Space);
            Space = nullptr;
        }
    }

    std::uint8_t TProcessSE::IsSpaceOpen() {
        return !(Space == nullptr);
    }

    void TProcessSE::PopulateAmbientObjects(std::int32_t Radius, std::int32_t BackgroundImage, std::uint32_t Seed) {
        SE_Comet::TCometSE* CometObj{};
        SE_Angel::TAngelSE* AngelObj{};
        SE_Meteorite::TMeteoriteSE* MeteoriteObj{};
        std::int32_t Index{};
        std::int32_t BlockIndex{};
        std::int32_t VariantCount{};
        std::int32_t VariantIndex{};
        std::int32_t Diameter{};
        WindowsSdk::TPoint CountRange{};
        SE_Space::TObjectSE* Obj{};
        pas::WideString Text{};
        std::int32_t Count = 0;
        SE_Space::TObjectSE* NextObj = FirstObject;
        while (NextObj != nullptr) {
            Obj = NextObj;
            NextObj = NextObj->ProcessNext;
            if (pas::class_cast_if<SE_Comet::TCometSE*>(Obj) != nullptr || pas::class_cast_if<SE_Angel::TAngelSE*>(Obj) != nullptr || pas::class_cast_if<SE_Meteorite::TMeteoriteSE*>(Obj) != nullptr) {
                Obj->DetachFromSpace();
                RemoveObject(Obj);
            }
        }
        std::int32_t MinRadius = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, 0)->MapDiameter;
        std::int32_t MaxRadius = MinRadius;
        for (auto cpp_range = pas::for_to<std::int32_t>(1, pas::list_count(aGalaxy::Galaxy->Stars) - 1); cpp_range.next(Index); ) {
            Diameter = pas::list_at<aGalaxy::TStar>(aGalaxy::Galaxy->Stars, Index)->MapDiameter;
            MinRadius = std::min<std::int32_t>(MinRadius, Diameter);
            MaxRadius = std::max<std::int32_t>(MaxRadius, Diameter);
        }
        MinRadius = MinRadius / 2;
        MaxRadius = MaxRadius / 2;
        EC_BlockPar::TBlockParEC* Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Anim.BGO_HS.Objects"_wref.get());
        if (GlobalsV::Comet > 0) {
            if (BackgroundImage < 10) {
                Text = ([&] {
                    const pas::WideString& cpp_arg = static_cast<pas::WideString>(pas::concat_ansi({"0", SysUtils::IntToStr(BackgroundImage)}));
                    EC_BlockPar::TBlockParEC* blockByPath = GR_Main::GameDataConfig->GetBlockByPath(u"StyleComet"_wref.get());
                    return blockByPath->GetParam(cpp_arg);
                }());
            } else {
                Text = ([&] {
                    const pas::WideString& intToStr = pas::wide_int_to_str(BackgroundImage);
                    EC_BlockPar::TBlockParEC* blockByPath_2 = GR_Main::GameDataConfig->GetBlockByPath(u"StyleComet"_wref.get());
                    return blockByPath_2->GetParam(intToStr);
                }());
            }
            Index = aMyFunction::NextRandomIntRange(0, EC_Str::CountDelimitedPartsW(Text, u","_wref.get()) / 2 - 1, Seed) * 2;
            VariantCount = EC_Str::ExtractDigitsToIntW(EC_Str::ExtractDelimitedPartW(Text, Index + 1, u","_wref.get()));
            Text = EC_Str::ExtractDelimitedPartW(Text, Index, u","_wref.get());
            CountRange.X = 15;
            CountRange.Y = 30;
            if (Block->CountParams(u"CometCount"_wref.get()) > 0) {
                CountRange = GI_Main::GetPointGI(Block->GetParam(u"CometCount"_wref.get()));
            }
            if (GlobalsV::Comet == 1) {
                Count = CountRange.X;
            } else {
                Count = CountRange.Y;
            }
            Count = System::Round(static_cast<long double>(aMyFunction::RemapClamped(Radius, MinRadius, MaxRadius, 0.5, 2.0)) * Count);
            for (auto cpp_range_2 = pas::for_to<std::int32_t>(1, Count); cpp_range_2.next(Index); ) {
                CometObj = pas::construct_call<SE_Comet::TCometSE>(SE_Comet::TCometSE_Create, u"Anim.BGO_HS.Comet"_w, ClassesImports::Point(0, 0));
                VariantIndex = aMyFunction::NextRandomIntRange(0, VariantCount - 1, Seed);
                if (VariantIndex < 10) {
                    CometObj->ImagePath = pas::concat_wide({u"Bm.Comet.", Text, u"0", pas::wide_int_to_str(VariantIndex)});
                } else {
                    CometObj->ImagePath = pas::concat_wide({u"Bm.Comet.", Text, pas::wide_int_to_str(VariantIndex)});
                }
                AddObject(CometObj);
            }
        }
        if (Block->CountParams(u"AngelCount"_wref.get()) > 0) {
            CountRange = GI_Main::GetPointGI(Block->GetParam(u"AngelCount"_wref.get()));
            Count = aMyFunction::RandomIntRange(CountRange.X, CountRange.Y);
        }
        for (auto cpp_range_3 = pas::for_to<std::int32_t>(1, Count); cpp_range_3.next(Index); ) {
            AngelObj = pas::construct_call<SE_Angel::TAngelSE>(SE_Space::TObjectSE_Create, u"Anim.BGO_HS.Angel"_wref.get(), ClassesImports::Point(0, 0));
            AddObject(AngelObj);
        }
        CountRange = GI_Main::GetPointGI(GR_Main::GameDataConfig->GetParamByPathOrMarker(u"SE.Anim.BGO_HS.Objects.MeteoriteCount"_wref.get()));
        Count = aMyFunction::RandomIntRange(CountRange.X, CountRange.Y);
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.Meteorite"_wref.get());
        std::int32_t BlockCount = Block->GetBlockCount();
        for (auto cpp_range_4 = pas::for_to<std::int32_t>(0, Count - 1); cpp_range_4.next(Index); ) {
            BlockIndex = aMyFunction::RandomIntRange(0, BlockCount - 1);
            MeteoriteObj = pas::construct_call<SE_Meteorite::TMeteoriteSE>(SE_Meteorite::TMeteoriteSE_Create, pas::concat_wide({u"Meteorite.", Block->GetBlockNameByIndex(BlockIndex)}), ClassesImports::Point(0, 0));
            AddObject(MeteoriteObj);
        }
    }

    void TProcessSE::StartBackgroundEffects() {
        StopBackgroundEffects();
        if (IsSpaceOpen()) {
            ViewRect = ClassesImports::Rect(-100001, -100001, -100000, -100000);
            UpdateViewRect();
            BackgroundTimer = Space->CreateTimer(GlobalsV::BGOTime, GlobalsV::BGOTime, pas::bind_method<&TProcessSE::AdvanceBackgroundEffects>(this), 0);
        }
    }

    void TProcessSE::StopBackgroundEffects() {
        SE_Space::TObjectSE* Obj{};
        std::int32_t Index{};
        if (IsSpaceOpen()) {
            for (auto cpp_range = pas::for_to<std::int32_t>(0, pas::list_count(RetainedObjects) - 1); cpp_range.next(Index); ) {
                Obj = pas::list_at<SE_Space::TObjectSE>(RetainedObjects, Index);
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj));
            }
            pas::list_clear(RetainedObjects);
            if (BackgroundTimer != nullptr) {
                Space->DeleteTimer(BackgroundTimer);
                BackgroundTimer = nullptr;
            }
        }
    }

    void TProcessSE::AdvanceBackgroundEffects(SE_Space::PSpaceTimerSE Timer, std::int32_t UserData) {
        SE_Space::TObjectSE* Obj{};
        EC_Struct::TPointF Position{};
        std::int32_t Index = 0;
        while (Index < pas::list_count(RetainedObjects)) {
            Obj = pas::list_at<SE_Space::TObjectSE>(RetainedObjects, Index);
            if (!Obj->IsAttachedToSpace()) {
                pas::list_delete(RetainedObjects, Index);
                SE_Space::ReleaseSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj));
            } else {
                ++Index;
            }
        }
        if (GlobalsV::BGOCount > pas::list_count(RetainedObjects)) {
            std::int32_t cpp_left = aMyFunction::RandomIntRange(0, GlobalsV::BGOCount - 1);
            if (cpp_left >= pas::list_count(RetainedObjects) - 1) {
                SE_Space::RetainSpaceObject(pas::Var<SE_Space::TObjectSE*>(&Obj), pas::construct_call<SE_Anim::TAnimSE>(SE_Space::TObjectSE_Create, TProcessSE::SelectBackgroundAnimation(), ClassesImports::Point(0, 0)));
                Position.X = aMyFunction::RandomIntRange(ViewRect.Left, ViewRect.Right);
                Position.Y = aMyFunction::RandomIntRange(ViewRect.Top, ViewRect.Bottom);
                Obj->SetPosition(Position);
                Obj->AttachToSpace(Space);
                pas::list_add(RetainedObjects, reinterpret_cast<void*>(Obj));
            }
        }
    }

    void TProcessSE::UpdateViewRect() {
        PreviousViewRect = ViewRect;
        ViewRect = Space->MapPanel->GetVisibleContentRect();
    }

    pas::WideString TProcessSE::SelectBackgroundAnimation() {
        EC_BlockPar::TBlockParEC* Block{};
        std::int32_t Index{};
        Block = GR_Main::GameDataConfig->GetBlockByPath(u"SE.BGO"_wref.get());
        std::int32_t Count = Block->GetParamCount();
        std::int32_t Weight = 0;
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Weight += EC_Str::ExtractDigitsToIntW(Block->GetParamName(Index));
        }
        Weight = aMyFunction::RandomIntRange(0, Weight - 1);
        Index = 0;
        Weight -= EC_Str::ExtractDigitsToIntW(Block->GetParamName(Index));
        while (Weight >= 0) {
            ++Index;
            Weight -= EC_Str::ExtractDigitsToIntW(Block->GetParamName(Index));
        }
        return Block->GetParamValue(Index);
    }

    void TProcessSE::LoadFromBlock(EC_BlockPar::TBlockParEC* Block) {
        SE_Space::TObjectSE* Obj{};
        EC_BlockPar::TBlockParEC* Objects{};
        std::int32_t Index{};
        Objects = Block->GetBlockByPath(u"Objects"_wref.get());
        std::int32_t Count = Objects->GetBlockCount();
        for (auto cpp_range = pas::for_to<std::int32_t>(0, Count - 1); cpp_range.next(Index); ) {
            Obj = ([&] {
                WindowsSdk::TPoint pointGI = GI_Main::GetPointGI(Objects->GetBlockByIndex(Index)->GetParam(u"Size"_wref.get()));
                const pas::WideString& param = Objects->GetBlockByIndex(Index)->GetParam(u"Type"_wref.get());
                const pas::WideString& blockNameByIndex = Objects->GetBlockNameByIndex(Index);
                return SE_Process::CreateSpaceObjectByName(blockNameByIndex, param, pointGI);
            }());
            if (Obj != nullptr) {
                AddObject(Obj);
                Obj->ApplyConfig(Objects->GetBlockByIndex(Index));
            }
        }
    }

    void TProcessSE::p_destroy() {
        SE_Process::TProcessSE_Destroy(this);
    }

} // namespace SE_Process
