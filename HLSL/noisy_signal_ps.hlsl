#include "fullscreen_quad.hlsli"
#include "constants.hlsli"
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
Texture2D texture_map : register(t0);
SamplerState sampler_states[3] : register(s0);
float rand(float2 coord)
{
    //frac: xの小数部を返す。
    return frac(sin(dot(coord.xy, float2(12.9898, 78.233))) * 437.645);
}


float4 main(VS_OUT pin) : SV_TARGET
{
    float3 color = texture_map.Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    //random_num
    float2 tex = pin.texcoord;
    tex *= sin(time);
    float r = rand(tex);

    float3 noise = { r, r, r }; //ノイズの色
    float noise_intensity = 0.5f; //1に近づけるけるほどノイズの色が濃くなる（裏が見えづらくなる）

    color = lerp(color,noise, noise_intensity);
    return float4(color, 1.0f).rgba;


}