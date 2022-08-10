#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState pointSamplerState : register(s0); //https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-sample
SamplerState linerSamplerState : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);
#if 0
float4 main(VS_OUT pin) : SV_TARGET    //戻り値のセマンティック   SVはシステムバリュー      //https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-to-sample
{
    return diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
}
#endif

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
    float alpha = color.a;
#if 1
    // Inverse gamma process
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);
#endif
    return float4(color.rgb, alpha) * pin.color;
}
