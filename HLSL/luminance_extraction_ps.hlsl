#include "constants.hlsli"
#include "fullscreen_quad.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture_maps.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    float alpha = color.a;
    color.rgb = smoothstep(l_min.x, l_max.x, dot(color.rgb, lumina.xyz)) * color.rgb; //smoothstep :0と1の間のスムーズなエルミート補間 float3(0.299, 0.587, 0.114
    return float4(color.rgb, alpha);
}
