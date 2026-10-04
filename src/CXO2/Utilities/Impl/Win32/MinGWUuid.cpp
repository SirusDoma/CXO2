// MinGW's d3d11shader.h defines IID_ID3D11ShaderReflection but no __uuidof
// specialization, so SFML's Direct3D 11 backend fails to link without this.
#if defined(__MINGW32__)
#include <d3d11shader.h>

template<> const GUID& __mingw_uuidof<ID3D11ShaderReflection>()
{
    static constexpr GUID id = {0x8d536ca1, 0x0cca, 0x4956, {0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84}};
    return id;
}
#endif
