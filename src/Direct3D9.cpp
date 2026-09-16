#include "layout/Direct3D9.hpp"
#include "units/Direct3D9.hpp"
#include "units/System.hpp"

namespace Direct3D9 {
    Direct3D9::TDirect3DCreate9 Direct3DCreate9{};

    void PAS_STDCALL CreateDirect3D9(std::uint32_t SDKVersion, IDirect3D9& Result) {
        Result = pas::ComView<IDirect3D9_Tag>(Direct3D9::Direct3DCreate9(SDKVersion));
        if (Result != nullptr) {
            System::IInterface_u_Release(Result);
        }
        return;
    }

} // namespace Direct3D9
